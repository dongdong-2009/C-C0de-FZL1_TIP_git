/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessageConfig.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/MessageConfig.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/data_access_interface/src/MessageTypeData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/IDL/src/GenericMessageCorbaDef.h"

#include <boost/regex.hpp>


namespace TA_Base_Core
{
    std::string gFindAndReplaceParams( const std::string& s, const DescriptionParameters& replacementList)
    {
        // The description
        std::string description( s );

        // The location counter
        std::string::size_type pos(0);
        for (unsigned int i = 0; i < replacementList.size(); i++)
        {
            // Add the opening and closing brace to the name
            std::string name( "[" + replacementList[i]->name + "]" );

            // Find the first name
            pos = description.find( name );

            // If found
            if ( pos != description.npos )
            {
                description.erase( pos, name.length() );
                description.insert( pos, replacementList[i]->value );
            }
        }

        return description;
    }

    void gGetStringFromParameters(const DescriptionParameters& parameters, std::string& paramStr)
    {
        // loop through the vector and generate the string
        for (unsigned int i = 0; i < parameters.size(); i++)
        {
            paramStr.append( parameters[i]->name + ":" );
            paramStr.append( parameters[i]->value + ",");
        }
    }

    void gGetParametersFromString(const std::string& paramStr, DescriptionParameters& parameters)
    {
        // go through the string and rip out the necessary bits
        /*unsigned int pos(0);
        while ( pos < paramStr.length() )
        {
        unsigned int nameEnds( paramStr.find(':', pos) );

          if (nameEnds == std::string::npos)
          {
            break;
          }

            // If we found a ':' we will have a ','
            unsigned int valueEnds( paramStr.find(',', nameEnds) );

              if(valueEnds == std::string::npos)
              {
                return;
              }

                parameters.push_back(
                new NameValuePair( paramStr.substr( pos, ( nameEnds - pos ) ),
                paramStr.substr( nameEnds + 1, ( valueEnds - ( nameEnds + 1) ) ) ) );

                  pos = valueEnds + 1 ;
        }*/
        boost::regex expression("([^:,]+):([^:]+|[^,]+),");
        std::string::const_iterator start = paramStr.begin();
        std::string::const_iterator end = paramStr.end();
        boost::match_results<std::string::const_iterator> what;
        boost::match_flag_type flags = boost::match_default;
        while(boost::regex_search(start, end, what, expression, flags))
        {
            parameters.push_back(
                new NameValuePair(std::string(what[1].first, what[1].second),
                std::string(what[2].first, what[2].second)));
            start = what[0].second;
            // update flags:
            flags |= boost::match_prev_avail;
            flags |= boost::match_not_bob;
        }
    }


    // initialise statics
    TA_Base_Core::NonReEntrantThreadLockable MessageTypeTableAccessor::m_singletonLock;
    MessageTypeTableAccessor* MessageTypeTableAccessor::m_theClassInstance = NULL;

    MessageTypeTableAccessor::~MessageTypeTableAccessor()
    {
        {
            TA_THREADGUARD( m_mapLock );

            MsgTypeMap::iterator it = m_msgTypeMap.begin();
            for ( ; it != m_msgTypeMap.end(); it++ )
            {
                delete it->second;
            }
            m_msgTypeMap.clear();
        }

        m_theClassInstance = NULL;
    }


    MessageTypeTableAccessor& MessageTypeTableAccessor::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL
        if ( NULL == m_theClassInstance )
        {
            // Create the one & only object
            m_theClassInstance = new MessageTypeTableAccessor();
        }
        
        return *m_theClassInstance;
    }

    void MessageTypeTableAccessor::destroyInstance()
    {
        TA_THREADGUARD( m_singletonLock );
        delete m_theClassInstance;
        m_theClassInstance = NULL;
    }

    bool MessageTypeTableAccessor::isLoggable(const MessageType& messageType)
    {
        // Load the message Type data
        TA_Base_Core::IMessageTypeData* msgTypeData =
            getMessageTypeRecord( messageType.getTypeKey() );

        // Return default if null
        if ( NULL == msgTypeData )
        {
            return false;
        }

        try
        {
            TA_THREADGUARD( m_daiLock );
            return msgTypeData->toBeLogged();
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to determine if Message was loggable" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }
        return false;
    }


    bool MessageTypeTableAccessor::isLoggable(const std::string& messageTypeKey, bool reload /*= false*/)
    {
        // Load the message Type data
        TA_Base_Core::IMessageTypeData* msgTypeData =
            getMessageTypeRecord( messageTypeKey, reload );

        // Return default if null
        if ( NULL == msgTypeData )
        {
            return false;
        }
        
        try
        {
            TA_THREADGUARD( m_daiLock );
            return msgTypeData->toBeLogged();
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to determine if Message was loggable" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }
        return false;
    }


    bool MessageTypeTableAccessor::isPrintable(const MessageType& messageType)
    {
        // Load the message Type data
        TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageType.getTypeKey() );

        // Return default if null
        if ( NULL == msgTypeData )
        {
            return false;
        }

        try
        {
            TA_THREADGUARD( m_daiLock );
            return msgTypeData->toBePrinted();
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to determine if Message was printable" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }
        return false;
    }


    short MessageTypeTableAccessor::getPriority(const MessageType& messageType)
    {
        // Load the message Type data
        TA_Base_Core::IMessageTypeData* msgTypeData =
            getMessageTypeRecord( messageType.getTypeKey() );

        // Return default if null
        if ( NULL == msgTypeData )
        {
            return 0;
        }

        try
        {
            TA_THREADGUARD( m_daiLock );
            return msgTypeData->getPriority();
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to determine Message priority" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }            
        
        return 0;
    }


	bool MessageTypeTableAccessor::isRealTimeEvent(const std::string& messageTypeKey)
	{
		// Load the message Type data
		TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageTypeKey );

		// Return default if null
		if ( NULL == msgTypeData )
		{
			return false;
		}

		try
		{
			TA_THREADGUARD( m_daiLock );
			return msgTypeData->getIsEnableRealTimeEvent();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Message was RealTime enabled " );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load MessageTypeData" );
		}
		return false;
	}

	unsigned int MessageTypeTableAccessor::getViewLevel(const std::string& messageTypeKey)
	{
		// Load the message Type data
		TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageTypeKey );

		// Return default if null
		if ( NULL == msgTypeData )
		{
			return 0;
		}

		try
		{
			TA_THREADGUARD( m_daiLock );
			return msgTypeData->getViewLevel();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine the view level of the Message" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load MessageTypeData" );
		}
		return 0;
	}

	unsigned long MessageTypeTableAccessor::getActionId(const std::string& messageTypeKey)
	{
		// Load the message Type data
		TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageTypeKey );

		// Return default if null
		if ( NULL == msgTypeData )
		{
			return 0;
		}

		try
		{
			TA_THREADGUARD( m_daiLock );
			return msgTypeData->getActionId();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine the action Id of the Message" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load MessageTypeData" );
		}
		return 0;
	}

	std::string MessageTypeTableAccessor::getMessageTypeName(const std::string& messageTypeKey)
	{
		// Load the message Type data
		TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageTypeKey );

		// Return default if null
		if ( NULL == msgTypeData )
		{
			return "";
		}

		try
		{
			TA_THREADGUARD( m_daiLock );
			return msgTypeData->getName();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine the name the Message Type" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load MessageTypeData" );
		}
		return "";
	}

    std::string MessageTypeTableAccessor::getExpandedDescription( const MessageType& messageType,
                                                                  const DescriptionParameters& parameters)
    {
        // Load the message Type data
        TA_Base_Core::IMessageTypeData* msgTypeData = getMessageTypeRecord( messageType.getTypeKey() );

        // Return default if null
        if ( NULL == msgTypeData )
        {
            return "Default";
        }

        try
        {
            // msgTypeData looks like a local variable but it isn't and since the DAI
            // stuff is not thread safe, we have to protect it here.
            TA_THREADGUARD( m_daiLock );
            return gFindAndReplaceParams( msgTypeData->getDescription(), parameters );
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to format message description" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }
        return "Default";
    }


    TA_Base_Core::IMessageTypeData* MessageTypeTableAccessor::getMessageTypeRecord( const std::string& typeKey,
                                                                                    bool reload /*= false*/)
    {
        if (reload)
        {
            try
            {
                // load from the database
                TA_Base_Core::IMessageTypeData* msgTypeData =
                    TA_Base_Core::MessageTypeAccessFactory::getInstance().getMessageTypeByKey( typeKey, false );

                TA_THREADGUARD( m_mapLock );
                // Add to the map
                m_msgTypeMap[typeKey] = msgTypeData;
                
                return msgTypeData;
            }
            catch ( const TA_Base_Core::DataException& /*ex */)
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                    "DataException", "Failed to load MessageTypeData" );
                return NULL;
            }
            catch ( const TA_Base_Core::DatabaseException& /*ex */)
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                    "DatabaseException", "Failed to connect to database to load MessageTypeData" );
                return NULL;
            }
        }

        TA_THREADGUARD( m_mapLock );

        // Find the object in the map
        MsgTypeMap::iterator it = m_msgTypeMap.find(typeKey);

        //did we find it?
        if ( it != m_msgTypeMap.end() )
        {
            // Return the object
            return it->second;
        }

        return NULL;
    }


    MessageTypeTableAccessor::MessageTypeTableAccessor()
    {
        try
        {
            // load from the database
            std::vector<IMessageTypeData*> messageTypes;
            std::vector<std::string> nullMessageFilter;
            messageTypes = TA_Base_Core::MessageTypeAccessFactory::getInstance().getAllMessageTypesAndAttributes( nullMessageFilter );

            for ( std::vector<IMessageTypeData*>::iterator itr = messageTypes.begin(); itr != messageTypes.end(); ++itr )
            {
                std::ostringstream typeKey;
                typeKey << (*itr)->getKey();
                m_msgTypeMap[typeKey.str()] = *itr;
            }
        }
        catch ( const TA_Base_Core::DataException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DataException", "Failed to load MessageTypeData" );
        }
        catch ( const TA_Base_Core::DatabaseException& /*ex */)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "DatabaseException", "Failed to connect to database to load MessageTypeData" );
        }
    }


}; // TA_Base_Core

