/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/EObjectType.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/24 13:41:41 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This file defines the type of objects that could register for 
  * online updates
  */

#if !defined(EObjectType_2C6640C3_F540_47a1_89D3_8BF6D1528F38__INCLUDED_)
#define EObjectType_2C6640C3_F540_47a1_89D3_8BF6D1528F38__INCLUDED_

#include <string>

namespace TA_Base_Core
{
    enum EObjectType
    {
        ALARM_PLAN_ASSOCIATION,     // Update on AlarmPlanAssocData
        ALARM_SEVERITY,             // Update on AlarmSeverityData
        ALARM_TYPE,                 // Update on AlarmData
        APPLICATION,                // Update on Gui data
        EVENT_TYPE,                 // Update on Event data
        INTERLOCKING,               // Update on Interlock data
        LOCATION,                   // Update on Location data
        SUBSYSTEM,                  // Update on Subsystem data
        PROFILE,                    // Update on Profile data
        ENTITY,                     // Update on an Entity User data (This will only be generated after Entity Owner data has been processed.  Shouldn't need to refresh data)
        ENTITY_OWNER,               // Update on an Entity Owner data (The owner must refresh data)
        OPERATOR,                   // Update on Operator data
        REGION,                     // Update on Region data
        SECURITY,                   // Update on Security data
        MESSAGE_TYPE,               // Update on MessageTypeData
        SCADA_DISPLAY,              // Update on ScadaDisplay data
        ACTION_GROUP,               // Update on Action Group data
        SYSTEM_CONTROLLER,          // Update on System Controller data
        SYSTEM_CONTROLLER_GROUP,    // Update on System Controller Group data
        ALARM_RULE,                 // Update on Alarm rule data
        PA_ZONE,                    // Update on PA Zone
        PA_ZONE_GROUP,              // Update on PA Zone Group
        PA_DVA_MESSAGE,             // Update on PA Dva Message
        PA_TRAIN_DVA_MESSAGE,       // Update on PA Train Dva Message
        PA_DVA_VERSION,             // Update on PA Dva Version
        PA_ATS_TRIGGERED_BROADCAST, // Update on PA ATS Triggered Broadcast
        PA_FAS_TRIGGERED_BROADCAST, // Update on PA FAS Triggered Broadcast
        PA_STATION,                 // Update on PA Station
        MMS_SCHEDULE,               // Update on MM_SCHEDULING table
        VI_STATION,                 // Update on CCTV to Transactive location mapping
        VI_TRIGGERING_EVENTS,       // Update on CCTV triggered events.
		// 7196-Start
		// Added for ECS config Plugins update and gloabal parameter updates
		ECS_MASTER_MODE,			// Update on Master Mode data 
		ECS_STATION_MODE,			// Updates on ECS Station Mode data
		ECS_ZONE,					// Updates on ECS Zone data
		GLOBAL_PARAMETERS,			// Updates on Global Parameters
		// 7196-End
        STIS_PID_GROUP,             // Update on saved STIS PID groups
        EVENT_PLAN_ASSOCIATION,      // Updates on EventPlanAssocData
    	INCIDENT_RIGHTS,		    // Updates on Incident Rights Access // TD15203
        OPC_TAG_ALIAS,               //updates on the OPC Tag Alias //TD15114    
		SOUND_FILE					// updates on the Sound File TD#15687 
    };

	std::string getDescFromEObjectType(EObjectType objectType);

}

#endif // !defined(EObjectType_2C6640C3_F540_47a1_89D3_8BF6D1528F38__INCLUDED_)
