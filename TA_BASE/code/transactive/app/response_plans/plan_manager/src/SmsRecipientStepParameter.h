/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/SmsRecipientStepParameter.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class stores the recipient identifier parameter information.
  *
  */

#if !defined(AFX_SMSRECIPIENTSTEPPARAMETER_H__DD31034B_0B41_4B40_B0B3_116AA2F177BA__INCLUDED_)
#define AFX_SMSRECIPIENTSTEPPARAMETER_H__DD31034B_0B41_4B40_B0B3_116AA2F177BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include <string>
#include "PlanStepParameter.h"

namespace TA_Base_App
{
    class SmsRecipientStepParameter : public PlanStepParameter
    {
    public:
        SmsRecipientStepParameter( PlanStep& parent, std::string name, std::string recipientIdentifier );
        SmsRecipientStepParameter( PlanStep& parent, const SmsRecipientStepParameter& parameter);
        SmsRecipientStepParameter( const SmsRecipientStepParameter& parameter );
        virtual ~SmsRecipientStepParameter();

        /**
          * addToParameterList
          * ------------------
          * Adds the parameter information to the plan step parameter list.
          *
          * @param listCtrl The list to add the parameter information to.
          */
        virtual void addToParameterList(CPlanStepParameterListCtrl &listCtrl);

        /**
          * setRecipientIdentifier
          * ----------------------
          * Sets the identifier stored in the object.
          *
          * @param recipientIdentifer The new recipient identifier.
          */
        void setRecipientIdentifier( const std::string& recipientIdentifier );

        /**
          * getRecipientIdentifier
          * ----------------------
          * Returns the identifier stored in the object.
          *
          * @return std::string The recipient identifier.
          */
        std::string getRecipientIdentifier() const { return m_recipientIdentifier; };

    protected:
        /**
          * getEditDialog
          * -------------
          * Returns the dialog associated with this parameter. This is required
          * when inheriting from PlanStepParameter, but is not actually used
          * for this parameter.
          *
          * @param canEdit Specifies whether the parameters should be editable.
          *
          * @return CPlanStepParameterEditDlg* The dialog object.
          */
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        SmsRecipientStepParameter& operator=( const SmsRecipientStepParameter& );

        // The string that stores the recipient identifier.
        std::string m_recipientIdentifier;
    };
}

#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

#endif // !defined(AFX_SMSRECIPIENTSTEPPARAMETER_H__DD31034B_0B41_4B40_B0B3_116AA2F177BA__INCLUDED_)
