/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/modbus_comms/src/ModbusService.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #15 $
  *
  * Last modification: $DateTime: 2017/07/06 11:21:55 $
  * Last modified by:  $Author: dhanshri $
  * 
  * Description: Opens a listen port to accept TCP/IP client connections (class ModbusClient) and responds in bytes, 
  *				converted from words, where words are firstly converted from datapoint structures.
  */

#include "ModbusService.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
	void PointAddress::print()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_wordAddress = %u , m_bitAddress = %u."
		, m_wordAddress, m_bitAddress);
	}

	ModbusService::ModbusService( std::string& port, unsigned int startWord, unsigned int length, unsigned long socketTO)
		:
		m_socketInited(false),
		m_servicePort(port),
		m_toTerminate(false),
		m_startWord(startWord),
		m_length(length),
		m_InitialisedDatas(false),
		m_socketAcceptTimeout(0, 500 * 1000), //(sec, microsecs)
		m_peersocketTimeout(socketTO)
	{
		try
		{
			if (0 == m_length)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "modbus data length is 0, change to 1");
				m_length = 1;
			}
			unsigned int maxLength = 100 * 1024;	// 100k
			if (maxLength < m_length)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "modbus data length[%d] is bigger then max length[%d], change to [%d]",
					m_length, maxLength, maxLength);
				m_length = maxLength;
			}
			m_datas = new WORD[m_length];
			memset(m_datas, 0, 2 * m_length);
			m_connects.clear();
		}
		catch( std::exception & e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Created ModbusService for portNumber= %s, startWord %d, length %d", m_servicePort.c_str(), m_startWord, m_length );
	
	}

	ModbusService::~ModbusService()
	{
		delete m_datas;
		m_datas = 0;
		TA_Base_Core::ThreadGuard guard(m_connectsLock);
		std::map<void*, ModbusInClient*>::iterator it = m_connects.begin();
		while (m_connects.end() != it)
		{
			delete it->second;
			it->second = 0;
			++it;
		}
	}

	void ModbusService::terminate()
	{
		m_toTerminate = true;
		m_InitialisedDatas = false;
		TA_Base_Core::ThreadGuard guard(m_connectsLock);
		std::map<void*, ModbusInClient*>::iterator it = m_connects.begin();
		while (m_connects.end() != it)
		{
			it->second->terminateAndWait();
			++it;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusService::terminate()");
	}

	void ModbusService::run()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusService  in run()");
		m_toTerminate = false;
		while( !m_toTerminate ) 
		{
			if (!m_socketInited)
			{
				socketInit();
			}
			if (m_socketInited && m_InitialisedDatas)
			{
				socketAccept();
			}
			else
			{
				TA_Base_Core::Thread::sleep(100);
			}
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"This ModbusService thread has been signalled to terminate..");
		if (m_socketInited)
		{
			if(0 == m_acceptor.close())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusService closed this thread's socket.");
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "This ModbusService could not close the socket , ACE errno = %d", ACE_OS::last_error());
			}
		}
		m_socketInited = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusService finished run()");
	}

	void ModbusService::socketInit()
	{
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusService m_socketInited %d", m_socketInited);
		if (m_socketInited)
		{
			return;
		}
		try
		{
			ACE_INET_Addr addr(m_servicePort.c_str());
			if (-1 == m_acceptor.open(addr, 1))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"ModbusService open on port:%s failed, ACE error:%d.", m_servicePort.c_str(), ACE_OS::last_error());
				return;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ModbusService bind on port:%s successful",  m_servicePort.c_str());
			m_socketInited = true;
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"ModbusService open on port:%s failed, ACE error:%d.", m_servicePort.c_str(), ACE_OS::last_error());	
		}
	}

	bool ModbusService::socketAccept()
	{
 		bool bRes = false;		
		try
		{
			ACE_SOCK_Stream* peer = new ACE_SOCK_Stream();
			int m_tcpNoDelay = 0;
			peer->set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );

			ACE_INET_Addr peer_addr;
			if (-1 == m_acceptor.accept (*peer, &peer_addr, &m_socketAcceptTimeout, 0))
			{
				if (ACE_OS::last_error() == ETIMEDOUT)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Encountered ETIMEDOUT, wait again for peer connection..");
				}
				else if (ENOTSOCK == ACE_OS::last_error())
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
						"ModbusService not a socket port: %s failed, ace error:%d.", m_servicePort.c_str(), ACE_OS::last_error());	
				}
				delete peer;
				peer = 0;
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Peer Connection accepted.");
				static ACE_TCHAR peer_name[MAX_HOST_ADDRESS_LENGTH];
				peer_addr.addr_to_string (peer_name, MAX_HOST_ADDRESS_LENGTH);
				std::string peerAddr(peer_name);
				int portNumberClient = peer_addr.get_port_number();
				ModbusInClient* connect = new ModbusInClient(peer, peerAddr, this, m_startWord, m_length);
				addNewClient(connect);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Created ModbusInClient for peerAddr %s and port %d, StartWord %u, Length %u",
								 peerAddr.c_str(),portNumberClient , m_startWord, m_length);
				connect->start();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ModbusService on port:%s accept a new connection:%s",  
					m_servicePort.c_str(), peerAddr.c_str());
				bRes = true;
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "on port:%s Encounterd accept() error!", m_servicePort.c_str());
			bRes = false;	
			return bRes;
		}
		
		return bRes;
	}

	void ModbusService::addNewClient(ModbusInClient* clientConnect)
	{
		TA_Base_Core::ThreadGuard guard(m_connectsLock);
		std::map<void*, ModbusInClient*>::iterator it = m_connects.begin();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In addNewClient(), current size = %d", m_connects.size());
		int iloop = 1;
		while (m_connects.end() != it)
		{
			std::string sRemoteAddr = it->second->getRemoteAddr();
			if (it->second->isClosed())
			{
				it->second->terminateAndWait();
				delete it->second;
				m_connects.erase(it++);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This iteration client connection[%d] for address [%s] is closed, hence erased from connects vector.", iloop, sRemoteAddr.c_str());
				iloop--;
			}
			else
			{
				++it;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This iteration client connection[%d] for address [%s] is not closed.", iloop++, sRemoteAddr.c_str());
			}
		}
		m_connects[clientConnect] = clientConnect;
	}

	bool ModbusService::getData(char* buf, unsigned int startWord, unsigned int wordCount)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"In getData(): for m_servicePort %s from startWord %d, total %d.", m_servicePort.c_str(),startWord, wordCount);
		if( m_InitialisedDatas == false)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "m_datas is not ready to send to Peer connections..");
			return false;
		}

		if (startWord < m_startWord || startWord + wordCount > m_startWord + m_length)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "getData error, paras(%d, %d) out of range (%d, %d).",
				startWord, wordCount, m_startWord, m_length);
			return false;
		}
		TA_Base_Core::ThreadGuard guard(m_dataLock);
		unsigned int dataIndex = startWord - m_startWord;
		unsigned int bufIndex = 0;
		//m_datas[0] has all quality which is register=1. 
		//Currently no need to get quality from m_datas[0]

		//MUST be sure that m_datas is updated before it is accessed by ModbusClient thread's processRequest()
		while (bufIndex < wordCount)
		{
			buf[2 * bufIndex] = (m_datas[dataIndex] >> 8);
			buf[2 * bufIndex + 1] = (m_datas[dataIndex] & 0xff);
			++dataIndex;
			++bufIndex;
		}
		/*LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "m_datas dataIndex %d : wordCount %d, byteCount %d.", 
			dataIndex, wordCount, (2 * bufIndex));*/
		return true;
	}
	
	//Currently not used.
	bool ModbusService::setData(unsigned int wordAddress, WORD value)
	{
		if (wordAddress < m_startWord || wordAddress >= m_startWord + m_length)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "setData error, wordAddress(%d) is out of range (%d, %d).",
				wordAddress, m_startWord, m_length);
			return false;
		}
		TA_Base_Core::ThreadGuard guard(m_dataLock);
		m_datas[wordAddress - m_startWord] = value;
		return true;
	}

	bool ModbusService::setData(PointValueVector& values)
	{
		unsigned int i = 0;
		unsigned int nSize = values.size();
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "ServicePort %s:Setting datapoints in buffer, current DP update count = %d."
									, m_servicePort.c_str(),nSize);
		//memset(m_datas, 0, 2 * m_length);

		{
			TA_Base_Core::ThreadGuard guard(m_dataLock);
			for (i = 0; i < nSize; ++i)
			{
				TA_Base_Bus::PointValueStruct& point = values[i];
				if ((!VALUE_IN_RANGE(m_startWord, m_length, point.m_address.m_wordAddress))
					|| (16 < point.m_size && (!VALUE_IN_RANGE(m_startWord, m_length, point.m_address.m_wordAddress + 1))))
				{
					LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "address[%d] is out of range[%d, len:%d], size[%d]", 
						point.m_address.m_wordAddress, m_startWord, m_length, point.m_size);
					continue;
				}
				if (16 <= point.m_address.m_bitAddress)
				{
					LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "address[%d],bit[%d] is out of range[0, 16)", 
						point.m_address.m_wordAddress, point.m_address.m_bitAddress);
					continue;
				}
				//LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "DP update: address %d, size %d", point.m_address.m_wordAddress, point.m_size);

				WORD& pos = m_datas[point.m_address.m_wordAddress - m_startWord];
				switch (point.m_size)
				{
				case 1:
					if (point.m_value > 0)
					{
						BIT_SET(pos, point.m_address.m_bitAddress);
					}
					else
					{
						BIT_CLEAR(pos, point.m_address.m_bitAddress);
					}
					break;
				case 16:
					pos = point.m_value & 0xffff;
					//pos = point.m_Dblvalue & 0xffff;
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "DP update: address %d, val=%u, Word=%u", point.m_address.m_wordAddress, point.m_value, pos);
					break;
				case 32:
					pos = point.m_value >> 16;	//high word.
					m_datas[point.m_address.m_wordAddress - m_startWord + 1] = point.m_value & 0xffff;	//low word.
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "DP update: address %d, val=%u, HighWord=%u, LowWord=%u", point.m_address.m_wordAddress, point.m_value, pos, (point.m_value & 0xffff));
					break;
				default:
					LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "setData error, value length(%d) is not supported", point.m_size);
					break;
				}
			}
		}

		return true;
	}
}
	