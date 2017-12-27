
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerPoint.h $
  * @author:  Derrick Liew
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * 
  *
  */
#if !defined(DAI_MODBUSSERVERPOINT_H)
#define DAI_MODBUSSERVERPOINT_H

#include "core/data_access_interface/src/IModbusServerPoint.h"

namespace TA_Base_Core
{
	class IData;

    class ModbusServerPoint : public IModbusServerPoint
    {

    public:
		ModbusServerPoint(const unsigned long row, TA_Base_Core::IData& data);
		~ModbusServerPoint();
        
        /**
         * Refer to IModbusServerPoint.h for a description of this method.
         */
		unsigned long getKey() {return m_pkey;};

        /**
         * Refer to IModbusServerPoint.h for a description of this method.
         */
		virtual void invalidate();

		unsigned long getAgentKey(){return m_agentKey;};
		std::string getAddress() {return m_address;};
		unsigned long getEntitykey() {return m_entityKey;};
		unsigned short getPort(){return m_port;};
		std::string getName(){return 	m_name;};
		std::string getLength(){return 	m_dataLength;}
		std::string getEntityAddress() {return m_entity_address;}

	public:
		static const std::string	ENTITY_AGENTKEY;
		static const std::string	MODBUS_POINT_PKEY;
		static const std::string	MODBUS_POINT_ENTITY_KEY;
		static const std::string	MODBUS_POINT_ADDRESS;
		static const std::string	MODBUS_POINT_PORT;
		static const std::string	MODBUS_POINT_DATALENGTH;
		static const std::string	MODBUS_POINT_ENTITY_ADDRESS;
		static const std::string	MODBUS_POINT_NAME;

    private:

        // Assignment operator not used so it is made private
        ModbusServerPoint( const ModbusServerPoint&);            
		ModbusServerPoint& operator=(const ModbusServerPoint&);

		unsigned long	m_agentKey;
        unsigned long	m_pkey;
        unsigned long	m_entityKey;
        std::string		m_address;
		std::string		m_entity_address;
		std::string		m_dataLength;
		unsigned short	m_port;
		std::string		m_name;

    };
} // closes TA_Base_Core

#endif // !defined(DAI_MODBUSSERVERPOINT_H)
