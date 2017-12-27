/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/EventLoggingProcessor.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This processor is responsible for writing events into the database.
  *
  */

#if !defined(_EVENT_LOGGING_PROCESSOR_H_)
#define _EVENT_LOGGING_PROCESSOR_H_

#include <map>

#include "core/event_processing/src/IEventProcessor.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace
{
    const unsigned char RETRY_COUNT(5);
    const unsigned long RETRY_DELAY(1000);
}

namespace TA_Base_Core
{
    class EventLoggingProcessor : public IEventProcessor, public virtual IOnlineUpdatable
    {
    public:
        /**
         * Constructor
         */
        EventLoggingProcessor();


        /**
         * Destructor
         */
        virtual ~EventLoggingProcessor();


        /**
         * getNumRetries
         *
         * Returns the number of times this processor would like to retry the processing
         * of an event in the situation where it fails.
         *
         * @return unsigned char The number of times to retry.
         */
        virtual unsigned char getNumRetries()
        {
            return RETRY_COUNT;
        }


        /**
         * getRetryDelay
         *
         * Returns the amount of number of milliseconds to wait before retrying the processing.
         *
         * @return unsigned long The number of milliseconds wait.
         */
        virtual unsigned long getRetryDelay()
        {
            return RETRY_DELAY;
        }


        /** 
          * processEvent
          *
          * This method will perform some processing on the provided event.
          *
          * @param event The event to process.
          *
          * @return bool True if the event processing succeeded. False will result in the processing
          *              being retried.
          */
        virtual bool processEvent( Event& event );

        
        /**
         * processUpdate
         *
         * When there is a configuration update of an alarm plan association then this method
         * will be invoked to process the update accordingly.
         *
         * @param ConfigUpdateDetails& - The update details so we know what to modify
         */
        virtual void processUpdate(const ConfigUpdateDetails& updateEvent);

    private:

        bool shouldWriteToDatabase(Event& event);

    private:
        // Hidden methods.
        EventLoggingProcessor( const EventLoggingProcessor& );
        EventLoggingProcessor& operator=( const EventLoggingProcessor& );

        std::map<std::string, bool> m_eventTypeLogEventMap;

        static const std::string LOG_MESSAGE_STATUS;
        static const std::string AUDIT_CHANNEL_NAME;

        static NonReEntrantThreadLockable m_cacheLock;
    };
};

#endif // !defined(_EVENT_LOGGING_PROCESSOR_H_)

