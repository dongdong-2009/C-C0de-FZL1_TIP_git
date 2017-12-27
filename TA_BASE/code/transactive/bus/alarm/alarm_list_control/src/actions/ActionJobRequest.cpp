/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionJobRequest.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2015/02/24 15:40:16 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that allows a job request to be initiated for an alarm.
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionJobRequest.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/ApplicationException.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmModificationHelper.h"

#include <sstream>

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterAlarmACKCommand.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSProfile.h"

#include "boost/shared_ptr.hpp"
using namespace I_ALARM_PROVIDER;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;


namespace TA_Base_Bus
{
    ActionJobRequest::ActionJobRequest(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu),
		m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper())
    {
		
    }

    
    ActionJobRequest::~ActionJobRequest()
    {
    }


    bool ActionJobRequest::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return false;
        }
		
		// Need to check that a job request is allowed for this particular alarm.
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
		bool isOpen = false;
		char mmsState = ' ';
		unsigned long assocEntityKey;
		
        try
        {
			AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pTemp == NULL) return false;

			std::auto_ptr<TA_Base_Bus::AlarmDataItem> temp(pTemp);
			alarm = temp; // transfer ownership
            isOpen = (alarm->state == TA_Base_Core::AlarmOpen)?true:false;
			mmsState = alarm->mmsState[0];
			assocEntityKey = alarm->assocEntityKey;

			switch (mmsState)
			{
				case 0 ://TA_Base_Core::MMS_NONE:
					// MMS functionality not available
					return false;

				case 'S'://TA_Base_Core::MMS_SEMI:
					// MMS functionality available.
					break;

				case 'A'://TA_Base_Core::MMS_AUTO:
					// MMS Alarm message already sent.
					return false;

				case 'R'://TA_Base_Core::MMS_JR_SENT:
					// Job request already raised.
					return false;

			/*	case TA_Base_Core::MMS_ALM_SENT:
					TA_ASSERT(false, "Received MMS_ALM_SENT mmsState, which is not supported");
					return false;
					*/

				case 'I'://TA_Base_Core::MMS_INHIBIT:
					// Alarm messages inhibited.
					return false;

				case 'D'://TA_Base_Core::MMS_FAILED:
					// It's up to the operator to retry after a failure.
					break;

				default:
					TA_ASSERT(false, "Received unknown mmsState. Has the MmsStateType enum changed?");
					return false; 
			}
		}
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine job request availability");
        }
		catch(...)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine job request availability");
		}

		if (!isOpen)
		{
			// If an alarm is normalised, you cannot raise a job request.
			return false;
		}

		//ATS Alarm check
		if (alarm->isExternalAlarm && ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), JOB_REQUEST))
		{
			FUNCTION_EXIT;
            return true;
		}
        // Need to check if the user can raise a job request.
        
        unsigned long action = TA_Base_Bus::aca_DATA_NODE_RAISE_JOB_REQUEST;

		FUNCTION_EXIT;
        return (alarmRetriever.getRightsChecker().canPerformAction(assocEntityKey, action));
    }


    void ActionJobRequest::performAction(IAlarmRetriever& alarmRetriever)
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
            // Function only available when one alarm is selected
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return;
        }

		/// Get the alarm's location key.
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(alarmRetriever.getAlarmItem(selectedAlarms[0]));
		unsigned long locationKey = alarm->locationKey;
		std::vector<TA_Base_Core::AlarmModifyItem*> alarmAckList;
		// For ISCS alarms
		if (false == alarm->isExternalAlarm)
		{
			// Put the alarm into a vector - this is needed for the following call to acknowledge alarms
			TA_Base_Core::AlarmModifyItem * pAlarmModifyItem = alarmRetriever.getAlarmModifyItem(selectedAlarms[0]);
			alarmAckList.push_back(pAlarmModifyItem);
		}

		// Setup the commandline in the format "--launch-condition=alarm:<location key>:<alarm ID>"
		std::ostringstream commandLine;
		commandLine << "--launch-condition=alarm:";
		commandLine << locationKey << ":";
		commandLine << alarm->alarmID << ":";
		commandLine << alarm->isExternalAlarm?1:0;

        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());

        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Job Request Manager at xPos: %d yPos: %d and it should resize to fit on a single screen.", position.left, position.top);
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(JOB_REQUEST_MANAGER,
                                                                 commandLine.str(),
                                                                 POS_BOUNDED,
                                                                 ALIGN_VERTICAL|ALIGN_HORIZONTAL,
                                                                 NULL,
                                                                 &position);//Mintao++ TD16121

        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Job Request Manager");

			alarmRetriever.freeAlarmModifyItems(alarmAckList);
			alarmRetriever.setIsActionAlreadyRunning(false);

            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_REQUEST_MANAGER) );
        }

		// if ATS then Acknowledge
		if(true == alarm->isExternalAlarm)
		{
			std::vector< boost::shared_ptr<AlarmACKCommand> > ATSAlarmForACK;
			//handle ATS alarm
			boost::shared_ptr<ExtProfileInfo> profile=DataStoreReceiverATSProfile::getInstance()->getProfile();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"ACK ATS Alarm %s", alarm->alarmID);
			boost::shared_ptr<AlarmACKCommand>  ackCommand (new AlarmACKCommand());
			ackCommand->ackTime=time(NULL);
			ackCommand->alarmAcknowledgeBy=profile->operatorId;
			ackCommand->alarmId=alarm->alarmID;
			ATSAlarmForACK.push_back(ackCommand);	
	
			try
			{
				DataStoreUpdaterAlarmACKCommand::getInstance()->onDispatchObjects(ATSAlarmForACK);
			}
			catch(char *str )
			{	
				alarmRetriever.setIsActionAlreadyRunning(false);
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARM) );
			}

			alarmRetriever.setIsActionAlreadyRunning(false);
			FUNCTION_EXIT;
			return;
		}

		// Now need to ack the alarm if the operator has rights.
		bool canAckAlarm = false;

		try
		{
            if( strcmp(alarm->alarmAcknowledgedBy.c_str(), "") == 0 )// Alarm still yet to be acknowledged
			{
				// Now get the action we should be checking
				
				// The messageTypeKey member actually holds the alarm type key
				// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
				// std::string alarmTypeKey = alarmRetriever.getDatabaseCache().getAlarmTypeKey(alarm->messageTypeKey.in());

				unsigned long action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
					TA_Base_Bus::AlarmActions::ACK_ALARM);
				 
				if( alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, action) )
				{
					canAckAlarm = true;
				}
			}
		}
		catch( const TA_Base_Core::AlarmListCtrlException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not determine rights to ack alarm");
		}
		
		if (canAckAlarm)
		{
			try
			{
				// Now acknowledge the alarm
				std::vector<std::string > alarmNoExist;
				m_alarmHelper.operatorAcknowledgeAlarmsEx(alarmAckList, alarmNoExist);
			}
			catch(...)
			{
			//	delete alarm;
				alarmRetriever.freeAlarmModifyItems(alarmAckList);
				alarmRetriever.setIsActionAlreadyRunning(false);

                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARM) );
			}
		}

		alarmRetriever.freeAlarmModifyItems(alarmAckList);
		alarmRetriever.setIsActionAlreadyRunning(false);

        FUNCTION_EXIT;
    }
}
