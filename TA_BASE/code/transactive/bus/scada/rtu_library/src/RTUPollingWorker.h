/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUPollingWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUPollingWorker object is responsible for polling input data from the PMS RTU
  *
  */

#ifndef RTUPOLLINGWORKER_H
#define RTUPOLLINGWORKER_H

#include <string>
#include <vector>

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"   //TD16182
#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "bus/scada/rtu_library/src/RTU.h"

namespace TA_Base_Bus
{
    class IModbus;
    class ModbusException;
}

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_Bus
{
	class RTUPollingWorker : public TA_Base_Core::Thread,
							 public TA_Base_Core::ITimeoutCallback
	{
	// operations
	public:

		RTUPollingWorker( RTU & rtu,
                          unsigned long rtuLocationKey,
                          const std::string & rtuServicePortNumber,
                          unsigned long startIOAddress,
                          unsigned long endIOAddress,
                          unsigned long statusStartAddress,
                          unsigned long statusEndAddress,
                          unsigned long statusOffsetAddress,
                          int scanTime,
                          int slaveID,
                          int commsErrorRetries);
                          
        virtual ~RTUPollingWorker();

		//
		// operations required by the TA_Core::TA_Thread interface
		//
		virtual void run();
		virtual void terminate();

		//
		// operations specific to ITimeoutCallback interface
		//

		/**
		  *
          * timerExpired
          *
          * This operation will be called when the registered timeout period has expired.
          *
          */
		virtual void timerExpired(long timerId, void* userData);


		//
		// operations specific to RTUPollingWorker
		//
        
        /**
		  * setInServiceState
		  *
		  * Set the worker thread in or out of service.  A worker thread that is out of service
		  * would close down the ModbusTcpConnection and not perform any task
		  *
		  * @param inService	Flag to indicate if the worker thread is in service or not
		  *
		  */
		virtual void setInServiceState ( bool inService );
		
		/**
		  * updateCommsErrorRetries
		  *
		  * Updated the comms errors retries of this worker thread with the new value
		  *
		  * @param retries	The new comms error retries
		  *
		  */
		virtual void updateCommsErrorRetries ( int retries );

		virtual bool isInService() const;
        
        virtual bool isThreadRunning() const;

		virtual void updatePollTimeout( int pollTimeout );   //wenching
		virtual void updateScanTimeMSecs(  int scanTimeMSecs );

		//
		// operations to monitor RTU status
		//

		bool getIsCommsOK() const;

	// operations
	private:

		/**
		  * processPollingData
		  *
		  * Process the input data stored in the specified address block
		  *
		  * @param addressBlock	The address block to be processed by the polling worker thread
		  *
		  */
		void processPollingData( const TA_Base_Bus::DataBlock< WORD > & addressBlock );

		/**
		  * processCommsErrorCounter
		  *
		  * Check the comms error counter and perform appropriated actions
		  *
		  */
		void processCommsErrorCounter();

		/**
		  * useSparedModbusConnection
		  *
		  * Create a modbus connection on the spared socket
		  *
		  */
		void useSparedModbusConnection();

        void CheckModbusException( const TA_Base_Bus::ModbusException & me );

        // disabled constructors
		RTUPollingWorker();
		RTUPollingWorker ( const RTUPollingWorker & obj );
		RTUPollingWorker & operator= ( const RTUPollingWorker & rhs );

	// attributes
	private:

		RTU& 						m_rtu;
        std::string 				m_rtuServicePortNumber;
        int 						m_rtuBaseServicePort;
        int 						m_scanTime;
        int 						m_slaveID;
        int 						m_commsErrorRetries;
        int 						m_commsErrorCounter;
        bool 						m_commsError;
        
        TA_Base_Bus::EQualityStatus m_qualityStatus;

        volatile bool m_socketTimeoutUpdated;
		volatile bool m_scanTimeUpdated;
        volatile bool m_inService;
        volatile bool m_threadTerminated;

        TA_Base_Bus::DataBlock< WORD > 					m_addressBlock;
        TA_Base_Bus::IModbus *                          m_currentModbusConnection;

        TA_Base_Core::SingletonTimerUtil&		        m_timerUtility;

        TA_Base_Core::WriteReadThreadLockable           m_modbusLock;
        TA_Base_Core::ReEntrantThreadLockable	        m_lock;
		TA_Base_Core::Condition					        m_timerCondition;
	};
}

#endif
