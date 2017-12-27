///////////////////////////////////////////////////////////
//  IMMSSubmitter.cpp
//  Implementation of the Class IMMSSubmitter
//  Created on:      16-¾ÅÔÂ-2011 16:52:28
//  Original author: huirong.luo
///////////////////////////////////////////////////////////

//#include "boost/tokenizer.hpp"
 
#include "core/alarm/src/AlarmConfig.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/MmsRequestAccessFactory.h"
 
#include "MMSDatabaseCache.h" 
#include "MMSUtility.h"


namespace TA_Base_Bus
{

	MMSUtility::MMSUtility()
	{

	}


	MMSUtility::~MMSUtility()
	{

	}

	std::string  MMSUtility::getAlarmFaultCode(std::string& entityName)
	{			 
		
		return MMSUtility::getMeterCode(entityName);
	}

	std::string  MMSUtility::getMeterCode(std::string& entityName)
	{
		std::string falutCode="";
		std::string::size_type pos = entityName.find_last_of ( ".", entityName.length() );
		if ( pos > 0 )
		{
			falutCode = entityName.substr ( pos+1, entityName.length() );
		}
		else
		{
			falutCode = "";
		}
		return falutCode.c_str();
	}


	TA_Base_Core::MmsRequestAlarmData* MMSUtility::getMmsRequestAlarmData(const TA_Base_Core::AlarmDetailCorbaDef& alarm,std::string sessionID)
	{
		//transfer Alarm info to JobRequest info
		//alarm data/time  ok
		//alarm severity   ok
		//equipment id      38 ok
		//alarm fault code  30  
		//alarm description 200 ok
		TA_Base_Core::IEntityData& entity=MMSDatabaseCache::getInstance()->getEntity(alarm.assocEntityKey);

		unsigned long alarmSeverity=alarm.alarmSeverity;
		std::string alarmID=alarm.alarmID.in();
		std::string equipmentID=alarm.assetName.in();
		std::string entityName=entity.getName();
		std::string alarmFaultCode=MMSUtility::getAlarmFaultCode(entityName);
		std::string alarmDescription=getAlarmDescription(alarm.messageTypeKey, alarm.alarmParameterList.in(), alarm.alarmValue.in());
		std::string requestor="";
		std::string problemReport="";
		bool isAutoTrigger=true;
		timeb  sourceTime;
		sourceTime.time=alarm.sourceTime.time;
		sourceTime.millitm=alarm.sourceTime.millitm;
		timeb targetTime=sourceTime;
		if (!sessionID.empty())
		{
			isAutoTrigger=false;
		}

		//TODO: smart point
		TA_Base_Core::MmsRequestAlarmData* pmmsReqestData=NULL;
		pmmsReqestData= new TA_Base_Core::MmsRequestAlarmData(0,alarmID,sourceTime,alarmSeverity,equipmentID,alarmFaultCode,
			alarmDescription,requestor,problemReport,targetTime,isAutoTrigger);		
		return pmmsReqestData;


	}



	bool MMSUtility::saveMmsRequestAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{

		TA_Base_Core::MmsRequestAlarmData* pmmsReqestData= NULL;
		pmmsReqestData= getMmsRequestAlarmData(alarm);

		std::auto_ptr<TA_Base_Core::MmsRequestAlarmData> mmsReqestData(pmmsReqestData);

		return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestAlarm( *(mmsReqestData.get()) );


	}



	bool MMSUtility::submitJobRequest(const TA_Base_Bus::JobRequestDetailCorbaDef& details)
	{
		if( 0 != strcmp("NOT_FROM_ALARM", details.alarmUID.in()) )
		{
			try //try and set the state of the alarm to show that a JR has been raised on it
			{
				//translate JobRequest to database 
				std::string alarmID= details.alarmUID.in();
				timeb alarmDate;
				alarmDate.time =details.alarmTime;
				alarmDate.millitm=0;
				unsigned long alarmSeverity=details.alarmSeverity;
				std::string equipmentID=details.assetId.in();
				std::string alarmFaultCode=details.alarmFaultCode.in();
				std::string alarmDescription=details.alarmDescField.in();
				std::string requestor=details.requestor.in();
				std::string problemReport=details.problemReport.in();
				timeb targetDate;			 
				targetDate.time =details.targetDate;
				targetDate.millitm=0;
				bool isAutoTrigger = false;
				TA_Base_Core::MmsRequestAlarmData* pmmsReqestData=new  TA_Base_Core::MmsRequestAlarmData
					(0,alarmID,alarmDate,alarmSeverity,equipmentID,alarmFaultCode, alarmDescription,
					requestor,problemReport,targetDate,isAutoTrigger);				 
				std::auto_ptr<TA_Base_Core::MmsRequestAlarmData> mmsReqestData(pmmsReqestData);
				return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestAlarm(*(mmsReqestData.get()));
			}
			catch(...)
			{
				LOG0( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, "submitJobRequestMessage saveMmsRequestAlarm errro" );
			}
			return false;
		}
		else //the Job Request was not from an alarm... nothing to do but log
		{
			//translate JobRequest to database 
			unsigned long entityKey=details.dataPointKey;    
			std::string equipmentID=details.assetId.in();		 
			std::string requestor=details.requestor.in();
			std::string problemReport=details.problemReport.in();
			timeb targetDate;			 
			targetDate.time =details.targetDate;
			targetDate.millitm=0; 
			TA_Base_Core::MmsRequestSchematicData*  pmmsReqestData =new TA_Base_Core::MmsRequestSchematicData(0,entityKey,      
				equipmentID, requestor, problemReport,targetDate);
			std::auto_ptr<TA_Base_Core::MmsRequestSchematicData> mmsReqestData(pmmsReqestData);
			return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestSchematic(*(mmsReqestData.get()));
		}
	}

    std::string MMSUtility::getAlarmDescription( const unsigned long ulMsgKey, const std::string& strParamList, const std::string& strAlarmValue )
    {
        std::string alarmMessage = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( ulMsgKey,  strParamList );
        alarmMessage = alarmMessage + ", " + strAlarmValue;	

        const unsigned int ALARM_MSG_SIZE = 200 * 4;
        //if less than 152 chars just set references
        if(alarmMessage.length() > ALARM_MSG_SIZE)
        {
            alarmMessage = alarmMessage.substr( 0, ALARM_MSG_SIZE );
        }

        return alarmMessage;
    }

 
}