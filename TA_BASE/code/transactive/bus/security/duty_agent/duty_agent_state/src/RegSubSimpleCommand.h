/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/RegSubSimpleCommand.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Region or Subsystem commands that take no parameters.
 *
 */

#ifndef REGSUBSIMPLECOMMAND_H_INCLUDED
#define REGSUBSIMPLECOMMAND_H_INCLUDED

namespace TA_Base_Bus
{
	template< class RegSub >
    class RegSubSimpleCommand : public IRegSubCommand<RegSub>
    {
    public:

		RegSubSimpleCommand( void (RegSub::*simpleCmd)(DutyState&), DutyState& state );

		virtual void execute(RegSub& subsystem, bool doApply);

	private:
		void (RegSub::*m_simpleCmd)(DutyState&);
		DutyState& m_state;
    };

	template< class RegSub >
	RegSubSimpleCommand<RegSub>::RegSubSimpleCommand( void (RegSub::*simpleCmd)(DutyState&), DutyState& state ) :
		m_simpleCmd(simpleCmd),
		m_state(state)
	{
	}
	
	template< class RegSub >
	void RegSubSimpleCommand<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		(regSub.*m_simpleCmd)(m_state);

// 		if( doApply )
// 		{
// 			regSub.apply(m_state);
// 		}
	}
}

#endif // REGSUBSIMPLECOMMAND_H_INCLUDED
