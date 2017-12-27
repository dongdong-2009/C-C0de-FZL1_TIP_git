#ifndef BUSSINESS_MANAGER_H
#define BUSSINESS_MANAGER_H

#include <vector>
#include "bus/scada/business_library/src/BusinessFactory.h"
#include "bus/scada/business_library/src/Business.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class BusinessManager
	{
	public:
		static BusinessManager& getInstance();
		static void removeInstance();
		void setToMonitor();
		void setToControl();
		void hostStart();
		void hostStop();
		void addBusiness(Business* business);
	protected:
		BusinessManager();
		virtual ~BusinessManager();
		void createBusiness();
	private:
		std::vector<Business*>					m_businesses;
		static BusinessManager*					m_singleton;
		static TA_Base_Core:: ReEntrantThreadLockable	m_singletonLock; 
		BusinessFactory							m_businessFactory;
		bool									m_isControl;
	};
}

#endif