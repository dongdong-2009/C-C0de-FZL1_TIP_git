//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "StdAfx.h"
#include "Resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "LabelMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Core;


/////////////////////////////////////////////////////////////////////////////
// CLabelMaker

LabelMaker::LabelMaker()
{
    FUNCTION_ENTRY( "LabelMaker" );
    FUNCTION_EXIT;
}


LabelMaker::~LabelMaker()
{
    FUNCTION_ENTRY( "~LabelMaker" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CLabelMaker member functions

CString LabelMaker::getLabel(UINT labelResourceID, const char *defaultLabel)
{
    FUNCTION_ENTRY( "getLabel" );

    CString label;

    if (label.LoadString(labelResourceID) == 0)
    {
        label = defaultLabel;
    }

    FUNCTION_EXIT;
    return label;
}


CString LabelMaker::getAllPlanTreeRootNodeLabel()
{
    FUNCTION_ENTRY( "getAllPlanTreeRootNodeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TREE_ALLPLANS_ROOT_LABEL, "All Plans");
}


CString LabelMaker::getApprovedPlanTreeRootNodeLabel()
{
    FUNCTION_ENTRY( "getApprovedPlanTreeRootNodeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TREE_APPROVEDPLANS_ROOT_LABEL, "Approved Plans");
}


CString LabelMaker::getUnapprovedPlanTreeRootNodeLabel()
{
    FUNCTION_ENTRY( "getUnapprovedPlanTreeRootNodeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TREE_UNAPPROVEDPLANS_ROOT_LABEL, "Unapproved Plans");
}


CString LabelMaker::getPlanGeneralDetailTabLabel()
{
    FUNCTION_ENTRY( "getPlanGeneralDetailTabLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_PLAN_GENERAL_DETAIL_TAB_LABEL, "General");
}


CString LabelMaker::getPlanStepDetailTabLabel()
{
    FUNCTION_ENTRY( "getPlanStepDetailTabLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_PLAN_STEP_DETAIL_TAB_LABEL, "Steps");
}


CString LabelMaker::getActivePlanListPlanColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListPlanColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_PLAN_COLUMN, "Plan");
}


CString LabelMaker::getActivePlanListCategoryColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListCategoryColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_CATEGORY_COLUMN, "Category");
}


CString LabelMaker::getActivePlanListInstanceColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListInstanceColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_INSTANCE_COLUMN, "Instance");
}


CString LabelMaker::getActivePlanListOwnerColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListOwnerColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_OWNER_COLUMN, "Owner");
}


CString LabelMaker::getActivePlanListLocationColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListLocationColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_LOCATION_COLUMN, "Location");
}


CString LabelMaker::getActivePlanListActivationColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListActivationColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_ACTIVATION_COLUMN, "Activation");
}


CString LabelMaker::getActivePlanListStateColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListStateColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_STATE_COLUMN, "State");
}


CString LabelMaker::getActivePlanListStepColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListStepColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_STEP_COLUMN, "Step");
}


CString LabelMaker::getActivePlanListRemarksColumnLabel()
{
    FUNCTION_ENTRY( "getActivePlanListRemarksColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_ACTIVE_PLAN_LIST_REMARKS_COLUMN, "Remarks");
}


CString LabelMaker::getStepListStepColumnLabel()
{
    FUNCTION_ENTRY( "getStepListStepColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_STEP_COLUMN, "Step");
}


CString LabelMaker::getStepListNameColumnLabel()
{
    FUNCTION_ENTRY( "getStepListNameColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_NAME_COLUMN, "Name");

}


CString LabelMaker::getStepListTypeColumnLabel()
{
    FUNCTION_ENTRY( "getStepListTypeColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_TYPE_COLUMN, "Type");

}


CString LabelMaker::getStepListDescriptionColumnLabel()
{
    FUNCTION_ENTRY( "getStepListDescriptionColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_DESCRIPTION_COLUMN, "Description");

}


CString LabelMaker::getStepListSkipColumnLabel()
{
    FUNCTION_ENTRY( "getStepListSkipColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_SKIP_COLUMN, "Skip");

}


CString LabelMaker::getStepListIgnoreFailureColumnLabel()
{
    FUNCTION_ENTRY( "getStepListIgnoreFailureColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_IGNORE_FAILURE_COLUMN, "Ignore Failure");

}

CString LabelMaker::getStepListNoWaitColumnLabel()
{
    return getLabel(IDS_STEP_LIST_NOWAIT_COLUMN, "No Wait");
}

CString LabelMaker::getStepListDelayColumnLabel()
{
    FUNCTION_ENTRY( "getStepListDelayColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_LIST_DELAY_COLUMN, "Delay\n(mm:ss)");

}


CString LabelMaker::getStepParameterListNameColumnLabel()
{
    FUNCTION_ENTRY( "getStepParameterListNameColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_PARAMETER_LIST_NAME_COLUMN, "Parameter Name");

}


CString LabelMaker::getStepParameterListValueColumnLabel()
{
    FUNCTION_ENTRY( "getStepParameterListValueColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_PARAMETER_LIST_VALUE_COLUMN, "Parameter Value");
}


CString LabelMaker::getNodeListNameColumnLabel()
{
    FUNCTION_ENTRY( "getNodeListNameColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_LIST_NAME_COLUMN, "Name");
}


CString LabelMaker::getNodeListTypeColumnLabel()
{
    FUNCTION_ENTRY( "getNodeListTypeColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_LIST_TYPE_COLUMN, "Type");
}


CString LabelMaker::getNodeListStatusColumnLabel()
{
    FUNCTION_ENTRY( "getNodeListStatusColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_LIST_STATUS_COLUMN, "Status");
}


CString LabelMaker::getTisMessageListTypeColumnLabel()
{
    FUNCTION_ENTRY( "getTisMessageListTypeColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TISMSG_LIST_TYPE_COLUMN, "Type");
}


CString LabelMaker::getTisMessageListIdColumnLabel()
{
    FUNCTION_ENTRY( "getTisMessageListIdColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TISMSG_LIST_ID_COLUMN, "ID");
}


CString LabelMaker::getTisMessageListMessageColumnLabel()
{
    FUNCTION_ENTRY( "getTisMessageListMessageColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TISMSG_LIST_MESSAGE_COLUMN, "Message");
}


CString LabelMaker::getTisMessageListPriorityColumnLabel()
{
    FUNCTION_ENTRY( "getTisMessageListPriorityColumnLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_TISMSG_LIST_PRIORITY_COLUMN, "Priority");
}


CString LabelMaker::getYesNoLabel(bool affirmative)
{
    FUNCTION_ENTRY( "getYesNoLabel" );

    if (affirmative)
    {
        FUNCTION_EXIT;
        return getLabel(IDS_YES, "Yes");
    }

    FUNCTION_EXIT;
    return getLabel(IDS_NO, "No");
}


CString LabelMaker::getTrueFalseLabel(bool condition)
{
    FUNCTION_ENTRY( "getTrueFalseLabel" );

    if (condition)
    {
        FUNCTION_EXIT;
        return getLabel(IDS_TRUE, "True");
    }

    FUNCTION_EXIT;
    return getLabel(IDS_FALSE, "False");
}


CString LabelMaker::getNewActivePlanInstanceLabel()
{
    FUNCTION_ENTRY( "getNewActivePlanInstanceLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NEW_ACTIVE_PLAN_INSTANCE, "нч");
}


CString LabelMaker::getRootNodeTypeLabel()
{
    FUNCTION_ENTRY( "getRootNodeTypeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_TYPE_ROOT, "Root");
}


CString LabelMaker::getCategoryNodeTypeLabel()
{
    FUNCTION_ENTRY( "getCategoryNodeTypeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_TYPE_CATEGORY, "Category");
}


CString LabelMaker::getPlanNodeTypeLabel()
{
    FUNCTION_ENTRY( "getPlanNodeTypeLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_NODE_TYPE_PLAN, "Plan");
}


CString LabelMaker::getEmptyCategoryLabel()
{
    FUNCTION_ENTRY( "getEmptyCategoryLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_CATEGORY_EMPTY, "Empty");
}


CString LabelMaker::getNonEmptyCategoryLabel()
{
    FUNCTION_ENTRY( "getNonEmptyCategoryLabel" );
    FUNCTION_EXIT;
    return getLabel(IDS_CATEGORY_NONEMPTY, "Populated");
}


CString LabelMaker::getCategoryHeadingPrefix()
{
    FUNCTION_ENTRY( "getCategoryHeadingPrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_CATEGORY_HEADING_PREFIX, "Category");
}


CString LabelMaker::getPlanHeadingPrefix()
{
    FUNCTION_ENTRY( "getPlanHeadingPrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_PLAN_HEADING_PREFIX, "Plan");
}


CString LabelMaker::getStepHeadingPrefix()
{
    FUNCTION_ENTRY( "getStepHeadingPrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_STEP_HEADING_PREFIX, "Step");
}


CString LabelMaker::getDecisionStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getDecisionStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_DECISION_STEP_DIALOG_TITLE_PREFIX, "Decision");
}


CString LabelMaker::getLogEntryStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getLogEntryStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_LOGENTRY_STEP_DIALOG_TITLE_PREFIX, "Log");
}


CString LabelMaker::getPromptStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getPromptStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_PROMPT_STEP_DIALOG_TITLE_PREFIX, "Prompt");
}


CString LabelMaker::getRunPlanStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getRunPlanStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_RUN_PLAN_STEP_DIALOG_TITLE_PREFIX, "Run Plan");
}


CString LabelMaker::getLaunchGuiAppStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getLaunchGuiAppStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_LAUNCH_GUI_APP_STEP_DIALOG_TITLE_PREFIX, "Launch GUI Application");
}


CString LabelMaker::getLaunchSchematicStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getLaunchSchematicStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_LAUNCH_SCHEMATIC_STEP_DIALOG_TITLE_PREFIX, "Launch Schematic");
}


CString LabelMaker::getFailureSummaryStepDialogTitlePrefix()
{
    FUNCTION_ENTRY( "getFailureSummaryStepDialogTitlePrefix" );
    FUNCTION_EXIT;
    return getLabel(IDS_FAILURE_SUMMARY_STEP_DIALOG_TITLE_PREFIX, "Failed Step Summary");
}


