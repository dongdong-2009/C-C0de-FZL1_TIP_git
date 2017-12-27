#ifndef FRONTENDSTATECHANGESUBJECT_H_INCLUDED
#define FRONTENDSTATECHANGESUBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/FrontEndStateChangeSubject.h $
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
  *  There is also a peer duty change subject which will only triggered from
  *  the LocalDutyPeer.
  */

namespace TA_Base_Bus
{

	class FrontEndStateChangeSubjectImpl : public SeDutyStateSubject<FrontEndStateChangeSubjectImpl>
	{
	public:

		friend class ACE_Singleton<FrontEndStateChangeSubjectImpl, ACE_Recursive_Thread_Mutex>;
		friend class FrontEndStateChangeSubjectWorkItem;

		void update( const DutyState& state );


// 		DutyState* getStateUpdate() const { 
// 			
// 			return(m_state.ts_object());
// 		}

	private:

		FrontEndStateChangeSubjectImpl() ;
		~FrontEndStateChangeSubjectImpl();

		// Don't want to copy a Singleton
		FrontEndStateChangeSubjectImpl( const FrontEndStateChangeSubjectImpl& );
		FrontEndStateChangeSubjectImpl& operator=( const FrontEndStateChangeSubjectImpl& );

		TA_Base_Core::ThreadPoolSingletonManager * m_threadPool;
	// 	ACE_TSS<DutyState> m_state;
	};

	typedef ACE_Singleton<FrontEndStateChangeSubjectImpl, ACE_Recursive_Thread_Mutex> FrontEndStateChangeSubject;
	
	class FrontEndStateChangeSubjectWorkItem: public TA_Base_Core::IWorkItem
	{
	public:
		FrontEndStateChangeSubjectWorkItem(FrontEndStateChangeSubjectImpl& parent,boost::shared_ptr<DutyState> state);
		virtual ~FrontEndStateChangeSubjectWorkItem();
		virtual void executeWorkItem();
	private:
		FrontEndStateChangeSubjectImpl& parent_;
		boost::shared_ptr<DutyState>  state_;
	};
}

#endif // FRONTENDSTATECHANGESUBJECT_H_INCLUDED
