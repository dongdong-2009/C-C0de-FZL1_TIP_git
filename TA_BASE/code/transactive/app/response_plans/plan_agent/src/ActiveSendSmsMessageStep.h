#if defined ( STEPTYPE_SEND_SMS_MESSAGE )
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bradley Cleaver
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is responsible for sending an SMS message as part of
  * a plans execution.
  *
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

#ifndef ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED
#define ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED


namespace TA_Base_App
{
	//Class declaration
	class ActiveSendSmsMessageStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveSendSmsMessageStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::SendSmsMessageParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveSendSmsMessageStep& operator=( const ActiveSendSmsMessageStep& );
		ActiveSendSmsMessageStep( const ActiveSendSmsMessageStep& );

		const TA_Base_Core::SendSmsMessageParameters m_params;
	}; // ActiveSendSmsMessageStep
}; // TA_Base_App

#endif // ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_SEND_SMS_MESSAGE
