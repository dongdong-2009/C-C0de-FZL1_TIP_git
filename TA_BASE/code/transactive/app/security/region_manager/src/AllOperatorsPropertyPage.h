#if !defined(AFX_ALLOPERATORSPROPERTYPAGE_H__E2FB04E1_77F2_42BF_B7CC_EA0AAD826B08__INCLUDED_)
#define AFX_ALLOPERATORSPROPERTYPAGE_H__E2FB04E1_77F2_42BF_B7CC_EA0AAD826B08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/AllOperatorsPropertyPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "All Operators" tab. It displays the default 
  * operator-region assignments for all configured operators.
  *
  */
#include "RegionManager.h"

namespace TA_Base_App
{
    class AllOperatorsPropertyPage : public CPropertyPage
    {
	    DECLARE_DYNCREATE(AllOperatorsPropertyPage)

    // Operations
    public:
	    AllOperatorsPropertyPage();
	    virtual ~AllOperatorsPropertyPage();


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(AllOperatorsPropertyPage)
    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(AllOperatorsPropertyPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(AllOperatorsPropertyPage)
	    enum { IDD = IDD_ALL_OPERATORS_DLG };
		    // NOTE - ClassWizard will add data members here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_DATA

        CGridCtrl m_defRgnAssignGrid;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::DefaultRegionAssignmentMatrix& m_defRgnAssignMatrix;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ALLOPERATORSPROPERTYPAGE_H__E2FB04E1_77F2_42BF_B7CC_EA0AAD826B08__INCLUDED_)
