#ifndef PA_TABLE_CONSTS__INCLUDED_
#define PA_TABLE_CONSTS__INCLUDED_

#include <string>

namespace TA_Base_Core
{
// Common column names
const std::string KEY_COL = "PKEY";
const std::string LOCATION_KEY_COL = "LOCATION_KEY";

const std::string DATE_MODIFIED_COL = "DATE_MODIFIED";
const std::string DATE_CREATED_COL = "DATE_CREATED";

// Table PA_DVA_MESSAGE
const std::string PA_DVA_MESSAGE_TABLE = "PA_DVA_MESSAGE";

const std::string DVA_MSG_TABLE_KEY_COL				= "PA_DVA_MESSAGE.PADMES_ID";
const std::string DVA_MSG_TABLE_LOCATIONKEY_COL		= "PA_DVA_MESSAGE.LOCATIONKEY";
const std::string DVA_MSG_TABLE_ID_COL				= "PA_DVA_MESSAGE.ID";
const std::string DVA_MSG_TABLE_LABEL_COL			= "PA_DVA_MESSAGE.LABEL";
const std::string DVA_MSG_TABLE_TYPE_COL			= "PA_DVA_MESSAGE.TYPE";
const std::string DVA_MSG_TABLE_DESCRIPTION_COL		= "PA_DVA_MESSAGE.MSG_DESCRIPTION";
const std::string DVA_MSG_TABLE_DATEMODIFIED_COL	= "PA_DVA_MESSAGE.DATE_MODIFIED";
const std::string DVA_MSG_TABLE_DATECREATED_COL		= "PA_DVA_MESSAGE.DATE_CREATED";


const std::string PA_DVA_MESSAGE_SEQUENCE_COUNTER = "PADMES_SEQ";

const std::string MSG_ID_COL = "ID";
const std::string MSG_NAME_COL = "LABEL";
const std::string MSG_DESCRIPTION_COL = "MSG_DESCRIPTION";
const std::string MSG_TYPE_COL = "TYPE";

const std::string LOCATION_KEY = "Location Key";
const std::string MSG_ID  = "Msg ZONE_ID";
const std::string MSG_NAME = "Msg Name";
const std::string MSG_DESCRIPTION = "Msg Description";
const std::string MSG_TYPE = "Msg Type";

const unsigned short EMERGENCY_DVA_MSG_TYPE = 1;
const unsigned short NORMAL_DVA_MSG_TYPE = 2;
const unsigned short OTHER_DVA_MSG_TYPE = 3;

// Table PA_BROADCAST_SHORTCUT
const std::string PA_BROADCAST_SHORTCUT_TABLE = "PA_DVA_MSG_SHORTCUT_CONFIG";

const std::string PA_DVA_BROADCAST_SHORTCUT_SEQUENCE_COUNTER = "PA_DVA_MSG_SHORTCUT_SEQ";

const std::string SHORTCUT_NO_COL = "SHORTCUT_NUMBER";
const std::string DVA_MSG_KEY_COL = "DVA_MSG_KEY";

const std::string SHORTCUT_NO = "Shortcut No";
const std::string DVA_MSG_KEY = "DVA Msg";

// Table PA_ZONE_HELPER
const std::string PA_ZONE_HELPER_TABLE = "PA_ZONE_HELPER";
const std::string PA_ZONE_HELPER_SEQUENCE_COUNTER = "PAZHEL_SEQ";
const std::string PA_ZONE_HELPER_TABLE_KEY_COL = "PAZHEL_ID";
const std::string PA_ZONE_HELPER_TABLE_PID_EQUIPMENT_COL = "PID_ZONE_EQUIPMENT";
const std::string PA_ZONE_HELPER_TABLE_ZONE_KEY_COL = "PAZONE_ID";

// Table PA_ZONE
const std::string PA_ZONE_TABLE = "PA_ZONE";
const std::string PA_ZONE_SEQUENCE_COUNTER = "PAZONE_SEQ";

const std::string PA_ZONE_TABLE_KEY_COL                     = "PAZONE_ID";
const std::string PA_ZONE_TABLE_LOCATION_KEY_COL            = "LOCATIONKEY";
const std::string PA_ZONE_TABLE_ID_COL                      = "ID";
const std::string PA_ZONE_TABLE_LABEL_COL                   = "LABEL";
const std::string PA_ZONE_TABLE_STN_EQUIPMENT_NAME_COL      = "STN_EQUIPMENT";
const std::string PA_ZONE_TABLE_OCC_EQUIPMENT_NAME_COL      = "OCC_EQUIPMENT";

const std::string ZONE_ID = "Zone ID";
const std::string ZONE_NAME = "Zone name";
const std::string STN_EQUIP_ENTITY_NAME = "Station Equipment Name";
const std::string OCC_EQUIP_ENTITY_NAME = "OCC Equipment Name";
const std::string EQUIPMENT = "Equipment Entity Associations";

// Table PA_ZONE_GROUP_HELP
const std::string PA_ZONE_GROUP_HELPER_TABLE = "PA_ZONE_GROUP_HELPER";
const std::string PA_ZONE_GROUP_HELPER_SEQUENCE_COUNTER = "PAZGHE_SEQ";

const std::string PA_ZONE_GROUP_HELPER_TABLE_KEY_COL                = "PA_ZONE_GROUP_HELPER.PAZGHE_ID";
const std::string PA_ZONE_GROUP_HELPER_TABLE_PA_ZONE_GROUP_KEY_COL  = "PA_ZONE_GROUP_HELPER.PAZGRO_ID";
const std::string PA_ZONE_GROUP_HELPER_TABLE_PA_ZONE_KEY_COL        = "PA_ZONE_GROUP_HELPER.PAZONE_ID";

// Table PA_ZONE_GROUP
const std::string PA_ZONE_GROUP_TABLE = "PA_ZONE_GROUP";
const std::string PA_ZONE_GROUP_SEQUENCE_COUNTER = "PAZGRO_SEQ";

const std::string PA_ZONE_GROUP_TABLE_KEY_COL = "PAZGRO_ID";
const std::string PA_ZONE_GROUP_TABLE_LOCATION_KEY_COL = "LOCATIONKEY";
const std::string PA_ZONE_GROUP_TABLE_LABEL_COL = "LABEL";
const std::string PA_ZONE_GROUP_TABLE_IS_EVENT_GROUP_COL = "IS_EVENT_GROUP";

// Table PA_STATION
const std::string PA_STATION_TABLE = "PA_STATION";
const std::string PA_STATION_TABLE_KEY_COL = "PASTAT_ID";
const std::string PA_STATION_TABLE_SCHEMATIC_FILE_NAME_COL = "SCHEMATIC_FILE_NAME";
const std::string PA_STATION_TABLE_LOCATION_KEY_COL = "LOCATIONKEY";

// Table PA_PRIORITY_GROUP
const std::string PA_PRIORITY_GROUP_TABLE = "PA_PRIORITY_GROUP";
const std::string PA_PRIORITY_GROUP_KEY_COL = "PA_PRIORITY_GROUP.PKEY";
const std::string PA_PRIORITY_GROUP_PA_PRIORITY_ID_COL = "PA_PRIORITY_GROUP.PA_PRIORITY_ID";
const std::string PA_PRIORITY_GROUP_PA_PRIORITY_NAME_COL = "PA_PRIORITY_GROUP.PA_PRIORITY_NAME";

// Table PA_SIG_SETTING
const std::string PA_SIG_SETTING_TABLE = "PA_SIG_SETTING";

const std::string ENABLE_STATUS_COL = "ENABLE_STATUS";
const std::string DELAY_TIME_COL = "DELAY_TIME";

const std::string PA_SIG_SETTING_SEQUENCE_COUNTER = "PA_SIG_SETTING_SEQ";

// Table PA_SCHEDULE_BROADCAST
const std::string PA_SCHEDULE_BROADCAST_TABLE = "PA_SCHEDULE_BROADCAST";

const std::string PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL = "SCHEDULE_NAME";
const std::string PA_SCHEDULE_BROADCAST_START_TIME_COL = "START_TIME";
const std::string PA_SCHEDULE_BROADCAST_END_TIME_COL = "END_TIME";
const std::string PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL = "INTERVAL_TYPE";
const std::string PA_SCHEDULE_BROADCAST_IS_ENABLE_COL = "IS_ENABLE";
const std::string PA_SCHEDULE_BROADCAST_AUTO_DEL_COL = "AUTO_DEL";
const std::string PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL = "DVA_MSG_KEY";
const std::string PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL = "CYCLIC_TIMES";
const std::string PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL = "PRIORY_GROUP";
const std::string PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL = "SOURCE_TYPE";
const std::string PA_SCHEDULE_BROADCAST_STATUS_COL = "STATUS";

const std::string PA_SCHEDULE_BROADCAST_SEQUENCE_COUNTER = "PA_SCHEDULE_BROADCAST_SEQ";

const std::string ONCE_SCHEDULE_INTERVAL_TYPE = "O";
const std::string EVERYDAY_SCHEDULE_INTERVAL_TYPE = "D";
const std::string EVERYWEEK_SCHEDULE_INTERVAL_TYPE = "W";
const std::string EVERYMONTH_SCHEDULE_INTERVAL_TYPE = "M";
const std::string EVERYYEAR_SCHEDULE_INTERVAL_TYPE = "Y";

const std::string DVA_MSG_SOURCE_TYPE = "D";
const std::string ADHOC_MSG_SOURCE_TYPE = "A";

const std::string SCHEDULE_STATUS_SUCCESSFUL = "S";
const std::string SCHEDULE_STATUS_FAILED = "F";
const std::string SCHEDULE_STATUS_WAITING = "W";

// Table PA_SCHEDULE_BROADCAST_ZONES
const std::string PA_SCHEDULE_BROADCAST_ZONES_TABLE = "PA_SCHEDULE_BROADCAST_ZONES";

const std::string PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL = "SCHEDULE_BROADCAST_KEY";
const std::string PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL = "ZONE_KEY";
}

#endif
