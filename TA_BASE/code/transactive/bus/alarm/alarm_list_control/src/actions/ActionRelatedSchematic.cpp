/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2015/01/23 15:32:17 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that displayed the schematic related to the alarm.
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"

#include "bus/alarm/alarm_list_control/src/ATSAlarmBridgeWrap.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/alarm/alarm_list_control/src/resource.h"

using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
	IMPLEMENT_DYNAMIC(ActionRelatedSchematic, CCmdTarget)

	BEGIN_MESSAGE_MAP(ActionRelatedSchematic, CCmdTarget)
	END_MESSAGE_MAP()

	BEGIN_DISPATCH_MAP(ActionRelatedSchematic, CCmdTarget)
		DISP_FUNCTION(ActionRelatedSchematic, "DisplaySchematic", OnDisplaySchematic, VT_I4, VTS_WBSTR VTS_WBSTR VTS_I2)
	END_DISPATCH_MAP()

	// Note: we add support for IID_ISink to support typesafe binding
	//  from VBA.  This IID must match the GUID that is attached to the 
	//  dispinterface in the .IDL file.

	// {51CF45A6-FD10-4F51-8618-AD03C9207F62}
	//static const IID IID_ISink =
	//{ 0x51CF45A6, 0xFD10, 0x4F51, { 0x86, 0x18, 0xAD, 0x3, 0xC9, 0x20, 0x7F, 0x62 } };

	BEGIN_INTERFACE_MAP(ActionRelatedSchematic, CCmdTarget)
		INTERFACE_PART(ActionRelatedSchematic, DIID__IATSAlarmActionManagerEvents, Dispatch)
	END_INTERFACE_MAP()

	void ActionRelatedSchematic::OnFinalRelease()
	{
		// When the last reference for an automation object is released
		// OnFinalRelease is called.  The base class will automatically
		// deletes the object.  Add additional cleanup required for your
		// object before calling the base class.

		CCmdTarget::OnFinalRelease();
	}
	// Sink message handler
	LONG ActionRelatedSchematic::OnDisplaySchematic(BSTR bstrAlarmID, BSTR bstrSchematic, SHORT leftPosition)
	{
		USES_CONVERSION;
		CPoint pt(GetMessagePos());
		try
		{
			std::string alarmID = OLE2A(bstrAlarmID);
			std::string strDisplay = OLE2A(bstrSchematic);
			
			leftPosition = pt.x;
			SCREENOFFSET_MAP::iterator itFind = m_mapScreenOffset.find(alarmID);
			if (itFind != m_mapScreenOffset.end())
			{
				leftPosition = (*itFind).second;
				m_mapScreenOffset.erase(itFind);
			}
				
			TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(strDisplay, "", leftPosition);
		}
		catch(TA_Base_Core::ApplicationException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Failed to launch graphworx screen with the specified display");
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName;
			unsigned int errorID = IDS_UE_010002;
			actionName.LoadString(IDS_SCHEMATIC);

			if (!actionName.IsEmpty())
			{
				userMsg << actionName;
			}
			userMsg.showMsgBox(errorID);
			//TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

		return S_OK;
	}

    ActionRelatedSchematic::ActionRelatedSchematic(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
		EnableAutomation();
    }
   
    ActionRelatedSchematic::~ActionRelatedSchematic()
    {
    }


    bool ActionRelatedSchematic::isAvailable(IAlarmRetriever& alarmRetriever)
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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_SCHEMATIC);
		}

        // First retrieve the entity name from the alarm
        unsigned long entityKey;
        try
        {
			alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL) return false;

            entityKey = alarm->assocEntityKey;
           // delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            return false;
        }

		try
		{
            TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
			std::string display = entity.getAlarmDisplay();

            // If the display string is not empty then this option is available
			if(!display.empty())
			{
                FUNCTION_EXIT;
                return true;
			}

		}
		catch(TA_Base_Core::DataException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not determine the alarm display for the selected alarm");
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not determine the alarm display for the selected alarm");
		}

        FUNCTION_EXIT;
		return false;
    }


    void ActionRelatedSchematic::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity name from the alarm
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
        unsigned long entityKey;
        try
        {
			AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pdataTemp == NULL) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve alarm details.");
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}

			alarm.reset( pdataTemp);
            entityKey = alarm->assocEntityKey;
		
            //delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
        }

		if (true == alarm->isExternalAlarm)
		{
			
			try 
			{
				DWORD dwAdviseCookie;
				CComPtr<IUnknown> pSinkUnk;
				std::auto_ptr<ATSAlarmBridgeWrap> pObjATSBridge(new ATSAlarmBridgeWrap);
				pSinkUnk = this->GetInterface(&IID_IUnknown);

				pObjATSBridge->registerConnectionPoint(pSinkUnk, dwAdviseCookie);

				SHORT nRetVal = 0;

				CPoint pt(GetMessagePos());
				// must be inserted before we call to showSchematic
				m_mapScreenOffset.insert(SCREENOFFSET_MAP::value_type(alarm->alarmID, pt.x));
				
				// Call ATS show schematic
				nRetVal = pObjATSBridge->showSchematic( alarm->alarmID, pt.x);
				
				if (0 != nRetVal)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to launch Schematic. Error Code=%d", nRetVal);
					TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
				}	

				pObjATSBridge->unRegisterConnectionPoint(dwAdviseCookie);
			}
			catch( const TA_Base_Core::AlarmListCtrlException& )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Error occurred while calling to ATSAlarmBridgeUtility.dll");
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Inspector Panel");
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}
			return;
		}

        std::string display("");
        std::string entityName("");
		try
		{
            TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
			display = entity.getAlarmDisplay();
            entityName = entity.getName();

            // If the display string is not empty then this option is available
			if(display.empty())
			{
                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not determine the alarm display for the selected alarm");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

		// Get x coordinates of the selected item so we know which GraphWorx screen to update
		CPoint pt(GetMessagePos());
        try
        {
			TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(display, entityName, pt.x);
        }
		catch(TA_Base_Core::ApplicationException&)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Failed to launch graphworx screen with the specified display");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

        FUNCTION_EXIT;
    }


	void ActionRelatedSchematic::launchGraphWorxDisplay(DatabaseCache& caches,unsigned long entityKey)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "launchGraphWorxDisplay come in");
		std::string display("");
		std::string entityName("");
		try
		{
			TA_Base_Core::IEntityData& entity = caches.getEntity(entityKey);
			display = entity.getAlarmDisplay();
			entityName = entity.getName();

			// If the display string is not empty then this option is available
			if(display.empty())
			{
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not determine the alarm display for the selected alarm");
			TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

		// Get x coordinates of the selected item so we know which GraphWorx screen to update
		CPoint pt(GetMessagePos());
		try
		{
			TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(display, entityName, pt.x);
		}
		catch(TA_Base_Core::ApplicationException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Failed to launch graphworx screen with the specified display");
			TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "launchGraphWorxDisplay come out");
	}


}
