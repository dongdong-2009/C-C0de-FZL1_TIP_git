
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/SNMPStatusAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    const std::string SNMPStatusAgentEntityData::ENTITY_TYPE     = "SNMPStatusAgent";
	const std::string SNMPStatusAgentEntityData::SNMP_TRAP_LISTEN_PORT     = "SNMPTrapListenPort";

	SNMPStatusAgentEntityData::SNMPStatusAgentEntityData()
    {
    }

	
    SNMPStatusAgentEntityData::SNMPStatusAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    SNMPStatusAgentEntityData::~SNMPStatusAgentEntityData()
    {
    }

	std::string SNMPStatusAgentEntityData::getType()
	{
		return getStaticType();
	}

    std::string SNMPStatusAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

          
    IEntityData* SNMPStatusAgentEntityData::clone ( unsigned long key )
    {
        return new SNMPStatusAgentEntityData ( key );
    }

    void SNMPStatusAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

	// SNMPStatusAgentEntityData 
	int SNMPStatusAgentEntityData::getSNMPTrapListenPort()
	{
		int data = 162;

		std::string stringValue = getHelper()->getParameter(SNMP_TRAP_LISTEN_PORT);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}

} //close namespace TA_Base_Core
