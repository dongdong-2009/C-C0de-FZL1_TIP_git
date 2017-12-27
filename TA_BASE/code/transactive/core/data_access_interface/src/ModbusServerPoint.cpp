/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerPoint.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * ModbusServerPoint is an implementation of IModbusServerPoint. It holds the data specific to an ModbusServerPoint entry
  * in the database, and allows read-only access to that data.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/ModbusServerPoint.h"

namespace TA_Base_Core
{
	const std::string	ModbusServerPoint::ENTITY_AGENTKEY		= "AGENTKEY";
	const std::string	ModbusServerPoint::MODBUS_POINT_PKEY	= "PKEY";
	const std::string	ModbusServerPoint::MODBUS_POINT_ENTITY_KEY	= "ENTITY_KEY";
	const std::string	ModbusServerPoint::MODBUS_POINT_ADDRESS	= "ADDRESS";
	const std::string	ModbusServerPoint::MODBUS_POINT_PORT	= "PORT";
	const std::string	ModbusServerPoint::MODBUS_POINT_DATALENGTH		= "DATALENGTH";
	const std::string	ModbusServerPoint::MODBUS_POINT_NAME	= "NAME";
	const std::string	ModbusServerPoint::MODBUS_POINT_ENTITY_ADDRESS		= "ENTITY_ADDRESS";
	
	ModbusServerPoint::ModbusServerPoint(const unsigned long row, TA_Base_Core::IData& data)
	{
		m_agentKey = data.getUnsignedLongData(row, ENTITY_AGENTKEY);
        m_pkey = data.getUnsignedLongData(row, MODBUS_POINT_PKEY);
        m_entityKey = data.getUnsignedLongData(row, MODBUS_POINT_ENTITY_KEY);
        m_address = data.getStringData( row, MODBUS_POINT_ADDRESS );
		m_dataLength = data.getStringData( row, MODBUS_POINT_DATALENGTH);
		m_port = (unsigned short)data.getUnsignedLongData(row, MODBUS_POINT_PORT);
		m_name = data.getStringData( row, MODBUS_POINT_NAME );
		m_entity_address = data.getStringData( row, MODBUS_POINT_ENTITY_ADDRESS );
	}

	ModbusServerPoint::~ModbusServerPoint()
	{
	}

    void ModbusServerPoint::invalidate()
    {
    }

} // closes TA_Base_Core
