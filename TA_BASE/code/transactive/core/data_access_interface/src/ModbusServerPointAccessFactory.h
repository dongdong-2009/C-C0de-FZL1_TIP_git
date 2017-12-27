/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerPointAccessFactory.h $
  * @author:  Anita Lee
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/19 17:01:52 $
  * Last modified by:  $Author: dhanshri $
  * 
  * Data is primarily retrieved from the modbus_server_point table
  *
  */


#if !defined(DAI_MODBUSSERVERPOINTACCESSFACTORY_H_)
#define DAI_MODBUSSERVERPOINTACCESSFACTORY_H_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
	class IModbusServerPoint;
	class IModbusServerAnaloguePoint;

	class ModbusServerPointAccessFactory
	{

	public:
        virtual ~ModbusServerPointAccessFactory() {};

		static void getAllModbusServerPoint(std::vector<IModbusServerPoint*>& ModbusServerPoints);
		static void getAllSubscribedAgents(std::vector<unsigned long>& vAgentKeys);
		static void getAllAnalogueData(std::vector<IModbusServerAnaloguePoint*>& ModbusAServerAPoints);

    private:
		ModbusServerPointAccessFactory() {};
		ModbusServerPointAccessFactory( const ModbusServerPointAccessFactory& theModbusServerPointAccessFactory);
		ModbusServerPointAccessFactory& operator=(const ModbusServerPointAccessFactory &);
    };

} // closes TA_Base_Core

#endif // !defined(DAI_MODBUSSERVERPOINTACCESSFACTORY_H_)
