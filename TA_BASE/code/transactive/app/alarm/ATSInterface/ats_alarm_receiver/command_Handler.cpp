// ommand_Handler.cpp: implementation of the Command_Handler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "command_Handler.h"
#include <iostream>
#include "windows.h"
#include "..\ShareMemoryDLL\AlarmApi.h"
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
    std::cout << "\t<R> Retrieve All                                            \n" ;
    std::cout << "                                                              \n" ;
	std::cout << "\t<M> Menu                                                    \n" ;
    std::cout << "                                                              \n" ;
    std::cout << "\t<Q> Quit                                                    \n" ;
    std::cout << "\t**************************                                  \n" ;
  
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
		details.alarmDescription = description;
		details.alarmAcknowledgeBy = "davis";
		details.assetName = "asset";
		details.alarmValue = "value";
		details.alarmSeverity = 3;
		details.locationId = "DBG";
		details.assetName="assetname";
		details.avalancheHeadID="";
		details.parentAlarmID="";
		profileInfo.profileId="";
		profileInfo.loginLocation="DBG";
		profileInfo.operatorId="davis";
	
		

		g_pAtsInterface->submitATSAlarm(details, profileInfo);

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
