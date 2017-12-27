
// DllHostDlg.h : header file
//

#pragma once

#include "app/system_control/dll_host/src/resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"


namespace TA_Base_App
{
    class CDllComponentMgr;


    class CDllHostDlg : public TA_Base_Bus::TransActiveDialog
    {
    public:
	    CDllHostDlg(TA_Base_Bus::IGUIAccess& gui);	// standard constructor

	    enum { IDD = IDD_TA_DLL_HOST_DIALOG };

    protected:

	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
        virtual BOOL OnInitDialog();

    public:

        virtual void init();
        virtual void uninit();

    private:

        void hideWindow();
        afx_msg void OnClose();

    private:

        CDllComponentMgr* m_dllComponentMgr;
        HICON m_hIcon;

        DECLARE_MESSAGE_MAP()
    };

}
