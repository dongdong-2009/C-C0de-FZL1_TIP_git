// IWatchObject.cpp: implementation of the IWatchObject class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/alarm/alarm_store_library/src/IWatchObject.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{

	const unsigned long DEFAULT_TS_UPDATE_INTERVAL	= 20000; // 20 seconds
	const std::string SELF_CHECK_INTERVAL = "SelfCheckInterval";
	IWatchedObject::IWatchedObject()
	{

	}

	IWatchedObject::~IWatchedObject()
	{

	}

	CheckWorker::CheckWorker( IWatchedObject * watchedObject )
		: m_pWatchedObject(watchedObject),
		m_ulCheckGap(DEFAULT_TS_UPDATE_INTERVAL), m_bTerminate(false)

	{
		if( TA_Base_Core::RunParams::getInstance().isSet(SELF_CHECK_INTERVAL.c_str()) )
		{
			m_ulCheckGap = atol(TA_Base_Core::RunParams::getInstance().get(SELF_CHECK_INTERVAL.c_str()).c_str());
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "start CheckWorker thread in CheckWorker constructor, m_ulCheckGap=%li", m_ulCheckGap);
		start();
	}
	
	CheckWorker::~CheckWorker()
	{
		terminateAndWait();
	}
	
	void CheckWorker::run()
	{
		if ( m_pWatchedObject != NULL )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"CheckWorker start to run...." );
			while( !m_bTerminate )
			{
				m_pWatchedObject->check();
				if(!m_bTerminate)
				{	
					m_CondTerminate.timedWait( m_ulCheckGap );	// using condition wait
				}
			}
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"CheckWorker stop running...." );
		}
	}
	
	void CheckWorker::terminate()
	{
		m_bTerminate = true;
		m_CondTerminate.signal();
	}
	
	unsigned long CheckWorker::setCheckGap( unsigned long ulCheckGap )
	{
		unsigned long tmp = m_ulCheckGap;
		m_ulCheckGap = ulCheckGap;
		return tmp;
	}
}