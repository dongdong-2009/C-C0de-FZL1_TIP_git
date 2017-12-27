/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAlarm.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that acknowledges the selected alarms
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAlarm.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/PreConflictAckDlg.h"
#include "bus/alarm/alarm_list_control/src/TrainAlarms.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"
#include "core/message/types/TrainAgentAlarms_MessageTypes.h"
#include "core/message/types/TrainCctvCameraAlarms_MessageTypes.h"

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterAlarmRequest.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSProfile.h"

#include "boost/shared_ptr.hpp"

using namespace I_ALARM_PROVIDER;
using namespace TA_Base_Core;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionAcknowledgeAlarm::ActionAcknowledgeAlarm(bool isAvailableInContextMenu /*=true*/)
        : m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
          m_isAvailableInContextMenu(isAvailableInContextMenu), m_cctvTrainPreConflictAlarmTypeKey(0),
		  m_trainCctvCameraAlarmTypeKey(0)
    {
		std::stringstream stringStream;
		stringStream << TA_Base_Core::TrainAgentAlarms::CctvTrainPreConflictAlarm.getTypeKey();
		stringStream >> m_cctvTrainPreConflictAlarmTypeKey;
		stringStream.clear();
		stringStream << TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm.getTypeKey();
		stringStream >> m_trainCctvCameraAlarmTypeKey;
    }

    
    ActionAcknowledgeAlarm::~ActionAcknowledgeAlarm()
    {
        try
        {
            TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        }
        catch( ... )
        {
        }
    }


    bool ActionAcknowledgeAlarm::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

		std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty())
        {
            // No alarms selected.
            FUNCTION_EXIT;
            return false;
        }

        for(std::vector<std::string>::iterator iter = selectedAlarms.begin(); iter != selectedAlarms.end(); ++iter)
        {
            try
            {
				AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(*iter);
				if (pTemp == NULL) continue;
				
				std::auto_ptr<AlarmDataItem> alarm(pTemp);
                if (0 == strcmp(alarm->alarmAcknowledgedBy.c_str(), "")) // Alarm still yet to be acknowledged
                {
                    // Now get the action we should be checking

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
					//ATS Alarm check			
					//LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "ack==  %s", selectedAlarms[0]->alarmDescription);
					if (alarm->isExternalAlarm && ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), ACKNOWLEDGE_ALARM))
					{
						FUNCTION_EXIT;
						LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "ack= %s is a ATS alarm", alarm->alarmDescription.c_str());
						return true;
					}
                    unsigned long action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                TA_Base_Bus::AlarmActions::ACK_ALARM);

                    if( alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, action) )
                    {
                        // Alarm is not acked and the user has permission to ack so we can enable this option
                       // delete alarm;
                        return true;
                    }
                }
                //delete alarm;
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to acknowledge it");
            }
        }

        // No suitable alarms were found so this option is unavailable
        FUNCTION_EXIT;
        return false;
    }


    void ActionAcknowledgeAlarm::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

		std::vector< boost::shared_ptr<AlarmRequest> > selectedATSACKAlarm;

        if (alarmRetriever.isActionAlreadyRunning())
        {
            FUNCTION_EXIT;
            return;
        }
        alarmRetriever.setIsActionAlreadyRunning(true, "Acknowledging alarms . . .");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();

        if (selectedAlarms.empty())
        {
            // Nothing to do as no alarm is selected (or too many are selected)
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the alarm details for each of the alarms and add them to a vector
        bool wasSuccessful = true;
  
		//CL-20990 xinyang++
		std::vector<TA_Base_Core::AlarmModifyItem*> tempselectalarm;
		tempselectalarm.reserve( selectedAlarms.size());
		//++xinyang
        for(std::vector<std::string>::iterator itr = selectedAlarms.begin(); itr!=selectedAlarms.end(); itr++)
		{
			AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(*itr);
			if (pTemp == NULL) continue;
			
			std::auto_ptr<AlarmDataItem> alarm(pTemp);
			TA_Base_Core::AlarmModifyItem* pAlarmItem = NULL;
			if(bool(alarm->state == TA_Base_Core::AlarmOpen || alarm->state == TA_Base_Core::AlarmClosed))
			{
				unsigned long alarmTypeKey = alarm->messageTypeKey;
			   if (alarm->isExternalAlarm)
				{
					//handle ATS alarm
				    boost::shared_ptr<ExtProfileInfo> profile=DataStoreReceiverATSProfile::getInstance()->getProfile();
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"ACK ATS Alarm %s", alarm->alarmID.c_str());
					boost::shared_ptr<AlarmRequest>  ackCommand (new AlarmRequest());
					ackCommand->requestTime=time(NULL);
					ackCommand->operatorID=profile->operatorId;
					ackCommand->alarmId=alarm->alarmID;
					ackCommand->state=I_ALARM_PROVIDER::AlarmAcknowledged;
					selectedATSACKAlarm.push_back(ackCommand);					
				}
				else
				{
					if( alarmTypeKey == m_cctvTrainPreConflictAlarmTypeKey)
					{
						PreConflictAckDlg preconflictAckDlg;
						//CL-20990 xinyang++
						//preconflictAckDlg.DoModal(*itr);		--xinyang	
						if (IDOK == preconflictAckDlg.DoModal(alarm.get()))	
						{
							pAlarmItem = new TA_Base_Core::AlarmModifyItem(alarm->alarmID, alarm->alarmAcknowledgedBy, alarm->locationKey);
							tempselectalarm.push_back(pAlarmItem);
						}
						//++xinyang
					}
					else if( alarmTypeKey == m_trainCctvCameraAlarmTypeKey)
					{
						TA_Base_Bus::TrainAlarms::getInstance().acknowledgeAlarm(alarm.get());
						pAlarmItem = new TA_Base_Core::AlarmModifyItem(alarm->alarmID, alarm->alarmAcknowledgedBy, alarm->locationKey);
						tempselectalarm.push_back(pAlarmItem);		//CL-20990 xinyang++
					}
					//CL-20990 xinyang++
					else
					{
						pAlarmItem = new TA_Base_Core::AlarmModifyItem(alarm->alarmID, alarm->alarmAcknowledgedBy, alarm->locationKey);
						tempselectalarm.push_back(pAlarmItem);
					}
				}
			}
			//CL-20990 xinyang++
			else
			{
				pAlarmItem = new TA_Base_Core::AlarmModifyItem(alarm->alarmID, alarm->alarmAcknowledgedBy, alarm->locationKey);
				tempselectalarm.push_back(pAlarmItem);
			}	
			//++xinyang

		}
		
		// Now get those alarms acknowledged
        try
        {
			if (tempselectalarm.size()>0) 
			{		 
				std::vector<std::string>  alarmNoExists;
				wasSuccessful=!m_alarmHelper.operatorAcknowledgeAlarmsEx(tempselectalarm,alarmNoExists);
				if (alarmNoExists.size()>0)
				{		 
				   alarmRetriever.deleteSharedAlarm(alarmNoExists);
				}
			}
		  // m_alarmHelper.operatorAcknowledgeAlarms(selectedAlarms);
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
		
		alarmRetriever.freeAlarmModifyItems(tempselectalarm);
        alarmRetriever.setIsActionAlreadyRunning(false);

        if (!wasSuccessful)
        {
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_ALARMS) );
        }

        FUNCTION_EXIT;
   }


}
