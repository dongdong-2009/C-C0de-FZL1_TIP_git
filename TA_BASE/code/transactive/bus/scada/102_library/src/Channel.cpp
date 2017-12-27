#include "bus/scada/102_library/src/Channel.h"
#include "bus/scada/102_library/src/ChannelManager.h"
#include "core/utilities/src/DebugUtil.h"

#include <iomanip>				
using TA_Base_Core::DebugUtil;


namespace TA_Base_Bus
{
	Channel::Channel(const std::string& address, const std::string& port, ChannelManager& manager)
		:
	m_established(false),
	m_host(address),
	m_port(port),
	m_channelManager(manager),
	m_tcpSocket(NULL)
	{
		FUNCTION_ENTRY("Channel()");
		FUNCTION_EXIT;
	}

	Channel::~Channel()
	{
		FUNCTION_ENTRY("~Channel()");

		if (m_tcpSocket != NULL)
		{
			m_tcpSocket->removeObserver(*this);
			m_tcpSocket->terminateAndWait();
			delete m_tcpSocket;
			m_tcpSocket = NULL;
		}
	
		FUNCTION_EXIT;
	}

	void Channel::writeDataToChannel( const std::string& data )
	{
		FUNCTION_ENTRY("writeDataToChannel()");
		if (m_tcpSocket != NULL)
		{
			m_tcpSocket->write(data.c_str(), data.length());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send: %s",(TA_Base_Bus::HexHelper::hexToStr(data)).c_str());
		}
		FUNCTION_EXIT;
	}

	void Channel::processReceivedData( std::vector<unsigned char> & data, TA_Base_Core::ISocket * pSocket )
	{
		FUNCTION_ENTRY("processReceivedData()");
		std::vector<unsigned char> revBuffer(data);
		data.clear();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Recv: %s",(TA_Base_Bus::HexHelper::hexToStr(revBuffer)).c_str());

		m_channelManager.processData(revBuffer);
		FUNCTION_EXIT;
	}

	void Channel::writeFailed()												//gets called by underlying socket
	{
		FUNCTION_ENTRY("writeFailed()");
		//m_established = false;
		ChannelMsg msg(E_CHANNEL_WRITE_FAILED);
		m_channelManager.postChannelMsg(msg);
		FUNCTION_EXIT;
	}

	bool Channel::connectionLost()										//gets called by underlying socket
	{
		FUNCTION_ENTRY("connectionLost()");
		if (m_established)
		{
			m_established = false;
			ChannelMsg msg(E_CHANNEL_CONNECTION_LOST);
			m_channelManager.postChannelMsg(msg);
			//m_channelManager.handleConnectionLost();
		}
		FUNCTION_EXIT;
		return true;
	}

	void Channel::connectionEstablished()							//gets called when connection to the RTU terminal has been established
	{
		FUNCTION_ENTRY("connectionEstablished()");
		m_established = true;
		ChannelMsg msg(E_CHANNEL_ESTABLISHED);
		m_channelManager.postChannelMsg(msg);
		FUNCTION_EXIT;
	}

	void Channel::startChannel()
	{
		FUNCTION_ENTRY("startChannel()");
		if (NULL == m_tcpSocket)
		{
			m_tcpSocket = new TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>(m_host, m_port);
			m_tcpSocket->addObserver(*this);
			m_tcpSocket->start();
		}
		FUNCTION_EXIT;
	}

	void Channel::stopChannel()
	{
		FUNCTION_ENTRY("stopChannel()");
		if (m_tcpSocket != NULL)
		{
			m_tcpSocket->removeObserver(*this);
			m_tcpSocket->terminateAndWait();
			delete m_tcpSocket;
			m_tcpSocket = NULL;
		}
		FUNCTION_EXIT;
	}
	
	bool Channel::isChannelOk()
	{
		FUNCTION_ENTRY("isChannelOk()");
		FUNCTION_EXIT;
		return m_established;
	}
}

