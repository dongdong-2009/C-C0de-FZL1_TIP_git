/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#include "StdAfx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include <algorithm>
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "PlanAgentAccess.h"
#include "PlanStepMap.h"
#include "PlanStep.h"
#include "DataPointCheckStep.h"
#include "DataPointSetStep.h"
#include "EndStep.h"
#include "ActivePlanCheckStep.h"
#include "ActivateCameraPresetStep.h"
#include "AbortStep.h"
#include "DecisionStep.h"
#include "JumpStep.h"
#include "LaunchGuiAppStep.h"
#include "LogStep.h"
#include "PromptStep.h"
#include "RunPlanStep.h"
#include "TerminatePlanStep.h"
#include "VideoWallStep.h"
#include "VmsBlankoutStep.h"
#include "VmsRestoreScheduleStep.h"
#include "VmsSetSceneStep.h"
#include "StationPAStep.h"
#include "AssignCameraToMonitorStep.h"
#include "SendSmsMessageStep.h"
#include "LaunchSchematicStep.h"
#include "EvaluateExpressionStep.h"
#include "StationTISStep.h"
#include "TrainTISStep.h"
#include "TrainPAStep.h"
#include "FailureSummaryStep.h" //DP-changes
#include "StationPAByAlarmStep.h"
#include "BackgroundLaunchSchematicStep.h"
#include "BackgroundLaunchGuiAppStep.h"
#include "BackgroundDecisionStep.h"
#include "BackgroundLogStep.h"
#include "BackgroundPromptStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    struct PlanStepComparator
    {
        bool operator()(PlanStep *&step1, PlanStep *&step2) const
        {
            return step1->getStepPosition() < step2->getStepPosition();
        }
    };
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepMap

PlanStepMap::PlanStepMap(PlanNode* parent,const TA_Base_Core::StepDetails &stepDetails) : m_parent(parent)
{
    FUNCTION_ENTRY( "PlanStepMap" );

    for (CORBA::ULong i = 0; i < stepDetails.length(); i++)
    {
        PlanStep *planStep = createPlanStep(stepDetails[i]);

        m_planStepMap.insert(planStep->getStepId(), planStep);
    }

    FUNCTION_EXIT;
}


PlanStepMap::~PlanStepMap()
{
    FUNCTION_ENTRY( "~PlanStepMap" );

    while (!m_planStepMap.empty())
    {
        PlanStep *planStep = m_planStepMap.front();
        m_planStepMap.erase(planStep->getStepId());

        delete planStep;
    }

    FUNCTION_EXIT;
}


void PlanStepMap::getPlanSteps(PlanStepList &planSteps)
{
    FUNCTION_ENTRY( "getPlanSteps" );

    m_planStepMap.getItems(planSteps);

    std::sort(planSteps.begin(), planSteps.end(), PlanStepComparator());

    FUNCTION_EXIT;
}


PlanStep *PlanStepMap::getPlanStep(PlanStepId stepId)
{
    FUNCTION_ENTRY( "getPlanStep" );

    if (m_planStepMap.find(stepId))
    {
        FUNCTION_EXIT;
        return m_planStepMap[stepId];
    }

    FUNCTION_EXIT;
    return NULL;
}


PlanStep *PlanStepMap::createPlanStep(const TA_Base_Core::StepDetail &stepDetail)
{
    FUNCTION_ENTRY( "createPlanStep" );

    switch (stepDetail.type)
    {
#if defined (STEPTYPE_ABORT)
        case TA_Base_Core::ABORT_STEP:
            FUNCTION_EXIT;
            return new AbortStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)
        case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            FUNCTION_EXIT;
            return new ActivateCameraPresetStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)
        case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
            FUNCTION_EXIT;
            return new ActivePlanCheckStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
        case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
            FUNCTION_EXIT;
            return new AssignCameraToMonitorStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_DATA_POINT_CHECK)
        case TA_Base_Core::DATA_POINT_CHECK_STEP:
            FUNCTION_EXIT;
            return new DataPointCheckStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_DATA_POINT_SET)
        case TA_Base_Core::DATA_POINT_SET_STEP:
            FUNCTION_EXIT;
            return new DataPointSetStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_DECISION)
        case TA_Base_Core::DECISION_STEP:
            FUNCTION_EXIT;
            return new DecisionStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_EVALUATE_EXPRESSION)
        case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            FUNCTION_EXIT;
            return new EvaluateExpressionStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_JUMP)
        case TA_Base_Core::JUMP_STEP:
            FUNCTION_EXIT;
            return new JumpStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_LAUNCH_GUI_APP)
        case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            FUNCTION_EXIT;
            return new LaunchGuiAppStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)
        case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            FUNCTION_EXIT;
            return new LaunchSchematicStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_LOG)
        case TA_Base_Core::LOG_STEP:
            FUNCTION_EXIT;
            return new LogStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_PROMPT)
        case TA_Base_Core::PROMPT_STEP:
            FUNCTION_EXIT;
            return new PromptStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_RUN_PLAN)
        case TA_Base_Core::RUN_PLAN_STEP:
            FUNCTION_EXIT;
            return new RunPlanStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_SEND_SMS_MESSAGE)
        case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            FUNCTION_EXIT;
            return new SendSmsMessageStep( m_parent, stepDetail );
#endif

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
        case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return new StationTisStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_TERMINATE_PLAN)
        case TA_Base_Core::TERMINATE_PLAN_STEP:
            FUNCTION_EXIT;
            return new TerminatePlanStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
        case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return new TrainTisStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
        case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            FUNCTION_EXIT;
            return new StationPAStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
        case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            FUNCTION_EXIT;
            return new TrainPAStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_VIDEOWALL)
        case TA_Base_Core::VIDEO_WALL_STEP:
            FUNCTION_EXIT;
            return new VideoWallStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_VMS_BLANKOUT)
        case TA_Base_Core::VMS_BLANKOUT_STEP:
            FUNCTION_EXIT;
            return new VmsBlankoutStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_VMS_RESTORE_SCHEDULE)
        case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            FUNCTION_EXIT;
            return new VmsRestoreScheduleStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_VMS_SET_SCENE)
        case TA_Base_Core::VMS_SET_SCENE_STEP:
            FUNCTION_EXIT;
            return new VmsSetSceneStep(m_parent, stepDetail);
#endif

//lizettejl++ (DP-changes)
#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)
        case TA_Base_Core::FAILURE_SUMMARY_STEP:
            FUNCTION_EXIT;
            return new FailureSummaryStep(m_parent, stepDetail);
#endif
//++lizettejl

//WuZhongyi New Step Fire Alarm Station PA
#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM)
		case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM :
			FUNCTION_EXIT;
			return new StationPAByAlarmStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)
        case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP :
            FUNCTION_EXIT;
            return new BackgroundLaunchSchematicStep(m_parent, stepDetail);
#endif


#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
        case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP :
            FUNCTION_EXIT;
            return new BackgroundLaunchGuiAppStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_BACKGROUND_DECISION)
        case TA_Base_Core::BACKGROUND_DECISION_STEP :
            FUNCTION_EXIT;
            return new BackgroundDecisionStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_BACKGROUND_LOG)
        case TA_Base_Core::BACKGROUND_LOG_STEP :
            FUNCTION_EXIT;
            return new BackgroundLogStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_BACKGROUND_PROMPT)
        case TA_Base_Core::BACKGROUND_PROMPT_STEP :
            FUNCTION_EXIT;
            return new BackgroundPromptStep(m_parent, stepDetail);
#endif

#if defined (STEPTYPE_BACKGROUND_RUN_PLAN)
        case TA_Base_Core::BACKGROUND_RUN_PLAN_STEP :
            FUNCTION_EXIT;
            return new BackgroundRunPlanStep(m_parent, stepDetail);
#endif

        case TA_Base_Core::END_STEP:
            FUNCTION_EXIT;
            return new EndStep(m_parent, stepDetail);

        default:
            std::ostringstream errorMessage;
            errorMessage << "Unknown step type " << stepDetail.type << " for step ID " << stepDetail.step;

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::UNDEFINED_STEP));
    }
}


PlanStep *PlanStepMap::createNewPlanStep(const TA_Base_Core::EStep stepType, PlanStepId stepId)
{
    FUNCTION_ENTRY( "createNewPlanStep" );

    switch (stepType)
    {
#if defined (STEPTYPE_ABORT)
        case TA_Base_Core::ABORT_STEP:
            FUNCTION_EXIT;
            return new AbortStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)
        case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            FUNCTION_EXIT;
            return new ActivateCameraPresetStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)
        case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
            FUNCTION_EXIT;
            return new ActivePlanCheckStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
        case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
            FUNCTION_EXIT;
            return new AssignCameraToMonitorStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_DATA_POINT_CHECK)
        case TA_Base_Core::DATA_POINT_CHECK_STEP:
            FUNCTION_EXIT;
            return new DataPointCheckStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_DATA_POINT_SET)
        case TA_Base_Core::DATA_POINT_SET_STEP:
            FUNCTION_EXIT;
            return new DataPointSetStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_DECISION)
        case TA_Base_Core::DECISION_STEP:
            FUNCTION_EXIT;
            return new DecisionStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_EVALUATE_EXPRESSION)
        case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            FUNCTION_EXIT;
            return new EvaluateExpressionStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_JUMP)
        case TA_Base_Core::JUMP_STEP:
            FUNCTION_EXIT;
            return new JumpStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_LAUNCH_GUI_APP)
        case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            FUNCTION_EXIT;
            return new LaunchGuiAppStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)
        case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            FUNCTION_EXIT;
            return new LaunchSchematicStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_LOG)
        case TA_Base_Core::LOG_STEP:
            FUNCTION_EXIT;
            return new LogStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_PROMPT)
        case TA_Base_Core::PROMPT_STEP:
            FUNCTION_EXIT;
            return new PromptStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_RUN_PLAN)
        case TA_Base_Core::RUN_PLAN_STEP:
            FUNCTION_EXIT;
            return new RunPlanStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_SEND_SMS_MESSAGE)
        case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            FUNCTION_EXIT;
            return new SendSmsMessageStep( m_parent, stepId );
#endif

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
        case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return new StationTisStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_TERMINATE_PLAN)
        case TA_Base_Core::TERMINATE_PLAN_STEP:
            FUNCTION_EXIT;
            return new TerminatePlanStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
        case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return new TrainTisStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_VIDEOWALL)
        case TA_Base_Core::VIDEO_WALL_STEP:
            FUNCTION_EXIT;
            return new VideoWallStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_VMS_BLANKOUT)
        case TA_Base_Core::VMS_BLANKOUT_STEP:
            FUNCTION_EXIT;
            return new VmsBlankoutStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_VMS_RESTORE_SCHEDULE)
        case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            FUNCTION_EXIT;
            return new VmsRestoreScheduleStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_VMS_SET_SCENE)
        case TA_Base_Core::VMS_SET_SCENE_STEP:
            FUNCTION_EXIT;
            return new VmsSetSceneStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
        case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            FUNCTION_EXIT;
            return new StationPAStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
        case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            FUNCTION_EXIT;
            return new TrainPAStep(m_parent, stepId);
#endif

//lizettejl++ (DP-changes)
#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)
        case TA_Base_Core::FAILURE_SUMMARY_STEP:
            FUNCTION_EXIT;
            return new FailureSummaryStep(m_parent, stepId);
#endif
//++lizettejl

//WuZhongyi New Step Fire Alarm Station PA
#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM)
		case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
			FUNCTION_EXIT;
			return new StationPAByAlarmStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)
        case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
            FUNCTION_EXIT;
            return new BackgroundLaunchSchematicStep(m_parent, stepId);
#endif


#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
        case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
            FUNCTION_EXIT;
            return new BackgroundLaunchGuiAppStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_BACKGROUND_DECISION)
        case TA_Base_Core::BACKGROUND_DECISION_STEP:
            FUNCTION_EXIT;
            return new BackgroundDecisionStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_BACKGROUND_LOG)
        case TA_Base_Core::BACKGROUND_LOG_STEP:
            FUNCTION_EXIT;
            return new BackgroundLogStep(m_parent, stepId);
#endif

#if defined (STEPTYPE_BACKGROUND_PROMPT)
        case TA_Base_Core::BACKGROUND_PROMPT_STEP:
            FUNCTION_EXIT;
            return new BackgroundPromptStep(m_parent, stepId);
#endif

        case TA_Base_Core::END_STEP:
            // Only the agent creates the end step!
            TA_THROW(TA_Base_Core::PlanStepException("Attempt to create END_STEP at GUI - the agent creates this", TA_Base_Core::UNDEFINED_STEP));

        default:
            std::ostringstream errorMessage;
            errorMessage << "Unknown step type " << stepType << " for new step ID " << stepId;

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::UNDEFINED_STEP));
    }
}


void PlanStepMap::insertPlanStep(TA_Base_Core::EStep stepType, PlanStepNumber position)
{
    FUNCTION_ENTRY( "insertPlanStep" );

    // Get a step ID for the new step
    PlanStepId stepId = PlanAgentAccess::getInstance().getNextStepId();

    // Create the new step
    PlanStep* newPlanStep = createNewPlanStep(stepType, stepId);
    newPlanStep->setStepPosition(position);

    // Renumber all plan steps below the new step. We don't want to insert the new step prior to doing this
    // because its position would clash with the position of a step already in the map, i.e. getPlanSteps()
    // could not guarantee returning all steps in the correct order.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    // The step list is sorted by step position. Start at the first step affected by the addition of the new step.
    for (PlanStepList::const_iterator iter = planSteps.begin() + (position - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        // Increment the position
        planStepToRenumber->setStepPosition(planStepToRenumber->getStepPosition() + 1);
    }

    // Now add the new step to the map.
    m_planStepMap.insert(newPlanStep->getStepId(), newPlanStep);

    FUNCTION_EXIT;
}


PlanStepNumber PlanStepMap::deletePlanSteps(PlanStepList &deleteSteps)
{
    FUNCTION_ENTRY( "deletePlanSteps" );

    // Remove the specified steps, determining the position of topmost one at the same time.
    PlanStepNumber firstRemovedPosition = getLastStepPosition();
    for (PlanStepList::const_iterator iter = deleteSteps.begin(); iter != deleteSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        m_planStepMap.erase(planStep->getStepId());
        firstRemovedPosition = __min(firstRemovedPosition, planStep->getStepPosition());

        delete planStep;
        planStep = NULL;
    }
    deleteSteps.clear();

    // Renumber all plan steps following the first removed step.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    int newPosition = firstRemovedPosition;
    for (PlanStepList::const_iterator iter = planSteps.begin() + (firstRemovedPosition - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        // Adjust the position
        planStepToRenumber->setStepPosition(newPosition++);
    }

    FUNCTION_EXIT;
    return firstRemovedPosition;
}


void PlanStepMap::copyPlanSteps(const PlanStepList &copySteps, PlanStepNumber position)
{
    FUNCTION_ENTRY( "copyPlanSteps" );

    // Get step IDs for the copied steps
    std::vector<PlanStepId> stepIds;
    stepIds.reserve(copySteps.size());
    for (unsigned int i = 0; i < copySteps.size(); i++)
    {
        stepIds.push_back(PlanAgentAccess::getInstance().getNextStepId());
    }

    // Renumber all plan steps below the plan copy insertion point. We don't want to insert the copied steps
    // prior to doing this because their position would clash with the position of steps already in the map,
    // i.e. getPlanSteps() could not guarantee returning all steps in the correct order.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    for (PlanStepList::const_iterator iter = planSteps.begin() + (position - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        // Adjust the position
        planStepToRenumber->setStepPosition(planStepToRenumber->getStepPosition() + copySteps.size());
    }

    // Sort the steps being copied so that we can duplicate and insert them in the correct order.
    PlanStepList sortedCopySteps(copySteps);
    std::sort(sortedCopySteps.begin(), sortedCopySteps.end(), PlanStepComparator());

    // Clone the steps being copied
    int stepIdIdx = 0;
    int newPosition = position;
    for (PlanStepList::const_iterator iter = sortedCopySteps.begin(); iter != sortedCopySteps.end(); iter++)
    {
        PlanStep *planStep = *iter;
        PlanStep *planStepCopy = planStep->clone();
        TA_ASSERT(planStepCopy, "Failed to make copy of plan step");

        planStepCopy->setStepId(stepIds[stepIdIdx++]);
        planStepCopy->setStepPosition(newPosition++);

        // Add to the step map
        m_planStepMap.insert(planStepCopy->getStepId(), planStepCopy);
    }

    FUNCTION_EXIT;
}


PlanStepNumber PlanStepMap::movePlanSteps(const PlanStepList &moveSteps, PlanStepNumber position)
{
    FUNCTION_ENTRY( "movePlanSteps" );

    PlanStepNumber firstPosition = getLastStepPosition();  // position of the first moved step
    PlanStepNumber lastPosition = 1;                       // position of the last moved step

    unsigned long precedingTarget = 0;  // number of steps being moved that are before the target position
    unsigned long followingTarget = 0;  // number of steps being moved that are after the target position

    // Determine the positions of the first and last step being moved.
    // Determine the number of steps being moved from before and after the target position.
    for (PlanStepList::const_iterator iter = moveSteps.begin(); iter != moveSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        firstPosition = __min(firstPosition, planStep->getStepPosition());
        lastPosition = __max(lastPosition, planStep->getStepPosition());

        if (planStep->getStepPosition() < position)
        {
            precedingTarget++;
        }
        else
        {
            followingTarget++;
        }

        // Remove the moved steps from our step map so that the remaining ones can be renumbered
        m_planStepMap.erase(planStep->getStepId());
    }

    // Get the steps not being moved. They are sorted by their existing position.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    // If steps are being moved from before the target position, renumber the remaining steps
    // between the original position of the first step being moved and the target position.
    if (precedingTarget > 0)
    {
        PlanStepList::const_iterator beginIter = planSteps.begin() + (firstPosition - 1);
        PlanStepList::const_iterator endIter = planSteps.begin() + (position - precedingTarget - 1);

        PlanStepNumber newPosition = firstPosition;
        for (PlanStepList::const_iterator iter = beginIter; iter != endIter; iter++)
        {
            PlanStep *planStepToRenumber = *iter;

            planStepToRenumber->setStepPosition(newPosition++);
        }
    }

    // If steps are being moved from after the target position, renumber the remaining steps
    // between the target position and the original position of the last step being moved.
    if (followingTarget > 0)
    {
        PlanStepList::const_iterator beginIter = planSteps.begin() + (position - precedingTarget - 1);
        PlanStepList::const_iterator endIter = planSteps.begin() + (lastPosition - moveSteps.size());

        PlanStepNumber newPosition = position + followingTarget;
        for (PlanStepList::const_iterator iter = beginIter; iter != endIter; iter++)
        {
            PlanStep *planStepToRenumber = *iter;

            planStepToRenumber->setStepPosition(newPosition++);
        }
    }

    // Sort the steps being moved so that we can reassign their positions in the correct order.
    PlanStepList sortedMoveSteps(moveSteps);
    std::sort(sortedMoveSteps.begin(), sortedMoveSteps.end(), PlanStepComparator());

    PlanStepNumber newPosition = position - precedingTarget;
    for (PlanStepList::const_iterator iter = sortedMoveSteps.begin(); iter != sortedMoveSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        planStep->setStepPosition(newPosition++);

        // Add back to the step map
        m_planStepMap.insert(planStep->getStepId(), planStep);
    }

    FUNCTION_EXIT;
    return (position - precedingTarget);
}


PlanStepNumber PlanStepMap::getLastStepPosition()
{
    FUNCTION_ENTRY( "getLastStepPosition" );
    FUNCTION_EXIT;
    return m_planStepMap.size();
}


