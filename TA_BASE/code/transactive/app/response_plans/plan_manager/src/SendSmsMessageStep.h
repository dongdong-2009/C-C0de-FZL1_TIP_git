/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/SendSmsMessageStep.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the information required by the Plan Manager
  * for the Send SMS Message plan step.
  *
  */

#if !defined(AFX_SENDSMSMESSAGESTEP_H__5C4E8584_C4B0_465D_93D8_30A725A13D07__INCLUDED_)
#define AFX_SENDSMSMESSAGESTEP_H__5C4E8584_C4B0_465D_93D8_30A725A13D07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include "PlanStep.h"

namespace TA_Base_App
{
    class SmsRecipientStepParameter;
    class TextualStepParameter;

    class SendSmsMessageStep : public PlanStep
    {
    public:
        SendSmsMessageStep( PlanNode* parent, const TA_Base_Core::StepDetail& stepDetail );
        SendSmsMessageStep( PlanNode* parent, PlanStepId stepId );  // For new plan steps
        virtual ~SendSmsMessageStep();

        /**
          * clone
          * -----
          * Returns a copy of the current SendSmsMessageStep object.
          *
          * @return PlanStep* The copy.
          */
        virtual PlanStep* clone();

        /**
          * isStepBranching
          * ---------------
          * Tells the Plan Manager that the step does not branch.
          *
          * @return bool False - This isn't a branching step.
          */
        virtual bool isStepBranching() const
        {
            return false;
        }

        /**
          * canIgnoreFailure
          * ---------------
          * Is it possible to continue executing the plan when the step fails?
          *
          * @return bool True - not a branching step, so yes, failure can be ignored
          */
        virtual bool canIgnoreFailure() const
        {
            return true;
        }

        /**
          * getRecipientIdentifierParameter
          * -------------------------------
          * Returns the parameter representing the recipient identifier.
          *
          * @return SmsRecipientStepParameter* The recipient identifier.
          */
        SmsRecipientStepParameter* getRecipientIdentifierParameter()
        {
            return m_recipientIdentifierParameter;
        }

        /**
          * getTextMessageParameter
          * -----------------------
          * Returns the parameter representing the text message.
          *
          * @return TextualStepParameter* The text message.
          */
        TextualStepParameter* getTextMessageParameter()
        {
            return m_textMessageParameter;
        }

    protected:
        SendSmsMessageStep( const SendSmsMessageStep& );

        /**
          * createPlanStepParameters
          * ------------------------
          * Creates the parameters with default values and adds them to the
          * parameter list.
          *
          * @param stepParameters The list the parameters are added to.
          */
        virtual void createPlanStepParameters( PlanStepParameterList& stepParameters );

        /**
          * createPlanStepParameters
          * ------------------------
          * Creates the parameters with existing values and adds them to the
          * parameter list.
          *
          * @param parameterSet The existing parameter values.
          * @param stepParameters The list the parameters are added to.
          */
        virtual void createPlanStepParameters( const TA_Base_Core::StepParameters& parameterSet,
                                               PlanStepParameterList& stepParameters);

        /**
          * getUpdatedParameters
          * --------------------
          * Updates the step parameters to the current parameters.
          *
          * @param step The step to update.
          */
        virtual void getUpdatedParameters( TA_Base_Core::Step& step );

        /**
          * getParameterEditPage
          * --------------------
          * Returns the dialog that can be used to configure the step
          * parameters.
          *
          * @return CPlanStepEditParameterPage* The dialog object.
          */
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        SendSmsMessageStep& operator=( const SendSmsMessageStep& );

        SmsRecipientStepParameter* m_recipientIdentifierParameter;
        TextualStepParameter* m_textMessageParameter;
    };
}

#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

#endif // !defined(AFX_SENDSMSMESSAGESTEP_H__5C4E8584_C4B0_465D_93D8_30A725A13D07__INCLUDED_)
