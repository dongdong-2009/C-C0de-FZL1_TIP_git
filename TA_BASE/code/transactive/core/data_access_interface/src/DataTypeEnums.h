#if !defined(DataTypeEnums_H_INLCUDED)
#define DataTypeEnums_H_INLCUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source:  $
  * @author:  Robert Stagg
  * @version: $Revision: #4 $
  *
  * Last modification: $Date: 2017/08/24 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This file is generated automagicly from Database_Directory.xls
  * Version 2.01
  * Do NOT edit it
  *
  */


namespace TA_Base_Core
{
    enum EDataTypes
    {
        Event_Ad,
        Sessions_Ad,
        Statistics_Ad,
        Trend_Ad,
        Incident_Ad,
        Common_Ad,
        Alarm_Sd,
        CallStack_Sd,
        TisRatis_Sd,
        Version_Sd,
        Alarms_Cd,
        AlarmsLocal_Cd,
        Application_Cd,
        Archiving_Cd,
        Ats_Cd,
        Dr_Cd,
        Ecs_Cd,
        Incident_Cd,
        MmsScheduling_Cd,
        OfflineSecurity_Cd,
        OnlineSecurity_Cd,
        OnlineUpdatable_Cd,
        Pa_Cd,
        Plans_Cd,
        Radio_Ad,
        Radio_Cd,
        Scada_Cd,
        Scheduling_Cd,
        Sound_Cd,
        SysController_Cd,
        Telephone_Cd,
        Tis_Cd,
        TisDisplayRequestLogs,
        Video_Cd,
        Pids_Cd,
        Signalling_Cd,
        AlarmRules_Cd,
        GlobalParameter_Cd,
        Tis_OCC_Cd,
        Archive_d,
        Naming_Cd,
		MMSRequest,
		BasTimetable_Cd,
		//Gena_Cd,
		// for new added item should put above this item		
		NotExists_d,  // for handling data type in csv file can not found in here  
        MaxDataTypes
    };

    enum EDataActions
    {
        Read,
        Write,
        MaxDataActions
    };

} // namespace

#endif
