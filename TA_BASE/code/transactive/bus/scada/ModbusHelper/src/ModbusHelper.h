#ifndef MODBUSHELPER_H
#define MODBUSHELPER_H

#include <string>

#include "bus/modbus_comms/src/DataBlock.h"
#include "IModbusHelper.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "boost/shared_ptr.hpp"
//using namespace std;
namespace TA_Base_Bus {
	class ModbusHelper: public IModbusHelper{
		
		
	public:

		/**
		* In this class, it doesn't know which station system(PA, PIDS, CCTC etc) it will communicate,so you need provide the following parameters 
		* when you new an instance of ModbusHelper.
		*
		* you can get all the parameters information from your systems' FEP configuration
		*
		* @param primaryHost      host name or ip of the primary FEP
		* @param secondaryHost    host name or ip of the secondary FEP 
		* @param service          service port
		* @param slaveAddress     slave address / device id.  polling 3011-3014, command port n
		*/

		ModbusHelper(std::string primaryHost,  std::string primaryPort, std::string secondaryHost,std::string secondaryPort, std::string slaveAddress, int retries = 3, unsigned long socketTimeout = 1000, bool steModbus = true);

		ModbusHelper(std::string primaryHost,  std::string primaryPort, std::string secondaryHost,std::string secondaryPort, int slaveAddress, int startAddress,int retries = 3,unsigned long socketTimeout = 1000, bool steModbus = true);

		virtual ~ModbusHelper();

		/**
		* find the active FEP and send data
		* primary host, secondary host, service and slave address can get from FEP configuration file. 
		* Function code will be 0x10
		*/
		virtual int SendData(DataBlock<WORD>& block);
		virtual int SendData(DataBlock<WORD>& block,WORD startAddess);

		virtual	int SendData(DataBlock<BYTE>& block);

		virtual int SendData(std::string& block);
		/**
		* Appends a WORD to the supplied communication data block, the size of datablock will be changed.
		* When sending, bytes of the word will be laid out according Modbus standard wordformat

		*
		* @param block  The datablock to append to.
		* @param word     The word value to append.
		*/
		virtual void appendWord( DataBlock<WORD>&block, WORD word );

		/**
		* If the size of datablock is less than offset + 2, it will be resized to offset + 2. 
		* If the size of datablock is larger than offset + 2, the size of datablock is not be changed. 
		* When sending , bytes of the word will be laid out according Modbus standard wordformat
		*
		* @param block  The datablock to append to.
		* @param offset the start location to fill the word
		* @param word   The word value to append.
		# 
		*/
		virtual void fillWord(DataBlock<WORD>& block, int offset, WORD word);	 
	private:
		class FEPConnectionInfo
		{
		public: 
			FEPConnectionInfo(std::string host, std::string port,unsigned long socketTimeout, bool steModbus)
				:
			m_bSteModbus(steModbus),
			m_socketTimeout(socketTimeout),
			m_host(host),
			m_port(port)
			{
				m_modbusConn.reset();
			}

			boost::shared_ptr<IModbus> getConnection()
			{
				if (m_modbusConn.get() == NULL)
				{
					if (m_bSteModbus)
					{
						m_modbusConn = boost::shared_ptr<IModbus>(new SteModbusTcpConnection(m_host, m_port, m_socketTimeout));
					}
					else
					{
						m_modbusConn = boost::shared_ptr<IModbus>(new ModbusTcpConnection(m_host, m_port, m_socketTimeout));
					}
				}

				return m_modbusConn;
			}

			void resetConnection()
			{
				m_modbusConn.reset();
			}

			bool isSTEModbus()
			{
				return m_bSteModbus;
			}

		private:
			bool		 	m_bSteModbus;
			unsigned long 	m_socketTimeout;
			std::string	 	m_host;
			std::string  	m_port;
			boost::shared_ptr<IModbus> m_modbusConn;
		};
	private:

		inline bool  internalSendData(DataBlock<WORD>& block, boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception);
		inline bool  internalSendData(DataBlock<WORD>& block,WORD startAddress, boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception);
		inline bool  internalSendData(DataBlock<BYTE>& block, boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception);
		inline bool  internalSendData(std::string& block, boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception);
		
		std::string   combineExceptions(ModbusException & primaryEx, ModbusException& secondaryEx);		
		std::string  DataBlock2String(DataBlock<WORD>& block);
		std::string  DataBlock2String(DataBlock<BYTE>& block);
		void String2DataBlock(std::string& string, DataBlock<BYTE> & block);
		/*
		* convert the data to a hex string
		*/
		void HexString(const std::string& data,std::string& output );
	private:
		std::string slaveAddress_;
		int deviceId_;
		int startAddress_; //not used anyway
		int retries_;

		boost::shared_ptr<FEPConnectionInfo> primaryFep_;
		boost::shared_ptr<FEPConnectionInfo> standbyFep_;

	};
}
#endif