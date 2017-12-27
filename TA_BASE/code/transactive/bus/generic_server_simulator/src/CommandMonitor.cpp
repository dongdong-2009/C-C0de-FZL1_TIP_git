/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/CommandMonitor.cpp $
  * @author:  Ross Mckillop
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation file for the CommandMonitor class.  
  * CommandMonitor provides functionality to constantly listen for
  * user input from the command line, and to process that input
  * and execute the appropriate commands.
  */
#ifdef WIN32
#pragma warning(disable : 4786)
#endif // #ifdef WIN32

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_server_simulator/src/CommandMonitor.h"
#include "bus/generic_server_simulator/src/ISimulationCommandProcessor.h"
#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "bus/generic_server_simulator/src/GssSimulationModuleFactory.h"
#include "bus/generic_server_simulator/src/GenericCommandProcessor.h"
#include "bus/generic_server_simulator/src/ConsoleDetachUtility.h"

using namespace TA_Base_Bus;

CommandMonitor::CommandMonitor(const std::string & desc, GenericSimulator * simulator)
: m_desc(desc)
, m_simulator(simulator)
, m_currentModule(0)
, m_doQuit(false)
, m_commandProcessor(0)
{
    m_commandProcessor = new GenericCommandProcessor(this);
}

CommandMonitor::~CommandMonitor()
{
}


bool CommandMonitor::monitorUserInput()
{
	// set the current module to the last one on the list
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (modules.size() > 0)
	{
		m_currentModule = modules[modules.size() - 1];
	}
	else
	{
		m_currentModule = 0;
	}

	char input[1024];
	std::string inputString;
	
	//output the welcome message and the first prompt
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "                 Ripple Systems Pty Ltd" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << m_desc << std::endl << std::endl;
	printHelp();
	std::cout << "Note: commands and parameters are case-sensitive" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl << std::endl;
	// std::cout << "Simulator instance id: " << "0x" << std::hex <<((unsigned int) m_simulator) << std::dec << std::endl;

	listModules();

    std::string startupScript = TA_Base_Core::RunParams::getInstance().get("StartupScript");
    if ( !startupScript.empty() )
    {
        executeScript(startupScript);
    }


	//This while loop sits and accepts user input via the command line.
	//When the quit command is received, the global continue variable
	//is set to false, and the while loop will exit, and the thread  
	//will finish.
	while (!m_doQuit)
	{
		if (m_currentModule != 0)
			std::cout << m_currentModule->getName();
		std::cout << "> ";
		std::cin.getline(input, 1024); //pull the line from std::cin

		// hackish trim
		char* sptr = input;
		while (*sptr != 0 && (*sptr == ' ' || *sptr == '\t'))
		{
			sptr++;
		}
		char* eptr = input + strlen(input) - 1;
		while (eptr >= sptr && (*eptr == ' ' || *eptr == '\t'))
		{
			*eptr = 0;
			eptr--;
		}

		inputString = sptr;
		
		bool ok = parseCommand(inputString);
	}

	return true;
}


bool CommandMonitor::executeScript(const std::string& filename)
{
	static int recursion = 0;

	if (filename.size()<1)
	{
		std::cout << "Filename not specified to open." << std::endl;
		return false;
	}

    std::ifstream* inFile = 0;

	inFile = new std::ifstream(filename.c_str());
	if (!inFile || !inFile->is_open()) //standard file check
	{
		std::cout << "Could not open file: " << filename << std::endl;
		return false;
	}

	if (recursion>50)
	{
		// Probably recursed too much!
		std::cout << "Script recursion exceeded 50 deep." << std::endl;
		return false;
	}

	recursion++;
	
	// Save current module
	ISimulationModule*	currentModule = m_currentModule;

	char input[1024];
	std::string inputString;
	
	//This while loop sits and accepts user input via the command line.
	//When the quit command is received, the global continue variable
	//is set to false, and the while loop will exit, and the thread  
	//will finish.
	while (!m_doQuit)
	{
		// read from input file
		if (inFile->getline(input, 1024))
		{
			if (m_currentModule != 0)
				std::cout << m_currentModule->getName();
			std::cout << "> " << input << std::endl;
		}
		else
		{
			// EOF
			break;
		}

		// hackish trim
		char* sptr = input;
		while (*sptr != 0 && (*sptr == ' ' || *sptr == '\t'))
		{
			sptr++;
		}
		char* eptr = input + strlen(input) - 1;
		while (eptr >= sptr && (*eptr == ' ' || *eptr == '\t'))
		{
			*eptr = 0;
			eptr--;
		}

		inputString = sptr;
		
		bool ok = parseCommand(inputString);

		if (!ok) break;
	}
	
	recursion--;

	inFile->close();

	// Restore current module
	m_currentModule = currentModule;

	return true;
}



bool CommandMonitor::parseCommand(std::string & inputString)
{
    // take reentrant lock
    TA_Base_Core::ThreadGuard oguard(m_commandLock);

	// ignore blank input and comments
	if ( inputString.length() == 0 || inputString.at(0) == '#')
	{
		return true;
	}

	// If starts with ! submit to system command processor.
	// This way we can do dir's, more's and ..er.. stuff. Like.
	if (inputString.at(0) == '!')
	{
		system(inputString.c_str() + 1);
		return true;
	}

    TokenList cmdParts;
    if ( GssUtility::tokenise(inputString.c_str(), cmdParts, " ", 2) < 1 )
    {
        std::cerr << "ERROR: no command specified" << std::endl;
        return false;
    }

    
	std::string cmdName = POP_TOKEN(cmdParts);

	std::string cmdValue;
    if (!cmdParts.empty())
    {
        cmdValue = POP_TOKEN(cmdParts);
    }

    bool handled=true;
    bool ok = doCommand(cmdName, cmdValue, handled);

    // if we handled the command withing this class, return the value that our
    // handling returned
    if (handled)
    {
        return ok;
    }

    if (m_currentModule != 0)
	{
		// now see if the module can execute the command
		ISimulationCommandProcessor & cmd = m_currentModule->getCommandProcessor();

		if ( &cmd != 0 )
		//if(cmd)
		{
			bool cmdOK = false;

			if (cmd.hasCommandName(cmdName))
			{
				if (!(cmdOK = cmd.execute(cmdName, cmdValue)))
				{
					if (cmd.errorMsg() != "")
					{
						std::cerr << "ERROR:" << std::endl << cmd.errorMsg() << std::endl;
					}
					std::cerr << "Usage:" << std::endl << cmd.usageString(cmdName) << std::endl;

					// cmdOK = false;
				}
			}
			else
			{
				std::cerr << "Unknown command: " << cmdName << std::endl;
			}

			return cmdOK;
		}
		else
		{
			std::cerr << "Current module does not have a command processor." << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "No current module." << std::endl;
        listModules();
		return false;
	}

	// obligatory Half-Life quote:
	std::cerr << "This can't be happening to me." << std::endl;
	return false;
}


void CommandMonitor::listModules()
{
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (modules.size() > 0)
	{
		std::cout << "Modules loaded:" << std::endl;
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			std::cout << modules[i]->getName() << " (0x" << std::hex << ((unsigned int) modules[i]) << std::dec << ") - " << modules[i]->getSimDescription() << std::endl;
		}
	}
	else
	{
		std::cout << "There are no modules loaded." << std::endl;
	}
	if (m_currentModule != 0)
	{
		std::cout << "Current module is: " << m_currentModule->getName() << std::endl;
	}
}


bool CommandMonitor::switchModule(const std::string & moduleName)
{
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (modules.size() > 0)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			if (modules[i]->getName() == moduleName)
			{
				std::cout << "Switching to module: " << moduleName << std::endl;
				m_currentModule = modules[i];
				return true;
			}
		}

		std::cerr << "Module '" << moduleName << "' not found.  Check for no extra spaces." << std::endl;
	}
	else
	{
		std::cout << "There are no modules loaded." << std::endl;
	}

	return false;
}


void CommandMonitor::printHelp()
{
	std::cout << "General Commands" << std::endl;
	std::cout << "----------------" << std::endl;
	std::cout << "list - list modules" << std::endl;
	std::cout << "addmod <name> <type>" << std::endl;
	std::cout << "module <module> - switch to <module>" << std::endl;
	std::cout << "start [all] - start current module or all modules" << std::endl;
	std::cout << "stop [all] - stop current module or all modules" << std::endl;
	std::cout << "restart [all] - restart current module or all modules" << std::endl;
//	std::cout << "exec <script> - run a command script" << std::endl;
	std::cout << "sleep <milliseconds> - sleep for a while" << std::endl;
	std::cout << "help - show this (and current module help)" << std::endl;
	std::cout << "reset - reset the simulation, all modules destroyed" << std::endl;
	std::cout << "quit, exit - quit simulator" << std::endl;

    m_commandProcessor->execute("help", "");

    std::cout << "lines starting with '!' are executed by the system command processor." << std::endl;
}

bool CommandMonitor::doCommand(const std::string & cmdName, const std::string & cmdValue, bool & wasHandled)
{
    // let us suppose that we are going to handle this
    wasHandled = true;

	if (cmdName == "quit" || cmdName == "exit")
	{
        if (ConsoleDetachUtility::isDetached())
        {
        
            std::cout << "This will " << cmdName << " the simulator executable, not disconnect. Is this what you want to do [y|N] ? ";
            std::cout.flush();

            char c;
            std::cin >> c;

            if ( (c!='Y') && (c!='y') )
            {
                std::cout << cmdName << " cancelled" << std::endl << std::endl;;
                std::cout << "Hint: Use Ctrl-D, \"bye\" or \"logout\" to disconnect." << std::endl << std::endl;;
            }
            else
            {
                m_doQuit = true;
            }
        }
        else
        {
            m_doQuit = true;
        }
		
		return true;
	}

    if (cmdName == "list")
	{
		listModules();
		return true;
	}
	
	if (cmdName == "module")
	{
		return switchModule(cmdValue);
	}

	//if (cmdName == "exec")
	//{
	//    return doExec(cmdValue);
	//}

	if (cmdName == "start")
	{
		return doStart(cmdValue);
	}

	if (cmdName == "restart")
	{
		return doRestart(cmdValue);
	}

	if (cmdName == "stop")
	{
		return doStop(cmdValue);
	}

	if (cmdName == "sleep")
	{
		return doSleep(cmdValue);
	}

	if (cmdName == "addmod")
	{
		return doAddmod(cmdValue);
	}

	if (cmdName == "reset")
	{
		return doReset(cmdValue);
	}

	if (cmdName == "?" || cmdName == "help")
	{
		printHelp();

		if (m_currentModule == 0)
		{
			std::cout << "No current module" << std::endl;
            return true;
		}
		std::cout << std::endl << "Module Help" << std::endl;
		std::cout << "-----------" << std::endl;
        wasHandled = false;
        return false;
	}

    if (m_commandProcessor->hasCommandName(cmdName))
    {
        return m_commandProcessor->execute(cmdName, cmdValue);
    }

    // if the flow of execution got here, the command wasn't handled here.
    wasHandled = false;

    // also, should return false
    return false;
}

bool CommandMonitor::doExec(const std::string& cmdValue)
{
	if (cmdValue != "")
	{
		bool ok = executeScript(cmdValue);
		if (ok)
		{
			std::cout << "finished exec: " << cmdValue << std::endl;
		}
		else
		{
			std::cout << "ERROR: exec failed: " << cmdValue << std::endl;
		}

		return ok;
	}
	else
	{
		std::cout << "Usage:" << std::endl << "exec <filename>" << std::endl;
		return false;
	}	
}


bool CommandMonitor::doStart(const std::string& cmdValue)
{
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (m_currentModule == 0 || modules.size() == 0)
	{
		std::cout << "No current module" << std::endl;
		return false;
	}
	else
	{
		if (cmdValue == "all")
		{
			std::cout << "Starting all modules..." << std::endl;
			for (unsigned int i = 0; i < modules.size(); i++)
			{
				modules[i]->startSim();
			}
		}
		else
		{
			std::cout << "Starting module " << m_currentModule->getName() << std::endl;
			m_currentModule->startSim();
		}
	}

	return true;
}


bool CommandMonitor::doRestart(const std::string& cmdValue)
{
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (m_currentModule == 0 || modules.size() == 0)
	{
		std::cout << "No current module" << std::endl;
		return false;
	}
	else
	{
		if (cmdValue == "all")
		{
			std::cout << "Restarting all modules..." << std::endl;
			for (unsigned int i = 0; i < modules.size(); i++)
			{
				modules[i]->restartSim();
			}
		}
		else
		{
			std::cout << "Restarting module " << m_currentModule->getName() << std::endl;
			m_currentModule->restartSim();
		}
	}

	return true;
}


bool CommandMonitor::doStop(const std::string& cmdValue)
{
	const ModuleContainer & modules = m_simulator->getSimulationModules();
	if (m_currentModule == 0 || modules.size() == 0)
	{
		std::cout << "No current module" << std::endl;
		return false;
	}
	else
	{
		if (cmdValue == "all")
		{
			std::cout << "Stopping all modules..." << std::endl;
			for (unsigned int i = 0; i < modules.size(); i++)
			{
				modules[i]->stopSim();
			}
		}
		else
		{
			std::cout << "Stopping module " << m_currentModule->getName() << std::endl;
			m_currentModule->stopSim();
		}
	}

	return true;
}


bool CommandMonitor::doSleep(const std::string& cmdValue)
{
	int timeout = 0;

	if (sscanf(cmdValue.c_str(), "%i", &timeout) != 1 || timeout < 0)
	{
		std::cerr << "ERROR: invalid timeout specification" << std::endl;
		std::cerr << "Usage:" << std::endl << "sleep <timeout millis>" << std::endl;
		return false;
	}

	// traveller, rest yon weary head
	TA_Base_Core::Thread::sleep(timeout);

	return true;
}


bool CommandMonitor::doAddmod(const std::string& cmdValue)
{
    TokenList parameters;
    int numtkns = GssUtility::tokenise(cmdValue.c_str(), parameters, " ", 3);

    //
    // at least two parameters are required
    if (numtkns<2)
    {
        addmodUsage();
        return false;
    }

    std::string name = POP_TOKEN(parameters);
    std::string type = POP_TOKEN(parameters);

    std::string paramstr = "";
    if (!parameters.empty())
    {
        paramstr = POP_TOKEN(parameters);
    }


	bool ok = false;

	if (m_simulator->getModuleByName(name))
	{
		std::cerr << "ERROR: there is already a module named " << name << std::endl;
	}
	else
	{
		ISimulationModule * simModule = createSimulationModule(type,name,paramstr);
		if (simModule != 0)
		{
			m_simulator->addModule(simModule);
			m_currentModule = simModule;
            ok = true;
		}
	}
	
	if (!ok)
	{
        addmodUsage();
	}

	return ok;
}


bool CommandMonitor::doReset(const std::string& cmdValue)
{
	std::cout << "Resetting..." << std::endl;
	m_currentModule=NULL;

	// Stop any current simulation
	m_simulator->stopSimulation();

	// Remove and delete all the simulation modules
    m_simulator->clearSimulationModules();

	std::cout << "done." << std::endl;

	return true;
}

void CommandMonitor::addmodUsage()
{
    std::cerr << "Usage:" << std::endl;
    std::cerr << "addmod <name> <type>" << std::endl << std::endl;
    std::cerr << "where:" << std::endl
          << "    name    is an arbitrary but useful name for the module being added" << std::endl
          << "    type    is the name of a module that can be created. Possibilities are ";

    std::list<std::string> names;
    if (GssSimulationModuleFactory::getInstance().getRegisteredModuleNames(names) != 0)
    {
        while (!names.empty())
        {
            std::string name = names.front(); names.pop_front();
            std::cerr << name << " " << std::endl;
        }
    }
    else
    {
        std::cerr << " [None Found!!]" << std::endl;
    }

    std::cerr << " The module itself may require extra parameters (for instance a hostname and port)." << std::endl;
}

