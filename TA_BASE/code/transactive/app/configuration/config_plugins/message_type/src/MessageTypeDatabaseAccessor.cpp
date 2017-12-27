/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/message_type/src/MessageTypeDatabaseAccessor.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve MessageType information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "app/configuration/config_plugins/message_type/src/StdAfx.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigMessageType.h"
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const std::string MessageTypeDatabaseAccessor::AUDIT_CHANNEL_NAME = "Audit";
    
    
    MessageTypeDatabaseAccessor::MessageTypeDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    MessageTypeDatabaseAccessor::~MessageTypeDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedMessageTypes::iterator iter = m_messageTypes.begin(); iter != m_messageTypes.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void MessageTypeDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the message types and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedMessageTypes::iterator messageTypeToDelete = m_messageTypes.begin(); messageTypeToDelete != m_messageTypes.end(); ++messageTypeToDelete)
        {
            if( (messageTypeToDelete->second) != NULL )
            {
                delete messageTypeToDelete->second;
                messageTypeToDelete->second = NULL;
            }
        }
        m_messageTypes.clear();


        // Now reload all the Audit message types
        std::vector<std::string> auditMessageFilter;
        auditMessageFilter.push_back(AUDIT_CHANNEL_NAME);

        bool writable = true;
        std::vector<TA_Base_Core::IMessageTypeData*> messageTypes =
            TA_Base_Core::MessageTypeAccessFactory::getInstance().getAllMessageTypes(
                writable, auditMessageFilter);

        // And insert the message types into our map. At the same time cast them to IConfigMessageType*
        // instead of IMessageTypeData*.
        for (std::vector<TA_Base_Core::IMessageTypeData*>::iterator iter = messageTypes.begin();
             iter != messageTypes.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigMessageType* castMessageType = dynamic_cast<TA_Base_Core::IConfigMessageType*>(*iter);
             if( castMessageType != NULL )
             {
                m_messageTypes.insert( LoadedMessageTypes::value_type(castMessageType->getUniqueIdentifier(), castMessageType) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> MessageTypeDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of message types times by 2. This is because we want to step for each
        // MessageType while it is loaded and then again while each MessageType is added to the list.
        progress.SetRange( 0, m_messageTypes.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedMessageTypes::iterator iter = m_messageTypes.begin();
             iter != m_messageTypes.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                //names.insert( std::map<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
				names.insert( std::multimap<CString, unsigned long>::value_type( iter->second->getDisplayName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* MessageTypeDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the MessageType matching the key passed in.
        LoadedMessageTypes::iterator matching = m_messageTypes.find(key);
        if (matching == m_messageTypes.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Alarm Type with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void MessageTypeDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* MessageTypeDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        FUNCTION_EXIT;
        return NULL;
    }


    TA_Base_Core::IConfigItem* MessageTypeDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");
        FUNCTION_EXIT;
        
        return NULL;
    }


    bool MessageTypeDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& messageTypesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(messageTypesNotApplied.empty(), "The vector of strings passed was empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedMessageTypes::iterator iter = m_messageTypes.begin(); iter != m_messageTypes.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    messageTypesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Alarm Type that has not had it's changes applied yet");
                    messageTypesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Alarm Type that has not had it's changes applied yet");
                    messageTypesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !messageTypesNotApplied.empty();
    }


}
