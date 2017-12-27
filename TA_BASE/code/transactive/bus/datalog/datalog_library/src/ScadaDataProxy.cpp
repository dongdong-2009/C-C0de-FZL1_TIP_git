
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h" 
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library//src/DpValue.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "ScadaDataProxy.h"
 

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_Bus
{

	ScadaDataProxy* ScadaDataProxy::m_instance = 0;

	TA_Base_Core::NonReEntrantThreadLockable ScadaDataProxy::m_singletonLock;

	ScadaDataProxy::ScadaDataProxy()
	{
		m_proxyFactory=0;
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
	}



	ScadaDataProxy::~ScadaDataProxy()
	{
		releaseAllDataPoints();
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
	}

	void ScadaDataProxy::removeInstance()
	{
		ThreadGuard guard(m_singletonLock);
		if( m_instance != 0 )
		{
			m_instance->releaseAllDataPoints();
			delete m_instance;
			m_instance=0;
			
		}		 
		
	}


	//
	// getInstance
	//
	ScadaDataProxy* ScadaDataProxy::getInstance()
	{
		ThreadGuard guard(m_singletonLock);

		if( m_instance == 0 )
		{
			m_instance = new ScadaDataProxy();
		}
		return m_instance;
	}


	void ScadaDataProxy::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{

	}

	bool ScadaDataProxy::delDataPoint(unsigned long entityKey)
	{
		FUNCTION_ENTRY("ScadaDataProxy::addDataPoint");

		ThreadGuard guard(m_dataPointMapLock);
		DataPointProxyMap::iterator itFind=m_dataPoints.find(entityKey);
		if (itFind != m_dataPoints.end())
		{	
			TA_Base_Bus::DataPointProxySmartPtr *pDataPoint = itFind->second;
			if (pDataPoint!=NULL)
			{		 
				delete pDataPoint;
				itFind->second=NULL;
			}		 
			m_dataPoints.erase(entityKey);
			return true;
		} 

		return true;
	}


	bool ScadaDataProxy::addDataPoint(unsigned long entityKey)
	{
		FUNCTION_ENTRY("ScadaDataProxy::addDataPoint");

		ThreadGuard guard(m_dataPointMapLock);
		DataPointProxyMap::iterator itFind=m_dataPoints.find(entityKey);
		if (itFind != m_dataPoints.end())
		{	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint:%ld already exists, no need to be added.", entityKey);
			FUNCTION_EXIT;
			return true;
		} 

		

		if (m_proxyFactory==NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"failed to create ScadaProxyFactory ");
			FUNCTION_EXIT;
			return false;
		}

		TA_Base_Bus::DataPointProxySmartPtr *pDataPoint = new TA_Base_Bus::DataPointProxySmartPtr();
        
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "createDataPointProxy for DataPoint:%ld", entityKey);
		try
		{
			 
			m_proxyFactory->createDataPointProxy(entityKey, *this, *pDataPoint);			 
			m_dataPoints[entityKey]=pDataPoint;
		}
		
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught in createDataPointProxy for DataPoint:%ld", entityKey);
			FUNCTION_EXIT;
			return false;
		}
		

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint:%ld is added successfully.", entityKey);
		FUNCTION_EXIT;
		return true;



	}

	bool ScadaDataProxy::releaseAllDataPoints()
	{
		FUNCTION_ENTRY("ScadaDataProxy::releaseAllDataPoints");
		ThreadGuard guard(m_dataPointMapLock);
		DataPointProxyMap::iterator iter=m_dataPoints.begin();
		for (; iter != m_dataPoints.end();iter++ )
		{			
			TA_Base_Bus::DataPointProxySmartPtr *pDataPoint = iter->second;
			if (pDataPoint!=NULL)
			{		 
				delete pDataPoint;
				iter->second=NULL;
			}		 
		} 
		m_dataPoints.clear();
		FUNCTION_EXIT;
		return true;
	}


	bool ScadaDataProxy::getDataPointValue(unsigned long entityKey,double& dpValue)
	{
		FUNCTION_ENTRY("ScadaDataProxy::getDataPointValue");		 

		ThreadGuard guard(m_dataPointMapLock);
		//dpValue=0;
		DataPointProxyMap::iterator itFind=m_dataPoints.find(entityKey);
		if (itFind == m_dataPoints.end())
		{	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPoint:%ld not found in ScadaDataProxy", entityKey);
			FUNCTION_EXIT;
			return false;
		} 
		TA_Base_Bus::DataPointProxySmartPtr *pDataPoint = itFind->second;
		if (pDataPoint==NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPoint:%ld found in ScadaDataProxy is NULL", entityKey);
			FUNCTION_EXIT;
			return false;
		}
		 
		//    QUALITY_GOOD_NO_SPECIFIC_REASON = 192,	    // 00000000 11000000
		//    QUALITY_GOOD_LOCAL_OVERRIDE = 216				// 00000000 11011000
		try
		{
			if ((*pDataPoint)->getStatus()==TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON || (*pDataPoint)->getStatus()==TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE )
			{		
				if ( (*pDataPoint)->isNumber())
				{
					dpValue= (*pDataPoint)->getValueAsDouble();
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get datapoint dpValue=%f  for DataPoint:%ld", dpValue, entityKey);
					FUNCTION_EXIT;
					return  true;
				}
				else if ( (*pDataPoint)->isBoolean())
				{
					bool blnValue= (*pDataPoint)->getValueAsBool();
					if (blnValue) 
					{
						dpValue=1;
						
					}
					else
					{
						dpValue=0;
						
					}
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get datapoint dpValue=%f  for DataPoint:%ld", dpValue, entityKey);
					FUNCTION_EXIT;
					return  true;
				}
				else 
				{
					TA_Base_Bus::DpValue  objDpValue=(*pDataPoint)->getValue();
					TA_Base_Bus::EDataPointType dpDataType=(*pDataPoint)->getType();
					if (dpDataType==TA_Base_Bus::DPT_ENUM_TYPE)
					{
						dpValue=objDpValue.getEnum();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get datapoint dpValue=%f  for DataPoint:%ld", dpValue, entityKey);
						FUNCTION_EXIT;
						return  true;
					}
				}
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't get datapoint dpValue for DataPoint:%ld .", entityKey);
				FUNCTION_EXIT;
				return  false;		
			} 
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Exception caught in ScadaDataProxy getDataPointValue()");
			
		}
  
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't get datapoint dpValue for DataPoint:%ld", entityKey);
		FUNCTION_EXIT;
		return false;
	}
}