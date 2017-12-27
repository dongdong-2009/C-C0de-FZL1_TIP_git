#include "core/configuration_updates/src/EObjectType.h"

namespace TA_Base_Core
{
	std::string getDescFromEObjectType(EObjectType objectType)
	{
        switch(objectType)
		{
			
		case ALARM_PLAN_ASSOCIATION:     // Update on AlarmPlanAssocData
			return  "ALARM_PLAN_ASSOCIATION";
		case ALARM_SEVERITY:             // Update on AlarmSeverityData
			return "ALARM_SEVERITY";
		case ALARM_TYPE:                 // Update on AlarmData
			return "ALARM_TYPE";
		case APPLICATION:                // Update on Gui data
			return "APPLICATION";
		case EVENT_TYPE:                 // Update on Event data
			return "EVNET_TYPE";
		case INTERLOCKING:               // Update on Interlock data
			return "INTERLOCKING";
		case LOCATION:                   // Update on Location data
			return "LOCATION";
		case SUBSYSTEM:                  // Update on Subsystem data
			return "SUBSYSTEM";
		case PROFILE:                    // Update on Profile data
			return "PROFILE";
		case ENTITY:                     // Update on an Entity User data (This will only be generated after Entity Owner data has been processed.  Shouldn't need to refresh data)
			return "ENTITY";
		case ENTITY_OWNER:               // Update on an Entity Owner data (The owner must refresh data)
			return "ENTITY_OWNER";
		case OPERATOR:                   // Update on Operator data
			return "OPERATOR";
		case REGION:                     // Update on Region data
			return "REGION";
		case SECURITY:                   // Update on Security data
			return "SECURITY";
		case MESSAGE_TYPE:               // Update on MessageTypeData
			return "MESSAGE_TYPE";
		case SCADA_DISPLAY:              // Update on ScadaDisplay data
			return "SCADA_DISPLAY";
		case ACTION_GROUP:               // Update on Action Group data
			return "ACTION_GROUP";
		case SYSTEM_CONTROLLER:          // Update on System Controller data
			return "SYSTEM_CONTROLLER";
		case SYSTEM_CONTROLLER_GROUP:    // Update on System Controller Group data
			return "SYSTEM_CONTROLLER_GROUP";
		case ALARM_RULE:                 // Update on Alarm rule data
			return "ALARM_RULE";
		case PA_ZONE:                    // Update on PA Zone
			return "PA_ZONE";
		case PA_ZONE_GROUP:              // Update on PA Zone Group
			return "PA_ZONE_GROUP";
		case PA_DVA_MESSAGE:             // Update on PA Dva Message
			return "PA_DVA_MESSAGE";
		case PA_TRAIN_DVA_MESSAGE:       // Update on PA Train Dva Message
			return "PA_TRAIN_DVA_MESSAGE";
		case PA_DVA_VERSION:             // Update on PA Dva Version
			return "PA_DVA_VERSION";
		case PA_ATS_TRIGGERED_BROADCAST: // Update on PA ATS Triggered Broadcast
			return "PA_ATS_TRIGGERED_BROADCAST";
		case PA_STATION:                 // Update on PA Station
			return "PA_STATION";
		case MMS_SCHEDULE:               // Update on MM_SCHEDULING table
			return "MMS_SCHEDULE";
		case VI_STATION:                 // Update on CCTV to Transactive location mapping
			return "VI_STATION";
		case VI_TRIGGERING_EVENTS:       // Update on CCTV triggered events.
			return "VI_TRIGGERING_EVENTS";
			// 7196-Start
			// Added for ECS config Plugins update and gloabal parameter updates
		case ECS_MASTER_MODE:			// Update on Master Mode data 
			return "ECS_MASTER_MODE";
		case ECS_STATION_MODE:			// Updates on ECS Station Mode data
			return "ECS_STATION_MODE";
		case ECS_ZONE:					// Updates on ECS Zone data
			return "ECS_ZONE";
		case GLOBAL_PARAMETERS:			// Updates on Global Parameters
			return "GLOBAL_PARAMETERS";
			// 7196-End
		case STIS_PID_GROUP:             // Update on saved STIS PID groups
			return "STIS_PID_GROUP";
		case EVENT_PLAN_ASSOCIATION:      // Updates on EventPlanAssocData
			return "EVENT_PLAN_ASSOCIATION";
		case INCIDENT_RIGHTS:		    // Updates on Incident Rights Access // TD15203
			return "INCIDENT_RIGHTS";
		case OPC_TAG_ALIAS:               //updates on the OPC Tag Alias //TD15114    
			return "OPC_TAG_ALIAS";
		case SOUND_FILE:					// updates on the Sound File TD#15687 
			return "SOUND_FILE";
		}
		return "NoObjectType";
	}
}