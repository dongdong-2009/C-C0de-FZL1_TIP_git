#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif

#include "bus/scada/business_library/src/BusinessFactory.h"
#include "bus/scada/business_library/src/BusinessHeartbeat.h"
#include "bus/scada/business_library/src/BusinessMonitorAndWrite.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{

	Business* BusinessFactory::createBusiness(TA_Base_Core::IScadaBusinessItem* businessEntity)
	{
		TA_ASSERT(businessEntity != NULL, "BI--input null entity data");

		Business * business = NULL;

		TA_Base_Core::EScadaBusinessType type = businessEntity->getType();

		switch(type)
		{
		case TA_Base_Core::SCADA_BUSINESS_TYPE_HEARTBEAT:

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--create Heartbeat");
	
			business = new BusinessHeartbeat(*(businessEntity->getOutputEntities()), businessEntity->getIntervalTime());

			break;

		case TA_Base_Core::SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE:

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--create BusinessMonitorAndWrite");
	
			business = new BusinessMonitorAndWrite(*(businessEntity->getInputEntites()), 
												   *(businessEntity->getOutputEntitiesOrdering()),
												   businessEntity->getLocation(),
												   businessEntity->getSubsystem());
			break;

		default:
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--unknown Business %d", type);
		}

		return business;
	}

}