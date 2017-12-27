#ifndef IEC102_CONTROLLER_172_H
#define IEC102_CONTROLLER_172_H

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
	class IEC102Protocol172;
	class IEC102Controller172 : public TA_Base_Bus::IEC102FlowController
	{
		public:
		IEC102Controller172(ChannelManager& channelManager,IEC102ConfigData& objConfigData,unsigned int startAddr, unsigned int endAddr, time_t startTime, time_t endTime, unsigned char rad);
		
		virtual ~IEC102Controller172();
		
		// process the working  flow
		virtual bool processFlow();
		
		// set the informatioin for the next flow 
		virtual void setFlowInfo();	

		// Check whehter the next flow exist.
		virtual bool getNextFlow();	


		void getClassIData();
		void call172Data();
	private:
		time_t 						m_startTime;
		time_t 						m_endTime;
		unsigned short				m_startAddr;
		unsigned short				m_endAddr;	
		unsigned char 				m_rad;	

		//Used for Virtual Address in the message.
		unsigned short				m_currentStartAddr;
		unsigned short				m_currentEndAddr;	
		bool 						m_bNextRoundFlow;
		IEC102Protocol172* 			m_protocol172;
				
	};
}

#endif
