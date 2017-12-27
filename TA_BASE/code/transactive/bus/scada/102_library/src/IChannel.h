#ifndef IEC102_ICHANNEL_H
#define IEC102_ICHANNEL_H

#include "core/sockets/src/ITcpSocketObserver.h"

namespace TA_Base_Bus
{
	enum EChannelMsg
	{
		E_CHANNEL_ESTABLISHED = 0,
		E_CHANNEL_CONNECTION_LOST,
		E_CHANNEL_WRITE_FAILED,
		E_CHANNEL_RECEIVE_DATA
	};

	class ChannelMsg
	{
	public:
		ChannelMsg(EChannelMsg eMsg)
			: m_msgType(eMsg)
		{
		}

		ChannelMsg()
		{
		}

		~ChannelMsg()
		{
		}

		EChannelMsg  m_msgType;

	};
	
	class IChannel : public TA_Base_Core::ITcpSocketObserver
	{
	public:
		virtual ~IChannel() {};

		virtual void startChannel() = 0;

		virtual void stopChannel() = 0;

		virtual void writeDataToChannel(const std::string& data) = 0;

		virtual bool isChannelOk() = 0;
	};
}

#endif

