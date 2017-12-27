/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <time.h>

#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"
#include "app/alarm/alarm_agent/src/PrintQueue.h"
#include "app/alarm/alarm_agent/src/UnixLinePrinter.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/AlarmStateUpdate_MessageTypes.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/alarm/alarm_common/src/AlarmConstants.h"

#include "ace/OS.h"

using TA_Base_Core::AlarmMessageCorbaDef;
using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::MessagePublicationManager;
using TA_Base_Core::AlarmAgentQueueItemCorbaDef;



namespace TA_Base_App
{

    //
    // Constructor
    //
    PrintQueue::PrintQueue()
    : TransactionQueue()
    {
        m_stateSender = MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                            TA_Base_Core::AlarmStateUpdate::Context);

        m_printer = new UnixLinePrinter( RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::PRINTER_NAME.c_str()),
                                         RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::PRINTER_PORT.c_str()),
                                         RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::PRINTER_HOST.c_str()) );
    }


    //
    // Destructor
    //
    PrintQueue::~PrintQueue()
    {
        clear();

        if( m_stateSender != NULL )
        {
            delete m_stateSender;
            m_stateSender = 0;
        }

        if( m_printer != NULL )
        {
            delete m_printer;
            m_printer = 0;
        }
    }


    //
    // run
    //
    void PrintQueue::run()
    {
		AlarmQueueWrapper* wrapper = NULL;
        while( true )
        {
            m_queueSemaphore.wait();

            if( m_terminated )
            {
                break;
            }
            std::string msgID = "";  //for log only
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {

                UpdateQueue& updateQueue = getQueue();
                try
                {
                    wrapper = NULL;						
					wrapper = popTop();
					//get the id in case we got an exception for logging
					msgID = wrapper->getAlarmAgentQueueItem()->theMessage.alarmID.in();
					
					std::auto_ptr<AlarmQueueWrapper> newWrapper(wrapper);
					
					handleNewQueueItem(newWrapper.get());

                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
							"Queue size is %d", size());
                }
                catch( ... )
                {
                     
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                         "Alarm printing failed. Removing queue item" );
                 
                }                    

            }
        }
    }


    //
    // terminate
    //
    void PrintQueue::terminate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "terminate");

        ThreadGuard guard(m_lock);

        m_terminated = true;

        m_queueSemaphore.post();
    }


    //
    // handleNewQueueItem
    //
    void PrintQueue::handleNewQueueItem( AlarmQueueWrapper* pWrapper )
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "handleNewQueueItem");

        CORBA::Any stateUpdate;

        stateUpdate <<=  pWrapper->getAlarmAgentQueueItem();

        if( pWrapper->getAlarmAgentQueueItem()->containsAlarmDetailForPrintingCorbaDef )
        {
            printAlarm( pWrapper->getAlarmAgentQueueItem()->theMessage );

            m_stateSender->sendStateUpdateMessage(
                TA_Base_Core::AlarmStateUpdate::QueueItemRemoved,
                0,
                RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(), stateUpdate);
        }

        //delete pWrapper;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "handleNewQueueItem");
    }


    //
    // removeQueueItem
    //
    void PrintQueue::removeQueueItem(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item)
    {
        if(0 != RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                 "Received a removeQueueItem request while in control mode. Ignoring." );
            return;
        }

        ThreadGuard guard(m_lock);

        UpdateQueue& updateQueue = getQueue();

        UpdateQueue::iterator p;
        UpdateQueue::iterator begin = updateQueue.begin();
        UpdateQueue::iterator end = updateQueue.end();

        for( p = begin; p != end; p++ )
        {
            if( (*p)->getAlarmAgentQueueItem()->containsAlarmDetailCorbaDef == p_item.containsAlarmDetailCorbaDef )
            {
                if( p_item.containsAlarmDetailCorbaDef )
                {
                    if( 0 == std::string(p_item.theMessage.alarmID).compare((*p)->getAlarmAgentQueueItem()->theMessage.alarmID) )
                    {
                        // This is it
                        delete *p;
                        updateQueue.erase(p);
                        break;
                    }
                }
                else
                {
                    if( 0 == std::string(p_item.theUpdate.alarmID).compare((*p)->getAlarmAgentQueueItem()->theUpdate.alarmID) )
                    {
                        if( p_item.theUpdate.alarmHasBeenAcknowledged == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenAcknowledged )
                        {
                            if( p_item.theUpdate.alarmHasBeenClosed == (*p)->getAlarmAgentQueueItem()->theUpdate.alarmHasBeenClosed )
                            {
                                if( p_item.theUpdate.decisionSupportStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.decisionSupportStateHasChanged )
                                {
									if (p_item.theUpdate.mmsStateHasChanged == (*p)->getAlarmAgentQueueItem()->theUpdate.mmsStateHasChanged )
									{
										// This is it
										delete *p;
										updateQueue.erase(p);
										break;
									}
                                }
                            }
                        }
                    }
                }
            }
        }
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Queue size is %d", updateQueue.size());
    }


    //
    // printAlarm
    //
    void PrintQueue::printAlarm( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "printAlarm");

        std::string lineToPrint;
		long date;

		if (TA_Base_Core::RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_USESOURCETIME.c_str()))
		{
			date = (long)p_alarm.sourceTime.time;
		}
		else
		{
			date = (long)p_alarm.displayTime.time;
		}

        struct tm t;
        ACE_OS::localtime_r(&date, &t);

        char dateString[25];
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_USESOURCETIME.c_str()))
		{
			sprintf(dateString, "%d/%d/%d %d:%d:%d.%d", 
				t.tm_mday, (t.tm_mon + 1), (t.tm_year + 1900), t.tm_hour, t.tm_min, t.tm_sec, p_alarm.sourceTime.millitm);
		}
		else
		{
			sprintf(dateString, "%d/%d/%d %d:%d:%d.%d", 
				t.tm_mday, (t.tm_mon + 1), (t.tm_year + 1900), t.tm_hour, t.tm_min, t.tm_sec, p_alarm.displayTime.millitm);
		}

        lineToPrint= dateString;
        lineToPrint+= " : ";
        lineToPrint+= std::string(p_alarm.entityName);
        lineToPrint+= " : ";
        lineToPrint+= std::string(p_alarm.alarmDescription);
        lineToPrint+= "\n";

        //m_printer->print(lineToPrint);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "printAlarm");
    }
};
