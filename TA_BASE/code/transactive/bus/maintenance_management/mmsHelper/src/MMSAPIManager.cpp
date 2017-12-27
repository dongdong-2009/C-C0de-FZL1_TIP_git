///////////////////////////////////////////////////////////
//  MMSAPIManager.cpp
//  Implementation of the Class MMSAPIManager
//  Created on:      16-¾ÅÔÂ-2011 16:52:28
//  Original author: huirong.luo
///////////////////////////////////////////////////////////

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/utilities/src/DebugUtil.h"
#include "MMSUtility.h"
#include "MMSAPIManager.h"
#include "MMSSubmitterDatabase.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	MMSAPIManager* MMSAPIManager::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable MMSAPIManager::m_singletonlock;

	MMSAPIManager* MMSAPIManager::getInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_singletonlock );
		//FUNCTION_ENTRY("getInstance");
		if(0 == m_instance)
		{
			m_instance = new MMSAPIManager();
		}

		//FUNCTION_EXIT;
		return m_instance;
	}

	void MMSAPIManager::releaseInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_singletonlock );
		FUNCTION_ENTRY("releaseInstance");
		if(0 != m_instance)
		{
			delete m_instance;
			m_instance=NULL;
		}

		FUNCTION_EXIT;
		 
	}



	MMSAPIManager::MMSAPIManager():m_isInit(false),m_MMSAPIImplementor(NULL)
	{
		 
	}


	MMSAPIManager::~MMSAPIManager()
	{
		if (m_MMSAPIImplementor !=NULL)
		{
			delete m_MMSAPIImplementor;
			m_MMSAPIImplementor=NULL;
		}
	}

	void MMSAPIManager::initMMSAPI(IMMSAPI* apiImplementor,TA_Base_Core::MmsAgentEntityDataPtr pData)
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (apiImplementor==NULL) return;
		if (m_MMSAPIImplementor !=apiImplementor)
		{
			delete m_MMSAPIImplementor;
			m_MMSAPIImplementor=NULL;
		}
		m_MMSAPIImplementor=apiImplementor;
		m_MMSAPIImplementor->initConfiguration(pData);
		m_isInit=true;
	}

	bool MMSAPIManager::isInitialized()
	{		 
		if (m_MMSAPIImplementor ==NULL || m_isInit==false)
		{
			return false;			 
		} 
		return true;
	}

	bool  MMSAPIManager::submitAutoAlarmMessage(const TA_Base_Core::AlarmDetailCorbaDef &alarm)
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return false;
		//translate AlarmDetailCorbaDef into MmsRequestAlarmData
		TA_Base_Core::MmsRequestAlarmData* pmmsReqestData= NULL;
		pmmsReqestData= MMSUtility::getMmsRequestAlarmData(alarm);
		std::auto_ptr<TA_Base_Core::MmsRequestAlarmData> mmsReqestData(pmmsReqestData);
		return m_MMSAPIImplementor->submitAutoAlarmMessage(*(mmsReqestData.get()));

	}


	bool MMSAPIManager::submitJobRequestMessage(const TA_Base_Bus::JobRequestDetailCorbaDef& details)
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return false;
		return m_MMSAPIImplementor->submitJobRequestMessage(details);
		 	 
	}

	bool MMSAPIManager::submitScheduledMessage(MMSScheduledMessage& msg)
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return false;
		return m_MMSAPIImplementor->submitScheduledMessage(msg);
	}

	bool MMSAPIManager::isMMSLive()
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return false;
		return m_MMSAPIImplementor->isMMSLive();
	}


	void MMSAPIManager::setToControlMode()
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return;
		return m_MMSAPIImplementor->setToControlMode();
	}

	void MMSAPIManager::setToMonitorMode()
	{
		TA_Base_Core::ThreadGuard guard( m_lockImplementor);
		if (isInitialized()==false) return;
		return m_MMSAPIImplementor->setToMonitorMode();
	}

}