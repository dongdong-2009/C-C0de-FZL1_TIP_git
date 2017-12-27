/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeWorker.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSRealTimeWorker is ...
  *
  */

#if !defined(EA_06F6169A_2F42_41b0_968E_CA45E80573FC__INCLUDED_)
#define EA_06F6169A_2F42_41b0_968E_CA45E80573FC__INCLUDED_

#include "IEventWorker.h"
#include "IEventWorkObserver.h"
#include "app/event/event_viewer/src/ISCSRealTimeDBWorker.h"
#include "app/event/event_viewer/src/ISCSRealTimeAgentWorker.h"

#include "core/event/idl/src/EventDetailCorbaDef.h"


namespace TA_Base_App
{
	
	class ISCSRealTimeWorker : public IEventWorker
	{

	public:
		ISCSRealTimeWorker(IEventWorkObserver* observer);
		virtual ~ISCSRealTimeWorker();
		virtual void run();
		virtual void doCurrentTask();

	private:
		
		void clear2Workers();
		void ISCSRealTimeWorker::init2Workers();
		void ISCSRealTimeWorker::pause2workers();
		ISCSRealTimeDBWorker * m_DBWorker;
		ISCSRealTimeAgentWorker * m_agentWorker;
		unsigned long m_refreshRateAgent;

	};
};

#endif // !defined(EA_06F6169A_2F42_41b0_968E_CA45E80573FC__INCLUDED_)
