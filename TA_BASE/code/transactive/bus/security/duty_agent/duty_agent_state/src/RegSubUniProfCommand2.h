/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/RegSubUniProfCommand2.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/10/31 12:12:40 $
 * Last modified by:  $Author: grace.koh $
 *
 * Region or Subsystem commands that operatate on a specific profile associated with a session.
 *
 */

#ifndef RegSubUniProfCommand2_H_INCLUDED
#define RegSubUniProfCommand2_H_INCLUDED

namespace TA_Base_Bus
{
	template< class RegSub >
    class RegSubUniProfCommand2 : public IRegSubCommand<RegSub>
    {
    public:
		RegSubUniProfCommand2( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&,bool apply),
			DutyState& state, const DutyPrimitive& session, bool apply );

		virtual void execute(RegSub& regSub, bool doApply);

	private:
		void (RegSub::*m_stateCmd)(DutyState&, const DutyPrimitive&,bool apply);
		DutyState& m_state;
		const DutyPrimitive& m_session;
		bool m_apply;
    };

	template < class RegSub >
	RegSubUniProfCommand2<RegSub>::RegSubUniProfCommand2( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&,bool apply),
		DutyState& state, const DutyPrimitive& session,bool apply ) :
		m_stateCmd(stateCmd),
		m_state(state),
		m_session(session),
		m_apply(apply)
	{
	}

	template < class RegSub >
	void RegSubUniProfCommand2<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		(regSub.*m_stateCmd)( m_state, m_session,m_apply);

// 		if( doApply )
// 		{
// 			regSub.apply(m_state);
// 		}
	}
}

#endif // RegSubUniProfCommand2_H_INCLUDED
