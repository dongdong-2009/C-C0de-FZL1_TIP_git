/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Liu Yong
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is a timer for pscada 104 protocol message created via the PScadaUnit. Once the message was not received
  * within the defined time, the relevant datapoint will raise a fieldchange.
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <time.h>
#include "PScadaCommandPolicy.h"
#include "app/scada/PScadaAgent/src/PScadaMsgTimer.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_IRS_App
{
    unsigned int PScadaMsgTimer::PSCADA_PROTOCOL_MESSAGE_TIMEOUT = 5;
	unsigned int PScadaMsgTimer::PSCADA_PROTOCOL_OPERATE_TIMEOUT = 60;

    TA_Base_Core::ReEntrantThreadLockable PScadaMsgTimer::m_lock;
    
	PScadaMsgTimer::PScadaMsgTimer() :
          m_messageTimeout(PScadaMsgTimer::PSCADA_PROTOCOL_MESSAGE_TIMEOUT),
		  m_operateTimeout(PScadaMsgTimer::PSCADA_PROTOCOL_OPERATE_TIMEOUT),
          m_terminated(false)
    {
        FUNCTION_ENTRY("PScadaMsgTimer");
		m_timedMessages.clear();
		//start processor thread
		start();
        FUNCTION_EXIT;
    }


    PScadaMsgTimer::~PScadaMsgTimer()
    {
    	this->terminateAndWait();
    }


    void PScadaMsgTimer::run()
    {
        FUNCTION_ENTRY("run");

        while(!m_terminated)
        {
			{

				TA_Base_Core::ThreadGuard guard(m_lock);

				std::map<unsigned long, PScadaTimerInfo*>::iterator iter = m_timedMessages.begin();
				while(iter != m_timedMessages.end())
				{
					time_t timeNow;
					time(&timeNow);

					time_t timeExpired = iter->second->timeExpired;
					//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "check message %d for data point %s is insert into map. current_time=%d, target_time=%d.",iter->second->controlOperation,iter->second->dataPoint->getDataPointName().c_str(),timeNow,timeExpired);
					if(timeExpired < timeNow)
					{
						// message has expired
	                
						switch (iter->second->controlOperation)
						{
						case TA_Base_Bus::selectCmdConfirmTimeout:
							{
								PScadaCommandPolicy::updateControlState(iter->second->dataPoint, TA_Base_Bus::selectCmdConfirmTimeout, true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation %d Timer expired for datapoint %s ",iter->second->controlOperation, iter->second->dataPoint->getDataPointName().c_str());
							}
							break;
						case TA_Base_Bus::execCmdConfirmTimeout:
							{
								PScadaCommandPolicy::updateControlState(iter->second->dataPoint, TA_Base_Bus::execCmdConfirmTimeout, true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation %d Timer expired for datapoint %s ",iter->second->controlOperation, iter->second->dataPoint->getDataPointName().c_str());
							}
							break;
						case TA_Base_Bus::cancelCmdConfirmTimeout:
							{
								PScadaCommandPolicy::updateControlState(iter->second->dataPoint, TA_Base_Bus::cancelCmdConfirmTimeout, true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation %d Timer expired for datapoint %s ",iter->second->controlOperation, iter->second->dataPoint->getDataPointName().c_str());
							}
							break;
						case TA_Base_Bus::operateCmdTimeout:
							{
								PScadaCommandPolicy::updateControlState(iter->second->dataPoint, TA_Base_Bus::operateCmdTimeout, true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation %d Timer expired for datapoint %s ",iter->second->controlOperation, iter->second->dataPoint->getDataPointName().c_str());
							}
							break;
						default:
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation %d Timer expired for datapoint %s ",iter->second->controlOperation, iter->second->dataPoint->getDataPointName().c_str());
							break;
						}
						
						//removeMessage(iter->second->dataPoint,iter->second->controlOperation);
						delete iter->second;
						m_timedMessages[iter->first] = NULL;

						// Remove the suppression rule from the alarm agent
						m_timedMessages.erase(iter);


						iter = m_timedMessages.begin();
					}    
					else
					{
						++iter;
					}
				}
			}
            int sleepInMillisecs = 200; // wait 200 m second before checking again.
            TA_Base_Core::Thread::sleep(sleepInMillisecs);
        }

		//need to delete all PScadaTimerInfo
		std::map<unsigned long, PScadaTimerInfo*>::iterator iter = m_timedMessages.begin();
		while(iter != m_timedMessages.end())
		{
			delete iter->second;
			m_timedMessages[iter->first] = NULL;
			m_timedMessages.erase(iter++);

		}

        FUNCTION_EXIT;
    }
    

    void PScadaMsgTimer::terminate()
    {
        FUNCTION_ENTRY("terminate");

        m_terminated = true;

        FUNCTION_EXIT;
    }


	void PScadaMsgTimer::addMessage( TA_Base_Bus::DataPoint* dataPoint, TA_Base_Bus::EDataPointControlState controlOperation)
    {
        FUNCTION_ENTRY("addMessage");
		TA_Base_Core::ThreadGuard guard(m_lock);

		// If the m_messageTimeout for this agent is set to zero, there is no expiry
		// ie do not add the message for timing
		if (m_messageTimeout==0) return;

		unsigned long nAddress = dataPoint->getDataPointRegisterAddress();
        std::map<unsigned long, PScadaTimerInfo*>::iterator iter = m_timedMessages.find(nAddress);
        
        if(iter != m_timedMessages.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "message %d for data point %s already exists.",controlOperation,dataPoint->getDataPointName().c_str());
        }
        else
        {

            // Determine the expiry time
            time_t timeNow;
            time(&timeNow);
			time_t expiryTime;
			if (controlOperation == TA_Base_Bus::operateCmdTimeout)
			{
				expiryTime = timeNow + m_operateTimeout;
			}
			else
			{
				expiryTime = timeNow + m_messageTimeout;
			}


			PScadaTimerInfo* timerInfo = new PScadaTimerInfo();
			timerInfo->controlOperation = controlOperation;
			timerInfo->dataPoint = dataPoint;
			timerInfo->timeExpired = expiryTime;

			{
				m_timedMessages.insert(std::map<unsigned long, PScadaTimerInfo*>::value_type(nAddress, timerInfo));
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "message %d for data point %s is insert into map. current_time=%d, target_time=%d.",controlOperation,dataPoint->getDataPointName().c_str(),timeNow,expiryTime);
			}
        }
        
        FUNCTION_EXIT;
    }


	void PScadaMsgTimer::removeMessage( TA_Base_Bus::DataPoint* dataPoint, TA_Base_Bus::EDataPointControlState controlOperation)
    {
        FUNCTION_ENTRY("removeRule");
		TA_Base_Core::ThreadGuard guard(m_lock);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remove message %d for data point %s .",controlOperation,dataPoint->getDataPointName().c_str());

		unsigned long nAddress = dataPoint->getDataPointRegisterAddress();
        std::map<unsigned long, PScadaTimerInfo*>::iterator iter = m_timedMessages.find(nAddress);
        
        if(iter == m_timedMessages.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "message %d for data point %sis not exist.",controlOperation,dataPoint->getDataPointName().c_str());
        }
        else
        {

			delete iter->second;
			m_timedMessages[iter->first] = NULL;

            // Remove the suppression rule from the alarm agent
            m_timedMessages.erase(iter);

		}

        FUNCTION_EXIT;
    }
    
};
