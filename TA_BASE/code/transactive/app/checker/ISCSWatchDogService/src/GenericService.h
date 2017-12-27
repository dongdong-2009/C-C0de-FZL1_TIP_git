// GenericService.h : Defines the generic functions for the service.
//

/*
 * this file is copied from bus\generic_service\src\GenericService.h, and is reduced some functions 
 * to become more simple.
 */
#ifndef _GENERIC_SERVICE_H
#define _GENERIC_SERVICE_H
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <string>

// For ACE Initialisation
#include "ace/ACE.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/process_management/src/UtilityInitialiser.h"
//#include "core/utilities/src/RunParams.h"
// // #include "core/exceptions/src/GenericAgentException.h"
// 
// #include "bus/generic_service/src/ServiceAgent.h"
// // #include "bus/generic_service/message_file/src/messages.h"
// #include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{
    const std::string INSTALL_ARGUMENT = "-install";
    const std::string REMOVE_ARGUMENT = "-remove";
    const std::string DEBUG_ARGUMENT = "--service-debug";
	const std::string DEBUG_ARGUMENT_RPARAM = "ServiceDebug";
    const std::string VERSION_ARGUMENT = "--version";


	class DummyService
	{
	public:
		DummyService()
			: m_running(false)
		{

		}

		~DummyService()
		{

		}

		void run()
		{
			m_running = true;
			while(m_running)
			{
				Sleep(100);
			}
		}

		void stop()
		{
			m_running = false;
		}

	private:
		volatile bool m_running;
	};

	template<typename SERVICEIMPL> 
	class GenericService
	{
	public:

		static GenericService<SERVICEIMPL>& getInstance()
		{
			{
				TA_Base_Core::ThreadGuard guard( m_singletonLock );

				if ( m_singleton == NULL )
				{
					m_singleton = new GenericService<SERVICEIMPL>;
				}
			}

			TA_ASSERT(m_singleton != NULL, "GenericService - Error in getInstance");

			return *m_singleton;
		}	

		void initialise(const std::string& name)
		{
			m_name = name;
			m_paramKey = "SYSTEM\\CurrentControlSet\\Services\\"+ m_name +"\\Parameters";
		}

		void run(int argc, char* argv[])
		{
			SERVICE_TABLE_ENTRY   DispatchTable[] =
			{
				{ (char *)m_name.c_str(), GenericService::staticMain },
				{ NULL, NULL }
			};

			for (int i = 1; i < argc; i++)
			{
				std::string arg = argv[i];

				if (arg[0] == '-' )
				{
					if (arg == INSTALL_ARGUMENT)
					{
						install(argc, argv);
						return;
					}
					else if (arg == REMOVE_ARGUMENT)
					{
						remove();
						return;
					}
                    else if (arg == VERSION_ARGUMENT)
                    {
                        TA_Base_Core::RunParams::getInstance().parseCmdLine(VERSION_ARGUMENT.c_str());   
                        return;
                    }
					// Only take -debug as the last arg as it simplifies processing
					else if (arg == DEBUG_ARGUMENT && i==argc-1)
					{
						// Don't try to get service handle
						m_debug = true;
						// Run the service main directly
						serviceMain(argc, argv);
					}
				}
			}

			if (!StartServiceCtrlDispatcher( DispatchTable)) 
			{ 
				std::string log = " [" + m_name + "] ";
				log += "StartServiceCtrlDispatcher error = ";
				log += " = %d\n"; 
				debugOut(log.c_str(), GetLastError()); 
			}
		}

		static void removeInstance()
		{
			TA_Base_Core::ThreadGuard guard( m_singletonLock );

			if ( m_singleton != NULL )
			{
				delete m_singleton;
				m_singleton = NULL;
			}
		}

		static VOID WINAPI staticMain(DWORD argc, LPTSTR *argv)
		{
			getInstance().serviceMain(argc, argv);
		}

		static VOID WINAPI staticCtrlHandler(DWORD opcode)
		{
			getInstance().ctrlHandler(opcode);
		}

	protected:

		GenericService() 
			: m_name (""), m_statusHandle (NULL), m_eventHandle (NULL),
			  m_serviceImpl (NULL), m_debug (false)
		{
		};

		virtual ~GenericService() {};

		VOID WINAPI serviceMain(DWORD argc, LPTSTR *argv)
		{
			//initializeUtil(argc, argv);
			TA_Base_Core::UtilityInitialiser::initialiseUtilities(argc, argv);

			if (!m_debug)
			{
				m_statusHandle = RegisterServiceCtrlHandler(m_name.c_str(), staticCtrlHandler);
				TA_ASSERT(m_statusHandle != NULL, "RegisterServiceCtrlHandler failed");

				m_eventHandle = RegisterEventSource(NULL, m_name.c_str());
				TA_ASSERT(m_eventHandle != NULL, "RegisterEventSource failed");
			}

			m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
			m_status.dwCurrentState = SERVICE_START_PENDING;
			m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
			m_status.dwWin32ExitCode = NO_ERROR;
			m_status.dwServiceSpecificExitCode = 0;
			m_status.dwCheckPoint = 1;
			m_status.dwWaitHint = 1000;

			if (m_statusHandle != NULL)
			{
				SetServiceStatus(m_statusHandle, &m_status);
			}

			HKEY regKey = NULL;
			PBYTE buffer = NULL;
			char** regArgv = NULL;

			try
			{
				// Set up the registry key
				if (argc <= 1)
				{
					// No real parameters - get them from the registry
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_paramKey.c_str(), NULL, KEY_READ, &regKey);

					if (regKey != NULL)
					{
						DWORD bufferLen = sizeof(DWORD);
						DWORD valueType;
						DWORD regArgc = 0;

						RegQueryValueEx(regKey, "argc", 0, &valueType, (PBYTE)&regArgc, &bufferLen);

						if (valueType == REG_DWORD)
						{
							RegQueryInfoKey(regKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &bufferLen, NULL, NULL);

							buffer = new BYTE [bufferLen];

							RegQueryValueEx(regKey, "argv", 0, &valueType, (PBYTE)buffer, &bufferLen);

							if (valueType == REG_MULTI_SZ)
							{
								// Need to construct an array of pointers for this
								regArgv = new char* [regArgc];
								regArgv[0] = (char *)buffer;

								for (unsigned int i = 1; i < regArgc; i++)
								{
									regArgv[i] = regArgv[i-1] + strlen(regArgv[i-1]) + 1;

                                    // This indicates the 'argc' is set to the bigger number than the actual 'argv' parameters
                                    // Just set regArgc to the correct number of parameters to avoid access out of range memory.
                                    if (0 == regArgv[i][0])
                                    {
                                        reportEvent(std::string("The argc and argv parameter are not concurrent, the setting of argc is ingored."));
                                        regArgc = i;
                                        break;
                                    }
								}

								m_serviceImpl = new SERVICEIMPL(regArgc, regArgv);
							}
						}
						else
						{
							regArgc = 0;
						}
					
						delete [] buffer;
						buffer = NULL;

						delete [] regArgv;
						regArgv = NULL;
					}
				}
				
				if (regKey == NULL)
				{
					m_serviceImpl = new SERVICEIMPL(argc, argv);
				}
				else
				{
					RegCloseKey(regKey);
					regKey = NULL;
				}

				
				m_serviceImpl->startService();

				m_status.dwCurrentState = SERVICE_RUNNING;
				m_status.dwCheckPoint = 0;
				m_status.dwWaitHint = 0;

				if (m_statusHandle != NULL)
				{
					SetServiceStatus(m_statusHandle, &m_status);
				}

				// here will block program exit.
				m_dummyService.run();
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				reportEvent(te.what());
                exit(0); //exit directly to force service manager to restart me again
			}
			catch( ... )
			{
				reportEvent(std::string("Unexpected exception during running"));
                exit(0); //exit directly to force service manager to restart me again
			}

			m_status.dwCurrentState = SERVICE_STOP_PENDING;
			m_status.dwCheckPoint = 2;
			m_status.dwWaitHint = 20000;

			if (m_statusHandle != NULL)
			{
				SetServiceStatus(m_statusHandle, &m_status);
			}

			if (regKey != NULL)
			{
				RegCloseKey(regKey);
			}

			if (buffer != NULL)
			{
				delete [] buffer;
			}
			
			if (regArgv != NULL)
			{
				delete [] regArgv;
			}


			m_status.dwCurrentState = SERVICE_STOP_PENDING;
			m_status.dwCheckPoint = 3;
			m_status.dwWaitHint = 10000;

			if (m_statusHandle != NULL)
			{
				SetServiceStatus(m_statusHandle, &m_status);
			}

			try
			{
				if (m_serviceImpl != NULL)
				{
					delete m_serviceImpl;
					m_serviceImpl = NULL;
				}
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				reportEvent(te.what());
			}
			catch( ... )
			{
				reportEvent(std::string("Unexpected exception during deleting ServiceAgentUser"));
			}
			
			m_status.dwCurrentState = SERVICE_STOPPED;

			if (m_statusHandle != NULL)
			{
				SetServiceStatus(m_statusHandle, &m_status);
			}

			if (m_eventHandle != NULL)
			{
				DeregisterEventSource(m_eventHandle);
				m_eventHandle = NULL;
			}

			m_statusHandle = NULL;
		}

		void reportEvent(const std::string& error)
		{
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Fatal Error occured %s", error.c_str());
		}

		VOID WINAPI ctrlHandler(DWORD opcode)
		{
			// Uncomment this to debug as a service
			// Alternatively, add -debug to the end of the command line to run as an app
			//DebugBreak();

			switch (opcode)
			{
			case SERVICE_CONTROL_STOP:
			case SERVICE_CONTROL_SHUTDOWN:

				m_status.dwCurrentState = SERVICE_STOP_PENDING;
				m_status.dwCheckPoint = 1;
				m_status.dwWaitHint = 20000;

				if (m_statusHandle != NULL)
				{
					SetServiceStatus(m_statusHandle, &m_status);
				}

				try
				{
					m_serviceImpl->stopService();
					m_dummyService.stop();
				}
				catch( ... )
				{
					reportEvent(std::string("Unexpected exception during ServiceAgent Terminating"));
				}
				
				break;
			
			case SERVICE_CONTROL_INTERROGATE:
				m_status.dwCheckPoint = 0;
				m_status.dwWaitHint = 0;

				if (m_statusHandle != NULL)
				{
					SetServiceStatus(m_statusHandle, &m_status);
				}
				break;

			case SERVICE_CONTROL_PAUSE:
			case SERVICE_CONTROL_CONTINUE:
			case SERVICE_CONTROL_PARAMCHANGE:
			case SERVICE_CONTROL_NETBINDADD:
			case SERVICE_CONTROL_NETBINDREMOVE:
			case SERVICE_CONTROL_NETBINDENABLE:
			case SERVICE_CONTROL_NETBINDDISABLE:
			default:
				// Not used
				break;
			}
		}

		void install(DWORD argc, LPTSTR *argv)
		{
			SC_HANDLE scmHandle = NULL;
			SC_HANDLE serviceHandle = NULL;
			HKEY regKey = NULL;

			try
			{
				// Open the SCM on this machine
				scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

				if (scmHandle != NULL)
				{
					// Get the full pathname
					TCHAR modulePathname[_MAX_PATH * 2];
					GetModuleFileName(NULL, modulePathname, sizeof(modulePathname)/sizeof(modulePathname[0]));

					// Add this service to the SCM's database.
					serviceHandle = CreateService(scmHandle, m_name.c_str(), m_name.c_str(),
													SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
													SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
													modulePathname, NULL, NULL, NULL, NULL, NULL);

					if (serviceHandle != NULL)
					{
                        // Convert std::string to char*
                        char* description = new char[m_name.size() + 1];
                        strcpy(description, m_name.c_str());

						SERVICE_DESCRIPTION sd = 
						{ 
							TEXT(description)
						};

						ChangeServiceConfig2(serviceHandle, SERVICE_CONFIG_DESCRIPTION, &sd);
                        delete description;

						SC_ACTION sca[3] = { {SC_ACTION_RESTART,1000}, {SC_ACTION_RESTART,1000}, {SC_ACTION_RESTART,1000} };
						
						SERVICE_FAILURE_ACTIONS sfa =
						{
							86400, // 1 Day
							NULL,
							NULL,
							3,
							sca
						};
						
						ChangeServiceConfig2(serviceHandle, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
					}
				}

				// Set up the registry key
				RegCreateKeyEx(HKEY_LOCAL_MACHINE, m_paramKey.c_str(), 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_WRITE, 
					NULL, &regKey, NULL);

				if (regKey != NULL)
				{
					// Construct argc/argv from the current parameter list
					std::string myArgv = argv[0];
					myArgv += '\0';
					DWORD myArgc = 1;

					for (unsigned int i = 1; i < argc; i++)
					{
						std::string arg = argv[i];

						if (arg[0] == '-' && arg[1] == '-')
						{
							myArgv += arg;
							myArgv += '\0';
							myArgc++;
						}
					}

					myArgv += '\0';

					RegSetValueEx(regKey, "argc", 0, REG_DWORD, (PBYTE)&myArgc, sizeof(DWORD));
					RegSetValueEx(regKey, "argv", 0, REG_MULTI_SZ, (PBYTE)myArgv.c_str(), myArgv.size()*sizeof(char));
				}

				if (regKey != NULL)
				{
					RegCloseKey(regKey);
					regKey = NULL;
				}

				// Set up the messages registry key
				std::string messagesKey = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"+ m_name;
				RegCreateKeyEx(HKEY_LOCAL_MACHINE, messagesKey.c_str(), 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_WRITE, 
					NULL, &regKey, NULL);

				if (regKey != NULL)
				{
					DWORD types = EVENTLOG_ERROR_TYPE;
					RegSetValueEx(regKey, "TypesSupported", 0, REG_DWORD, (PBYTE)&types, sizeof(DWORD));

					std::string eventMessageFileLocation = "c:\\TransActive\\lib\\ServiceMessageFile.dll";
					RegSetValueEx(regKey, "EventMessageFile", 0, REG_EXPAND_SZ, (PBYTE)eventMessageFileLocation.c_str(), eventMessageFileLocation.size()*sizeof(char));

					DWORD categoryCount = 0;
					RegSetValueEx(regKey, "CategoryCount", 0, REG_DWORD, (PBYTE)&categoryCount, sizeof(DWORD));

				}
			}
			catch (...)
			{
				// Ensure clean-up occurs
			}

 			if (scmHandle != NULL)
			{
				CloseServiceHandle(scmHandle);
			}

			if (serviceHandle != NULL)
			{
				CloseServiceHandle(serviceHandle);
			}

			if (regKey != NULL)
			{
				RegCloseKey(regKey);
				regKey = NULL;
			}
		}

		void remove() 
		{
			SC_HANDLE scmHandle = NULL;
			SC_HANDLE serviceHandle = NULL;

			try
			{
				// Remove the parameters from the registry
				RegDeleteKey(HKEY_LOCAL_MACHINE, m_paramKey.c_str());
				std::string messagesKey = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"+ m_name;
				RegDeleteKey(HKEY_LOCAL_MACHINE, messagesKey.c_str());

				// Open the SCM on this machine.
				scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

				if (scmHandle != NULL)
				{
					// Open this service for DELETE access
					serviceHandle = OpenService(scmHandle, m_name.c_str(), DELETE);

					if (serviceHandle != NULL)
					{
						// Remove this service from the SCM's database.
						DeleteService(serviceHandle);
					}
				}
			}
			catch (...)
			{
				// Ensure clean-up occurs
			}

 			if (scmHandle != NULL)
			{
				CloseServiceHandle(scmHandle);
			}

			if (serviceHandle != NULL)
			{
				CloseServiceHandle(serviceHandle);
			}
		}

		VOID debugOut(LPCSTR String, DWORD Status) 
		{ 
			CHAR  Buffer[1024]; 
			if (strlen(String) < 1000) 
			{ 
				sprintf(Buffer, String, Status); 
				OutputDebugString(Buffer); 
			}
		}

		bool initializeUtil(int argc, char* argv[])
		{
			try
			{
				if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv) )
				{
					if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_VERSION))
					{
						// Versioning info printed to stdout. Not an exceptional condition,
						// but there's no need to continue.
						return false;
					}
					
					TA_THROW (TA_Base_Core::InvalidCommandLineException
						("Command line arguments are in invalid format"));
				}

				ACE::init();
				TA_Base_Core::gSetDebugUtilFromRunParams();
				TA_Base_Core::gInitStructuredExceptionHandler();   // Only initialises the calling thread
			}
			catch( const TA_Base_Core::UtilitiesException& )
			{
				throw; // Ensuring we only throw the exceptions listed in the documentation
			}
			catch( const TA_Base_Core::InvalidCommandLineException& )
			{
				throw; // Ensuring we only throw the exceptions listed in the documentation
			}
			catch( ... )
			{
                TA_THROW( TA_Base_Core::UtilitiesException("One or more utilities failed to initialise") );
			}
			
			return true;
		}

	protected:
		std::string										m_name;
		std::string										m_paramKey;
		SERVICE_STATUS									m_status; 
		SERVICE_STATUS_HANDLE							m_statusHandle;
		HANDLE											m_eventHandle;
		SERVICEIMPL*									m_serviceImpl;
		DummyService									m_dummyService;
		bool											m_debug;
		static TA_Base_App::GenericService<SERVICEIMPL>*	m_singleton;
		static TA_Base_Core::ReEntrantThreadLockable			m_singletonLock;

	};
}

#endif //

