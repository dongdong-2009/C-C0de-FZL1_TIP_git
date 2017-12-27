/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationProgressDlg.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class implements a dialog that is used to represent the percentage of
  * configuration plugins that have been loaded.
  *
  */

#include "stdafx.h"
#include "configurationeditor.h"
#include "ConfigurationProgressDlg.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigurationProgressDlg dialog

CConfigurationProgressDlg::CConfigurationProgressDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CConfigurationProgressDlg::IDD, pParent),
      m_totalComponentNumber(100)
{
    //{{AFX_DATA_INIT(CConfigurationProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CConfigurationProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CConfigurationProgressDlg)
    DDX_Control(pDX, IDC_LOADPROGRESS, m_progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigurationProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CConfigurationProgressDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigurationProgressDlg message handlers

BOOL CConfigurationProgressDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Initialise the progress bar.
    m_progress.SetRange(0, m_totalComponentNumber);
    m_progress.SetPos(0);

    // Should the progress be always on top?
    //SetWindowPos( &this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigurationProgressDlg::updateLoadProgress(unsigned char percentage)
{
    CWnd* pWnd = GetDlgItem(IDC_PROGRESS_STATIC);

    if (pWnd != NULL)
    {
        pWnd->ShowWindow(SW_SHOW);
        CString strMsg;
        strMsg.Format("正在加载插件，当前进度: %d", percentage);
        strMsg += "%";
        pWnd->SetWindowText(strMsg);
    }

    // Clamp the upper limit.
    if (100 < percentage)
    {
        percentage = 100;
    }

    // Update progress.
    m_progress.SetPos(percentage);
}

void CConfigurationProgressDlg::setTotalComponentNumber(size_t totalComponentNumber)
{
    m_totalComponentNumber = totalComponentNumber;
    m_progress.SetRange(0, totalComponentNumber);
    m_progress.SetPos(0);
}

void CConfigurationProgressDlg::setLoadedComponentNumber(size_t loadedComponentNumber)
{
    m_progress.SetPos(loadedComponentNumber);
    CString strMsg;
    strMsg.Format("正在加载插件，当前进度: %d%% (%d/%d)", (loadedComponentNumber * 100 / m_totalComponentNumber), loadedComponentNumber, m_totalComponentNumber);
    GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(strMsg);
}
