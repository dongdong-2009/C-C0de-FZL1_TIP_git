#if !defined(AFX_RegionDutyRemovalDlg_H__B413492E_ADA6_4F1C_AF00_492765B1089A__INCLUDED_)
#define AFX_RegionDutyRemovalDlg_H__B413492E_ADA6_4F1C_AF00_492765B1089A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyRemovalDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog enables the operator to select from a list of removable regions.
  * A warning is displayed if the operator decides to effect a duty removal that 
  * leads to uncontrolled subsystems.
  *
  */

namespace TA_Base_App
{
    class RegionDutyRemovalDlg : public CDialog
    {
    // Operations
    public:
	    RegionDutyRemovalDlg(const TA_Base_Bus::SessionId& sourceSessionId, CWnd* pParent = NULL);
        virtual ~RegionDutyRemovalDlg();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RegionDutyRemovalDlg)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(RegionDutyRemovalDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnUnassignBtn();
	    afx_msg void OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(RegionDutyRemovalDlg)
	    enum { IDD = IDD_REGION_DUTY_REMOVE_DLG };
	    CButton	m_unassignBtn;
	    CListCtrl	m_removableRegionList;
	    CStatic	m_sourceOperator;
	    //}}AFX_DATA

        TA_Base_Bus::SessionId m_sourceSessionId;
        TA_Base_Bus::SubsystemTreeMap m_removableRegions;

        TA_Base_Bus::DutyAgentAccess m_dutyAgent;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_RegionDutyRemovalDlg_H__B413492E_ADA6_4F1C_AF00_492765B1089A__INCLUDED_)
