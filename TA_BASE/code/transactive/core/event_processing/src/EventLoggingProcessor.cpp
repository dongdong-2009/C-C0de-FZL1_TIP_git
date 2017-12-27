/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/EventLoggingProcessor.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This processor is responsible for writing events into the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/event_processing/src/EventLoggingProcessor.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/message/src/MessageConfig.h"

namespace TA_Base_Core
{

    // The value of this member is the same as that defined in ConfigMesageType.cpp
    const std::string EventLoggingProcessor::LOG_MESSAGE_STATUS = "Log Status";

    NonReEntrantThreadLockable EventLoggingProcessor::m_cacheLock;


    EventLoggingProcessor::EventLoggingProcessor()
    {
        OnlineUpdateListener::getInstance().registerAllInterests(MESSAGE_TYPE , *this);
    }


    EventLoggingProcessor::~EventLoggingProcessor()
    {
        OnlineUpdateListener::getInstance().deregisterAllInterests(MESSAGE_TYPE, *this);
    }


    bool EventLoggingProcessor::processEvent( Event& event )
    {
        FUNCTION_ENTRY( "processEvent" );

        bool processingSuccessful = true;

        if(!shouldWriteToDatabase(event))
        {
            return processingSuccessful;
        }

        try
        {
            // Instruct the object to write itself to the database
            event->writeToDatabase();
        }
        catch(const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what() );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Audit item not logged - Retrying..." );

            processingSuccessful = false;
        }
        catch(const DataException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.what() );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Audit item not logged" );
        }
        catch(const DataConfigurationException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataConfigurationException", ex.what() );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Audit item not logged" );

        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "unknown", "Unknown exception thrown while trying to insert item into database." );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Audit item not logged" );
        }

        FUNCTION_EXIT;
        return processingSuccessful;
    }


    void EventLoggingProcessor::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        if (updateEvent.getType() != MESSAGE_TYPE)
        {
            // Only interested in alarm plan association updates
            FUNCTION_EXIT;
            return;
        }

        // We are only listening for update events for Event Types. Since event types cannot
        // be created or deleted, we only deal with the 'Update' type.

        if(updateEvent.getModifications() == Update)
        {
            unsigned long eventTypeKey = updateEvent.getKey();
            std::ostringstream str;
            str << eventTypeKey;

            bool found = false;
            {
                // Guard the cache (m_eventTypeLogEventMap)
                ThreadGuard guard(m_cacheLock);
                // Only process this update if we have this event type cached
                std::map<std::string, bool>::iterator mapIter = m_eventTypeLogEventMap.find(str.str());
                found = (mapIter != m_eventTypeLogEventMap.end());
            }
            
            if(found)
            {
                std::vector<std::string> changedParams = updateEvent.getChangedParams();
                std::vector<std::string>::iterator iter = changedParams.begin();
                for(; iter != changedParams.end(); ++iter)
                {
                    if(*iter == LOG_MESSAGE_STATUS)
                    {
                        bool shouldWrite = MessageTypeTableAccessor::getInstance().isLoggable(str.str(), true);

                        // Guard the cache (m_eventTypeLogEventMap)
                        ThreadGuard guard(m_cacheLock);

                        m_eventTypeLogEventMap[str.str()] = shouldWrite;
                        break;
                    }
                }
            }
        }
        
        FUNCTION_EXIT;
    }


    bool EventLoggingProcessor::shouldWriteToDatabase(Event& event)
    {
        // First check if the event type is cached
        std::ostringstream str;
        {
            ThreadGuard guard(m_cacheLock);

            str << event->getEventTypeKey();
       
            std::map<std::string, bool>::iterator iter = m_eventTypeLogEventMap.find(str.str());
        
            if(iter != m_eventTypeLogEventMap.end())
            {
                return iter->second;
            }
        }

        bool shouldWrite = MessageTypeTableAccessor::getInstance().isLoggable(str.str());
        // Guard the cache (m_eventTypeLogEventMap)
        ThreadGuard guard(m_cacheLock);
        m_eventTypeLogEventMap.insert(std::map<std::string, bool>::value_type(str.str(), shouldWrite));
        return shouldWrite;
    }
};
