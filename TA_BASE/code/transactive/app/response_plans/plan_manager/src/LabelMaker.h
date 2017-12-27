//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LabelMaker.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_LABELMAKER_H__A6EF7B1D_89CB_4405_905A_98F3C2A307FD__INCLUDED_)
#define AFX_LABELMAKER_H__A6EF7B1D_89CB_4405_905A_98F3C2A307FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // LabelMaker

    class LabelMaker
    {
    // Operations
    public:
        LabelMaker();
        virtual ~LabelMaker();

        static CString getAllPlanTreeRootNodeLabel();
        static CString getApprovedPlanTreeRootNodeLabel();
        static CString getUnapprovedPlanTreeRootNodeLabel();
        static CString getPlanGeneralDetailTabLabel();
        static CString getPlanStepDetailTabLabel();
        static CString getActivePlanListPlanColumnLabel();
        static CString getActivePlanListCategoryColumnLabel();
        static CString getActivePlanListInstanceColumnLabel();
        static CString getActivePlanListOwnerColumnLabel();
        static CString getActivePlanListLocationColumnLabel();
        static CString getActivePlanListActivationColumnLabel();
        static CString getActivePlanListStateColumnLabel();
        static CString getActivePlanListStepColumnLabel();
        static CString getActivePlanListRemarksColumnLabel();
        static CString getStepListStepColumnLabel();
        static CString getStepListNameColumnLabel();
        static CString getStepListTypeColumnLabel();
        static CString getStepListDescriptionColumnLabel();
        static CString getStepListSkipColumnLabel();
        static CString getStepListIgnoreFailureColumnLabel();
		static CString getStepListNoWaitColumnLabel();
        static CString getStepListDelayColumnLabel();
        static CString getStepParameterListNameColumnLabel();
        static CString getStepParameterListValueColumnLabel();
        static CString getNodeListNameColumnLabel();
        static CString getNodeListTypeColumnLabel();
        static CString getNodeListStatusColumnLabel();
        static CString getTisMessageListTypeColumnLabel();
        static CString getTisMessageListIdColumnLabel();
        static CString getTisMessageListMessageColumnLabel();
        static CString getTisMessageListPriorityColumnLabel();
        static CString getYesNoLabel(bool affirmative);
        static CString getTrueFalseLabel(bool condition);
        static CString getNewActivePlanInstanceLabel();
        static CString getRootNodeTypeLabel();
        static CString getCategoryNodeTypeLabel();
        static CString getPlanNodeTypeLabel();
        static CString getEmptyCategoryLabel();
        static CString getNonEmptyCategoryLabel();
        static CString getCategoryHeadingPrefix();
        static CString getPlanHeadingPrefix();
        static CString getStepHeadingPrefix();
        static CString getDecisionStepDialogTitlePrefix();
        static CString getLogEntryStepDialogTitlePrefix();
        static CString getPromptStepDialogTitlePrefix();
        static CString getRunPlanStepDialogTitlePrefix();
        static CString getLaunchGuiAppStepDialogTitlePrefix();
        static CString getLaunchSchematicStepDialogTitlePrefix();
        static CString getFailureSummaryStepDialogTitlePrefix();
    private:
        static CString getLabel(UINT labelResourceID, const char *defaultLabel);
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_LABELMAKER_H__A6EF7B1D_89CB_4405_905A_98F3C2A307FD__INCLUDED_)
