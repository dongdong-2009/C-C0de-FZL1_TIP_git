/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/05/13 17:20:00 $
  * Last modified by: $Author: dhanshri $
  * 
  * 
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{
	static const std::string DEFAULT_MODBUS_SERVICE_PORT = "502";
	static const std::string DEFAULT_MODBUS_SERVICE_STARTWORD = "0";
	static const std::string DEFAULT_MODBUS_SERVICE_LENGTH = "2000";
    const std::string ModbusServerAgentEntityData::ENTITY_TYPE			= "ModbusServerAgent";
    const std::string ModbusServerAgentEntityData::SERVICE_PORT			= "ServicePort";
	const std::string ModbusServerAgentEntityData::SERVICE_START_WORD	= "ServiceStartWord";
	const std::string ModbusServerAgentEntityData::SERVICE_WORD_LENGTH	= "ServiceWordLength";

	ModbusServerAgentEntityData::ModbusServerAgentEntityData()
    {
    }

	
    ModbusServerAgentEntityData::ModbusServerAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    ModbusServerAgentEntityData::~ModbusServerAgentEntityData()
    {
    }

    std::string ModbusServerAgentEntityData::getServicePort()
	{
        std::string port = getHelper()->getParameter(SERVICE_PORT);
		if ("" == port)
		{
			port = DEFAULT_MODBUS_SERVICE_PORT;
		}
		return port;
	}

    std::string ModbusServerAgentEntityData::getServiceStartWord()
	{
		std::string value = getHelper()->getParameter(SERVICE_START_WORD);
		if ("" == value)
		{
			value = DEFAULT_MODBUS_SERVICE_STARTWORD;
		}
		return value;
	}

	std::string ModbusServerAgentEntityData::getServiceWordLength()
	{
		std::string value = getHelper()->getParameter(SERVICE_WORD_LENGTH);
		if ("" == value)
		{
			value = DEFAULT_MODBUS_SERVICE_LENGTH;
		}
		return value;
	}

	std::string ModbusServerAgentEntityData::getType()
	{
		return getStaticType();
	}

    std::string ModbusServerAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
          
    IEntityData* ModbusServerAgentEntityData::clone ( unsigned long key )
    {
        return new ModbusServerAgentEntityData ( key );
    }

    void ModbusServerAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
