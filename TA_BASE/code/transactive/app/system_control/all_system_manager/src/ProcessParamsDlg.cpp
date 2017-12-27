/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ProcessParamsDlg.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/09/27 14:15:49 $
  * Last modified by:  $Author: limin.zhu $
  *
  * ProcessParamsDlg is associated with the dialog box allowing operators 
  * to set the processes' runtime parameters. In the current version, it  
  * only allows the debug level of processes to be changed.
  *
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "SystemManager.h"
#include "ProcessParamsDlg.h"
#include "SystemManagerDlg.h"
#include "AdminManager.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/exceptions/src/SystemControllerException.h"

namespace TA_Base_App
{

    ProcessParamsDlg::ProcessParamsDlg(const std::vector<std::string> processes,
                                         CWnd* pParent)
        : CDialog(ProcessParamsDlg::IDD, pParent), m_processes(processes)
    { 
        //{{AFX_DATA_INIT(ProcessParamsDlg)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }

    void ProcessParamsDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(ProcessParamsDlg)
        DDX_Control(pDX, IDC_DEBUG_LEVEL_COMBO, m_debugLevel);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(ProcessParamsDlg, CDialog)
        //{{AFX_MSG_MAP(ProcessParamsDlg)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void ProcessParamsDlg::OnOK()
    {
        // Get Debug Level
        CString debugLevel;
        int selection = m_debugLevel.GetCurSel();
        TA_ASSERT(selection != CB_ERR, "Invalid selection in Debug Level combo box");
        m_debugLevel.GetLBText(selection, debugLevel);
        TA_Base_Core::RunParam runParam;

		std::string name = RPARAM_DEBUGLEVEL;
		std::string value = debugLevel.GetBuffer(0); 
        runParam.name  = name.c_str();
        runParam.value = value.c_str();
        
        AdminManager* pActiveAdminManager = static_cast<SystemManagerDlg*>(AdminManager::getMainWnd())->getActiveAdminManagerPtr();
        if ( NULL == pActiveAdminManager )
        {
            return;
        }
        // For all selected processes
        std::string process;
        for(std::vector<std::string>::iterator iter = m_processes.begin(); 
            iter != m_processes.end(); iter++)
        {
            process = *iter;

            // Log an event.

            const unsigned long entityKey = pActiveAdminManager->getHostEntityKey();
            std::string sessionId = AdminManager::getSessionId();
            std::string currentUser = AdminManager::getOperatorName();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Operator %s has requested that the debug level of process '%s' be changed to '%s'",
                currentUser.c_str(), process.c_str(), runParam.value);

            
            // Only send an audit message if in a valid session.
            if (AdminManager::validSession())
            {
                TA_Base_Core::AuditMessageSender* auditSender;
                auditSender = 
                    TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::OperatorAudit::Context );
        
                TA_Base_Core::DescriptionParameters desc;
                TA_Base_Core::NameValuePair param1("ProcessId", process);
                desc.push_back(&param1);
                TA_Base_Core::NameValuePair param2("debuglevel", debugLevel.GetBuffer(0));
                desc.push_back(&param2);

                auditSender->sendAuditMessage(
                    TA_Base_Core::OperatorAudit::ApplicationDebugLevelChange, // Message Type
                    entityKey,   // Entity key
                    desc,        // Description
                    "",          // Additional details
                    sessionId,   // SessionID if applicable
                    "",          // AlarmID if an alarm associated with event
                    "",          // IncidentKey if incident associated with event
                    "");         // EventID of a parent event, used to link events

				delete auditSender;
            }

            try
            {
                pActiveAdminManager->setProcessRunParam(process, runParam);
            }
            catch(TA_Base_Core::SystemControllerException&)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << process;
                userMsg.showMsgBox(IDS_UE_020024);
                return;
            }
        }

        CDialog::OnOK();
    }

    BOOL ProcessParamsDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        if(m_processes.empty())
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020036);
            EndDialog(IDCANCEL);
            return TRUE;
        }

        // Load all the allowed debug levels
        for (int i = TA_Base_Core::DebugUtil::DebugFatal; i < TA_Base_Core::DebugUtil::getInstance().countDebugLevels() ; i++)
        {
            m_debugLevel.AddString(TA_Base_Core::DebugUtil::getInstance().getStringDebugLevel((TA_Base_Core::DebugUtil::EDebugLevel)i));
        }

        // Need to display the current debug level (if all selected processes
        // are set to the same debug level) or a default debug level.

        // Firstly, get the debug level of the first selected process.
        std::string entityName = m_processes[0];
        AdminManager* pActiveAdminManager = static_cast<SystemManagerDlg*>(GetParent())->getActiveAdminManagerPtr();
        if ( NULL != pActiveAdminManager )
        {
            std::string debugLevelString = pActiveAdminManager->getProcessDebugLevel(entityName);
            TA_Base_Core::DebugUtil::EDebugLevel firstProcessDebugLevel = 
                TA_Base_Core::DebugUtil::getInstance().getDebugLevelFromString(debugLevelString.c_str());
    
            std::string process;
            TA_Base_Core::DebugUtil::EDebugLevel debugLevel;

            // Now loop through each process, checking if any process doesn't
            // match the first process (in which case use the default).

            for(std::vector<std::string>::iterator iter = m_processes.begin(); 
                iter != m_processes.end(); iter++)
            {
                process = *iter;
                debugLevelString = pActiveAdminManager->getProcessDebugLevel(process);
                debugLevel = TA_Base_Core::DebugUtil::getInstance().getDebugLevelFromString(debugLevelString.c_str());
                                
                if (debugLevel != firstProcessDebugLevel)
                {
                    debugLevel = TA_Base_Core::DebugUtil::DebugInfo; // The default.
                    break;
                }
            }

            m_debugLevel.SetCurSel(debugLevel);
        }

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }

} // namespace TA_Base_App

