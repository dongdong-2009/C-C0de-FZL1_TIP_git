#if !defined(XACMLCONTEXTFACTORY_H_INCLUDED)
#define XACMLCONTEXTFACTORY_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/xacml_factory/src/XacmlContextFactory.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 */

namespace TA_Base_Bus
{
	class XacmlContextFactory
	{
	public:
		enum StationDutyStatus
		{
			UnKnown = 0,
			OnLine = 1,
			OffLine = 2,
		};

	public:
		XacmlContextFactory();
		~XacmlContextFactory();
		std::auto_ptr<Context> build( const TA_Base_Bus::SessionInfo& sessionId,
			unsigned long subject, unsigned long resource, unsigned long action );

		std::auto_ptr<Context> build( const TA_Base_Bus::SessionInfo& sessionId,
			unsigned long subject, unsigned long location, unsigned long subsystem, unsigned long action );

		std::auto_ptr<Context> buildWoDuty( const TA_Base_Bus::SessionInfo & session,
			unsigned long subject, unsigned long resource, unsigned long action );	

		void updateStationStatus(unsigned long locationKey, StationDutyStatus status);
		
	private:

		XacmlContextFactory( const XacmlContextFactory& x );
		XacmlContextFactory& operator=( const XacmlContextFactory& x );

		void XacmlContextFactory::initCache();

		void getActionAttributes( std::auto_ptr<Context>& ctx, unsigned long action );

		void getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx,
			unsigned long subject, unsigned long resource );

		void getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx,
			unsigned long subject, unsigned long location, unsigned long subsystem );

		void getSubjectAttributesFromDutyAgent( std::auto_ptr<Context>& ctx,
			const TA_Base_Bus::SessionInfo& session, unsigned long subject,
			unsigned long resource );

		void getSubjectAttributesFromDutyAgent( std::auto_ptr<Context>& ctx,
			const TA_Base_Bus::SessionInfo& session, unsigned long subject,
			unsigned long location, unsigned long subsystem );

		bool isPhysicalResource( unsigned long resource );
		bool isPhysicalSubsystem( unsigned long subsystem);
		bool isDisplayOnly(const TA_Base_Bus::SessionInfo& session);
		
		

		//bool isPhysical( unsigned long resource );
		
		void XacmlContextFactory::getLocSub( unsigned long resource,
			unsigned long& location, unsigned long& subsystem );

		void getSubsystem( TA_Base_Core::IEntityData* resource, unsigned long& subsystem, bool& isPhysical ) const;

		std::string XacmlContextFactory::getProfileNameFromCache( unsigned long subject );

		std::string XacmlContextFactory::getActionGroupNameFromCache( unsigned long dutyType );

		RightsType::ProfileMap     m_profileMap;
		RightsType::ActionMap      m_actionMap;
		RightsType::ResourceMap    m_resourceMap;
		RightsType::SubsystemMap   m_subsystemMap;
		RightsType::ActionGroupMap m_actionGroupMap;
		RightsType::LocSubDutyMap  m_locSubDutyMap;

		// TODO: Due to caching, only the write lock is being used. Maybe
		// replace these with simple Mutex locks (except for locSubDuty).
		ACE_RW_Mutex m_profileLock;
		ACE_RW_Mutex m_actionLock;
		ACE_RW_Mutex m_resourceLock;
		ACE_RW_Mutex m_subsystemLock;
		ACE_RW_Mutex m_actionGroupLock;
		ACE_RW_Mutex m_locSubDutyLock;
		
		ContextOnlineUpdatable      m_onlineUpdater;
		ContextSubDutyMsgSubscriber m_dutyMsgSubscriber;

		typedef TA_Base_Core::NamedObject<IDutyAgentQueryCorbaDef,
				IDutyAgentQueryCorbaDef_ptr,
				IDutyAgentQueryCorbaDef_var> DutyAgentQueryType;		

		typedef std::map<unsigned long,boost::shared_ptr<DutyAgentQueryType> > DutyAgentMap;
		DutyAgentMap m_dutyagentMap;

		typedef std::map<unsigned long, StationDutyStatus> DutyAgentStatusMap;
		DutyAgentStatusMap m_dutyAgentStatusMap;
		ACE_RW_Mutex m_statusMapLock;
		
		unsigned long m_stationDeployKey;

		static const std::string INVALID_RIGHTS;
		static const std::string INVALID_DUTY;

	};
}

#endif // !defined(XACMLCONTEXTFACTORY_H_INCLUDED)
