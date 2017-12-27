/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/DuplexModbusTcpConnection.h $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A TCP/IP connection to a Modbus device.
  *
  * 
  *
  */

#ifndef DuplexModbusTcpConnection_h
#define DuplexModbusTcpConnection_h

#include <string>
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "boost/shared_ptr.hpp"
#include "bus/modbus_comms/src/ModbusConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
	class ModbusTcpReplyItem
	{
	public:
		ModbusTcpReplyItem(int transactiveId);
		~ModbusTcpReplyItem();
		int		getId();
		void	setException(ModbusException::EModbusFail exceptionId, const std::string& reason);
		void	setReply(const std::string& reply);
		void	getReply(std::string& reply);
		bool    waitForResponse(unsigned long millSec);
		void    notifyReply();
	
	private:
		void    throwModbusException();
		
	private:
		volatile bool			m_getReplyed;
		int						m_transactiveId;
		std::string				m_reply;
		TA_Base_Core::Condition m_condition;
		ModbusException::EModbusFail m_exceptionId;
		std::string				m_reason;
	};

	typedef std::map<unsigned int, boost::shared_ptr<ModbusTcpReplyItem> > ModbusTcpReplyMap;

	class DuplexModbusTcpConnection : public ModbusConnection, public TA_Base_Core::Thread
    {

    public:
        /**
          * ModbusTcpConnection
          *
          * Constructor.
          *
          * @param host The name or IP address of the Modbus device to be
          * communicated to via a TCP/IP connection.
          */
        DuplexModbusTcpConnection( const std::string& host,
                             const std::string& service,
                             long socketTimeout = 1000,
                             bool tcpNoDelay = false);

        /**
          * ~ModbusTcpConnection
          *
          * Destructor.
          */
        virtual ~DuplexModbusTcpConnection();

        /**
          * disable
          *
          * Disables Modbus communications via the object. The current connection
          * to the Modbus device is disconnected. Any subsequent communication method
          * calls on this object will fail (until it is enabled again). The disable
          * method is useful in redundancy scenarios where a IModbus instance is
          * being used as a "hot standby" but should not take up any Modbus device
          * connection resources.
          */
        virtual void disable();

		/**
          * isConnected
          *
          * Checks the device connection and returns true if the device is connected
		  *
          * @return			True if the device is connected, false otherwise
          */
		virtual bool isConnected();

		 /**
          * ensureConnected
          *
          * Connects to the Modbus device if a connection is not current.
          *
          * @exception TcpSocketException Thrown if a TCP/IP connection problem is
          * encountered.
          */
        void ensureConnected();

    protected:
        /**
          * closeSocket
          *
          * Closes the socket connection if open and destroys the socket object.
          * This method does not change the enable status.
          */
        void closeSocket();

        /**
          * writeRequest
          *
          * Writes Modbus request data to the communication connection.
          *
		  * @return				The transaction id of this MODBUS request
          * @param request      The data buffer defining the request.
          */
        virtual void writeRequest( const std::string& request );

        /**
          * readReply
          *
          * Reads the reply to a Modbus request.
          *
          * @param reply        A buffer to store the reply data in. Note that the reply
          *                     data does not include any transport layer specific header
          *                     or footer data.
		  * @param expectedSize The expected number of bytes in the reply packet assuming
		  *                     the reply isn't an exception/error response.
          */
        virtual void readReply( std::string& reply, int expectedSize);


        /**
          * writeSocketData
          *
          * Wraps the TcpSocket write function with some exception
          * processing code.
          *
          * @param buffer   The buffer to be written to the socket.
          * @param length   The length of the buffer to be written.
          * @return         The number of characters actually written.
          */
		int writeSocketData( const char* buffer, const unsigned int length );

		/**
          * readSocketData
          *
          * Wraps the TcpSocket read function with some exception
          * processing code.
          *
          * @param buffer       The buffer to be read from the socket.
          * @param bytesRead    The actual number of bytes read.
          * @param length       The maximum allowed length of the read buffer returned.
          * @return             The number of characters actually read.
          */
		int readSocketData( char* buffer, const unsigned int bufferLength );

		void run();

		void terminate();

    private:
		/**
          * selectSocket
          *
          * detect socket whether has data to read 
          * @exception TcpSocketException Thrown if a TCP/IP connection problem is
          * @return  if true means socket has data to read, otherwise hasn't.
          */
		bool selectSocket();

		/**
          * readTCPModubusReply
          *
          * read one complete frame of TCP modbus response data from socket which
		  * send by modbus sever
          * 
		  * @param reply   get response string
		  * @exception TcpSocketException Thrown if a TCP/IP connection problem is
          * @return transaction id of this response
          */
		int readTCPModubusReply(std::string& reply);
		
		/**
          * notifyReplyItemsException
          *
          * notify exception to all response items 
          * 
		  * @param exceptionId   exception id
		  * @param reason		 exception description
          * 
          */
		void notifyReplyItemsException(ModbusException::EModbusFail exceptionId, const std::string& reason);

		/**
          * generateNextTransactiveId
          *
          * generate next transaction id
          * 
		  * @return transaction id
          * 
          */
		int generateNextTransactiveId();
		
		boost::shared_ptr<ModbusTcpReplyItem> removeReplyItem(int itemId);

		DuplexModbusTcpConnection( const DuplexModbusTcpConnection& );
        DuplexModbusTcpConnection& operator=( const DuplexModbusTcpConnection& );
		
    protected:
        int                                     m_transactionIdSeed;
        int                                     m_tcpNoDelay;
		volatile bool							m_running;
		volatile bool                           m_closed;
        std::string                             m_host;
        std::string                             m_service;
		unsigned long							m_timeOut;
		ACE_Time_Value							m_socketSelectTimeOut;
        ACE_Time_Value                          m_socketTimeout;
        ACE_SOCK_Connector                      m_sockConnector;
        ACE_SOCK_Stream                         m_sockStream;
        TA_Base_Core::ReadWriteThreadLockable   m_socketStreamLock;
		TA_Base_Core::ReEntrantThreadLockable   m_requestWriteLock;
		TA_Base_Core::ReEntrantThreadLockable	m_transactionIdLock;
		ModbusTcpReplyMap						m_replyMap;
		TA_Base_Core::ReEntrantThreadLockable	m_replyMapLock;
		TA_Base_Core::Semaphore					m_connectedSemphore;

        /** The transaction ID in the modbus header is sent as
          * 16 bits, this is used to make sure we dont have a 
          * transaction ID greater than this.
          */
        static const int MAX_TRANSACTION_ID;
        static const int MODBUS_PROTOCOL_ID;
        static const int MBAP_HEADER_SIZE;
        static const int ERROR_LENGTH;
		static const int INVALID_TRANSACTION_ID_NO;
		static const unsigned long SELECT_SOCKET_TIME_OUT;
     };
}

#endif // ModbusTcpConnection_h
