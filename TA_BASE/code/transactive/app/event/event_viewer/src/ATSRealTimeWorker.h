/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSRealTimeWorker.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSRealTimeWorker is ...
  *
  */
#if !defined(EA_22F53032_2C9E_4306_BAB0_C0F0CF3F57CE__INCLUDED_)
#define EA_22F53032_2C9E_4306_BAB0_C0F0CF3F57CE__INCLUDED_

#include "IEventWorker.h"
#include "IEventWorkObserver.h"


namespace TA_Base_App
{
	class ATSRealTimeWorker : public IEventWorker
	{

	public:
		ATSRealTimeWorker(IEventWorkObserver* observer);
		virtual ~ATSRealTimeWorker();
		/*virtual void run();*/
		//virtual void terminate();
		virtual void doCurrentTask();
		virtual void run();
		void startNewSubscription();
		void stopLastSubscriptionIfNeeded();
		static void setCurrentMaxKey (unsigned long key);
		static unsigned long getCurrentMaxKey();

	private:

		bool m_ARunningSubscriptionExist;
		static unsigned long currentMaxPkey;

	};
};
#endif // !defined(EA_22F53032_2C9E_4306_BAB0_C0F0CF3F57CE__INCLUDED_)
