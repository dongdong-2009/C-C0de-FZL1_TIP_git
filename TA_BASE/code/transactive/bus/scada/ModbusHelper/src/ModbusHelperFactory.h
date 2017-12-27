#ifndef MODBUSFACTORY_H
#define MODBUSFACTORY_H

#include "IModbusHelper.h"
#include "ModbusHelper.h"
#include "boost/shared_ptr.hpp"
#include "core/exceptions/src/TransactiveException.h"
/**
* the purpose of this class is to reduce the impact of changes in Helper class to client code.
*/
namespace TA_Base_Bus {

	class NotFoundFEPConfigException :public TA_Base_Core::TransactiveException
	{
	public:
		NotFoundFEPConfigException(const std::string& message) throw();
		~NotFoundFEPConfigException() throw();
	};

	class ModbusHelperFactory
	{
	public: 
	
		/**
		* enumerator of subsystems which will use this class
		*/
		enum EnumSubSystem{
			PA,
			CCTV,
			PIDS,
			BAS,
			FAS,
			SIGNALLING
		};
		/**
		* create an instance of the helper class
		*
		* @param  primaryHost    ip or host name of the primary FEP. Actually the primary and secondary FEPs can swap their roles during runtime
		* @param  primaryPort    port number or service number of the primary FEP
		* @param  secondaryHost  ip or host name of the secondary FEP
		* @param  secondaryPort  port number or service number of the secondary FEP
		* @param  slaveAddress   slave address, unit identifier or device id
		* @param  retries        retry times if failed to send data
		* @param  socketTimeout  connect or communication time out for modbus connection
		* @param  isSTEModbus    if this is a standard tcp modbus connection or ste modbus
		*
		* @return boost::shared_ptr<IModbusHelper>
		*/
		static boost::shared_ptr<IModbusHelper> createHelper(std::string primaryHost, std::string primaryPort, std::string secondaryHost, std::string secondaryPort,
			std::string slaveAddress, int retries = 3, unsigned long socketTimeout = 1000, bool isSTEModbus = true);

		/**
		* create the helper according configurations in FEP_CONFIG table. If failed to get the FEP configurations, throw NotFoundFEPConfigException
		* @param location    location id
		* @param subsystem   constant subsystem
		* 
		*/
		static boost::shared_ptr<IModbusHelper> createHelper(unsigned long location, EnumSubSystem subsystem);
	private:
		/**
		* get FEP configuration according to location and subsystem parameters. 
		* @param location       location key
		* @param subsystem      subsystem enum
		* @param primaryHost    out parameter, primary FEP ip or host name
		* @param primaryPort    out parameter, primary FEP port number or service number
		* @param secondaryHost  out parameter, secondary FEP ip or host name
		* @param secondaryPort  out paramter, secondary FEP port number or service number
		* @param slaveAddress   out parameter, slave address, device id, unit identifier
		* @param startAddress   out parameter, startaddress of datablocks 
		* @param retries        out parameter, retry times if failed to send data
		* @param socketTimeout  connect or communication time out for modbus connection
		* @param isSTEModbus    out parameter, if this is a standard tcp modbus connection or ste modbus
		*/
		static bool getConfig(unsigned long location,EnumSubSystem subsystem,std::string& primaryHost,std::string& primaryPort, 
			std::string& secondaryHost, std::string& secondaryPort, int& slaveAddress, int& startAddress, int& retries, 
			unsigned long &socketTimeout, bool& isSTEModbus);		
	/*	static bool getConfigPA(unsigned long location,std::string& primaryHost,std::string& primaryPort, std::string& secondaryHost, std::string& secondaryPort,std::string& slaveAddress);
		static bool getConfigCCTV(unsigned long location,std::string& primaryHost,std::string& primaryPort, std::string& secondaryHost, std::string& secondaryPort,std::string& slaveAddress);
		static bool getConfigPIDS(unsigned long location,std::string& primaryHost,std::string& primaryPort, std::string& secondaryHost, std::string& secondaryPort,std::string& slaveAddress);*/
	};
}
#endif
