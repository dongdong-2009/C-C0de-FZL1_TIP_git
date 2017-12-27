#pragma once
#include "app/configuration/config_plugins/alarm_type/src/StdAfx.h"
#include <string>


namespace alarmType_chinese {


	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S";

	// The title of the dialog that changes the alarm type
	const CString CHANGE_ALARMSEVERITY_DIALOG_TITLE = "更改警报严重性";  //"Change Alarm Severity";

	const CString ATTRIBUTE_NAME_COLUMN_NAME   = "属性";  //"Attribute";
	const CString ATTRIBUTE_VALUE_COLUMN_NAME  = "值";    //"Value";
	const CString ATTRIBUTE_UPDATE_COLUMN_NAME = "重启";  // "Restart";
	const std::string BOOLEAN_TRUE ="是";                 // "Yes";
	const std::string BOOLEAN_FALSE ="否";                // "No";

}