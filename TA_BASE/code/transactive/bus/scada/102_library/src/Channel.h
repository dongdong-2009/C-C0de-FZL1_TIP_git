#ifndef IEC102_CHANNEL_H
#define IEC102_CHANNEL_H

#include "bus/scada/102_library/src/IChannel.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ISocket.h"
#include "core/sockets/src/TcpSocket.h"
#include "bus/scada/102_library/src/HexHelper.h"
namespace TA_Base_Bus
{
	class ChannelManager;

	class Channel : public TA_Base_Bus::IChannel
	{
	public:
		Channel(const std::string& address, const std::string& port, ChannelManager& manager);
		
		virtual ~Channel();

		virtual void startChannel();

		virtual void stopChannel();

		/**
 		* processReceivedData
 		*
 		* This method is called whenever data is received from the socket.
 		* Typically the implementation of this method will determine whether
 		* a complete message is stored in the received data, and remove it if
 		* there is.  The remaining data is left in the buffer.
 		*
 		* @param std::string & data - received data
 		* @param ISocket * pSocket   - the socket that received the data
 		*
 		* @returns nothing
 		*
 		*/
		virtual void processReceivedData( std::vector<unsigned char> & data,  TA_Base_Core::ISocket * pSocket = NULL );
		/**
 		* writeFailed
 		*
 		* This method is called whenever writing to the socket fails.
 		*
 		* @returns nothing
 		*/
		virtual void writeFailed();

		/**
 		* connectionLost
 		*
 		* This method is called whenever the observed socket loses the 
 		* connection to the remote socket.
 		*
 		* @returns false - socket should no longer be observed
 		* @returns true  - socket should continue to be observed
 		*/
		virtual bool connectionLost();

		/**
 		* connectionEstablished
 		*
 		* This method is called whenever the observed socket establishes a
 		* connection to the remote socket.  It is used in conjunction with
 		* the connectionLost method in order to handle socket reconnection
 		* at runtime.
 		*
 		* @returns nothing
 		*/
		virtual void connectionEstablished();

		virtual void writeDataToChannel(const std::string& data);

		virtual bool isChannelOk();

	private:
		bool						  m_established;
		std::string					  m_host;
		std::string					  m_port;
		ChannelManager&				  m_channelManager;
		TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket> * m_tcpSocket;
	};
}

#endif

