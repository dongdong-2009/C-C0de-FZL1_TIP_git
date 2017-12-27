/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedDSS.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * This class is for the action that allows a related decision support plan to be run.
 */

#pragma warning(disable:4786)

#include <sstream>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedDSS.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/PlanAssociationDialog.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionRelatedDSS::ActionRelatedDSS(bool isAvailableInContextMenu /*=true*/)
        : m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
          m_isAvailableInContextMenu(isAvailableInContextMenu),
          m_planManagerKey(0)
    {

    }

    
    ActionRelatedDSS::~ActionRelatedDSS()
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


    bool ActionRelatedDSS::isAvailable(IAlarmRetriever& alarmRetriever)
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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_DSS);
		}

        // Now determine if we have the rights to do this
        if (alarmRetriever.getRightsChecker().canPerformAction(getPlanManagerKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        {
            // First retrieve the entity details from the alarm
			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
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

               // delete alarm;
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


    void ActionRelatedDSS::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity details from the alarm
        TA_Base_Core::PlanDetailsList planList;
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
        try
        {
			std::auto_ptr<TA_Base_Bus::AlarmDataItem> pTemp (alarmRetriever.getAlarmItem(selectedAlarms[0]));
			alarm = pTemp;

	        unsigned long entityKey = alarm->assocEntityKey;
	        unsigned long entityTypeKey = alarm->assocEntityTypeKey;
            // The messageTypeKey member actually holds the alarm type name
			// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
			/*
            std::string alarmTypeKey = alarmRetriever.getDatabaseCache().getAlarmTypeKey(alarm->messageTypeKey.in());
			*/

		    planList = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().getAssociatedPlanDetails(entityKey,
                                                                                                    entityTypeKey,
																									alarm->messageTypeKey);

        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine plan associations");
//
//            if (alarm != NULL)
//            {
//                delete alarm;
//                alarm = NULL;
//            }

            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the alarm plan associations from the database");

//            if (alarm != NULL)
//            {
//                delete alarm;
//                alarm = NULL;
//            }

            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the alarm plan associations from the database");

//            if (alarm != NULL)
//            {
//                delete alarm;
//                alarm = NULL;
//            }

            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }

        PlanAssociationDialog dialog(planList);
        if (IDOK == dialog.DoModal())
        {
            std::ostringstream commandLine;
            commandLine << "--exec-plan-id=" << dialog.getSelectedPlanPath();

            using namespace TA_Base_Bus::TA_GenericGui;

            CPoint pt(GetMessagePos());

            try
            {
                RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                           AREA_SCHEMATIC,
                                                                           pt.x);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Plan manager at xPos: %d yPos: %d and it should resize to fit on a single screen.", position.left, position.top);   
                TA_Base_Bus::AppLauncher::getInstance().launchApplication(PLAN_MANAGER_GUI_APPTYPE,
                                                                     commandLine.str(),
                                                                     POS_BOUNDED|POS_RESIZABLE,
                                                                     ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                     NULL,
                                                                     &position);
            }
            catch(TA_Base_Core::ApplicationException&)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch plan manager");

//                if (alarm != NULL)
//                {
//                    delete alarm;
//                    alarm = NULL;
//                }

                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN) );
            }

            try
            {
                // Let the alarm agent know the plan has been run
				std::auto_ptr<TA_Base_Core::AlarmModifyItem> pAlarmModifyItem(alarmRetriever.getAlarmModifyItem(selectedAlarms[0]));
                m_alarmHelper.setAssociatedPlanWasRun(pAlarmModifyItem.get()); 
            }
            catch( const TA_Base_Core::NoAlarmAgentException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", "Could not set that the plan was run on the agent");

//                if (alarm != NULL)
//                {
//                    delete alarm;
//                    alarm = NULL;
//                }

                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN) );
             }
        }

//        if (alarm != NULL)
//        {
//            delete alarm;
//        }
        
        FUNCTION_EXIT;
    }
	void ActionRelatedDSS::preformPreAction()
	{
		getPlanManagerKey();
	}

    unsigned long ActionRelatedDSS::getPlanManagerKey()
    {
        FUNCTION_ENTRY("getPlanManagerKey");

        if (0 == m_planManagerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList planManagers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::PlanManagerEntityData::getStaticType(),0); //for fast query who knows the reason

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
