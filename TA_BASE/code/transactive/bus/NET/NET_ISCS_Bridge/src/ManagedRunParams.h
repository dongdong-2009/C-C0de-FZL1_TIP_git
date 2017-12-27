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


#pragma once
#include <vcclr.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

#include "core\utilities\src\RunParams.h"


namespace NET_ISCS_Bridge 
{
	//forward declaration
	class ManagedRunParamUser;

	public ref class IRunParameterObserver abstract
	{
	public:
		virtual void onRunParamChange(String^ name, String^ value) = 0; 
		void SetUnmanagedRunParamUser(ManagedRunParamUser* obj)
		{
			m_pRunParamUser = obj;
		}
		ManagedRunParamUser* GetUnmanagedRunParamUser()
		{
			return m_pRunParamUser;
		}
	private:
		ManagedRunParamUser* m_pRunParamUser;
	};

	public class ManagedRunParamUser: public TA_Base_Core::RunParamUser
	{
	public:

		ManagedRunParamUser(IRunParameterObserver^ observer)
		{
			m_observer = observer;
			m_observer->SetUnmanagedRunParamUser(this);
		}

		~ManagedRunParamUser()
		{
			//to check to delete obervers
		}


		//event delegate to define here
		void onRunParamChange(const std::string& name, const std::string& value);		

	private:
		gcroot<IRunParameterObserver^> m_observer;

	};



	public ref class ManagedRunParams
	{
	public:

		static ManagedRunParams^ GetInstance();
		
		static void ReleaseInstance();

		~ManagedRunParams();

		/**
		  * set
		  * 
		  * Add a parameter
		  *
		  * @param name Name of parameter
          * @param value Value of parameter
          *
          * Pre: name and value are not NULL
		  */
		void Set(String^ name, String^ value);

        /**
		  * get
		  * 
		  * Retrieve a parameter value
		  *
          * @return Value of parameter
          *
		  * @param name Name of parameter
          *
          * Pre: name is not NULL
          */
		String^ Get(String^ name);

		/**
		  * isSet
		  * 
		  * Determine whether a parameter with the given name has been set
		  *
          * @return True (parameter set), False (parameter not set)
          *
		  * @param name Name of parameter
          *
          * Pre: name is not NULL
          */
		bool IsSet(String^ name);

		/**
          * registerRunParamUser
          *
		  * Register this given callback object against the parameter with the
		  * given name.
          *
          * @param user Pointer to RunParamUser object
          * @param name Name of the parameter
          *
          * Pre: The RunParamUser cannot be NULL
          *      The name cannot be NULL
          */
		void registerRunParamUser(IRunParameterObserver^ user, String^ name);

        /**
          * deregisterRunParamUser
          *
		  * Cleanup any callback registrations associated with this object
		  * Note: this used to be called Deregister().
          *
          * @param user Pointer to RunParamUser object
          *
          * Pre: The RunParamUser cannot be NULL
          */
		void deregisterRunParamUser(IRunParameterObserver^ user);

	private:

		ManagedRunParams();

		static ManagedRunParams^ m_instance;

	};




}