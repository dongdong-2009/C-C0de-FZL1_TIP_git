/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/LocalRunParamDefinitions.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include <string>

//Temporary
#define RPARAM_STANDBY				"Standby"

namespace TA_Base_App
{
	namespace TA_EventAgent
	{
		static const std::string SESSION = "Session";
		static const std::string ENTITY_KEY = "EntityKey";
		static const std::string ENTITY_TYPE_KEY = "EntityTypeKey";
		static const std::string SUBSYSTEM_KEY = "SubsystemKey";
		static const std::string LOCATION_NAME = "LocationName";
		static const std::string ENABLE_FEDERATION = "EnableFederation";
		static const std::string ASSET_NAME = "AssetName";
		static const std::string DATABASE_REFRESH_INTERVAL = "DbRefreshInterval";
		static const std::string EVENT_SUBMIT_INTERVAL = "SubmitInterval";
		static const std::string MAX_CACHE_SIZE = "MaxCacheSize";
		static const std::string AGENT_REFRESH_INTERVAL = "AgentRefreshInterval";
		static const std::string PROPAGATE_BATCH_SIZE = "PropagateBatchSize";
	}
}
