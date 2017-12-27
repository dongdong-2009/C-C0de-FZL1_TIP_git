/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditSendSmsMessagePage.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is responsible for allowing the user to specify the
  * configuration required for the Send SMS Message plan step.
  *
  */

#if !defined(AFX_CPLANSTEPEDITSENDSMSMESSAGEPAGE_H__70F94939_7054_4B1F_9CE5_BF354F5C0F5E__INCLUDED_)
#define AFX_CPLANSTEPEDITSENDSMSMESSAGEPAGE_H__70F94939_7054_4B1F_9CE5_BF354F5C0F5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_SEND_SMS_MESSAGE)

#include "PlanStepEditParameterPage.h"
#include "SmsRecipientStepParameter.h"
#include "TextualStepParameter.h"

#include "app/messaging/sms_agent/IDL/src/ISmsAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"


/////////////////////////////////////////////////////////////////////////////
// PlanStepEditSendSmsMessagePage dialog

namespace TA_Base_App
{
    class SendSmsMessageStep;

    class CPlanStepEditSendSmsMessagePage : public CPlanStepEditParameterPage
    {
           DECLARE_DYNAMIC(CPlanStepEditSendSmsMessagePage)

    // Construction
    public:
        CPlanStepEditSendSmsMessagePage( SendSmsMessageStep* step );

        ~CPlanStepEditSendSmsMessagePage()
        {
        }

    // Operations
    protected:
        /**
          * EnableParameterControls
          * -----------------------
          * Used to enable or disable all of the controls.
          *
          * @param enable Specify whether the controls are enabled or disabled.
          */
        virtual void EnableParameterControls(bool enable);

        /**
          * DisplayParameters
          * -----------------
          * Used to tell the dialog to update the data displayed to the user.
          */
        virtual void DisplayParameters();

        /**
          * UpdateParameters
          * ----------------
          * Used to tell the dialog to save its data back into the parameter
          * objects.
          */
        virtual void UpdateParameters();

    private:
        /**
          * GetRecipientsList
          * -----------------
          * Retrieves the list of recipients from the SMS Agent.
          */
        void GetRecipientsList();

        /**
          * GetGroupsList
          * -------------
          * Retrieves the list of groups from the SMS Agent.
          */
        void GetGroupsList();

        /**
          * GetMaxMsgLength
          * ---------------
          * Retrieves the maximum message length from the SMS Agent.
          */
        void GetMaxMsgLength();

        /**
          * UpdateCharCount
          * ---------------
          * Updates the text in the frame to reflect the number of
          * remaining characters.
          */
        void UpdateCharCount();

        // Counters.
        unsigned int m_numRecipients;
        unsigned int m_numGroups;
        unsigned int m_maxMsgLength;
        unsigned int m_charsRemaining;

        // Parameters.
        SmsRecipientStepParameter m_recipientIdentifierParameter;
        TextualStepParameter m_textMessageParameter;

        // The SMS Agent.
/*
        TA_Base_Core::NamedObject<
            ISmsAgentCorbaDef,
            ISmsAgentCorbaDef_ptr,
            ISmsAgentCorbaDef_var> m_smsAgent;
*/
    // Dialog Data
        // {{AFX_DATA(CPlanStepEditSendSmsMessagePage)
        enum { IDD = IDD_PLAN_STEP_EDIT_SEND_SMS_MESSAGE_DLG };
        CButton    m_bCharCount;
        CEdit    m_cMessage;
        CButton    m_bRadioRecipient;
        CButton m_bRadioGroup;
        CButton m_bRadioUnlisted;
        CEdit    m_cRecipientPhone;
        CComboBox    m_cGroupsList;
        CComboBox    m_cRecipientsList;
        CStatic    m_sListName;
        CString    m_sRecipientPhone;
        CString    m_sMessage;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditSendSmsMessagePage)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditSendSmsMessagePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnRadioRecipient();
        afx_msg void OnRadioGroup();
        afx_msg void OnRadioUnlisted();
        afx_msg void OnRecipientSelChange();
        afx_msg void OnGroupSelChange();
        afx_msg void OnUnlistedChange();
        afx_msg void OnMessageChange();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // defined (STEPTYPE_SEND_SMS_MESSAGE)

#endif // !defined(AFX_PLANSTEPEDITSENDSMSMESSAGEPAGE_H__70F94939_7054_4B1F_9CE5_BF354F5C0F5E__INCLUDED_)
