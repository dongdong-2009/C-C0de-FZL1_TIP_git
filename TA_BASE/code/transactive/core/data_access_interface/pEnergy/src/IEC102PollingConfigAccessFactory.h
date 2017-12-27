#ifndef IEC_102_POLLINGCONFIG_ACCESS_FACTORY_INCLUDED
#define IEC_102_POLLINGCONFIG_ACCESS_FACTORY_INCLUDED

#include "IEC102PollingConfig.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include <vector>
#include <sys/timeb.h>

namespace TA_Base_Core
{
	/* This database access class is for PEnergy using (IEC102 protocal) */
	class PollingConfigAccessFactory
	{
	private:
		PollingConfigAccessFactory();  // Becase we are using singelton instance
		
		static TA_Base_Core::NonReEntrantThreadLockable m_nonReEntreanLock;
		static PollingConfigAccessFactory* m_self; //It will store our singelton instance pointer
	
	public:
		virtual ~PollingConfigAccessFactory(void){}
		
		static PollingConfigAccessFactory& getInstance(); // get class singleton object
		
		std::vector< IEC102PollingConfig* > getStartTime(unsigned long equipmentKey); // return data based on equipment entity key

		std::vector< IEC102PollingConfig* > getAllStartTime(); //return all the data
		
		void setStartTime(unsigned long equipmentKey, unsigned long pollingTypeKey, time_t input_time); //based on polling type pkey and equipment entity key to update start time
	};
}	// namespace TA_Base_Core


#endif // #ifndef IEC_102_POLLINGCONFIG_ACCESS_FACTORY_INCLUDED