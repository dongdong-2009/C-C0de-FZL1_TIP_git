/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/bus/scada/rtu_library/src/StandardRTUManager.h $
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

#if !defined(AFX_STANDARD_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_STANDARD_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/scada/rtu_library/src/BaseRTUManager.h"
#include "bus/scada/rtu_library/src/CommandQueue.h"


namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
};

namespace TA_Base_Bus
{
    class StandardRTUManager : public TA_Base_Bus::BaseRTUManager
    {
    public:
        explicit StandardRTUManager( int groupId, 
									 bool supportOutput,
									 bool supportHeartBeat,
                   RTU_REDENDENCY_MODE rtuRedundancyMode,
									 bool overlapModbusRequest,
									 bool standardModbusLengthLimit,
									 unsigned int maxModbusLength,
									 unsigned int interval,
                   std::string writeFunctionCode = "0F");

        explicit StandardRTUManager( int groupId, 
                   bool supportOutput,
                   bool supportHeartBeat,
                   RTU_REDENDENCY_MODE rtuRedundancyMode);

        virtual  ~StandardRTUManager();
 
 
        // for Agent to use.
		    virtual TA_Base_Bus::IEntity* createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData);

        //change behavior 
        void addDpProcessor(ModbusDpProcessor* dpProcessor);

        void addPollingScheduler(unsigned long startPollingAddress,
                                 unsigned long endPollingAddress,
                                 int pollTimeout,
                                 TA_Base_Bus::ECommandType commandType);
        
        virtual void setRTUStatusWorker();

        /**
        * send data block use modbus function code 0x10
        * @param dataBlock  the data block to be sent
        * @return       if return 0,success;if return < 0,fail;
        **/
		    virtual int  sendDataBlock(const TA_Base_Bus::DataBlock< WORD > &dataBlock);

        //for redundancymanager callback to process 
        virtual void processNoPrimaryRTU(){};
        virtual void processMultiplePrimaryRTU(){};
        virtual void processOnePrimaryRTU(){};
        virtual void dispatchDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);
		
    		bool getOverlapModbusRequest() { return m_overlapModbusRequest; };
    		bool getStandardModbusLengthLimit() { return m_standardModbusLengthLimit; };
    		int  getMaxModbusLength() { return m_maxModbusLength; };
    		int  getIntervalBetweenModbusTransaction() { return m_interval; };
    		bool getIsWriteEabled() { return m_supportOutput; };
        std::string& getdigitalWriteFunctionCode(){return m_digitalWriteFunctionCode;};

        bool setOverlapModbusRequest(bool overlapModbusRequest) { m_overlapModbusRequest = overlapModbusRequest; };
        bool setStandardModbusLengthLimit(bool standardModbusLengthLimit) { m_standardModbusLengthLimit = standardModbusLengthLimit; };
        int  setMaxModbusLength(unsigned int maxModbusLength) { m_maxModbusLength = m_maxModbusLength; };
        int  setIntervalBetweenModbusTransaction(unsigned int interval) { m_interval = interval; };
        void setdigitalWriteFunctionCode(const std::string  digitalWriteFunctionCode){ m_digitalWriteFunctionCode = digitalWriteFunctionCode;};    	
    protected:

    		bool			     m_overlapModbusRequest;
    		bool			     m_standardModbusLengthLimit;
    		unsigned int	 m_maxModbusLength;
    		unsigned int	 m_interval;
        //Write Function code can be "0F" or "10"
        std::string    m_digitalWriteFunctionCode;
	};
}

#endif // !defined(AFX_STANDARD_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
