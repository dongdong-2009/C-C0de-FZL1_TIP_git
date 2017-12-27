#ifndef SEDUTYSTATESUBJECT_H_INCLUDED
#define SEDUTYSTATESUBJECT_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/bus/security/security_utilities/src/SeSubject.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/05/24 20:57:03 $
  * Last modified by:  $Author: shurui.liu $
  *
  * An implementation of the subject part of the observer pattern.
  * This implementation allows multiple subjects to be observerved.
  *
  */

namespace TA_Base_Bus
{
	class DutyState;
	template<typename Sub>
	class SeDutyStateSubject
	{
	public:
		typedef SeDutyStateObserver<Sub> ObserverType;

		SeDutyStateSubject() {}
		virtual ~SeDutyStateSubject() {}

        // The attach/detach methods must be named differently to
        // their SeObserver counterparts in order to avoid ambiguity
        // in situations where a subject is also an observer.
		void attachObserver(ObserverType& observer); 
		void detachObserver(ObserverType& observer);

	protected:
		void notify(Sub& subject, const DutyState & state);
        
	private:
		TA_Base_Core::NonReEntrantThreadLockable m_observerLock;

		typedef std::set<ObserverType* > ObserverSet;
		ObserverSet m_observers;
	};

	template<class Sub>
	void SeDutyStateSubject<Sub>::attachObserver( ObserverType& observer )
	{
		FUNCTION_ENTRY("attachObserver");

		TA_Base_Core::ThreadGuard guard(m_observerLock);
		m_observers.insert(&observer);
		FUNCTION_EXIT;
	}

	template<class Sub>
	void SeDutyStateSubject<Sub>::detachObserver( ObserverType& observer )
	{
		FUNCTION_ENTRY("detachObserver");

		TA_Base_Core::ThreadGuard guard(m_observerLock);
		m_observers.erase(&observer);
		FUNCTION_EXIT;
	}

	template<class Sub>
	void SeDutyStateSubject<Sub>::notify( Sub& subject,const DutyState & state)
	{
		FUNCTION_ENTRY("notify");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin notify %s", typeid(*this).name());
		m_observerLock.acquire();
		ObserverSet localObservers(m_observers);
		m_observerLock.release();

		typename ObserverSet::iterator it;
		

		for( it = localObservers.begin(); it != localObservers.end(); ++it )
		{       
			(*it)->update(subject,state);
		}
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end notify %s", typeid(*this).name());
		FUNCTION_EXIT;
	}
}

#endif // SEDUTYSTATESUBJECT_H_INCLUDED
