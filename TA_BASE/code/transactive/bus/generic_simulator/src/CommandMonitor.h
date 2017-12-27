#if !defined(AFX_COMMANDMONITOR_H__BE504C32_681F_438D_96C1_F48F87F3FEF7__INCLUDED_)
#define AFX_COMMANDMONITOR_H__BE504C32_681F_438D_96C1_F48F87F3FEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_simulator/src/CommandMonitor.h $
  * @author:  Ross Mckillop
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * CommandMonitor provides a new thread for the simulator, to listen for and 
  * respond to user commands in runtime.
  */
#pragma warning(disable : 4786)

#include <string>

#define RPARAM_STARTUP_SCRIPT   "StartupScript"

namespace TA_Base_Bus
{

	class ISimulationModule;
	class GenericSimulator;

	class CommandMonitor  
	{
		public:
			CommandMonitor(const std::string& desc, GenericSimulator *simulator);
			virtual ~CommandMonitor();

			bool monitorUserInput();
			bool executeScript(const std::string& filename);

			// Override this method to create the supported simulation classes
			virtual ISimulationModule* createSimulationModule(const std::string& type,
															  const std::string& name,
															  const std::string& parameters)=0;

		private:

			bool parseCommand(std::string& inputString);
			void listModules();
			bool switchModule(std::string &moduleName);
			void printHelp();

			bool doExec(const std::string& cmdValue);
			bool doStart(const std::string& cmdValue);
			bool doRestart(const std::string& cmdValue);
			bool doStop(const std::string& cmdValue);
			bool doSleep(const std::string& cmdValue);
			bool doAddmod(const std::string& cmdValue);
			bool doReset(const std::string& cmdValue);

			std::string			m_desc;
			ISimulationModule*	m_currentModule;
			GenericSimulator*	m_simulator;

			bool m_doQuit;
	};
};  // Namespace

#endif // !defined(AFX_COMMANDMONITOR_H__BE504C32_681F_438D_96C1_F48F87F3FEF7__INCLUDED_)

