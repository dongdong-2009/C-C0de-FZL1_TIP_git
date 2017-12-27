#ifndef IEC102_CHANNEL_MANAGER_H
#define IEC102_CHANNEL_MANAGER_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"
#include "bus/scada/102_library/src/Channel.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class ChannelMsg;
	class IEC102FlowController;
	class IEC102Library;
	
	class ChannelManager : public TA_Base_Core::Thread
	{
	public:
		ChannelManager(IEC102Library& iec102Library);
		virtual ~ChannelManager();
		void startProcess();
		void stopProcess();
		void terminate();
		void run();
		
		// Create channel to send and receive data
		bool createChannel(const std::string& address, const std::string& port);
		
		void postChannelMsg(ChannelMsg& msg);
		
		// Pass data to be sent out from controller to channel
		void writeDataToChannel(const std::string& data);
		
		// pass incomingBuffer from channel to controller to process the data
		void processData(std::vector<unsigned char>& buffer);
		
		IChannel* getChannel();
		void setChannel(IChannel* channel);		

		// Get current physical channel status
		bool getChannelStatus();
	private:

		void handleChannelMsg(ChannelMsg& msg);
		bool getNextMsg(ChannelMsg& msg);

	private:
		volatile bool				m_running;
		IChannel*					m_channel;
		std::queue<ChannelMsg>		m_channelMsgs;
		TA_Base_Core::ReEntrantThreadLockable m_msgLock;
		IEC102Library& 				m_iec102Library;
		TA_Base_Core::Semaphore		m_msgSemphore;
	};
}

#endif

