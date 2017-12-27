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
// Definitions commonly used by various Plan Manager classes.

#if !defined(AFX_PLANMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_PLANMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "boost/shared_ptr.hpp"
#include "bus/response_plans/active_plans_display/src/ActivePlansDisplayCommonDefs.h"

namespace TA_Base_App
{
    using TA_Base_Bus::TreeNodeId;
    using TA_Base_Bus::PlanInstanceNumber;

    // Forwards
    class TreeNode;
    class RootNode;
    class PlanStep;
    class PlanStepParameter;
    class PlanInstance;

    // Types
    typedef unsigned long PlanStepId;
    typedef unsigned long PlanStepNumber;
    typedef unsigned long LocationKey;

    typedef std::vector<TreeNodeId> TreeNodeIdList;
    typedef std::vector<TreeNode *> TreeNodeList;
    typedef std::vector<RootNode *> RootNodeList;
    typedef std::vector<PlanStep *> PlanStepList;
    typedef std::vector<PlanStepParameter *> PlanStepParameterList;
    typedef boost::shared_ptr<PlanInstance> PlanInstanceSharedPtr;
    typedef std::vector< PlanInstanceSharedPtr > PlanInstanceList;

    // Constants
    const unsigned int MIN_INTERACTIVE_STEP_TIMEOUT = 10;   // seconds
    const unsigned int MAX_INTERACTIVE_STEP_TIMEOUT = 300;  // seconds
    const unsigned int MIN_STEP_DELAY_MINUTE = 0;
    const unsigned int MAX_STEP_DELAY_MINUTE = 59;
    const unsigned int MIN_STEP_DELAY_SECOND = 0;
    const unsigned int MAX_STEP_DELAY_SECOND = 59;
     const unsigned int MIN_PA_MESSAGE_INTERVAL = 0;//seconds
     const unsigned int MAX_PA_MESSAGE_INTERVAL = 10;//seconds
     const unsigned int MIN_PA_CYLIC_TIMES = 1;
     const unsigned int MAX_PA_CYLIC_TIMES = 255;
    const unsigned int REREGISTRATION_TIMEOUT_SECONDS = 30;//seconds
    const char *const STEP_DELAY_EDIT_MASK = "[0-5][0-9]:[0-5][0-9]";  // mm:ss (00:00 -> 59:59)
    const char *const RPARAM_LOCKCATEGORY = "LockCategory";
    const char *const RPARAM_FWDEXECPLANID = "FwdExecPlanId";
    const char *const RPARAM_FWDVIEWPLANID = "FwdViewPlanId";
    const char* const RPARAM_FWDASSOCALARMID = "FwsAssocAlarmId";
    const char *const RPARAM_FLOWCHARTDIR = "FlowchartDir";
    const char *const RPARAM_REPORTTEMPLATE = "ReportTemplate";
    const char *const RPARAM_MAXCATEGORY = "MaxCategoryDepth";

     static const std::string PA_STATION_MESSAGE_TYPE = "车站广播消息";
     static const std::string PA_TRAIN_MESSAGE_TYPE    = "列车消息";
     static const std::string NO_MESSAGE = "没有选定消息";

    // Plan tree control item images, as per IDB_TREE_NODE_IMAGE_LIST_BMP
    typedef enum
            {
                TNIT_CLOSED_FOLDER = 0,
                TNIT_OPEN_FOLDER,
                TNIT_APPROVED_PLAN,
                TNIT_UNAPPROVED_PLAN,
                TNIT_ROOT
            }
            ETreeNodeImageType;

    // Plan step list item images, as per IDB_STEP_EXEC_STATE_IMAGE_LIST_BMP
    typedef enum
            {
                PSIT_NEXT_STEP = 0,
                PSIT_PAUSED_STEP,
                PSIT_EXECUTED_STEP,
                PSIT_FAILED_STEP,
                PSIT_SKIPPED_STEP,
                PSIT_UNEXECUTED_STEP,
                PSIT_DELAYED_STEP
            }
            EPlanStepImageType;

    // Custom window messages
    #define WM_WINDOW_HAS_BECOME_VISIBLE                (WM_USER + 510)
    #define WM_RESET_TREE_NODE_VIEW                     (WM_USER + 520)
    #define WM_TREE_NODE_SELECTION_CHANGED              (WM_USER + 530)
    #define WM_CATEGORY_VIEW_SELECTION_CHANGED          (WM_USER + 531)
    #define WM_ADJUST_CONTROL_POSITIONS                 (WM_USER + 535)
    #define WM_ADJUST_PLAN_FILTER_BUTTON_STATES         (WM_USER + 536)
    #define WM_PLAN_DETAIL_PAGE_SELECTION_CHANGED       (WM_USER + 560)
    #define WM_INTERACTIVE_STEP_EXEC_REQUEST            (WM_USER + 570)
    #define WM_PLAN_CUSTOMISATION_UPDATE                (WM_USER + 580)
    #define WM_REFRESH_PLAN_GENERAL_DETAIL              (WM_USER + 610)
    #define WM_REFRESH_PLAN_STEP_LIST                   (WM_USER + 620)
    #define WM_REFRESH_PLAN_STEP_PARAMETER_LIST         (WM_USER + 630)
    #define WM_SELECT_ACTIVE_PLAN_INSTANCE              (WM_USER + 640)
    #define WM_INTERACTIVE_STEP_EXECUTION_INTERRUPTED   (WM_USER + 650)
    #define WM_DISPLAY_STEP_PARAMETERS                  (WM_USER + 660)
    #define WM_CONTROLLED_PLAN_APPROVAL_STATE_CHANGED   (WM_USER + 670)
    #define WM_SESSION_ID_CHANGED                       (WM_USER + 680)
    #define WM_PLAN_STEP_FAILED                         (WM_USER + 690)

    // Case insensitive character comparison function
    struct CaseInsensitiveCompare
    {
        bool operator()(const char& ch1, const char& ch2) const
        {
            return ::tolower(ch1) < ::tolower(ch2);
        }
    };

    // Command UI enable state tester
    struct CCmdUIProbe : public CCmdUI
    {
        CCmdUIProbe(int cmdID)
        {
            m_nID = cmdID;
            m_enabled = TRUE;
        }

        void Enable(BOOL value)
        {
            m_enabled &= value;
        }

        BOOL m_enabled;
    };

    struct PlanInstanceId
    {
        PlanInstanceId(PlanInstanceNumber instance, LocationKey location) :
            m_instance(instance), m_location(location)
        {
        }

        bool operator!=(const PlanInstanceId &other) const
        {
            return (m_instance != other.m_instance || m_location != other.m_location);
        }

        bool operator==(const PlanInstanceId &other) const
        {
            return (m_instance == other.m_instance && m_location == other.m_location);
        }

        bool operator<(const PlanInstanceId &other) const
        {
            if (m_location < other.m_location)
            {
                return true;
            }

            if (m_location > other.m_location)
            {
                return false;
            }

            return (m_instance < other.m_instance);
        }

        bool operator>(const PlanInstanceId &other) const
        {
            return !(*this == other || *this < other);
        }

        PlanInstanceNumber m_instance;
        LocationKey m_location;
    };

    typedef enum { STEP_NOT_USE = 0, STEP_USE} EStepUseType;

} // namespace TA_Base_App


#endif // !defined(AFX_PLANMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
