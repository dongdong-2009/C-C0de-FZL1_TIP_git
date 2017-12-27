/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyStateConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * An implementation of the Builder pattern. This allows the duty state cache to
 * be converted into various formats to be retured to clients of the duty agent.
 * 
 * The appropriate converter is passed in when the state is requested.
 * 
 * This Builder pattern was used to allow the Region Manager (not yet defined) to have
 * state returned to it in a convenient format.
 */

#if !defined(EA_AF7486C6_05D4_453c_9383_DCC06DADB868__INCLUDED_)
#define EA_AF7486C6_05D4_453c_9383_DCC06DADB868__INCLUDED_

namespace TA_Base_Bus
{
	class IDutyStateConverter
	{
	public:

		virtual ~IDutyStateConverter(){}

		virtual void setSession( const std::string& session ) = 0;

        // Note that the action is passed as a std::string to decouple TA_Base_Bus from the
        // set of possible actions (which is determined by applications).  This is quite ugly
        // and it would seem better to use a polymorphic object to represent the action if we
        // really need this decoupling, or to use an enum if we don't.
		virtual void setAction ( const std::string& action  ) = 0;

		virtual void setRegionCount   ( unsigned long count ) = 0;
		virtual void setRegion        ( const DASType::DbKey& region   ) = 0;
		virtual void regionComplete   () = 0;
		virtual void setSubsystemCount( unsigned long count ) = 0;
		virtual void setSubsystem     ( const DASType::DbKey& subsytem, const SubsystemState::State& addState,
			const SubsystemState::State& remState ) = 0;
		virtual void subsystemComplete() = 0;

		virtual void setRegionAddCount   ( unsigned long count ) = 0;
		virtual void setRegionRemoveCount( unsigned long count ) = 0;

		virtual void regionAdd    ( const DutyPrimitive& session ) = 0;
		virtual void regionRemove ( const DutyPrimitive& session ) = 0;

		virtual void setSubsystemAddCount    ( unsigned long count ) = 0;
		virtual void setSubsystemRemoveCount ( unsigned long count ) = 0;

		virtual void subsystemAdd    ( const DutyPrimitive& session ) = 0;
		virtual void subsystemRemove ( const DutyPrimitive& session ) = 0;
		virtual void subsystemDeny   ( const DutyPrimitive& session ) = 0;
		virtual void subsystemTimeout( const DutyPrimitive& session ) = 0;
	};
}






#endif // !defined(EA_AF7486C6_05D4_453c_9383_DCC06DADB868__INCLUDED_)
