// AlarmSharedMapReader.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <strstream>
#include <iomanip>
#include <fstream>

#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
#include "core/threads/src/Thread.h"

void outputVec(displayVec& vec, std::ofstream& file)
{
	try
	{
		if(vec.size() <= 0)
		{
			file << "The shared map is empty" << std::endl;
			std::cout << "The shared map is empty." << std::endl;
		}
		else
		{
			file << "count = " << vec.size() << std::endl;
			TA_Base_Core::AlarmItem* alarmItem = NULL;
			for(unsigned int i = 0; i < vec.size(); ++i)
			{
				alarmItem = vec[i];
				file << alarmItem->alarmID << ", "
					<< alarmItem->alarmSeverity << ","
					<< std::setw(25)
					<< alarmItem->assetName << ","
					<< std::setw(16)
					<< alarmItem->assocEntityKey << ","
					<< std::setw(2)
					<< alarmItem->locationKey << ","
					<< std::setw(16)
					<< alarmItem->messageTypeKey << std::endl;

				std::cout << alarmItem->alarmID << ","
					<< alarmItem->alarmSeverity << ","
					<< std::setw(25)
					<< alarmItem->assetName << ","
					<< std::setw(16)
					<< alarmItem->assocEntityKey << ","
					<< std::setw(2)
					<< alarmItem->locationKey << ","
					<< std::setw(16)
					<< alarmItem->messageTypeKey << std::endl;

			}
		}
	}
	catch(...)
	{
		std::cout << "Caught unknown exception in outputVec." << std::endl;
	}

}


int main(int argc, char* argv[])
{
	srand(time(NULL));

	std::cout << "Start to reading shared map:" << std::endl;
	
	try
	{
		std::stringstream fileName;

		fileName << "c:\\DumpAlarmMap."
			<< std::setw(4) << std::setfill('0' )<< rand() % 1000 
			<< ".txt";

		std::ofstream file(fileName.str().c_str());
		if( !(file.is_open()) )
		{
			std::cout << "ERROR: open the file: " << fileName.str().c_str() << " failed." << std::endl; 
			std::cout << "Exit Program." << std::endl;
			exit(0);
		}
		file << "===================================================================================================" << std::endl;
		file << "Output alarms with columns:  AlarmId,  severity, assetName, entityKey, locationKey, messageTypeKey" << std::endl;
		file << "===================================================================================================" << std::endl;
		
		TA_Base_Bus::TA_SharedAlarm sa;
		sa.init("C:/Shared.map", false);
		displayVec vec;
		bool isFilterAlarm = false;
		for( int x = 0; x < 3; ++x)
		{
			try
			{
				if(x == 0)
				{
					std::cout << "------------------------- all alarms ---------------------------" << std::endl;
					file << "------------------------- all alarms ---------------------------" << std::endl;
					sa.getAllAlarms(&vec);
					outputVec(vec, file);
				}
				else if(x == 1)
				{
					std::cout << "------------------------- duty alarms ---------------------------" << std::endl;
					file << "------------------------- duty alarms ---------------------------" << std::endl;
					sa.getAlarmsWithDuty(&vec);
					outputVec(vec, file);
				}
				else if(x == 2)
				{
					for(int pos = 0; pos < CLIENTS_NUM; ++pos)
					{
						sa.getAlarmsWithFilter(&vec, pos);
						if(vec.size() > 0)
						{
							std::cout << "------------------------- filter alarms, pos=" << pos << "---------------------------" << std::endl;
							file << "------------------------- filter alarms, pos=" << pos << "---------------------------" << std::endl;
							outputVec(vec, file);
						}
					}
				}
			}
			catch(...)
			{
				std::cout << "Caught unknown exception when operate SharedAlarm." << std::endl;
			}
		}

		file << "===================================================================================================" << std::endl;
		std::cout << "finished sucessfully. exit." << std::endl;

		sa.fini();
		file.close();
	}
	catch (...)
	{
		std::cout << "Caught unknown exception in main." << std::endl;
	}


	return 0;
}
