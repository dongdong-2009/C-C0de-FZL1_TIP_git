/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ChangeOperationModeDlg.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ChangeOperationModeDlg is the class associated with the
  * Change Operation Mode dialog.
  *
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "AdminManager.h"
#include "ChangeOperationModeDlg.h"
#include "SystemManagerDlg.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/SystemControllerException.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

namespace TA_Base_App
{

    ChangeOperationModeDlg::ChangeOperationModeDlg(const std::vector<std::string>& processes, 
                                                    CWnd* parent)
        : CDialog(ChangeOperationModeDlg::IDD, parent), m_processes(processes)
    {
        //{{AFX_DATA_INIT(CStartProcessDlg)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }

    void ChangeOperationModeDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ChangeOperationModeDlg)
	DDX_Control(pDX, IDC_MODE_PROCESS_LIST, m_processList);
	//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(ChangeOperationModeDlg, CDialog)
	    //{{AFX_MSG_MAP(ChangeOperationModeDlg)
	    ON_BN_CLICKED(IDC_CONTROL, OnRadioControl)
	    ON_BN_CLICKED(IDC_MONITOR, OnRadioMonitor)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL ChangeOperationModeDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        // Set the label containing the processes.
        std::string processList;
        for(std::vector<std::string>::iterator iter = m_processes.begin(); 
            iter != m_processes.end(); iter++)
        {
			m_processList.AddString((*iter).c_str());
        }

        CButton RadioButton;
        if (RadioButton.Attach(GetDlgItem(IDC_CONTROL)->m_hWnd))
        {
            RadioButton.SetCheck(1);
            RadioButton.Detach();
            OnRadioControl();
        }
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void ChangeOperationModeDlg::OnOK() 
    {
        const unsigned long entityKey = AdminManager::getInstance().getEntityKey();
        std::string sessionId = AdminManager::getInstance().getSessionId();
        std::string currentUser = AdminManager::getInstance().getOperatorName();

        std::string process;

        for(std::vector<std::string>::iterator iter = m_processes.begin(); 
            iter != m_processes.end(); iter++)
        {
            process = *iter;

            //
            // Log an event.
            //

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Operator %s requested that process ''%s'' have its operation mode changed to %d.",
                currentUser.c_str(), process.c_str(), m_operationMode);

            // Only send an audit message if in a valid session.
            if (AdminManager::getInstance().validSession())
            {
                TA_Base_Core::AuditMessageSender* auditSender;
                auditSender = 
                    TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::OperatorAudit::Context);

                TA_Base_Core::DescriptionParameters desc;
                TA_Base_Core::NameValuePair param1("ProcessId", process.c_str());
                TA_Base_Core::NameValuePair param2("mode", 
                    m_operationMode == TA_Base_Core::Control ? SystemManagerDlg::CONTROL : SystemManagerDlg::MONITOR);

                desc.push_back(&param1);
                desc.push_back(&param2);

                auditSender->sendAuditMessage(
                    TA_Base_Core::OperatorAudit::ApplicationChangeModeRequest, // Message Type
                    entityKey,   // Entity key
                    desc,        // Description
                    "",          // Additional details
                    sessionId,   // SessionID if applicable
                    "",          // AlarmID if an alarm associated with event
                    "",          // IncidentKey if incident associated with event
                    "");         // EventID of a parent event, used to link events

				delete auditSender;
            }

            //
            // Change the operation mode
            //

            try
            {
                AdminManager::getInstance().changeProcessOperationMode(process, m_operationMode);
            }
            catch(TA_Base_Core::SystemControllerException& )
            {
                // UE-020040
                UINT selectedButton = showMsgBox(IDS_UE_020040, process);
            }

        }
	    
	    CDialog::OnOK();
    }

    void ChangeOperationModeDlg::OnRadioControl() 
    {
        m_operationMode = TA_Base_Core::Control;
    }

    void ChangeOperationModeDlg::OnRadioMonitor() 
    {
	    m_operationMode = TA_Base_Core::Monitor;
    }

} // namespace TA_Base_App
