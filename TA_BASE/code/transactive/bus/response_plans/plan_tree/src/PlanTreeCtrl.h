/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanTreeCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 18:34:29 $
  * Last modified by:  $Author: qi.huang $
  *
  */
// PlanTreeCtrl.h : header file
//

#if !defined(AFX_PLANTREECTRL_H__DD2CEDF3_5F34_4A04_830D_9D37314146D7__INCLUDED_)
#define AFX_PLANTREECTRL_H__DD2CEDF3_5F34_4A04_830D_9D37314146D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeCtrl.h"
#include "bus/response_plans/plan_tree/src/PlanFilter.h"
#include "bus/response_plans/plan_tree/src/IPlanAccess.h"


namespace TA_Base_Bus
{
    class IPlanAccess;
    class CRootItem;
    class CPlanItem;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanTreeCtrl window

    class CPlanTreeCtrl : public CObjectTreeCtrl
    {
    public:
        // Support initialization language switch
        static std::string RootItemName;

    // Construction/Destruction
    public:
        // Standard constructor
	    CPlanTreeCtrl(PlanFilter::PlanFilterType planFilterType = PlanFilter::PFT_ALL_PLANS, bool directDbAccess = true);
	    
        // Allows a list of plans to be specified that are not affected by the filter when the filter is anything
        // other than PFT_ALL_PLANS. These plans will appear in the tree even if they do not satisfy the requirements
        // of the filter.
        CPlanTreeCtrl(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans, bool directDbAccess);

	    virtual ~CPlanTreeCtrl();

    // Operations
    public:
        // Finds the specified plan in the plan tree and selects it.
        // The tree is automatically expanded to show the selected plan.
        // If the specified plan cannot be found, the current selection remains unchanged.
        void setSelectedPlan(TA_Base_Core::NodeId planId);
        void setSelectedPlan(const std::string& planPath);

        // Returns NULL if no plan is selected or the selected plan tree item is not a plan.
        CPlanItem* getSelectedPlan();


    // Attributes
    private:
       IPlanAccess* m_planAccess;
       CRootItem* m_rootItem;


    // Overrides
    protected:
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CPlanTreeCtrl)
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL

    // Generated message map functions
    protected:
	    //{{AFX_MSG(CPlanTreeCtrl)
        afx_msg LRESULT OnInitialise(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnSetSelectedPlan(WPARAM wParam, LPARAM lParam);
        afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANTREECTRL_H__DD2CEDF3_5F34_4A04_830D_9D37314146D7__INCLUDED_)
