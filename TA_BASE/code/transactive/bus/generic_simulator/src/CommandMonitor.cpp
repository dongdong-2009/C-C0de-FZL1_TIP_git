/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_simulator/src/CommandMonitor.cpp $
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
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_simulator/src/CommandMonitor.h"
#include "bus/generic_simulator/src/ISimulationCommandProcessor.h"
#include "bus/generic_simulator/src/GenericSimulator.h"

using namespace TA_Base_Bus;


/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/

CommandMonitor::CommandMonitor(const std::string &desc, GenericSimulator *simulator)
 : m_desc(desc), m_simulator(simulator), m_currentModule(0), m_doQuit(false)
{

}

CommandMonitor::~CommandMonitor()
{
}


/* ----------------------------------------------------------------------------
	Method     : MonitorUserInput()
	Description: This method takes the user input and executes
				 the parseCommand() method.
 * --------------------------------------------------------------------------*/

bool CommandMonitor::monitorUserInput()
{
	// set the current module to the last one on the list
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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
	std::cout << "Simulator instance id: " << "0x" << std::hex <<((unsigned int) m_simulator) << std::dec << std::endl;

	listModules();

    std::string startupScript = TA_Base_Core::RunParams::getInstance().get(RPARAM_STARTUP_SCRIPT);
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

/* ----------------------------------------------------------------------------
	Method     : executeScript()
	Description: This method reads the specified script file and executes
				 the parseCommand() method.
				 Note: This method may be called recursively by another script.
 * --------------------------------------------------------------------------*/
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


/* ----------------------------------------------------------------------------
	Method     : parseCommand()
	Description: This method takes the input string from the user
				 and attempts to create a new ICommand object based
				 on that instruction.  It splits the command string into
				 a name/value pair and calls CommandFactory::createCommand
 * --------------------------------------------------------------------------*/

bool CommandMonitor::parseCommand(std::string & inputString)
{
	// ignore blank input and comments
	if ( inputString.length() == 0 || inputString.at(0) == '#')
	{
		return true;
	}

	// hackish.  If starts with ! submit to system command processor.
	// This way we can do dir's, more's and stuff
	if (inputString.at(0) == '!')
	{
		system(inputString.c_str() + 1);
		return true;
	}

	std::string cmdName;	//we need a name/value pair to split the input
	std::string cmdValue;	//command into.  

	//cmdName is everything up to the first space in the inputString
	cmdName = inputString.substr( 0, inputString.find(" ", 0) );

	//cmdValue is everything after the first space
	cmdValue = inputString.substr( inputString.find(" ", 0)+1, inputString.size() );

	if (cmdName == "quit" || cmdName == "exit")
	{
		m_doQuit = true;
		
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

	if (cmdName == "exec")
	{
		return doExec(cmdValue);
	}

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
	}

	if (m_currentModule != 0)
	{
		// now see if the module can execute the command
		ISimulationCommandProcessor& cmd = m_currentModule->getCommandProcessor();

		if ( &cmd != 0 )
		//if(cmd)
		{
			bool cmdOK = true;

			if (cmd.hasCommandName(cmdName))
			{
				if (!cmd.execute(cmdName, cmdValue))
				{
					if (cmd.errorMsg() != "")
					{
						std::cout << "ERROR:" << std::endl << cmd.errorMsg() << std::endl;
					}
					std::cout << "Usage:" << std::endl << cmd.usageString(cmdName) << std::endl;

					cmdOK = false;
				}
			}
			else
			{
				std::cout << "Unknown command: " << cmdName << std::endl;
				cmdOK = false;
			}

			//delete cmd;

			return cmdOK;
		}
		else
		{
			std::cout << "Current module does not have a command processor." << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "No current module." << std::endl;
		return false;
	}

	// obligatory Half-Life quote:
	std::cout << "This can't be happening to me." << std::endl;
	return false;
}


void CommandMonitor::listModules()
{
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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


bool CommandMonitor::switchModule(std::string &moduleName)
{
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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

		std::cout << "Module '" << moduleName << "' not found.  Check for no extra spaces." << std::endl;
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
	std::cout << "start {all} - start current module or all modules" << std::endl;
	std::cout << "stop {all} - stop current module or all modules" << std::endl;
	std::cout << "restart {all} - restart current module or all modules" << std::endl;
	std::cout << "exec <script> - run a command script" << std::endl;
	std::cout << "sleep <milliseconds> - sleep for a while" << std::endl;
	std::cout << "help - show this (and current module help)" << std::endl;
	std::cout << "reset - reset the simulation, all modules destroyed" << std::endl;
	std::cout << "quit, exit - quit simulator" << std::endl;
	std::cout << "lines starting with '!' are executed by the system command processor." << std::endl;
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
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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
	std::vector<ISimulationModule *>& modules = m_simulator->getSimulationModules();
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
		std::cout << "ERROR: invalid timeout specification" << std::endl;
		std::cout << "Usage:" << std::endl << "sleep <timeout millis>" << std::endl;
		return false;
	}

	// traveller, rest yon weary head
	TA_Base_Core::Thread::sleep(timeout);

	return true;
}


bool CommandMonitor::doAddmod(const std::string& cmdValue)
{
	// TODO:  hackish.  Unclean.  Move to a factory.
	std::string name;
	std::string type;
	std::string parameters;

	
	int param = 0;
	for (unsigned int index=0;index<cmdValue.size();index++)
	{
		char   character = cmdValue[index];

		if ((param<2)&&(character==' '))
		{
			param++;
		}
		else
		{
			if (param==0)
			{
				name+=character;
			}
			else if (param==1)
			{
				type+=character;
			}
			else
			{
				parameters+=character;
			}
		}
	}

	// Check name and type specified
	if (name.size()<1)
	{
		std::cout << "Module name not specified." << std::endl;
		return false;
	}

	if (type.size()<1)
	{
		std::cout << "Module type not specified." << std::endl;
		return false;
	}

//	int numTokens = sscanf(cmdValue.c_str(), "%s %s %s %s", name, type, dg, port);
	bool ok = true;
//	if (numTokens >= 2)
//  {


	if (m_simulator->getModuleByName(name))
	{
		std::cout << "ERROR: there is already a module named " << name << std::endl;
		ok = false;
	}
	else
	{
		ISimulationModule* simModule = createSimulationModule(type,name,parameters);
		if (simModule==NULL)
		{
			 ok = false;
		}
		else
		{
			m_simulator->addModule(simModule);
			m_currentModule = simModule;
		}
	}
	
	if (!ok)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "addmod <name> <type>" << std::endl;
		return false;
	}

	return ok;
}


bool CommandMonitor::doReset(const std::string& cmdValue)
{
	std::cout << "Resetting..." << std::endl;
	m_currentModule=NULL;

	// Stop any current simulation
	m_simulator->stopSimulation();

	// Remove all the simulation modules
	std::vector<ISimulationModule *> modules = m_simulator->getSimulationModules();
	std::vector<ISimulationModule *>::iterator it = modules.begin();
	while (it != modules.end())
	{
		ISimulationModule* module = *it;
		m_simulator->removeModule(module);
		delete module;
		it++;
	}

	std::cout << "done." << std::endl;

	return true;
}
