/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/modbus_comms/src/ModbusInClient.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2017/07/11 18:51:14 $
  * Last modified by:  $Author: yong.liu $
  * 
  * .
  */

#include <iostream>
#include <iomanip>
#include <sstream>

#include "ModbusInClient.h"
#include "core/utilities/src/DebugUtil.h"
//#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
	ModbusInClient::ModbusInClient(ACE_SOCK_Stream*	connect, std::string& remoteAddr, ModbusService* service, unsigned int startWord, unsigned int length)
	:
	m_toTerminate(false),
	m_bClosed(false),
	m_connect(connect),
	m_remoteAddr(remoteAddr),
	m_service(service),
	m_startWord(startWord),
	m_length(length),
	m_socketTimeout(0, 2000 * 1000)//(sec, microsecs)
	{
		m_buf = new char[m_length * 2]; // word to char.
		m_socketTimeout = ACE_Time_Value(0, m_service->getPeerSocketTimeout()* 1000);


	}

	ModbusInClient::~ModbusInClient()
	{
		delete m_connect;
		delete[] m_buf;
		m_connect = 0;
	}

	void ModbusInClient::terminate()
	{
		m_toTerminate = true;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusInClient::terminate()");
	}

	
	void ModbusInClient::run()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusInClient  in run()");
		while( !m_toTerminate ) 
		{
			if (!readRequest())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, " readRequest() returned false, breaking loop.");
				break;
			}
			processRequest();
			if (!responseRequest())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, " responseRequest() returned false, breaking loop.");
				break;
			}
		}
		m_toTerminate = true;
		m_bClosed = true;
		if(m_connect->close() == -1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Peer %s:Call to ACE_SOCK_Stream::close() failed.",m_remoteAddr.c_str());
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusInClient finished run()");
	}

	bool ModbusInClient::readRequest()
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"In ModbusInClient::readRequest()");
		if (readData(m_buf, MODBUS_MBAP_HEADER_LENGTH))
		{
			WORD w = 0;
			getWord(m_buf + MODBUS_MBAP_HEADER_LEN_INDEX, w);
			int moreReadLength = w;
			moreReadLength = moreReadLength - 1;	// 1 for slave id. the byte is in header structrue.
			if (readData(m_buf + MODBUS_MBAP_HEADER_LENGTH, moreReadLength))
			{
				return true;
			}
		}
		return false;
	}

	bool ModbusInClient::processRequest()
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"In ModbusInClient::processRequest()");
		bool bOk = false;
		char eCode = 0;
		char functionCode = m_buf[MODBUS_REQ_FUNCTION_CODE_INDEX];
		if (MB_FC_READ_INPUT_REGISTERS_CODE == functionCode)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Processing MB_FC_READ_INPUT_REGISTERS_CODE");		
			bOk = readInputRegisters(eCode);
		}
		else
		{
			bOk = false;
			eCode = MB_EC_ILLEGAL_FUNCTION_ERR;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Processing MB_EC_ILLEGAL_FUNCTION_ERR");		
		}
		if (!bOk)
		{
			m_buf[MODBUS_MBAP_HEADER_LEN_INDEX] = 0;
			m_buf[MODBUS_MBAP_HEADER_LEN_INDEX + 1] = 3;
			m_buf[MODBUS_REQ_FUNCTION_CODE_INDEX] = m_buf[MODBUS_REQ_FUNCTION_CODE_INDEX] | 0x80;
			m_buf[MB_RESPONSE_EXCEPTION_CODE_INDEX] = eCode;
		}
		return true;
	}

	bool ModbusInClient::responseRequest()
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"In ModbusInClient::responseRequest()");
		WORD w = 0;
		getWord(m_buf + MODBUS_MBAP_HEADER_LEN_INDEX, w);
		int toWriteLength = w;
		toWriteLength = toWriteLength + MODBUS_MBAP_HEADER_LENGTH - 1; // 1 for slave id.
		if (writeData(m_buf, toWriteLength))
		{
			return true;
		}
		return false;
	}

	bool ModbusInClient::readData(char* buf, const unsigned int length)
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"In ModbusInClient::readData()");
		unsigned int totalBytesRead = 0;
		size_t bytesRead = 0;
		int result = 0;
		int WaitForReConnectCount = 3;
	
		while( !m_toTerminate ) 
		{
			if (totalBytesRead == length)
			{
				break;
			}

			result = m_connect->recv_n(buf + totalBytesRead, length - totalBytesRead, &m_socketTimeout, &bytesRead);
			totalBytesRead += bytesRead;
			if (-1 == result)
			{
				if (ETIME != errno)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
							"ModbusInClient read data from %s failed, ace error:%d.", m_remoteAddr.c_str(), ACE_OS::last_error());
					return false;
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Result = -1 on port %s as errno = %d (ETIME). Try again (max 3 times) for this recv ..", m_remoteAddr.c_str() , ACE_OS::last_error());
					// Check if need to return here...
					if( WaitForReConnectCount == 1)
					{
						// Count reached 3 hence returning as probably the peer connection has died so need to terminate thread.					
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Result = -1 on port %s as errno = %d (ETIME) for 3 times. Returning..", m_remoteAddr.c_str() , ACE_OS::last_error());
						return false;
					}
					WaitForReConnectCount--;
					//return false;
				}
			}
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
						"ModbusInClient read data from %s, bytes:%d.", m_remoteAddr.c_str(), totalBytesRead);
		hexDump( "ModbusInClient::readData: ", buf, length);
		return true;
	}

	bool ModbusInClient::writeData(const char* buf, const unsigned int length)
	{
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusInClient write data %d", length);
		hexDump( "ModbusInClient::writeData: ", buf, length);
		unsigned int totalBytesWritten = 0;
		int result = 0;
		while( !m_toTerminate ) 
		{
			result = m_connect->send_n(m_buf + totalBytesWritten, length - totalBytesWritten, &m_socketTimeout);
			if (result > 0)
			{
				totalBytesWritten += result;
			}
			else
			{
				if (ETIME != errno)
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
						"ModbusInClient write to client %s failed, ace error:%d.", m_remoteAddr.c_str(), ACE_OS::last_error());
                }
				else
				{
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Timeout writing to %s", m_remoteAddr.c_str());
				}
				return false;
			}
			if (totalBytesWritten >= length)
			{
				break;
			}
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
						"ModbusInClient write to client %s, bytes:%d.", m_remoteAddr.c_str(), totalBytesWritten);
		return true;
	}

	bool ModbusInClient::readInputRegisters(char& eCode)
	{
		bool bOK = false;
		WORD w = 0;
		getWord(m_buf + MODBUS_REQ_START_ADDRESS_INDEX, w);
		unsigned int addr = w;
		getWord(m_buf + MODBUS_REQ_NUM_OF_REGISTER_INDEX, w);
		unsigned int length = w;
		if (addr < m_startWord || addr + length > m_startWord + m_length)
		{
			eCode = MB_EC_ILLEGAL_DATA_ADDRESS_ERR;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "readInputRegisters - Encountered MB_EC_ILLEGAL_DATA_ADDRESS_ERR for addr %d, length %d: m_startWord %d, m_length %d",  addr, length, m_startWord , m_length);
		}
		else
		{
			if (m_service->getData(m_buf + MB_RESPONSE_DATA_START_INDEX, addr, length))
			{
				setWord(m_buf + MODBUS_MBAP_HEADER_LEN_INDEX, w * 2 + 3); // 3 for:id, function, length.
				m_buf[MB_RESPONSE_DATA_LENGTH_INDEX] = 0;	//just set to 0.
				bOK = true;
			}
			else
			{
				eCode = MB_EC_SLAVE_DEVICE_FAILURE_ERR;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "readInputRegisters - Encountered MB_EC_SLAVE_DEVICE_FAILURE_ERR from Peer at %s.", m_remoteAddr.c_str());
			}
		}
		return bOK;
	}

	void ModbusInClient::hexDump( const char* prefix, const char* buffer, int length, bool fullDump )
	{
		static const int MAX_BYTES_LOGGED = 128;

		if ( true == fullDump || DebugUtil::getInstance().getLevel() >= DebugUtil::DebugDebug )
        {
            std::ostringstream message;
            message << prefix
                    << std::hex << std::uppercase << std::setfill('0');
        
            int nDumpLength = length;
            
            if ( false == fullDump )
            {
                nDumpLength = length < MAX_BYTES_LOGGED ? length : (MAX_BYTES_LOGGED - 2);
            }

            for( int i=0; i < nDumpLength; ++i )
            {
                unsigned int value = 0xff & (unsigned int) buffer[i];
                message << " " << std::setw(2) << value;
            }
        
            if ( length > MAX_BYTES_LOGGED )
            {
                unsigned int crc1 = buffer[ length - 2] & 0xff;
                unsigned int crc2 = buffer[ length - 1 ] & 0xff;
                message << " ... " << std::setw(2) << crc1 << " " << std::setw(2) << crc2;
            }

            LOG_GENERIC( SourceInfo, fullDump ? DebugUtil::DebugInfo : DebugUtil::DebugDebug, "%s", message.str().c_str() );
        }
    }
}
