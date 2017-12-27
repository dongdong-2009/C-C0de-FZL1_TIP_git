/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_Review/transactive/bus/scada/rtu_library/src/RTURedundancyWorker.cpp $
  * @author  Liu Yong
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2009/10/01 16:36:38 $
  * Last modified by : $Author: grace.koh $
  *
  *
  * The RTUOutputWorker thread is responsible
  * for sending DataPointWriteRequest to the actual hardware
  *
  */
#include <sstream>

#include "bus/scada/rtu_library/src/BaseRTUManager.h"
#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/RTURedundancyWorker.h"



using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	RTURedundancyWorker::RTURedundancyWorker(BaseRTUManager& rtuManager)
    :
    m_currentPrimaryRTU(NULL),
    m_rtuManager(rtuManager)
    {

    }   

    RTURedundancyWorker::~RTURedundancyWorker()
    {

    }   

    void RTURedundancyWorker::addRTU(RTU* rtu)
    {
        FUNCTION_ENTRY("addRTU");
        TA_Base_Core::ThreadGuard guard(m_primaryRTULock);
        m_rtus.push_back(rtu);
        FUNCTION_EXIT;
    }

    RTU* RTURedundancyWorker::getPrimaryRTU()
    {
        FUNCTION_ENTRY("getPrimaryRTU");
        TA_Base_Core::ThreadGuard guard(m_primaryRTULock);
        return m_currentPrimaryRTU;
        FUNCTION_EXIT;
    }

	void RTURedundancyWorker::checkAllRTUStatus()
    {
        FUNCTION_ENTRY("checkAllRTUStatus");
        // to avoid multiple threads access at same time
        // m_commsDownRTUs.clear();
        m_swithchingRTUs.clear();
        m_primaryRTUs.clear();
        // m_standbyRTUs.clear();

        //Check the ste rtu status of each RTU and then put it into the related list.
        for (unsigned long i = 0; i < m_rtus.size(); ++i)
        {

            RTU_STATUS rtuCommsStatus = m_rtus[i]->getRTUStatus();

            switch (rtuCommsStatus)
            {
                // case RTU_COMMS_DOWN:
                //     m_commsDownRTUs.push_back( m_rtus[i]);
                //     break;
                case RTU_SWITCHING:
                    m_swithchingRTUs.push_back( m_rtus[i]);
                    break;
                case RTU_PRIMARY:
                    m_primaryRTUs.push_back( m_rtus[i]);
                    break;
                // case RTU_STANDBY:
                //     m_standbyRTUs.push_back( m_rtus[i]);
                //     break;
                default:
                    break;                                        
            }
        }
        FUNCTION_EXIT;
	}

    bool RTURedundancyWorker::checkSwitchingRTUs()
    {
        FUNCTION_ENTRY("checkSwitchingRTUs");
        unsigned long swithchingRTUNum  =  m_swithchingRTUs.size();
        if (swithchingRTUNum > 0)
        {
            //but if some RTU is switching now, then no need to process. the next processing to check 
            //whether the switching is finished.
            std::string strSwithchingRTUName;

            for (unsigned long j = 0; j < m_swithchingRTUs.size(); ++j)
            {
                if (0 == strSwithchingRTUName.length())
                {
                    strSwithchingRTUName = m_swithchingRTUs[j]->getName();
                }
                else
                {
                    strSwithchingRTUName = strSwithchingRTUName + "," +  m_swithchingRTUs[j]->getName();
                }
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "RTU %s in switching",strSwithchingRTUName.c_str());
            return true;
        }        
        FUNCTION_EXIT;
        return false;
    }

    SinglePrimaryRTURedundancyWorker::SinglePrimaryRTURedundancyWorker(BaseRTUManager& rtuManager)
    :
    RTURedundancyWorker(rtuManager)
    {

    }   

    void SinglePrimaryRTURedundancyWorker::processRTUData(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        FUNCTION_ENTRY("SinglePrimaryRTURedundancyWorker::processRTUData");
        TA_Base_Core::ThreadGuard guard(m_primaryRTULock);
        checkAllRTUStatus();

        unsigned long primaryRTUNum     =  m_primaryRTUs.size();
        
        if (0 == primaryRTUNum )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No Primary RTU exist now.");
            
            m_rtuManager.processNoPrimaryRTU();
            //If primaryRTU was set, means this primary rtu currently change to non primary state.
            if (NULL != m_currentPrimaryRTU)
            {
                //if the processing rtu is the primary rtu. need to process for updating the data as quality change.
                if (m_currentPrimaryRTU == &rtu)
                {
                     m_rtuManager.dispatchDataBlock(rtu,addressBlock);
                }
            }
            else
            {
                //If last processing can not identify the primary RTU, no need to process.
                return;
            }            
        }
        else if (1 < primaryRTUNum )
        {
            //multiple primary RTUs exist.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Multiple Primary RTU exist now.");        

            m_rtuManager.processMultiplePrimaryRTU();
            if (NULL != m_currentPrimaryRTU)
            {
                //if the processing rtu is the primary rtu. need to process for updating the data as quality change.
                if (m_currentPrimaryRTU == &rtu)
                {
                     m_rtuManager.dispatchDataBlock(rtu,addressBlock);
                }
            }
            else
            {
                //If last processing can not identify the primary RTU, no need to process.
                return;
            }
        }
        else
        {
            //correct, only 1 primary rtu found, then only process the primary rtu data.
            //clear all of the abnormal status setting.
            m_currentPrimaryRTU = m_primaryRTUs[0];

            m_rtuManager.processOnePrimaryRTU();
            
            if (m_currentPrimaryRTU == &rtu)
            {
                m_rtuManager.dispatchDataBlock(rtu,addressBlock);
            }
        }        
               
        FUNCTION_EXIT;
    }


    MultiplePrimaryRTURedundancyWorker::MultiplePrimaryRTURedundancyWorker(BaseRTUManager& rtuManager)
    :
    RTURedundancyWorker(rtuManager)
    {

    }   

    void MultiplePrimaryRTURedundancyWorker::processRTUData(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        FUNCTION_ENTRY("MultiplePrimaryRTURedundancyWorker::processRTUData");
        TA_Base_Core::ThreadGuard guard(m_primaryRTULock);
        checkAllRTUStatus();
        unsigned long primaryRTUNum     =  m_primaryRTUs.size();
        
        if (0 == primaryRTUNum )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No Primary RTU exist now.");
            //No primary rtu found now.

            if (true == checkSwitchingRTUs())
            {
                return;
            }

            //If primaryRTU was set, means this primary rtu currently change to non primary state.
            if (NULL != m_currentPrimaryRTU)
            {
                //if the processing rtu is the primary rtu. need to process for updating the data as quality change.
                if (m_currentPrimaryRTU == &rtu)
                {
                     m_rtuManager.dispatchDataBlock(rtu,addressBlock);
                }
                //after processing, clear primary RTU
                m_currentPrimaryRTU = NULL;
            }
            else
            {
                //If last processing can not identify the primary RTU, no need to process.
                return;
            }

            m_rtuManager.processNoPrimaryRTU();
        }
        else
        {
            bool bFound = false;
            std::vector< RTU* >::iterator rtuIter;
            for (rtuIter = m_primaryRTUs.begin(); rtuIter != m_primaryRTUs.end();++rtuIter)
            {
                if (*rtuIter == &rtu)
                {
                    bFound = true;
                    break;
                }
            }

            if ( false == bFound )
            {
                if (NULL == m_currentPrimaryRTU)
                {
                    m_currentPrimaryRTU = m_primaryRTUs[0];
                }
            }
            else
            {
                if (NULL == m_currentPrimaryRTU)
                {
                    m_currentPrimaryRTU = m_primaryRTUs[0];
                }

                if (&rtu == m_currentPrimaryRTU)
                {
                    m_rtuManager.dispatchDataBlock(rtu,addressBlock);
                }

             }
        }        

        FUNCTION_EXIT;
    }
}
