#ifndef SUMMARY_QUEUE_HANDLER_H
#define SUMMARY_QUEUE_HANDLER_H

#include <map>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/threads/src/QueueProcessorPool.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/scada/datanode_library/src/SummaryRequest.h"

namespace TA_Base_Bus
{
	class IEntity;
	class DataNode;
}

namespace TA_Base_Bus 
{
	class SummaryQueueHandler : public TA_Base_Core::Thread,
								public TA_Base_Core::QueueProcessorPoolCallback<SummaryRequest>
	{
	public:
		typedef std::map<unsigned long, TA_Base_Bus::IEntity*> EntityMap;
		SummaryQueueHandler();
		virtual ~SummaryQueueHandler();
		void run();
		void terminate();
		void setEntityMap(const EntityMap* entityMap);
		void setOperationMode(TA_Base_Core::EOperationMode opMode);
		void queueItemUnique(SummaryRequest itemRequest, bool forceHandle);
		void queueProcessorPoolCallback(boost::shared_ptr<SummaryRequest> newRequest);
		void stopProcessing();
		void startProcessing();
		
	protected:
		void buildDataNodeLayer(const EntityMap* entityMap);
		void buildDataNodeLayerFromTop(std::map<unsigned long, DataNode*>& dnMap, DataNode* topDN, int layer);
		void processQueue();
		void cleanQueue();
	private:
		volatile bool						m_bTerminate;
		volatile bool						m_logStats;
		TA_Base_Core::EOperationMode		m_opMode;
		TA_Base_Core::Condition				m_signalCondition;
		unsigned long						m_timeWaitOneLoop;
		const EntityMap*					m_entityMap;
		std::map<unsigned long, unsigned int> m_dnLayerMap;
		static const unsigned long			s_longTimeWaitForStandby;
		std::vector<SummaryRequest>			m_queue;
		TA_Base_Core::NonReEntrantThreadLockable m_queueLock;
		TA_Base_Core::QueueProcessorPool<SummaryRequest, 
			TA_Base_Core::QueueProcessorWorker<SummaryRequest> >*   m_summaryProcessor;
	};
}

#endif