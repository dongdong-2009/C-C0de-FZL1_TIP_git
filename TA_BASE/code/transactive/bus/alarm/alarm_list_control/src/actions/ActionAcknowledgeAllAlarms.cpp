/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAllAlarms.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that acknowledges all visible alarms
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAllAlarms.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/resource.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"
#include "core/message/types/TrainAgentAlarms_MessageTypes.h"
#include "core/message/types/TrainCctvCameraAlarms_MessageTypes.h"
#include "bus/alarm/alarm_list_control/src/TrainAlarms.h"

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterAlarmRequest.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSProfile.h"

#include "boost/shared_ptr.hpp"


using namespace I_ALARM_PROVIDER;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionAcknowledgeAllAlarms::ActionAcknowledgeAllAlarms(HWND parentWindowHandle, 
                                            bool isAvailableInContextMenu /*=true*/)
        : m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
          m_isAvailableInContextMenu(isAvailableInContextMenu),
          m_parentWindowHandle(parentWindowHandle), m_cctvTrainPreConflictAlarmTypeKey(0),
		  m_trainCctvCameraAlarmTypeKey(0)
    {
    	std::stringstream stringStream;
		stringStream << TA_Base_Core::TrainAgentAlarms::CctvTrainPreConflictAlarm.getTypeKey();
		stringStream >> m_cctvTrainPreConflictAlarmTypeKey;
		stringStream.clear();
		stringStream << TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm.getTypeKey();
		stringStream >> m_trainCctvCameraAlarmTypeKey;
		stringStream.clear();
    }

    
    ActionAcknowledgeAllAlarms::~ActionAcknowledgeAllAlarms()
    {
        try
        {

            TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        }
        catch( ... )
        {
        }
    }


    bool ActionAcknowledgeAllAlarms::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

        //std::vector<std::string>& allAlarms = alarmRetriever.getAllVisibleAlarms();
        //if (allAlarms.empty())
		if (alarmRetriever.isAlarmsVisible() == false)
        {
            // No alarms selected.
            FUNCTION_EXIT;
            return false;
        }
		// No suitable alarms were found so this option is unavailable
		FUNCTION_EXIT;
		return true;
    }


    void ActionAcknowledgeAllAlarms::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        //TA_ASSERT(m_alarmHelper != NULL, "Alarm helper was null so this was not initialised correctly in the constructor");
		std::vector< boost::shared_ptr<AlarmRequest> > selectedATSACKAlarm;

        if (alarmRetriever.isActionAlreadyRunning())
        {
            FUNCTION_EXIT;
            return;
        }
        alarmRetriever.setIsActionAlreadyRunning(true, "Acknowledging alarms . . .");

        CString actionName;
        actionName.LoadString(IDS_ACKNOWLEDGE_ALL_VIEWABLE_ALARMS);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);
		if (IDNO == selectedButton)
        {
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return;
        }

        std::vector<TA_Base_Core::AlarmModifyItem*> allAlarms = alarmRetriever.getAllVisibleModItems();

        if (allAlarms.empty())
        {
            // Nothing to do as no alarms
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the alarm details for each of the alarms and add them to a vector
        bool wasSuccessful = true;

		std::vector<TA_Base_Core::AlarmModifyItem*> tempselectalarm;
		tempselectalarm.reserve( allAlarms.size());

		for(std::vector<TA_Base_Core::AlarmModifyItem*>::iterator itr = allAlarms.begin(); itr!=allAlarms.end(); itr++)
		{
			AlarmDataItem * pTemp = alarmRetriever.getAlarmItem((*itr)->alarmID);
			if (pTemp == NULL) continue;

			std::auto_ptr<AlarmDataItem> pAlarm (pTemp);
			if(pAlarm->state == TA_Base_Core::AlarmOpen || pAlarm->state == TA_Base_Core::AlarmClosed)
			{
			   unsigned long alarmTypeKey = pAlarm->messageTypeKey;
			   if (pAlarm->isExternalAlarm)
				{
					//handle ATS alarm
					boost::shared_ptr<ExtProfileInfo> profile=DataStoreReceiverATSProfile::getInstance()->getProfile();
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"ACK ATS Alarm %s", pAlarm->alarmID.c_str());
					boost::shared_ptr<AlarmRequest>	ackCommand (new AlarmRequest());
					ackCommand->requestTime=time(NULL);
					ackCommand->operatorID=profile->operatorId;
					ackCommand->alarmId=pAlarm->alarmID;
					ackCommand->state=I_ALARM_PROVIDER::AlarmAcknowledged;
					selectedATSACKAlarm.push_back(ackCommand);					
				}
				else
				{
					if( alarmTypeKey == m_cctvTrainPreConflictAlarmTypeKey)
					{
						//TODO: to be confirmed with requirement
						/*
						PreConflictAckDlg preconflictAckDlg;
						//CL-20990 xinyang++
						//preconflictAckDlg.DoModal(*itr);		--xinyang	
						if (IDOK == preconflictAckDlg.DoModal(*itr))	
						{
							//When the returns is IDOK, Acknowledged selectedAlarms.
							tempselectalarm.push_back(*itr);
						}
						//++xinyang
						*/
					}
					else if( alarmTypeKey == m_trainCctvCameraAlarmTypeKey)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"ACK CctvCamera Alarm");
						TA_Base_Bus::TrainAlarms::getInstance().acknowledgeAlarm(pAlarm.get());
						tempselectalarm.push_back(*itr);		//CL-20990 xinyang++

					}
					//CL-20990 xinyang++
					else
					{
						tempselectalarm.push_back(*itr);
					}
				}
			}
			//CL-20990 xinyang++
			else
			{
				tempselectalarm.push_back(*itr);
			}	
			//++xinyang
		}

        // Now get those alarms acknowledged
        try
        {
			std::vector<std::string>  alarmNoExists;
            wasSuccessful=!m_alarmHelper.operatorAcknowledgeAlarmsEx(tempselectalarm,alarmNoExists); 
			if (alarmNoExists.size()>0)
			{		 
				alarmRetriever.deleteSharedAlarm(alarmNoExists);				
			}
//            m_alarmHelper.operatorAcknowledgeAlarms(tempselectalarm);
        }
        catch( const TA_Base_Core::NoAlarmAgentException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", "Could not acknowledge some or all of the alarms");
            wasSuccessful = false;
        }
		
		if (selectedATSACKAlarm.size()>0) 
		{
			std::vector< boost::shared_ptr<AlarmRequest> > ATSAlarmForACK;
			std::vector< boost::shared_ptr<AlarmRequest> >::iterator iterStart = selectedATSACKAlarm.begin();
			std::vector< boost::shared_ptr<AlarmRequest> >::iterator iterEnd;

			//When error happened,try three times.
			int ErrTimes=0;

			for(int i=0;i<=(selectedATSACKAlarm.size()/AlarmRequestManager::MaxItemCount);i++)
			{
				//the last time
				if (i==(selectedATSACKAlarm.size()/AlarmRequestManager::MaxItemCount))
				{
					ATSAlarmForACK.assign(iterStart,selectedATSACKAlarm.end());
				}
				else
				{
					iterEnd = iterStart + AlarmRequestManager::MaxItemCount;
					ATSAlarmForACK.assign(iterStart,iterEnd);
				}
				try
				{
					DataStoreUpdaterAlarmRequest::getInstance()->onDispatchObjects(ATSAlarmForACK);
				}
				catch(char *str )
				{
					if (ErrTimes<3)
					{
						ErrTimes++;
						i--;
						ATSAlarmForACK.clear();
						Sleep(1000);
						continue;
					}
					else
					{
						alarmRetriever.freeAlarmModifyItems(allAlarms);
						alarmRetriever.setIsActionAlreadyRunning(false);
						LOG_EXCEPTION_CATCH(SourceInfo, str, "ATS queue overflow, Could not acknowledge some or all of the alarms.");
						TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARMS) );
					}
				}
				Sleep(500);
				iterStart = iterEnd;
				ATSAlarmForACK.clear();
			}
		
		}
		
		alarmRetriever.freeAlarmModifyItems(allAlarms);
        alarmRetriever.setIsActionAlreadyRunning(false);

        if (!wasSuccessful)
        {
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARMS) );
        }

        FUNCTION_EXIT;
   }
}
