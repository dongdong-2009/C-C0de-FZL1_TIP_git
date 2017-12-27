/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PMSAgent/src/RTUPollingWorker.h $
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

#ifndef MODBUSPOLLINGSCHEDULER_H
#define MODBUSPOLLINGSCHEDULER_H

#include <string>
#include <vector>

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/rtu_library/src/TimerWorker.h"
#include "bus/scada/rtu_library/src/CommandQueue.h"

namespace TA_Base_Bus
{
    class STDModbusPollingWorker;
    class ModbusPollingScheduler : public TimerWorker
    {
    // operations
    public:

    	ModbusPollingScheduler( unsigned long startIOAddress,
                            unsigned long endIOAddress,
                            int scanTime,
                            ECommandType pollingType,
                            TA_Base_Bus::STDModbusPollingWorker* pollingWorker);
                            
      virtual ~ModbusPollingScheduler();

      virtual void process();
          //void    setPollingWorker(const TA_Base_Bus::RTUPollingWorker* pollingWorker);
          
    // operations
    private:

          // disabled constructors
    	ModbusPollingScheduler();
    	ModbusPollingScheduler ( const ModbusPollingScheduler & obj );
    	ModbusPollingScheduler & operator= ( const ModbusPollingScheduler & rhs );

    // attributes
    private:
          unsigned long m_startAddress;
          unsigned long m_endAddress;
          ECommandType  m_commandType;
          TA_Base_Bus::STDModbusPollingWorker* m_pollingWorker;
    };
}

#endif
