/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorTask.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/06/18 16:11:24 $
 * Last modified by:  $Author: haijun.li $
 *
 * 
 * Manages the threads required by DutyMonitor. 
 */

#if !defined(DUTYMONITORTASK_H_INCLUDED)
#define DUTYMONITORTASK_H_INCLUDED

#include "app/security/duty_agent/src/DutyMonitor.h"

namespace TA_Base_App
{
	class DutyMonitorTask : public ACE_Task<ACE_MT_SYNCH>
	{

	public:
		/**
		 * @param dutyPeer
		 * 
		 */
		DutyMonitorTask();

		virtual ~DutyMonitorTask();
		
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

		void start();
		void stop();

		void add( TA_Base_Bus::DASType::DbKey region );
		void rem( TA_Base_Bus::DASType::DbKey region );

		void add( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem );
		void rem( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem );

	private:
		
		class StartMO : public ACE_Method_Request
		{
		public:
			StartMO( DutyMonitorTask& task );

			virtual int call();

		private:
			StartMO& operator=( const StartMO& );
			StartMO( const StartMO& );

			DutyMonitorTask& m_task;
		}; 

		class AddRegMO : public ACE_Method_Request
		{
		public:
			AddRegMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region );
			virtual int call();

		private:
			AddRegMO& operator=( const AddRegMO& );
			AddRegMO( const AddRegMO& );

			DutyMonitorTask& m_task;
			TA_Base_Bus::DASType::DbKey m_region;
		}; 

		class RemRegMO : public ACE_Method_Request
		{
		public:
			RemRegMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region );

			virtual int call();

		private:
			RemRegMO& operator=( const RemRegMO& );
			RemRegMO( const RemRegMO& );

			DutyMonitorTask& m_task;
			TA_Base_Bus::DASType::DbKey m_region;
		}; 

		class AddSubMO : public ACE_Method_Request
		{
		public:
			AddSubMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region,
				const TA_Base_Bus::DASType::DbKey& subsystem );

			virtual int call();

		private:
			AddSubMO& operator=( const AddSubMO& );
			AddSubMO( const AddSubMO& );

			DutyMonitorTask& m_task;
			TA_Base_Bus::DASType::DbKey m_region;
			TA_Base_Bus::DASType::DbKey m_subsystem;
		}; 

		class RemSubMO : public ACE_Method_Request
		{
		public:
			RemSubMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region,
				const TA_Base_Bus::DASType::DbKey& subsystem );

			virtual int call();

		private:
			RemSubMO& operator=( const RemSubMO& );
			RemSubMO( const RemSubMO& );

			DutyMonitorTask& m_task;
			TA_Base_Bus::DASType::DbKey m_region;
			TA_Base_Bus::DASType::DbKey m_subsystem;
		}; 

		friend class StartMO;
		friend class AddRegMO;
		friend class RemRegMO;
		friend class AddSubMO;
		friend class RemSubMO;

		void startImpl();
		void addImpl  ( TA_Base_Bus::DASType::DbKey region );
		void remImpl  ( TA_Base_Bus::DASType::DbKey region );
		void addImpl  ( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem );
		void remImpl  ( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem );

		ACE_Activation_Queue m_activationQueue;

		static const ACE_Time_Value m_mqTimeout;
		static const ACE_Time_Value m_aqTimeout;

		TA_Base_Core::Condition m_stopSig;
	 	ACE_Thread_Semaphore m_stopSem;
		ACE_Thread_Semaphore m_startSem;

		DutyMonitor m_dutyMonitor;

		static const unsigned long POLL_PERIOD_IN_MS;
		static const unsigned long THREAD_COUNT;
	};
} // TA_Base_Bus







#endif // !defined(DUTYMONITORTASK_H_INCLUDED)
