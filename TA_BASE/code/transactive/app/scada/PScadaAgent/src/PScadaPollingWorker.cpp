
#include "app/scada/PScadaAgent/src/PScadaUnit.h"
#include "app/scada/PScadaAgent/src/PScadaPollingWorker.h"
#include "app/scada/PScadaAgent/src/PScadaUnit.h"
using TA_Base_Core::DebugUtil;
namespace TA_IRS_App
{
	PScadaPollingWorker::PScadaPollingWorker()
		:m_periodInSec(-1),
		m_inService(false),
		m_threadTerminated(false),
		m_periodInSecUpdated(false),
		m_actived(false),
		m_timerUtility( TA_Base_Core::SingletonTimerUtil::getInstance() )
	{
		FUNCTION_ENTRY("Constructor");

		m_pscadaUnits.clear();

		FUNCTION_EXIT("Constructor");
	}

	PScadaPollingWorker::~PScadaPollingWorker()
	{
		FUNCTION_ENTRY("Destructor");

		if(true == m_inService)
		{
			this->setInServiceState(false);
		}
		m_pscadaUnits.clear();

		FUNCTION_EXIT("Destructor");
	}


	void PScadaPollingWorker::addPScadaUnit(PScadaUnit& pscadaUnit)
	{
		FUNCTION_ENTRY("addPScadaUnit");

		m_pscadaUnits.push_back( &pscadaUnit );
		if(pscadaUnit.getPollAllDataTimeInSec()<=0)
		{
			m_periodInSec = pscadaUnit.getPollKwhDataTimeInSec();
			if( m_periodInSec < 300 )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "the config time [%d] is less than 300, it will change to 300",m_periodInSec);
				m_periodInSec = 300;
			}
			m_actived = true;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "set periodInsec to [%d]",m_periodInSec);
		}
		else
		{
			m_actived = false;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "poll all data is set no need to poll kwh periodly");
			
		}

		FUNCTION_EXIT("addPScadaUnit");
	}


	void PScadaPollingWorker::updatePollingPeriod(int newPeriodInSec)
	{
		FUNCTION_ENTRY("updatePollingPeriod");
		if( newPeriodInSec < 300 )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "the new time [%d] is less than 300, it will change to 300",newPeriodInSec);
			newPeriodInSec = 300;
		}
		if(m_periodInSec == newPeriodInSec)
		{
			FUNCTION_EXIT("updatePollingPeriod");
			return ;
		}
		else
		{
			m_periodInSec = newPeriodInSec;
			m_periodInSecUpdated = true;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "change periodInsec to [%d]",m_periodInSec);
		}
		FUNCTION_EXIT("updatePollingPeriod");
	}


	void PScadaPollingWorker::run()
	{
		if(false == m_actived)
		{
			return;
		}
		TA_ASSERT(m_periodInSec > 0,"invalid period time");
		m_timerUtility.startPeriodicTimeOutClock( this, m_periodInSec*1000, false );

		while ( false == m_threadTerminated )
		{
			// if the worker thread is in service
			if ( true == m_inService )
			{
				//reset the scan time
				if ( true == m_periodInSecUpdated )
				{
					TA_ASSERT(m_periodInSec > 0,"invalid period time");
					m_timerUtility.startPeriodicTimeOutClock(this, m_periodInSec*1000, false);
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "reset timer due to periodInSec has updated");
					m_periodInSecUpdated = false;
				}

				for ( std::vector<PScadaUnit *>::iterator itr = m_pscadaUnits.begin(); 
					itr != m_pscadaUnits.end(); ++itr )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "peform polling data on pscadaunit whose slaveId is [%d]",(*itr)->getSlaveID());
					(* itr)->processPollKwhData();
				}
			}
			m_timerCondition.wait();
		}
		m_timerUtility.stopPeriodicTimeOutClock(this);

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}

	void PScadaPollingWorker::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_threadTerminated = true;
		m_timerCondition.signal();

		FUNCTION_EXIT("terminate");
	}

	
	void PScadaPollingWorker::setInServiceState( bool inService )
	{
		FUNCTION_ENTRY("setInServiceState");
        if (m_inService == inService)
        {
            return;
        }

		m_inService = inService;

		if ( true == inService )
		{
            start();
		}
		else
		{
            terminateAndWait();
		}
		FUNCTION_EXIT("setInServiceState");
	}

	void PScadaPollingWorker::timerExpired(long timerId, void* userData)
	{
		FUNCTION_ENTRY("timerExpired");
		m_timerCondition.signal();
		FUNCTION_EXIT("timerExpired");
	}
}
