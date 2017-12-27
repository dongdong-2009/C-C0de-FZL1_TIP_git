/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/DpUpdateThread.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * DpUpdateThread class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/DpUpdateThread.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/ISelfResolvingPointContainer.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

static const long FORCED_POLLING_REST_MSEC = 10;

namespace TA_Base_App
{
	DpUpdateThread::DpUpdateThread(TaComponentsDriver* driver) 
		: m_driver(driver), m_requestStop(false)
	{
		FUNCTION_ENTRY("DpUpdateThread");
		FUNCTION_EXIT;

	}

	DpUpdateThread::~DpUpdateThread()
	{
		FUNCTION_ENTRY("~DpUpdateThread");
		FUNCTION_EXIT;

	}

	void DpUpdateThread::run()
	{
		FUNCTION_ENTRY("run");
		m_requestStop = false;

		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"DpUpdateThread::run() called.");

		while(!m_requestStop)
		{

			ISelfResolvingPointContainer* pPc = m_driver->getNextDataPoint();
			if(pPc != 0)
			{
				try
				{
                    // Note that any sleeping for this thread will be done in updateDataPoint()
					pPc->updateDataPoint();
				}
				catch(...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"updateDataPoint() for point with pkey %u threw.", pPc->getDataPoint()->getEntityKey());

				}
			}

            // Force a sleep just to avoid thrashing the CPU
            TA_Base_Core::Thread::sleep(FORCED_POLLING_REST_MSEC);

		}
		FUNCTION_EXIT;
	}

	void DpUpdateThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_requestStop = true;
		FUNCTION_EXIT;
	}

} //end namespace TA_Base_App
