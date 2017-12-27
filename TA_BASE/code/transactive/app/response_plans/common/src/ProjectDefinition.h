//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Mahaveer Pareek
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// This file contains project-specific preprocessor definitions used 
// for conditional compilation of Plan Agent and Plan Manager.
//
// Each project should uncomment the relevant PROJECT_XXXX definition.
//
// STEPTYPE_* definitions control the types of plan step that are 
// supported within each project.
//

#ifndef PROJECT_DEFINITION_HEADER_INCLUDED
#define PROJECT_DEFINITION_HEADER_INCLUDED


// ----------------------------------------------
// Project definitions - uncomment as appropriate
// ----------------------------------------------
//#define  PROJECT_4669
//#define  PROJECT_3001
//#define  PROJECT_GF
#define  PROJECT_GZL6
// ----------------------------------------------


//Comment out the definition of DEVELOPMENT_BUILD for RELEASE BUILD
//#define DEVELOPMENT_BUILD


//Steps included for Project 4669
#if defined ( PROJECT_4669 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LAUNCH_SCHEMATIC
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN
	#define STEPTYPE_EVALUATE_EXPRESSION
	//#define STEPTYPE_FAILURE_SUMMARY_STEP

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_GRC
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR    
    #define STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_STATION_PUBLIC_ADDRESS
    #define STEPTYPE_TRAIN_PUBLIC_ADDRESS
#endif

//Steps included for Project 3001
#if defined ( PROJECT_3001 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LAUNCH_SCHEMATIC
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN
    #define STEPTYPE_EVALUATE_EXPRESSION
    #define STEPTYPE_PLAN_CHECK_TIMER
    #define STEPTYPE_PLAN_SET_TIMER
    #define STEPTYPE_EXECUTE_SCRIPT
    #define STEPTYPE_DATA_POINT_TIME_CHECK
    #define STEPTYPE_POPUP_MFT_MESSAGE
    #define STEPTYPE_SHUTDOWN_SYSTEM
    //#define STEPTYPE_FAILURE_SUMMARY_STEP

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_GRC
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR    
    #define STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_STATION_PUBLIC_ADDRESS
    #define STEPTYPE_TRAIN_PUBLIC_ADDRESS
#endif

//Steps included for Project GZL6
#if defined ( PROJECT_GZL6 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LAUNCH_SCHEMATIC
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN
	#define STEPTYPE_EVALUATE_EXPRESSION
	//#define STEPTYPE_FAILURE_SUMMARY_STEP
	#define STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC
    #define STEPTYPE_BACKGROUND_LAUNCH_GUI_APP
    //#define STEPTYPE_DOWNLOAD_BAS_TIMETABLE

    #define STEPTYPE_BACKGROUND_LOG
    #define STEPTYPE_BACKGROUND_PROMPT
    #define STEPTYPE_BACKGROUND_DECISION

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_GRC
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR    
    #define STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
    //#define STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_STATION_PUBLIC_ADDRESS
    //#define STEPTYPE_TRAIN_PUBLIC_ADDRESS
	//WuZhongyi New Step Fire Alarm Station PA
	#define STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM
#endif


#endif // PROJECT_DEFINITION_HEADER_INCLUDED
