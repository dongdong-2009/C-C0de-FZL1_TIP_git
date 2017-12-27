#include "app/system_control/dll_host/src/stdafx.h"
#include "app/system_control/dll_host/src/DllHost.h"
#include "app/system_control/dll_host/src/DllHostDlg.h"
#include "app/system_control/dll_host/src/DllComponentMgr.h"


namespace TA_Base_App
{

    CDllHostDlg::CDllHostDlg(TA_Base_Bus::IGUIAccess& gui)
	    : TA_Base_Bus::TransActiveDialog(gui, CDllHostDlg::IDD),
          m_dllComponentMgr(NULL)
    {
	    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }


    void CDllHostDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
    }


    BEGIN_MESSAGE_MAP(CDllHostDlg, TransActiveDialog)
    END_MESSAGE_MAP()


    BOOL CDllHostDlg::OnInitDialog()
    {
        TA_Base_Bus::ResizingProperties properties = { true, 0, 0, 0, 0 };

        setResizingProperties(properties);

        TransActiveDialog::OnInitDialog();

        hideWindow();

        return TRUE;
    }


    void CDllHostDlg::init()
    {
        m_dllComponentMgr = new CDllComponentMgr;
    }


    afx_msg void CDllHostDlg::OnClose()
    {
        TransActiveDialog::OnClose();
    }


    void CDllHostDlg::uninit()
    {
        if ( m_dllComponentMgr != NULL )
        {
            m_dllComponentMgr->deactivateAndDeleteServant();
            m_dllComponentMgr = NULL;
        }
    }


    void CDllHostDlg::hideWindow()
    {
        MoveWindow(0, 0, 0, 0);
        SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
    }

}
