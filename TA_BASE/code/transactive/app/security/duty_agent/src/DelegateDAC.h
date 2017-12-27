#ifndef DELEGATEDAC_H_INCLUDED
#define DELEGATEDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DelegateDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "delegate" command.
 */

namespace TA_Base_App
{
    class DelegateDAC : public TA_Base_App::IDutyAgentCommand
    {
    public:
	
		DelegateDAC( DutyAgentFacade& facade, const char* session );		

		virtual void execute();

	protected:

		virtual std::string getSessionId()  const{ return(m_session); }
		virtual std::string getActionName() const{ return("System-Wide Delegation"); }
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_DEGRADE_SUBSYSTEMS); }
		
	private:

		DutyAgentFacade& m_facade;
		const char* m_session;
    };
}

#endif // DELEGATEDAC_H_INCLUDED
