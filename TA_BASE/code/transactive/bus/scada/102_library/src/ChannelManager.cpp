#include "bus/scada/102_library/src/ChannelManager.h"
#include "bus/scada/102_library/src/IEC102Library.h"

namespace TA_Base_Bus
{
	ChannelManager::ChannelManager(IEC102Library& iec102Library)
		:
	m_running(true),
	m_channel(NULL),
	m_iec102Library(iec102Library),
	m_msgSemphore(0)
	{
		FUNCTION_ENTRY("ChannelManager()");
		FUNCTION_EXIT;
	}

	ChannelManager::~ChannelManager()
	{
		FUNCTION_ENTRY("~ChannelManager()");
		terminateAndWait();
		delete m_channel;
		m_channel = NULL;
		FUNCTION_EXIT;
	}

	IChannel* ChannelManager::getChannel()
	{
		return m_channel;
	}

	void ChannelManager::setChannel(IChannel* channel)
	{
		m_channel = channel;
	}

	void ChannelManager::terminate()
	{
		FUNCTION_ENTRY("terminate()");
		m_running = false;
		m_msgSemphore.post();
		FUNCTION_EXIT;
	}

	void ChannelManager::run()
	{
		while(m_running)
		{
			m_iec102Library.UpdateChannelStatus(getChannelStatus());
			ChannelMsg msg;
			if( getNextMsg(msg) )
			{
				handleChannelMsg(msg);
			}
			m_msgSemphore.wait();
		}
	}

	void ChannelManager::startProcess()
	{
		FUNCTION_ENTRY("startProcess()");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start Process of ChannelManager");	
		m_running = true;
		start();
		
		m_channel->startChannel();
		
		FUNCTION_EXIT;
	}

	void ChannelManager::stopProcess()
	{
		FUNCTION_ENTRY("stopProcess()");
		m_channel->stopChannel();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removing channelMsgs");	
		while (!m_channelMsgs.empty()) m_channelMsgs.pop();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Stop Process of ChannelManager");	
		terminateAndWait();

		FUNCTION_EXIT;
	}
	
	void ChannelManager::writeDataToChannel(const std::string& data)
	{
		m_channel->writeDataToChannel(data);
	}

	void ChannelManager::processData(std::vector<unsigned char>& buffer)
	{
		return m_iec102Library.processData(buffer);
	}
	
	bool ChannelManager::createChannel( const std::string& address, const std::string& port )
	{
		FUNCTION_ENTRY("createChannel()");
		
		m_channel = new Channel(address, port, *this);
		if(m_channel == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Create channel with %s:%s failed.", address.c_str(),port.c_str());	
			FUNCTION_EXIT;			
			return false;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create channel with%s:%s successfully.", address.c_str(), port.c_str());
		FUNCTION_EXIT;
		return true;
	}
	
	void ChannelManager::postChannelMsg( ChannelMsg& msg )
	{
		FUNCTION_ENTRY("postChannelMsg()");
		TA_Base_Core::ThreadGuard guard(m_msgLock);
		m_channelMsgs.push(msg);
		m_msgSemphore.post();
		FUNCTION_EXIT;
	}

	void ChannelManager::handleChannelMsg( ChannelMsg& msg )
	{
		FUNCTION_ENTRY("handleChannelMsg()");
		EChannelMsg msgType = msg.m_msgType;

		switch(msgType)
		{
		case E_CHANNEL_ESTABLISHED:
		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection established");
			m_iec102Library.handleConnectionEstablished(); 
			break;
		case E_CHANNEL_CONNECTION_LOST:
		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection lost");
			m_iec102Library.handleConnectionLost();
			break;
		case E_CHANNEL_WRITE_FAILED:
		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Write Failed");
			m_iec102Library.handleWriteFailed();
			break;
		default:
			break;
		}
		FUNCTION_EXIT;
	}
	
	bool ChannelManager::getChannelStatus()
	{
		return m_channel->isChannelOk();
	}

	bool ChannelManager::getNextMsg( ChannelMsg& msg )
	{
		FUNCTION_ENTRY("getNextMsg()");
		bool bGetMsg = false;
		TA_Base_Core::ThreadGuard guard(m_msgLock);
		if (!m_channelMsgs.empty())
		{
			msg = m_channelMsgs.front();
			m_channelMsgs.pop();
			bGetMsg = true;
		}
		FUNCTION_EXIT;
		return bGetMsg;
	}
}

