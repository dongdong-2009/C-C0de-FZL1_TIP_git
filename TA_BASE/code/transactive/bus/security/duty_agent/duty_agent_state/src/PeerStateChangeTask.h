// PeerStateChangeTask.h: interface for the PeerStateChangeTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEERSTATECHANGETASK_H__98652BE1_BB07_4EE8_BA37_D9CD88323980__INCLUDED_)
#define AFX_PEERSTATECHANGETASK_H__98652BE1_BB07_4EE8_BA37_D9CD88323980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
namespace TA_Base_Bus
{
	
	class PeerStateChangeTaskImpl: public ACE_Task<ACE_MT_SYNCH>  
	{
	public:
		friend class ACE_Singleton<PeerStateChangeTaskImpl, ACE_Recursive_Thread_Mutex>;
	public:
		
		PeerStateChangeTaskImpl();
		virtual ~PeerStateChangeTaskImpl();
		
		virtual int open(void*);
		/**
		* @param flags
		* 
		*/
		virtual int close(u_long flags = 0 );
		/**
		* svc  This is the entry point for the callback from ACE_Task.
		* 
		*/
		virtual int svc(void);
		
		void updateSubject(DutyState& state);
	private:
		ACE_Activation_Queue m_activationQueue;
		
		static const ACE_Time_Value m_mqTimeout;
		static const ACE_Time_Value m_aqTimeout;
		
		static const unsigned long THREAD_COUNT;
		
	};
	typedef ACE_Singleton<PeerStateChangeTaskImpl, ACE_Recursive_Thread_Mutex> PeerStateChangeTask;
}
#endif // !defined(AFX_PEERSTATECHANGETASK_H__98652BE1_BB07_4EE8_BA37_D9CD88323980__INCLUDED_)
