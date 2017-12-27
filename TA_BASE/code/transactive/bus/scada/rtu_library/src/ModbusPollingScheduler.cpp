/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PMSAgent/src/RTUPollingWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The RTUPollingWorker object is responsible for polling input data from the PMS RTU
  *
  */

#include <sys/timeb.h>

#include "bus/scada/rtu_library/src/STDModbusPollingWorker.h"
#include "bus/scada/rtu_library/src/ModbusPollingScheduler.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	ModbusPollingScheduler::ModbusPollingScheduler( unsigned long startIOAddress,
                                        unsigned long endIOAddress,
                                        int scanTime,
                                        ECommandType commandType,
                                        TA_Base_Bus::STDModbusPollingWorker* pollingWorker)
	:
    TA_Base_Bus::TimerWorker(scanTime),
    m_startAddress(startIOAddress),
    m_endAddress(endIOAddress),
    m_commandType(commandType),
    m_pollingWorker(pollingWorker)
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "In ModbusPollingScheduler, scanTime = %d, startIOAddress = %d, endIOAddress = %d", scanTime, startIOAddress, endIOAddress);
	}


	ModbusPollingScheduler::~ModbusPollingScheduler()
	{
        setInServiceState( false );
    }

	void ModbusPollingScheduler::process()
	{
        ModbusCommandPtr cmd(new ModbusCommand(m_commandType,m_startAddress,m_endAddress));
        m_pollingWorker->postRequest(cmd);
		    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPollingScheduler, m_startAddress = %d, m_endAddress = %d", m_startAddress, m_endAddress);
	}
}





















