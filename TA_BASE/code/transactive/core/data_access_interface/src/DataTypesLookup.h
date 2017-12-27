#if !defined(DataTypeLookups_H_INLCUDED)
#define DataTypeLookups_H_INLCUDED

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

#include <string>
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/utilities/src/TAAssert.h"

#ifdef __WIN32__
#pragma warning(disable:4715)  // not all control paths return a value
#endif


namespace TA_Base_Core
{
    EDataTypes getDataTypesEnum( const std::string& str )
    {
        if ( str.compare( "Event_Ad" ) == 0 ) { return Event_Ad; }
        if ( str.compare( "Sessions_Ad" ) == 0 ) { return Sessions_Ad; }
        if ( str.compare( "Statistics_Ad" ) == 0 ) { return Statistics_Ad; }
        if ( str.compare( "Trend_Ad" ) == 0 ) { return Trend_Ad; }
        if ( str.compare( "Incident_Ad" ) == 0 ) { return Incident_Ad; }
        if ( str.compare( "Common_Ad" ) == 0 ) { return Common_Ad; }
        if ( str.compare( "Alarm_Sd" ) == 0 ) { return Alarm_Sd; }
        if ( str.compare( "CallStack_Sd" ) == 0 ) { return CallStack_Sd; }
        if ( str.compare( "TisRatis_Sd" ) == 0 ) { return TisRatis_Sd; }
        if ( str.compare( "Version_Sd" ) == 0 ) { return Version_Sd; }
        if ( str.compare( "Alarms_Cd" ) == 0 ) { return Alarms_Cd; }
        if ( str.compare( "AlarmsLocal_Cd" ) == 0 ) { return AlarmsLocal_Cd; }
        if ( str.compare( "Application_Cd" ) == 0 ) { return Application_Cd; }
        if ( str.compare( "Archiving_Cd" ) == 0 ) { return Archiving_Cd; }
        if ( str.compare( "Ats_Cd" ) == 0 ) { return Ats_Cd; }
        if ( str.compare( "Dr_Cd" ) == 0 ) { return Dr_Cd; }
        if ( str.compare( "Ecs_Cd" ) == 0 ) { return Ecs_Cd; }
        if ( str.compare( "Incident_Cd" ) == 0 ) { return Incident_Cd; }
        if ( str.compare( "MmsScheduling_Cd" ) == 0 ) { return MmsScheduling_Cd; }
        if ( str.compare( "OfflineSecurity_Cd" ) == 0 ) { return OfflineSecurity_Cd; }
        if ( str.compare( "OnlineSecurity_Cd" ) == 0 ) { return OnlineSecurity_Cd; }
        if ( str.compare( "OnlineUpdatable_Cd" ) == 0 ) { return OnlineUpdatable_Cd; }
        if ( str.compare( "Pa_Cd" ) == 0 ) { return Pa_Cd; }
        if ( str.compare( "Plans_Cd" ) == 0 ) { return Plans_Cd; }
        if ( str.compare( "Radio_Ad" ) == 0 ) { return Radio_Ad; }
        if ( str.compare( "Radio_Cd" ) == 0 ) { return Radio_Cd; }
        if ( str.compare( "Scada_Cd" ) == 0 ) { return Scada_Cd; }
        if ( str.compare( "Scheduling_Cd" ) == 0 ) { return Scheduling_Cd; }
        if ( str.compare( "Sound_Cd" ) == 0 ) { return Sound_Cd; }
        if ( str.compare( "SysController_Cd" ) == 0 ) { return SysController_Cd; }
        if ( str.compare( "Telephone_Cd" ) == 0 ) { return Telephone_Cd; }
        if ( str.compare( "Tis_Cd" ) == 0 ) { return Tis_Cd; }
        if ( str.compare( "TisDisplayRequestLogs" ) == 0 ) { return TisDisplayRequestLogs; }
        if ( str.compare( "Video_Cd" ) == 0 ) { return Video_Cd; }
        if ( str.compare( "Pids_Cd" ) == 0 ) { return Pids_Cd; }
        if ( str.compare( "Signalling_Cd" ) == 0 ) { return Signalling_Cd; }
        if ( str.compare( "AlarmRules_Cd" ) == 0 ) { return AlarmRules_Cd; }
        if ( str.compare( "GlobalParameter_Cd" ) == 0 ) { return GlobalParameter_Cd; }
        if ( str.compare( "Tis_OCC_Cd" ) == 0 ) { return Tis_OCC_Cd; }
        if ( str.compare( "Archive_d" ) == 0 ) { return Archive_d; }
        if ( str.compare( "Naming_Cd" ) == 0 ) { return Naming_Cd; }
		if ( str.compare("MmsRequest" ) == 0 ) { return MMSRequest;}
		if ( str.compare("BasTimetable_Cd" ) == 0 ) { return BasTimetable_Cd;}
		//if ( str.compare("Gena_Cd" ) == 0 ) { return Gena_Cd;}
		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"Unknown enum DataType name: %s ", str.c_str());
		return NotExists_d;
 //       TA_ASSERT(false, "Unknown enum name");
    }

    EDataActions getDataActionsEnum( const std::string& str )
    {
        if ( str.compare( "Read" ) == 0 ) { return Read; }
        if ( str.compare( "Write" ) == 0 ) { return Write; }
        TA_ASSERT(false, "Unknown enum name");
    }

} // namespace

#endif
