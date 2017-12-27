#include <windows.h> 
#include <iostream>
#include <string>
#include <fstream>
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

#define CONFIGURE_FILE "PlanBannerSimulator_config.ini"

//global variables
unsigned long locationKey;
std::string planPath;
bool globalFlag = false;        // communication among the threads
int sec = 0;                    // the interval time
unsigned long counter =0;       // number of plans that are added.
bool block = false;             // indicate that a thread of adding plan has been started.

// thread of adding new plan 
DWORD WINAPI addPlan(LPVOID lib) 
{ 	
	TA_Base_Bus::PlanAgentLibrary   *myLib   =  (TA_Base_Bus::PlanAgentLibrary *)lib;  
	while (!globalFlag)
	{
		try
		{
			std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();
			myLib->runPlanByName(session.c_str(), planPath.c_str());
			counter++;
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "There's an error during adding plan for the &l time",counter);
		}
		Sleep(sec * 1000);		
	}
	return 0;
}

//show the options to choose
int choose(void) 
{ 
	std::cout << std::endl;
	std::cout << "1. adding every X sec"<<std::endl;
	std::cout << "2. stop adding plan" << std::endl;
	std::cout << "3. exit" << std::endl;
	std::cout << "-----------------------" << std::endl;
	std::cout << "Choose the choice you want:";
	char c;
	std::cin >> c;
	return(atoi(&c)); 
} 

void main(int argc, char **argv) 
{ 	
	// Read the command line arguments
	TA_Base_Core::RunParams::getInstance().parseCmdLine( argc, argv );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting tests");

    // Init CorbaUtil;
    TA_Base_Core::CorbaUtil& instanceRef = TA_Base_Core::CorbaUtil::getInstance();
    instanceRef.initialise();
    instanceRef.activate();
	 

	HANDLE hThread; 
	DWORD threadId;
		
	//read the location key and plan path from the configure file.
	std::ifstream ifs(CONFIGURE_FILE);	
	char key[1024] = {0},path[1024] = {0};
	ifs.getline(key, sizeof(key));	
	ifs.getline(path, sizeof(path));	
	locationKey = atol(key) ;
	planPath = path; 
	ifs.close();

	TA_Base_Bus::PlanAgentLibrary m_planAgentLibrary( locationKey );
	
	//start adding plans
	bool f = true;
	while(f)
	{	
		int choice = choose();	
		switch( choice) 
		{
		case 1:
			{   
				if(!block)
				{
					std::cout <<"adding every X sec..."<<std::endl;
					//start a new thread to add the plan
					std::cout << "Please input the interval time:";
					block = true;
					char s;
					std::cin >> s;
					sec = atoi(&s);

					hThread = CreateThread(NULL,0,addPlan,&m_planAgentLibrary,0,&threadId);
					//CloseHandle(hThread);
				}
				else
				{
					std::cout << "You cannot start a thread when another thread is in process!"<<std::endl;
				}
				break;
			}
		case 2: 
			{   
				std::cout <<"stop adding ..."<<std::endl;
				globalFlag = true;	
				block = false;
				std::cout << "Number of plans that have been added is :" <<counter<<std::endl; 
				counter =0;

				CloseHandle(hThread);
				break;
			}
		case 3: 
			{ 
				std::cout <<"exit ..."<<std::endl;
				f = false;
				globalFlag = true;
				break;
			}
		default:
			{
				std::cout<< "Please choose the right choice!"<<std::endl<<std::endl;
		        choose();
			}
		}		
	} 
	
	return;
}



