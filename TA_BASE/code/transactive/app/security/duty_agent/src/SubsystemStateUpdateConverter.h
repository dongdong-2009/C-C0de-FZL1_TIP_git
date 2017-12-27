/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateUpdateConverter.h $
 * @author:   Derrick Liew
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a sequence of Subsystem State updates..
 */

#if !defined(SUBSYSTEMSTATEUPDATECONVERTER_H_INCLUDED)
#define SUBSYSTEMSTATEUPDATECONVERTER_H_INCLUDED

namespace TA_Base_App
{
    class SubsystemStateUpdateConverter : public TA_Base_Bus::IDutyStateConverter
    {
    public:

        SubsystemStateUpdateConverter();
        ~SubsystemStateUpdateConverter();

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
         * getRegionKey
         *
         * Returns the region the state updates are related to.
         *
         * @return DASType::DbKey The key of the region.
         */
        TA_Base_Bus::DASType::DbKey getRegionKey()
        {
            return m_regionKey;
        }


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
        SubsystemStateUpdateConverter( const SubsystemStateUpdateConverter& );
        const SubsystemStateUpdateConverter& operator=( const SubsystemStateUpdateConverter& );

        // The list used to store subsystem state information.
        std::list<TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState> m_subsystemStates;

        // This variable tracks the key of the current region.
        TA_Base_Bus::DASType::DbKey m_regionKey;

		//TD15417++
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState m_setSubsystemState;
		bool m_bSubSystemSet;
		//++TD15417
    };
}

#endif // !defined(SUBSYSTEMSTATEUPDATECONVERTER_H_INCLUDED)
