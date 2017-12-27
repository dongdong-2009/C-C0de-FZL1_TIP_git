/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedInspector.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that launches the Inspector Panel displaying the related information
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedInspector.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EquipmentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"

#include "bus/alarm/alarm_list_control/src/ATSAlarmBridgeWrap.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{

    const std::string ActionRelatedInspector::INSPECTOR_PANEL_ENTITY_TYPE_NAME("InspectorPanel");


    ActionRelatedInspector::ActionRelatedInspector(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu),
        m_inspectorPanelKey(0)
    {
    }

    
    ActionRelatedInspector::~ActionRelatedInspector()
    {
    }


    bool ActionRelatedInspector::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_INSPECTOR);
		}
        // First retrieve the entity key from the alarm
        unsigned long entityKey;
        try
        {
			AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pdataTemp == NULL) return false;

			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarmDetail(pdataTemp);
            entityKey = alarmDetail->assocEntityKey;
           // delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            FUNCTION_EXIT;
            return false;
        }

        // Now determine if we have the rights to do this
        if (alarmRetriever.getRightsChecker().canPerformAction(getInspectorPanelKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        {
            // Now determine if this entity is a datapoint.
		    try
		    {
                TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
                TA_Base_Core::DataPointEntityData* dataPoint = dynamic_cast<TA_Base_Core::DataPointEntityData*>(&entity);

                if( dataPoint != NULL)
                {
                    // This is a datapoint so this option is available if the datapoint has a parent
                    if (dataPoint->getParent() != 0)
                    {
                        return true;
                    }
                }
                else
                {
                    TA_Base_Core::EquipmentEntityData* equipment = dynamic_cast<TA_Base_Core::EquipmentEntityData*>(&entity);
                    if (equipment != NULL)
                    {
                        // This is a piece of equipment so this option is available
                        return true;
                    }
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


    void ActionRelatedInspector::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity key from the alarm
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarmDetail;
        unsigned long entityKey;
        try
        {
			alarmDetail.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (NULL == alarmDetail.get()) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve alarm details.");
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL));
			}
            entityKey = alarmDetail->assocEntityKey;
            
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
        }

		
		if(true == alarmDetail->isExternalAlarm)
		{
			CPoint pt(GetMessagePos());
			RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_GenericGui::SCREEN_CURRENT,
				TA_GenericGui::AREA_SCHEMATIC,
				pt.x);
			
			if (alarmDetail->dataSource == ATS_SUBSYSTEM_ID)
			{
				try 
				{
					SHORT nRetVal = 0;
					std::auto_ptr<ATSAlarmBridgeWrap> pObjATSBridge(new ATSAlarmBridgeWrap);

					nRetVal = pObjATSBridge->showIP(alarmDetail->alarmID, position.left);

					if (0 != nRetVal)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error returned calling ShowIP. Error Code=%d", nRetVal);
						TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
					}
				}
				catch( const TA_Base_Core::AlarmListCtrlException& )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Error occurred while calling to ATSAlarmBridgeUtility.dll");
					TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Inspector Panel");
					TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
				}
			}
			else if (alarmDetail->dataSource == VA_SUBSYSTEM_ID)
			{
				try
				{
					char szParam[256];
					std::string strVAClientPath = ExtAlarmActionChecker::getInstance().getVAClientPath();
					sprintf(szParam, "-X=%d -Y=%d ¨CAlarmID=%s", position.left, position.top, alarmDetail->alarmID.c_str());

					HINSTANCE hRetVal = ShellExecute(NULL, "open", strVAClientPath.c_str(), szParam, NULL, SW_SHOWDEFAULT);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Return value calling ShellExecute. Code=%d", hRetVal);

				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown error occurred");
				}
			}
			
			return;
		}

        std::string entityName("");
		try
		{
            TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
            entityName = entity.getName();

            TA_Base_Core::DataPointEntityData* dataPoint = dynamic_cast<TA_Base_Core::DataPointEntityData*>(&entity);
            if( dataPoint != NULL)
            {
                // This is a datapoint so get the parent to pass in as the control entity
                if (entity.getParent() != 0)
                {
                    entityName = entity.getParentName();
                }
                else
                {
                    TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
                }
            }
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not determine the entity data for the selected alarm");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
		}

		std::string commandLine("--control-entity=");
        commandLine += entityName;
        // TD15369 display Information tab by default when launch InspectorPanel from AlarmManager
        commandLine += " --screen-name=INFORMATION";

        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Inspector Panel at xPos: %d yPos: %d.", position.left, position.top);   
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(INSPECTOR_PANEL_GUI_APPTYPE,
                                                                 commandLine,
                                                                 POS_BOUNDED,
                                                                 ALIGN_VERTICAL|ALIGN_HORIZONTAL,
                                                                 NULL,
                                                                 &position);
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Inspector Panel");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL) );
        }


        FUNCTION_EXIT;
    }

    void ActionRelatedInspector::preformPreAction()
	{
		getInspectorPanelKey();
	}

    unsigned long ActionRelatedInspector::getInspectorPanelKey()
    {
        FUNCTION_ENTRY("getInspectorPanelKey");

        // Before we can check if this alarm really is related to a datapoint, 
        // need to work out if the user can acutally use the inspector panel.
        // To do that, we need to get its entity key.
        
        if (0 == m_inspectorPanelKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList inspectorPanels = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(INSPECTOR_PANEL_ENTITY_TYPE_NAME,0);//for fast query who knows the reason

                if (1 == inspectorPanels.size())
                {
                    m_inspectorPanelKey = inspectorPanels[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu inspector panel entities in the database. There should be one only.",
                        inspectorPanels.size());
                    if ( inspectorPanels.size() > 1)
                    {
                        // use the first one
                        m_inspectorPanelKey = inspectorPanels[0]->getKey();
                    }
                }

                // free the entity data
                while (! inspectorPanels.empty())
                {
                    delete inspectorPanels.back();
                    inspectorPanels.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Inspector Panel entity key");
            }
        }

        FUNCTION_EXIT;
        return m_inspectorPanelKey;
    }
}
