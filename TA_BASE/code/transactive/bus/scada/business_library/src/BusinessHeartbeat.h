#ifndef HEART_BEAT_WORKER_H
#define HEART_BEAT_WORKER_H

#include "bus/scada/business_library/src/Business.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/synchronisation/src/Condition.h"
#include "core/data_access_interface/src/IScadaBusinessItem.h"

namespace TA_Base_Bus
{
	class NullHeartbeartProcessor : public IEntityUpdateEventProcessor
	{
	public:
		virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType)
		{ 
			// do nothing
		}
	};
	
	class BusinessHeartbeat : public Business,
							public TA_Base_Core::Thread,
							public TA_Base_Core::ITimeoutCallback						
	{
	public:
		static const unsigned long BusinessHeartbeat::HEARTBEAT_DEFAULT_INTERVAL = 5000;

		BusinessHeartbeat(const TA_Base_Core::ScadaBusinessEntitiesVector& heartbeatDpKeys,unsigned long heartbeatInterval = HEARTBEAT_DEFAULT_INTERVAL);
		virtual ~BusinessHeartbeat();
		void setToMonitor();
		void setToControl();
		void hostStart();
		void hostStop();
		void timerExpired(long timerId, void* userData);
		void run();
		void terminate();
	protected:	
		void createHeartbeatProxies();
		void clearHeartbeatProxies();
		void process();
	protected:
		volatile bool				m_isRunning;
		volatile bool				m_enable;
		TA_Base_Core::Condition     m_enableSignal;
		TA_Base_Core::SingletonTimerUtil&		m_timerUtility;

		unsigned long				m_hbIntervalTime;
		std::vector<DataPointProxySmartPtr*>	m_dataPoints;
		std::vector<unsigned long>							m_heartbeatDpKeys;
		ScadaProxyFactory* m_scadaProxyFactory;

		NullHeartbeartProcessor		m_nullHeartbeatProcessor;
	};
}

#endif