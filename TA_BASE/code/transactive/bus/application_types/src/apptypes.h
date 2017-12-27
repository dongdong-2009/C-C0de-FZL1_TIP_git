/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/application_types/src/apptypes.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/08/31 17:23:36 $
  * Last modified by:  $Author: yong.liu $
  *
  */
#ifndef APP_TYPES_H
#define APP_TYPES_H


enum AppTypes
{
    ADMIN_GUI_APPTYPE                               =  1,
    ALARM_GUI_APPTYPE                               =  2,
    CCTV_GUI_APPTYPE                                =  3,
    CONTROLROOM_GUI_APPTYPE,                      //=  4,
    EVENT_GUI_APPTYPE,                            //=  5,
    HISTORIAN_GUI_APPTYPE,                        //=  6,
    MAP_GUI_APPTYPE,                              //=  7,
    NETMON_GUI_APPTYPE,                           //=  8,
    OPLOGS_GUI_APPTYPE,                           //=  9,
    SIGNS_GUI_APPTYPE,                            //= 10,
    VIDIWALL_GUI_APPTYPE,                         //= 11,
    IRPLANS_GUI_APPTYPE,                          //= 12,
    AREAALLOC_GUI_APPTYPE,                        //= 13,
    GFF_GUI_APPTYPE,                              //= 14,
    IRPCONFIG_GUI_APPTYPE,                        //= 15,
    BACKUP_GUI_APPTYPE,                           //= 16,
    SIGN_VIEWER_GUI_APPTYPE,                      //= 17,
    SCENES_EDITOR_GUI_APPTYPE,                    //= 18,
    ALARM_SCHED_GUI_APPTYPE,                      //= 19,
    SEND_EMSERVMESG_GUI_APPTYPE                     = 22,
    TREND_VIEWER_GUI_APPTYPE                        = 23,
    DATA_POINT_PANEL_GUI_APPTYPE                    = 24,
    OPC_BRIDGE_APPTYPE                              = 27,
    PLAN_MANAGER_GUI_APPTYPE                        = 29,
    BANNER_GUI_APPTYPE                              = 30,
    INSPECTOR_PANEL_GUI_APPTYPE                     = 31,
    RADIO_CONTROLLER_GUI_APPTYPE                    = 32,
    CONFIG_EDITOR_GUI_APPTYPE                       = 33,
    TELEPHONE_CONTROLLER_GUI_APPTYPE                = 34,
    STIS_MANAGER_GUI_APPTYPE                        = 35,
    PLAN_SCHEDULER_GUI_APPTYPE                      = 36,
    DUTY_MANAGER_GUI_APPTYPE                        = 37,
    TIS_LOG_VIEWER_GUI_APPTYPE                      = 38,
    TTIS_MANAGER_GUI_APPTYPE                        = 39,
    ECS_STATION_MODES_GUI_APPTYPE                   = 40,				
    ECS_STATION_MODES_OVERVIEW_GUI_APPTYPE          = 41,				
    ECS_NORMAL_MASTER_MODES_GUI_APPTYPE             = 42,				
    ECS_EMERGENCY_MASTER_MODES_GUI_APPTYPE          = 43,
    ECS_EMERGENCY_MASTER_MODES_FOR_ZONE_GUI_APPTYPE = 44,
    ATS_ECS_AGENT_SIMULATOR_GUI_APPTYPE             = 45,
    EQUIP_STATUS_VIEWER_GUI_APPTYPE                 = 46,
    PA_MANAGER_GUI_APPTYPE                          = 47,
    REPORT_MANAGER_GUI_APPTYPE	                    = 48,
    ARCHIVE_MANAGER_APPTYPE                         = 49,
    INCIDENT_MANAGER_GUI_APPTYPE                    = 50,
    ECS_CONGESTION_PARAMETERS_APPTYPE               = 51,
    ECS_STATION_TIMETABLE_APPTYPE                   = 52,
    JOB_REQUEST_MANAGER                             = 53,
    CONTROL_STATION                                 = 54,
	MMS_CONTROLLER_GUI_APPTYPE						= 56,
    SCHEDULING_MANAGER                              = 57,
    REGION_MANAGER_GUI_APPTYPE                      = 58,
    INTEGRATED_ALARM_GUI_APPTYPE                    = 59,
    DVR_GUI_APPTYPE                                 = 60,
    ECS_OVERALL_TIMETABLE_SETTING_GUI_APPTYPE       = 61,
    TRAIN_PA_MANAGER_GUI_APPTYPE                    = 63,
    TRAIN_PID_MANAGER_GUI_APPTYPE                   = 64,
	TELEPHONE_MANAGER_GUI_APPTYPE                   = 65,
    PID_MANAGER_GUI_APPTYPE                         = 75,
    RADIO_MANAGER_GUI_APPTYPE                       = 76,
    INTERNET_EXPLORER_APPTYPE                       = 77,
	PID_LOG_VIEWER_GUI_APPTYPE                      = 78,
	POWER_DEMAND_EDITOR_GUI_APPTYPE					= 79, //14000
	TRN_MANAGER_GUI_APPTYPE							= 80,	
    TRAIN_MANAGER_GUI_APPTYPE					    = 81,
	HIERARCHICAL_ALARM_VIEWER_GUI_APPTYPE           = 82,
	PID_PREVIEW_APPTYPE								= 83, //TD17160
	DLL_HOST_GUI_APPTYPE							= 84,
	BAS_MODE_MANAGER_GUI_APPTYPE					= 150,
	BAS_TIMETABLE_MANAGER_GUI_APPTYPE				= 151,
	BAS_PARAMETER_MANAGER_GUI_APPTYPE				= 152,
	BAS_TUNNEL_SELECTION_MANAGER_GUI_APPTYPE		= 153,
	BAS_TUNNEL_FIRE_MANAGER_GUI_APPTYPE		        = 154,
	BAS_TUNNEL_CONGESTION_MANAGER_GUI_APPTYPE       = 157,
	BAS_EMERGENCY_MODE_MANAGER                      = 158,
	WF_INHIBITOR_GUI_APPTYPE						= 160,
	SOE_EVENT_VIEWER                                = 101,
	TRAIN_TIMETABLE_VIEWER							= 102
};

#endif
