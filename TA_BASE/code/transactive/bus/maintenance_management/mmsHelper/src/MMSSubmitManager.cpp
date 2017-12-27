///////////////////////////////////////////////////////////
//  MMSSubmitManager.cpp
//  Implementation of the Class MMSSubmitManager
//  Created on:      16-¾ÅÔÂ-2011 16:52:28
//  Original author: huirong.luo
///////////////////////////////////////////////////////////

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
#include "MMSSubmitManager.h"
#include "MMSSubmitterDatabase.h"

namespace TA_Base_Bus
{
	MMSSubmitManager* MMSSubmitManager::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable MMSSubmitManager::m_singletonlock;

	MMSSubmitManager* MMSSubmitManager::getInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_singletonlock );

		FUNCTION_ENTRY("getInstance");

		if(0 == m_instance)
		{
			m_instance = new MMSSubmitManager();
		}

		FUNCTION_EXIT;
		return m_instance;
	}


	MMSSubmitManager::MMSSubmitManager()
	{
		m_submitterBase=NULL;
	}


	MMSSubmitManager::~MMSSubmitManager()
	{
		if (m_submitterBase !=NULL)
		{
			delete m_submitterBase;
			m_submitterBase=NULL;
		}
	}

	IMMSSubmitter*  MMSSubmitManager::getSubmitter()
	{
		TA_Base_Core::ThreadGuard guard( m_lockSubmitter);
		if (m_submitterBase ==NULL)
		{
			 
			m_submitterBase= new MMSSubmitterDatabase();
		} 
		return m_submitterBase;
	}

}