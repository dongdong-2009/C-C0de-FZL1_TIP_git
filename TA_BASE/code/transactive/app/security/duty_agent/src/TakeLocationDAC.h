#ifndef TAKELOCATIONDAC_H_INCLUDED
#define TAKELOCATIONDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/TakeLocationDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "take(locations)" command.
 */

namespace TA_Base_App
{
    class TakeLocationDAC : public TA_Base_App::IDutyAgentCommand
    {
    public:
	
		TakeLocationDAC( DutyAgentFacade& facade, const char* session,
			const TA_Base_Bus::IDutyAgentCorbaDef::KeySequence& locations );		

		virtual void execute();

	protected:
		virtual std::string getSessionId()   const{ return(m_session); }
		virtual std::string getActionName()  const{ return("Take Location Duty"); }
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_TRANSFER_REGION); }

	private:

		DutyAgentFacade& m_facade;
		const char* m_session;
		const TA_Base_Bus::IDutyAgentCorbaDef::KeySequence& m_locations;
    };
}

#endif // TAKELOCATIONDAC_H_INCLUDED
