#if !defined(AFX_SUBSYSTEMUTYTRANSFERDLG_H__B48B85EC_6382_477E_A396_2BB0899BA990__INCLUDED_)
#define AFX_SUBSYSTEMUTYTRANSFERDLG_H__B48B85EC_6382_477E_A396_2BB0899BA990__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/SubsystemDutyTransferDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog enables the operator to select from a list of transferable subsystems
  * and request duties for these subsystems.
  *
  */

namespace TA_Base_App
{
    class SubsystemDutyTransferDlg : public CDialog
    {
    // Operations
    public:
	    SubsystemDutyTransferDlg(CWnd* pParent = NULL);
        virtual ~SubsystemDutyTransferDlg();
		int GetColumnCount();//cl-19416--lkm
		void AdjustColumnWidth();//cl-19416--lkm
    protected:
        void updateTransferableSubsystems(const TA_Base_Bus::RegionKeyList& regionKeys);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SubsystemDutyTransferDlg)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(SubsystemDutyTransferDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSubsystemListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnSelectRgnsBtn();
	    afx_msg void OnRequestDutyBtn();
	    afx_msg void OnRequestAlldutyBtn();
		afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(SubsystemDutyTransferDlg)
	    enum { IDD = IDD_SUBSYSTEM_DUTY_XFER_DLG };
	    CEdit	m_regionsEdit;
	    CButton	m_regionsBox;
	    CButton	m_selectRegionsBtn;
	    CButton	m_requestDutyBtn;
		CButton	m_requestAllDutyBtn;
	    CListCtrl	m_transferableSubsystemList;
	    //}}AFX_DATA

        TA_Base_Bus::TransferableSubsystemList m_transferableSubsystems;

        TA_Base_Bus::DutyAgentAccess m_dutyAgent;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
        TA_Base_Bus::RegionDutyMatrix& m_regionDutyMatrix;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SUBSYSTEMUTYTRANSFERDLG_H__B48B85EC_6382_477E_A396_2BB0899BA990__INCLUDED_)
