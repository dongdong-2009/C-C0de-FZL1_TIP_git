/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintableEventGenerator.cpp $
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

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    const std::string PKEY_COLUMN_NAME("PKEY");
    const std::string PRINTER_COLUMN_NAME("PRINTER");
    const std::string ALARMFLAG_COLUMN_NAME("ALARMFLAG"); // ALARM: alarms; ALARMEVENT: alarm related events; EVENT: other events
	//TD14536
    const std::string ALARM_TYPE("ALARM");
    const std::string ALARMEVENT_TYPE("ALARMEVENT");
    //const std::string EVENT_TYPE("EVENT");

    const unsigned long EVENT_BATCH_SIZE(100); // max number of events to read from db at a time

    const long THREAD_PROPERTIES(THR_SCOPE_PROCESS | THR_JOINABLE);
    const int THREAD_COUNT(1);
    const ACE_Time_Value MQ_ENQUEUE_TIMEOUT(1, 0); // 1 second
    const int GENERAL_ACE_ERROR(-1);
    
    // Custom message block types
    enum
    {
        MB_OPMODE = ACE_Message_Block::MB_USER | 0x01,
        MB_CONFIG = ACE_Message_Block::MB_USER | 0x02
    };
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


PrintableEventGenerator::PrintableEventGenerator(PrintingService& printingService, TA_Base_Core::EOperationMode operationMode) :
    m_printingService(printingService),
    m_operationMode(operationMode),
    m_lastProcessedEvent(TA_Base_Core::PrintableEventAccessFactory::INVALID_SEQUENCE_NUMBER)
{
    FUNCTION_ENTRY("PrintableEventGenerator(PrintingService& printingService)");

    // Set up the names of extractable event view columns.
    m_TableColumnNames.push_back(PKEY_COLUMN_NAME);
    m_TableColumnNames.push_back(PRINTER_COLUMN_NAME);
    m_TableColumnNames.push_back(ALARMFLAG_COLUMN_NAME);

    // TD14045
    //const ColumnList& printableColumns(ConfigData::getInstance().getColumns());
    const ColumnList& printableColumns = ConfigData::getInstance().getColumns();

    for (ColumnList::const_iterator colIt(printableColumns.begin()); colIt != printableColumns.end(); colIt++)
    {
        // Make sure column names are not duplicated.
        if (colIt->name == PKEY_COLUMN_NAME || colIt->name == PRINTER_COLUMN_NAME || colIt->name == ALARMFLAG_COLUMN_NAME)
        {
            continue;
        }

        m_TableColumnNames.push_back(colIt->name);
    }
    
    // Determine the starting point for processing new events.
    initialiseLastProcessedEvent();

    FUNCTION_EXIT;
}


PrintableEventGenerator::~PrintableEventGenerator()
{
    FUNCTION_ENTRY("~PrintableEventGenerator()");
    FUNCTION_EXIT;
}


void PrintableEventGenerator::setOperationMode(TA_Base_Core::EOperationMode operationMode)
{
    std::ostringstream fn;
    fn << "setOperationMode(" << (operationMode == TA_Base_Core::Control? "CONTROL" : "MONITOR") << ")";
    FUNCTION_ENTRY(fn.str().c_str());

    {
        TA_Base_Core::ThreadGuard guard(m_readWriteLock);

        if (m_operationMode == operationMode)
        {
            std::ostringstream msg;
            msg << "Already in " << (operationMode == TA_Base_Core::Control? "CONTROL" : "MONITOR") << " mode, ignoring set request.";
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, msg.str().c_str());

            FUNCTION_EXIT;
            return;
        }

        m_operationMode = operationMode;
    }
    
    // Let the service thread know immediately when going into control mode.
    if (operationMode == TA_Base_Core::Control)
    {
        if (sendMessage(MB_OPMODE) == GENERAL_ACE_ERROR)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
                        "Failed to enqueue MB_OPMODE msg, mode change will not take effect until next database poll.");
        }
    }

    FUNCTION_EXIT;
}


void PrintableEventGenerator::setLastProcessedEvent(EventSequenceNumber lastProcessedEvent)
{
    std::ostringstream fn;
    fn << "setLastProcessedEvent(" << lastProcessedEvent << ")";
    FUNCTION_ENTRY(fn.str().c_str());

    TA_Base_Core::ThreadGuard guard(m_readWriteLock);

    if (m_lastProcessedEvent == lastProcessedEvent)
    {
        std::ostringstream msg;
        msg << "Last processed event already set to " << lastProcessedEvent << ", ignoring set request.";
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, msg.str().c_str());

        FUNCTION_EXIT;
        return;
    }

    m_lastProcessedEvent = lastProcessedEvent;
    TA_Base_Core::PrintableEventAccessFactory::getInstance().removePrintedEvents(m_lastProcessedEvent, ConfigData::getInstance().getAgentEntityLocationKey());
    
    if (m_operationMode == TA_Base_Core::Control)
    {
        // Keep the monitor agent informed.
        m_stateUpdateSender.sendEventProcessedUpdate(lastProcessedEvent);
    }
    
    FUNCTION_EXIT;
}


TA_Base_Core::EOperationMode PrintableEventGenerator::getOperationMode()
{
//    FUNCTION_ENTRY("getOperationMode()");

    TA_Base_Core::ThreadReadGuard guard(m_readWriteLock);

//    FUNCTION_EXIT;
    return m_operationMode;
}


EventSequenceNumber PrintableEventGenerator::getLastProcessedEvent()
{
//    FUNCTION_ENTRY("getLastProcessedEvent()");

    TA_Base_Core::ThreadReadGuard guard(m_readWriteLock);

//    FUNCTION_EXIT;
    return m_lastProcessedEvent;
}


void PrintableEventGenerator::initialiseLastProcessedEvent()
{
    FUNCTION_ENTRY("initialiseLastProcessedEvent()");

    try
    {
        if (getOperationMode() == TA_Base_Core::Control)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Initilising last processed event using the db event message sequence.");

            // when agent startup, start from 0
            m_lastProcessedEvent = TA_Base_Core::PrintableEventAccessFactory::INVALID_SEQUENCE_NUMBER;
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Requesting state update from the control agent.");

            m_stateUpdateSender.requestStateUpdate();
        }
    }
    catch (const TA_Base_Core::DatabaseException& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", ex.what());

        TA_THROW(OnlinePrintingAgentException("Unable to initialise last processed event", ex.what()));
    }
    catch (const TA_Base_Core::DataException& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", ex.what());

        TA_THROW(OnlinePrintingAgentException("Unable to initialise last processed event", ex.what()));
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception", "");

        TA_THROW(OnlinePrintingAgentException("Unable to initialise last processed event"));
    }

    FUNCTION_EXIT;
}


void PrintableEventGenerator::publishState()
{
    FUNCTION_ENTRY("publishState()");

    // The state presently includes just the last processed event.
    m_stateUpdateSender.sendEventProcessedUpdate(getLastProcessedEvent());

    FUNCTION_EXIT;
}


void PrintableEventGenerator::start()
{
    FUNCTION_ENTRY("start()");

    // Subscribe for updates to agent parameters.
    ConfigData::getInstance().registerObserver(*this);

    // Activate the service thread(s).
    if (open(0) == GENERAL_ACE_ERROR)
    {
        TA_THROW(OnlinePrintingAgentException("Failed to activate the printable event generator task."));
    }

    FUNCTION_EXIT;
}


void PrintableEventGenerator::terminate()
{
    FUNCTION_ENTRY("terminate()");

    // Unsubscribe from updates to agent parameters.
    ConfigData::getInstance().deregisterObserver(*this);

    // Send the threads a terminate message and wait for all to exit.
    if (sendMessage(ACE_Message_Block::MB_HANGUP) != GENERAL_ACE_ERROR)
    {
        wait();
    }

    FUNCTION_EXIT;
}


void PrintableEventGenerator::parametersChanged(const EntityParameterList& parameters)
{
    FUNCTION_ENTRY("parametersChanged(const EntityParameterList& parameters)");

    // When the polling frequency or printing enable state change, the service thread(s) need to be notified 
    // in case they are on a long dequeue timeout.
    for (EntityParameterList::const_iterator paramIt(parameters.begin()); paramIt != parameters.end(); paramIt++)
    {
        if (*paramIt == TA_Base_Core::OnlinePrintingAgentEntityData::VIEW_POLL_FREQUENCY_PARAM ||
            *paramIt == TA_Base_Core::OnlinePrintingAgentEntityData::IS_PRINTING_ENABLED_PARAM)
        {        
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Agent configuration changed, waking up service thread.");

            sendMessage(MB_CONFIG);
            break;
        }
    }

    FUNCTION_EXIT;
}


int PrintableEventGenerator::sendMessage(int msgType)
{
    FUNCTION_ENTRY("sendMessage(int msgType)");

    ACE_Message_Block* msg = new ACE_Message_Block;
    msg->msg_type(msgType);
    
    ACE_Time_Value mqTimeout(MQ_ENQUEUE_TIMEOUT);

    int result(putq(msg, &mqTimeout));

    FUNCTION_EXIT;    
    return result;
}


int PrintableEventGenerator::open(void* /*args*/)
{
    FUNCTION_ENTRY("open(void* args)");
    FUNCTION_EXIT;

    return activate(THREAD_PROPERTIES, THREAD_COUNT);
}


int PrintableEventGenerator::close(unsigned long /*flags*/)
{
    FUNCTION_ENTRY("close(unsigned long flags)");
    FUNCTION_EXIT;

    return 0;
}


int PrintableEventGenerator::svc(void)
{
    FUNCTION_ENTRY("svc()");

	int result(0);

	try
	{
        ACE_Time_Value pollFrequency(ConfigData::getInstance().getTablePollFrequency(), 0);
        ACE_Time_Value nextPollTime(ACE_OS::gettimeofday() + pollFrequency);
        ACE_Message_Block* msg = NULL;

        // Go into sleep and wake up at regular intervals to poll the database for new events to be printed.
        // Exit the loop when awakened by a hangup message.
		while (true)
		{
            // Clean up the previous message (if any).
			if (msg != NULL)
            {
                msg->release();
                msg = NULL;
            }

            // getq() will block until nextPollTime, unless a message arrives first.
 			if (getq(msg, &nextPollTime) != GENERAL_ACE_ERROR)
			{
                // Check for terminate requests.
				if (msg->msg_type() == ACE_Message_Block::MB_HANGUP)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received MB_HANGUP message.");

                    // Let other threads see the message and bail out.
					putq(msg);
					break;
				}

                // Ignore all messages other than those advising operation mode or config data change.
                switch (msg->msg_type())
                {
                    case MB_OPMODE:
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Received MB_OPMODE message.");
                        break;

                    case MB_CONFIG:
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Received MB_CONFIG message.");
                        break;

                    default:
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Ignoring unsupported message type (%d).", msg->msg_type());
                        continue; // dequeue the next message
                }       
			}
			else
			{
                // Check the message queue is OK.
				if (ACE_OS::last_error() != EWOULDBLOCK)
				{
					std::ostringstream msg;
					msg << "Failed to dequeue from message queue (errno " << ACE_OS::last_error() << ").";

					TA_THROW(OnlinePrintingAgentException(msg.str()));
				}
            }            

            // Right, it's time to poll the database for new events to be printed.
            processOutstandingEvents();

            // Set the time of the next database poll.
            pollFrequency.set(ConfigData::getInstance().getTablePollFrequency(), 0); // can change as it's online updateable
            nextPollTime = ACE_OS::gettimeofday() + pollFrequency;
		}
	}
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
        result = GENERAL_ACE_ERROR;
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
        result = GENERAL_ACE_ERROR;
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
        result = GENERAL_ACE_ERROR;
    }

    FUNCTION_EXIT;
	return result;
}


void PrintableEventGenerator::processOutstandingEvents()
{
    FUNCTION_ENTRY("processOutstandingEvents()");
   
    // Make sure the agent is actually allowed to print.
    if (getOperationMode() != TA_Base_Core::Control || !ConfigData::getInstance().isPrintingEnabled())
    {
        FUNCTION_EXIT;
        return;
    }

    // Make sure we have a starting point for processing events. Last processed event could
    // be invalid if a monitor agent went into control mode without first receiving the
    // requested state update from the control agent.
    if (getLastProcessedEvent() == TA_Base_Core::PrintableEventAccessFactory::INVALID_SEQUENCE_NUMBER)
    {
        initialiseLastProcessedEvent();
    }

    try
    {
        bool processingEvents(true);   

        while (processingEvents)
        {
            // Get from the event view the next batch of events following the last processed event.
            typedef std::vector<TA_Base_Core::IData*> DataList;
            DataList events(TA_Base_Core::PrintableEventAccessFactory::getInstance().getAllMessage(
                 m_TableColumnNames, getLastProcessedEvent(), EVENT_BATCH_SIZE, ConfigData::getInstance().getAgentEntityLocationKey()));
            TA_ASSERT(!events.empty(), "getMessagesFromView() returned an empty vector");

            for (DataList::iterator dataIt(events.begin()); dataIt != events.end(); dataIt++)
            {
                std::auto_ptr<TA_Base_Core::IData> eventData(*dataIt);
                TA_ASSERT(eventData.get() != NULL, "getMessagesFromView() returned NULL pointer");
                unsigned long rowCount(eventData->getNumRows());

                // See if there's any more events to be processed.
                if (rowCount == 0 && events.size() == 1)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No events to be processed."); 
                    processingEvents = false;
                }

                // The processing of events may have been interrupted prematurely (e.g. printing was disabled),
                // but we still want to iterate over any remaining IData record sets to make sure that they
                // are destroyed by the auto ptr.
                if (!processingEvents)
                {
                    continue;
                }

                PrintableDataElementList printableEvents;
                EventSequenceNumber currentEventNumber(getLastProcessedEvent());
                std::string currentPrinter("");

                // Iterate over all events in this event record set.
                for (unsigned long i(0); i < rowCount; i++)
                {
                    EventSequenceNumber eventNumber(eventData->getUnsignedLongData(i, PKEY_COLUMN_NAME));
					//TD14536
                    //bool isAlarmEvent(!(eventData->isNull(i, ALARMFLAG_COLUMN_NAME) || eventData->getStringData(i, ALARMFLAG_COLUMN_NAME).empty()));
                    // alarm always get printed, if printAlarmEventsOnly flag is set, 
                    // alarm related events will be printed, other events won't be printed
                    bool isAlarmEvent(!(eventData->isNull(i, ALARMFLAG_COLUMN_NAME)) && 
                        ((eventData->getStringData(i, ALARMFLAG_COLUMN_NAME).compare(ALARMEVENT_TYPE) == 0) || 
                        (eventData->getStringData(i, ALARMFLAG_COLUMN_NAME).compare(ALARM_TYPE) == 0)));

                    // If printing alarm events only, some special treatment is required.
                    if (ConfigData::getInstance().printAlarmEventsOnly())
                    {
                        // Skip anything not alarm related, but keep track of progress nonetheless.
                        if (!isAlarmEvent)
                        {
                            currentEventNumber = eventNumber; 
                            continue;
                        }

                        // OK, this must the the first printable event we have encountered in this record set, 
                        // all previous events would have been skipped. Update where we are in processing events 
                        // out of the database before any attempts are made to print.
                        if (printableEvents.empty())
                        {
                            // This also keeps the monitor agent up to date.
                            setLastProcessedEvent(currentEventNumber); 
                        }
                    }

                    // To reduce the traffic going through the print queues, we want to print batches of consecutive events 
                    // that are destined for the same printer. Where the given event's target printer is different, it is
                    // our cue to submit the printable events collected up to that point.
                    std::string printer(eventData->getStringData(i, PRINTER_COLUMN_NAME));

                    if (printer != currentPrinter && !currentPrinter.empty() && !printableEvents.empty())
                    {
                        // Request that the events be printed. Bail out if this fails, we'll try again in the next poll cycle.
                        PrintableData printableData(currentPrinter, printableEvents);
                        if (!printEvents(printableData))
                        {
                            processingEvents = false;
                            break; // get out and let the auto ptr destroy any remaining IData objects
                        }

                        // Update where we are in processing events out of the database. 
                        // This also keeps the monitor agent up to date.
                        setLastProcessedEvent(currentEventNumber); 

                        // Purge the before the next batch of printable events is generated.
                        printableEvents.clear();  
                    }

                    // Extract event data values for the printable columns.
                    // TD14045
                    //const ColumnList& printableColumns(ConfigData::getInstance().getColumns());
                    const ColumnList& printableColumns = ConfigData::getInstance().getColumns();
                    PrintableDataElement pde;

                    for (ColumnList::const_iterator colIt(printableColumns.begin()); colIt != printableColumns.end(); colIt++)
                    {
                        pde.insert(std::make_pair(colIt->name, eventData->getStringData(i, colIt->name)));
                    }

                    // Append the data to the printable event list.
                    printableEvents.push_back(pde);

                    // Update where we are in processing events out of the current record set.
                    currentEventNumber = eventNumber;
                    currentPrinter = printer;
                }

                // Print any remaining events collected from the record set. The list will be empty if 
                // printAlarmEventsOnly is enabled and the record set contained no alarm related events.
                // Either way, the last processed event must be updated.
                if (processingEvents)
                {
                    if (!printableEvents.empty())
                    {
                        // Request that the events be printed. Bail out if this fails, we'll try again in the next poll cycle.
                        PrintableData printableData(currentPrinter, printableEvents);
                        if (!printEvents(printableData))
                        {
                            processingEvents = false;
                            continue;
                        }
                    }                    

                    // Update where we are in processing events out of the database. 
                    // This also keeps the monitor agent up to date.
                    setLastProcessedEvent(currentEventNumber);                     
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
    }    

    FUNCTION_EXIT;
}


bool PrintableEventGenerator::printEvents(const PrintableData& printableData)
{
    FUNCTION_ENTRY("printEvents(const std::string& printer, const PrintableData& printableData)");

    // Last minute check to make sure the agent is still allowed to print.
    if (getOperationMode() != TA_Base_Core::Control)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Gone into MONITOR mode while printing events, stopping immediately.");

        FUNCTION_EXIT;
        return false;
    }

    if (!ConfigData::getInstance().isPrintingEnabled())
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Printing has been disabled while in progress, stopping immediately.");

        FUNCTION_EXIT;
        return false;
    }

    // Request that the events be printed.
    ACE_Future<bool> result(m_printingService.print(printableData));

    // Block until the print job goes out to the print system.
    bool dataPrinted;
    result.get(dataPrinted);

    if (!dataPrinted)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                    "Failed to print on printer %s. Will try again in the next db poll cycle.", printableData.getPrinter().c_str());

        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return true;
}
