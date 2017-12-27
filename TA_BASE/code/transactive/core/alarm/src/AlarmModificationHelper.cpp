/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmModificationHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:25:27 $
  * Last modified by:  $Author: huirong.luo $
  * 
  * This class manages closing, acknowledging, setting comments etc on alarms. It is used
  * by the alarm list control. It is not a class for general use by all apps.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperBase.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/exceptions/src/NoAlarmAgentException.h"


namespace TA_Base_Core
{
	const unsigned int AlarmModificationHelper::ACKNOWLEDGE_IN_BATCH_COUNT = 500;

    AlarmModificationHelper::AlarmModificationHelper(AlarmHelperBase& alarmHelperBase)
		:m_alarmHelperBase(alarmHelperBase)
    {
		//TD17994,jianghp
        //TA_ASSERT(NULL != alarmHelperBase, "AlarmHelperBase was NULL");
    }


    AlarmModificationHelper::~AlarmModificationHelper()
    {
        // Do not destroy the alarmHelperBase as it is
        // handled by the AlarmHelperManager
    }


    void AlarmModificationHelper::acknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId /*= NULL*/)   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("acknowledgeAlarms");

		operatorAcknowledgeAlarms(alarms, sessionId, false);

        FUNCTION_EXIT;
    }


    void AlarmModificationHelper::operatorAcknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId /*= NULL*/, bool isOperatorTriggered /*= true*/ )   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("operatorAcknowledgeAlarms");


        bool didFailureOccur = false;

        typedef std::map<unsigned long, std::vector<AlarmDetailCorbaDef*> > LocationToAlarmMap;
        LocationToAlarmMap locationToAlarmMap;
        for(std::vector<AlarmDetailCorbaDef*>::iterator itr=alarms.begin() ; itr!=alarms.end()  ; itr++)
        {
            locationToAlarmMap[(*itr)->locationKey].push_back(*itr);
        }

        for (LocationToAlarmMap::iterator mapItr = locationToAlarmMap.begin(); mapItr != locationToAlarmMap.end(); mapItr++)
        {
            bool wasSuccessful = acknowledgeAlarmsForLocation(mapItr->second, mapItr->first, sessionId, isOperatorTriggered );  // Raymond Pau++ TD13367 
            if (!wasSuccessful)
            {
                didFailureOccur = true;
            }
        }

        if (didFailureOccur)
        {
            TA_THROW( NoAlarmAgentException("Could not acknowledge one or more alarms for some reason") );
        }

        FUNCTION_EXIT;
    }
    
	bool AlarmModificationHelper::operatorAcknowledgeAlarmsEx(std::vector<AlarmModifyItem*>& alarms,
		std::vector<std::string>&  alarmNoExists,
		const char * sessionId /*= NULL*/, bool isOperatorTriggered /*= true*/ )   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("operatorAcknowledgeAlarms");


        bool didFailureOccur = false;

        typedef std::map<unsigned long, std::vector<AlarmModifyItem*> > LocationToAlarmMap;
        LocationToAlarmMap locationToAlarmMap;
        for(std::vector<AlarmModifyItem*>::iterator itr=alarms.begin() ; itr!=alarms.end()  ; itr++)
        {
            locationToAlarmMap[(*itr)->locationKey].push_back(*itr);
        }

        for (LocationToAlarmMap::iterator mapItr = locationToAlarmMap.begin(); mapItr != locationToAlarmMap.end(); mapItr++)
        {
			bool wasSuccessful = acknowledgeAlarmsForLocation(mapItr->second, alarmNoExists, mapItr->first, sessionId, isOperatorTriggered );  // Raymond Pau++ TD13367 
            if (!wasSuccessful)
            {
                didFailureOccur = true;
            }
        }

//         if (didFailureOccur)
//         {
//             TA_THROW( NoAlarmAgentException("Could not acknowledge one or more alarms for some reason") );
//         }	

        FUNCTION_EXIT;
		return didFailureOccur;
    }




    void AlarmModificationHelper::operatorCloseAndAcknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId /*= NULL*/)   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("operatorCloseAndAcknowledgeAlarms");

        bool didFailureOccur = false;

        typedef std::map<unsigned long, std::vector<AlarmDetailCorbaDef*> > LocationToAlarmMap;
        LocationToAlarmMap locationToAlarmMap;
        for(std::vector<AlarmDetailCorbaDef*>::iterator itr=alarms.begin() ; itr!=alarms.end()  ; itr++)
        {
            locationToAlarmMap[(*itr)->locationKey].push_back(*itr);
        }

        for (LocationToAlarmMap::iterator mapItr = locationToAlarmMap.begin(); mapItr != locationToAlarmMap.end(); mapItr++)
        {
            bool wasSuccessful = acknowledgeAndCloseAlarmsForLocation(mapItr->second, mapItr->first, sessionId);  // Raymond Pau++ TD13367 
            if (!wasSuccessful)
            {
                didFailureOccur = true;
            }
        }

        if (didFailureOccur)
        {
            TA_THROW( NoAlarmAgentException("Could not acknowledge and/or close one or more alarms for some reason") );
        }

        FUNCTION_EXIT;
    }

	bool AlarmModificationHelper::operatorCloseAndAcknowledgeAlarmsEx(std::vector<AlarmModifyItem*>& alarms,
		std::vector<std::string>&  alarmNoExists,
		const char * sessionId /*= NULL*/)   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("operatorCloseAndAcknowledgeAlarms");

        bool didFailureOccur = false;

        typedef std::map<unsigned long, std::vector<AlarmModifyItem*> > LocationToAlarmMap;
        LocationToAlarmMap locationToAlarmMap;
        for(std::vector<AlarmModifyItem*>::iterator itr=alarms.begin() ; itr!=alarms.end()  ; itr++)
        {
            locationToAlarmMap[(*itr)->locationKey].push_back(*itr);
        }

        for (LocationToAlarmMap::iterator mapItr = locationToAlarmMap.begin(); mapItr != locationToAlarmMap.end(); mapItr++)
        {
            bool wasSuccessful = acknowledgeAndCloseAlarmsForLocation(mapItr->second,alarmNoExists, mapItr->first, sessionId);  // Raymond Pau++ TD13367 
            if (!wasSuccessful)
            {
                didFailureOccur = true;
            }
        }

        FUNCTION_EXIT;
		return didFailureOccur;
    }

    bool AlarmModificationHelper::updateComments(std::vector<AlarmDetailCorbaDef*>& alarms,
                                                 const std::string& comments)
    {
        FUNCTION_ENTRY("updateComments");

        bool didFailureOccur = false;
        for ( std::vector<AlarmDetailCorbaDef*>::iterator itr=alarms.begin(); itr!=alarms.end(); ++itr )
        {
            if (*itr != NULL)
			{
				unsigned long locationKey = (*itr)->locationKey;
				bool wasSuccessful = updateCommentsForLocation(alarms, locationKey, comments);
				if (!wasSuccessful)
				{
					didFailureOccur = true;
				}
			}
        }

        if (didFailureOccur)
        {
            TA_THROW( NoAlarmAgentException("Could not update comment for one or more alarms for some reason") );
        }

        FUNCTION_EXIT;
		return didFailureOccur;
    }
   
	bool AlarmModificationHelper::updateComments(std::vector<AlarmModifyItem*>& alarms,
                                                 const std::string& comments)
    {
        FUNCTION_ENTRY("updateComments");

        bool didFailureOccur = false;
        for ( std::vector<AlarmModifyItem*>::iterator itr=alarms.begin(); itr!=alarms.end(); ++itr )
        {
            if (*itr != NULL)
			{
				unsigned long locationKey = (*itr)->locationKey;
				bool wasSuccessful = updateCommentsForLocation(alarms, locationKey, comments);
				if (!wasSuccessful)
				{
					didFailureOccur = true;
				}
			}
        }

        if (didFailureOccur)
        {
            TA_THROW( NoAlarmAgentException("Could not update comment for one or more alarms for some reason") );
        }

        FUNCTION_EXIT;
		return didFailureOccur;
    }
    
    void AlarmModificationHelper::setAssociatedPlanWasRun(AlarmDetailCorbaDef* alarm)
    {
        FUNCTION_ENTRY("setAssociatedPlan");

		std::string alarmID = alarm->alarmID.in();
		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the non-visible alarm type.") );
		}

        // Now find the alarm agent we are interested in
        unsigned long locationKey = alarm->locationKey;
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update alarm plan status");
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );
        }

        // Ok now tell the agent the plan status
        try
        {
			CORBA_CALL((*agent), planAssociatedWithAlarmWasRun, (alarm->alarmID, sessionId.c_str()));
            //(*agent)->planAssociatedWithAlarmWasRun(alarm->alarmID, sessionId.c_str());

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }

        TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );

        FUNCTION_EXIT;
    }

	void AlarmModificationHelper::setAssociatedPlanWasRun(AlarmModifyItem* alarm)
    {
        FUNCTION_ENTRY("setAssociatedPlan");

		std::string alarmID = alarm->alarmID.c_str();
		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the non-visible alarm type.") );
		}

        // Now find the alarm agent we are interested in
        unsigned long locationKey = alarm->locationKey;
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update alarm plan status");
            TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );
        }

        // Ok now tell the agent the plan status
        try
        {
			CORBA_CALL((*agent), planAssociatedWithAlarmWasRun, (alarm->alarmID.c_str(), sessionId.c_str()));
            //(*agent)->planAssociatedWithAlarmWasRun(alarm->alarmID, sessionId.c_str());

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }

        TA_THROW( NoAlarmAgentException("Could not set the plan status to run for the selected alarm.") );

        FUNCTION_EXIT;
    }


    void AlarmModificationHelper::setJobRequestSubmitted(const std::string& alarmID,unsigned long locationKey)
    {
        FUNCTION_ENTRY("setJobRequestSubmitted");

		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not submit the MMS job request for the non-visible alarm type.") );
		}

        //find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the MMS Status for the selected alarm.") );
        }


        // Ok now tell the agent the MMS status
		std::string errorMessage;
        try
        {
			//IDL REF:
            //void setMmsState(in string p_alarmId, in MmsStateType p_mmsState)
            //    raises( OperationModeException, AlarmException );

			CORBA_CALL((*agent), setMmsState, (alarmID.c_str(), MMS_JR_SENT));
            //(*agent)->setMmsState(alarmID.c_str(), MMS_JR_SENT);

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			errorMessage = "Could not resolve the alarm agent";
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
			errorMessage = "Could not resolve the alarm agent";
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
			errorMessage = "Alarm was not found or a Job Request was already submitted on this alarm";
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "Alarm Agent was not in Control mode";
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "An unknown error occured";
        }

        TA_THROW( NoAlarmAgentException(errorMessage.c_str() ) );

        FUNCTION_EXIT;
    }

	void AlarmModificationHelper::setJobRequestSubmitted(const std::string& alarmID,unsigned long locationKey, const char * sessionId, bool bNeedAckAlarm/*= true*/)
	{
		FUNCTION_ENTRY("setJobRequestSubmitted");

		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
			TA_THROW( NoAlarmAgentException("Could not submit the MMS job request for the non-visible alarm type.") );
		}

		//find the alarm agent we are interested in
		AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);

		if (agent == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
			TA_THROW( NoAlarmAgentException("Could not set the MMS Status for the selected alarm.") );
		}


		// Ok now tell the agent the MMS status
		std::string errorMessage;
		try
		{
			//IDL REF:
			//void setMmsState(in string p_alarmId, in MmsStateType p_mmsState)
			//    raises( OperationModeException, AlarmException );

			CORBA_CALL((*agent), setMmsState, (alarmID.c_str(), MMS_JR_SENT));
			//(*agent)->setMmsState(alarmID.c_str(), MMS_JR_SENT);

			if (bNeedAckAlarm)
			{
				std::auto_ptr<TA_Base_Core::AlarmModifyItem> pAlarmModItem(new TA_Base_Core::AlarmModifyItem(alarmID, "", locationKey));
				std::vector<TA_Base_Core::AlarmModifyItem*> vecItems;
				vecItems.push_back(pAlarmModItem.get());

				std::vector<std::string>  alarmNoExists;
				operatorAcknowledgeAlarmsEx(vecItems, alarmNoExists, sessionId, true);

			}

			FUNCTION_EXIT;
			return;
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			errorMessage = "Could not resolve the alarm agent";
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
			errorMessage = "Could not resolve the alarm agent";
		}
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
			LOGMORE(SourceInfo, ex.reason.in());
			errorMessage = "Alarm was not found or a Job Request was already submitted on this alarm";
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "Alarm Agent was not in Control mode";
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "An unknown error occured";
		}

		TA_THROW( NoAlarmAgentException(errorMessage.c_str() ) );

		FUNCTION_EXIT;
	}

	void AlarmModificationHelper::setAlarmMmsState(const std::string& alarmID,unsigned long locationKey, MmsStateType mmsState)
	{
        FUNCTION_ENTRY("setAlarmMmmsState");

		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not set the MMS state for the non-visible alarm type.") );
		}


        //find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the MMS Status for the selected alarm.") );
        }


        // Ok now tell the agent the MMS status
		std::string errorMessage;
        try
        {
			CORBA_CALL((*agent), setMmsState, (alarmID.c_str(), mmsState));
            //(*agent)->setMmsState(alarmID.c_str(), mmsState);

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			errorMessage = "Could not resolve the alarm agent";
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
			errorMessage = "Could not resolve the alarm agent";
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
			errorMessage = "Alarm was not found or a Job Request was already submitted on this alarm";
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "Alarm Agent was not in Control mode";
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the MMS status for the alarm on the alarm agent retrieved");
			errorMessage = "An unknown error occured";
        }

        TA_THROW( NoAlarmAgentException(errorMessage.c_str() ) );

        FUNCTION_EXIT;
	}

    void AlarmModificationHelper::postponePlanExecution(AlarmDetailCorbaDef* alarm)
    {
        FUNCTION_ENTRY("postponePlanExecution");

		std::string alarmID = alarm->alarmID.in();
		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the non-visible alarm type.") );
		}

        // Now find the alarm agent we are interested in
        unsigned long locationKey = alarm->locationKey;
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update alarm plan status");
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );
        }

        // Ok now tell the agent the plan status
        try
        {
			CORBA_CALL((*agent), planAssociatedWithAlarmWasPostponed, (alarm->alarmID, sessionId.c_str()));
            //(*agent)->planAssociatedWithAlarmWasPostponed(alarm->alarmID, sessionId.c_str());

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }

        TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );

        FUNCTION_EXIT;
    }


	void AlarmModificationHelper::postponePlanExecution(AlarmModifyItem* alarm)
    {
        FUNCTION_ENTRY("postponePlanExecution");

		std::string alarmID = alarm->alarmID.c_str();
		if (alarmID==TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING)
		{
			// Ignore and throw an exception
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the non-visible alarm type.") );
		}

        // Now find the alarm agent we are interested in
        unsigned long locationKey = alarm->locationKey;
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update alarm plan status");
            TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );
        }

        // Ok now tell the agent the plan status
        try
        {
			CORBA_CALL((*agent), planAssociatedWithAlarmWasPostponed, (alarm->alarmID.c_str(), sessionId.c_str()));
            //(*agent)->planAssociatedWithAlarmWasPostponed(alarm->alarmID, sessionId.c_str());

            FUNCTION_EXIT;
            return;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
        catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Alarm agent did not recognise the alarm uuid passed in");
            LOGMORE(SourceInfo, ex.reason.in());
        }
        catch(TA_Base_Core::OperationModeException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the plan status for the alarm on the alarm agent retrieved");
        }

        TA_THROW( NoAlarmAgentException("Could not set the plan status to postponed for the selected alarm.") );

        FUNCTION_EXIT;
    }

    bool AlarmModificationHelper::acknowledgeAlarmsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms,
                                                               unsigned long locationKey, const char * sessionId /*= NULL*/, bool isOperatorTriggered /*= false*/)    // Raymond Pau++ TD13367
    {
        FUNCTION_ENTRY("acknowledgeAlarmsForLocation");

        // Find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

		//TD14558
		//marvin++
		//set worst case corba timeout
		try
		{
			agent->setObjectTimeout(calculateCorbaTimeout(ACKNOWLEDGE_IN_BATCH_COUNT));
		}
		catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
		catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
		catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not set corba timeout");
        }
		//TD14558
		//++marvin



        // Raymond Pau++ TD13367
        std::string sessionID;
        if ( sessionId==NULL )
        {
            sessionID = RunParams::getInstance().get(RPARAM_SESSIONID);
        }
		else
		{
			sessionID = sessionId;
		}
        // ++Raymond Pau TD13367

        if (sessionID.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not acknowledge alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Acknowledge all the alarms in lots of 500
        bool wasSuccessful = true;
        int alarmCount = 0;
        std::vector<AlarmDetailCorbaDef*>::iterator iter = alarms.begin();

        do
        {
            // First pull out all alarms from the list that are at the specified location and add them
            // to the sequence that the alarm agent requires
            TA_Base_Core::StringSequence alarmIds;
            alarmIds.length(ACKNOWLEDGE_IN_BATCH_COUNT);

/* Haipeng Jiang commets this block
            alarmCount = 0;
            while( iter != alarms.end() && alarmCount < ACKNOWLEDGE_IN_BATCH_COUNT)
            {
                if( (*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = (*iter)->alarmID;
					
					//need to delete it. Haipeng added the below line
					delete *iter;
					//TD14343

                    *iter = NULL; // Set up ready to be removed
                    ++alarmCount;
                }
                ++iter;
            }
*/
			//Haipeng Jiang added the codes 
			for (alarmCount = 0; iter != alarms.end() && alarmCount < 500; )
			{
				std::string alarmID = (*iter)->alarmID.in();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = (*iter)->alarmID;
					
                    // owner suppose to clean up memory after use, cos we don't know if it's being used by caller after this
					// //need to delete it. Haipeng added the below line
					// delete *iter;
					// //TD14343
	
                    // *iter = NULL; // Set up ready to be removed
					// iter = alarms.erase(iter);
					++alarmCount;
                }
                ++iter;
			}
			//Haipeng Jiang added the codes 

            alarmIds.length(alarmCount);
        
            if (alarmCount != 0)
            {
                // Erase all those alarms we are processing now.
				//commets by Haipeng, wrongh algorithm below
                //alarms.erase( std::remove(alarms.begin(), alarms.end(), static_cast<AlarmDetailCorbaDef*>(NULL)), alarms.end() );


                // Ok now lets tell it to acknowledge those alarms.
                try
                {
					if (isOperatorTriggered)
					{
						CORBA_CALL((*agent), operatorAcknowledgeSeq, (alarmIds, sessionID.c_str()));
						//(*agent)->operatorAcknowledgeSeq(alarmIds, sessionID.c_str());
					}
					else
					{
						CORBA_CALL((*agent), acknowledgeSeq, (alarmIds, sessionID.c_str()));
						//(*agent)->acknowledgeSeq(alarmIds, sessionID.c_str());
					}
                }
                catch(const CORBA::SystemException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::ObjectResolutionException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AccessDeniedException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to acknowledge this alarm");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::OperationModeException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not acknowledge the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not acknowledge one or more of the alarms on the alarm agent retrieved");
                    LOGMORE(SourceInfo, ex.reason.in());
                    wasSuccessful = false;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not acknowledge the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
            }
        }
        while(alarmCount != 0);

        FUNCTION_EXIT;
        return wasSuccessful;
    }

	bool AlarmModificationHelper::acknowledgeAlarmsForLocation(std::vector<AlarmModifyItem*>& alarms,
															std::vector<std::string>& alarmsNoExist,
                                                               unsigned long locationKey, const char * sessionId /*= NULL*/, bool isOperatorTriggered /*= false*/)    // Raymond Pau++ TD13367
    {
        FUNCTION_ENTRY("acknowledgeAlarmsForLocation");

        // Find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

		//TD14558
		//marvin++
		//set worst case corba timeout
		try
		{
			agent->setObjectTimeout(calculateCorbaTimeout(ACKNOWLEDGE_IN_BATCH_COUNT));
		}
		catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
		catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
		catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not set corba timeout");
        }
		//TD14558
		//++marvin



        // Raymond Pau++ TD13367
        std::string sessionID;
        if ( sessionId==NULL )
        {
            sessionID = RunParams::getInstance().get(RPARAM_SESSIONID);
        }
		else
		{
			sessionID = sessionId;
		}
        // ++Raymond Pau TD13367

        if (sessionID.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not acknowledge alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Acknowledge all the alarms in lots of 500
        bool wasSuccessful = true;
        int alarmCount = 0;
        std::vector<AlarmModifyItem*>::iterator iter = alarms.begin();

        do
        {
            // First pull out all alarms from the list that are at the specified location and add them
            // to the sequence that the alarm agent requires
            TA_Base_Core::StringSequence alarmIds;
            alarmIds.length(ACKNOWLEDGE_IN_BATCH_COUNT);

/* Haipeng Jiang commets this block
            alarmCount = 0;
            while( iter != alarms.end() && alarmCount < ACKNOWLEDGE_IN_BATCH_COUNT)
            {
                if( (*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = (*iter)->alarmID;
					
					//need to delete it. Haipeng added the below line
					delete *iter;
					//TD14343

                    *iter = NULL; // Set up ready to be removed
                    ++alarmCount;
                }
                ++iter;
            }
*/
			//Haipeng Jiang added the codes 
			for (alarmCount = 0; iter != alarms.end() && alarmCount < 500; )
			{
				std::string alarmID = (*iter)->alarmID.c_str();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = CORBA::string_dup((*iter)->alarmID.c_str());
					
                    // owner suppose to clean up memory after use, cos we don't know if it's being used by caller after this
					// //need to delete it. Haipeng added the below line
					// delete *iter;
					// //TD14343
	
                    // *iter = NULL; // Set up ready to be removed
					// iter = alarms.erase(iter);
					++alarmCount;
                }
                ++iter;
			}
			//Haipeng Jiang added the codes 

            alarmIds.length(alarmCount);
        
            if (alarmCount != 0)
            {
                // Erase all those alarms we are processing now.
				//commets by Haipeng, wrongh algorithm below
                //alarms.erase( std::remove(alarms.begin(), alarms.end(), static_cast<AlarmDetailCorbaDef*>(NULL)), alarms.end() );


                // Ok now lets tell it to acknowledge those alarms.
                try
                {
					if (isOperatorTriggered)
					{
						//CORBA_CALL((*agent), operatorAcknowledgeSeqEx, );
						TA_Base_Core::StringSequence* alarmList=NULL;
						CORBA_CALL_RETURN( alarmList,
							(*agent),
							operatorAcknowledgeSeqEx,
			                   (alarmIds, sessionID.c_str()) );
						for (int i=0;i<alarmList->length();i++)
						{
							alarmsNoExist.push_back((*alarmList)[i].in());
						}
					}
					else
					{
						CORBA_CALL((*agent), acknowledgeSeq, (alarmIds, sessionID.c_str()));
						//(*agent)->acknowledgeSeq(alarmIds, sessionID.c_str());
					}
                }
                catch(const CORBA::SystemException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::ObjectResolutionException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AccessDeniedException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to acknowledge this alarm");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::OperationModeException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not acknowledge the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not acknowledge one or more of the alarms on the alarm agent retrieved");
                    LOGMORE(SourceInfo, ex.reason.in());
                    wasSuccessful = false;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not acknowledge the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
            }
        }
        while(alarmCount != 0);

        FUNCTION_EXIT;
        return wasSuccessful;
    }
	//TD14558
	//marvin++
	unsigned short AlarmModificationHelper::calculateCorbaTimeout(unsigned int ulNumAlarms)
	{
		unsigned short ulDefaultCorbaTimeOutInSec = 20;
		unsigned short ulDefaultCorbaTimeoutUpperLimit = 300;
		//Lan usage worst case is at 5%
		//   2MB/  1024KB/ = 2048KB/sec *.05 = 102.4KB/sec
		//   1sec *    1MB
		const unsigned int ulMaximumLanRateInKBPerSec = 102;

		// Based on log files with omnilog an alarm is approx this big
		const unsigned int ulApproxSizeOfAlarmPerPacketInKB = 24;
		const unsigned int ulProccessingTimePaddingInSec = 50; 

		unsigned int ulTotalSizeOfAlarmInKB =0;
		unsigned short ulTotalNumberOfSecondsToProcessAlarms =0;

		ulTotalSizeOfAlarmInKB = ulApproxSizeOfAlarmPerPacketInKB * ulNumAlarms;
		ulTotalNumberOfSecondsToProcessAlarms = ulTotalSizeOfAlarmInKB / ulMaximumLanRateInKBPerSec;

		if (ulTotalNumberOfSecondsToProcessAlarms < 20)
		{
			//return the minimum number of seconds if the result is less than 20
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Calculated default %d sec corba timeout for %d alarms",ulDefaultCorbaTimeOutInSec,ulNumAlarms);
			return ulDefaultCorbaTimeOutInSec;
		}
		else
		{
			// take into consideration processing time
			ulTotalNumberOfSecondsToProcessAlarms+=ulProccessingTimePaddingInSec;
			if(ulTotalNumberOfSecondsToProcessAlarms > 300)
			{
				//return upper limit, warning the number of alarms that can be processed is above corba timeout limit. this transaction would be prone to corba timeouts
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Above corba timeout limit. Using uper limit of %d sec corba timeout for %d alarms. Corba timeout may occur",ulDefaultCorbaTimeoutUpperLimit,ulTotalNumberOfSecondsToProcessAlarms);
				return ulDefaultCorbaTimeoutUpperLimit;
			}
			else
			{
				//return the computed number of seconds
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Calculated %d sec corba timeout for %d alarms",ulTotalNumberOfSecondsToProcessAlarms,ulTotalNumberOfSecondsToProcessAlarms);
				return ulTotalNumberOfSecondsToProcessAlarms;
			}
		}
	}
	//TD14558
	//++marvin


    bool AlarmModificationHelper::acknowledgeAndCloseAlarmsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms,
                                                                       unsigned long locationKey, const char * sessionId /*= NULL*/)   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("acknowledgeAndCloseAlarmsForLocation");

        // Find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);

        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

		//TD14558
		//marvin++
		//set worst case corba timeout
		try
		{
			agent->setObjectTimeout(calculateCorbaTimeout(ACKNOWLEDGE_IN_BATCH_COUNT));
		}
		catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
		catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
		catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not set corba timeout");
        }
		//TD14558
		//++marvin

        // Raymond Pau++ TD13367
        std::string sessionID;
        if ( sessionId==NULL )
        {
            sessionID = RunParams::getInstance().get(RPARAM_SESSIONID);
        }
		else
		{
			sessionID = sessionId;
		}
        // ++Raymond Pau TD13367

        if (sessionID.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not acknowledge and close alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Acknowledge all the alarms in lots of 500
        bool wasSuccessful = true;
        int alarmCount = 0;
        std::vector<AlarmDetailCorbaDef*>::iterator iter = alarms.begin();

        do
        {
            // First pull out all alarms from the list that are at the specified location and add them
            // to the sequence that the alarm agent requires
            TA_Base_Core::StringSequence alarmIds;
            TA_Base_Core::StringSequence alarmIdsForAcking;
			TA_Base_Core::StringSequence alarmCloseValues;
			TA_Base_Core::StringSequence alarmCloseTimes;
            alarmIds.length(ACKNOWLEDGE_IN_BATCH_COUNT);
            alarmIdsForAcking.length(ACKNOWLEDGE_IN_BATCH_COUNT);
			// no need to obtain the close time and value
			// get the close time from the alarm close time and value from parameter

            alarmCount = 0;
            int alarmForAckingCount = 0;
            while( iter != alarms.end() && alarmCount < ACKNOWLEDGE_IN_BATCH_COUNT)
            {
				std::string alarmID = (*iter)->alarmID.in();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = (*iter)->alarmID;
                    ++alarmCount;

                    std::string ackedBy = (*iter)->alarmAcknowledgedBy.in();
                    if ( ackedBy.empty() )
                    {
                        alarmIdsForAcking[alarmForAckingCount] = (*iter)->alarmID;
                        ++alarmForAckingCount;
                    }
                    // *iter = NULL; // Set up ready to be removed
                }
                ++iter;
            }
            alarmIds.length(alarmCount);
            alarmIdsForAcking.length(alarmForAckingCount);
        
            if (alarmCount != 0)
            {
                // // Erase all those alarms we are processing now.
                // alarms.erase( std::remove(alarms.begin(), alarms.end(), static_cast<AlarmDetailCorbaDef*>(NULL)), alarms.end() );

                // Ok now lets tell it to acknowledge those alarms.
                if (alarmForAckingCount != 0)
                {
                    try
                    {
						CORBA_CALL((*agent), operatorAcknowledgeSeq, (alarmIdsForAcking, sessionID.c_str()));
                        //(*agent)->operatorAcknowledgeSeq(alarmIdsForAcking, sessionID.c_str());
                    }
                    catch(const CORBA::SystemException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::ObjectResolutionException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::AccessDeniedException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to acknowledge this alarm");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::OperationModeException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not acknowledge the alarm on the alarm agent retrieved");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not acknowledge one or more of the alarms on the alarm agent retrieved");
                        LOGMORE(SourceInfo, ex.reason.in());
                        wasSuccessful = false;
                    }
                    catch(...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not acknowledge the alarm on the alarm agent retrieved");
                        wasSuccessful = false;
                    }
                }

                // Now try closing them
                try
                {
					CORBA_CALL((*agent), operatorCloseAlarmSequence, (alarmIds, sessionID.c_str() ));
                    //(*agent)->operatorCloseAlarmSequence(alarmIds, sessionID.c_str() );
                }
                catch(const CORBA::SystemException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::ObjectResolutionException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AccessDeniedException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to close this alarm");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::OperationModeException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not close the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not close one or more of the alarms on the alarm agent retrieved");
                    LOGMORE(SourceInfo, ex.reason.in());
                    wasSuccessful = false;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not close the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }

            }
        }
        while(alarmCount != 0);

        FUNCTION_EXIT;
        return wasSuccessful;
    }
    
    bool AlarmModificationHelper::acknowledgeAndCloseAlarmsForLocation(std::vector<AlarmModifyItem*>& alarms,
		std::vector<std::string>&  alarmsNoExist,
                                                                       unsigned long locationKey, const char * sessionId /*= NULL*/)   // Raymond Pau++ TD13367 
    {
        FUNCTION_ENTRY("acknowledgeAndCloseAlarmsForLocation");

        // Find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);

        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

		//TD14558
		//marvin++
		//set worst case corba timeout
		try
		{
			agent->setObjectTimeout(calculateCorbaTimeout(ACKNOWLEDGE_IN_BATCH_COUNT));
		}
		catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
		catch(TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
        }
		catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not set corba timeout");
        }
		//TD14558
		//++marvin

        // Raymond Pau++ TD13367
        std::string sessionID;
        if ( sessionId==NULL )
        {
            sessionID = RunParams::getInstance().get(RPARAM_SESSIONID);
        }
		else
		{
			sessionID = sessionId;
		}
        // ++Raymond Pau TD13367

        if (sessionID.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not acknowledge and close alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Acknowledge all the alarms in lots of 500
        bool wasSuccessful = true;
        int alarmCount = 0;
        std::vector<AlarmModifyItem*>::iterator iter = alarms.begin();

        do
        {
            // First pull out all alarms from the list that are at the specified location and add them
            // to the sequence that the alarm agent requires
            TA_Base_Core::StringSequence alarmIds;
            TA_Base_Core::StringSequence alarmIdsForAcking;
			TA_Base_Core::StringSequence alarmCloseValues;
			TA_Base_Core::StringSequence alarmCloseTimes;
            alarmIds.length(ACKNOWLEDGE_IN_BATCH_COUNT);
            alarmIdsForAcking.length(ACKNOWLEDGE_IN_BATCH_COUNT);
			// no need to obtain the close time and value
			// get the close time from the alarm close time and value from parameter

            alarmCount = 0;
            int alarmForAckingCount = 0;
            while( iter != alarms.end() && alarmCount < ACKNOWLEDGE_IN_BATCH_COUNT)
            {
				std::string alarmID = (*iter)->alarmID.c_str();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*iter)->locationKey == locationKey)
                {
                    alarmIds[alarmCount] = CORBA::string_dup((*iter)->alarmID.c_str());
                    ++alarmCount;

                    std::string ackedBy = (*iter)->alarmAcknowledgedBy.c_str();
                    if ( ackedBy.empty() )
                    {
                        alarmIdsForAcking[alarmForAckingCount] = CORBA::string_dup((*iter)->alarmID.c_str());
                        ++alarmForAckingCount;
                    }
                    // *iter = NULL; // Set up ready to be removed
                }
                ++iter;
            }
            alarmIds.length(alarmCount);
            alarmIdsForAcking.length(alarmForAckingCount);
        
            if (alarmCount != 0)
            {
                // // Erase all those alarms we are processing now.
                // alarms.erase( std::remove(alarms.begin(), alarms.end(), static_cast<AlarmDetailCorbaDef*>(NULL)), alarms.end() );

                // Ok now lets tell it to acknowledge those alarms.
                if (alarmForAckingCount != 0)
                {
                    try
                    {
						TA_Base_Core::StringSequence* alarmList=NULL;
						CORBA_CALL_RETURN( alarmList,
							(*agent),
							operatorAcknowledgeSeqEx,
							(alarmIdsForAcking, sessionID.c_str())  );
						for (int i=0;i<alarmList->length();i++)
						{
							alarmsNoExist.push_back((*alarmList)[i].in());
						}
						
						//CORBA_CALL((*agent), operatorAcknowledgeSeq, (alarmIdsForAcking, sessionID.c_str()));
                       
                    }
                    catch(const CORBA::SystemException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::ObjectResolutionException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::AccessDeniedException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to acknowledge this alarm");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::OperationModeException&)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not acknowledge the alarm on the alarm agent retrieved");
                        wasSuccessful = false;
                    }
                    catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not acknowledge one or more of the alarms on the alarm agent retrieved");
                        LOGMORE(SourceInfo, ex.reason.in());
                        wasSuccessful = false;
                    }
                    catch(...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not acknowledge the alarm on the alarm agent retrieved");
                        wasSuccessful = false;
                    }
                }

                // Now try closing them
                try
                {
					TA_Base_Core::StringSequence* alarmList=NULL;
					CORBA_CALL_RETURN( alarmList,
						(*agent),
						operatorCloseAlarmSequenceEx,
						(alarmIds, sessionID.c_str())  );
					for (int i=0;i<alarmList->length();i++)
					{
						alarmsNoExist.push_back((*alarmList)[i].in());
					}
					
					//CORBA_CALL((*agent), operatorCloseAlarmSequence, (alarmIds, sessionID.c_str() ));
                    

                }
                catch(const CORBA::SystemException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::ObjectResolutionException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AccessDeniedException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to close this alarm");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::OperationModeException&)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not close the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }
                catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not close one or more of the alarms on the alarm agent retrieved");
                    LOGMORE(SourceInfo, ex.reason.in());
                    wasSuccessful = false;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not close the alarm on the alarm agent retrieved");
                    wasSuccessful = false;
                }

            }
        }
        while(alarmCount != 0);

        FUNCTION_EXIT;
        return wasSuccessful;
    }

    bool AlarmModificationHelper::updateCommentsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms,
                                                            unsigned long locationKey,
                                                            const std::string& comment)
    {
        FUNCTION_ENTRY("updateCommentsForLocation");

        // Now find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update comment on alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Pull out all alarms from the list that are at the specified location and update comments as we go
        bool hasErrorOccurred = false;

        for ( std::vector<AlarmDetailCorbaDef*>::iterator itr=alarms.begin(); itr!=alarms.end(); ++itr )
        {
            if (*itr != NULL)
			{
				std::string alarmID = (*itr)->alarmID.in();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*itr)->locationKey == locationKey)
				{
					// Ok now lets tell it to update the comments on those alarms.
					try
					{
						CORBA_CALL((*agent), updateComments, (comment.c_str(), (*itr)->alarmID, sessionId.c_str()));
						//(*agent)->updateComments(comment.c_str(), (*itr)->alarmID, sessionId.c_str());

						std::stringstream ss;
						ss << "Updated comment for alarm ID:" << (*itr)->alarmID;
						ss << " for location ID: " << (*itr)->locationKey;
						ss << " of comment = " << comment.c_str();
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str() );
					}
					catch(const CORBA::SystemException& ex)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::ObjectResolutionException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::AccessDeniedException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to update the comment on this alarm");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::OperationModeException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the comment for the alarm on the alarm agent retrieved");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not update the comment for the alarm on the alarm agent retrieved");
						LOGMORE(SourceInfo, ex.reason.in());
						hasErrorOccurred = true;
					}
					catch(...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the comment for the alarm on the alarm agent retrieved");
						hasErrorOccurred = true;
					}
				}// end of if (*itr)->locationKey
			} // end of if *itr != null
		}// end of for loop

        FUNCTION_EXIT;
        return !hasErrorOccurred;
    }

	bool AlarmModificationHelper::updateCommentsForLocation(std::vector<AlarmModifyItem*>& alarms,
                                                            unsigned long locationKey,
                                                            const std::string& comment)
    {
        FUNCTION_ENTRY("updateCommentsForLocation");

        // Now find the alarm agent we are interested in
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            FUNCTION_EXIT;
            return false;
        }

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        if (sessionId.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session Id was not set so could not update comment on alarms");
            FUNCTION_EXIT;
            return false;
        }

        // Pull out all alarms from the list that are at the specified location and update comments as we go
        bool hasErrorOccurred = false;

        for ( std::vector<AlarmModifyItem*>::iterator itr=alarms.begin(); itr!=alarms.end(); ++itr )
        {
            if (*itr != NULL)
			{
				std::string alarmID = (*itr)->alarmID.c_str();
                if( alarmID!=TA_Base_Core::NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING && 
					(*itr)->locationKey == locationKey)
				{
					// Ok now lets tell it to update the comments on those alarms.
					try
					{
						CORBA_CALL((*agent), updateComments, (comment.c_str(), (*itr)->alarmID.c_str(), sessionId.c_str()));
						//(*agent)->updateComments(comment.c_str(), (*itr)->alarmID, sessionId.c_str());

						std::stringstream ss;
						ss << "Updated comment for alarm ID:" << (*itr)->alarmID;
						ss << " for location ID: " << (*itr)->locationKey;
						ss << " of comment = " << comment.c_str();
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str() );
					}
					catch(const CORBA::SystemException& ex)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::ObjectResolutionException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::AccessDeniedException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", "Did not have permission to update the comment on this alarm");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::OperationModeException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Could not update the comment for the alarm on the alarm agent retrieved");
						hasErrorOccurred = true;
					}
					catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "AlarmException", "Could not update the comment for the alarm on the alarm agent retrieved");
						LOGMORE(SourceInfo, ex.reason.in());
						hasErrorOccurred = true;
					}
					catch(...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not update the comment for the alarm on the alarm agent retrieved");
						hasErrorOccurred = true;
					}
				}// end of if (*itr)->locationKey
			} // end of if *itr != null
		}// end of for loop

        FUNCTION_EXIT;
        return !hasErrorOccurred;
    }

}; // namespace TA_Base_Core
