/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/HandoverAlarmConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to the format used for duty handover pop-ups.
 */

#if !defined(HANDOVERALARMCONVERTER_H_INCLUDED)
#define HANDOVERALARMCONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class HandoverAlarmConverter : public TA_Base_Bus::IDutyStateConverter
	{
	public:

		typedef std::vector<TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup> PopupData;

		HandoverAlarmConverter();
		~HandoverAlarmConverter();

		std::vector<TA_Base_Core::DescriptionParameters*>& buildDeniedAlarmDesc();
		std::vector<TA_Base_Core::DescriptionParameters*>& buildTimedOutAlarmDesc();

		
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
		HandoverAlarmConverter( const HandoverAlarmConverter& );
		HandoverAlarmConverter& operator =( const HandoverAlarmConverter& );

		static const std::string SRC_OP;
		static const std::string SRC_PR;
		static const std::string DST_OP;
		static const std::string DST_PR;
		static const std::string REGIONS;

		typedef std::map<TA_Base_Bus::DutyPrimitive, std::set<TA_Base_Bus::DASType::DbKey> > Recalcitrants;
		
		void mapInsert( Recalcitrants& recals, const TA_Base_Bus::DutyPrimitive& session );

		void deleteDesc( std::vector<TA_Base_Core::DescriptionParameters*>& desc );

		void buildDescVector( Recalcitrants& recals, std::vector<TA_Base_Core::DescriptionParameters*>& descVector );

		Recalcitrants m_denied;
		Recalcitrants m_timedOut;

		TA_Base_Bus::DASType::DbKey m_region;
		TA_Base_Bus::DutyPrimitive m_affectedSession;

		std::set<TA_Base_Bus::DASType::DbKey> m_emptyRegSet;

		std::vector<TA_Base_Core::DescriptionParameters*> m_deniedDesc;
		std::vector<TA_Base_Core::DescriptionParameters*> m_timedOutDesc;
	};
}

#endif // !defined(HANDOVERALARMCONVERTER_H_INCLUDED)
