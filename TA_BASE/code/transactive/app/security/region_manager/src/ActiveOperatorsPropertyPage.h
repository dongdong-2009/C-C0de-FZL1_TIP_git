#if !defined(AFX_ACTIVEOPERATORSPROPERTYPAGE_H__4C34A92A_A151_40E2_ADCE_056BEB3E5193__INCLUDED_)
#define AFX_ACTIVEOPERATORSPROPERTYPAGE_H__4C34A92A_A151_40E2_ADCE_056BEB3E5193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/ActiveOperatorsPropertyPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Active Operators" tab. It displays the active 
  * region duty state for logged in operators.
  *
  */

namespace TA_Base_App
{
    class ActiveOperatorsPropertyPage : public CPropertyPage,
                                        public TA_Base_Bus::ISessionObserver,
                                        public TA_Base_Bus::IRegionDutyObserver
    {
	    DECLARE_DYNCREATE(ActiveOperatorsPropertyPage)

    // Operations
    public:
	    ActiveOperatorsPropertyPage();
	    virtual ~ActiveOperatorsPropertyPage();

        // ISessionObserver interface
        virtual void sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate);

        // IRegionDutyObserver interface
        virtual void dutyChanged(const TA_Base_Bus::RegionDutyUpdate& dutyUpdate);

    protected:
        void addOperator(const TA_Base_Bus::SessionInfoEx& sessionInfoEx);
        void addOperator(const TA_Base_Bus::SessionId& sessionId);
        void updateOperator(int row, const TA_Base_Bus::SessionInfoEx& sessionInfoEx, bool redrawRow = false);
        void updateOperator(int row, const TA_Base_Bus::SessionId& sessionId, bool redrawRow = false);
        void removeOperator(int row);
        TA_Base_Bus::SessionInfoEx getSessionInfo(int row);
        void enableDutyControlBtns();


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(ActiveOperatorsPropertyPage)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(ActiveOperatorsPropertyPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg void OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnAssignRegionsBtn();
	    afx_msg void OnUnassignRegionsBtn();
        afx_msg LRESULT OnReceiveSessionUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnReceiveRegionDutyUpdate(WPARAM wParam, LPARAM lParam);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(ActiveOperatorsPropertyPage)
	    enum { IDD = IDD_ACTIVE_OPERATORS_DLG };
	    CButton	m_assignRegionsBtn;
	    CButton	m_unassignRegionsBtn;
	    //}}AFX_DATA

        CGridCtrl m_regionDutyGrid;

        TA_Base_Bus::DutyRights m_dutyRights;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
        TA_Base_Bus::RegionDutyMatrix& m_regionDutyMatrix;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ACTIVEOPERATORSPROPERTYPAGE_H__4C34A92A_A151_40E2_ADCE_056BEB3E5193__INCLUDED_)
