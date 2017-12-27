/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ShutdownSystemControllerDlg.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ShutdownSystemControllerDlg is associated with the dialog box 
  * allowing operators to shutdown the System Controller currently
  * logged into.
  *
  */

#pragma warning(disable:4786)

#include <string>

#include "StdAfx.h"
#include "SystemManager.h"
#include "ShutdownSystemControllerDlg.h"
#include "SystemManagerDlg.h"
#include "AdminManager.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"


namespace TA_Base_App
{

    ShutdownSystemControllerDlg::ShutdownSystemControllerDlg(std::string hostname, CWnd* pParent)
    : CDialog(ShutdownSystemControllerDlg::IDD, pParent), m_hostname(hostname)
    {
        //{{AFX_DATA_INIT(ShutdownSystemControllerDlg)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }


    void ShutdownSystemControllerDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(ShutdownSystemControllerDlg)
        DDX_Control(pDX, IDOK, m_shutdownButton);
        DDX_Control(pDX, IDC_HOSTNAME, m_hostnameLabel);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(ShutdownSystemControllerDlg, CDialog)
    //{{AFX_MSG_MAP(ShutdownSystemControllerDlg)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ShutdownSystemControllerDlg message handlers

    BOOL ShutdownSystemControllerDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        m_hostnameLabel.SetWindowText(m_hostname.c_str());
        m_shutdownButton.EnableWindow(TRUE);

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }

    void ShutdownSystemControllerDlg::OnOK()
    {
        const unsigned long entityKey = AdminManager::getInstance().getEntityKey();
        std::string sessionId = AdminManager::getInstance().getSessionId();
        std::string currentUser = AdminManager::getInstance().getOperatorName();

        //
        // Log an event
        //
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Operator %s has requested that the System Controller shut down",
            currentUser.c_str());

        // Only send an audit message if in a valid session.
        if (AdminManager::getInstance().validSession())
        {
            TA_Base_Core::AuditMessageSender* auditSender;
            auditSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::OperatorAudit::Context );
    
            TA_Base_Core::DescriptionParameters desc;
            TA_Base_Core::NameValuePair param1("hostname", m_hostname);
            desc.push_back(&param1);

            auditSender->sendAuditMessage(
                TA_Base_Core::OperatorAudit::SystemControllerShutdown, // Message Type
                entityKey,   // Entity key
                desc,        // Description
                "",          // Additional details
                sessionId,   // SessionID if applicable
                "",          // AlarmID if an alarm associated with event
                "",          // IncidentKey if incident associated with event
                "");         // EventID of a parent event, used to link events

			delete auditSender;
        }

        // Post a message so the SystemManagerDlg shuts down the System Controller.
        CWnd* pWnd = GetParent();
        if (pWnd)
        {
            ::PostMessage(pWnd->m_hWnd, WM_SHUTDOWN_SYSTEM_CONTROLLER, 0, 0);
        }

        CDialog::OnOK();
    }

} // namespace TA_Base_App

