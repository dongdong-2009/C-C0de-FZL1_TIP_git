// ommand_Handler.cpp: implementation of the Command_Handler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "command_Handler.h"
#include <iostream>
#include "windows.h"
#include <time.h>
#include <sstream>
using namespace I_ATS_ISCS_ALARM;

extern IATSAlarm *g_pAtsInterface;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Command_Handler::Command_Handler()
{

}

Command_Handler::~Command_Handler()
{

}

int Command_Handler::service()
{
    char option[BUFSIZ];
    char buf1[BUFSIZ];
    char buf2[BUFSIZ];
	
    if (::scanf ("%s", option) <= 0)
	{
		std::cout << "try again!! \n";
        return 0;
	}
	
    int result = 0;
    switch (option[0])
	{

	case 'S' :
	case 's' :
        if (::scanf ("%s", buf1) <= 0)
			break;
		if (::scanf ("%s", buf2) <= 0)
			break;
        result = submitATSAlarm (buf1, buf2);
        break;
	case 'B' :
	case 'b' :
		{	 
		if (::scanf ("%s", buf1) <= 0)
			break;	
		if (::scanf ("%s", buf2) <= 0)
			break;	
		int bb=atol(buf1);
		int ee=atol(buf2);
		result = submitATS1000Alarms ("testing", bb,ee);
		break;
		}
	case 'T' :
	case 't' :		 
		result = setUpdateTime ();
		break;

	case 'R' :
	case 'r' :
		result = retrieveAll();
		break;

	case 'M' :
	case 'm' :
		menu();		
        return 0;


	case 'Q' :
	case 'q' :
        return -1;
	default :
        std::cout << "unrecognized command \n" ;
		menu();
		return 0;
	}

    if (result == 0)
	{
		std::cout << "Last operation was successful!! \n";
	}
    else
	{
		std::cout << "Last operation failed!! \n";
		//menu ();
	}
		
    return 0;
}

void Command_Handler::menu()
{

    std::cout << "\t**************************                                  \n" ;
    std::cout << "\tThe ats alarm share memory menu                             \n" ;
    std::cout << "\t<S> Submit ats alarm  [ID] [Description]                    \n" ;
    std::cout << "                                                              \n" ;
	std::cout << "\t<B> Submit alarms  [Begin ID] [End ID]                      \n" ;
	std::cout << "                                                              \n" ;
    std::cout << "\t<R> Retrieve All                                            \n" ;
    std::cout << "                                                              \n" ;
	std::cout << "\t<T> UpdateTime                                              \n" ;
	std::cout << "                                                              \n" ;
	std::cout << "\t<M> Menu                                                    \n" ;
    std::cout << "                                                              \n" ;
    std::cout << "\t<Q> Quit                                                    \n" ;
    std::cout << "\t**************************                                  \n" ;
  
}


AlarmDetail* Command_Handler::createNewATSAlarm(char * alarmID, char* description)
{
	AlarmDetail* details=new AlarmDetail();
	memset(details, 0, sizeof(AlarmDetail));
	details->alarmID = alarmID;
	details->alarmDescription = description;	 
	details->alarmAcknowledgeBy = "";
	details->assetName = "asset";
	details->alarmValue = "value";
	details->alarmSeverity = 5;
	details->locationId = "DBG";
	details->assetName="assetname";
	details->avalancheHeadID="";
	details->parentAlarmID=""; 
	details->state=AlarmState::AlarmOpen;
	return details;
}

int Command_Handler::setUpdateTime()
{
	if(g_pAtsInterface)
	{
		g_pAtsInterface->setUpdateTime(time(NULL));
	}
	return 0;
}


int Command_Handler::submitATS1000Alarms(char* description, int idBegin, int idEnd)
{
    ProfileInfo profileInfo;
	if(g_pAtsInterface)
	{
		try{
			std::list<AlarmDetail*> alarmList;
			profileInfo.profileId="";
			profileInfo.loginLocation="DT01";
			profileInfo.operatorId="davis";
			int i=0;
			//400000000
			//while(i<1000)		
			for(int i=idBegin;i<idEnd;i++)
			{	 				
				    //i++;				 	 
					AlarmDetail* pDetails=new AlarmDetail();
					AlarmDetail&  details=*pDetails;
					std::stringstream st;
					st<<i;
					//memset(&details, 0, sizeof(AlarmDetail));
					//memset(&profileInfo, 0, sizeof(ProfileInfo));
					details.ackTime=time(NULL);
					details.alarmAcknowledgeBy = "";	  
					details.alarmDescription = description;					 
					details.alarmID =  st.str();
					details.alarmSeverity = 5;
					details.alarmValue = "value";
					details.assetName="assetname";
					details.avalancheHeadID="";
					details.closeTime=0;
					details.isChildOfAvalanche=false;
					details.isHeadOfAvalanche=false;
					details.locationId = "DT01";
					details.mmsState=(I_ATS_ISCS_ALARM::MmsStateType)(i%7);
					details.omAlarm='O';
					
					details.parentAlarmID="";				
					details.sourceTime.time=time(NULL);					
					details.sourceTime.millitm=0;
					details.state=AlarmState::AlarmOpen;

					details.alarmType="50505";
					details.subsytemkey="11";
					details.systemkey="1";

					 
					alarmList.push_back(pDetails);		

// 					if ((i%500)==0)
// 					{ 
// 						g_pAtsInterface->submitATSAlarmList(alarmList, profileInfo);
// 						std::cout << "sent   alarms" << st.str() << "\n";
// 						std::list<AlarmDetail*>::iterator iter=alarmList.begin();
// 						for (;iter!=alarmList.end();iter++)
// 						{
// 							delete (*iter);
// 						}			
// 						alarmList.clear();		
// 						Sleep(700);
// 					}
// 					if((i%5000)==0)
// 					{	
// 
//                         setUpdateTime();
// 						Sleep(2000);
// 					}
			}
			g_pAtsInterface->submitATSAlarmList(alarmList, profileInfo);
			//std::cout << "sent   alarms" << st.str() << "\n";
			std::list<AlarmDetail*>::iterator iter=alarmList.begin();
			for (;iter!=alarmList.end();iter++)
			{
				delete (*iter);
			}			
			alarmList.clear();		
		}
		catch(char *str )
		{
			 std::cout << "Caught some other exception: " << str << "\n";
			 return -1;
		}
		catch(...)
		{
			std::cout << "sent   alarms failed  \n";
			return -1;        
		}
		 
		std::cout << "sent   alarms\n";
		return 0;
	}

	return -1;
}

int Command_Handler::submitATSAlarm(char *alarmID, char* description)
{

	if(g_pAtsInterface)
	{
		AlarmDetail details;
		ProfileInfo profileInfo;
		
		memset(&details, 0, sizeof(AlarmDetail));
		memset(&profileInfo, 0, sizeof(ProfileInfo));
		
		details.alarmID = alarmID;
		details.sourceTime.time=time(NULL);
		details.ackTime=time(NULL);
		details.alarmDescription = description;	 
		details.alarmAcknowledgeBy = "";	 
		details.alarmValue = "value";
		details.alarmSeverity = 5;
		details.locationId = "DT01";
		details.assetName="assetname";
		details.avalancheHeadID="";
		details.parentAlarmID="";
		profileInfo.profileId="";
		profileInfo.loginLocation="DT01";
		profileInfo.operatorId="davis";
		details.state=AlarmState::AlarmOpen;
		details.alarmType="50505";
		details.subsytemkey="11";
		details.systemkey="1";

// 		std::list<AlarmDetail*> alarmList;
// 
// 		alarmList.push_back(  createNewATSAlarm(alarmID,description));
// 		alarmList.push_back(  createNewATSAlarm(alarmID,description));
// 		alarmList.push_back(  createNewATSAlarm(alarmID,description));

	
		

		g_pAtsInterface->submitATSAlarm(details, profileInfo);

		//g_pAtsInterface->submitATSAlarmList(alarmList,profileInfo);

		std::cout << "send alarm  ID=" << details.alarmID.c_str() << "\n";

		return 0;
	}

	return -1;
}

int Command_Handler::retrieveAll()
{
	if(g_pAtsInterface)
	{
		//retrieve ack
		std::list<AlarmAckUpdate*> AckupdateList;
		g_pAtsInterface->retrieveATSAlarmAckUpdate(AckupdateList);
		for(std::list<AlarmAckUpdate*>::iterator itr = AckupdateList.begin();
		itr != AckupdateList.end(); 
		++itr)
		{
			
			std::cout << "---recv Ack:" 
				<<" " <<(*itr)->alarmId
				<<" " <<(*itr)->alarmAcknowledgeBy
				<<" " <<(*itr)->ackTime
				<< "\n";	
		}
		
		//retrieve mms
		std::list<MmsJobRequest*> MmsupdateList;
		g_pAtsInterface->retrieveATSMmsJobRequest(MmsupdateList);
		for(std::list<MmsJobRequest*>::iterator itr1 = MmsupdateList.begin();
		itr1 != MmsupdateList.end(); 
		++itr1)
		{
			
			std::cout << "---recv Mms:" 
				<<" " << (*itr1)->AlarmId
				<<" "<<(*itr1)->AssetName 
				<<" "<<(*itr1)->OperatorId 
				<<" "<<(*itr1)->targetDate 
				<<" "<<(*itr1)->problemReport 
				<< "\n";	
		}
	
	}

	return 0;
}
