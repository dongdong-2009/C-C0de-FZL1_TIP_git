#if !defined(PRINTABLEEVENTGENERATOR_H_INCLUDED)
#define PRINTABLEEVENTGENERATOR_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintableEventGenerator.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Reads new events from the database and submits them to the printing service.
  * The database is polled at regular intervals as specified in agent configuration.
  *
  */

namespace TA_Base_App
{
    class PrintableEventGenerator : public ACE_Task<ACE_MT_SYNCH>,
                                    public IConfigDataObserver
    {
    // Operations
    public:
        PrintableEventGenerator(PrintingService& printingService, TA_Base_Core::EOperationMode operationMode);
        virtual ~PrintableEventGenerator();

        /**
         * setOperationMode 
         */	
        void setOperationMode(TA_Base_Core::EOperationMode operationMode);

        /**
         * setLastProcessedEvent 
         */	
        void setLastProcessedEvent(EventSequenceNumber lastProcessedEvent);

        /**
         * publishState
         *
         * Sends out a state update for the current state of processed events. 
         */	
        void publishState();

        /**
         * start 
         *
         * Activates the PrintableEventGenerator task.
         */	
        virtual void start();

        /**
         * terminate 
         *
         * Shuts down the PrintableEventGenerator task.
         */	
        virtual void terminate();

        /**
         * parametersChanged
         *
         * Implements IConfigDataObserver::parametersChanged().
         */
        virtual void parametersChanged(const EntityParameterList& parameters);
 
    protected:
        /**
         * getOperationMode 
         */	
        virtual TA_Base_Core::EOperationMode getOperationMode();

        /**
         * getLastProcessedEvent 
         */	
        virtual EventSequenceNumber getLastProcessedEvent();

        /**
         * initialiseLastProcessedEvent
         *
         * In control mode sets the last processed event based on the most recent event
         * in the database, in monitor mode sends out a request for a state update. 
         */	
        virtual void initialiseLastProcessedEvent();

        /**
         * printOutstandingEvents
         *
         * Looks up the database for new events and submits them to the printing service.
         */        
        virtual void processOutstandingEvents();

       /**
         * printEvents
         *
         * Submits the specified events to the printing service for queuing to the printer.
         * Returns true to indicate that processing of events should continue, false otherwise.
         */
        virtual bool printEvents(const PrintableData& printableData);

        /**
         * sendMessage
         *
         * Puts an empty message of the specified type on the task's message queue and
         * returns the result.
         */ 
        virtual int sendMessage(int msgType);

        /**
         * open/close
         *
         * Task activation/cleanup methods.
         */
        virtual int open(void* args);
        virtual int close(unsigned long flags = 0);

        /**
         * svc
         *
         * Entry point for all threads created by the task.
         */
        virtual int svc(void);

    private:
        PrintableEventGenerator(const PrintableEventGenerator&);
        PrintableEventGenerator& operator=(const PrintableEventGenerator&);        


    // Attributes
    private:
        PrintingService& m_printingService;
        StateUpdateSender m_stateUpdateSender;
        std::vector<std::string> m_TableColumnNames;

        TA_Base_Core::ReadWriteThreadLockable m_readWriteLock;
        TA_Base_Core::EOperationMode m_operationMode;
        EventSequenceNumber m_lastProcessedEvent;
    };
}

#endif // !defined(PRINTABLEEVENTGENERATOR_H_INCLUDED)
