/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/TrapReceiver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__TRAP_RECEIVER_H_
#define SNMP__TRAP_RECEIVER_H_


#include <string>
#include <vector>

#include "bus/generic_agent/src/IEntity.h"

#include "core/data_access_interface/entity_access/src/SNMPDeviceEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/io_manager/src/IProtocolDevice.h"
#include "bus/scada/io_manager/src/IProtocolManager.h"
#include "bus/scada/io_manager/src/ProtocolDevice.h"

#include "SNMPData.h"
#include "SNMPCommand.h"
#include "IWorkerPool.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointWriteRequest;
}

namespace TA_Base_Bus
{
	class SNMPProtocolManager;

	/** this will receive SNMP trap message.
	 *  defualt listen port is 162.
	 *
     */
	class TrapReceiver: public TA_Base_Core::Thread
	{
	public:
		/** constructor
         *
         * @param manager: SNMPProtocolManager, when got data, submit to the manager.
		 * @param trapPort: SNMP trap listen on the port.
         */
		TrapReceiver ( SNMPProtocolManager* manager, int trapPort = 162);
		virtual ~TrapReceiver();

		/** Thread's main function.
         *
         * @param 
		 * @return
         */
		void run();

		/** when thread terminate call back, terminate is called by terminateAndWait.
		 * @return void:
         */
		void terminate(){m_threadRunning = false;};

		/** when SNMP trap got data, call this function to process. this is a static function.
		 *
		 * @param reason: SNMP trap reason.
		 * @param session: SNMP trap session.
		 * @param pdu: SNMP trap pdu.
		 * @param target: SNMP trap target.
		 * @param data: SNMP trap data.
		 * @return void:
         */
		static void trapCallBack(int reason, Snmp *session, Pdu &pdu, SnmpTarget &target, void *data);

		/** set the service state, true: will waiting trap, false: do nothing.
         *
         * @param work: service state.
		 * @return
         */
		void setServiceState(bool work){ m_bInService = work;};

		/** update the trap listenning port.
         * when service state is true, need to reset this service state.
		 *
         * @param port: new trap listen port.
		 * @return
         */
		void updateTrapPort(int port){m_trapPort = port;};

		// operations
	private:
		// disabled contructors
		TrapReceiver();
		TrapReceiver( const TrapReceiver & obj );
		void operator= ( const TrapReceiver & rhs );

		void startTrap();
		// attributes
	private:

		static SNMPProtocolManager* m_manager;

		volatile bool	m_threadRunning;
		volatile bool	m_bInService;
		int				m_trapPort;
	};
}
#endif
