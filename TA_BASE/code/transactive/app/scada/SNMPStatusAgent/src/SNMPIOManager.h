
#ifndef SNMP_IOMANAGER_H 
#define SNMP_IOMANAGER_H

#include "bus/scada/io_manager/src/IOManager.h"

namespace TA_IRS_App
{
	class SNMPIOManager : public TA_Base_Bus::IOManager
	{
		public:
			SNMPIOManager();
			virtual ~SNMPIOManager();
			TA_Base_Bus::IProtocolManager* createProtocolManager(std::string entityType);		
	};
}

#endif