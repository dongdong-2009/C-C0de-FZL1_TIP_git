// ISCSAlarm.cpp: implementation of the CISCSAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISCSAlarm.h"
 
//#include "ShmemQueue.h"
#include "ISCSAlarmQueueItem.h"
#include "AckQueueItem.h"
#include "MmsQueueItem.h"
#include "ShareMemoryDLL.h"
// #include "ace\ACE.h"
// #include "ace\Unbounded_Queue.h"

#include "DataStoreUpdaterATSProfile.h"
#include "DataStoreReceiverISCSProfile.h"
#include "DataStoreReceiverATSAlarmInfo.h"
#include "DataStoreUpdaterACKCommand.h"
#include "DataStoreUpdaterAlarmRequest.h"
#include "DataStoreUpdaterMMSJobCommand.h"
 
 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CISCSAlarm::CISCSAlarm()
{
}

CISCSAlarm::~CISCSAlarm()
{

}

unsigned long CISCSAlarm::retrieveAlarm( std::list<ExAlarmQueueItem*>& details )
{
  
	std::vector<boost::shared_ptr<ExAlarmQueueItem> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverATSAlarmInfo::getInstance()->getData();

	if (vec==NULL)
	{
		return 0;
	}

	std::vector<boost::shared_ptr<ExAlarmQueueItem> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		ExAlarmQueueItem *pExAlarmQueueItem = new ExAlarmQueueItem();
        pExAlarmQueueItem->detail.sourceTime = (*iter)->detail.sourceTime;
		pExAlarmQueueItem->detail.alarmID = (*iter)->detail.alarmID.c_str();
		pExAlarmQueueItem->detail.ackTime = (*iter)->detail.ackTime;
		pExAlarmQueueItem->detail.closeTime = (*iter)->detail.closeTime;
		pExAlarmQueueItem->detail.assetName = (*iter)->detail.assetName.c_str();
		pExAlarmQueueItem->detail.alarmSeverity = (*iter)->detail.alarmSeverity;
		pExAlarmQueueItem->detail.alarmDescription = (*iter)->detail.alarmDescription.c_str();
		pExAlarmQueueItem->detail.alarmAcknowledgeBy = (*iter)->detail.alarmAcknowledgeBy.c_str();
		pExAlarmQueueItem->detail.state = (*iter)->detail.state;
		pExAlarmQueueItem->detail.locationId = (*iter)->detail.locationId.c_str() ;
		//std::string profileLocaiton=(*iter)->detail.m_loginLocation;
		//pExAlarmQueueItem->detail.locationId =pExAlarmQueueItem->detail.locationId ;// + " " +  profileLocaiton;	 
	    pExAlarmQueueItem->detail.parentAlarmID = (char*)(*iter)->detail.parentAlarmID.c_str();	 
		pExAlarmQueueItem->detail.avalancheHeadID = (char*)(*iter)->detail.avalancheHeadID.c_str();		 
		pExAlarmQueueItem->detail.isHeadOfAvalanche = (*iter)->detail.isHeadOfAvalanche;
		pExAlarmQueueItem->detail.mmsState = (*iter)->detail.mmsState;
		pExAlarmQueueItem->detail.alarmValue = (*iter)->detail.alarmValue.c_str();
		pExAlarmQueueItem->detail.omAlarm = (*iter)->detail.omAlarm;

        pExAlarmQueueItem->detail.alarmType=(char*)(*iter)->detail.alarmType.c_str();
		pExAlarmQueueItem->detail.systemkey=(char*)(*iter)->detail.systemkey.c_str();
		pExAlarmQueueItem->detail.subsytemkey=(char*)(*iter)->detail.subsytemkey.c_str();

		pExAlarmQueueItem->profileInfo.loginLocation="";
		pExAlarmQueueItem->profileInfo.operatorId="";
		pExAlarmQueueItem->profileInfo.profileId="";
 	
		details.push_back(pExAlarmQueueItem);
	} 
	delete vec;
	boost::shared_ptr<ISCSProfileInfo> profile;
	profile=TA_Base_Bus::DataStoreReceiverISCSProfile::getInstance()->getProfile();	 
	return profile->updateTime;
}

void CISCSAlarm::setProfileInfo(const ProfileInfo& profileInfo)
{
	std::vector<boost::shared_ptr<ExtProfileInfo> > batch;	
	boost::shared_ptr<ExtProfileInfo> atsProfile (new ExtProfileInfo() );
	atsProfile->loginLocation=profileInfo.loginLocation;
	atsProfile->operatorId=profileInfo.operatorId;
	atsProfile->profileId=profileInfo.profileId;
	atsProfile->updateTime=profileInfo.updateTime;
	atsProfile->displayOnly=profileInfo.displayOnly;
	//TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->Initialize(batch);
	batch.push_back(atsProfile);
	TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->onDispatchObjects(batch);
}

void CISCSAlarm::submitAlarmAckUpdate( const AlarmAckUpdate& update )//May catch 
{ 
	std::vector<boost::shared_ptr<AlarmACKCommand> > batch;	
	boost::shared_ptr<AlarmACKCommand> ackCommand (new AlarmACKCommand() );

	ackCommand->ackTime=update.ackTime;
	ackCommand->alarmAcknowledgeBy=update.alarmAcknowledgeBy.c_str();
	ackCommand->alarmId=update.alarmId.c_str();
 
	//TA_Base_Bus::DataStoreUpdaterACKCommand::getInstance()->Initialize(batch);
	batch.push_back(ackCommand);
	TA_Base_Bus::DataStoreUpdaterACKCommand::getInstance()->onDispatchObjects(batch);
 
}

void CISCSAlarm::submitAlarmRequest( const AlarmRequest& update )//May catch 
{ 
	std::vector<boost::shared_ptr<AlarmRequest> > batch;	
	boost::shared_ptr<AlarmRequest> alarmReq (new AlarmRequest() );

	alarmReq->alarmId=update.alarmId.c_str();
	alarmReq->requestTime=update.requestTime;
	alarmReq->operatorID=update.operatorID.c_str();
	alarmReq->state = update.state;

	//TA_Base_Bus::DataStoreUpdaterACKCommand::getInstance()->Initialize(batch);
	batch.push_back(alarmReq);
	TA_Base_Bus::DataStoreUpdaterAlarmRequest::getInstance()->onDispatchObjects(batch);

}

void CISCSAlarm::submitMmsJobRequest( const MmsJobRequest& update )
{
	std::vector<boost::shared_ptr<AlarmMmsJobCommand> > batch;	
	boost::shared_ptr<AlarmMmsJobCommand> mmsJobCommand (new AlarmMmsJobCommand() );

	mmsJobCommand->assetName=update.AssetName.c_str();
	mmsJobCommand->operatorId=update.OperatorId.c_str();
	mmsJobCommand->alarmId=update.AlarmId.c_str();
	mmsJobCommand->problemReport=update.problemReport.c_str();
	mmsJobCommand->targetDate=update.targetDate;
   	//TA_Base_Bus::DataStoreUpdaterMMSJobCommand::getInstance()->Initialize(batch);	 
	batch.push_back(mmsJobCommand);
	TA_Base_Bus::DataStoreUpdaterMMSJobCommand::getInstance()->onDispatchObjects(batch);
}
