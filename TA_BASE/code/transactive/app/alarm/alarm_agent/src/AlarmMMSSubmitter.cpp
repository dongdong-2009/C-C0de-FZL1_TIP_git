/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmMMSSubmitter.cpp $
* @author:
* @version: $Revision:  $
*
* Last modification: $DateTime: $
* Last modified by:  $Author:  $
*
*/

#ifdef _MSC_VER
#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER


#include "app/alarm/alarm_agent/src/AlarmMMSSubmitter.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"

#include "bus/maintenance_management/mmsHelper/src/MMSSubmitManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/exceptions/src/DbConnectionFailed.h"

using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	AlarmMMSSubmitter* AlarmMMSSubmitter::m_submitterInstance = 0;
    TA_Base_Core::NonReEntrantThreadLockable AlarmMMSSubmitter::m_singletonLock;

   
    //
    // getInstance
    //
    AlarmMMSSubmitter* AlarmMMSSubmitter::getInstance()
    {
        ThreadGuard guard(m_singletonLock);
        
        if( m_submitterInstance == 0 )
        {
            m_submitterInstance = new AlarmMMSSubmitter();
        }
        return m_submitterInstance;
    }


	//
    // ReleaseInstance
    //
    void AlarmMMSSubmitter::releaseInstance()
    {
        ThreadGuard guard(m_singletonLock);
        
        if( m_submitterInstance != 0 )
        {
            delete m_submitterInstance ;
        }
        m_submitterInstance=NULL;
    }
	//
	// Constructor
	//
	AlarmMMSSubmitter::AlarmMMSSubmitter()
		:m_terminated(false), m_queueSemaphore(0)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::AlarmMMSSubmitter");
		start();
		FUNCTION_EXIT;
	}


	//
	// Destructor
	//
	AlarmMMSSubmitter::~AlarmMMSSubmitter()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::~AlarmMMSSubmitter");
		clear();
		FUNCTION_EXIT;
	}


	//
	// run
	//
	void AlarmMMSSubmitter::run()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::run");
		AlarmDetailCorbaDef* pAlarm = NULL;

		while( true )
		{
			m_queueSemaphore.wait();

			if( m_terminated )
			{
				break;
			}
			MoveToInternalQueue();
			if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
			{
				try
				{
					while( (!isInternalQueueEmpty()) && (0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL)) && !m_terminated )
					{
						pAlarm = NULL;
						pAlarm = popTopInternalQueue();

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Submitting mms alarm message for alarm %s",
							pAlarm->alarmID.in());

						bool bReturn = TA_Base_Bus::MMSSubmitManager::getInstance()->getSubmitter()->submitAutoAlarmMessage(*pAlarm);

						//based on return value retry or abort - current implementation always true.

						//send updateMMSState for updating the database and Monitor agent to remove from quene
						//send Alarm details inorder update state for ack and normalised alarm
						AlarmCache::getInstance()->updateAlarmMMSStateToSend(pAlarm);

						delete pAlarm;
						pAlarm=NULL;

					}
				}
				catch(TA_Base_Core::DbConnectionFailed& ex)
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to save MMS Service request into the Database" );

					LOG( SourceInfo, DebugUtil::ExceptionCatch,"DbConnectionFailed", 
						ex.what() );

					//push back the item to quene
					pushBackInternalQueue(pAlarm);

				}
				catch(TA_Base_Core::DatabaseException& ex)
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to save MMS Service request into the Database" );

					LOG( SourceInfo, DebugUtil::ExceptionCatch,"DatabaseException", 
						ex.what() );

					//push back the item 
					pushBackInternalQueue(pAlarm);

				}
				catch(...) //unknown exception
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError," Unknown excpetion while saving MMS Service request into the Database");					

					//push back the item 
					pushBackInternalQueue(pAlarm);
				}
			}
		}
		FUNCTION_EXIT;

	}



	//
	// terminate
	//
	void AlarmMMSSubmitter::terminate()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::terminate");
		
		ThreadGuard guard(m_lock);

		m_terminated = true;

		m_queueSemaphore.post();
		FUNCTION_EXIT;
	}

	//
	// clear
	//
	void AlarmMMSSubmitter::clear()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::clear");
		std::deque<AlarmDetailCorbaDef*>::iterator p;
		{
			ThreadGuard guard(m_lock);

			for( p = m_alarmMMSQuene.begin(); p != m_alarmMMSQuene.end(); p++ )
			{
				if( *p != NULL )
				{
					delete *p;
				}
			}
			m_alarmMMSQuene.clear();

		}
		{

			ThreadGuard guardInternal(m_internalLock);
			for( p = m_alarmMMSInternalQuene.begin(); p != m_alarmMMSInternalQuene.end(); p++ )
			{
				if( *p != NULL )
				{
					delete *p;
				}
			}
			m_alarmMMSInternalQuene.clear();

		}
		FUNCTION_EXIT;
	}

	void AlarmMMSSubmitter::push(AlarmDetailCorbaDef* pAlarm )
	{
		FUNCTION_ENTRY("push");

		if( !m_terminated && !findAlarm(std::string(pAlarm->alarmID)) )
		{
			ThreadGuard guard(m_lock);
			m_alarmMMSQuene.push_back( pAlarm );
			// to be removed
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, " AlarmMMSSubmitter::push - received alarm id - %s and size of quene is %d",pAlarm->alarmID.in(),m_alarmMMSQuene.size());
			m_queueSemaphore.post();
		}
		else
		{
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, " alarm id - %s is already there in the MMS Submitter's Quene",pAlarm->alarmID.in());

			//delete the meaage detail copy
			delete pAlarm;
			pAlarm = NULL;
		}
		FUNCTION_EXIT;
	}

	/*void AlarmMMSSubmitter::push( std::deque<AlarmDetailCorbaDef*> alarmList )
	{
	FUNCTION_ENTRY("push");

	ThreadGuard guard(m_lock);

	if( !m_terminated )
	{
	std::deque<AlarmDetailCorbaDef*>::iterator p;
	for( p = alarmList.begin(); p != alarmList.end(); p++ )
	{
	m_alarmMMSQuene.push_back( *p );
	}

	m_queueSemaphore.post();
	}
	FUNCTION_EXIT;
	}*/


	void AlarmMMSSubmitter::MoveToInternalQueue()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::MoveToInternalQueue");
		ThreadGuard guard(m_lock);
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"MMS Queue size is %d", m_alarmMMSQuene.size());

		if (!m_alarmMMSQuene.empty())
		{
			ThreadGuard guardInternal(m_internalLock);
			m_alarmMMSInternalQuene.insert(m_alarmMMSInternalQuene.end(), m_alarmMMSQuene.begin(), m_alarmMMSQuene.end());
			m_alarmMMSQuene.clear();
			// to be removed
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, " AlarmMMSSubmitter::MoveToInternalQueue - size of internalQuene is %d",m_alarmMMSInternalQuene.size());		
		}

		FUNCTION_EXIT;
	}


	AlarmDetailCorbaDef* AlarmMMSSubmitter::popTopInternalQueue()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::popTopInternalQueue");
		AlarmDetailCorbaDef *pAlarm = NULL;
		ThreadGuard guard(m_internalLock);
		pAlarm = m_alarmMMSInternalQuene.front();
		m_alarmMMSInternalQuene.pop_front();
		FUNCTION_EXIT;
		return pAlarm;
	}


	bool AlarmMMSSubmitter::isInternalQueueEmpty()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::isInternalQueueEmpty");
		ThreadGuard guard(m_internalLock);
		FUNCTION_EXIT;
		return m_alarmMMSInternalQuene.empty();

	}

	/*void AlarmMMSSubmitter::remove(TA_Base_Core::AlarmDetailCorbaDef& p_item)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::remove");
		if(0 != RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Received a removeMMSQueueItem request while in control mode. Ignoring." );
			return;
		}

		remove(std::string(p_item.alarmID));

		FUNCTION_EXIT;

	}*/

	void AlarmMMSSubmitter::remove(std::string messageId)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::removeString");
		if(0 != RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Received a removeMMSQueueItem request while in control mode. Ignoring." );
			return;
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"removeMMSQueueItem" );

		//find mms alarm in quene and erase
		bool bRet = false;
		{
			ThreadGuard guard(m_lock);
			if(m_alarmMMSQuene.size() != 0)
				bRet = removeQueueItem(messageId,&m_alarmMMSQuene);
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"removeMMSQueueItem bret = %d" , bRet);

		if (!bRet)
		{
			//not found in client's quene, so search in internal quene
			ThreadGuard guard(m_internalLock);
			if(m_alarmMMSInternalQuene.size() != 0)
				removeQueueItem(messageId,&m_alarmMMSInternalQuene);
		}

		FUNCTION_EXIT;
	}

	bool AlarmMMSSubmitter::removeQueueItem(std::string messageId, std::deque<TA_Base_Core::AlarmDetailCorbaDef*>* quene)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::removeQueueItem");
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"MMS Queue size is %d before erase", quene->size());
		bool bFound = false;
		std::deque<AlarmDetailCorbaDef*>::iterator p;
		for( p = quene->begin(); p != quene->end(); p++ )
		{
			if(0 == messageId.compare((*p)->alarmID))
			{
				delete *p;
				quene->erase(p);
				bFound = true;
				break;
			}
		}
		if (bFound)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
				"MMS Queue size is %d after erase", quene->size());
		}
		else
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
				"alarm id %s was not found in quene",messageId.c_str());
		}
		FUNCTION_EXIT;
		return bFound;
	}	

	
	void AlarmMMSSubmitter::pushTopInternalQueue(TA_Base_Core::AlarmDetailCorbaDef* pAlarm )
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::pushTopInternalQueue");
		if (pAlarm==NULL) return;
		ThreadGuard guard(m_internalLock);		
		m_alarmMMSInternalQuene.push_front(pAlarm);
		FUNCTION_EXIT;
	}

	void AlarmMMSSubmitter::pushBackInternalQueue(TA_Base_Core::AlarmDetailCorbaDef* pAlarm )
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::pushBackInternalQueue");
		if (pAlarm==NULL) return;
		ThreadGuard guard(m_internalLock);		
		m_alarmMMSInternalQuene.push_back(pAlarm);
		FUNCTION_EXIT;
	}

	bool AlarmMMSSubmitter::findAlarm(std::string alarmID)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::findAlarm");
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			"findAlarm ALARM ID = %s",alarmID.c_str());

		//find mms alarm in quene
		bool bRet = false;
		{
			ThreadGuard guard(m_lock);
			if(m_alarmMMSQuene.size() != 0)
				bRet = findQueneItem(alarmID,&m_alarmMMSQuene);
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"findAlarm bret = %d" , bRet);

		if (!bRet)
		{
			//not found in client's quene, so search in internal quene
			ThreadGuard guard(m_internalLock);
			if(m_alarmMMSInternalQuene.size() != 0)
				bRet = findQueneItem(alarmID,&m_alarmMMSInternalQuene);
		}
		FUNCTION_EXIT;
		return bRet;
	}

	bool AlarmMMSSubmitter::findQueneItem(std::string messageId, std::deque<TA_Base_Core::AlarmDetailCorbaDef*>* quene)
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::findQueueItem");
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"MMS Queue size is %d ", quene->size());
		bool bFound = false;
		std::deque<AlarmDetailCorbaDef*>::iterator p;
		for( p = quene->begin(); p != quene->end(); p++ )
		{
			if(0 == messageId.compare((*p)->alarmID))
			{
				bFound = true;
				break;
			}
		}		
		FUNCTION_EXIT;
		return bFound;
	}	

	void AlarmMMSSubmitter::startProcessingMMS()
	{
		FUNCTION_ENTRY("AlarmMMSSubmitter::startProcessingMMS");
		m_queueSemaphore.post();
		FUNCTION_EXIT;
	}

};