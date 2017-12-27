//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/MessageBox.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_)
#define AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Core
{
    class TransactiveException;
    class RightsException;
    class PlanStepException;
    class PlanStepDetailException;
    class PlanAgentException;
}


namespace TA_Base_App
{
    class PlanNotFoundException;


    /////////////////////////////////////////////////////////////////////////////
    // MessageBox utility

    class MessageBox
    {
    // Operations
    public:
        /**
          * error
          *
          * Displays the error message based on the what's specified in the exception
          *
          * @param actionId The type of action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          */
        static void error(const unsigned long actionId, const TA_Base_Core::TransactiveException& exception);

        /**
          * error
          *
          * Displays the error message based on the what's specified in the exception
          *
          * @param actionId The type of action that is being performed that caused the exception
          *                 to be thrown.
          * @param actionParam Additional parameter to the action
          * @param exception The exception that was thrown
          *
          */
        static void error(const unsigned long actionId, const CString& actionParam, const TA_Base_Core::TransactiveException& exception);

        /**
          * error
          *
          * Displays the generic "Cannot do x because an error has occurred" message.
          *
          * @param actionId The type of action that is being performed
          */
        static void error(const unsigned long actionId);

    private:

        /**
          * displayError
          *
          * Displays the message caused by error. Determines error type from the exception.
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayError(const CString& actionName, const TA_Base_Core::TransactiveException& exception);

        /**
          * displayPermissionError
          *
          * Displays the message caused by permission error
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayPermissionError(const CString& actionName, const TA_Base_Core::RightsException& exception);


        /**
          * displayPlanNotFoundError
          *
          * Displays the message as a result of plan not being found
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayPlanNotFoundError(const CString& actionName, const PlanNotFoundException& exception);


        /**
          * displayPlanStepError
          *
          * Displays the message caused by invalid plan step
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayPlanStepError(const CString& actionName, const TA_Base_Core::PlanStepException& exception);


        /**
          * displayPlanStepDetailError
          *
          * Displays the message caused by step detail error
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayPlanStepDetailError(const CString& actionName, const TA_Base_Core::PlanStepDetailException& exception);


        /**
          * displayPlanAgentError
          *
          * Displays the message caused by a plan agent error that does not fall into above's category
          *
          * @param actionName The action that is being performed that caused the exception
          *                 to be thrown.
          * @param exception The exception that was thrown
          *
          * @return True if it has displayed the appropriate message
          *
          */
        static void displayPlanAgentError(const CString& actionName, const TA_Base_Core::PlanAgentException& exception);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_)


