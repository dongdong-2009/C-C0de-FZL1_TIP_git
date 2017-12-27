#ifndef SEDUTYSTATEOBSERVER_H_INCLUDED
#define SEDUTYSTATEOBSERVER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/bus/security/security_utilities/src/SeObserver.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * An implementation of the observer part of the observer pattern.
  * This implementation allows multiple subjects to be observerved.
  *
  */

namespace TA_Base_Bus
{
	class DutyState;

	template<class Sub>
	class SeDutyStateObserver
	{
	public:
		SeDutyStateObserver( Sub& subject );
		virtual ~SeDutyStateObserver();

		virtual void update( Sub& subject,const DutyState& state ) = 0;
	
	protected:
		void attach();
		// allow the observer to detach BEFORE it is destructed
		void detach();

	private:
		Sub& m_subject;
		bool m_detached;
	};

	template<class Sub>
	SeDutyStateObserver<Sub>::SeDutyStateObserver( Sub& subject ) :
		m_subject(subject),
		m_detached(true)
	{
		attach();
	}

	template<class Sub>
	void SeDutyStateObserver<Sub>::attach()
	{
		if( m_detached )
		{
			m_subject.attachObserver(*this);
			m_detached = false;
		}
	}

	template<class Sub>
	void SeDutyStateObserver<Sub>::detach()
	{
		if( !m_detached )
		{
			m_subject.detachObserver(*this);
			m_detached = true;
		}
	}

	template<class Sub>
	SeDutyStateObserver<Sub>::~SeDutyStateObserver()
	{
		detach();
	}
}
#endif // SEDUTYSTATEOBSERVER_H_INCLUDED
