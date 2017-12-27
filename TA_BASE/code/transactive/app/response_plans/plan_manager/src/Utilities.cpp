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
  * This is a utility class. It provides a set of general purpose methods,
  * mainly for string conversion.
  *
  **/

#include "stdafx.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/uuid/src/TAuuid.h"
#include "PlanManager.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// Utilities

std::string Utilities::getCurrentSessionId()
{
    FUNCTION_ENTRY( "getCurrentSessionId" );
    FUNCTION_EXIT;
    return TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
}


std::string Utilities::getPlanManagerId()
{
    FUNCTION_ENTRY( "getPlanManagerId" );

    static const std::string PLAN_MANAGER_ID = TA_Base_Core::TA_Uuid::generateUUID();

    FUNCTION_EXIT;
    return PLAN_MANAGER_ID;
}


std::string Utilities::convertToString(const TA_Base_Core::Seconds delay)
{
    FUNCTION_ENTRY( "convertToString" );

    time_t minutes = delay / 60;
    time_t seconds = delay % 60;

    std::ostringstream minAndSec;
    minAndSec << std::setw(2) << std::setfill('0') << minutes << ':' << std::setw(2) << std::setfill('0') << seconds;

    FUNCTION_EXIT;
    return minAndSec.str();
}


std::string Utilities::convertToString(const TA_Base_Core::EApprovalState appState)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (appState)
    {
        case TA_Base_Core::APPROVED_APPROVAL_STATE:
            FUNCTION_EXIT;
            return getString(IDS_PLAN_APPROVED_STATUS, "Approved");

        case TA_Base_Core::UNAPPROVED_APPROVAL_STATE:
            FUNCTION_EXIT;
            return getString(IDS_PLAN_UNAPPROVED_STATUS, "Unapproved");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_PLAN_UNDEFINED_APPROVAL_STATUS, "Undefined");
}


std::string Utilities::convertToString(const TA_Base_Core::EStep stepType)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (stepType)
    {
        case TA_Base_Core::ABORT_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_ABORT, "Abort");

        case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_ACTIVATE_CAMERA_PRESET, "Activate Camera Preset");

        case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_ACTIVE_PLAN_CHECK, "Active Plan Check");

        case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_ASSIGN_CAMERA_TO_MONITOR, "Assign Camera To Monitor");

        case TA_Base_Core::DATA_POINT_CHECK_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_DATA_POINT_CHECK, "Data Point Check");

        case TA_Base_Core::DATA_POINT_SET_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_DATA_POINT_SET, "Set Data Point");

        case TA_Base_Core::DECISION_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_DECISION, "Decision");

        case TA_Base_Core::END_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_END, "End Step");

          case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_EVALUATE_EXPRESSION, "Evaluate Expression");

        case TA_Base_Core::JUMP_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_JUMP, "Jump");

        case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_LAUNCH_GUI_APP, "Launch GUI Application");

        case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_LAUNCH_SCHEMATIC, "Launch Schematic");

        case TA_Base_Core::LOG_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_LOG, "Log");

        case TA_Base_Core::PROMPT_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_PROMPT, "Prompt");

        case TA_Base_Core::RUN_PLAN_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_RUN_PLAN, "Run Plan");

        case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_SEND_SMS_MESSAGE, "Send SMS Message" );

        case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_STATION_TRAVELLER_INFO_SYSTEM, "Station Traveller Info System" );

        case TA_Base_Core::TERMINATE_PLAN_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_TERMINATE_PLAN, "Terminate Plan");

        case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_TRAIN_TRAVELLER_INFO_SYSTEM, "Train Traveller Info System" );

        case TA_Base_Core::VIDEO_WALL_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_VIDEO_WALL, "Assign Layout To Wallboard");

        case TA_Base_Core::VMS_BLANKOUT_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_VMS_BLANKOUT, "VMS Blankout");

        case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_VMS_RESTORE_SCHEDULE, "VMS Restore Schedule");

        case TA_Base_Core::VMS_SET_SCENE_STEP:
            FUNCTION_EXIT;
            return getString(IDS_STEP_TYPE_VMS_SET_SCENE, "VMS Set Scene");

          case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
             FUNCTION_EXIT;
             return getString(IDS_STEP_TYPE_STATION_PUBLIC_ADDRESS, "Station PA");

          case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
             FUNCTION_EXIT;
             return getString(IDS_STEP_TYPE_TRAIN_PUBLIC_ADDRESS, "Train PA");

          case TA_Base_Core::FAILURE_SUMMARY_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_TYPE_FAILURE_SUMMARY, "Failure Summary");
		
		  case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
			  FUNCTION_EXIT;
			  return getString(IDS_STEP_TYPE_STATION_PA_BY_ALARM, "Station PA By Alarm");

          case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_BACKGROUND_LAUNCH_SCHEMATIC_STEP, "Background Launch Schematic Step");

          case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_BACKGROUND_LAUNCH_GUI_APP_STEP, "Background Launch Gui App Step");

          case TA_Base_Core::BACKGROUND_DECISION_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_BACKGROUND_DECISION_STEP, "Background Decision Step");

          case TA_Base_Core::BACKGROUND_LOG_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_BACKGROUND_LOG_STEP, "Background Log Step");

          case TA_Base_Core::BACKGROUND_PROMPT_STEP:
              FUNCTION_EXIT;
              return getString(IDS_STEP_BACKGROUND_PROMPT_STEP, "Background Prompt Step");
        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_STEP_TYPE_UNDEFINED, "Undefined step type");
}


//lizettejl++ (DP-changes: Failure Summary)
std::string Utilities::convertToString(const TA_Base_Core::EStepFailureCategory failureCategory)
{
    FUNCTION_ENTRY( "convertToString" );

    switch(failureCategory)
    {
    case TA_Base_Core::SFC_ALL_STEPS:
        FUNCTION_EXIT;
        return getString(IDS_FAILURE_CATEGORY_ALL_STEPS, "All step failures");
    case TA_Base_Core::SFC_SET_DATA_POINT_STEPS:
        FUNCTION_EXIT;
        return getString(IDS_FAILURE_CATEGORY_DATAPOINT_STEPS, "BRC Step");
    default:
        ;
    }

    FUNCTION_EXIT;
    return getString(IDS_FAILURE_CATEGORY_UNDEFINED, "Undefined failure category");
}


//++lizettejl

std::string Utilities::convertToString(const TA_Base_Core::EDataPointTest testType)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (testType)
    {
        case TA_Base_Core::EQUAL_TO_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_EQUAL, "Equal");

        case TA_Base_Core::NOT_EQUAL_TO_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_NOT_EQUAL, "Not Equal");

        case TA_Base_Core::LESS_THAN_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_LESS, "Less Than");

        case TA_Base_Core::GREATER_THAN_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_GREATER, "Greater Than");

        case TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_LESS_OR_EQUAL, "Less Than or Equal");

        case TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST:
            FUNCTION_EXIT;
            return getString(IDS_DP_TEST_TYPE_GREATER_OR_EQUAL, "Greater Than or Equal");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_DP_TEST_TYPE_UNDEFINED, "Undefined test type");
}


std::string Utilities::convertToString(const TA_Base_Core::ETrainGroup trainGroup)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (trainGroup)
    {
        case TA_Base_Core::TG_INBOUND_TRAINS:
            FUNCTION_EXIT;
            return getString(IDS_TRAIN_GROUP_INBOUND, "Inbound trains");

        case TA_Base_Core::TG_OUTBOUND_TRAINS:
            FUNCTION_EXIT;
            return getString(IDS_TRAIN_GROUP_OUTBOUND, "Outbound trains");

        case TA_Base_Core::TG_ALL_TRAINS:
            FUNCTION_EXIT;
            return getString(IDS_TRAIN_GROUP_ALL, "All trains");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_TRAIN_GROUP_UNDEFINED, "Undefined train group");
}


std::string Utilities::convertToString(const TA_Base_Core::ELibrarySection librarySection)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (librarySection)
    {
        case TA_Base_Core::EMERGENCY_SECTION:
            FUNCTION_EXIT;
            return getString(IDS_TIS_LIBRARY_SECTION_EMERGENCY, "Emergency");

        case TA_Base_Core::NORMAL_SECTION:
            FUNCTION_EXIT;
            return getString(IDS_TIS_LIBRARY_SECTION_NORMAL, "Normal");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_TIS_LIBRARY_SECTION_UNDEFINED, "Undefined");
}


std::string Utilities::convertToString(const DataPointConfigAccess::EDataPointType dataPointType)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (dataPointType)
    {
        case DataPointConfigAccess::DPT_UNSIGNED_INT16:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_UINT16, "16-bit unsigned integer");

        case DataPointConfigAccess::DPT_SIGNED_INT16_2C:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_INT16_2C, "16-bit signed integer (2's complement)");

        case DataPointConfigAccess::DPT_SIGNED_INT16_SM:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_INT16_SM, "16-bit signed integer (sign-magnitude)");

        case DataPointConfigAccess::DPT_UNSIGNED_INT32:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_UINT32, "32-bit unsigned integer");

        case DataPointConfigAccess::DPT_SIGNED_INT32_2C:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_INT32_2C, "32-bit signed integer (2's complement)");

        case DataPointConfigAccess::DPT_SIGNED_INT32_SM:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_INT32_SM, "32-bit signed integer (sign-magnitude)");

        case DataPointConfigAccess::DPT_IEEE_FLOAT32:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_IEEE32, "IEEE single-precision decimal");

        case DataPointConfigAccess::DPT_BOOLEAN:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_BOOL, "Boolean");

        case DataPointConfigAccess::DPT_TEXT:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_TEXT, "Text");

        case DataPointConfigAccess::DPT_DERIVED_IN:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_DERIVED_IN, "Derived Input");

        case DataPointConfigAccess::DPT_COMBINED_OUT:
            FUNCTION_EXIT;
            return getString(IDS_DP_DATA_TYPE_COMBINED_OUT, "Combined Output");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_DP_DATA_TYPE_UNDEFINED, "Undefined data type");
}


std::string Utilities::convertToString(const TisMessageStepParameter::EMessageType messageType)
{
    FUNCTION_ENTRY( "convertToString" );

    switch (messageType)
    {
        case TisMessageStepParameter::MT_STATION:
            FUNCTION_EXIT;
            return getString(IDS_TIS_MESSAGE_TYPE_STATION, "Station");

        case TisMessageStepParameter::MT_TRAIN:
            FUNCTION_EXIT;
            return getString(IDS_TIS_MESSAGE_TYPE_TRAIN, "Train");

        default:
            ;
    }

    FUNCTION_EXIT;
    return getString(IDS_TIS_MESSAGE_TYPE_UNDEFINED, "Undefined");
}


void Utilities::convertFromString(std::string str, double& value, unsigned long& precision)
{
    FUNCTION_ENTRY( "convertFromString" );

    value = atof(str.c_str());

    std::string::size_type decPointPos = str.find_last_of('.');
    if (decPointPos == std::string::npos)
    {
        precision = 0;
    }
    else
    {
        std::string::size_type lastDigitPos = str.find_last_not_of(" \t");
        precision = lastDigitPos - decPointPos;
    }

    FUNCTION_EXIT;
}


void Utilities::convertFromString(std::string str, bool &value)
{
    FUNCTION_ENTRY( "convertFromString" );

    value = (str != "= 0" && str[0] != 'f' && str[0] != 'F' && str.compare("ผู") != 0 );

    FUNCTION_EXIT;
}


void TA_Base_App::Utilities::convertFromString( std::string strSrc, int& nValue )
{
    FUNCTION_ENTRY( "convertFromString" );

    std::stringstream stmConvertor;
    stmConvertor << strSrc;
    stmConvertor >> nValue;

    FUNCTION_EXIT;
}

bool Utilities::isPlanManagerConfigurationMode()
{
    FUNCTION_ENTRY( "isPlanManagerConfigurationMode" );

    static const char* const PLAN_MANAGER_SYSTEM_MODE_STRING = "Admin";
    static const bool bPlanManagerMode = TA_Base_Core::RunParams::getInstance().isSet(PLAN_MANAGER_SYSTEM_MODE_STRING);

    FUNCTION_EXIT;
    return bPlanManagerMode;
}

// Return:  0 if updateId1 is same as updateId2
//         -1 if updateId1 is less recent than updateId2
//         +1 if updateId1 is more recent than updateId2
int Utilities::compareUpdateIds(const TA_Base_Core::UpdateId updateId1, const TA_Base_Core::UpdateId updateId2)
{
    FUNCTION_ENTRY( "compareUpdateIds" );

    static const double WRAPAROUND_GAP = (TA_Base_Core::MAX_UPDATE_ID - TA_Base_Core::MIN_UPDATE_ID) / 2;

    if (updateId2 == updateId1)
    {
        FUNCTION_EXIT;
        return 0;
    }

    // If update ID has wrapped around
    if (::fabs((double) updateId2 - (double) updateId1) > WRAPAROUND_GAP)
    {
        FUNCTION_EXIT;
        return (updateId1 < updateId2? 1 : -1);
    }

    FUNCTION_EXIT;
    return (updateId1 > updateId2? 1 : -1);
}


std::string Utilities::getString(UINT stringResourceID, std::string defaultString)
{
    FUNCTION_ENTRY( "getString" );

    CString string;

    if (string.LoadString(stringResourceID) == 0)
    {
        FUNCTION_EXIT;
        return defaultString;
    }

    FUNCTION_EXIT;
    return string.operator LPCSTR();
}
