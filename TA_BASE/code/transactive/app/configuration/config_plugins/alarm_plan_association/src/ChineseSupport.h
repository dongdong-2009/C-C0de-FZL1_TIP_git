#pragma once
#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include <string>


namespace alarmPlanAsso_chinese {

	const CString APPLIED_COLUMN_NAME ="状态";         //"Status";
	const CString ALARMTYPE_COLUMN_NAME ="警报类型";   //"Alarm Type";
	const CString ENTITYTYPE_COLUMN_NAME ="实体类型";  //"Entity Type";
	const CString ENTITY_COLUMN_NAME = "实体";         //"Entity";
	const CString PLAN_COLUMN_NAME = "预案";           //"Plan";

	// These are the possible values for the status of each alarm plan association
	const CString NEW = "新建";       //"New";
	const CString MODIFIED = "修改";   // "Modified";
	const CString UPTODATE = "";

	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S";
    
	// The titles for the change dialog box
	const CString CHANGE_ENTITYTYPE_DIALOG_TITLE    ="更改实体类型";    //"Change Entity Type";
	const CString CHANGE_ALARMTYPE_DIALOG_TITLE  = "更改警报类型";     //"Change Alarm Type";
}