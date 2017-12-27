#ifndef GIVESUBSYSTEMDAC_H_INCLUDED
#define GIVESUBSYSTEMDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GiveSubsystemDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "give(subsystems)" command.
 */

namespace TA_Base_App
{
    class GiveSubsystemDAC : public TA_Base_App::IDutyAgentCommand
    {
    public:
	
		GiveSubsystemDAC( DutyAgentFacade& facade, const char* session,
			TA_Base_Bus::IDutyAgentCorbaDef::Key& location,
			const TA_Base_Bus::IDutyAgentCorbaDef::KeySequence& subsystems );		

		virtual void execute();

	protected:

		virtual std::string getSessionId()  const{ return(m_session); }
		virtual std::string getActionName() const{ return("Give Subsystem Duty"); }
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_TRANSFER_SUBSYSTEM); }

	private:

		DutyAgentFacade& m_facade;
		const char* m_session;
		TA_Base_Bus::IDutyAgentCorbaDef::Key& m_location;
		const TA_Base_Bus::IDutyAgentCorbaDef::KeySequence& m_subsystems;
    };
}

#endif // GIVESUBSYSTEMDAC_H_INCLUDED
