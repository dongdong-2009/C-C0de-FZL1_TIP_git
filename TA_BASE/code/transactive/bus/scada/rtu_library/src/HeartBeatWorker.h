// HeartBeatWork.h: interface for the HeartBeatWork class.
//
//////////////////////////////////////////////////////////////////////
#ifndef HEARTBEATWORK_H
#define HEARTBEATWORK_H

#include <string>
#include <vector>

#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
namespace TA_Base_Core
{
    class SocketOptions;
}

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_Base_Bus
{
    // Forward declaration
	class RTU;
	class HeartBeatWorker : public TA_Base_Core::Thread
	{

	public:

		static  HeartBeatWorker* getInstance();
		static  void             removeInstance();

		virtual void run();

		virtual void terminate();

		void addRTU(RTU& rtu);

        void setToControl();//limin, failover issue

        void setToMonitor();//limin, failover issue

	protected:
		HeartBeatWorker();

		virtual ~HeartBeatWorker ();
		
		virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newHeartBeat );

		std::vector<RTU *> m_rtus;

		volatile bool m_keepRunning;
		volatile bool m_isInControlMode;

        unsigned long m_operationMode; //limin, failover issue

		static HeartBeatWorker *m_singleton;
		static unsigned int     m_refCounter;
		static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;
	};

}
#endif
