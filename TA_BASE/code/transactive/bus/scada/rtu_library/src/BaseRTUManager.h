/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/bus/scada/rtu_library/src/BaseRTUManager.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The BaseRTUManager object is the interface which manage RTUs' running mode and processing RTUs' poll/write data
  *
  */

#if !defined(AFX_BASERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_BASERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/scada/rtu_library/src/RTUOutputWorker.h"
#include "bus/scada/rtu_library/src/HeartBeatWorker.h"
#include "bus/scada/rtu_library/src/DataBlockInterface.h"
#include "bus/scada/rtu_library/src/RTURedundancyWorker.h"
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
};

namespace TA_Base_Bus
{
    //struct RTUDataBlock;
    class RTU;
    class ModbusDpProcessor;
    class IEntity;
    class DataPoint;

    class BaseRTUManager : TA_Base_Bus::IProcessDataBlock
    {
    public:
        explicit BaseRTUManager(int  groupId, 
                                bool supportOutput, 
                                bool supportHeartBeat,
                                RTU_REDENDENCY_MODE rtuRedundancyMode );
        virtual  ~BaseRTUManager();
 
 
        // for Agent to use.
	    virtual TA_Base_Bus::IEntity* createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData) = 0;

	    bool updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);

        void createRTUWorkers();
        bool addDataPoint(TA_Base_Bus::DataPoint* dp);
        bool removeDataPoint(TA_Base_Bus::DataPoint* dp);

        // response to agent running mode
        void setToControl();
        void setToMonitor();

        // for RTU callback
        void processRTUDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);

        void setAnalogueDpUpdateInterval(int analogueDpUpdateInterval);
        void setAgentAssetName(std::string& assetName);
        std::string& getAgentAssetName();
        void setStationName(std::string& stationName);
        std::string& getStationName();
        void setIsRTUSwitchingDisabled(bool isDisabled);

    	/**
		*	send data block use modbus function code 0x10
		*	@param dataBlock	the data block to be sent
		*	@return				if return 0,success;if return < 0,fail;
		**/
		virtual int  sendDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock) = 0;
		virtual void registerDataBlockObserver(DataBlockObserver * observer);
		virtual void removeDataBlockObserver(const DataBlockObserver * observer);

        //for redundancymanager callback to process 
        virtual void processNoPrimaryRTU() = 0;
        virtual void processMultiplePrimaryRTU()  = 0;
        virtual void processOnePrimaryRTU()  = 0;
        virtual void dispatchDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock) = 0;

        RTU* getPrimaryRTU();
            
	protected:
            // internal utility functions
    		void processDataBlockForObserver(const TA_Base_Bus::DataBlock< WORD > & addressBlock);
    		
    		void addRTU(RTU& rtu);
    

        int					m_analogueDpUpdateInterval;
        std::string			m_agentAssetName;
        std::string			m_stationName;
        bool                m_isRTUSwitchingDisabled;

        // an event processor for datapoint write request
		bool			        m_supportOutput;
        RTUOutputWorker*        m_rtuOutputWorker;

		//heart beat processor
        bool				    m_supportHeartBeat;
        HeartBeatWorker*        m_rtuHeartBeatWorker;

        // a list of data point processors for input datapoints
        std::vector< ModbusDpProcessor* >           m_rtuDpProcessors;
        std::vector<TA_Base_Bus::EQualityStatus> 	m_qualityStatus;
        TA_Base_Core::ReEntrantThreadLockable   	m_dpProcessorLock;
        
        // enhence to unlimited number of rtus
        std::vector< RTU* >		m_rtus;
        int                   m_groupId;

        //RTURedundancyWorker process the redundancy policy.
        RTURedundancyWorker*                        m_redundancyWorker;

        // global parameters
        TA_Base_Core::ReEntrantThreadLockable       m_observersLock;
        std::vector<DataBlockObserver*>             m_observers;
    };
}

#endif // !defined(AFX_BASERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
