#if !defined(_DMFDEFS_H__C0113529_4DA5_4BB0_9BFE_CE78A489C631__INCLUDED_)
#define _DMFDEFS_H__C0113529_4DA5_4BB0_9BFE_CE78A489C631__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DMFTypeDefs.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Common type definitions.
  *
  */

namespace TA_Base_Bus
{
    typedef std::string SessionId;
    typedef unsigned long SessionKey;
    typedef unsigned long DataItemKey;
    typedef DataItemKey OperatorKey; 
    typedef DataItemKey ProfileKey; 
    typedef DataItemKey ActionGroupKey;
    typedef DataItemKey LocationKey; 
    typedef DataItemKey RegionKey; 
    typedef DataItemKey SubsystemKey;
    typedef DataItemKey WorkstationKey;

    typedef std::vector<SessionId> SessionIdList;
    typedef std::vector<OperatorKey> OperatorKeyList;
    typedef std::vector<ProfileKey> ProfileKeyList;
    typedef std::vector<ActionGroupKey> ActionGroupKeyList;
    typedef std::vector<LocationKey> LocationKeyList;
    typedef std::vector<RegionKey> RegionKeyList;
    typedef std::vector<SubsystemKey> SubsystemKeyList;
    typedef std::map<RegionKey, SubsystemKeyList> SubsystemTreeMap;
	typedef std::vector<DutyAgentTypeCorbaDef::SubsystemState> SubsystemStateList;
    typedef std::vector<DutyAgentTypeCorbaDef::RegionDuty> RegionDutyList;
    typedef std::vector<DutyAgentTypeCorbaDef::SubsystemDuty> SubsystemDutyList;
    typedef std::vector<DutyAgentTypeCorbaDef::TransferableRegion> TransferableRegionList;
    typedef std::vector<DutyAgentTypeCorbaDef::TransferableSubsystem> TransferableSubsystemList;

	//jinhua++
	typedef struct 
	{
		RegionKey regionKey;
		SubsystemKey subsystemKey;
	} RegionSubsystemKey;

    typedef struct
    {
        TA_Base_Core::EModificationType updateType;
        TA_Base_Core::EObjectType itemType;
        DataItemKey itemKey;
    } DataUpdate;
           
    typedef struct
    {
        TA_Security::EUpdateType updateType;
        SessionId sessionId;
        SessionKey sessionKey;
    } SessionUpdate;

    typedef enum { DUT_ADD, DUT_REMOVE, DUT_STATE, DUT_UNDEFINED, DUT_CHANGE } EDutyUpdateTupe;
    template <typename DutyType> struct DutyUpdate
    {
        EDutyUpdateTupe updateType;
        std::vector<DutyType> duties;
    };

    typedef DutyUpdate<DutyAgentTypeCorbaDef::RegionDuty> RegionDutyUpdate;

    // TD12449: We now need to notify observers of changes in subsystem state so the generic DutyUpdate
    //          template can no longer be used.
    typedef struct
    {
        EDutyUpdateTupe updateType;
        SubsystemStateList states;
        SubsystemDutyList duties;
    } SubsystemDutyUpdate;

    struct SessionInfoEx : public SessionInfo
    {
        SessionKey sessionKey;
    };

    // TD12449: This enumeration of subsystem states should be the same as that defined
    //          inside the duty agent so when the state number is typecast back to the
    //          enumerated type its value is consistant.
    typedef enum { E_SS_NORMAL, E_SS_DELEGATED, E_SS_DEGRADED } ESubsystemState;
}

#endif // !defined(_DMFDEFS_H__C0113529_4DA5_4BB0_9BFE_CE78A489C631__INCLUDED_)
