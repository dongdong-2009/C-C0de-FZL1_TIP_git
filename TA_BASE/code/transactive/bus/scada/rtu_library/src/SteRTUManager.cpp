/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_Review/transactive/bus/scada/rtu_library/src/SteRTUManager.cpp $
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

#include "bus/scada/rtu_library/src/SteRTUManager.h"
#include "bus/scada/rtu_library/src/SteRTUDpProcessor.h"

#include "bus/scada/rtu_library/src/SteRTU.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
    //Swithing Command should not be sent every processing, will wait 2s to send again.
    const unsigned int SwitchingCommandInterval = 2; 

    SteRTUManager::SteRTUManager(   int groupId, 
                                    bool supportOutput, 
                                    bool supportHeartBeat,
                                    RTU_REDENDENCY_MODE rtuRedundancyMode)
        :
        BaseRTUManager(groupId,supportOutput,supportHeartBeat,rtuRedundancyMode),
        m_isRTUSwitchingDisabled(false),
        m_foundMultiplePrimaryRTU(false),
        m_foundNoPrimaryRTU(false),
        m_foundWrongStatusTime(0),
        m_wrongStatus(false)
    {
	}
    

    SteRTUManager::~SteRTUManager()
    {
    }

    TA_Base_Bus::IEntity* SteRTUManager::createRTUEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    {
        // cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr rtuEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);
        
        TA_ASSERT( NULL != rtuEntityData.get(), "rtuEntityData is NULL in createRTUEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating SteRTU entity for [%s]...", rtuEntityData->getName().c_str());
        // create RTU entity
		TA_Base_Bus::RTU* rtu = new SteRTU(*this, rtuEntityData);
        
		addRTU(*rtu);

        return rtu;
    }

    bool SteRTUManager::addStationSystem(TA_Base_Core::IEntityData& EntityData)
    {
        // cast the IEntityData to StationSystemEntityData
        TA_Base_Core::StationSystemEntityData* stationSystemEntityData = NULL;
        stationSystemEntityData = dynamic_cast< TA_Base_Core::StationSystemEntityData * > (&EntityData);
        TA_ASSERT( NULL != stationSystemEntityData, "EntityData can't be cast to StationSystemEntityData" );

		for (unsigned long i = 0; i < m_rtus.size(); i ++)
		{
			if(m_rtus[i]->getGroupID() != stationSystemEntityData->getGroupID() )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Add stationSystem %s failed, group id %d not same as rtu" ,
					stationSystemEntityData->getName().c_str(),
                    stationSystemEntityData->getGroupID());
				return false;
			}
		}

        

		for (unsigned long i = 0; i < m_rtus.size(); i ++)
        {
           if(m_rtus[i]->getPollingStartAddress() > stationSystemEntityData->getPollingStartAddress()
			   || m_rtus[i]->getPollingEndAddress() < stationSystemEntityData->getPollingEndAddress())
		   {
			   LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Add stationSystem %s failed polling start address = %d,end address = %d" , 
				   stationSystemEntityData->getName().c_str(),
                   stationSystemEntityData->getPollingStartAddress(),
                   stationSystemEntityData->getPollingEndAddress());
			   return false;
		   }
			
        }

        // create station system worker
        ModbusDpProcessor* dpProcessor = NULL;

        #if defined ( USE_STATUS_WORD_NUMBER )
            dpProcessor = new SteRTUDpProcessor(
                    m_stationName,
                    stationSystemEntityData->getName(),
                    stationSystemEntityData->getPollingStartAddress(),
                    stationSystemEntityData->getPollingEndAddress(),
                    0, // offset address for station system is always equal to 0
                    stationSystemEntityData->getBlockAddresses(),
                    stationSystemEntityData->getSlaveID(),
                    stationSystemEntityData->getStatusWordNumber(),
                    m_analogueDpUpdateInterval
                );        
        #else
            dpProcessor = new SteRTUDpProcessor(
                    m_stationName,
                    stationSystemEntityData->getName(),
                    stationSystemEntityData->getPollingStartAddress(),
                    stationSystemEntityData->getPollingEndAddress(),
                    0, // offset address for station system is always equal to 0
                    stationSystemEntityData->getBlockAddresses(),
                    stationSystemEntityData->getSlaveID(),
                    0,
                    m_analogueDpUpdateInterval
                );            
        #endif //USE_STATUS_WORD_NUMBER

        
        if (dpProcessor != NULL)
        {
            m_rtuDpProcessors.push_back(dpProcessor);
			m_qualityStatus.push_back(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
        }

        // create rtu output worker
        for (unsigned long i = 0; i < m_rtus.size(); i ++)
        {
            // for bit output worker
            m_rtus[i]->createOutputWriter(
                        stationSystemEntityData->getName(),
                        stationSystemEntityData->getOutputServiceNumber(),
                        stationSystemEntityData->getOutputStartBitAddress(),
                        stationSystemEntityData->getOutputEndBitAddress(),
                        stationSystemEntityData->getSlaveID(),
						stationSystemEntityData->getDigitalWriteFunCode());

            // for word output worker
            m_rtus[i]->createOutputWriter(
                        stationSystemEntityData->getName(),
                        stationSystemEntityData->getOutputServiceNumber(),
                        stationSystemEntityData->getOutputStartWordAddress(),
                        stationSystemEntityData->getOutputEndWordAddress(),
                        stationSystemEntityData->getSlaveID(),
						stationSystemEntityData->getDigitalWriteFunCode());
        }
		return true;
    }

    void SteRTUManager::processNoPrimaryRTU()
    {
        if (false == m_foundNoPrimaryRTU)
        {
            m_foundNoPrimaryRTU = true;
            m_foundWrongStatusTime = time(NULL);
        }

        switchStandbyRTUstoPrimary();
    }

    void SteRTUManager::processMultiplePrimaryRTU()
    {
        if (false == m_foundMultiplePrimaryRTU)
        {
            m_foundMultiplePrimaryRTU = true;
            m_foundWrongStatusTime = time(NULL);
        }
        switchPrimaryRTUstoStandby();
    }

    void SteRTUManager::processOnePrimaryRTU()
    {
        if (m_foundMultiplePrimaryRTU)
        {
            m_foundMultiplePrimaryRTU = false;
            m_foundWrongStatusTime = 0;
        }

         if (m_foundNoPrimaryRTU)
        {
            m_foundNoPrimaryRTU = false;
            m_foundWrongStatusTime = 0;
        }
        
        m_wrongStatus = false;
    }

    void SteRTUManager::dispatchDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch SteRTU [%s] datablock to station systems...", 
            rtu.getName().c_str() );

        // to avoid multiple threads access at same time
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);

        // if there is no primary rtu, or this is primary rtu
        for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            SteRTUDpProcessor* steProcessor = dynamic_cast< SteRTUDpProcessor* >(m_rtuDpProcessors[i]);
            SteRTU* steRTU = dynamic_cast< SteRTU* >(&rtu);
                
            // only process data as good quality when this rtu is primary one and station system is rtu internal system
            if (steRTU->getIsCommsWithStationSystemsValid(steProcessor->getStatusWordNumber(),steProcessor->getSlaveID()) &&
                false == m_wrongStatus)
            {
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
            }
            else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus[i] ||
                     true == m_wrongStatus)
            {           
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
            }

            m_rtuDpProcessors[i]->processDataBlock(addressBlock, m_qualityStatus[i]);
        }

        if (rtu.getIsCommsOK())
        {
            processDataBlockForObserver(addressBlock);
        }
    }

    void SteRTUManager::switchPrimaryRTUstoStandby()
    {

        if (m_isRTUSwitchingDisabled)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There are multiple primary RTUs, but RTU switching is disabled");
            m_wrongStatus = true;
            return;
        }

        if (true == m_foundMultiplePrimaryRTU)   
        {
            time_t currentTime = time(NULL);
            time_t timeSpanFromFound = currentTime - m_foundWrongStatusTime;
            if (timeSpanFromFound <= SwitchingCommandInterval)
            {
                return;
            }
            else
            {
                m_wrongStatus = true;
            }
        } 

        SteRTU* keepPrimaryRTU = NULL;
        std::vector< SteRTU* > primaryRTUs;

        //Check the ste rtu status of each RTU
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {

            SteRTU* steRTU = dynamic_cast<SteRTU*> (m_rtus[i]);
            RTU_STATUS rtuCommsStatus = m_rtus[i]->getRTUStatus();

            if (RTU_PRIMARY == rtuCommsStatus)
            {
                primaryRTUs.push_back(steRTU);

                //found the largest weightage rtu;
                if (NULL == keepPrimaryRTU)
                {
                    keepPrimaryRTU = steRTU;
                }
                else
                {
                    int primaryWeightage = keepPrimaryRTU->getSWCWeightage();
                    int newWeightage = steRTU->getSWCWeightage();
                    
                    if (primaryWeightage < newWeightage)
                    {
                        keepPrimaryRTU = steRTU;
                    }
                }
            }
        }

        if (1 < primaryRTUs.size() )
        {
            for (unsigned long j = 0; j < primaryRTUs.size(); ++j)
            {
                if (keepPrimaryRTU != primaryRTUs[j] )
                {
                     primaryRTUs[j]->switchPrimaryToStandby();
                }
            }               
        }

        m_foundWrongStatusTime = time(NULL);       
    }

    void SteRTUManager::switchStandbyRTUstoPrimary()
    {
        if (m_isRTUSwitchingDisabled)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There are no primary RTUs, but RTU switching is disabled");
            m_wrongStatus = true;
            return;
        }

        if (true == m_foundNoPrimaryRTU)   
        {
            time_t currentTime = time(NULL);
            time_t timeSpanFromFound = currentTime - m_foundWrongStatusTime;
            if (timeSpanFromFound <= SwitchingCommandInterval)
            {
                return;
            }
            else
            {
                m_wrongStatus = true;
            }
        }

        SteRTU* keepPrimaryRTU = NULL;
        std::vector< SteRTU* > primaryRTUs;

        //Check the ste rtu status of each RTU
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {

            SteRTU* steRTU = dynamic_cast<SteRTU*> (m_rtus[i]);
            RTU_STATUS rtuCommsStatus = m_rtus[i]->getRTUStatus();

            if (RTU_STANDBY == rtuCommsStatus)
            {
                //found the largest weightage rtu;
                if (NULL == keepPrimaryRTU)
                {
                    keepPrimaryRTU = steRTU;
                }
                else
                {
                    int primaryWeightage = keepPrimaryRTU->getSWCWeightage();
                    int newWeightage = steRTU->getSWCWeightage();
                    
                    if (primaryWeightage < newWeightage)
                    {
                        keepPrimaryRTU = steRTU;
                    }
                }
            }
            else if (RTU_PRIMARY == rtuCommsStatus)
            {
                primaryRTUs.push_back(steRTU);
            }
        }


        // if we can find an RTU which can be switched from standby to primary, and
        // no primary rtus exist currently. then switch it.
        if (NULL != keepPrimaryRTU && 0 == primaryRTUs.size())
        {            
            keepPrimaryRTU->switchStandbyToPrimary();
            m_foundWrongStatusTime = time(NULL);
        }
    }
}
