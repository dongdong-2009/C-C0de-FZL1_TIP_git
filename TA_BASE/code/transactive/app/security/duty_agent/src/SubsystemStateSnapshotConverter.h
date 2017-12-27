/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateSnapshotConverter.h $
 * @author:   Bradley Cleaver
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a sequence of subsystem states.
 */

#if !defined(_SUBSYSTEM_STATE_SNAPSHOT_CONVERTER_H_)
#define _SUBSYSTEM_STATE_SNAPSHOT_CONVERTER_H_

#include <list>

namespace TA_Base_App
{
    class SubsystemStateSnapshotConverter : public TA_Base_Bus::IDutyStateConverter
    {
    public:

        SubsystemStateSnapshotConverter();
        SubsystemStateSnapshotConverter( const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems );
        ~SubsystemStateSnapshotConverter();

        // Methods inherited from IDutyStateConverter
        virtual void setSession( const std::string& session );
        virtual void setAction( const std::string& action );

        virtual void setRegionCount( unsigned long count );
        virtual void setRegion( const TA_Base_Bus::DASType::DbKey& region );
        virtual void regionComplete();

        virtual void setSubsystemCount( unsigned long count );
        virtual void setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
            const TA_Base_Bus::SubsystemState::State& addState,
            const TA_Base_Bus::SubsystemState::State& remState );
        virtual void subsystemComplete();

        virtual void setRegionAddCount( unsigned long count );
        virtual void setRegionRemoveCount( unsigned long count );

        virtual void regionAdd( const TA_Base_Bus::DutyPrimitive& session );
        virtual void regionRemove( const TA_Base_Bus::DutyPrimitive& session );

        virtual void setSubsystemAddCount( unsigned long count );
        virtual void setSubsystemRemoveCount( unsigned long count );

        virtual void subsystemAdd( const TA_Base_Bus::DutyPrimitive& session );
        virtual void subsystemRemove( const TA_Base_Bus::DutyPrimitive& session );
        virtual void subsystemDeny( const TA_Base_Bus::DutyPrimitive& session );
        virtual void subsystemTimeout( const TA_Base_Bus::DutyPrimitive& session );

        /**
         * getSubsystemStates
         *
         * Returns a sequence of subsystem states.
         * NOTE: It is the responsibility of the caller to free the memory allocated
         *       to the returned object.
         *
         * @return SubsystemStateSequence The sequence of duty states.
         */
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* getSubsystemStates();


    private:
        // Hidden methods.
        SubsystemStateSnapshotConverter( const SubsystemStateSnapshotConverter& );
        const SubsystemStateSnapshotConverter& operator=( const SubsystemStateSnapshotConverter& );

        /**
         * isSubsystemOfInterest
         *
         * Determines whether the specified subsystem is a subsystem of interest.
         *
         * @param subsystemKey The key of the subsystem.
         *
         * @return bool True if the subsystem is of interest.
         */
        bool isSubsystemOfInterest( const TA_Base_Bus::DASType::DbKey& subsystem );

        // The sequence of subsystems to retrieve the state information for.
        TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence m_subsystemsOfInterest;

        // The list used to store subsystem state information.
        std::list<TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState> m_subsystemStates;

        // This variable tracks the key of the current region.
        TA_Base_Bus::DASType::DbKey m_regionKey;
    };
}

#endif // !defined(_SUBSYSTEM_STATE_SNAPSHOT_CONVERTER_H_)
