#pragma once
#include "app/configuration/config_plugins/alarm_type/src/StdAfx.h"
#include <string>


namespace alarmType_chinese {


	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S";

	// The title of the dialog that changes the alarm type
	const CString CHANGE_ALARMSEVERITY_DIALOG_TITLE = "���ľ���������";  //"Change Alarm Severity";

	const CString ATTRIBUTE_NAME_COLUMN_NAME   = "����";  //"Attribute";
	const CString ATTRIBUTE_VALUE_COLUMN_NAME  = "ֵ";    //"Value";
	const CString ATTRIBUTE_UPDATE_COLUMN_NAME = "����";  // "Restart";
	const std::string BOOLEAN_TRUE ="��";                 // "Yes";
	const std::string BOOLEAN_FALSE ="��";                // "No";

}