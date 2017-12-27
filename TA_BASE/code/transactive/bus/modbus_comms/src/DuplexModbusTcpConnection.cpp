/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/DuplexModbusTcpConnection.cpp $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  */

#include <sstream>
#include <iomanip>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
#include <ace/os_include/netinet/os_tcp.h>
#include <ace/Handle_Set.h>
#include <ace/ACE.h>
#include "bus/modbus_comms/src/DuplexModbusTcpConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"

#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const int DuplexModbusTcpConnection::MAX_TRANSACTION_ID = 0xffff;     // maximum of 16 bits
    const int DuplexModbusTcpConnection::MODBUS_PROTOCOL_ID = 0;
    const int DuplexModbusTcpConnection::MBAP_HEADER_SIZE = 7;
    const int DuplexModbusTcpConnection::ERROR_LENGTH     = 5;            // ERROR reply length.
	const int DuplexModbusTcpConnection::INVALID_TRANSACTION_ID_NO = -1;
	const unsigned long DuplexModbusTcpConnection::SELECT_SOCKET_TIME_OUT = 500;

	
	ModbusTcpReplyItem::ModbusTcpReplyItem(int transactiveId)
		:
	m_getReplyed(false),
	m_transactiveId(transactiveId),
	m_exceptionId(ModbusException::NO_EXCEPTION)
	{
	}

	ModbusTcpReplyItem::~ModbusTcpReplyItem()
	{
		m_condition.signal();
	}

	int ModbusTcpReplyItem::getId() 
	{
		return m_transactiveId; 
	}

	void ModbusTcpReplyItem::setException(ModbusException::EModbusFail exceptionId, const std::string& reason) 
	{ 
		m_exceptionId = exceptionId;
		m_reason = reason;
		m_getReplyed = true;
	}

	void ModbusTcpReplyItem::getReply(std::string& reply)
	{ 
		throwModbusException();
		reply = m_reply; 
	}

	void ModbusTcpReplyItem::setReply(const std::string& reply)
	{
		m_reply = reply;
		m_getReplyed = true;
	}

	bool ModbusTcpReplyItem::waitForResponse(unsigned long millSec)
	{
		bool noReplyForTimeOut = false;
		if ( !m_getReplyed )
		{
			int ret = m_condition.timedWait(millSec);
			bool timeOut = (ret == 1 ? false : true);
			if (timeOut && !m_getReplyed)
			{
				noReplyForTimeOut = true;
			}

			if (timeOut && m_getReplyed)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "reply item:%d timeout but get reply", m_transactiveId);
			}
		}
		return noReplyForTimeOut;
	}

	void ModbusTcpReplyItem::notifyReply()
	{
		m_condition.signal();
	}

	void ModbusTcpReplyItem::throwModbusException()
	{
		if (m_exceptionId != ModbusException::NO_EXCEPTION)
		{
			throw TA_Base_Bus::ModbusException(m_reason, m_exceptionId);
		}
	}

    DuplexModbusTcpConnection::DuplexModbusTcpConnection( const std::string& host,
                                              const std::string& service,
                                              long socketTimeout,
                                              bool tcpNoDelay)
    :
    m_transactionIdSeed( 0 ),
    m_tcpNoDelay( tcpNoDelay ? 1 : 0 ),
	m_running( true ),
    m_closed( true ),
    m_host( host ),
    m_service( service ),
    m_socketTimeout( socketTimeout / 1000,  (socketTimeout % 1000) * 1000 ),
	m_timeOut(socketTimeout),
	m_connectedSemphore(0)
    {
        FUNCTION_ENTRY("Constructor");

		m_socketSelectTimeOut.set(SELECT_SOCKET_TIME_OUT / 1000,  (SELECT_SOCKET_TIME_OUT % 1000) * 1000);

		start();
		
        FUNCTION_EXIT;
    }


    DuplexModbusTcpConnection::~DuplexModbusTcpConnection()
    {
        FUNCTION_ENTRY("Destructor");

		disable();

		terminateAndWait();

        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
		m_closed = true;
		m_sockStream.close();

        FUNCTION_EXIT;
    }


    void DuplexModbusTcpConnection::disable()
    {
        FUNCTION_ENTRY("disable");

        ModbusConnection::disable();
        closeSocket();

        FUNCTION_EXIT;
    }


    bool DuplexModbusTcpConnection::isConnected()
    {
		TA_Base_Core::ThreadReadGuard guard(m_socketStreamLock);
        return (false == m_closed);
    }

    void DuplexModbusTcpConnection::closeSocket()
    {
        FUNCTION_ENTRY("closeSocket");

		TA_Base_Core::ThreadGuard guard(m_socketStreamLock);

		m_closed = true;

        FUNCTION_EXIT;
    }


    void DuplexModbusTcpConnection::writeRequest( const std::string& request )
    {
        FUNCTION_ENTRY("writeRequest");

        if ( request.size() == 0 )
        {
            TA_THROW( ModbusException( "No data in the request", ModbusException::NO_REQUEST_DATA ) );
        }

		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}

		int transactiveId = generateNextTransactiveId();
		unsigned int threadId = TA_Base_Core::Thread::getCurrentThreadId();
		boost::shared_ptr<ModbusTcpReplyItem> replyItem(new ModbusTcpReplyItem(transactiveId));
		{
			TA_Base_Core::ThreadGuard guard(m_replyMapLock);
			m_replyMap[threadId] = replyItem;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "add reply item:%d-size:%d", transactiveId, m_replyMap.size());
		}		

        int length = request.size() + 1;
		/*
        header[0] = MSB( m_transactionID );
        header[1] = LSB( m_transactionID );
        header[2] = MSB( MODBUS_PROTOCOL_ID );
        header[3] = LSB( MODBUS_PROTOCOL_ID );
        header[4] = MSB( length );
        header[5] = LSB( length );
        header[6] = getDeviceAddress();
		*/
		std::string requestPacket;
		requestPacket += MSB( transactiveId );
		requestPacket += LSB( transactiveId );
		requestPacket += MSB( MODBUS_PROTOCOL_ID );
		requestPacket += LSB( MODBUS_PROTOCOL_ID );
		requestPacket += MSB( length );
		requestPacket += LSB( length );
		requestPacket += getDeviceAddress();

		requestPacket.append(request);
		
		try
		{
			// this lock protect multi-thread write data into socket at the 
			// same time, we need write entire request into socket one by one
			TA_Base_Core::ThreadGuard guard( m_requestWriteLock );
			writeSocketData( requestPacket.c_str(), requestPacket.size() );
		}	
		catch(TA_Base_Bus::ModbusException& e)
		{
			removeReplyItem(threadId);
			throw e;
		}

		bool timeOut = replyItem->waitForResponse(m_timeOut);
		if (timeOut)
		{
			removeReplyItem(threadId);
			TA_THROW( ModbusException( "get reply timed out", ModbusException::REPLY_TIMEOUT ) );
		}

        FUNCTION_EXIT;
    }


    void DuplexModbusTcpConnection::readReply( std::string& reply, int expectedSize)
    {
        FUNCTION_ENTRY("readReply");
		// TD# 3012
		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}
		
		unsigned int threadId = TA_Base_Core::Thread::getCurrentThreadId();
		boost::shared_ptr<ModbusTcpReplyItem> pItem = removeReplyItem(threadId);
		if (pItem.get() != NULL)
		{
			pItem->getReply(reply);	
		}
		
        FUNCTION_EXIT;
    }
    

    void DuplexModbusTcpConnection::ensureConnected()
    {
        FUNCTION_ENTRY("ensureConnected");

		if ( !m_closed ) return;

		TA_Base_Core::ThreadGuard guard( m_socketStreamLock );

		if ( !m_closed ) return;

        u_short port_number;
        std::stringstream ssPortNumber;
        ssPortNumber << m_service;
        ssPortNumber >> port_number;
        
        ACE_INET_Addr remote_sap( port_number, m_host.c_str() );
        ACE_Time_Value timeout( m_socketTimeout );

        if ( m_sockStream.close() == -1 )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed before connecting to %s:%s",
                m_host.c_str(), m_service.c_str() );
        }
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connecting to %s:%s...", m_host.c_str(), m_service.c_str() );

        int result = m_sockConnector.connect( m_sockStream, remote_sap, &timeout );
        
        if ( 0 != result )
        {
            if ( m_sockStream.close() == -1 )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed while connecting to %s:%s",
                    m_host.c_str(), m_service.c_str() );
            }

            std::ostringstream desc;
            desc << "Failed to connect to " << m_host << ":" << port_number << "; errno: " << errno;
            TA_THROW( TA_Base_Bus::ModbusException( desc.str(), TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connected to %s:%s", m_host.c_str(), m_service.c_str() );
        
		result = m_sockStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
        if (result != 0)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
                m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
        }

		m_closed = false;
		m_connectedSemphore.post();

        FUNCTION_EXIT;
    }


	int DuplexModbusTcpConnection::writeSocketData( const char* buffer, const unsigned int length )
    {
		FUNCTION_ENTRY("write");

        ensureConnected();

		hexDump( "Writing: ", buffer, length );

        unsigned int totalBytesWritten=0;

		while ( totalBytesWritten < length )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a write.", TA_Base_Bus::ModbusException::CONNECTION_CLOSED ) );
            }			

            ACE_Time_Value timeout( m_socketTimeout );
			
			int result = 0;

			{
				TA_Base_Core::ThreadReadGuard guard(m_socketStreamLock); 
				result = m_sockStream.send_n( buffer + totalBytesWritten, length - totalBytesWritten, &timeout );
			}
            
            if (result > 0)
			{
				// something was written update the total bytes written
				totalBytesWritten += result;
			}
			else
            {
				closeSocket();

                if (result < 0)
                {
                    int errorCode = errno;
                    if ( errorCode == ETIME )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Timeout writing to %s:%s",
                            m_host.c_str(), m_service.c_str() );
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to write to %s:%s; Error Code: %d",
                            m_host.c_str(), m_service.c_str(), errorCode);
                    }
                }

                TA_THROW( TA_Base_Bus::ModbusException( "Failed to write to device.", TA_Base_Bus::ModbusException::SOCKET_WRITE_ERROR ) );
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,  "wrote %d of %d bytes", totalBytesWritten, length);
		}

        FUNCTION_EXIT;
        return totalBytesWritten;
    }

	int DuplexModbusTcpConnection::readSocketData( char* buffer, const unsigned int bufferLength )
    {
		FUNCTION_ENTRY("readModbusData");

        unsigned int totalBytesRead = 0;
        size_t bytesRead = 0;
        int result;
        
        while ( totalBytesRead < bufferLength )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a read.", TA_Base_Bus::ModbusException::CONNECTION_CLOSED ) );
            }			          

            ACE_Time_Value timeout( m_socketTimeout );

			{
				TA_Base_Core::ThreadReadGuard guard(m_socketStreamLock); 
				result = m_sockStream.recv_n( buffer + totalBytesRead, bufferLength - totalBytesRead, &timeout, &bytesRead );
			}
			
            if ( result > 0 )
            {
                hexDump( "Read: ", buffer, bytesRead );
            }
            else
			{
				closeSocket();
				std::stringstream ss;
				ss << "Modbus socket read error (error code = " << errno << ")";
				TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
			}

			totalBytesRead += bytesRead;

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "read %d of %d bytes", totalBytesRead, bufferLength );
        }

		FUNCTION_EXIT;
        return totalBytesRead;
    }

	void DuplexModbusTcpConnection::run()
	{
		m_running = true;

		m_connectedSemphore.wait();
		
		int noReadDataTimes = 0;
		while ( m_running )
		{
			try
			{
				bool hasReadData = selectSocket();
				if (m_running && hasReadData)
				{
					noReadDataTimes = 0;
					std::string reply;
					int	replyTransactionID = INVALID_TRANSACTION_ID_NO;

					replyTransactionID = readTCPModubusReply(reply);

					bool findReplyItem = false;
					TA_Base_Core::ThreadGuard guard(m_replyMapLock);
					ModbusTcpReplyMap::iterator itMap;
					for (itMap = m_replyMap.begin(); itMap != m_replyMap.end(); ++itMap)
					{
						boost::shared_ptr<ModbusTcpReplyItem>& pItem = itMap->second;
						if (pItem->getId() == replyTransactionID)
						{
							findReplyItem = true;
							pItem->setReply(reply);
							pItem->notifyReply();
							break;
						}
					}

					if ( !findReplyItem )
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "unknown reply transaction id : %d, discard", replyTransactionID);
					}
				}
				else if ( !hasReadData )
				{
					noReadDataTimes++;
					if (noReadDataTimes  > 10)
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "socket no read data");
						noReadDataTimes = 0;
					}
				}
			}
			catch(ModbusException& e)
			{
				ModbusException::EModbusFail exceptionId = e.getFailType();
				if (exceptionId != ModbusException::CONNECTION_CLOSED)
				{
					closeSocket();
				}
				std::string reason = e.what();
				notifyReplyItemsException(exceptionId, reason);
				m_connectedSemphore.wait();
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "read reply thread unknown exception, close connection");
				closeSocket();
				m_connectedSemphore.wait();
			}
		}

		std::string reason("connection closed");
		notifyReplyItemsException(ModbusException::CONNECTION_CLOSED, reason);
	}

	void DuplexModbusTcpConnection::terminate()
	{
		m_running = false;
		m_connectedSemphore.post();
	}

	int DuplexModbusTcpConnection::generateNextTransactiveId()
	{
		TA_Base_Core::ThreadGuard guard(m_transactionIdLock);
		m_transactionIdSeed++;
		m_transactionIdSeed &= MAX_TRANSACTION_ID;
		return m_transactionIdSeed;
	}

	int DuplexModbusTcpConnection::readTCPModubusReply(std::string& reply)
	{
		FUNCTION_ENTRY("readTCPModubusReply");
		// Read the Header.
		char header[ MBAP_HEADER_SIZE ];
		int bytesRead = readSocketData( header, MBAP_HEADER_SIZE );
		if ( bytesRead != MBAP_HEADER_SIZE )
		{
			std::stringstream err;
			err << "Failed to read reply header (" << bytesRead << " bytes were read)";
			TA_THROW( ModbusException( err.str(), ModbusException::REPLY_HEADER_READ_FAILED ) );
		}

		// Interpret the header contents.
		int replyTransactionID = getWord( header[0], header[1] );
		int protocolID = getWord( header[2], header[3] );

		// Note the reply size includes the device address which is part of the
		// Modbus TCP header. Hence the actual reply body size is one less than this.
		WORD replySize = getWord( header[4], header[5] ) - 1;
		int replyDevice = (unsigned char) header[6];

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "reply transactionID = %d", replyTransactionID);

		// Read the Modbus packet body.
		// do this before checking the header, because if we throw
		// an exception we are left with this data in front of any valid data
		// following the next request
		reply.resize( replySize );
		bytesRead = readSocketData( const_cast<char*>( reply.data() ), replySize );

		// Check the header contents are as they should be.
		if ( protocolID != MODBUS_PROTOCOL_ID )
		{
			TA_THROW( ModbusException( "Incorrect Protocol ID in the reply packet.", ModbusException::INVALID_PROTOCOL_ID ) );
		}
		if ( replyDevice != getDeviceAddress() )
		{
			TA_THROW( ModbusException( "Reply packet has an incorrect device address.", ModbusException::INCORRECT_DEVICE_ADDRESS ) );
		}

		if ( (WORD)bytesRead != replySize )
		{
			std::stringstream err;
			err << "Failed to read the complete reply packet (Bytes read = " << bytesRead << ", expected = " << replySize << ")";
			TA_THROW( ModbusException( err.str(), ModbusException::REPLY_PACKET_READ_ERROR ) ); 
		}

		FUNCTION_EXIT;
		return replyTransactionID;
	}

	void DuplexModbusTcpConnection::notifyReplyItemsException(ModbusException::EModbusFail exceptionId, const std::string& reason)
	{
		FUNCTION_ENTRY("notifyReplyItemsException");

		TA_Base_Core::ThreadGuard guard(m_replyMapLock);
		ModbusTcpReplyMap::iterator itMap;
		for (itMap = m_replyMap.begin(); itMap != m_replyMap.end(); ++itMap)
		{
			boost::shared_ptr<ModbusTcpReplyItem>& pItem = itMap->second;
			pItem->setException(exceptionId, reason);
			pItem->notifyReply();
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "notify items exception count:%d-id:%d", m_replyMap.size(), exceptionId);
		
		FUNCTION_EXIT;
	}

	boost::shared_ptr<ModbusTcpReplyItem> DuplexModbusTcpConnection::removeReplyItem( int itemId )
	{
		FUNCTION_ENTRY("removeReplyItem");
		
		boost::shared_ptr<ModbusTcpReplyItem> replyItem;
		TA_Base_Core::ThreadGuard guard(m_replyMapLock);
		ModbusTcpReplyMap::iterator itReply = m_replyMap.find(itemId);
		if (itReply != m_replyMap.end())
		{
			replyItem = itReply->second;
			m_replyMap.erase(itReply);
		}
		
		FUNCTION_EXIT;
		return replyItem;
	}

	bool DuplexModbusTcpConnection::selectSocket()
	{
		FUNCTION_ENTRY("selectSocket");

		int result = 0;

		{	
			TA_Base_Core::ThreadReadGuard guard(m_socketStreamLock); 
			ACE_Handle_Set handleSet;
			handleSet.set_bit(m_sockStream.get_handle());
			int width = (int)handleSet.max_set() + 1;
			result = ACE::select(width, handleSet, &m_socketSelectTimeOut);
		}
		
		if (result > 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		else if (result == 0)
		{
			FUNCTION_EXIT;
			return false;
		}
		else 
		{
			closeSocket();
			std::stringstream ss;
			ss << "Modbus socket select error (error code = " << errno << ")";
			TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_SELECT_ERROR ) );
		}
		FUNCTION_EXIT;
	}
}
