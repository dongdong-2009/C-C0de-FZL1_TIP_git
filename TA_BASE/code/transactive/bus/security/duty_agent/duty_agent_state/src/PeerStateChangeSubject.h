#ifndef PEERSTATECHANGESUBJECT_H_INCLUDED
#define PEERSTATECHANGESUBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/PeerStateChangeSubject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Publishes state changes to interested observers. These state changes come from
  *  the duty agent "peer" which means they will be published for every duty agent
  *  involved in a duty change.
  *
  *  There is also a front-end duty change subject which will only triggered at the
  *  Duty Agent that receives the initial method invocation.
  */

namespace TA_Base_Bus
{
	class PeerStateChangeSubjectImpl : public SeDutyStateSubject<PeerStateChangeSubjectImpl>
	{
	public:

		friend class ACE_Singleton<PeerStateChangeSubjectImpl, ACE_Recursive_Thread_Mutex>;

		void update( const DutyState& state );

	 
	// 	DutyState* getStateUpdate() const { return(m_state.ts_object()); }

	private:

		PeerStateChangeSubjectImpl()  {}
		~PeerStateChangeSubjectImpl(){}

		// Don't want to copy a Singleton
		PeerStateChangeSubjectImpl( const PeerStateChangeSubjectImpl& );
		PeerStateChangeSubjectImpl& operator=( const PeerStateChangeSubjectImpl& );

		// ACE_TSS<DutyState> m_state;
	};

	typedef ACE_Singleton<PeerStateChangeSubjectImpl, ACE_Recursive_Thread_Mutex> PeerStateChangeSubject;

}

#endif // PEERSTATECHANGESUBJECT_H_INCLUDED
