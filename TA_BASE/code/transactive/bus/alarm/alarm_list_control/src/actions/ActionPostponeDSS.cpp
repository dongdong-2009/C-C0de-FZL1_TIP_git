/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionPostponeDSS.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that postpones the running of a decision support plan
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionPostponeDSS.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/data_access_interface/plans/src/AlarmPlanAssocData.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionPostponeDSS::ActionPostponeDSS(bool isAvailableInContextMenu /*=true*/)
        :  m_isAvailableInContextMenu(isAvailableInContextMenu),
           m_planManagerKey(0),
		   m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper())
    {
        
    }

    
    ActionPostponeDSS::~ActionPostponeDSS()
    {
        try
        {
            TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
            TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().cleanUp();
        }
        catch( ... )
        {
        }
    }


    bool ActionPostponeDSS::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return false;
        }
	
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
		//ATS Alarm check
		if (alarmRetriever.getNumberAttr(selectedAlarms[0], IS_EXT_ALARM))
		{
			alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL) return false;

			FUNCTION_EXIT;
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), POSTPONE_DSS);
		}

        // Before we can check if this alarm has a related plan, 
        // need to work out if the user can acutally use the plan manager.
        // Now determine if we have the rights to do this
        if (alarmRetriever.getRightsChecker().canPerformAction(getPlanManagerKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        {

            // Get the alarm details
            try
            {
				alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
				if (alarm.get() == NULL) return false;
				
                // The messageTypeKey member actually holds the alarm type name
				// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
				/*
                std::string alarmTypeKey = alarmRetriever.getDatabaseCache().getAlarmTypeKey(alarm->messageTypeKey.in());
                if (alarmTypeKey.empty())
                {
                    delete alarm;
                    return false;
                }
				*/
                bool doesAssocExist = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().associationExists(
                                                                                             alarm->assocEntityKey,
                                                                                             alarm->assocEntityTypeKey,
																							 alarm->messageTypeKey);

                // Finally, check that the plan has not already been run.
				//TD16116++
                doesAssocExist = doesAssocExist && 
						((strcmp(alarm->decisionSupportState.c_str(), "X") != 0) && 
						(strcmp(alarm->decisionSupportState.c_str(), "R") != 0));
				//	((alarm->decisionSupportState != TA_Base_Core::RelatedPlanHasBeenPostponed) && 
				//	 (alarm->decisionSupportState != TA_Base_Core::RelatedPlanHasBeenRun));
				//++TD16116

                // NOTE: Cannot use "decisionSupportState" to determine if an alarm has an associated plan, because
                // the Alarm Library and Alarm Agent do NOT set this informaiton - they rely on the Alarm Manager 
                // to do so!

               // delete alarm;

                FUNCTION_EXIT;
                return doesAssocExist;
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine plan associations");
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the alarm plan associations from the database");
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the alarm plan associations from the database");
            }

        }

        FUNCTION_EXIT;
		return false;
    }


    void ActionPostponeDSS::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            FUNCTION_EXIT;
            return;
        }
        alarmRetriever.setIsActionAlreadyRunning(true, "Acknowledging alarms . . .");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            alarmRetriever.setIsActionAlreadyRunning(false);
            return;
        }

        // First retrieve the entity details from the alarm
        TA_Base_Core::PlanDetailsList planList;
        try
        {
			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(alarmRetriever.getAlarmItem(selectedAlarms[0]));
            
	        unsigned long entityKey = alarm->assocEntityKey;
	        unsigned long entityTypeKey = alarm->assocEntityTypeKey;
            // The messageTypeKey member actually holds the alarm type name
			// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
            // std::string alarmTypeKey = alarmRetriever.getDatabaseCache().getAlarmTypeKey(alarm->messageTypeKey.in());

		    planList = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().getAssociatedPlanDetails(entityKey,
                                                                                                    entityTypeKey,
																									alarm->messageTypeKey);

        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine plan associations");

            alarmRetriever.setIsActionAlreadyRunning(false);
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the alarm plan associations from the database");

            alarmRetriever.setIsActionAlreadyRunning(false);
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the alarm plan associations from the database");

            alarmRetriever.setIsActionAlreadyRunning(false);
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }

        bool wasAcked = true;
		std::vector<TA_Base_Core::AlarmModifyItem*> alarmAckList;
		TA_Base_Core::AlarmModifyItem* pAlarmItem = NULL;
        try
        {
			// Put the alarm into the vector - this is needed for the following call to acknowledge alarms
			pAlarmItem = alarmRetriever.getAlarmModifyItem(selectedAlarms[0]);
            alarmAckList.push_back(pAlarmItem);

            // And now acknowledge the alarm
			//m_alarmHelper.operatorAcknowledgeAlarms(alarmAckList);
			std::vector<std::string>  alarmNoExists;
			bool wasSuccessful=!m_alarmHelper.operatorAcknowledgeAlarmsEx(alarmAckList,alarmNoExists);
          
        }
        catch(...)
        {
            wasAcked = false;
        }

        try
        {
            if (pAlarmItem != NULL)
            {
                m_alarmHelper.postponePlanExecution(pAlarmItem);
            }
            else
            {
				alarmRetriever.freeAlarmModifyItems(alarmAckList);
                alarmRetriever.setIsActionAlreadyRunning(false);
                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_POSTPONE_PLAN) );
            }
        }
        catch( const TA_Base_Core::NoAlarmAgentException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", "Could not set that the plan was postponed on the agent");

			alarmRetriever.freeAlarmModifyItems(alarmAckList);
            alarmRetriever.setIsActionAlreadyRunning(false);
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_POSTPONE_PLAN) );
        }

        if(!wasAcked)
        {
			alarmRetriever.freeAlarmModifyItems(alarmAckList);
            alarmRetriever.setIsActionAlreadyRunning(false);
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARM) );
        }

		alarmRetriever.freeAlarmModifyItems(alarmAckList);
        alarmRetriever.setIsActionAlreadyRunning(false);
        FUNCTION_EXIT;
    }
	
	void ActionPostponeDSS::preformPreAction()
	{
		getPlanManagerKey();
	}
	
    unsigned long ActionPostponeDSS::getPlanManagerKey()
    {
        FUNCTION_ENTRY("getPlanManagerKey");

        if (0 == m_planManagerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList planManagers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::PlanManagerEntityData::getStaticType(),0);//for fast query who knows the reason

                if (1 == planManagers.size())
                {
                    m_planManagerKey = planManagers[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu plan managers entities in the database. There should be one only.",
                        planManagers.size());
                    if ( planManagers.size() > 1)
                    {
                        // use the first one
                        m_planManagerKey = planManagers[0]->getKey();
                    }
                }

                // free the entity data
                while (! planManagers.empty())
                {
                    delete planManagers.back();
                    planManagers.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Plan Managers entity key");
            }
        }

        FUNCTION_EXIT;
        return m_planManagerKey;
    }
}
