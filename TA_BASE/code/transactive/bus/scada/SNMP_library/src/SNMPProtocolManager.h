/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPProtocolManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_PROTOCOL_MANAGER_H_
#define SNMP__SNMP_PROTOCOL_MANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/io_manager/src/ProtocolManager.h"
#include "SNMPDevice.h"
#include "SNMPDataUpdater.h"
#include "SNMPOutputSender.h"
#include "WorkerPool.h"

#include "TrapReceiver.h"

namespace TA_Base_Bus
{    
	/** define the SNMP protocol manager.
     *  derived from ProtocolManager.
     */
	class SNMPProtocolManager : public TA_Base_Bus::ProtocolManager
    {
    public:
        SNMPProtocolManager();
        virtual ~SNMPProtocolManager();

		virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		IProtocolIOBase* addDataPoint(DataPoint*);

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);

		virtual IProtocolDevice* getOutputDevice(std::string senderName);

		virtual void updateMoreParas(const std::map<std::string, std::string>&);

		static bool isEntityTypeMatchedStatic(const std::string&);
		virtual bool isEntityTypeMatched(const std::string&);

		/** process SNMP trap got data.
         *
         * @param ip: data coming ip address.
		 * @param data: SNMP trap data.
         * @return void:
         */
		void processTrapData(const std::string ip, IDeviceData* data);

	private:
		
		WorkerPool*		m_deviceWorkerPool;
		TrapReceiver*	m_trapReceiver;

        TA_Base_Core::ReEntrantThreadLockable   m_dpProcessorLock;


	public:
		static const std::string	SNMP_TRAP_LISTEN_PORT;
    };
}

#endif
