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
  * This class is C++/CLI Wrapper for Runtime Parameter Utiltiy in ISCS.
  */

#include "stdafx.h"
#include "ManagedRunParams.h"

using namespace System::Runtime::InteropServices;

namespace NET_ISCS_Bridge 
{

	//****************** ManagedRunParams Class members ********************************************************//

	ManagedRunParams::ManagedRunParams()
	{
		//m_observers = gcnew array<ManagedRunParamUser*> ();
	}

	ManagedRunParams::~ManagedRunParams()
	{
		//todo deregistere and delete indiviaial item in list

		//m_observers.Clear();

		TA_Base_Core::RunParams::removeInstance();
	}

	ManagedRunParams^ ManagedRunParams::GetInstance()
	{
		if(m_instance == nullptr)
		{
			m_instance = gcnew ManagedRunParams();
		}
		return m_instance;
	}


	void ManagedRunParams::ReleaseInstance()
	{
		if(m_instance != nullptr)
		{
			delete m_instance;
			m_instance = nullptr;
		}		
	}



	void ManagedRunParams::Set(String^ name, String^ value)
	{
		char* unmanagedName = (char*)Marshal::StringToHGlobalAnsi((String ^)name).ToPointer();	
		char* unmanagedValue = (char*)Marshal::StringToHGlobalAnsi((String ^)value).ToPointer();	

		TA_Base_Core::RunParams::getInstance().set(unmanagedName,unmanagedValue);


		Marshal::FreeHGlobal(IntPtr((void*)unmanagedName));
		Marshal::FreeHGlobal(IntPtr((void*)unmanagedValue));

	}


	String^ ManagedRunParams::Get(String^ name)
	{
		char* unmanagedName = (char*)Marshal::StringToHGlobalAnsi((String ^)name).ToPointer();	
		std::string value;
		value = TA_Base_Core::RunParams::getInstance().get(unmanagedName);

		String ^ managedValue = Marshal::PtrToStringAnsi((IntPtr) (char *) value.c_str());	

		Marshal::FreeHGlobal(IntPtr((void*)unmanagedName));

		return managedValue;
	}


	bool ManagedRunParams::IsSet(String^ name)
	{
		char* unmanagedName = (char*)Marshal::StringToHGlobalAnsi((String ^)name).ToPointer();	
		bool ret = TA_Base_Core::RunParams::getInstance().isSet(unmanagedName);
		Marshal::FreeHGlobal(IntPtr((void*)unmanagedName));
		return ret;
	}


	void ManagedRunParams::registerRunParamUser(IRunParameterObserver^ user, String^ name)
	{
		char* unmanagedName = (char*)Marshal::StringToHGlobalAnsi((String ^)name).ToPointer();	
		TA_Base_Core::RunParamUser * runParamUser = new ManagedRunParamUser(user);
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(runParamUser,unmanagedName);
		Marshal::FreeHGlobal(IntPtr((void*)unmanagedName));
	}


	void ManagedRunParams::deregisterRunParamUser(IRunParameterObserver^ user)
	{
		TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(user->GetUnmanagedRunParamUser());
	}





//***************************** ManagedRunParamUser methods *****************************************************
	
	void ManagedRunParamUser::onRunParamChange(const std::string& name, const std::string& value)
	{
		String ^ managedName = Marshal::PtrToStringAnsi((IntPtr) (char *) name.c_str());	
		String ^ managedValue = Marshal::PtrToStringAnsi((IntPtr) (char *) value.c_str());	
		m_observer->onRunParamChange(managedName,managedValue);
	}


}