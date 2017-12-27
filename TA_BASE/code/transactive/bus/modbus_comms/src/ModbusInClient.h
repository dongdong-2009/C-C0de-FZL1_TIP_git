/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/modbus_comms/src/ModbusInClient.h $
  * @author:  A. Ivankovic
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/05/06 11:28:35 $
  * Last modified by:  $Author: dhanshri $
  * 
  * .
  */

#ifndef __MODBUS_IN_CLIENT_H__
#define __MODBUS_IN_CLIENT_H__

#include <string>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "DataBlock.h"
#include "ModbusService.h"
#include "ModbusException.h"

// Remark : the different fields are encoded in Big-endian.
// index from 0;
const int MODBUS_MBAP_HEADER_LENGTH = 7;	//byte, MBAP(MODBUS Application Protocol header).
const int MODBUS_MBAP_HEADER_LEN_INDEX = 4;
const int MODBUS_REQ_FUNCTION_CODE_INDEX = 7;
const int MODBUS_REQ_START_ADDRESS_INDEX = 8;
const int MODBUS_REQ_NUM_OF_REGISTER_INDEX = 10;
const int MB_RESPONSE_DATA_LENGTH_INDEX = 8;
const int MB_RESPONSE_DATA_START_INDEX = 9;

const int MB_RESPONSE_EXCEPTION_CODE_INDEX = 8;

// modbus function code.
const char MB_FC_READ_COILS_CODE =                  0x01;
const char MB_FC_READ_DISCRETE_INPUTS_CODE =        0x02;
const char MB_FC_READ_HOLDING_REGISTERS_CODE =      0x03;
const char MB_FC_READ_INPUT_REGISTERS_CODE =        0x04;
const char MB_FC_WRITE_SINGLE_REGISTER_CODE =       0x06;
const char MB_FC_WRITE_MULTIPLE_COILS_CODE =        0x0F;
const char MB_FC_WRITE_MULTIPLE_REGISTERS_CODE =    0x10;
const char MB_FC_MASK_WRITE_REGISTER_CODE =         0x16;

// modbus exception code.
const char MB_EC_FUNCTION_TIMEOUT_ERR =				0x00;
const char MB_EC_ILLEGAL_FUNCTION_ERR =             0x01;
const char MB_EC_ILLEGAL_DATA_ADDRESS_ERR =         0x02;
const char MB_EC_ILLEGAL_DATA_VALUE_ERR =           0x03;
const char MB_EC_SLAVE_DEVICE_FAILURE_ERR =         0x04;
const char MB_EC_ACKNOWLEDGE_ERR =                  0x05;
const char MB_EC_SLAVE_DEVICE_BUSY_ERR =            0x06;
const char MB_EC_MEMORY_PARITY_ERR =                0x08;
const char MB_EC_GATEWAY_PATH_UNAVAILABLE_ERR =     0x0A;
const char MB_EC_TARGET_FAILED_TO_RESPOND_ERR =     0x0B;

const int MAX_HOST_ADDRESS_LENGTH = 512;

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	class ModbusService;

	class ModbusInClient : public TA_Base_Core::Thread
	{
	public:
		ModbusInClient(ACE_SOCK_Stream*	connect, std::string& clientAddr, ModbusService* service, unsigned int startWord, unsigned int length);

		~ModbusInClient();

		virtual void	run(); 
		virtual void	terminate();

		bool isClosed(){return m_bClosed;};
		std::string	getRemoteAddr()
		{
			return m_remoteAddr;
		}
		bool isSupportFunction(const char code);

		virtual bool readRequest();
		virtual bool processRequest();
		virtual bool responseRequest();

	protected:
		bool readData(char* buf, const unsigned int length);
		bool writeData(const char* buf, const unsigned int length);

		void getWord(char* buf, WORD & word){word = (unsigned char)buf[0]; word = (word << 8) | (unsigned char)buf[1];};
		void setWord(char* buf, WORD word){buf[0] = word >> 8; buf[1] = word & 0xff;};

		bool readInputRegisters(char& eCode);

		void hexDump( const char* prefix, const char* buffer, int length, bool fullDump = false );
	
		bool isConnected()
		{
			return (false == m_bClosed);
		}

	private:
		volatile bool		m_toTerminate;
		volatile bool		m_bClosed;
		ACE_SOCK_Stream*	m_connect;
		std::string			m_remoteAddr;
		ModbusService*		m_service;
		unsigned int		m_startWord;
		unsigned int		m_length;
		ACE_Time_Value		m_socketTimeout;
		char*				m_buf;
	};
}
#endif	//__MODBUS_IN_CLIENT_H__
