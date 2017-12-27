#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif

#include "bus/scada/business_library/src/BusinessMonitorAndWrite.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

namespace TA_Base_Bus
{
	BusinessMonitorAndWrite::BusinessMonitorAndWrite(const TA_Base_Core::ScadaBusinessEntitiesVector& entityKey, 
									 const TA_Base_Core::ScadaBusinessEntitiesVector& patchOrder,
									 unsigned long location,
									 TA_Base_Core::EScadaBusinessSubsystem subSystem)
		: m_isMonitorMode(true),
		  m_haveAllDpConfigValid(false),
		  m_preInAlarm(false),
		  m_scadaProxyFactory(&(ScadaProxyFactory::getInstance())),
		  m_location(location),
		  m_subSystem(subSystem),
		  m_startAddress(0),
		  m_patchBlockWordNumber(0)
	{		
		m_dataPoints.clear();
		m_patchOrder.clear();
		init(entityKey, patchOrder);
	}

	BusinessMonitorAndWrite::~BusinessMonitorAndWrite()
	{
		setToMonitor();
		terminateAndWait();
		clearDataPointProxies();
		ScadaProxyFactory::removeInstance();
	}

	void BusinessMonitorAndWrite::processEntityUpdateEvent( unsigned long entityKey,ScadaEntityUpdateType updateType )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BI--process update event %u,%d", entityKey, updateType);

		if (m_isMonitorMode)
		{
			return;
		}

		if (!m_haveAllDpConfigValid)
		{
			checkAllDpConfigValid();
		}
		//check this flag again
		if (m_haveAllDpConfigValid)
		{
			switch (updateType)
			{
			case AlarmSummaryUpdate:
			case ValueStateUpdate:
				{
					bool isAlarm = isAnyDpInAlarm();
					processFireSystemData(isAlarm);
				}
				break;
			default:
				break;
			}
		}
	}

	void BusinessMonitorAndWrite::setToMonitor()
	{	
		m_isMonitorMode = true;	

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--setToMonitor");
	}

	void BusinessMonitorAndWrite::setToControl()
	{
		m_isMonitorMode = false;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--setToControl");
	}

	void BusinessMonitorAndWrite::hostStart()
	{
		start();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--hostStart");
	}

	void BusinessMonitorAndWrite::hostStop()
	{
		m_isMonitorMode = false;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--hostStop");
	}

	void BusinessMonitorAndWrite::clearDataPointProxies()
	{
		std::vector<DataPointProxySmartPtr*>::iterator itDp;
		for (itDp = m_dataPoints.begin(); itDp != m_dataPoints.end(); ++itDp)
		{
			DataPointProxySmartPtr * pDataPoint = *itDp;
			if (NULL != pDataPoint)
			{
				delete pDataPoint;
			}
		}

		m_dataPoints.clear();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--clearDataPointProxies");
	}

	void BusinessMonitorAndWrite::createDataPointProxies()
	{
		if (NULL == m_scadaProxyFactory)
		{
			m_scadaProxyFactory = &(ScadaProxyFactory::getInstance());
		}

		std::map<unsigned long, unsigned long>::iterator iter;
		for (iter = m_patchOrder.begin(); iter != m_patchOrder.end(); ++iter)
		{
			DataPointProxySmartPtr *pDataPoint = new DataPointProxySmartPtr();

			try
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--createDataPointProxies() - dpkey:%u, pathcOrder:%u", iter->first, iter->second);

				m_scadaProxyFactory->createDataPointProxy(iter->first, *this, *pDataPoint);
				
				m_dataPoints.push_back(pDataPoint);
			}
			catch(TA_Base_Core::ScadaProxyException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--%s", e.what());
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--dpkey:%u Error for unknown reason",iter->first);
			}
			
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--createDataPointProxies() create %d datapoint proxy", m_dataPoints.size());
	}

	void BusinessMonitorAndWrite::processFireSystemData(bool inAlarm)
	{
		CalcStateRequest* writeRequest = new CalcStateRequest();
		if (NULL != writeRequest)
		{
			writeRequest->isInAlarm = inAlarm;
			boost::shared_ptr<CalcStateRequest> request(writeRequest);
			this->insert(request);
		}
	}

	void BusinessMonitorAndWrite::makeFireSystemDataBlock( DataBlock<WORD> &dataBlock )
	{
		// we assign m_lastSavedDataBlock to current dataBlock to
		// reserve bit state, if this data point in this bit raise
		// exception when get alarm state
		dataBlock = m_lastSavedDataBlock;

		std::vector<DataPointProxySmartPtr*>::iterator iter;
		for (iter = m_dataPoints.begin(); iter != m_dataPoints.end(); ++iter)
		{
			unsigned long entityKey = (*(*iter))->getEntityKey();
			unsigned long order = m_patchOrder[entityKey];

			unsigned int wordIndex = order / 16 + 1;
			unsigned int bitIndexInWord = order % 16;

			try
			{
				bool isAlarm     = (*(*iter))->isInAlarmState();
				WORD currentWord = dataBlock[wordIndex];

				if (isAlarm)
				{
					currentWord = currentWord | (0x0001 << bitIndexInWord);	
				}
				else
				{
					currentWord = currentWord & (~(0x0001 << bitIndexInWord));	
				}

				dataBlock.set(wordIndex, currentWord);
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--get data point:%u alarm state error", entityKey);
			}				
		}

		std::stringstream ss;
		for (int j = 0; j < dataBlock.length(); ++j)
		{
			ss << dataBlock[j + m_startAddress] <<"," ;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--patch block data : %s", ss.str().c_str());
	}

	void BusinessMonitorAndWrite::checkAllDpConfigValid()
	{
		//This means all datapoint proxies have already been created 
		if (m_dataPoints.size() == m_patchOrder.size())
		{
			bool isConfigValid = false;
			std::vector<DataPointProxySmartPtr*>::iterator iter;
			for (iter = m_dataPoints.begin(); iter != m_dataPoints.end(); ++iter)
			{
				isConfigValid = (*(*iter))->isConfigValid();
				if (!isConfigValid)
				{
					break;
				}
			}
			if (isConfigValid)
			{
				m_haveAllDpConfigValid = true;
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--checkAllDpConfigValid %d", m_haveAllDpConfigValid);
	}

	void  BusinessMonitorAndWrite::init(const TA_Base_Core::ScadaBusinessEntitiesVector &entityKey, const TA_Base_Core::ScadaBusinessEntitiesVector& patchOrder)
	{
		TA_ASSERT(entityKey.size() == patchOrder.size(), "BI--input datapoint and patch order size not equal");

		unsigned long minPatchOrder = ULONG_MAX;
		unsigned long maxPatchOrder = 0;

		TA_Base_Core::ScadaBusinessEntitiesVector::const_iterator iterDpKey;
		TA_Base_Core::ScadaBusinessEntitiesVector::const_iterator iterPatchOrder;
		for (iterDpKey = entityKey.begin(), iterPatchOrder = patchOrder.begin(); iterDpKey != entityKey.end(); ++iterDpKey,++iterPatchOrder)
		{			
			if (*iterPatchOrder > maxPatchOrder)
			{
				maxPatchOrder = *iterPatchOrder;
			}

			if (*iterPatchOrder < minPatchOrder)
			{
				minPatchOrder = *iterPatchOrder;
			}

			m_patchOrder[*iterDpKey] = *iterPatchOrder;
		}

		// for example if the package order of data point '10029741' 
		// is in the fifth bit of the second WORD in package data block,
		// it's package order is ((2 - 1) * 16 + 5 - 1 = 20) 

		m_startAddress			=	minPatchOrder / 16 + 1;
		m_patchBlockWordNumber  =	maxPatchOrder / 16 - minPatchOrder / 16 + 1;
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"BI--patch word:%u, max order:%u, min order:%u, startAddres:%u ,data point no:%u", 
			m_patchBlockWordNumber, maxPatchOrder, minPatchOrder, m_startAddress, patchOrder.size());
		
		// here we init m_lastSavedDataBlock's word to 0
		m_lastSavedDataBlock.setStartAndLength(m_startAddress, m_patchBlockWordNumber);
		m_lastSavedDataBlock.setAll(0x0000);

		createDataPointProxies();
	}

	bool BusinessMonitorAndWrite::isAnyDpInAlarm()
	{
		bool isAlarm = false;
		
		std::vector<DataPointProxySmartPtr*>::iterator iter;
		for (iter = m_dataPoints.begin(); !isAlarm && iter != m_dataPoints.end(); ++iter)
		{
			try
			{
				isAlarm = (*(*iter))->isInAlarmState();
			}
			catch(...)
			{
				// do nothing
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BIX--check in Alarm : %d", isAlarm);

		return isAlarm;
	}

	bool BusinessMonitorAndWrite::convertEScadaBusSubToEModbusFactorySub( const TA_Base_Core::EScadaBusinessSubsystem &subSystem,
				  ModbusHelperFactory::EnumSubSystem &retSubSystem)
	{
		bool isSuccess = false;

		switch(subSystem)
		{
		case TA_Base_Core::SCADA_BUSINESS_SUBSYSTEM_FAS:
			retSubSystem = ModbusHelperFactory::FAS;
			isSuccess = true;
			break;
		default:
			break;
		}

		return isSuccess;
	}

	void BusinessMonitorAndWrite::processEvent( boost::shared_ptr<CalcStateRequest> newEvent )
	{
		if (NULL == m_mobusHelper.get())
		{
			ModbusHelperFactory::EnumSubSystem subSystem;
			bool isSuccess = convertEScadaBusSubToEModbusFactorySub(m_subSystem, subSystem);
			if (!isSuccess)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--EScadaBusinessSubsystem can't convert ModbusHelperFactory::EnumSubSystem");
				return;
			}

			try
			{
				m_mobusHelper = ModbusHelperFactory::createHelper(m_location, subSystem);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--create modbus helper success!");
			}
			catch(NotFoundFEPConfigException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--create mobus helper error - %s", e.what());
				m_mobusHelper.reset();
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--create mobus helper error - unknown reason");
				m_mobusHelper.reset();
			}
		}

		if (NULL == m_mobusHelper.get() || NULL == newEvent.get())
		{
			return;
		}
		
		// 1. Alarm state changed, we need patch fireSystemData and send to FEP if data block changed
		// 2. Both previous and current in alarm state, we need patch fireSystemData and send to FEP if data block changed
		if ( (m_preInAlarm != newEvent->isInAlarm) ||
			 (m_preInAlarm && newEvent->isInAlarm) )
		{
			m_preInAlarm = newEvent->isInAlarm;

			DataBlock<WORD> fireSateData;
			makeFireSystemDataBlock(fireSateData);

			if (fireSateData != m_lastSavedDataBlock)
			{
				try
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--begin to write FireSystemData To FEP");
					m_mobusHelper->SendData(fireSateData);
					m_lastSavedDataBlock = fireSateData;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--write FireSystemData To FEP success!");
				}
				catch(ModbusException e)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--write FireSystemData To FEP error - %s", e.what());
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--write FireSystemData To FEP error - unknown reason");
				}
			}
		}
	}
}