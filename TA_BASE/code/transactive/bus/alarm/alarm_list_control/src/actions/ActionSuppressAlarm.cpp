/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that allows the user to create a suppression rule.
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
    
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "bus/alarm/alarm_list_control/src/SuppressAlarmDialog.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

namespace TA_Base_Bus
{
    ActionSuppressAlarm::ActionSuppressAlarm(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
        m_profileKey=ULONG_MAX;
		// session key of the current user activating the rule
		m_sessionKey="";
		m_operatorName="";
		// Retreive all current alarm types and create a map for later checking of system alarm
		createAlarmTypeKeySystemMap();

    }

    
    ActionSuppressAlarm::~ActionSuppressAlarm()
    {
		// delete all alarm type instances
		m_alarmTypeKeySystemMap.clear();
    }


	void ActionSuppressAlarm::createAlarmTypeKeySystemMap()
	{
		FUNCTION_ENTRY("createAlarmTypeKeySystemMap");

		try
		{
			std::vector< TA_Base_Core::IAlarmTypeData*> allAlarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
			if (allAlarmTypes.size()>0)
			{
				for (unsigned int i=0; i<allAlarmTypes.size(); i++)
				{
					TA_Base_Core::IAlarmTypeData* alarmTypeData = allAlarmTypes[i];

					//std::ostringstream alarmTypeKeyStr;

					//alarmTypeKeyStr << alarmTypeData->getKey();

					m_alarmTypeKeySystemMap.insert(std::pair<unsigned long, bool> ( alarmTypeData->getKey(), alarmTypeData->isSystemAlarm()) );


				}
				//delete the alarm type after use
				for (std::vector< TA_Base_Core::IAlarmTypeData*>::iterator itr = allAlarmTypes.begin();
					itr != allAlarmTypes.end(); ++itr)
				{
					TA_Base_Core::IAlarmTypeData* data = *itr;
					if (data != NULL)
					{
						delete data;
						data = NULL;
					}
				}
				allAlarmTypes.clear();
			}
			else
			{
				// Log to inform that the DB is empty
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "There is no alarm type data loaded from the Database.");
			}
		}
		catch (TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
		catch (TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch(...)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown error occurred while retrieving alarm type data.");
		}

		FUNCTION_EXIT;
	}

    bool ActionSuppressAlarm::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.size() != 1)
        {
            // Available only if one alarm is selected
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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), INHIBIT_ALARM);
		}

		// Need alarm manager entity key to check the rights of this action
		TA_Base_Core::IEntityData* myEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
			TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

		unsigned long entityKey = myEntity->getKey();
		delete myEntity;
		// Check for rights
        if( !alarmRetriever.getRightsChecker().canPerformAction(entityKey, TA_Base_Bus::aca_ADD_ALARM_RULE) )
		{
			FUNCTION_EXIT;
			return false;
		}
        
		bool canTakeAction = true;
		for (int i=0; i<selectedAlarms.size(); i++)
		{
			// Check for system alarm
			alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL) return false;

			std::map<unsigned long, bool>::iterator matching = m_alarmTypeKeySystemMap.find(alarm->messageTypeKey );
			if (matching!=m_alarmTypeKeySystemMap.end())
			{
				if (matching->second == false)
				{
					canTakeAction = false;
					break;
				}
			}

			// Check for rights in ack and close alarm
			// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name			
			/*
            std::string alarmTypeKey = std::string(alarmDetail->messageTypeKey.in());
            if (alarmTypeKey.empty())
            {
                canTakeAction = false;
				if (alarmDetail!=NULL)
				{
					delete alarmDetail;
					alarmDetail = NULL;
				}
				break;
            }
			*/

            unsigned long actionClose = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                        TA_Base_Bus::AlarmActions::CLOSE_ALARM);

            if( alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, actionClose) )
            {
                // Alarm is open and the user has permission to close it. Now check that if the alarm
                // is not acknowledged the user can acknowledge it.
                // Now get the action we should be checking
                unsigned long actionAck = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                              TA_Base_Bus::AlarmActions::ACK_ALARM);

                if( !(alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, actionAck)) )
                {
                    // User has no permission to ack
                    canTakeAction = false;
//					if (alarmDetail!=NULL)
//					{
//						delete alarmDetail;
//						alarmDetail = NULL;
//					}
					break;
                }
            }
            else // no permission to close alarm
            {
                canTakeAction = false;
//				if (alarmDetail!=NULL)
//				{
//					delete alarmDetail;
//					alarmDetail = NULL;
//				}
				break;
            }

//			if (alarmDetail!=NULL)
//			{
//				delete alarmDetail;
//				alarmDetail = NULL;
// 			}
		}

		if (canTakeAction == false)
		{
			FUNCTION_EXIT;
			return false;
		}

        FUNCTION_EXIT;
        return true;
    }


    void ActionSuppressAlarm::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

		bool canPerform = canPerformActionOnAlarm(alarmRetriever);

        if(canPerform)
        {
            // Launch the Suppress Alarm dialog
            CSuppressAlarmDialog dlg(m_pAlarmDetails, m_sessionKey, m_profileKey,m_operatorName);
		    dlg.DoModal();
		    if (!dlg.wasSuccessful())
		    {
				canPerform = false;
		    }
				    
		    // Clean up and exit
            alarmRetriever.setIsActionAlreadyRunning(false);
			//m_selectedAlarmDetails.clear();
        }
        
		if (canPerform == false)
        {
           //UE-040023
            TA_THROW( TA_Base_Core::AlarmActionException(TA_Base_Core::AlarmActionException::CANNOT_CREATE_SUPPRESSION_RULE, 
                "A system alarm inhibition rule could not be created for this alarm.") );
        }

        FUNCTION_EXIT;
	}

    void ActionSuppressAlarm::performActionByType(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performActionByType");

		bool canPerform = canPerformActionOnAlarm(alarmRetriever);

        if(canPerform)
        {
            // Launch the Suppress Alarm dialog
            CSuppressAlarmDialog dlg(m_pAlarmDetails, m_sessionKey, m_profileKey,m_operatorName);
		    dlg.inhibitAlarmByType();
		    if (!dlg.wasSuccessful())
		    {
			    canPerform = false;
		    }
				    
		    // Clean up and exit
            alarmRetriever.setIsActionAlreadyRunning(false);
 //           for(std::vector<TA_Base_Core::AlarmDetailCorbaDef*>::iterator selectedAlarm = m_selectedAlarmDetails.begin();
//                selectedAlarm != m_selectedAlarmDetails.end(); ++selectedAlarm)
//            {
//                delete *selectedAlarm;
 //           }
//			m_selectedAlarmDetails.clear();
        }

		if (canPerform == false)
        {
           //UE-040023
            TA_THROW( TA_Base_Core::AlarmActionException(TA_Base_Core::AlarmActionException::CANNOT_CREATE_SUPPRESSION_RULE, 
                "A system alarm inhibition rule could not be created for this alarm.") );
        }

        FUNCTION_EXIT;
	}

    void ActionSuppressAlarm::performActionByDescription(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performActionByDescription");

		bool canPerform = canPerformActionOnAlarm(alarmRetriever);

        if(canPerform)
        {
            // Launch the Suppress Alarm dialog
            CSuppressAlarmDialog dlg(m_pAlarmDetails, m_sessionKey, m_profileKey,m_operatorName);
			dlg.inhibitAlarmByDescription();
		    if (!dlg.wasSuccessful())
		    {
			    canPerform = false;
		    }
				    
		    // Clean up and exit
            alarmRetriever.setIsActionAlreadyRunning(false);
//            for(std::vector<TA_Base_Core::AlarmDetailCorbaDef*>::iterator selectedAlarm = m_selectedAlarmDetails.begin();
//                selectedAlarm != m_selectedAlarmDetails.end(); ++selectedAlarm)
//            {
//                delete *selectedAlarm;
//            }
//			m_selectedAlarmDetails.clear();
        }

		if (canPerform == false)
        {
           //UE-040023
            TA_THROW( TA_Base_Core::AlarmActionException(TA_Base_Core::AlarmActionException::CANNOT_CREATE_SUPPRESSION_RULE, 
                "A system alarm inhibition rule could not be created for this alarm.") );
        }

        FUNCTION_EXIT;
	}

	bool ActionSuppressAlarm::canPerformActionOnAlarm(IAlarmRetriever& alarmRetriever)
	{
		FUNCTION_ENTRY("canPerformActionOnAlarm");

		if (alarmRetriever.isActionAlreadyRunning())
        {
            FUNCTION_EXIT;
            return false;
        }
        alarmRetriever.setIsActionAlreadyRunning(true, "Creating suppression rule . . .");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.size() != 1)
        {
            // Can only perform action if one alarm is selected
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return false;
        }

		// Retrieve the alarm details for each of the alarms and add them to a vector
        bool loadError = false;
		m_pAlarmDetails.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));

        // Get the current profile
        std::vector<unsigned long> profiles;
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        try
        {
            TA_Base_Bus::AuthenticationLibrary authLib(true);
            profiles = authLib.getProfiles(sessionId);
			//SessionInfo sessionInfo= authLib.getSessionInfo(sessionId);				
			m_profileKey = -1;
			if(profiles.size() == 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No profiles were retrieved.");
				loadError = true;
			}
			else
			{
				m_profileKey = profiles[profiles.size() - 1];
				m_sessionKey = sessionId;
			}
			
 
			TA_Base_Core::IOperator* op=TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession(sessionId,false);                        
			m_operatorName=op->getName();
		  	delete op;
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.what());
            loadError = true;
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
            loadError = true;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown error occurred while retrieving current profile.");
            loadError = true;
        }

        

		FUNCTION_EXIT;
		return !(loadError);

	}
}
