/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/SNMPDeviceEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * SNMPDeviceEntityData implements the interface ISNMPDeviceEntityData to provide read-only access
  * to RTU entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/SNMPDeviceEntityData.h"

namespace TA_Base_Core
{
    const std::string SNMPDeviceEntityData::SNMP_ENTITY_TYPE		= "SNMPDevice";
	const std::string SNMPDeviceEntityData::SNMP_SCAN_INTERVAL		= "ScanInterval";
	const std::string SNMPDeviceEntityData::SNMP_GET_PORT			= "GetPort";
	const std::string SNMPDeviceEntityData::SNMP_GET_TIMEOUT		= "GetTimeout";
	const std::string SNMPDeviceEntityData::SNMP_GET_RETRY_TIMES	= "GetRetryTimes";
	const std::string SNMPDeviceEntityData::SNMP_SET_PORT			= "SetPort";
	const std::string SNMPDeviceEntityData::SNMP_SET_TIMEOUT		= "SetTimeout";
	const std::string SNMPDeviceEntityData::SNMP_SET_RETRY_TIMES	= "SetRetryTimes";

	const std::string SNMPDeviceEntityData::SNMP_VERSION			= "Version";
	const std::string SNMPDeviceEntityData::SNMP_COMMUNITY			= "Community";
	const std::string SNMPDeviceEntityData::SNMP_SECURITY_LEVEL		= "SecurityLevel";
	const std::string SNMPDeviceEntityData::SNMP_USER_NAME			= "UserName";
	const std::string SNMPDeviceEntityData::SNMP_AUTH_PROTOCOL		= "AuthProtocol";
	const std::string SNMPDeviceEntityData::SNMP_AUTH_PASS			= "AuthPass";
	const std::string SNMPDeviceEntityData::SNMP_PRIV_PROTOCOL		= "PrivProtocol";
	const std::string SNMPDeviceEntityData::SNMP_PRIV_PASS			= "PrivPass";



	SNMPDeviceEntityData::SNMPDeviceEntityData()
    {
    }

	
    SNMPDeviceEntityData::SNMPDeviceEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    SNMPDeviceEntityData::~SNMPDeviceEntityData()
    {
    }

          
	std::string SNMPDeviceEntityData::getType()
	{
		return getStaticType();
	}


    std::string SNMPDeviceEntityData::getStaticType()
    {
        return SNMP_ENTITY_TYPE;
    }


    IEntityData* SNMPDeviceEntityData::clone ( unsigned long key )
    {
        return new SNMPDeviceEntityData ( key );
    }


    void SNMPDeviceEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

	// SNMP 
	int SNMPDeviceEntityData::getScanInterval()
	{
		int data = 1000 * 30; // 30 seconds.

		std::string stringValue = getHelper()->getParameter(SNMP_SCAN_INTERVAL);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}

	unsigned long SNMPDeviceEntityData::getSNMPGetPort()
	{
		unsigned long data = 161;

		std::string stringValue = getHelper()->getParameter(SNMP_GET_PORT);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}

	int SNMPDeviceEntityData::getSNMPGetTimeout()
	{
		int data = 1000; // 1 seconds.

		std::string stringValue = getHelper()->getParameter(SNMP_GET_TIMEOUT);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}

	unsigned long SNMPDeviceEntityData::getSNMPGetRetryTimes()
	{
		unsigned long data = 2;

		std::string stringValue = getHelper()->getParameter(SNMP_GET_RETRY_TIMES);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}
	
	unsigned long SNMPDeviceEntityData::getSNMPSetPort()
	{
		unsigned long data = 161;

		std::string stringValue = getHelper()->getParameter(SNMP_SET_PORT);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}
	
	int SNMPDeviceEntityData::getSNMPSetTimeout()
	{
		int data = 1000; // 1 seconds.

		std::string stringValue = getHelper()->getParameter(SNMP_SET_TIMEOUT);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}

	unsigned long SNMPDeviceEntityData::getSNMPSetRetryTimes()
	{
		unsigned long data = 1;

		std::string stringValue = getHelper()->getParameter(SNMP_SET_RETRY_TIMES);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}

	std::string SNMPDeviceEntityData::getSNMPVersion()
	{
		std::string stringValue = getHelper()->getParameter(SNMP_VERSION);
		if (stringValue.empty())
		{
			stringValue = "1";
		}
		
		return stringValue;
	}
	
	std::string SNMPDeviceEntityData::getSNMPCommunity()
	{
		std::string stringValue = getHelper()->getParameter(SNMP_COMMUNITY);
		if (stringValue.empty())
		{
			stringValue = "public";
		}
		
		return stringValue;
	}


} //namespace TA_Base_Core
