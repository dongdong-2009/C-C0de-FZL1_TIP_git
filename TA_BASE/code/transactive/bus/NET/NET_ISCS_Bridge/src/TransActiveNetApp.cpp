/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source: $File: $
* @author 
* @version $Revision:  $
* Last modification: $DateTime: $
* Last modified by:  $Author:  $
* 
* This class is the main class for DOTNET application to use exiting ISCS GUI framework.
* All DOTNET Application needs to extends this abstract class to using ISCS GUI framework.
*/

#include "stdafx.h"

#include "TransactiveNetApp.h"
#include "core/naming/src/Naming.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "ace/ACE.h"
#include "GUINotificationObserver.h"
#include "core/utilities/src/DebugUtil.h"
#include "AssemblyInfoHelper.h"
#include "VersionInfoForm.h"


using namespace System::Runtime::InteropServices;
using namespace System::Reflection;

using TA_Base_Core::DebugUtil;

namespace NET_ISCS_Bridge 
{

	TransactiveNetApp::TransactiveNetApp()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "TransactiveNetApp Constructor" );
		m_bTerminateFromCtrlStat = false;
		m_guiEntity = NULL;
		LOG( SourceInfo, DebugUtil::FunctionExit, "TransactiveNetApp Constructor" );
	}

	TransactiveNetApp::~TransactiveNetApp()
	{
		if(m_pGuiApplication != NULL)
		{
			delete m_pGuiApplication;
			m_pGuiApplication	 = NULL;
		}
		if(NULL != m_pGenericGui)
		{
			delete m_pGenericGui;
			m_pGenericGui = NULL;
		}
		if(m_guiEntity != NULL)
		{
			delete m_guiEntity;
			m_guiEntity = NULL;
		}
	}

	void TransactiveNetApp::StartApplication(String^ applicationName, long applicationType)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "StartApplication" );
		m_applicationName = applicationName;
		m_nApplicationType = applicationType;
		char* unmanagedName;
		try
		{

			m_pGuiApplication = new NETGUIApplication(this);
			GUINotificationObserver^ observer = gcnew GUINotificationObserver();
			((NETGUIApplication *)m_pGuiApplication)->RegisterGuiNotificationObserver(observer);

			//Call to get assembly information file of that type
			SetMainFrmType();

			array<String^>^ arguments = Environment::GetCommandLineArgs();
			String^ commandLineArgs = String::Join(" ",arguments);
			unmanagedName = (char*)Marshal::StringToHGlobalAnsi((String ^)commandLineArgs).ToPointer();
			std::string unmanagedCommandLine = unmanagedName;

			//Checking Command line for Version .. if so display Information  and quit...
			if( commandLineArgs->ToUpper()->Contains("VERSION"))
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Retriving Version Information...");
				DisplayVersionInfo();
				return;
			}
			
			m_pMainFrm = CreateMainForm();
			observer->SetMainFrm(m_pMainFrm);
			RegisterFormCloseMsg();
		
			m_pGenericGui = new TA_Base_Bus::GenericGUI(*m_pGuiApplication, unmanagedCommandLine);	
				

			ShowApplication();

			if (! m_pGenericGui->init())
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Error while Init Generic GUI... Application closes");
				System::Windows::Forms::MessageBox::Show( "Something wrong in initializing the component. please contact system administrator.", "Transactive",MessageBoxButtons::OK,MessageBoxIcon::Stop)	;
				m_pMainFrm->Close();
				exit(0);
			}

			// Application start
			Run();

			Marshal::FreeHGlobal(IntPtr((void*)unmanagedName));
		}
		catch(Exception ^e)
		{
			char* unmanedexception = (char*)Marshal::StringToHGlobalAnsi((String ^)e->ToString()).ToPointer();			
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,unmanedexception );
			Marshal::FreeHGlobal(IntPtr((void*)unmanedexception));
		}
		catch(...)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"Exception Catched");
		}
		LOG( SourceInfo, DebugUtil::FunctionExit, "StartApplication" );
	}

	void TransactiveNetApp::Exit()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "Exit" );
		if (m_pGenericGui != NULL)
		{			
			if (m_pGuiApplication != NULL)
			{
				delete m_pGuiApplication;
				m_pGuiApplication = NULL;
			}

			m_pGenericGui->shutdown();
			m_pGenericGui->deactivateAndDeleteServant();
			// Don't need to delete m_genericGUI as the call above does this.
		}
		TA_Base_Core::Naming::cleanUp();
		// Clean up the Event Consumers
		TA_Base_Core::gDestroySubscriptionManager();
		TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
		TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

		try
		{
			//TA_Base_Core::OCIAccess::removeInstance();
			TA_Base_Core::CorbaUtil::getInstance().cleanup();
		}
		catch( ... )
		{
		}

		ACE::fini();

		LOG( SourceInfo, DebugUtil::FunctionExit, "Exit");
		return;
	}


	void TransactiveNetApp::PreTerminateApplication(TA_Base_Core::EManagedProcessTerminateCode TerminateState)
	{		
		LOG( SourceInfo, DebugUtil::FunctionEntry, "PreTerminateApplication" );
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Application PreTerminateApplication called...");
		// Called from control station
		m_bTerminateFromCtrlStat = true;
		PreApplicationClose();

		//Close MainFrm and Application
		TerminateGUI();

		TerminateApplication();
		m_bTerminateFromCtrlStat = false;
		LOG( SourceInfo, DebugUtil::FunctionExit, "PreTerminateApplication");
	}

	void TransactiveNetApp::TerminateGUI()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "TerminateGUI" );
		if(m_pMainFrm != nullptr)
		{
			//Close GUI FORM
			if (m_pMainFrm->InvokeRequired)
			{
				m_pMainFrm->BeginInvoke(gcnew ApplicationExitCallback(this,&TransactiveNetApp::TerminateGUI));
			}
			else
			{
				m_pMainFrm->Close();		
				System::Windows::Forms::Application::Exit();//->Exit();
			}
		}

		LOG( SourceInfo, DebugUtil::FunctionExit, "TerminateGUI" );
	}


	void TransactiveNetApp::OperationModeChanged()
	{
		//TODO
	}


	long TransactiveNetApp::GetApplicationtype()
	{
		return  m_nApplicationType;
	}

	void TransactiveNetApp::setGuiEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		m_guiEntity = guiEntity;
	}

	String^ TransactiveNetApp::getGuiEntityParameterValue(String^ parametername)
	{
		char * unmanagedPName = (char*)Marshal::StringToHGlobalAnsi((String ^)parametername).ToPointer();
		TA_Base_Core::EntityData * entityPtr = dynamic_cast<TA_Base_Core::EntityData *> (m_guiEntity);
		std::string value = entityPtr->getHelper()->getParameter(unmanagedPName);
		//std::string value("");
		String ^ managedValue = Marshal::PtrToStringAnsi((IntPtr) (char *) value.c_str());	
		return managedValue;
	}

	void TransactiveNetApp::TerminateApplication()
	{
		//TODO - Have any generic application termination code here 
		//which will be called by termination by user and by control station.
	}


	// Main Application Form Close Handler Function.
	// DOTNET application need not have handler function for Close Event.
	void TransactiveNetApp::MainFormClose(Object^ sender, FormClosedEventArgs^ e)
	{
		if(!m_bTerminateFromCtrlStat)
		{
			PreApplicationClose();
			TerminateApplication();
		}
	}

	// Main Application Form ResizeEnd Event Handler Function.	
	void TransactiveNetApp::ResizeForm(Object^ sender, EventArgs^ e)
	{
		if(m_pGuiApplication != NULL)
		{
			((NETGUIApplication *)m_pGuiApplication)->OnMainFrmResize();
		}
	}

	//Register the Close event to MainFormClose function
	void TransactiveNetApp::RegisterFormCloseMsg()
	{
		m_pMainFrm->FormClosed += gcnew FormClosedEventHandler(this, &TransactiveNetApp::MainFormClose);
		m_pMainFrm->ResizeEnd += gcnew EventHandler(this, &TransactiveNetApp::ResizeForm);
		m_pMainFrm->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
	}


	void TransactiveNetApp::ShowApplication()
	{
		m_pMainFrm->Show();
	}
	

	void TransactiveNetApp::Run()
	{
		m_pGenericGui->run();
		Application::Run(m_pMainFrm);
	}

	void TransactiveNetApp::SetupISCSRuntime()
	{
		//TODO
	}

	void TransactiveNetApp::UIThreadExceptionHandler()
	{
		//TODO
	}
	
	void TransactiveNetApp::DisplayVersionInfo()
	{
		AssemblyInfoHelper^ assemblyInfo = gcnew AssemblyInfoHelper(m_assemblytype);
		
		// Print out all the details relevant to this particular application
	    /*Console::Write("Company Details:\t\t {0}\n",assemblyInfo->Company);
	    Console::Write("Release Number:\t\t {0}\n",assemblyInfo->Version);
	    Console::Write("Build Time:\t\t {0}\n", assemblyInfo->BuildDate->ToString());
	    Console::Write("Component:\t\t {0}\n" , assemblyInfo->Product );
	    Console::Write("Component Version:\t {0}\n" , assemblyInfo->Version);
	    Console::Write("Component Copyright:\t {0}\n" , assemblyInfo->Copyright);*/

	    VersionInfoForm^ frm = gcnew  VersionInfoForm(assemblyInfo->Version,assemblyInfo->BuildDate->ToString(),assemblyInfo->Product,assemblyInfo->Copyright);
	    Application::Run(frm);
	}
}