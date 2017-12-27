/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/AccessControlHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  *
  */
#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


using namespace TA_Base_Core;
using namespace TA_Base_Bus;

static const long TRY_CONNECT_RIGHT_AGENT_TIME = 500;

namespace TA_Base_App
{
	AccessControlHelper* AccessControlHelper::m_singleton = NULL;

	AccessControlHelper::~AccessControlHelper()
	{
		TA_THREADGUARD(m_entitySubsystemActionLock);
		m_EntityInfoVector.clear();
		m_entitySubsystemMap.clear();
		m_subsystemActionsMap.clear();
		m_ActionsList.clear();

		if(m_pRightsMgr) delete m_pRightsMgr;
	}

	AccessControlHelper::AccessControlHelper()
	{ 
		// should only be done once
		InitActionsMap(); 
	}

	AccessControlHelper* AccessControlHelper::getInstance()
	{
		if (m_singleton == NULL)
		{
			m_singleton = new AccessControlHelper;
		}

		return m_singleton;
	};

	// Initialises the access control map 
	void AccessControlHelper::InitAccessControlMap(RightsLibrary* rightsMgr,
													std::string aSessionId, 													
													std::vector<AccessEntityInfo> entityInfo)
	{
		m_pRightsMgr		= rightsMgr;
		m_SessionId			= aSessionId.c_str();

		{
			TA_THREADGUARD(m_entitySubsystemActionLock);
			m_EntityInfoVector	= entityInfo;
		}	

		AssortSubsystem();

		if (m_isConnectingAgent == false)
		{			
			start();
		}
	}

	void AccessControlHelper::AssortSubsystem()
	{
		TA_THREADGUARD(m_entitySubsystemActionLock);

		std::vector<AccessEntityInfo>::iterator entityInfoVectorIt ;
		for(entityInfoVectorIt = m_EntityInfoVector.begin();entityInfoVectorIt != m_EntityInfoVector.end();entityInfoVectorIt++)
		{
			m_entitySubsystemMap.insert(std::make_pair(entityInfoVectorIt->entityKey,entityInfoVectorIt->subSystemKey));

			if (m_subsystemActionsMap.find(entityInfoVectorIt->subSystemKey) == m_subsystemActionsMap.end())
			{
				std::set<unsigned long> tNew;
				tNew.clear();
				m_subsystemActionsMap.insert(std::make_pair(entityInfoVectorIt->subSystemKey,tNew));
			}			
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"m_subsystemActionsMap.size :%d", m_subsystemActionsMap.size());
	}

	void AccessControlHelper::InitEntitiesRights()
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;


		//TA_THREADGUARD(m_entitySubsystemActionLock);
		SubsystemActionsMap::iterator subsystemActionMapIt;
		for(subsystemActionMapIt = m_subsystemActionsMap.begin(); subsystemActionMapIt != m_subsystemActionsMap.end(); subsystemActionMapIt++)
		{
			std::list<unsigned long> t_actionList ,t_getActionList;
			t_actionList = m_ActionsList;

			bool bFind = false;
			AccessEntityInfo aei;
			{
				TA_THREADGUARD(m_entitySubsystemActionLock);
				std::vector<AccessEntityInfo>::iterator iter = m_EntityInfoVector.begin();
				for (iter = m_EntityInfoVector.begin();iter != m_EntityInfoVector.end();iter++)
				{
					if (iter->subSystemKey == subsystemActionMapIt->first )
					{
						aei = *iter;
						bFind = true;
						break;
					}
				}
			}
			if ( false == bFind) continue;

			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"try to get rights from RightsAgent. subSystemKey: %d.size of getActionList:%d", aei.subSystemKey,t_getActionList.size());
				m_pRightsMgr->areActionsPermittedOnSubsystemAndLocSub(
					m_SessionId.c_str(), 
					aei.entityKey, 
					aei.locationKey,
					aei.subSystemKey,
					t_actionList, 
					t_getActionList,
					reason, 
					decisionModule);	
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"GotActionList from RightsAgent. subSystemKey: %d.size of getActionList:%d", aei.subSystemKey,t_getActionList.size());
			}
			catch (const TA_Base_Core::RightsException& ex)
			{							
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Cannot get Rights From RihgtsAgent.TA_Base_Core::RightsException:%s", ex.what());
				t_getActionList.clear();
				throw ( TA_Base_Core::RightsException(ex.what()) );
			} 
			catch (...)
			{	
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Cannot get Rights From RihgtsAgent.Unkown!");
				t_getActionList.clear();
			}	

			subsystemActionMapIt->second.clear();
			std::list<unsigned long>::iterator listIt = t_getActionList.begin();
			
			{
				TA_THREADGUARD(m_entitySubsystemActionLock);
				for (; listIt != t_getActionList.end(); listIt++)
				{
					subsystemActionMapIt->second.insert(*listIt);
				}
			}
		}


		CWinApp *app = AfxGetApp();
		if (app != NULL && app->m_pMainWnd != NULL)
		{
			app->m_pMainWnd->PostMessage(UPDATE_PERMISSION, 1, 0);
		}
	}

	// Returns a bool specifying if action should be permitted or not
	//
	bool AccessControlHelper::IsActionPermitted(unsigned long aEntityKey, AccessControlActions aAccessControlType)
	{
		TA_THREADGUARD(m_entitySubsystemActionLock);

		std::map<unsigned long, unsigned long >::iterator entitySubsystemIt = m_entitySubsystemMap.find(aEntityKey);
		if (entitySubsystemIt == m_entitySubsystemMap.end()) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"permit false"); 		
			return false;
		}
		SubsystemActionsMap::iterator subsystemActionsIt = m_subsystemActionsMap.find(entitySubsystemIt->second);
		if (subsystemActionsIt == m_subsystemActionsMap.end()) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"permit false"); 
			return false;
		}
		std::set<unsigned long>::iterator actionsSetIt = subsystemActionsIt->second.find(aAccessControlType);
		if ( actionsSetIt == subsystemActionsIt->second.end()) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"permit false"); 
			return false;
		}
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"permit true"); 
		return true;		
	}


	// Function maps an internal action to the actual action id stored in the database
	//
	void AccessControlHelper::InitActionsMap()
	{
		m_isConnectingAgent	= false;
		m_pRightsMgr		= NULL;
		m_SessionId			= "";

		TA_THREADGUARD(m_entitySubsystemActionLock);
		m_EntityInfoVector.clear();

		
		m_ActionsList.clear();
		m_ActionsList.push_back(DATA_POINT_WRITE);
		m_ActionsList.push_back(DATA_POINT_ALARM_INHIBIT);
		m_ActionsList.push_back(DATA_POINT_ALARM_UNINHIBIT);
		m_ActionsList.push_back(DATA_POINT_CONTROL_INHIBIT);
		m_ActionsList.push_back(DATA_POINT_CONTROL_UNINHIBIT);
		m_ActionsList.push_back(DATA_POINT_SCAN_UNINHIBIT);
		m_ActionsList.push_back(DATA_POINT_SCAN_INHIBIT);
		m_ActionsList.push_back(DATA_POINT_MMS_INHIBIT);
		m_ActionsList.push_back(DATA_POINT_MMS_UNINHIBIT);
		m_ActionsList.push_back(DATA_POINT_SET_FORCED_STATE);
		m_ActionsList.push_back(DATA_POINT_RESET_FORCED_STATE);
		m_ActionsList.push_back(DATA_POINT_METER_ADJUST);
		m_ActionsList.push_back(DATA_NODE_TAG);
		m_ActionsList.push_back(DATA_NODE_NOTES);
		m_ActionsList.push_back(DATA_NODE_TAG_PTW);
		m_ActionsList.push_back(IP_LIMITS_TAB_ACCESS);
	}

	void AccessControlHelper::run()
	{
		m_isConnectingAgent 	= true;
		while(	m_isConnectingAgent)
		{
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "try to get entities rights");
				InitEntitiesRights();
				m_isConnectingAgent = false;				
			}
			catch (const TA_Base_Core::RightsException& ex)
			{				
				//sleep(TRY_CONNECT_RIGHT_AGENT_TIME);
	
				for(int index =0;index <  10; index++)
				{
					if(!m_isConnectingAgent)
					{
						break;
					}
					else
					{
						m_condition.timedWait(TRY_CONNECT_RIGHT_AGENT_TIME);
					}
				}
				
			}
		}
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Thread exit");
	}
	void AccessControlHelper::terminate()
	{
		m_isConnectingAgent = false;
		m_condition.signal();
	}
} // namespace