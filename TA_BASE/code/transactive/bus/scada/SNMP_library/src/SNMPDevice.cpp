/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPDevice.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include <sstream>

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/utilities/src/RunParams.h"

#include "SNMPDevice.h"
#include "SNMPProtocolManager.h"
#include "SNMPCommand.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    SNMPDevice::SNMPDevice (SNMPProtocolManager& manager,  TA_Base_Core::SNMPDeviceEntityData* pEntityData)
	    :
		ProtocolDevice(pEntityData),
        m_manager(manager),
		m_snmpConnection(0),
		m_pdu(0),
		m_target(0)
    {
		m_scanInterval	= pEntityData->getScanInterval();
		m_getPort		= pEntityData->getSNMPGetPort();;
		m_getTimeout	= pEntityData->getSNMPGetTimeout();
		m_getRetryTimes	= pEntityData->getSNMPGetRetryTimes();

		m_setPort		= pEntityData->getSNMPSetPort();
		m_setTimeout	= pEntityData->getSNMPSetTimeout();
		m_setRetryTimes	= pEntityData->getSNMPSetRetryTimes();

		m_version		= pEntityData->getSNMPVersion();
		m_community		= pEntityData->getSNMPCommunity();
		//m_securityLevel	= pEntityData->getsnmpSecurityLevel();
		//m_userName;
		//m_authProtocol;
		//m_authPass;
		//m_privProtocol;
		//m_privPass;

		m_lastGetBulkTime.time = 0;
    }

	SNMPDevice::~SNMPDevice()
	{

    }

	void SNMPDevice::executeTask()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "in executeTask");
		initSNMPdata();
		timeb currentTimeStamp;
		ftime( &currentTimeStamp );
		if (false == executeGetAll())	// there are some bad oids, so get data one by one.
		{
			executeGet1by1();
		}
		m_lastGetBulkTime = currentTimeStamp;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "out executeTask");
	}

	void SNMPDevice::initSNMPdata()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		if (0 == m_snmpConnection)
		{
			int status;
			m_snmpConnection = new /*Snmp_pp::*/Snmp(status, 0, false);
			if ( status != SNMP_CLASS_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "create snmp failed, status:%s", /*Snmp_pp::*/Snmp::error_msg(status));
				delete m_snmpConnection;
				m_snmpConnection = 0;
				return;
			}
		}

		if (0 == m_pdu)
		{
			Pdu* tmpPdu = new /*Snmp_pp::*/Pdu();
			/*Snmp_pp::*/Vb vb;
			std::map<std::string, bool>::const_iterator it = m_oidMap.begin();
			while(m_oidMap.end() != it)
			{
				/*Snmp_pp::*/Oid oid(it->first.c_str());
				if (!oid.valid())
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "invalid oid:%s", it->first.c_str());
					delete tmpPdu;
					return;
				}
				vb.set_oid(oid);
				*tmpPdu += vb;

				++it;
			}
			m_pdu = tmpPdu;
		}

		if (0 == m_target)
		{
			/*Snmp_pp::*/UdpAddress address(m_entityAddress.c_str());
			if ( !address.valid())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid Address or DNS Name:%s", m_entityAddress.c_str());
				return;
		    }
			address.set_port(m_getPort);

			/*Snmp_pp::*/CTarget* pTarget = new /*Snmp_pp::*/CTarget(address);
			if (0 == pTarget)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "create CTarget failed, address:%s", m_entityAddress.c_str());
				return;
			}

			pTarget->set_version(/*Snmp_pp::*/version1);
			pTarget->set_retry(m_getRetryTimes);
			pTarget->set_timeout(m_getTimeout / 10);
			pTarget->set_readcommunity(m_community.c_str());

			m_target = pTarget;
		}
	}

	void SNMPDevice::executeGet1by1()
	{
		if (m_snmpConnection && m_pdu && m_target)
		{
			int status;
			std::map<std::string, bool>::const_iterator it = m_oidMap.begin();
			while (m_oidMap.end() != it)
			{				
				Oid oid(it->first.c_str());
				Vb vb;
				vb.set_oid(oid);

				SNMPData* data = new SNMPData();
				data->isPortionDevice = true;
				data->pdu += vb;

				status = m_snmpConnection->get(data->pdu, *m_target);
				if (status == SNMP_CLASS_SUCCESS)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "got data oid[%s] from %s. start to process data",
						it->first.c_str(), m_entityAddress.c_str());
					data->isGoodQuality = true;
					m_manager.processData(this, data);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SNMP++ get Error to %s,oid[%s], status (%d):%s, timeout[%d]ms, retry times[%d], to process data",
						m_entityAddress.c_str(), it->first.c_str(), status, /*Snmp_pp::*/Snmp::error_msg( status), m_getTimeout, m_getRetryTimes);

					data->isGoodQuality = false;
					m_manager.processData(this, data);
				}
				delete data;
				++it;
			}
		}
	}

	bool SNMPDevice::executeGetAll()
	{
		bool isSuccessful = true;
		if (m_snmpConnection && m_pdu && m_target)
		{
			int status;
			int non_reps=0;	// non repeaters default is 0
			int max_reps=1;	// maximum repetitions default is 1
			
			SNMPData* data = new SNMPData();
			data->isPortionDevice = false;
			data->pdu = *m_pdu;
			status = m_snmpConnection->get(data->pdu, *m_target);

			if (SNMP_CLASS_SUCCESS == status)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "SNMP++ get %d oids from %s(%s). start to process data",
					data->pdu.get_vb_count(), m_entityAddress.c_str(), m_entityDescription.c_str());
				data->isGoodQuality = true;
				m_manager.processData(this, data);
			}
			else if (SNMP_ERROR_NO_SUCH_NAME == status)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SNMP++ get Error to %s(%s), status (%d):%s, not to process data",
					m_entityAddress.c_str(), m_entityDescription.c_str(), status, /*Snmp_pp::*/Snmp::error_msg( status));
				isSuccessful = false;
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SNMP++ get Error to %s(%s), status (%d):%s, port[%d], timeout[%d]ms, retry times[%d], to process data",
						m_entityAddress.c_str(), m_entityDescription.c_str(), status, /*Snmp_pp::*/Snmp::error_msg( status), m_getPort, m_getTimeout, m_getRetryTimes);

				data->isGoodQuality = false;
				m_manager.processData(this, data);
			}
			delete data;
		}
		return isSuccessful;
	}

	void SNMPDevice::getBulk()
	{
		if (m_snmpConnection && m_pdu && m_target)
		{
			int status;
			int non_reps=0;	// non repeaters default is 0
			int max_reps=1;	// maximum repetitions default is 1
			
			SNMPData* data = new SNMPData();
			data->pdu = *m_pdu;
			status = m_snmpConnection->get_bulk(data->pdu, *m_target, non_reps, max_reps);

			if (status == SNMP_CLASS_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "got %d oids from %s. start to process data",
					data->pdu.get_vb_count(), m_entityAddress.c_str());
				data->isGoodQuality = true;
				m_manager.processData(this, data);
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SNMP++ GetBulk Error to %s, status:%s, to process data",
					m_entityAddress.c_str(), /*Snmp_pp::*/Snmp::error_msg( status));

				data->isGoodQuality = false;
				m_manager.processData(this, data);
			}
			delete data;
		}
	}

	void SNMPDevice::addDeviceOID(const std::string oid)
	{
		m_oidMap[oid] = true;
	}

	bool SNMPDevice::taskTimeExpired()
	{
		timeb currentTimeStamp;
		ftime( &currentTimeStamp );
		timeb tempTime;
		tempTime.time = m_lastGetBulkTime.time + ((m_lastGetBulkTime.millitm + m_scanInterval) / 1000);
		if (currentTimeStamp.time < tempTime.time)
		{
			return false;
		}
		if (currentTimeStamp.time > tempTime.time)
		{
			return true;
		}
		tempTime.millitm = (m_lastGetBulkTime.millitm + m_scanInterval) % 1000;
		if (currentTimeStamp.millitm < tempTime.millitm)
		{
			return false;
		}
		return true;
	}

	bool SNMPDevice::sendCommand(IDeviceCommand* sendCmd)
	{
		SNMPCommand* cmd = dynamic_cast<SNMPCommand*>(sendCmd);
		if (0 == cmd)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "sender %s can not cast command from %s to SNMPCommand type",
					m_entityName.c_str(), sendCmd->getStaticType().c_str());
			return false;
		}
		
		bool sended = false;
		initSNMPdata();
		if (m_snmpConnection)
		{
			Pdu setpdu;
			setpdu += cmd->vb; 
			
			UdpAddress address(m_entityAddress.c_str());
			if ( !address.valid())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid Address or DNS Name:%s", m_entityAddress.c_str());
				return false;
		    }
			address.set_port(m_setPort);

			/*Snmp_pp::*/CTarget target(address);
			target.set_version(/*Snmp_pp::*/version1);
			target.set_retry(m_setRetryTimes);
			target.set_timeout(m_setTimeout / 10);
			target.set_readcommunity(m_community.c_str());

			int status = 0;
			status = m_snmpConnection->set( setpdu, target);
			if(SNMP_CLASS_SUCCESS == status)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SNMP++ set oid:%s to %s successful, device is %s",
					cmd->vb.get_printable_oid(), cmd->vb.get_printable_value(), m_entityAddress.c_str());
				sended = true;
			}
			else 
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SNMP++ set oid:%s to %s failed (%d):%s, device is %s",
					cmd->vb.get_printable_oid(), cmd->vb.get_printable_value(), status, Snmp::error_msg( status), m_entityAddress.c_str());
			}
		}
		return sended;
	}
}
