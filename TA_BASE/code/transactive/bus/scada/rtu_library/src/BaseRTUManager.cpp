/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/BaseRTUManager.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #19 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: tianwai.teo $
  *
  * Description:
  *
  *	The RTUManager object is responsible for managing RTUs' running mode 
  * And processing RTUs' poll/write data
  *
  */

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"
#include "bus/scada/rtu_library/src/BaseRTUManager.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "bus/scada/rtu_library/src/RTU.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{

    BaseRTUManager::BaseRTUManager( int groupId, 
                                    bool supportOutput, 
                                    bool supportHeartBeat,
                                    RTU_REDENDENCY_MODE rtuRedundancyMode )
        :
        m_analogueDpUpdateInterval(DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL),
        m_agentAssetName(""),
        m_stationName(""),
        m_isRTUSwitchingDisabled(false),
		m_supportOutput(supportOutput),
		m_rtuOutputWorker(NULL),
		m_supportHeartBeat(supportHeartBeat),
        m_rtuHeartBeatWorker(NULL),
        m_groupId(groupId),
        m_redundancyWorker(NULL)

    {
        m_rtuDpProcessors.clear();
        m_qualityStatus.clear();
        m_rtus.clear();
		
		if (true == m_supportOutput)
		{
			m_rtuOutputWorker = RTUOutputWorker::getInstance();
		}

		if (true == m_supportHeartBeat)
		{
			m_rtuHeartBeatWorker = HeartBeatWorker::getInstance();
			// m_rtuHeartBeatWorker->start();//limin, failover issue
		}

        switch (rtuRedundancyMode)
        {
            case SINGLE_PRIMARY_RTU_PATTERN:
                m_redundancyWorker = new SinglePrimaryRTURedundancyWorker(*this);
                break;
            case MULTIPLE_PRIMARY_RTU_PATTERN:
                m_redundancyWorker = new MultiplePrimaryRTURedundancyWorker(*this);
                break;
            default:
                break;
        }
}

    BaseRTUManager::~BaseRTUManager()
    {
		for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++ i)
		{
			ModbusDpProcessor* dpProcessor = m_rtuDpProcessors[i];
			dpProcessor->terminateAndWait();
			delete dpProcessor;
		}
		
		m_rtuDpProcessors.clear();
		
		if( NULL != m_rtuHeartBeatWorker)
		{
			m_rtuHeartBeatWorker->terminateAndWait();
			HeartBeatWorker::removeInstance();
			m_rtuHeartBeatWorker = NULL;
		}
		
		if( NULL != m_rtuOutputWorker)
		{
			//m_rtuOutputWorker->clearWriteQueue();
            m_rtuOutputWorker->terminateAndWait();
			RTUOutputWorker::removeInstance();
			m_rtuOutputWorker = NULL;
		}

        if (NULL != m_redundancyWorker)
        {
            delete m_redundancyWorker;
            m_redundancyWorker = NULL;
        }
	}

    bool BaseRTUManager::updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {
            if (updateEvent.getKey() == m_rtus[i]->getPKey())
            {
                m_rtus[i]->updateRTUConfiguration(updateEvent);

                // return true to indicate no need to go further processing
                return true;
            }
        }

        // not rtu configuration update
        return false;
    }
	
    void BaseRTUManager::createRTUWorkers()
    {
        // create station system worker for rtu internal datapoints processing
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {
            // create RTU other workers
            m_rtus[i]->createRTUWorkers();
        }
    }
	
    bool BaseRTUManager::addDataPoint(TA_Base_Bus::DataPoint* dp)
    {
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;
        bool isAdded = false;

        //Add output datapoint to RTU to be processed by StationSystemOutputWirter
        if (dp->isWriteable())
        {
            // output datapoints are in both rtus, so ingore the return value
            for (i = 0; i < m_rtus.size(); ++ i)
            {
                m_rtus[i]->addDataPoint(dp);
            }

            return true;
        }
        else
        {
            for (i = 0; isAdded == false && i < m_rtus.size(); ++ i)
            {
                isAdded = m_rtus[i]->addDataPoint(dp);
            }
            
            for (i = 0; isAdded == false && i < m_rtuDpProcessors.size(); ++ i)
            {
                isAdded = m_rtuDpProcessors[i]->addDataPoint(dp);

            }
			if (isAdded == false)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "DataPoint [name=%s, address=%d] is not add to any station system of group %d", 
                    dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress(),m_groupId);
            }
            return isAdded;
        }
    }
        
    bool BaseRTUManager::removeDataPoint(TA_Base_Bus::DataPoint* dp)
    {
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;

        //remove datapoint to RTU to be processed by StationSystemOutputWirter
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->removeDataPoint(dp);
        }

        //remove datapoint to station system worker, one datapoint is only processed by on worker
        for (i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            m_rtuDpProcessors[i]->removeDataPoint(dp);
        }

        return true;
    }
        
    void BaseRTUManager::setToControl()
    {
        unsigned long i;

        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->setToControlMode();
        }

		if (NULL != m_rtuOutputWorker)
		{
            m_rtuOutputWorker->setToControl();
		}

		if (NULL != m_rtuHeartBeatWorker)
		{
			m_rtuHeartBeatWorker->setToControl();//limin, failover issue
		}
	}

    void BaseRTUManager::setToMonitor()
    {
        unsigned long i;
        
        //m_rtuHeartBeatWorker.terminateAndWait();//limin, failover issue
        if (NULL != m_rtuHeartBeatWorker)
        {
            m_rtuHeartBeatWorker->setToMonitor();//limin, failover issue
        }

        // before stop output thread, need to clear unsent write requests
 		if (NULL != m_rtuOutputWorker)
		{
			m_rtuOutputWorker->setToMonitor();
		}
		
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->setToMonitorMode();
        }
	}
 
    void BaseRTUManager::setAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
    {
        m_analogueDpUpdateInterval = analogueDpUpdateInterval;

        unsigned long i;

        // update rtu internal dp processor
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->updateAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }

        // update station system dp processors
        for (i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            m_rtuDpProcessors[i]->updateAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }
    }

    void BaseRTUManager::setAgentAssetName(std::string& assetName)
    {
        m_agentAssetName = assetName;
    }

    std::string& BaseRTUManager::getAgentAssetName()
    {
        return m_agentAssetName;
    }

    void BaseRTUManager::setStationName(std::string& stationName)
    {
        m_stationName = stationName;
    }

    std::string& BaseRTUManager::getStationName()
    {
        return m_stationName;
    }

    void BaseRTUManager::setIsRTUSwitchingDisabled(bool isDisabled)
    {
        m_isRTUSwitchingDisabled = isDisabled;
    }    

    void BaseRTUManager::processRTUDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        if (NULL != m_redundancyWorker )
        {
            m_redundancyWorker->processRTUData(rtu,addressBlock);
        }
    }    
	
	void BaseRTUManager::registerDataBlockObserver( DataBlockObserver * observer)
	{
        TA_Base_Core::ThreadGuard guard(m_observersLock);
	
		if( (std::find(m_observers.begin(), m_observers.end(), observer)) == m_observers.end())
		{
			m_observers.push_back(observer);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "add DataBlockObserver %p",observer);
		}
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataBlockObserver %p aready exist when added",observer);
        }
	}

	void BaseRTUManager::removeDataBlockObserver(const DataBlockObserver * observer)
	{
        TA_Base_Core::ThreadGuard guard(m_observersLock);
		std::vector<DataBlockObserver*>::iterator it;
		it = std::find(m_observers.begin(), m_observers.end(), observer);
		if(it != m_observers.end())
		{
			m_observers.erase(it);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataBlockObserver %p removed",observer);
		}
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataBlockObserver %p not exist, no need to remove",observer);
        }
	}

	void BaseRTUManager::processDataBlockForObserver(const TA_Base_Bus::DataBlock< WORD > & addressBlock)
	{
		std::vector<DataBlockObserver*>::iterator it;
        TA_Base_Core::ThreadGuard guard(m_observersLock);
		for(it = m_observers.begin(); it != m_observers.end(); ++it)
		{
			TA_Base_Bus::DataBlock< WORD > dataBlock( (*it)->getDataBlock() );
			//TA_ASSERT(addressBlock.isInRange(dataBlock.start()) && addressBlock.isInRange(dataBlock.end()),"dataBlock addres isnt in range");

            if (addressBlock.isInRange(dataBlock.start()) && addressBlock.isInRange(dataBlock.end()))
            {
    			for(int i=dataBlock.start();i <= dataBlock.end(); i++)
    			{
    				dataBlock.set(i, addressBlock[i]);
    			}

    			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch datablock to observer,datablock start =%d,end = %d",dataBlock.start(),dataBlock.end() );
    			(*it)->updataDataBlock(dataBlock);
            }
		}
	}

	void BaseRTUManager::addRTU(RTU& rtu)
	{
		if (NULL != m_rtuOutputWorker)
		{
			m_rtuOutputWorker->addRTU(rtu);
		}
		
		if (NULL != m_rtuHeartBeatWorker)
		{
			m_rtuHeartBeatWorker->addRTU(rtu);
		}

        std::vector< RTU* >::iterator it;
        it = std::find(m_rtus.begin(), m_rtus.end(), &rtu);

        if(it == m_rtus.end())
        {
            m_rtus.push_back(&rtu);
        }
		if (NULL !=m_redundancyWorker)
		{
			m_redundancyWorker->addRTU(&rtu);
		}
	}	

    RTU* BaseRTUManager::getPrimaryRTU()
    {
        if (NULL != m_redundancyWorker)
        {
            return m_redundancyWorker->getPrimaryRTU();
        }
        return NULL;
    }    
}
