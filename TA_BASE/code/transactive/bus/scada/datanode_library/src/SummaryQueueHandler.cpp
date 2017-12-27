#include "bus/scada/datanode_library/src/SummaryQueueHandler.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/MonotonicTimer.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	const unsigned long SummaryQueueHandler::s_longTimeWaitForStandby = (24 * 60 * 60 * 1000);

	SummaryQueueHandler::SummaryQueueHandler()
		:
	m_bTerminate(false),
	m_logStats(true),
	m_timeWaitOneLoop(100),
	m_summaryProcessor(NULL),
	m_opMode(TA_Base_Core::Standby)
	{
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "LogStatistics" ) )
		{
			m_logStats = true;
		}

		int maxNumber = 16;
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "SummaryProcessorWorkers" ) )
		{
			maxNumber = atoi ( TA_Base_Core::RunParams::getInstance().get( "SummaryProcessorWorkers" ).c_str() );
		}

		if ( m_logStats )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Starting Summary Processor with %d threads", maxNumber );
		}

		// create the processor pool, but don't start it until we have the entity map
		m_summaryProcessor = new TA_Base_Core::QueueProcessorPool<SummaryRequest, TA_Base_Core::QueueProcessorWorker<SummaryRequest> >( maxNumber, *this, false );
	}

	SummaryQueueHandler::~SummaryQueueHandler()
	{
		terminateAndWait();

		if (m_summaryProcessor != NULL)
		{
			delete m_summaryProcessor;
			m_summaryProcessor = NULL;
		}
	}

	void SummaryQueueHandler::setEntityMap(const EntityMap* entityMap)
	{
		m_entityMap = entityMap;
		buildDataNodeLayer(entityMap);
		// start its queue handle thread
		//start();
		// start the processors
		//m_summaryProcessor->startProcessing();
	}

	void SummaryQueueHandler::run()
	{
		m_bTerminate = false;
		while( !m_bTerminate )
		{
			unsigned long timeWaitThisLoop = (m_opMode == TA_Base_Core::Standby ? s_longTimeWaitForStandby : m_timeWaitOneLoop);
			m_signalCondition.timedWait(timeWaitThisLoop);
			processQueue();
		}
	}

	void SummaryQueueHandler::terminate()
	{
		m_bTerminate = true;
		m_signalCondition.signal();
	}

	void SummaryQueueHandler::buildDataNodeLayer(const EntityMap* entityMap)
	{
		TA_ASSERT( NULL != entityMap, "entityMap cannot be NULL" );

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Layer--begin buildDataNodeLayer, entity size:%d", entityMap->size());
		// first: save DataNode pointer map and try to find top DataNodes
		std::vector<DataNode*> topDnVec;
		std::map<unsigned long, DataNode*> tmpDnMap;
 		DataNode* dnTmp = NULL;
		EntityMap::const_iterator itEntityMap = entityMap->begin();
		for (; itEntityMap != entityMap->end(); ++itEntityMap)
		{
			dnTmp = dynamic_cast<DataNode*>(itEntityMap->second);
			if (dnTmp != NULL)
			{
				tmpDnMap[itEntityMap->first] = dnTmp;
				if (dnTmp->getParentKey() == dnTmp->getScadaRootKey())
				{
					topDnVec.push_back(dnTmp);
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Layer--Find top DataNode:%s", dnTmp->getName().c_str());
				}
			}
		}
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Layer--Top DataNode size:%d", topDnVec.size());

		// second: handle real top data nodes whose parent is scada root
		while( !topDnVec.empty() )
		{	
			DataNode* topDN = topDnVec.back();
			topDnVec.pop_back();
			buildDataNodeLayerFromTop(tmpDnMap, topDN, 0);
		}

		// third: handle the remain data nodes which is in isolate data node tree  
		while( !tmpDnMap.empty() )
		{
			// find top parent first
			std::map<unsigned long, DataNode*>::iterator itDnPre = tmpDnMap.end();
			std::map<unsigned long, DataNode*>::iterator itDnMap = tmpDnMap.begin();
			while(itDnMap != tmpDnMap.end())
			{
				itDnPre	= itDnMap;
				itDnMap	= tmpDnMap.find(itDnMap->second->getParentKey());
			}

			if ( itDnPre != tmpDnMap.end() )
			{
				buildDataNodeLayerFromTop(tmpDnMap, itDnPre->second, 0);
			}
		}
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Layer--finish buildDataNodeLayer, Layer size:%d", m_dnLayerMap.size());
	}

	void SummaryQueueHandler::buildDataNodeLayerFromTop(std::map<unsigned long, DataNode*>& dnMap, DataNode* topDN, int layer)
	{
		m_dnLayerMap[topDN->getEntityKey()] = layer;
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Layer--key:%u,layer:%d", topDN->getEntityKey(), layer);

		std::vector<std::pair<DataNode*, unsigned int> > tmpStack;
		tmpStack.push_back( std::make_pair(topDN, layer) );

		while ( !tmpStack.empty() )
		{
			std::pair<DataNode*, unsigned int> tmpItem = tmpStack.back();
			tmpStack.pop_back();

			DataNode* dnParent = tmpItem.first;
			int currentLayer   = tmpItem.second;

			//first remove this data node from tmpDnMap
			dnMap.erase(dnParent->getEntityKey());

			//second traversal it's children if it is not list data node(what about both data node?)
			TA_Base_Core::DataNodeEntityData::ESynthesisType eType = dnParent->getSynthesisTypeEnum();
			if (eType != TA_Base_Core::DataNodeEntityData::List)
			{
				std::map<unsigned long, DataNode*>::iterator itDnChild;
				boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> dnStaticData = 
					dnParent->getCompleteStaticConfigurations();
				TA_Base_Core::SharedIEntityDataList& dnList = dnStaticData->childDataNodeEntities;

				std::vector< boost::shared_ptr<TA_Base_Core::IEntityData> >::iterator itEntity;
				for (itEntity = dnList.begin(); itEntity != dnList.end(); ++itEntity)
				{
					unsigned long entityKey = (*itEntity)->getKey();
					// need check whether this child exist in map
					itDnChild = dnMap.find(entityKey);
					if ( itDnChild != dnMap.end())
					{
						m_dnLayerMap[itDnChild->first] = currentLayer + 1;
						LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Layer--key:%u,layer:%d", itDnChild->first, currentLayer+1);
						tmpStack.push_back( std::make_pair(itDnChild->second, currentLayer+1) );
					}
				}	
			}	
		}
	}

	void SummaryQueueHandler::setOperationMode(TA_Base_Core::EOperationMode opMode)
	{
		if (opMode != m_opMode)
		{
			m_opMode = opMode;
			m_signalCondition.signal();		
		}
	}

	void SummaryQueueHandler::queueItemUnique(SummaryRequest itemRequest, bool forceHandle)
	{
		if ( forceHandle && m_opMode != TA_Base_Core::Standby)
		{
			SummaryRequest* pRequest = new SummaryRequest(itemRequest.getEntityKey(), itemRequest.getRequestType());
			boost::shared_ptr<SummaryRequest> rePtr(pRequest);
			m_summaryProcessor->queueItemUnique(itemRequest.getEntityKey(), rePtr);
		}
		else
		{
			TA_Base_Core::ThreadGuard lock(m_queueLock);

			bool isExist = false;
			std::vector<SummaryRequest>::iterator itTmp;
			for (itTmp = m_queue.begin(); itTmp != m_queue.end(); ++itTmp)
			{
				if (*itTmp == itemRequest)
				{
					isExist = true;
					break;
				}
			}

			if ( !isExist )
			{
				m_queue.push_back(itemRequest);
			}	
		}
	}

	void SummaryQueueHandler::cleanQueue()
	{
		TA_Base_Core::ThreadGuard lock(m_queueLock);
		m_queue.clear();
	}

	void SummaryQueueHandler::processQueue()
	{
		std::vector<SummaryRequest> tmpQueue;
		{
			TA_Base_Core::ThreadGuard lock(m_queueLock);
			tmpQueue.swap(m_queue);
		}
		
		if (tmpQueue.size() <= 0) return;

		std::map<int, std::vector<SummaryRequest> > tmpLayerMap;

		int maxLayer = 0;
		std::vector<SummaryRequest>::iterator itTmpQueue;
		for (itTmpQueue = tmpQueue.begin(); itTmpQueue != tmpQueue.end(); ++itTmpQueue)
		{
			int layer = 0;
			unsigned long entityKey = (*itTmpQueue).getEntityKey();
			layer = m_dnLayerMap[entityKey];
			if (layer > maxLayer)
			{
				maxLayer = layer;
			}
			tmpLayerMap[layer].push_back(*itTmpQueue);
		}

		for (int iLayer = maxLayer; iLayer >= 0; iLayer--)
		{
			std::map<int, std::vector<SummaryRequest> >::iterator itLayer = tmpLayerMap.find(iLayer);
			if (itLayer != tmpLayerMap.end())
			{
				std::vector<SummaryRequest>& tmpVec = itLayer->second;
				std::vector<SummaryRequest>::iterator itRequest;
				for (itRequest = tmpVec.begin(); itRequest != tmpVec.end(); ++itRequest)
				{
					unsigned long entitykey		= (*itRequest).getEntityKey();
					SummaryRequest::ESummaryRequestType type	= (*itRequest).getRequestType();
					SummaryRequest* pRequest = new SummaryRequest(entitykey, type);
					boost::shared_ptr<SummaryRequest> rePtr(pRequest);
					m_summaryProcessor->queueItemUnique(entitykey, rePtr);
				}
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SummaryQueueHandler, queue Layer:%d,count:%d", 
					iLayer, tmpVec.size());
			}		
		}
	}

	void SummaryQueueHandler::queueProcessorPoolCallback( boost::shared_ptr<SummaryRequest> newRequest )
	{
		if (newRequest.get() != NULL && m_entityMap != NULL && m_opMode != TA_Base_Core::Standby)
		{           
			unsigned long entityKey	= newRequest->getEntityKey();
			SummaryRequest::ESummaryRequestType requestType = newRequest->getRequestType();

			//if ( m_logStats )
			//{
			//	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing summary request key:%lu, requestType:%d start", entityKey, requestType );
			//}
			TA_Base_Core::MonotonicTimer timeStart;

			EntityMap::const_iterator it = m_entityMap->find(entityKey);
			if (it == m_entityMap->end())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Cannot find DataNode with Key:%lu", entityKey );
			}
			else
			{
				DataNode* dataNode = dynamic_cast<DataNode*>(it->second);
				if (dataNode != NULL)
				{
					switch( requestType )
					{
					case SummaryRequest::ReCalculateRequestFromChildren:
						dataNode->processRecalculateRequestFromChildren();
						break;
					case SummaryRequest::ReCalculateRequestFromParent:
						dataNode->processRecalculateRequestFromParent();
						break;
					case SummaryRequest::UpdateActivation:
						dataNode->processUpdateActivation();
						break;

					default:
						LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled Request Type: %d", requestType );
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "cast entity %lu to DataNode failed!", entityKey );
				}    
			}                
			unsigned long elapsed = timeStart.elapsed();
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing summary request key:%lu, requestType:%d TimeSpan %lu ms", entityKey, requestType, elapsed );
		}
	}

	void SummaryQueueHandler::stopProcessing()
	{
		terminate();
		m_summaryProcessor->stopProcessing();
	}

	void SummaryQueueHandler::startProcessing()
	{
		// start its queue handle thread
		start();
		// start the processors
		m_summaryProcessor->startProcessing();

	}

}

