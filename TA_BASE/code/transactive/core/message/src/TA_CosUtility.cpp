    /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TA_CosUtility.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/02 14:28:39 $
  * Last modified by:  $Author: limin.zhu $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/utilities/src/Hostname.h"
#include <boost/lexical_cast.hpp>
#ifdef WIN32 // for pid
    #include <process.h>
#else
    #include <unistd.h>
#endif


namespace TA_Base_Core
{
    const char* DOMAIN_NAME_FIELDNAME = "domain_name";
    const char* TYPE_NAME_FIELDNAME = "type_name";
    const char* EVENT_NAME_FIELDNAME = "event_name";
    const char* SENDER_ENTITY_FIELDNAME = "sender_entity"; // yanrong++: CL-21199, 2010-01-19

    const char* TYPE_NAME_SEPARATOR = "_";
    const char* WILDCARD_STR = "*";

    std::string gStringFromEventType(const CosNotification::EventType& type)
    {
        return gStringFromPair(type.domain_name.in(), type.type_name.in());
    }

    std::string gStringFromEventTypes(const CosNotification::EventTypeSeq& seq)
    {
        std::string str("{");
        for ( unsigned int i = 0; i < seq.length() ; i++ )
        {
            str += gStringFromEventType(seq[i]);
            str += ", ";
        }

        str += "}";

        return str;
    }

    std::string gStringFromCorbaAny(const CORBA::Any& val)
    {
        std::string strAny;
        const char* tmp;

        // CORBA::Any::type() creates a TypeCode_ptr that must be released.
        CORBA::TypeCode_ptr valType = val.type();
        if (valType->kind() == CORBA::tk_null)
        {
            strAny = "<nil>";
        }
        else if (valType->kind() == CORBA::tk_ulong)
        {
            CORBA::ULong ul;
            val >>= ul;
            strAny = gStringFromUnsignedLong(ul);
        }
        else if (val >>= tmp)
        {
            strAny = tmp;
        }
        else
        {
            strAny = "<Unknown type in Any>";
        }

        CORBA::release(valType);
        return strAny;
    }

    // Returns a string of the form (propName1, propValue1), (propName2, propValue2), ...
    std::string stringFromPropertySeq(const CosNotification::PropertySeq& seq)
    {
        std::string str("{");
        for (unsigned int i = 0; i < seq.length(); ++i)
        {
            str += gStringFromPair(seq[i].name.in(), gStringFromCorbaAny(seq[i].value));
            str += ", ";
        }

        str += "}";

        return str;
    }

    std::string gStringFromEventHeader(const CosNotification::StructuredEvent& event, bool fGetBasicInfo)
    {
        std::string str;

        // First the fixed event header, then the variable header, then the filterable data.
        // The three sections are delimited by braces - {fixed_header}, {variable_header}, {filterable_data}
        str += "{";
        str += gStringFromPair(DOMAIN_NAME_FIELDNAME, event.header.fixed_header.event_type.domain_name.in());
        str += ", ";
        str += gStringFromPair(TYPE_NAME_FIELDNAME, event.header.fixed_header.event_type.type_name.in());
        if ( fGetBasicInfo )
        {
            str +="}";
        }
        else
        {
            //xinsong++: eventName is alwarws empty
            /*
            str += ", ";
            str += gStringFromPair(EVENT_NAME_FIELDNAME, event.header.fixed_header.event_name.in());
            */
            str += "}, ";
            str += stringFromPropertySeq(event.header.variable_header);
            str += ", ";
            str += stringFromPropertySeq(event.filterable_data);
        }

        return str;
    }


    unsigned int findFirstInSequence(const CosNotification::PropertySeq& seq, const std::string& name)
    {
        int index = seq.length();
        for (unsigned int i = 0; i < seq.length(); ++i)
        {
            if ( 0 == strcmp(seq[i].name, name.c_str()) )
            {
                index = i;
                break;
            }
        }

        return index;
    }


    std::string gObjectRefToStr(CORBA::Object_ptr obj)
    {
        CORBA::String_var str = CorbaUtil::getInstance().getOrb().object_to_string( obj );
        return str.in();
    }


    bool getValueFromEventHeader(const CosNotification::StructuredEvent& event, const std::string& name, std::string& value)
    {
        if (name == TA_Base_Core::DOMAIN_NAME_FIELDNAME)
        {
            value = event.header.fixed_header.event_type.domain_name.in();
            return true;
        }
        else if (name == TA_Base_Core::TYPE_NAME_FIELDNAME)
        {
            value = event.header.fixed_header.event_type.type_name.in();
            return true;
        }
        else if (name == TA_Base_Core::EVENT_NAME_FIELDNAME)
        {
            value = event.header.fixed_header.event_name.in();
            return true;
        }
        else
        {
            unsigned int index;
            index = findFirstInSequence(event.header.variable_header, name);
            if (index != event.header.variable_header.length())
            {
                value = TA_Base_Core::gStringFromCorbaAny(event.header.variable_header[index].value);
                return true;
            }

            index = findFirstInSequence(event.filterable_data, name);
            if (index != event.filterable_data.length())
            {
                value = TA_Base_Core::gStringFromCorbaAny(event.filterable_data[index].value);
                return true;
            }
        }

        // TODO LPP: Perhaps an empty string should be returned if the value is missing.
        value = "<missing>";
        return false;
    }


    std::string    gGetSegmentedEntityKey( unsigned long entityKey )
    {
        static int segmentValue = 0;
        if ( 0 == segmentValue )
        {
            if ( RunParams::getInstance().isSet( "EntitySegmentSize" ) )
            {
                segmentValue = atoi( RunParams::getInstance().get( "EntitySegmentSize" ).c_str() );
            }
            else
            {
                // The default window
                segmentValue = 10;
            }

            TA_ASSERT(segmentValue > 0, "segmentValue > 0");
        }

        std::ostringstream str;
        str << ( entityKey / segmentValue );
        return str.str();
    }


    bool gIsStarStar( const CosNotification::EventType& eventType )
    {
        if ( ( 0 == strcmp( "%ALL", eventType.type_name.in() ) ) ||
             ( ( 0 == strcmp( WILDCARD_STR, eventType.domain_name.in() ) && ( 0 == strcmp( WILDCARD_STR, eventType.type_name.in() ) ) ) ) )
        {
            return true;
        }

        return false;
    }


    unsigned long gGetLocalLocationKey()
    {
        static unsigned long s_localLocationKey = LONG_MAX;

        if ( s_localLocationKey == LONG_MAX )
        {
            // LocationKey not retrieved yet. make sure it is set
            TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "LocationKey is not set" );
            s_localLocationKey = strtoul( RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str() , NULL, 10 );

            LOG1( SourceInfo, DebugUtil::DebugInfo, 
                "gGetLocalLocationKey(): local location key=%d", s_localLocationKey);
        }

        return s_localLocationKey;
    }

    void gPopulateStructuredEventHeader( CosNotification::StructuredEvent& event,
                                         const std::string& domainName,
                                         const std::string& typeName,
                                         const FilterData* variableHeaderData,
                                         const FilterData* filterableData )
    {
        TA_ASSERT( typeName.length() > 0, "Undefined Type Name for Structured Event" );

        // Fixed Header
        event.header.fixed_header.event_type.domain_name = domainName.c_str();
        event.header.fixed_header.event_type.type_name = typeName.c_str();
    
        // We replace all existing variable_header and filterable_data fields
        event.header.variable_header.length(0);
        event.filterable_data.length(0);

        // Variable Header
        if ( variableHeaderData != NULL)
        {
            gAppendFilterDataToPropertySeq(event.header.variable_header, *variableHeaderData);
        }

        // yanrong++: CL-21199, 2010-01-19
        if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENTITYNAME))
        {
            int index = event.header.variable_header.length();
            event.header.variable_header.length(index + 1);            
            static std::string v = Hostname::getHostname() + ":" + boost::lexical_cast<std::string>(::getpid()) + ":" + RunParams::getInstance().get(RPARAM_ENTITYNAME);
            event.header.variable_header[index].name = SENDER_ENTITY_FIELDNAME;
            event.header.variable_header[index].value <<= v.c_str();
        }
        // ++yanrong

        // Filterable Body Fields
        if ( filterableData != NULL )
        {
            gAppendFilterDataToPropertySeq(event.filterable_data, *filterableData);
        }
    }

    void gAppendFilterDataToPropertySeq(CosNotification::PropertySeq& seq, const FilterData& data)
    {
        int pos = seq.length();
        seq.length( pos + data.size() );
        
        for( FilterData::const_iterator iter = data.begin(); iter != data.end(); ++iter, ++pos)
        {
            NameValuePair* nvp = *iter;
            if (NULL != nvp)
            {
                seq[pos].name = nvp->name.c_str();
                seq[pos].value <<= nvp->value.c_str();
            }else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NameValuePair* nvp == NULL");
            }
        }
    }

    void gAppendToEventTypeSeq( CosNotification::EventTypeSeq& seq, 
                                const std::string& domainName, 
                                const std::string& typeName)
    {
        int pos = seq.length();
        seq.length(pos + 1);

        seq[pos].domain_name = domainName.c_str();
        seq[pos].type_name = typeName.c_str();
    }

    void gAppendToEventTypeSeq( CosNotification::EventTypeSeq& seq, 
                                const std::string& domainName, 
                                const std::vector<std::string>& typeNames )
    {
        int pos = seq.length();
        seq.length(pos + typeNames.size());

        for ( std::vector<std::string>::const_iterator iter = typeNames.begin(); iter != typeNames.end(); ++iter, ++pos )
        {
            seq[pos].domain_name = domainName.c_str();
            seq[pos].type_name = iter->c_str();
        }
    }


    std::string gGenerateConstraintExpression(const FilterData* filterableData)
    {            
        // If there is nothing to filter on, then just return an empty constraint
        if ( NULL == filterableData || 0 == filterableData->size() )
        {
            return "true";
        }

        std::string constraint("( ");

        // Generate the constraint expression as defined by the OMG corba specification
        // Basically loop through the filterableData & work it all out
        for (FilterData::const_iterator it = filterableData->begin(); it != filterableData->end(); )
        {
            // Get the first Name Value Pair
            // '$' is the current event
            // '.' is used to specify the filter header name

            // This constraint appears to work with omniNotify
            // There are two types of evalution, equals, or not equals
            constraint += "$" + (*it)->name;
            if ( NameValuePair::EQUALS == (*it)->evalType )
            {
                constraint += " == '";
            }
            else // NOT_EQUALS
            {
                constraint += " != '";
            }
            constraint += (*it)->value + "' )";

            // if there are more constraints to go then put an "and" in there
            it++;
            if ( it != filterableData->end())
            {
                constraint += " and ( ";
            }
        }

        return constraint;
    }

    //xinsong ++ just for debug easily 
    //used to log specific message
    /*
        event.header.fixed_header.event_type.domain_name = CORBA::string_dup( domainName.c_str() );
        event.header.fixed_header.event_type.type_name = CORBA::string_dup( typeName.c_str() );
    */

    void gLogSpecificMessage(const CosNotification::StructuredEvent& event, const char * infoPrefix)
    {
        return;
        
        const char * msgContext = event.header.fixed_header.event_type.domain_name.in();
        const char * msgTypeKey = event.header.fixed_header.event_type.type_name.in();
        //i need to log AuthenticationAgent bindrequest/rebind message
        bool needToLog = false;
        if ( 0 == strcmp(msgContext, NamingService::Context.getContextName().c_str()) )
        {
            if ( strstr(msgTypeKey, "OccAuthenticationAgent") != NULL)
            {
                needToLog = true;
            }
            else if ( strstr(msgTypeKey, "OccDutyAgent") != NULL)
            {
                needToLog = true;
            }
            
            if ( needToLog )
            {
                std::string eventDesc = gStringFromEventHeader(event);
                
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  
                    "+++LogMessage+++,  %s, send specific messge: <%s>", infoPrefix, eventDesc.c_str() );
            }
        }

        return;
    }


}; // TA_Base_Core

