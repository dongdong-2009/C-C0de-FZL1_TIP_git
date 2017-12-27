#if !defined(AFX_REGIONDUTYCHANGECONFIRMDLG_H__A5FE92D1_707F_402D_AA33_8C6A5B35011F__INCLUDED_)
#define AFX_REGIONDUTYCHANGECONFIRMDLG_H__A5FE92D1_707F_402D_AA33_8C6A5B35011F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyChangeConfirmDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog is shown to the operator before effecting region duty transfer/removal
  * that leads to uncontrolled subsystems. The uncontrolled subsystems are displayed so
  * that the operator can decide whether or not to proceed with the duty change.
  *
  */

namespace TA_Base_App
{
    class RegionDutyChangeConfirmDlg : public CDialog
    {
    // Construction
    public:
	    RegionDutyChangeConfirmDlg(const TA_Base_Bus::SubsystemTreeMap& uncontrolledSubsystems, UINT nIDTemplate, CWnd* pParent = NULL);
        virtual ~RegionDutyChangeConfirmDlg();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RegionDutyChangeConfirmDlg)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(RegionDutyChangeConfirmDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnYes();
	    afx_msg void OnNo();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    public:
	    enum { IDD_XFER = IDD_REGION_DUTY_XFER_CONFIRM_DLG, IDD_REMOVE = IDD_REGION_DUTY_REMOVE_CONFIRM_DLG };

    private:
	    //{{AFX_DATA(RegionDutyChangeConfirmDlg)
	    CTreeCtrl	m_subsystemTree;
	    //}}AFX_DATA

        TA_Base_Bus::SubsystemTreeMap m_uncontrolledSubsystems;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONDUTYCHANGECONFIRMDLG_H__A5FE92D1_707F_402D_AA33_8C6A5B35011F__INCLUDED_)
