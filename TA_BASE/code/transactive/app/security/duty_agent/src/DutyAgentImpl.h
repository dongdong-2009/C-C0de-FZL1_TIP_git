#ifndef DUTYAGENTIMPL_H_INCLUDED
#define DUTYAGENTIMPL_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAgentImpl.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the TA_Base_Bus::IDutyAgentCorbaDef interface.
 */

#include "bus/security/duty_agent/IDL/src/IDutyAgentCorbaDef.h"

#include "app/security/duty_agent/src/DutyManagerUpdateSender.h"
#include "app/security/duty_agent/src/RightsLibraryUpdateSender.h"
#include "app/security/duty_agent/src/PopupMessageSender.h"
#include "app/security/duty_agent/src/DutyAlarmSender.h"
#include "app/security/duty_agent/src/DutyAuditMessageSender.h"
#include "app/security/duty_agent/src/StateUpdateSender.h"
#include "app/security/duty_agent/src/DutyMonitorManager.h"
#include "app/security/duty_agent/src/SubsystemStateUpdateSender.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_App
{
	class IDutyAgentExecutive;
	class IDutyAgentCommand;

    class DutyAgentImpl : public virtual POA_TA_Base_Bus::IDutyAgentCorbaDef,
                          public virtual TA_Base_Core::ServantBase
    {
    public:
        DutyAgentImpl(const std::string& agentName);
	    virtual ~DutyAgentImpl();

		// auth
		virtual void authAllocateDuty  (const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);
		virtual void authReallocateDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& b4Session,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& aftSession);
		virtual void authFreeDuty      (const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);
    
		// auto
		virtual void autoDegradeSubsystems(const char* session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems);
		
		// mgr
		/**************************************************************************
		* This function is to provide anterface for triggering degradation
		* of subsystems at specified location (required under TD 10163, 6986, 8721 & 7277)
		* 
		* @param Session Id: The session id which trigger the degradation
		* @param Subsystems Id: Sequence of subsystems' Id that are to be degraded
		* @param Location key: Location of the subsystems to be degraded
		* 
		*/
		virtual void mgrDegradeSubsystemsAtLocation(const char* session, 
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems, unsigned long locationKey);

	    virtual void mgrTransferRegionDuty(const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& ,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions);

		virtual void mgrTransferSubsystemDuty(const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems);
		virtual void mgrDegradeSubsystemsAtRegions(const char* session, 
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems, 
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions);

		virtual void mgrDelegateSubsystemDuty(const char* session, 
			const ::TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions, 
			const ::TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems) ;

		virtual void mgrTransferAllSubsystemDuty(const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session);

		virtual void mgrDegradeAllSubsystems(const char* session);

		virtual void mgrGetDutyState(const char* session,
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_out subState,
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_out subDuty,
			TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_out regDuty);

		virtual void mgrGetSessionDutyState(const char* session,
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_out subState,
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_out subDuty,
			TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_out regDuty);

		/**************************************************************************
		* This function is to provide the state information of specified/selected
		* subsystems at specified location (required under TD 10163, 6986, 8721 & 7277)
		* 
		* @param Session Id: The session id which trigger the degradation
		* @param Subsystems Id: Sequence of subsystems' Id that are to be searched
		* @param Location key: Location of the subsystems to be searched
		* 
		*/
		virtual TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* mgrGetSubsystemDutyState(const char* session, 
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subKeys, TA_Base_Bus::DutyAgentTypeCorbaDef::Key locationKey);
			
		virtual TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence*
			mgrGetTransferableRegions(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);

		virtual TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableSubsystemSequence*
			mgrGetTransferableSubsystems(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions);

		virtual TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence* regMgrGetRemovableRegions(
			const char* session );
 
		virtual TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence* regMgrGetTransferableRegions(
			const char* session, TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_out uncontrolledSubs );

		virtual void regMgrTransferRegionDuty( const char* user, const char* session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions );

		virtual void regMgrRemoveRegionDuty( const char* user, const char* session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions );

		// peer
		virtual void peerSessionBegin(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);

		virtual void peerSessionOverrideBegin(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);

		virtual void peerSessionOverrideEnd(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);

		virtual void peerSessionEnd(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions );

		virtual void peerGetTransferableRegions(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, bool checkSubs );

		virtual void peerGetTransferableSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions );

		virtual void peerTransferRegions(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const char* user, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions);

		virtual void peerTransferSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session, const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems, const char* action);

		virtual void peerGetState(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState);

		virtual void peerGetSessionState(const char* session,TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState);

		virtual void peerGetSubsystemState(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems);

		virtual void peerDegradeSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const char* session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems);

		virtual void peerGetDuty(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem);

		virtual void peerGetExclusiveDuty(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session);

		// query
	    virtual CORBA::Boolean queryHasDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key& actionGroup);

	    virtual CORBA::Boolean queryHasExclusiveDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_out subsystems);

		// duty request-response
		virtual void respResponse( const char* uniqueId, TA_Base_Bus::IDutyAgentRespCorbaDef::EDutyResponse response );

		// Methods used by the DutyAgentUser
		void setExecutive( IDutyAgentExecutive* exec );

		// The two methods localDegrade and localGetDutyState are called by dutyagentuser. localDegrade
		// is called when dutyagentuser detects that the system has gone into degraded mode.
		void localDegrade();

		// Used for getting bulk state information that is sent from the control
		// to the monitor agent. Note that getting the information goes through
		// the normal execution path because it is executed on the control agent
		// but updates can be applied directly to the localdutypeer when received
		// because the receiver is in monitor mode and will not be processing
		// any other requests. Therefore there will be no threading conflicts.
		void localSendBulkUpdate();

		void localSendBulkUpdateRequest();

		void localStartDutyMonitor();
		void localStopDutyMonitor();

    private:
        DutyAgentImpl(const DutyAgentImpl& );
        DutyAgentImpl& operator=(const DutyAgentImpl& );

		void execute( IDutyAgentCommand& command, bool checkRights = true );

		void sessionBegin( TA_Base_Bus::DutyState& dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void sessionOverrideBegin( TA_Base_Bus::DutyState& dutyState,
			const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void sessionOverrideEnd( TA_Base_Bus::DutyState& dutyState,
			const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void sessionEnd( TA_Base_Bus::DutyState& dutyState,
			const TA_Base_Bus::SessionInfo& session,
			const TA_Base_Bus::DASType::LocationSet& locSet, const TA_Base_Bus::DASType::RegionSet& regions,
			bool checkRights = true );

		void transferRegionDuty( TA_Base_Bus::DutyState& dutyState, const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions, bool checkRights = true );

		void transferSubsystemDuty( TA_Base_Bus::DutyState& dutyState,const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems, bool checkRights = true );

		void degradeSubsystems( TA_Base_Bus::DutyState& dutyState,const char* session,
			const TA_Base_Bus::DASType::SubsystemSet& subsystems, const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void getDutyState( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void getSessionDutyState( TA_Base_Bus::DutyState& dutyState, const char* session, const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void getSubsystemDutyState( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::DASType::SubsystemSet& subsystems,const TA_Base_Bus::DASType::LocationSet& locSet,
			bool checkRights = true );

		void getTransferableRegions( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			const TA_Base_Bus::DASType::LocationSet& locSet, bool checkSubs, bool checkRights = true );

		void getTransferableSubsystems( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions, bool checkRights = true );

		void hasDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef & session,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem,
			bool checkRights = true );

		void hasDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef & session,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem,
			bool checkRights = true );

		void hasExclusiveDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef & session,
			const TA_Base_Bus::DASType::LocationSet& locSet, bool checkRights = true );

		void EnsureInControlState()
		{
			if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
			{
                throw(TA_Base_Core::OperationModeException(RPARAM_MONITOR));
			}
		}

		TA_Base_Bus::DutyPeerTaskManager m_taskManager;
		TA_Base_Bus::RightsLibrary *m_rightsLib;
		unsigned long m_resourceKey;

		IDutyAgentExecutive* m_executive;

		TA_Base_Bus::DASType::LocationSet m_feLocSet;
		TA_Base_Bus::DASType::LocationSet m_localLocSet;

		mutable ACE_RW_Mutex m_executiveLock;

		boost::shared_ptr<DutyManagerUpdateSender>    m_pDutyManagerSender;
		//boost::shared_ptr<RightsLibraryUpdateSender>  m_pRightsLibrarySender;
		boost::shared_ptr<PopupMessageSender>         m_pPopupSender;
		boost::shared_ptr<DutyAlarmSender>            m_pAlarmSender;
		boost::shared_ptr<DutyAuditMessageSender>     m_pAuditSender;
		boost::shared_ptr<StateUpdateSender>          m_pStateUpdateSender;
		boost::shared_ptr<DutyMonitorManager>         m_pDutyMonitorManager;
        boost::shared_ptr<SubsystemStateUpdateSender> m_pSubsystemStateUpdateSender;
    };
}

#endif // DUTYAGENTIMPL_H_INCLUDED
