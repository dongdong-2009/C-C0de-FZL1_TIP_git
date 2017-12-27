// Ats_Alarm_Sender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

#include "..\ShareMemoryDLL\AlarmApi.h"
using namespace I_ATS_ISCS_ALARM;

#include "ReceiveThread.h"
#include "command_Handler.h"
#include <iostream>

IATSAlarm *g_pAtsInterface = NULL;

int main(int argc, char* argv[])
{
	//	create interface

    

    g_pAtsInterface = getATSInterface();

	CReceiveThread receiveThread;
	
	Command_Handler handler;
	
	handler.menu();
	
	for (;;)
	{
		if (handler.service () == -1)
		{
			std::cout <<	"closing down ....\n";
			break;
		}
	}
	
	return 0;
}
