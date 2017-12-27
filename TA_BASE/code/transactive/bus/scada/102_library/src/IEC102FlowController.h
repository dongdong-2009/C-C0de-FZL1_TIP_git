#ifndef IEC102_BASE_CONTROLLER_H
#define IEC102_BASE_CONTROLLER_H

#include "bus/scada/102_library/src/IEC102DataStruct.h"
#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "bus/scada/102_library/src/ChannelManager.h"
#include "boost/shared_ptr.hpp"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class ChannelManager;
	class IEC102ConfigData;
	
	class IEC102FlowController
	{
	public:

		virtual ~IEC102FlowController();
		
		// Update replyState
		virtual void updateLinkControlState(EFlowControlState newState);
		
		// process the working  flow
		virtual bool processFlow() = 0;	

		// set the informatioin for the next flow 
		virtual void setFlowInfo() = 0;	

		// Check whehter the next flow exist.
		virtual bool getNextFlow() = 0;			

		// Pass incomingBuffer from ChannelManager to protocol to extract the data
		void processData(std::vector<unsigned char>& buffer);
		
		// Update replyState
		void updateFlowControlState(EFlowControlState newState);

		// Set equipment and agent variable
		void setupConfiguration(const IEC102ConfigData& objConfigData );

		unsigned char getHighRemoteAddress();
		unsigned char getLowRemoteAddress();

		void addEnergyData(const std::vector<PowerEnergyData>& energyData);
		std::vector<PowerEnergyData> getEnergyData() {return m_energyData;}
		void clearEnergyData() {m_energyData.clear();}
	private:
		/**
         * operator= - Hidden as not used
         */
        IEC102FlowController& operator=(const IEC102FlowController& rhs);

        /**
         * Copy constructor - Hidden as not useds
         */
        IEC102FlowController(const IEC102FlowController&);


	protected:
		IEC102FlowController(ChannelManager &channelManager,IEC102ConfigData& objConfigData);
		bool sendAndWait(std::string& sendData,EFlowControlState& replyState);
		
		void setStateObserver(ControlStateObserver* pObserver);
		void removeStateObserver();

		TA_Base_Core::ReEntrantThreadLockable 	m_stateObserverLock;
		ChannelManager& 			m_channelManager;

		int							m_sendRetryTime;
		unsigned long				m_messageResponseTime;
		unsigned short 				m_remoteAddress;

		FrameEnergyData				m_FrameData;	

		E102FlowStep				m_nextStep;
		bool						m_bFlowRunning;
		IEC102Protocol*				m_102Protocol;
		ControlStateObserver*  		m_stateObserver;

		unsigned char				m_lowRemoteAddress;
		unsigned char				m_highRemoteAddress;
		
		std::vector<PowerEnergyData> m_energyData;
	};
}

#endif

