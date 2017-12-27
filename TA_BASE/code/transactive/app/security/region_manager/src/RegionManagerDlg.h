#if !defined(AFX_REGIONMANAGERDLG_H__5D32B9B7_F2FD_42C5_8771_826D65493DC8__INCLUDED_)
#define AFX_REGIONMANAGERDLG_H__5D32B9B7_F2FD_42C5_8771_826D65493DC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the main Region Manager dialog window.
  *
  */

namespace TA_Base_App
{
    class RegionManagerDlg : public TA_Base_Bus::TransActiveDialog,
                             public TA_Base_Bus::IConfigDataObserver
    {
    // Operations
    public:
	    RegionManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback);
        virtual ~RegionManagerDlg();

        // IConfigDataObserver interface
        virtual void dataChanged(const TA_Base_Bus::DataUpdate& dataUpdate);

    private:
        RegionManagerDlg(const RegionManagerDlg& regionManagerDlg);
        RegionManagerDlg& operator=(const RegionManagerDlg& regionManagerDlg);


    // Overrides
	    //{{AFX_VIRTUAL(RegionManagerDlg)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(RegionManagerDlg)
	    virtual BOOL OnInitDialog();
        afx_msg void OnAppAbout();
	    afx_msg void OnPaint();
	    afx_msg HCURSOR OnQueryDragIcon();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAppExit();
	afx_msg void OnClose();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog data
    private:
	    //{{AFX_DATA(RegionManagerDlg)
	    enum { IDD = IDD_REGION_MANAGER_DLG };
	    //}}AFX_DATA

	    HICON m_hIcon;

        RegionManagerPropertySheet m_regionPropertySheet;
        AllOperatorsPropertyPage m_allOpsPropertyPage;
        ActiveOperatorsPropertyPage m_activeOpsPropertyPage;
        SubsystemsPropertyPage m_subsystemsPropertyPage;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONMANAGERDLG_H__5D32B9B7_F2FD_42C5_8771_826D65493DC8__INCLUDED_)
