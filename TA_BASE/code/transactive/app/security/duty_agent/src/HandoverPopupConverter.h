/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/HandoverPopupConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to the format used for duty handover pop-ups.
 */

#if !defined(HANDOVERPOPUPCONVERTER_H_INCLUDED)
#define HANDOVERPOPUPCONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class HandoverPopupConverter : public TA_Base_Bus::IDutyStateConverter
	{
	public:

		typedef std::vector<TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup> PopupData;

		HandoverPopupConverter();

		void buildPopupData(PopupData& popupData) const;
		
		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const TA_Base_Bus::DASType::DbKey& region);

		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const TA_Base_Bus::DASType::DbKey& subsystem,
			const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState );

		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void regionRemove( const TA_Base_Bus::DutyPrimitive& session );
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd    ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemRemove ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemDeny   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemTimeout( const TA_Base_Bus::DutyPrimitive& session );

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:

		typedef std::map<TA_Base_Bus::DutyPrimitive, TA_Base_Bus::DASType::SubsystemMap > DutyTree;
		typedef std::map<TA_Base_Bus::DutyPrimitive, DutyTree > InternalPopupMap;

		void buildCorbaStruct( const DutyTree& internalData,
			TA_Base_Bus::DutyAgentTypeCorbaDef::DutyTreeSequence& corbaData ) const;

		std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> mapInsertReg( InternalPopupMap& popupMap,
			const TA_Base_Bus::DutyPrimitive& keySession, const TA_Base_Bus::DutyPrimitive& valSession );

		void mapInsertSub( InternalPopupMap& popupMap, const TA_Base_Bus::DutyPrimitive& keySession,
			const TA_Base_Bus::DutyPrimitive& valSession );
		

		TA_Base_Bus::DASType::DbKey        m_region;
		TA_Base_Bus::DASType::DbKey        m_subsystem;
		TA_Base_Bus::SubsystemState::State m_remState;
		
		InternalPopupMap    m_gainedMap;
		InternalPopupMap    m_lostMap;
		InternalPopupMap    m_deniedMap;

		TA_Base_Bus::DASType::DutySet m_addedSessions;

		DutyTree                      m_emptyDutyTree;
		TA_Base_Bus::DASType::SubsystemMap m_emptySubMap;
		TA_Base_Bus::DASType::SubsystemSet m_emptySubSet;

		TA_Base_Bus::DutyPrimitive m_sessionToAdd;

		bool m_subAdded;
		bool m_subRemoved;
		bool m_regAdded;
		bool m_regRemoved;

		bool isSessionActive(std::string session); //TD15566
		bool m_sessionToAddIsActive; //TD15566
	};
}

#endif // !defined(HANDOVERPOPUPCONVERTER_H_INCLUDED)
