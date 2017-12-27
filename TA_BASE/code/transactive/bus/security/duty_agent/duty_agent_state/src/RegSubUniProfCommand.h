/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/RegSubUniProfCommand.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Region or Subsystem commands that operatate on a specific profile associated with a session.
 *
 */

#ifndef REGSUBUNIPROFCOMMAND_H_INCLUDED
#define REGSUBUNIPROFCOMMAND_H_INCLUDED

namespace TA_Base_Bus
{
	template< class RegSub >
    class RegSubUniProfCommand : public IRegSubCommand<RegSub>
    {
    public:
		RegSubUniProfCommand( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
			DutyState& state, const DutyPrimitive& session );

		virtual void execute(RegSub& regSub, bool doApply);

	private:
		void (RegSub::*m_stateCmd)(DutyState&, const DutyPrimitive&);
		DutyState& m_state;
		const DutyPrimitive& m_session;
    };

	template < class RegSub >
	RegSubUniProfCommand<RegSub>::RegSubUniProfCommand( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
		DutyState& state, const DutyPrimitive& session ) :
		m_stateCmd(stateCmd),
		m_state(state),
		m_session(session)
	{
	}

	template < class RegSub >
	void RegSubUniProfCommand<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		(regSub.*m_stateCmd)( m_state, m_session );

// 		if( doApply )
// 		{
// 			regSub.apply(m_state);
// 		}
	}
}

#endif // REGSUBUNIPROFCOMMAND_H_INCLUDED
