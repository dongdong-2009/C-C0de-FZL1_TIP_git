#if !defined(TA_CosUtility_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
#define TA_CosUtility_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TA_CosUtility.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Utility functions for use with Corba COS Notification library
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "COS/CosNotifyFilter.hh"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/NameValuePair.h"
#include <sstream>

namespace CORBA
{
    class Any;
};

// TODO LPP: This header contains some functions that don't really belong in 
// such a general header.

namespace TA_Base_Core
{
    // Functions for converting an event header and other corba types to/from a string.

    // Converts the given EventType to a string (useful for logging) of the form
    // (domain_name0, type_name0)
    std::string gStringFromEventType(const CosNotification::EventType& type);

    // Converts the given EventTypeSeq to a string (useful for logging) of the form
    // {eventtype1, eventtype2, ..., eventtypen}
    std::string gStringFromEventTypes(const CosNotification::EventTypeSeq& seq);

    std::string gStringFromCorbaAny(const CORBA::Any& val);

    // Names of fixed fields in the event header.
    extern const char* DOMAIN_NAME_FIELDNAME;  // "domain_name"
    extern const char* TYPE_NAME_FIELDNAME;    // "type_name"
    extern const char* EVENT_NAME_FIELDNAME;   // "event_name"
    // yanrong++: CL-21199, 2010-01-19
    extern const char* SENDER_ENTITY_FIELDNAME;   // "sender_entity"
    // ++yanrong

    // Converts the header and filterable_data elements of the event to a string of the
    // form {(fieldName1, fieldValue1), (fieldName2, fieldValue2), ..., (fieldNamen, fieldValuen),}
    std::string gStringFromEventHeader(const CosNotification::StructuredEvent& event, bool fGetBasicInfo = false);

    // Searches the variable_header and filterable_data headers in 'event' for the
    // given name.  If found, the value is copied into 'value' and true is returned.
    // Otherwise false is returned.
    bool getValueFromEventHeader(const CosNotification::StructuredEvent& event, const std::string& name, std::string& value);

    // Returns the index of the first (name, value) with the given name, or an index
    // equal to the length of the sequence if none is found.
    unsigned int findFirstInSequence(const CosNotification::PropertySeq& seq, const std::string& name);

    // Returns a std::string representation of the object reference 'obj'
    // by calling CORBA::object_to_string().
    std::string gObjectRefToStr(CORBA::Object_ptr obj);


    // Entity keys for related entities will often be 'close together' numerically.
    // Clients that subscribe to messages are often interested in a set of related
    // entities.  If the 'segmented' entity key is used for server-side filtering
    // rather than the complete entity key, any message with a key in a certain 
    // range (eg. in groups of 10) will be accepted.  Then a single constraint can 
    // be used for a group of entities, thereby reducing the number of constraints 
    // on the server side and improving performance.  A client-side filter would be 
    // used to eliminate all messages except those with the exact entity key.
    std::string gGetSegmentedEntityKey( unsigned long entityKey );


    // The type_name separator character is used when populating an EventType's
    // type_name with more than one field.  For example, <agentName>_<typeName>.
    extern const char* TYPE_NAME_SEPARATOR;  // = "_"
    extern const char* WILDCARD_STR;         // = "*"

    // The Corba Notification Service considers any of the following to be equivalent:
    // ("", "%ALL"), ("", ""), ("*", ""), ("", "*"), ("*", "*")
    // gIsStarStar() returns true if (domain_name, type_name) are either of:
    // (?, "%ALL"), ("*", "*")

    // TODO LPP: Why do we only check for two of the equivalent (*, *) types?
    // TODO LPP: RDIEvent.h in OmniNotify 2.1 states that "%ALL only valid if domain_name 
    // is * (or empty ... which was changed to *)".  Should we check for this here?
    bool gIsStarStar( const CosNotification::EventType& eventType );


    // This function will return the location key as specified by RPARAM_LOCATIONKEY.
    // Location is a logical rather than physical concept - eg. processes can execute on
    // the same physical host but each have a different logical location (and vice versa).
    // If RPARAM_LOCATIONKEY is not set gGetLocalLocationKey() will assert.
    unsigned long gGetLocalLocationKey();

    // Returns a string of the form "hostname:portnum"
    inline std::string gMakeServiceAddr(const std::string& hostname, unsigned short portnum)
    {
        std::ostringstream serviceAddr;
        serviceAddr << hostname << ":" << portnum;

        return serviceAddr.str();
    }


    // Sets (ie. replaces) the fields in the header of the StructuredEvent 'event' as follows:
    // - domainName is mapped to the domain_name field of the Structured Event
    // - typeName is mapped to the type_name field of the Structured Event
    // - variableHeaderData (name,value) pairs are mapped to the ohf_name,ohf_value fields
    //   (ie. in $.header.variable_header)
    // - filterableData (name,value) pairs are mapped to the fd_name,fd_value fields
    //   (ie. in $.header.filterable_data)

    // FilterData is a typedef for std::vector<NameValuePair*>.

    void gPopulateStructuredEventHeader( CosNotification::StructuredEvent& event,
                                         const std::string& domainName,
                                         const std::string& typeName,
                                         const FilterData* variableHeaderData,
                                         const FilterData* filterableData );

    // Copies the (name, value) fields in 'data' to 'seq'.  Existing fields in the 
    // PropertySeq will not be modified.
    void gAppendFilterDataToPropertySeq(CosNotification::PropertySeq& seq, const FilterData& data);

    // Appends a single EventType (domainName, typeName) to the given sequence (the length of
    // the sequence is increased by 1).
    void gAppendToEventTypeSeq( CosNotification::EventTypeSeq& seq, 
                                const std::string& domainName, 
                                const std::string& typeName );

    // Appends typeNames.size() EventTypes to the given sequence (the length of the sequence
    // is increased by typeNames.size() ). 
    // - the domain_name for each EventType is set to domainName.
    // - the type_name for each EventType is set to the corresponding element in typeNames
    void gAppendToEventTypeSeq( CosNotification::EventTypeSeq& seq, 
                                const std::string& domainName, 
                                const std::vector<std::string>& typeNames );


    // Returns a constraint expression for use with CosNotifyFilter::ConstraintExp 
    // objects of the form
    // - "true" if filterableData is empty or NULL, otherwise
    // - "($name1 comparison1 'value1') and ($name2 comparison2 'value2') ..."
    // where (nameN, valueN) are FilterData name-value pairs, and comparisonN
    // is either "==" or "!=" (depending on the FilterData evalType).
    // FilterData is a typedef for std::vector<NameValuePair*>.

    // TODO LPP: How is the value in the event header (a Corba::Any) compared with the 
    // filter value (a string)?  ie. what conversions are applied to the Any?
    std::string gGenerateConstraintExpression(const FilterData* filterableData);

    void gLogSpecificMessage(const CosNotification::StructuredEvent& event, const char * infoPrefix); 


    // Useful defaults for setting the batch size and batching interval of the BatchingQueue<T>.
    const unsigned int DEFAULT_BATCH_SIZE = 10;
    const unsigned int DEFAULT_SUCCESS_BATCH_INTERVAL = 50;
    const unsigned int DEFAULT_FAILED_BATCH_INTERVAL = 1000;

}; // TA_Base_Core


#endif // !defined(TA_CosUtility_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
