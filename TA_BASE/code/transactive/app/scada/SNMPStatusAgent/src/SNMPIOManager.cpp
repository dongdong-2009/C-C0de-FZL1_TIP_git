
#include "SNMPIOManager.h"
#include "bus/scada/SNMP_library/src/SNMPProtocolManager.h"
#include <string>

namespace TA_IRS_App
{
	SNMPIOManager::SNMPIOManager()
	{

	}

	SNMPIOManager::~SNMPIOManager()
	{

	}

	TA_Base_Bus::IProtocolManager* SNMPIOManager::createProtocolManager( std::string entityType )
	{
		TA_Base_Bus::IProtocolManager* snmpProtocol = NULL;
		if (TA_Base_Bus::SNMPProtocolManager::isEntityTypeMatchedStatic(entityType))
		{
			snmpProtocol = new TA_Base_Bus::SNMPProtocolManager();
		}
		return snmpProtocol;
	}
}