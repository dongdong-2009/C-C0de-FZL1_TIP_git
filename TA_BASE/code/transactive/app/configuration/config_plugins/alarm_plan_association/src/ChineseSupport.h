#pragma once
#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include <string>


namespace alarmPlanAsso_chinese {

	const CString APPLIED_COLUMN_NAME ="״̬";         //"Status";
	const CString ALARMTYPE_COLUMN_NAME ="��������";   //"Alarm Type";
	const CString ENTITYTYPE_COLUMN_NAME ="ʵ������";  //"Entity Type";
	const CString ENTITY_COLUMN_NAME = "ʵ��";         //"Entity";
	const CString PLAN_COLUMN_NAME = "Ԥ��";           //"Plan";

	// These are the possible values for the status of each alarm plan association
	const CString NEW = "�½�";       //"New";
	const CString MODIFIED = "�޸�";   // "Modified";
	const CString UPTODATE = "";

	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S";
    
	// The titles for the change dialog box
	const CString CHANGE_ENTITYTYPE_DIALOG_TITLE    ="����ʵ������";    //"Change Entity Type";
	const CString CHANGE_ALARMTYPE_DIALOG_TITLE  = "���ľ�������";     //"Change Alarm Type";
}