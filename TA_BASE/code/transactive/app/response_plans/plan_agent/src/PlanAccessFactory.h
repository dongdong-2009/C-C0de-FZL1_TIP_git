#ifndef PLANACCESSFACTORY_H_INCLUDED
#define PLANACCESSFACTORY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <string>
#include <list>

#include "ace/Singleton.h"

namespace TA_Base_App
{
    class IActivePlanFlow;
    class ActiveStep;

	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanAccessFactory" and that "PlanAccessFactoryImpl" can
	// not be used.
	class PlanAccessFactoryImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanAccessFactoryImpl, ACE_Recursive_Thread_Mutex>;

		// A list of steps that can be executed
        void getActiveStepList( const unsigned long plan, IActivePlanFlow& flowControl, std::string& reason, std::list<ActiveStep*>& lstSteps ) const;
	
	// Protected methods
	// Private methods
	private:
		PlanAccessFactoryImpl();
		~PlanAccessFactoryImpl(){}

		PlanAccessFactoryImpl( const PlanAccessFactoryImpl& );
		PlanAccessFactoryImpl& operator=( const PlanAccessFactoryImpl& );

	}; // PlanAccessFactoryImpl
	typedef ACE_Singleton<PlanAccessFactoryImpl, ACE_Recursive_Thread_Mutex> PlanAccessFactory;
} // TA_Base_App

#endif //PLANACCESSFACTORY_H_INCLUDED
