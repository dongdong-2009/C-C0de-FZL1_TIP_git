///////////////////////////////////////////////////////////
//  MMSSubmitterDatabase.cpp
//  Implementation of the Class MMSSubmitterDatabase
//  Created on:      16-¾ÅÔÂ-2011 16:52:28
//  Original author: huirong.luo
///////////////////////////////////////////////////////////
// #include <sstream>
// #include <string>
// #include <iomanip> 
// #include <memory>
//#include "boost/smart_ptr.hpp"
#include "MMSGlobalInhibitionManager.h"
#include "MMSUtility.h"
#include "MMSSubmitterDatabase.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmConfig.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/NoAlarmAgentException.h"




using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	MMSSubmitterDatabase::MMSSubmitterDatabase():m_needInitSubmitter(true)
	{
		 m_rightsLibrary = NULL;
	}

	MMSSubmitterDatabase::~MMSSubmitterDatabase()
	{

	}

	bool MMSSubmitterDatabase::submitAutoAlarmMessage(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{		 
		return MMSUtility::saveMmsRequestAlarm(alarm);
	}

	 bool  MMSSubmitterDatabase::initJobRequestSubmmit()
	 {
		 if (m_needInitSubmitter==true)
		 {
			 try
			 {
				 //resolve the submitter object
				 m_jobRequestSubmmit.setEntityName("MmsJobRequestSubmitter_occ",false);
				 m_needInitSubmitter=false;

			 }
			 catch(...)
			 {
				 LOG0 (SourceInfo, DebugUtil::DebugError, "cannot init MmsJobRequestSubmitter_occ");
				 return false;
			 }	
		 }
		 return true;
	 }

	bool MMSSubmitterDatabase::submitJobRequest(JobRequestDetailCorbaDef& jobRequest, std::string sessionID)
	{
	 
		FUNCTION_ENTRY("submitJobRequest"); 

		bool isJobRequestPermitted;
		try
		{
			isJobRequestPermitted = isRaiseJobRequestPermitted(sessionID, jobRequest.dataNodeKey);
		}
		catch(TA_Base_Core::AccessDeniedException&)
		{
			throw TA_Base_Bus::SubmissionDenied("Rights could not be determined for this action.",true,TA_Base_Bus::SUBMISSIONDENIED_NO_PRIVILEGE);
		}

		if(!isJobRequestPermitted)
		{
			std::string message = "Operator does not have sufficient privileges to raise Service Request.";
			throw TA_Base_Bus::SubmissionDenied(message.c_str(), true,TA_Base_Bus::SUBMISSIONDENIED_NO_PRIVILEGE);
		}

		//if this is being submitted from an alarm...
		if( 0 != strcmp("NOT_FROM_ALARM", jobRequest.alarmUID.in()) )
		{
			//check if the alarm state is ok
			std::string reason;
			TA_Base_Bus::ESubmissionDeniedError eType;
			if(false == alarmStateIsOk(jobRequest, reason,eType) )
			{				
				throw TA_Base_Bus::SubmissionDenied(reason.c_str(), false, eType);
			}

			//check if the MMS inhibit state is ok
			bool isInhibited = true;
			try
			{
				isInhibited = TA_Base_Bus::ScadaUtilities::getInstance().isDataPointMmsInhibited(jobRequest.dataPointKey);
			}
			catch(...)
			{
				throw TA_Base_Bus::SubmissionDenied("Could not determine the MMS inhibition state.", false, TA_Base_Bus::SUBMISSIONDENIED_NO_DETERMINE_INHIBITION_STATE);
			}
			if( isInhibited )
			{	

				throw TA_Base_Bus::SubmissionDenied("The datapoint associated with the alarm is MMS inhibited.", false,TA_Base_Bus::SUBMISSIONDENIED_DATAPOINT_MMS_INHIBITED);
			}

		}
		else //not from alarm so just check global inhibit state
		{
			bool isGlobalInhibit = true;
			try
			{
				isGlobalInhibit=MMSGlobalInhibitionManager::getInstance().getGlobalMmsInhibit();
			}
			catch( const TA_Base_Core::ObjectResolutionException& ore ) // when corba call MMsAgent failed.
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"MmsAgent can't be connected.");
				isGlobalInhibit = false;
			}
			catch ( const CORBA::SystemException& se )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"MmsAgent can't be connected.");
				isGlobalInhibit = false;
			}
			if( isGlobalInhibit )  //TODO: MmsServerState::getInstance()->getGlobalInhibit()
			{
				throw TA_Base_Bus::SubmissionDenied("Global MMS inhibition is applied.", false,TA_Base_Bus::SUBMISSIONDENIED_GLOBAL_MMS_INHIBITED);
			}
		}

		try
		{
			//MmsConnection::getInstance()->submitJobRequestMessage(jobRequest);
			bool ret=MMSUtility::submitJobRequest(jobRequest);
			if (ret==false)
			{
				std::string msg = "MMS Server communication failure";
				throw TA_Base_Bus::DependencyFailure(msg.c_str(),TA_Base_Bus::DEPENDENCYFAILURE_MMS_SERVER_NO_CONTACTED);
			}
		}
		catch(...)
		{		 
			std::string msg = "MMS Server communication failure";
			throw TA_Base_Bus::DependencyFailure(msg.c_str(),TA_Base_Bus::DEPENDENCYFAILURE_MMS_SERVER_NO_CONTACTED);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"submitJobRequestMessage was successful");
 
		//if this is being submitted from an alarm...
		if( 0 != strcmp("NOT_FROM_ALARM", jobRequest.alarmUID.in()) )
		{
			try //try and set the state of the alarm to show that a JR has been raised on it
			{

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"next call is AlarmModificationHelper::setJobRequestSubmitted()");
                TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().setJobRequestSubmitted(
                    jobRequest.alarmUID.in(), jobRequest.locationKey, sessionID.c_str());
                // acknowledge the alarm when mms status update successfully.
				//TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().setJobRequestSubmitted(
				//	jobRequest.alarmUID.in(), jobRequest.locationKey);
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"next call is AlarmModificationHelper::setJobRequestSubmitted()");

				//set the state of the alarm to show that a JR has been raised on it
				std::string reason = "The Service Request submission was successful but the associated alarm could not be updated.";

				throw TA_Base_Bus::SubmissionDenied(reason.c_str(),false,TA_Base_Bus::SUBMISSIONDENIED_ALARM_ALREADY_RAISED);
			}
		}
		else //the Job Request was not from an alarm... nothing to do but log
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Service Request was not submitted via an alarm");
		}
		return true;
	}
	 bool MMSSubmitterDatabase::isRaiseJobRequestPermitted(const std::string& sessionID, unsigned long dataNodeKey)
	 {
		 
		 try
		 {
			 TA_Base_Core::IResource* r = 
				 TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( dataNodeKey );
			 unsigned long resourceID = r->getKey();
			 delete r;

			 unsigned long actionKey = TA_Base_Bus::aca_DATA_NODE_RAISE_JOB_REQUEST;
			 TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
			 std::string reason;
			 {			 
				 TA_Base_Core::ThreadGuard guard(m_lockRightLibrary);
				 if (m_rightsLibrary==NULL) 
				 {
					 TA_Base_Bus::RightsLibraryFactory rlFactory;
					 m_rightsLibrary = rlFactory.buildRightsLibrary();
				 }

			 }

			 bool response = m_rightsLibrary->isActionPermittedOnResource( sessionID, resourceID, 
				 actionKey, reason, decisionModule );

		 
			 return response;
		 }
		 catch(...)
		 {
			 throw TA_Base_Core::AccessDeniedException("Rights could not be determined for raising service requests.");
		 }
		 return false;
	 }

	void MMSSubmitterDatabase::loadOperators()
	{
		FUNCTION_ENTRY("loadOperators");

		// Delete any existing operators
		for(std::map<unsigned long, TA_Base_Core::IOperator*>::iterator opToDelete = m_operators.begin(); opToDelete != m_operators.end(); ++opToDelete)
		{
			if (opToDelete->second != NULL)
			{
				delete opToDelete->second;
				opToDelete->second = NULL;
			}
		}
		m_operators.clear();

		// NOw load operators fresh from db
		std::vector<TA_Base_Core::IOperator*> operators;
		try
		{
			operators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperators();
			for(std::vector<TA_Base_Core::IOperator*>::iterator iter = operators.begin(); iter != operators.end(); ++iter)
			{
				m_operators[(*iter)->getKey()] = *iter;
			}
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load operators from the database");

			// Clean up and delete any locations that may have been retrieved
			for(std::vector<TA_Base_Core::IOperator*>::iterator iter = operators.begin(); iter != operators.end(); ++iter)
			{
				if (*iter != NULL)
				{
					delete *iter;
					*iter = NULL;
				}
			}
		}

		FUNCTION_EXIT;
	}

	std::string MMSSubmitterDatabase::getOperatorName (unsigned long key)
	{
		if (m_operators.empty())
		{
			loadOperators();
		}

		std::map<unsigned long, TA_Base_Core::IOperator*>::iterator matching = m_operators.find(key);
		if (matching == m_operators.end())
		{
			// Not found
			return "";
		}

		try
		{
			return matching->second->getDescription();
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve operator name");
		}

		return "";
	}

	std::string MMSSubmitterDatabase::getOperatorDesc(const std::string& sessionID)
	{
		FUNCTION_ENTRY("getOperatorDesc");

		std::string name = "";

		try
		{
			TA_Base_Bus::AuthenticationLibrary lib;
			TA_Base_Bus::SessionInfo si = lib.getSessionInfo(sessionID, sessionID);
			std::string fullname = getOperatorName(si.UserId);
			name.assign(fullname, 0, 20);
		}
		catch(...)
		{
			name = "";
		}

		if(name.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The operator details could not be retrieved.");
			throw TA_Base_Bus::DependencyFailure("The operator details could not be retrieved.",TA_Base_Bus::DEPENDENCYFAILURE_NO_OPERATOR_DETAIL);
		}

		FUNCTION_EXIT;
		return name;
	}

	
	JobRequestDetailCorbaDef* MMSSubmitterDatabase::getDataFromAlarm(std::string alarmUID,unsigned long alarmLocationKey, std::string sessionID)
	{
		FUNCTION_ENTRY("getDataFromAlarm");

		TA_Base_Core::ThreadGuard guard( m_lockSubmitter);

		if (initJobRequestSubmmit()==false) return NULL;

		TA_Base_Core::AlarmDetailCorbaDef_var alarm;

		std::string errorMessage = "NO_ERROR";
		TA_Base_Bus::EDependencyFailureError eType;

		try //try and get the alarm from an alarm agent
		{
			alarm = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().getAlarmAtLocation(alarmUID, alarmLocationKey);
		}
		catch(TA_Base_Core::NoAlarmAgentException)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException)
		{
			errorMessage = "The specified alarm could not be retrieved from the Alarm Agent";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM;
		}
		catch(...)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}

		if("NO_ERROR" != errorMessage)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),eType);
		}

		TA_Base_Bus::JobRequestDetailCorbaDef* details = new TA_Base_Bus::JobRequestDetailCorbaDef();

		details->assetId =CORBA::string_dup( alarm->assetName);//string assetId;

		//details->dataNodeKey = MmsMessageUtil::getInstance()->getNodeKey(alarm->assocEntityKey);

		TA_Base_Core::IEntityData* entityData = NULL;
        try
        {
            entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(alarm->assocEntityKey);
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when get entity" );
        }

        TA_Base_Core::DataPointEntityData* pDpData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData);

		if( NULL != pDpData )
        {
            details->dataNodeKey = pDpData->getParent();
            // Will be deleted in entityData
            pDpData = NULL;
		}
		else if ( NULL != entityData )
		{
            details->dataNodeKey = entityData->getKey();
		}
        else
        {
            details->dataNodeKey = 0u;
        }

        if ( NULL != entityData )
        {
            std::string entityName = entityData->getName();
            std::string faultcode = TA_Base_Bus::MMSUtility::getAlarmFaultCode(entityName);
            details->alarmFaultCode = CORBA::string_dup(faultcode.c_str());

            delete entityData;
            entityData = NULL;
        }
        else
        {
            details->alarmFaultCode = CORBA::string_dup("");
        }

		details->requestor = CORBA::string_dup(getOperatorDesc(sessionID).c_str());//string requestor;
		details->targetDateEnabled  = CORBA::Boolean(true);//boolean targetDateEnabled;

		time_t now;
		time(&now);
		details->targetDate = now + (24*60*60);//TA_Base_Core::DateTime targetDate;

		std::string alarmDesc = "";
		std::string probReport = "";

		/*MmsConnection::getInstance()->getDescAndReportFields(alarm.in(), alarmDesc, probReport);*/
		getDescAndReportFields(alarm.in(), alarmDesc, probReport);

		details->alarmDescField = CORBA::string_dup(alarmDesc.c_str());//string alarmDescField;
		details->problemReport  = CORBA::string_dup(probReport.c_str());//string problemReport;

		//the following values are populated by the agent
		//and should not be modified, or even used by the client:

		//note that the assetId above is editable and may not
		//coorospond to this entity
		details->dataPointKey = alarm->assocEntityKey;//unsigned long entityKey;
		//these values are used if the JRM was launched via an alarm
		details->alarmUID = alarm->alarmID;//string alarmUID;
		details->locationKey = alarmLocationKey;//unsigned long locationKey;
		details->alarmTime =alarm->sourceTime.time;
		details->isAutoTrigger=false;
		details->alarmSeverity=alarm->alarmSeverity;
		details->alarmValue= CORBA::string_dup(alarm->alarmValue);

		FUNCTION_EXIT;
		return details;
	}

	std::string MMSSubmitterDatabase::getAlarmSeverityNameInCache(unsigned long key)
	{
		if (m_alarmSeverities.empty())
		{
			// Now load alarm severities fresh from db
			std::vector<TA_Base_Core::IAlarmSeverityData*> severities;
			try
			{
				severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
				for(std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = severities.begin(); iter != severities.end(); ++iter)
				{
					m_alarmSeverities[(*iter)->getKey()] = *iter;
				}
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load alarm severities from the database");

				// Clean up and delete any locations that may have been retrieved
				for(std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = severities.begin(); iter != severities.end(); ++iter)
				{
					if (*iter != NULL)
					{
						delete *iter;
						*iter = NULL;
					}
				}
			}
		}

		std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*>::iterator matching = m_alarmSeverities.find(key);
		if (matching == m_alarmSeverities.end())
		{
			// Not found
			return "";
		}

		try
		{
			return matching->second->getName();
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve alarm severity name");
		}

		return "";
	}


	std::string MMSSubmitterDatabase::getAlarmSeverityName(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm)
	{
		FUNCTION_ENTRY("getAlarmSeverityName");

		std::string severity;
		try
		{
			severity = getAlarmSeverityNameInCache(p_alarm.alarmSeverity);
		}
		catch(...)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"m_dbCache.getAlarmSeverityName() threw an exception");

			severity = "";
		}

		FUNCTION_EXIT;
		return severity;

	}

	std::string MMSSubmitterDatabase::toDataPointValue(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm)
    {
		FUNCTION_ENTRY("toDataPointValue");

		//Xinsong ++
		FUNCTION_EXIT;
		return p_alarm.alarmValue.in();        
    }

		std::string MMSSubmitterDatabase::createMmsAlarmDesc( 
			const time_t alarmOccurence,
			const std::string& alarmSeverity,
			const std::string& alarmDesc,
			const std::string& dataPointValue,
			bool truncate /*= true*/)
		{
			FUNCTION_ENTRY("createMmsAlarmDesc");

			// ECP 257 - append the time of the alarm occurence
			// std::string alarmMessage = alarmSeverity + ", " + alarmDesc  + ", " + dataPointValue;
			//std::string strAlarmTime = timeToString(alarmOccurence);		
			//C955 new requirement	 
			//Note that the <Alarm Description> for alarms on datapoints currently has the following format: 
			//<Equipment Description> <Datapoint Description> <Equipment Location> <Datapoint Value>
			std::string alarmMessage = alarmDesc  + ", " + dataPointValue;	

			FUNCTION_EXIT;
			return alarmMessage;
		}

	void MMSSubmitterDatabase::getDescAndReportFields(const TA_Base_Core::AlarmDetailCorbaDef& alarm,
		std::string& alarmDescField, std::string& problemReportField)
	{
		FUNCTION_ENTRY("getDescAndReportFields");

		std::string alarmSeverity = getAlarmSeverityName(alarm);
		if(alarmSeverity.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "failed to retrieve severity name");
		}

		std::string dataPointValue = toDataPointValue(alarm);
		if(dataPointValue.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "failed to data point value description");
		}

		// ECP257 - added alarm source time when creating for the MmsAlarm description
		std::string alarmDesc = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm.messageTypeKey,  alarm.alarmParameterList.in());
		alarmDescField = createMmsAlarmDesc(alarm.sourceTime.time , alarmSeverity, alarmDesc, dataPointValue, false);

		problemReportField = "";
		FUNCTION_EXIT;
	}


	JobRequestDetailCorbaDef* MMSSubmitterDatabase::getDataFromSymbol(std::string entityName,  std::string sessionID)
	{
		FUNCTION_ENTRY("getDataFromSymbol");
		
		TA_Base_Core::ThreadGuard guard( m_lockSubmitter);
		JobRequestDetailCorbaDef*  ret=NULL;
		if (initJobRequestSubmmit()==false) return NULL;

		TA_Base_Bus::JobRequestDetailCorbaDef* details = new TA_Base_Bus::JobRequestDetailCorbaDef();

		try
		{
            TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
            if ( NULL == entityData  )
            {
                throw "bummer";
            }

            TA_Base_Core::DataNodeEntityData* pDnData  = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);
            if ( NULL == pDnData  )
            {
                delete entityData;
                entityData = NULL;
                throw "bummer";
            }

            details->assetId = CORBA::string_dup(pDnData->getAssetName().c_str());

            //note that the assetId above is editable and may not
            //coorospond to this entity
            details->dataNodeKey = pDnData->getKey();//unsigned long entityKey;

            delete pDnData;
            pDnData = NULL;
		}
		catch(...)
		{
			std::string errorMessage = "Configuration for the specified datapoint could not be retrieved.";
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_DATAPOINT);
		}


		details->requestor = CORBA::string_dup(getOperatorDesc(sessionID).c_str());//string requestor;

		details->targetDateEnabled  = CORBA::Boolean(true);//boolean targetDateEnabled;

		time_t now;
		time(&now);
		details->targetDate = now + (24*60*60);//TA_Base_Core::DateTime targetDate;

		details->alarmDescField = CORBA::string_dup("");//string alarmDescField;

		details->problemReport  = CORBA::string_dup("");//string problemReport;

		//these values are used only if the JRM was launched via an alarm
		details->alarmUID = CORBA::string_dup("NOT_FROM_ALARM");//string alarmUID;
		// AZ++
		details->locationKey = 0;//unsigned long locationKey;
		details->dataPointKey = 0;
		details->isAutoTrigger=false;
		details->alarmSeverity=0;
		details->alarmFaultCode= CORBA::string_dup("");
		details->alarmValue= CORBA::string_dup("");


		FUNCTION_EXIT;

		return details;

	}
	bool MMSSubmitterDatabase::alarmStateIsOk(const TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest, 
		std::string& reason, TA_Base_Bus::ESubmissionDeniedError& eType)
	{
		FUNCTION_ENTRY("alarmStateIsOk");

		//get the alarm from the alarm agent
		std::string alarmUID = jobRequest.alarmUID.in();
		unsigned long alarmLocationKey = jobRequest.locationKey;
		TA_Base_Core::AlarmDetailCorbaDef_var alarm;
		TA_Base_Bus::EDependencyFailureError eErrorType;
		std::string errorMessage = "NO_ERROR";
		try //try and get the alarm from an alarm agent
		{
			alarm = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().getAlarmAtLocation(alarmUID, alarmLocationKey);
		}
		catch(TA_Base_Core::NoAlarmAgentException)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException)
		{
			errorMessage = "The specified alarm could not be retrieved from the Alarm Agent";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM;
		}
		catch(...)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}

		if("NO_ERROR" != errorMessage)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),eErrorType);
		}

		//so now we have the alarm, so see if the state is OK
		if( TA_Base_Core::MMS_JR_SENT == alarm->mmsState)
		{
			reason = "The Service Request is already submitted.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_ALREADY_RAISED;
			FUNCTION_EXIT;
			return false;
		}

		// marvin++
		// bug 640 (TD10258)
		if(TA_Base_Core::AlarmClosed == alarm->state)
		{
			reason = "The alarm is normalized.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_NORMALIZED;
			FUNCTION_EXIT;
			return false;
		}

		if(TA_Base_Core::AlarmSuppressed == alarm->state)
		{
			reason = "The alarm is suppressed.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_SUPPRESSED;
			FUNCTION_EXIT;
			return false;
		}
		// ++marvin
		// bug 640 (TD10258)

		FUNCTION_EXIT;

		return true;
	}
	

}