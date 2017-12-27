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

#pragma once

#include "IProcessStateObserver.h"
#include "NETGUIApplication.h"
#include "bus\generic_gui\src\GenericGUI.h"
#using <System.Windows.Forms.dll>
using namespace System;
using namespace System::Windows::Forms;


namespace NET_ISCS_Bridge 
{

	public ref class TransactiveNetApp abstract:public IProcessStateObserver
	{
	public:
		TransactiveNetApp();
		~TransactiveNetApp();
		virtual Form^ CreateMainForm() = 0;
		virtual void Exit();
		virtual void MainFormClose(Object ^sender, FormClosedEventArgs ^e);
		void ResizeForm(Object^ sender, EventArgs^ e);
		virtual void PreApplicationClose() = 0;
		virtual void ShowApplication();
		virtual void SetMainFrmType()=0;
		
		//todo check visibility
		virtual void RegisterFormCloseMsg();
		virtual void Run() ;
		virtual void SetupISCSRuntime(); 
		void StartApplication(String^ applicationName, long applicationType);
		virtual void UIThreadExceptionHandler();
		String^ getGuiEntityParameterValue(String^ parametername);

		TA_Base_Core::IEntityData* getGuiEntity()
		{
			return m_guiEntity;
		}
				
		/****************************** IProcessStateObserver methods *************************************************/
	public:
		virtual void CheckCommandLine() = 0;
		virtual int DutyChanged() = 0;
		virtual void EntityChanged() =0 ;
		virtual void OperationModeChanged();
		virtual void PreTerminateApplication(TA_Base_Core::EManagedProcessTerminateCode TerminateState);
		virtual void ServerStateChange(bool ServerState) = 0;
		virtual void TerminateApplication(); 
		virtual long GetApplicationtype();
		virtual void setGuiEntity(TA_Base_Core::IEntityData* guiEntity);
		virtual void onInitGenericGUICompleted() = 0;

		/********************************************************************************************************/

	private:
		TA_Base_Bus::IGUIApplication* m_pGuiApplication;
		TA_Base_Bus::GenericGUI* m_pGenericGui;
		String^ m_applicationName;
		long m_nApplicationType;
		TA_Base_Core::IEntityData* m_guiEntity;

        //application termination delegate
        delegate void ApplicationExitCallback();
		void TerminateGUI();
		void DisplayVersionInfo();
	protected: 
		Form^ m_pMainFrm;
		bool m_bTerminateFromCtrlStat;
		Type^ m_assemblytype;
	};
}