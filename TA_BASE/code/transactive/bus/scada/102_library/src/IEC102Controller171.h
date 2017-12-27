#ifndef IEC102_CONTROLLER_171_H
#define IEC102_CONTROLLER_171_H

#include "bus/scada/102_library/src/IEC102DataStruct.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "boost/shared_ptr.hpp"
#include "core/threads/src/Thread.h"
#include <time.h>

namespace TA_Base_Bus
{
	class IEC102Protocol171;
	class IEC102Controller171 : public TA_Base_Bus::IEC102FlowController
	{
		public:
		IEC102Controller171(ChannelManager& channelManager,IEC102ConfigData& objConfigData,unsigned short startAddr, unsigned short endAddr);
		
		virtual ~IEC102Controller171();
		
		// process the working  flow
		virtual bool processFlow();
		
		// set the informatioin for the next flow 
		virtual void setFlowInfo();	

		// Check whehter the next flow exist.
		virtual bool getNextFlow();	

		void getClassIData();
		void call171Data();

	private:
		IEC102Protocol171* 			m_protocol171;
		unsigned short				m_startAddr;
		unsigned short				m_endAddr;	

		//Used for Virtual Address in the message.
		unsigned short				m_currentStartAddr;
		unsigned short				m_currentEndAddr;	
		bool 						m_bNextRoundFlow;		
	};
}

#endif
