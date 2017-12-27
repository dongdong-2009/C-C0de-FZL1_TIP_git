#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif

#include "bus/scada/business_library/src/BusinessHeartbeat.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	BusinessHeartbeat::BusinessHeartbeat(const TA_Base_Core::ScadaBusinessEntitiesVector& heartbeatDpKeys, unsigned long heartbeatInterval)
		: m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
		  m_scadaProxyFactory(&(ScadaProxyFactory::getInstance())),
		  m_enable(false),
		  m_isRunning(true),
		  m_hbIntervalTime(heartbeatInterval),
		  m_heartbeatDpKeys(heartbeatDpKeys)	  
	{
		m_dataPoints.clear();
		createHeartbeatProxies();
	}

	BusinessHeartbeat::~BusinessHeartbeat()
	{
		terminateAndWait();

		clearHeartbeatProxies();

		ScadaProxyFactory::removeInstance();
		TA_Base_Core::SingletonTimerUtil::removeInstance();
	}

	void BusinessHeartbeat::timerExpired( long timerId, void* userData )
	{
		m_enableSignal.signal();
	}

	void BusinessHeartbeat::run()
	{
		m_timerUtility.startPeriodicTimeOutClock(this, m_hbIntervalTime*1000, false);

		while (m_isRunning)
		{
			m_enableSignal.wait();

			if (m_enable)
			{
				try
				{
					process();
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--run exception for unknown reason");
				}
			}
		}

		m_timerUtility.stopPeriodicTimeOutClock(this);
	}

	void BusinessHeartbeat::terminate()
	{
		m_isRunning = false;
		m_enable = false;
		m_enableSignal.signal();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--terminate BusinessHeartbeat");
	}

	void BusinessHeartbeat::setToControl()
	{
		m_enable = true;
		m_enableSignal.signal();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--heartWorker set to control");
	}

	void BusinessHeartbeat::setToMonitor()
	{
		m_enable = false;
		m_enableSignal.signal();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--heartWorker set to Monitor");
	}

	void BusinessHeartbeat::hostStart()
	{
		start();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--start BusinessHeartbeat");
	}

	void BusinessHeartbeat::hostStop()
	{
		m_isRunning = false;
		m_enable = false;
		m_enableSignal.signal();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--stop BusinessHeartbeat");
	}

	void BusinessHeartbeat::process()
	{
		#define HEARTBEART_TRIGGER  0xfffffffe

		std::string val("1");

		std::vector<DataPointProxySmartPtr*>::iterator iter;
		for (iter = m_dataPoints.begin(); iter != m_dataPoints.end(); ++iter)
		{
			unsigned long dpKey = (*(*iter))->getEntityKey();
			try
			{
				(*(*iter))->setValue(val, TA_Base_Bus::DPT_INTERNAL_SESSION_ID, HEARTBEART_TRIGGER, NULL, false);

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--datapoint:%u,heartbeat success!", dpKey);
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--datapoint:%u,heartbeat failed! for %s", dpKey,e.what());
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--datapoint:%u,heartbeat failed for unknown reason", dpKey);
			}
		}		
	}

	void BusinessHeartbeat::createHeartbeatProxies()
	{
		if (NULL == m_scadaProxyFactory)
		{
			m_scadaProxyFactory = &(ScadaProxyFactory::getInstance());
		}

		std::vector<unsigned long>::iterator iter;
		for (iter = m_heartbeatDpKeys.begin(); iter != m_heartbeatDpKeys.end(); ++iter)
		{
			DataPointProxySmartPtr *pDataPoint = new DataPointProxySmartPtr();

			try
			{
				m_scadaProxyFactory->createDataPointProxy(*iter, m_nullHeartbeatProcessor, *pDataPoint);
		
				m_dataPoints.push_back(pDataPoint);		
			}
			catch(TA_Base_Core::ScadaProxyException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI-- %s", e.what());
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--createHeartbeatProxies() - dpkey:%u Error for unknown reason", *iter);
			}
		}
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--createHeartbeatProxies() - %d success", m_dataPoints.size());
	}

	void BusinessHeartbeat::clearHeartbeatProxies()
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

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--clearHeartbeatProxies");
	}
}

