/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/SCADA_development/TA_BASE/transactive/bus/scada/datapoint_library/src/LFThreadPoolSingleton.cpp $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/LFThreadPoolSingleton.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Bus
{
    LFThreadPoolSingleton*		LFThreadPoolSingleton::m_instance = 0;
	ReEntrantThreadLockable		LFThreadPoolSingleton::m_lock;

	LFThreadPoolSingleton::LFThreadPoolSingleton(bool autoStart)
	{
		int num = 16;
		if ( true == RunParams::getInstance().isSet ( "ThreadedWorkers" ) )
		{
			num = atoi ( RunParams::getInstance().get( "ThreadedWorkers" ).c_str() );
		}
		m_threadPoolManager = new LFThreadPoolManager(num, autoStart);
	}

	LFThreadPoolSingleton::~LFThreadPoolSingleton()
	{
		delete m_threadPoolManager;
		m_threadPoolManager = NULL;
	}

	LFThreadPoolSingleton& LFThreadPoolSingleton::getInstance()
	{
		if ( 0 == m_instance )
		{
			TA_THREADGUARD( m_lock );
			if (0 == m_instance)
			{
				m_instance = new LFThreadPoolSingleton();
			}
		}
		return *m_instance;
	}

	LFThreadPoolSingleton& LFThreadPoolSingleton::getInstanceWithArgs(bool autoStart)
	{
		if ( 0 == m_instance )
		{
			TA_THREADGUARD( m_lock );
			if (0 == m_instance)
			{
				m_instance = new LFThreadPoolSingleton(autoStart);
			}
		}
		return *m_instance;
	}

	void LFThreadPoolSingleton::removeInstance (const LFThreadPoolSingleton& instance )
	{
		if (&instance == m_instance )
		{
			TA_THREADGUARD( m_lock );
			delete m_instance;
			m_instance = 0;
		}
	}
}

