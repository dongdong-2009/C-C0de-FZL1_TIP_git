#if !defined(_DUTYAGENTACCESSSINGLETON_H__95BF721D_81A1_4C6F_9BC4_0E38191E91FB__INCLUDED_)
#define _DUTYAGENTACCESSSINGLETON_H__95BF721D_81A1_4C6F_9BC4_0E38191E91FB__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyAgentAccessSingleton.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 10:20:13 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Serves as a front end to the Duty Agent's IDutyAgentMgrCorbaDef interface.
  * Handles all Duty Agent exceptions and rethrows as DutyAgentAccessException.
  * To avoid multiple CORBA connections (named objects) being created by
  * Duty Agent clients, this class has been implemented as a singleton.
  *
  */

namespace TA_Base_Bus
{
    class DutyAgentAccessSingleton
    {
    public:
        /**
          * Returns the one and only instance of the DutyAgentAccessSingleton. 
          * Throws a DutyAgentAccessException if the named object's corba name 
          * cannot be initialised due to invalid Duty Agent entity configuration.
          *
          */
        static DutyAgentAccessSingleton& getInstance();

        /**
          * Destructor
          *
          */
        ~DutyAgentAccessSingleton();

        /**
          * Retrieves the list of subsystem and region duties for the specified session.
          * Throws a DutyMonitorFrameworkException if duties cannot be retrieved.
          *
          */ 
        void getDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
            SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties);

		/**
          * Retrieves the list of subsystem and region duties for the specified session.
          * Throws a DutyMonitorFrameworkException if duties cannot be retrieved.
          *
          */ 
        void getSessionDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
            SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties);

        /**
          * Retrieves the list of subsystem's states for the specified session and location
          * Throws a DutyMonitorFrameworkException if duties cannot be retrieved.
          *
          */ 
        SubsystemStateList getSubsystemDutyState(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey);

        /**
          * Retrieves the list of transferable regions for the specified session.
          * Throws a DutyMonitorFrameworkException if transferable regions cannot be retrieved.
          *
          */ 
        TransferableRegionList getTransferableRegions(const SessionInfo& sessionId);

        /**
          * Retrieves the list of transferable regions for the specified session.
          * Also returned is a list of subsystems per region for those regions whose
          * transfer would lead to the corresponding subsystems becoming uncontrolled.
          * Throws a DutyMonitorFrameworkException if transferable regions cannot be retrieved.
          *
          */ 
        void getTransferableRegions(const SessionId& sessionId, 
                                    TransferableRegionList& transferableRegions, 
                                    SubsystemTreeMap& uncontrolledSubsystems);

        /**
          * Retrieves removable regions for the specified session. With each region comes a list 
          * of subsystems that will become uncontrolled if the region is deassigned from the session.
          * Throws a DutyMonitorFrameworkException if removable regions cannot be retrieved.
          *
          */ 
        SubsystemTreeMap getRemovableRegions(const SessionId& sessionId);

        /**
          * Retrieves the list of transferable subsystems in the given regions for the specified session.
          * Throws a DutyMonitorFrameworkException if transferable subsystems cannot be retrieved.
          *
          */ 
        TransferableSubsystemList getTransferableSubsystems(const SessionInfo& sessioInfo, const RegionKeyList& regionKeys);

        /**
          * Assigns the given regions to the specified session.
          * Throws a DutyMonitorFrameworkException if transfer fails.
          *
          */
        void transferRegionDuty(const TA_Base_Bus::SessionInfo& session, const RegionKeyList& regionKeys);

        /**
          * Assigns the given regions to the specified session.
          * Throws a DutyMonitorFrameworkException if transfer fails.
          *
          */
        void transferRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys);

        /** 
          * Deassigns the given regions from the specified session.
          * Throws a DutyMonitorFrameworkException if removal fails.
          *
          */
        void removeRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys);

        /**
          * Assigns the given subsystems to the specified session.
          * Throws a DutyMonitorFrameworkException if transfer fails.
          *
          */
        void transferSubsystemDuty(const TA_Base_Bus::SessionInfo& session, const SubsystemTreeMap& subsystems);

        /**
          * Attempt to assign all subsystems in all regions to the specified session.
          * Throws a DutyMonitorFrameworkException if transfer fails.
          *
          */
		void transferAllSubsystemDuty(const TA_Base_Bus::SessionInfo& session);

        /**
          * Performs system-wide delegation. The current session is specified for security purposes only.
          * Throws a DutyMonitorFrameworkException if the delegation fails.
          *
          */
        void degradeAllSubsystems(const SessionId& sessionId);

        /**
          * Performs system-specific degrade. The current session is specified for security purposes only.
          * Throws a DutyMonitorFrameworkException if the degradation fails.
          *
          */
		void degradeSubsystemsAtLocation(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey);

        /**
          * Performs system-specific degrade. The current session is specified for security purposes only.
          * Throws a DutyMonitorFrameworkException if the degradation fails.
		  * for some duties can be shared in several locations, or location duty
          */		
		void degradeSubsystemsAtRegions(const SessionId& sessionId, const SubsystemKeyList& subsystems,const  RegionKeyList& regions);
    private:
        DutyAgentAccessSingleton();
        DutyAgentAccessSingleton(const DutyAgentAccessSingleton& dutyAgentAccessSingleton);
        DutyAgentAccessSingleton& operator=(const DutyAgentAccessSingleton& dutyAgentAccessSingleton);

	private:
		inline void setQueryTimeout(bool queryFlag = true);

    private:
        static DutyAgentAccessSingleton* s_dutyAgentAccessSingleton;
		static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        // Duty Agent named object
	    TA_Base_Core::NamedObject<IDutyAgentMgrCorbaDef,
			                 IDutyAgentMgrCorbaDef_ptr,
			                 IDutyAgentMgrCorbaDef_var> m_dutyAgent;
		unsigned short m_queryTimeout;
		unsigned short m_actionTimeout;
		bool m_queryTimeOutFlag;
    };

}

#endif // !defined(_DUTYAGENTACCESSSINGLETON_H__95BF721D_81A1_4C6F_9BC4_0E38191E91FB__INCLUDED_)
