// ATSAlarm.cpp: implementation of the CATSAlarm class.
//
//////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
#include "boost/shared_ptr.hpp"
#include "AlarmProvider.h"
 
#include "ISCSAlarmQueueItem.h"
#include "AckQueueItem.h"
#include "MmsQueueItem.h"
#include "ShareMemoryDLL.h"

#include "DataStoreReceiverATSProfile.h"

#include "DataStoreReceiverAlarmACKCommand.h"
#include "DataStoreReceiverAlarmMMSJobCommand.h"
#include "DataStoreUpdaterATSAlarmInfo.h"
#include "DataStoreUpdaterISCSProfile.h"

#include "DataStoreReceiverAlarmRequest.h"
#include "DataStoreUpdaterAlarmRequest.h"

//#include "ace/Log_Msg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAlarmProvider::CAlarmProvider(unsigned long _ulProviderID)
	:m_ulProviderID(_ulProviderID)
{	  
}

CAlarmProvider::~CAlarmProvider()
{
}

bool CAlarmProvider::getProfileInfo(ProfileInfo& profileInfo)
{
	boost::shared_ptr<ExtProfileInfo> extProfile;	
	extProfile=TA_Base_Bus::DataStoreReceiverATSProfile::getInstance()->getProfile();
	profileInfo.loginLocation=extProfile->loginLocation;
	profileInfo.operatorId=extProfile->operatorId;
	profileInfo.updateTime=extProfile->updateTime;
	profileInfo.profileId=extProfile->profileId;
	profileInfo.displayOnly=extProfile->displayOnly;
	return true;
}

unsigned long CAlarmProvider::retrieveAlarmAckUpdate(std::list<AlarmAckUpdate*>& updates)
{	  
	std::vector<boost::shared_ptr<AlarmACKCommand> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverAlarmACKCommand::getInstance()->getData();	 
	unsigned long nSize = 0;	 
	std::vector<boost::shared_ptr<AlarmACKCommand> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		AlarmAckUpdate *pAlarmAckUpdate = new AlarmAckUpdate();		
		pAlarmAckUpdate->alarmId = (*iter)->alarmId.c_str();
		pAlarmAckUpdate->alarmAcknowledgeBy =  (*iter)->alarmAcknowledgeBy.c_str();
		pAlarmAckUpdate->ackTime =  (*iter)->ackTime;
        updates.push_back(pAlarmAckUpdate);	
		std::ostringstream ss;
		ss<<pAlarmAckUpdate->ackTime;             
		std::string  logInfo=ss.str();
		logInfo="retrieveATSAlarmAckUpdate ID=[" +  pAlarmAckUpdate->alarmId + "] by=["  + pAlarmAckUpdate->alarmAcknowledgeBy + "] time=[" + logInfo + "]";
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());	 
		++nSize;
	}
	if(vec !=NULL)
	{
		vec->clear();
		delete vec;
	}
	 
	return nSize;
}

unsigned long CAlarmProvider::retrieveAlarmRequestUpdate( std::list<AlarmRequest*>& updates )
{
	std::vector<boost::shared_ptr<AlarmRequest> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverAlarmRequest::getInstance()->getData();	 
	unsigned long nSize = 0;	 
	std::vector<boost::shared_ptr<AlarmRequest> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		AlarmRequest *pAlarmReqUpdate = new AlarmRequest();		
		pAlarmReqUpdate->alarmId = (*iter)->alarmId.c_str();
		pAlarmReqUpdate->operatorID =  (*iter)->operatorID.c_str();
		pAlarmReqUpdate->requestTime =  (*iter)->requestTime;
		pAlarmReqUpdate->state =  (*iter)->state;
		updates.push_back(pAlarmReqUpdate);	
		std::ostringstream ss;
		ss<<pAlarmReqUpdate->requestTime;             
		std::string  logInfo=ss.str();
		logInfo="retrieveATSAlarmRequestUpdate ID=[" +  pAlarmReqUpdate->alarmId + "] by=["  + pAlarmReqUpdate->operatorID + "] time=[" + logInfo + "]";
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());	 
		++nSize;
	}
	if(vec !=NULL)
	{
		vec->clear();
		delete vec;
	}

	return nSize;
}

unsigned long CAlarmProvider::retrieveMmsJobRequest(std::list<MmsJobRequest*>& updates)
{
	 
	std::vector< boost::shared_ptr<AlarmMmsJobCommand> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverAlarmMMSJobCommand::getInstance()->getData();
	 
	unsigned long nSize = 0;
 
	std::vector<boost::shared_ptr<AlarmMmsJobCommand> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		MmsJobRequest *pMmsJobRequest = new MmsJobRequest();		
		pMmsJobRequest->AlarmId=(*iter)->alarmId.c_str();
		pMmsJobRequest->AssetName=(*iter)->assetName.c_str();
		pMmsJobRequest->OperatorId=(*iter)->operatorId.c_str();
		pMmsJobRequest->problemReport=(*iter)->problemReport.c_str();
		pMmsJobRequest->targetDate=(*iter)->targetDate;	 
		updates.push_back(pMmsJobRequest);	


		std::ostringstream ss;
		 ;             
		std::string  logInfo=ss.str();
		logInfo="retrieveATSMmsJobRequest ID=[" +  pMmsJobRequest->AlarmId + "] by=["  + pMmsJobRequest->OperatorId + "]" ;		 
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l retrieveATSMmsJobRequest ID=%s \n"), pMmsJobRequest->AlarmId.c_str()));
		++nSize;
	}
	if(vec !=NULL)
	{
		vec->clear();
		delete vec;
	}
	return nSize;

}

void CAlarmProvider::setUpdateTime(time_t updateTime)
{
	//clean all alarm in queue
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->clearAllData();

	//update update time
	std::vector<boost::shared_ptr<ISCSProfileInfo> > batch;	
	boost::shared_ptr<ISCSProfileInfo> profile (new ISCSProfileInfo() );
	profile->loginLocation="ATS";
	profile->operatorId="ATS";
	profile->profileId="ATS";
	profile->updateTime=updateTime;   
	std::ostringstream ss;
	ss<<updateTime;             
	std::string  logInfo=ss.str();
	logInfo="setUpdateTime" + logInfo;
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l setUpdateTime=%d \n"), updateTime));
	 
	batch.push_back(profile);
	TA_Base_Bus::DataStoreUpdaterISCSProfile::getInstance()->onDispatchObjects(batch);	 

}

void CAlarmProvider::submitAlarm(const AlarmDetail& details, const ProfileInfo& profileInfo) //May catch ATS_ALARM_QUEUE_OVERFLOW exception
{
	 
	std::vector<boost::shared_ptr<ExAlarmQueueItem> > batch;

    boost::shared_ptr<ExAlarmQueueItem> sharedobj (new ExAlarmQueueItem());

    sharedobj->detail=details;
	batch.push_back(sharedobj);


	std::ostringstream ss;
	ss<<" state=["<<details.state<<"] acktime=["<<details.ackTime<<"]" ;         
	std::string  logInfo=ss.str();	 
	logInfo="submitATSAlarm  id=[" + details.alarmID + "]  desc=[" + details.alarmDescription  + "] value=[" + details.alarmValue + "]  ackby=[" + details.alarmAcknowledgeBy + "]" + logInfo;
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());

 	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->onDispatchObjects(batch);

 
}

void CAlarmProvider::submitAlarmList(std::list<AlarmDetail*> details, const ProfileInfo& profileInfo) //May catch ATS_ALARM_QUEUE_OVERFLOW exception
{
	std::vector<boost::shared_ptr<ExAlarmQueueItem> > batch;	

	std::list<AlarmDetail*>::iterator iter = details.begin();

	for (; iter != details.end(); ++iter)
	{
		boost::shared_ptr<ExAlarmQueueItem> sharedobj (new ExAlarmQueueItem());
		AlarmDetail* detail=(*iter);
		sharedobj->detail=(*detail);
		sharedobj->profileInfo=profileInfo;
		batch.push_back(sharedobj);
		std::ostringstream ss;
		ss<<" state=["<<detail->state<<"] acktime=["<<detail->ackTime<<"]" ;          
		std::string  logInfo=ss.str();
		logInfo="submitATSAlarm  id=[" + detail->alarmID + "]  desc=[" + detail->alarmDescription  + "] value=[" + detail->alarmValue + "]  ackby=[" + detail->alarmAcknowledgeBy + "]" + logInfo;
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", logInfo.c_str());
	}
	std::string  logInfo;
	
    //ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l submitATSAlarmList=%d \n"), details.size()));
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->onDispatchObjects(batch);
 
}