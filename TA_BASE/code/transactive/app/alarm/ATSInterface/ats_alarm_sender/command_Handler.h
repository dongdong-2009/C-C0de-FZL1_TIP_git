// ommand_Handler.h: interface for the Command_Handler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMMAND_HANDLER_H__C6646D7E_949D_4998_AD7D_776685B62B5B__INCLUDED_)
#define AFX_OMMAND_HANDLER_H__C6646D7E_949D_4998_AD7D_776685B62B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\ShareMemoryDLL\AlarmApi.h"
 

class Command_Handler  
{
public:
	int retrieveAll();
	void menu();
	int service();
	Command_Handler();
	virtual ~Command_Handler();

protected:
	int submitATSAlarm(char * alarmID, char* description);
	int setUpdateTime();
	int submitATS1000Alarms(char* description, int idBegin, int idEnd);
	
	I_ATS_ISCS_ALARM::AlarmDetail* createNewATSAlarm(char * alarmID, char* description);

};

#endif // !defined(AFX_OMMAND_HANDLER_H__C6646D7E_949D_4998_AD7D_776685B62B5B__INCLUDED_)
