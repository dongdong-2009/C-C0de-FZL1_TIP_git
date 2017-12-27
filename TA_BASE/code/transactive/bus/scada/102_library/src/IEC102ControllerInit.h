#ifndef IEC102_CONTROLLER_INIT_H
#define IEC102_CONTROLLER_INIT_H

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
	class IEC102ProtocolInit;
	class IEC102ControllerInit : public TA_Base_Bus::IEC102FlowController
	{
	public:
		IEC102ControllerInit(ChannelManager& channelManager,IEC102ConfigData& objConfigData,bool isInitLink);
		
		virtual ~IEC102ControllerInit();

		// process the working  flow
		virtual bool processFlow();
		
		// set the informatioin for the next flow 
		virtual void setFlowInfo();	

		// Check whehter the next flow exist.
		virtual bool getNextFlow();	
		
		void callLinkState();
		void resetLinkUnit();
		void getClassIData();

	private:
		bool m_resetLink;
		bool m_callLink;
		bool m_isInitLink;
		IEC102ProtocolInit* m_protocolInit;
	};
}

#endif
