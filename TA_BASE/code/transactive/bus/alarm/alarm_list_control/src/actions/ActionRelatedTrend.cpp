/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedTrend.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that displays the trend viewer with related information
 */

#pragma warning(disable:4786)

#include <sstream>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedTrend.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/ApplicationException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionRelatedTrend::ActionRelatedTrend(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu),
        m_trendViewerKey(0)
    {
    }

    
    ActionRelatedTrend::~ActionRelatedTrend()
    {
    }


    bool ActionRelatedTrend::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // Function only available when one alarm is selected
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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_TREND);
		}

        // First retrieve the entity name from the alarm
        unsigned long entityKey;
        try
        {
			alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL) return false;

            entityKey =  alarm->assocEntityKey; //selectedAlarms[0]->assocEntityKey;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            FUNCTION_EXIT;
            return false;
        }

        // Before we can check if this alarm has a related plan, 
        // need to work out if the user can acutally use the plan manager.
        // Now determine if we have the rights to do this
        // take care of missing entity for trend viewer
        unsigned long trendViewerEntityKey = getTrendViewerKey();
        if (trendViewerEntityKey == 0)
        {
            // didn't find entity key for trend viewer, no need to check rights
            return false;
        }
        // if (alarmRetriever.getRightsChecker().canPerformAction(getTrendViewerKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        if (alarmRetriever.getRightsChecker().canPerformAction(trendViewerEntityKey, TA_Base_Bus::aca_RUN_APPLICATION))
        {        
     	    try
		    {
                TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
            
                TA_Base_Core::DataPointEntityData* dataPoint = dynamic_cast<TA_Base_Core::DataPointEntityData*>(&entity);
                if( dataPoint != NULL && dataPoint->getEnableTrending())
                {
                    // This is a datapoint and it has trending enabled
                    return true;
                }
		    }
		    catch(...)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not determine the entity data for the selected alarm");
		    }
        }

        FUNCTION_EXIT;
		return false;
    }


    void ActionRelatedTrend::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // Function only available when one alarm is selected
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity name from the alarm
		//TD16124
        unsigned long entityKey;
        std::string entityName("");
        try
        {
			AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pdataTemp == NULL) return;

			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarmDetail(pdataTemp);
            entityKey = alarmDetail->assocEntityKey;
			TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
            entityName = entity.getName();
			//AfxMessageBox(entityName.c_str());
            //entityName = alarm->entityName;
            //delete alarm;
        }
		//TD16124
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_TREND_VIEWER) );
        }
        

		std::ostringstream commandLine;

		//entityName = _T("DBG.ECS.TVS.TVF03.diiTVF-OffForward");//lkm
        commandLine << "--items=" << entityName;
       
        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Trend Viewer at xPos: %d yPos: %d and it should resize to fit on a single screen.", position.left, position.top);
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(TREND_VIEWER_GUI_APPTYPE,
                                                                 commandLine.str(),
                                                                 POS_BOUNDED|POS_RESIZABLE,
                                                                 ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                 NULL,
                                                                 &position);
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch trend viewer");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_TREND_VIEWER) );
        }

        FUNCTION_EXIT;
    }
	void ActionRelatedTrend::preformPreAction()
	{
		getTrendViewerKey();
	}

    unsigned long ActionRelatedTrend::getTrendViewerKey()
    {
        FUNCTION_ENTRY("getTrendViewerKey");

        if (0 == m_trendViewerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList trendViewers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::HistoryViewerData::getStaticType(),0); //for fast query who knows the reason

                if (1 == trendViewers.size())
                {
                    m_trendViewerKey = trendViewers[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu trend viewer entities in the database. There should be one only.",
                        trendViewers.size());
                    if ( trendViewers.size() > 1)
                    {
                        // use the first one
                        m_trendViewerKey = trendViewers[0]->getKey();
                    }
                }

                // free the entity data
                while (! trendViewers.empty())
                {
                    delete trendViewers.back();
                    trendViewers.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Trend Viewer entity key");
            }
        }

        FUNCTION_EXIT;
        return m_trendViewerKey;
    }
}
