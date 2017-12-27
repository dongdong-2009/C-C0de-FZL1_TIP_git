/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/pages/plan_page/src/PlanPage.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The class, PlanPage, provides a dialog that lists all Plans currently active in the system
  *
  * This class derives from BannerPage which allows to be a page in the Banner Framework.
  */

#if !defined(AFX_PLANDLG_H__8CC7AE74_3505_4003_AEDB_5C6FF0850B2E__INCLUDED_)
#define AFX_PLANDLG_H__8CC7AE74_3505_4003_AEDB_5C6FF0850B2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// forces mfc extensions to be imported
#define IMPORT_EXTENSIONS

#include <string>

#include "app\system_control\banner\pages\plan_page\src\PlanBannerPage_rc.h"
#include "core\utilities\src\RunParams.h"
#include "bus\mfc_extensions\src\toggleButton\toggleButton.h"
#include "bus\banner_framework\src\BannerPage.h"
#include "bus\response_plans\active_plans_display\src\AutonomousPlanActiveInstanceListCtrl.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"//TD16118
/////////////////////////////////////////////////////////////////////////////
// PlanPage dialog

class PlanPage : public TA_Base_Bus::BannerPage, TA_Base_Core::RunParamUser
{
// Construction
public:
    PlanPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog);
    virtual ~PlanPage();
    
    /**
     * onButtonDoubleClick
     *
     * Launch PlanManager on button double click
     */
    virtual void onButtonDoubleClick();

    /**
     * addBannerPage
     *
     * Tells the banner page to add itself to the banner.
     */
    virtual void addBannerPage();

    /**
     * onRunParamChange
     *
     * Receive SessionID notification changes so as to re-evaluate PLAN_VIEW_ACTIVE_ALL_LOCATIONS rights.
     */         
    virtual void onRunParamChange(const std::string& name, const std::string& value); 


protected:
    void LaunchPlanManager(const std::string &extraCommandLine);
    void SetAllLocationsBtnEnableState();
   

// Dialog Data
private:
	//{{AFX_DATA(PlanPage)
	enum { IDD = IDD_PLANS };
	ToggleButton m_allOwnersBtn;
	ToggleButton m_allLocationsBtn;
	TA_Base_Bus::CAutonomousPlanActiveInstanceListCtrl m_planListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlanPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   TA_Base_Bus::RightsLibrary* m_rightsLibrary;
	TA_Base_Bus::FixedHeaderCtrl m_headerCtrl;
protected:

	// Generated message map functions
	//{{AFX_MSG(PlanPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllOwnersCheck();
	afx_msg void OnAllLocationsCheck();
	afx_msg void OnDblclkPlanlist(NMHDR* pNMHDR, LRESULT* pResult);
	//TD16408_jinhua++
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANDLG_H__8CC7AE74_3505_4003_AEDB_5C6FF0850B2E__INCLUDED_)
