/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPDevice.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_DEVICE_H_
#define SNMP__SNMP_DEVICE_H_


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

	/** derived from ProtocolDevice, implement the IWorkerTask interface.
     * 
	 * __NOTE__: IProtocolDevice derived from IEntity.
     */
	class SNMPDevice : public TA_Base_Bus::ProtocolDevice, public IWorkerTask
	{
	public:

		/** constructor.
         *
         * @param manager: SNMPProtocolManager, when got data, submit to SNMPProtocolManager.
		 * @param manager: pEntityData, device entity data.
         */
		SNMPDevice ( SNMPProtocolManager& manager, TA_Base_Core::SNMPDeviceEntityData* pEntityData );
		virtual ~SNMPDevice();
	
		virtual bool sendCommand(IDeviceCommand*);

		/** add oid to this device, got data need those oids.
         *
         * @param oid: snmp protocol oid.
         */
		void addDeviceOID(const std::string oid);

		void executeTask();
		bool taskTimeExpired();

		// operations
	private:
		// disabled contructors
		SNMPDevice();
		SNMPDevice( const SNMPDevice & obj );
		void operator= ( const SNMPDevice & rhs );

		void initSNMPdata();


		void getBulk();
		void executeGet1by1();
		bool executeGetAll();

		// attributes
	private:

		SNMPProtocolManager& m_manager;
		TA_Base_Core::ReEntrantThreadLockable   m_lock;

		int			m_scanInterval;
		int			m_getPort;
		int			m_getTimeout;
		int			m_getRetryTimes;

		int			m_setPort;
		int			m_setTimeout;
		int			m_setRetryTimes;

		std::string	m_version;
		std::string m_community;
		std::string m_securityLevel;
		std::string m_userName;
		std::string m_authProtocol;
		std::string m_authPass;
		std::string m_privProtocol;
		std::string m_privPass;

		/*Snmp_pp::*/Snmp*		m_snmpConnection;
		/*Snmp_pp::*/Pdu*		m_pdu;
		/*Snmp_pp::*/SnmpTarget*	m_target;

		//<oid, bool>
		std::map<std::string, bool>	m_oidMap;

		timeb		m_lastGetBulkTime;

	};
}
#endif
