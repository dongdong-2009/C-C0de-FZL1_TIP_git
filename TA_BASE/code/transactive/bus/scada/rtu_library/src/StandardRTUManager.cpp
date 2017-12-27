/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/StandardRTUManager.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #19 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: tianwai.teo $
  *
  * Description:
  *
  *	The StandardRTUManager object is responsible for managing RTUs' running mode 
  * And processing RTUs' poll/write data
  *
  */

#include "bus/scada/rtu_library/src/StandardRTUManager.h"
#include "bus/scada/rtu_library/src/StandardRTU.h"
#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"
#include "bus/scada/rtu_library/src/RTUStatusWorker.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
    StandardRTUManager::StandardRTUManager(	int groupId, 
											bool supportOutput, 
											bool supportHeartBeat,
											RTU_REDENDENCY_MODE rtuRedundancyMode,
											bool overlapModbusRequest,
											bool standardModbusLengthLimit,
											unsigned int maxModbusLength,
											unsigned int interval,
                   							std::string writeFunctionCode)
        :
        BaseRTUManager(groupId,supportOutput,supportHeartBeat,rtuRedundancyMode),
		m_overlapModbusRequest(overlapModbusRequest),
		m_standardModbusLengthLimit(standardModbusLengthLimit),
		m_maxModbusLength(maxModbusLength),
		m_interval(interval),
		m_digitalWriteFunctionCode(writeFunctionCode)
    {
	}

    StandardRTUManager::StandardRTUManager(	int groupId, 
											bool supportOutput, 
											bool supportHeartBeat,
											RTU_REDENDENCY_MODE rtuRedundancyMode)
        :
        BaseRTUManager(groupId,supportOutput,supportHeartBeat,rtuRedundancyMode),
		m_overlapModbusRequest(false),
		m_standardModbusLengthLimit(false),
		m_maxModbusLength(124),
		m_interval(10),
		m_digitalWriteFunctionCode("0F")
    {
	}

    StandardRTUManager::~StandardRTUManager()
    {
    }

	TA_Base_Bus::IEntity* StandardRTUManager::createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
        // cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr rtuEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);
        
        TA_ASSERT( NULL != rtuEntityData.get(), "rtuEntityData is NULL in createRTUEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating Standard Modbus Equipment entity for [%s]...", rtuEntityData->getName().c_str());
        // create RTU entity
		RTU* rtu = new StandardRTU(*this, rtuEntityData);

		addRTU(*rtu);
        return rtu;	
	}

    void StandardRTUManager::addDpProcessor(ModbusDpProcessor* dpProcessor)
	{
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);
		m_rtuDpProcessors.push_back(dpProcessor);
		m_qualityStatus.push_back(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
	}

	void StandardRTUManager::addPollingScheduler(unsigned long startPollingAddress,
                                    unsigned long endPollingAddress,
									int pollTimeout,
									TA_Base_Bus::ECommandType commandType)
	{
		for (unsigned long i = 0; i < m_rtus.size(); ++i)
		{
			StandardRTU* standardRTU = dynamic_cast<StandardRTU*> (m_rtus[i]);
			
			if(NULL != standardRTU )
			{
				standardRTU->createPollingScheduler(startPollingAddress,endPollingAddress,pollTimeout,commandType);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Add pollingScheduler startAddress[%lu],endAddress[%lu],pollTimout[%d],commandType[%d]" ,
					startPollingAddress,endPollingAddress,pollTimeout,commandType);
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not cast rtu to StandardRTU " );
			}
		}
		
	}
	
    void StandardRTUManager::setRTUStatusWorker()
	{
		for (unsigned long i = 0; i < m_rtus.size(); ++i)
		{
			StandardRTU* standardRTU = dynamic_cast<StandardRTU*> (m_rtus[i]);
			RTUStatusWorker* rtuStatusWorker = new RTUStatusWorker();
			standardRTU->setRTUStatus(rtuStatusWorker);
		}		
	}

	void StandardRTUManager::dispatchDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch StandardRTU [%s] datablock to DpProcessors...", 
            rtu.getName().c_str() );

        // to avoid multiple threads access at same time
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);

        // if there is no primary rtu, or this is primary rtu
        for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            // only process data as good quality when this rtu is primary one and station system is rtu internal system
            if (rtu.getIsCommsOK())
            {
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
            }
			else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus[i])
			{			
				m_qualityStatus[i] = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			}

            m_rtuDpProcessors[i]->processDataBlock(addressBlock, m_qualityStatus[i]);
        }

		//Only the good quality will be processed.
        if (rtu.getIsCommsOK())
        {
			processDataBlockForObserver(addressBlock);
		}
	}

	int StandardRTUManager::sendDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock)
	{
		//get the primary rtu from all managed rtus.
		//Use this rtu to sendData.
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "StandardRTUManager sendDataBlock." );

		if (false == getIsWriteEabled())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No support Write command for this StandardRTUManager." );
			return -1;
		}

		RTU* rtu = getPrimaryRTU();
		if (NULL != rtu )
		{
			if (false == rtu->getIsCommsOK())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "primary RTU comms status is bad now." );
				return -1;
			}

			StandardRTU* standardRTU = dynamic_cast<StandardRTU*> (rtu);

			TA_Base_Bus::DataBlock<WORD> newDataBlock;

			int length = ( dataBlock.end() - dataBlock.start() + 1 );
			newDataBlock.setStartAndLength( dataBlock.start() , length );

			// copy the data from phicial table to logical table
			for ( unsigned long loc = dataBlock.start(); loc <= dataBlock.end(); ++ loc )
			{
				newDataBlock.set( loc, dataBlock[loc] );
			}

			bool bResult = standardRTU->sendData(newDataBlock);
			if (true == bResult)
			{
				return 0;
			}
			return -1;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Can not found primary RTU now." );
		return -1;
	}	
}
