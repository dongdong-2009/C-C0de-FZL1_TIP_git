/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/TrapReceiver.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "TrapReceiver.h"
#include "SNMPProtocolManager.h"
#include "SNMPData.h"
#include "snmp_pp/notifyqueue.h"

#include "core/utilities/src/RunParams.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SNMPProtocolManager* TrapReceiver::m_manager = 0;
    TrapReceiver::TrapReceiver (SNMPProtocolManager* manager, int trapPort)
	    :
		m_trapPort(trapPort),
		m_threadRunning(true),
		m_bInService(false)
    {
		m_manager = manager;
		start();
    }

	TrapReceiver::~TrapReceiver()
	{
		m_bInService = false;
		terminateAndWait();
    }

	void TrapReceiver::run()
	{
		int status = 0;
		int times;
		while (m_threadRunning)
		{
			if (m_bInService)
			{
				startTrap();
			}
			// after 10 minute try again.
			times = 0;
			while (m_bInService && m_threadRunning && times < 6000)
			{
				Thread::sleep(100);
				++times;
			}
			Thread::sleep(100);
		}
	}

	void TrapReceiver::startTrap()
	{
		int status = 0;
		int times;
		Snmp snmp( status, 0);
		if (SNMP_CLASS_SUCCESS != status)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "create trap receiver snmp failed, status:%s", /*Snmp_pp::*/Snmp::error_msg(status));
			return;
		}
		
		OidCollection oidc;
		TargetCollection targetc;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Trying to register for traps on port %d", m_trapPort);
		
		CNotifyEventQueue::set_listen_port(m_trapPort);
		status = snmp.notify_register(oidc, targetc, TrapReceiver::trapCallBack, NULL);
		if (SNMP_CLASS_SUCCESS != status)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error register for notify, status(%d):%s", status, /*Snmp_pp::*/Snmp::error_msg(status));
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "successfully register for traps on port %d", m_trapPort);
			times = 0;
			while (m_bInService)
			{
				snmp.eventListHolder->SNMPProcessEvents(1000);
				if ( 0 == (times % 600))
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Waiting for traps/informs on port %d", m_trapPort);
				}
				++times;
			}
		}
		snmp.notify_unregister();
	}
	
	void TrapReceiver::trapCallBack(int reason, Snmp *session, Pdu &pdu, SnmpTarget &target, void *data)
	{
		GenAddress addr;
		target.get_address(addr);
		//UdpAddress from(addr);	// with port when get_printable.
		IpAddress from(addr);	// no port when get_printable.

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "received trap/informs from %s, reason(%d):%s",
			from.get_printable(), reason, session->error_msg(reason));

		int pdu_error = pdu.get_error_status();
		if (pdu_error)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Response contains error(%d):%s",
				pdu_error, /*Snmp_pp::*/Snmp::error_msg(pdu_error));
		}
		if (pdu.get_vb_count() <= 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "trapCallBack pdu get_vb_count is %d, not process",
				pdu.get_vb_count());
			return;
		}

		Oid id;
		pdu.get_notify_id(id);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "notify oid is %s, pdu type:%d, get_vb_count:%d",
			id.get_printable(), pdu.get_type(), pdu.get_vb_count());

		SNMPData* newData = new SNMPData();
		newData->isGoodQuality = true;
		newData->isPortionDevice = true;
		newData->pdu = pdu;
		std::string ip(from.get_printable());
		m_manager->processTrapData(ip, newData);

		delete newData;
		newData = 0; 

		if (sNMP_PDU_INFORM == pdu.get_type())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "pdu type:%d, sending response to inform:",
				pdu.get_type());

			Vb nextVb;
			pdu.get_vb(nextVb, pdu.get_vb_count() - 1);
			nextVb.set_value("This is the response.");
			pdu.set_vb(nextVb, 0);
			session->response(pdu, target);
		}
	}

}
