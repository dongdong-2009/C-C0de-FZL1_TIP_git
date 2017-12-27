#pragma once
#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include <string>


namespace alarmrule_chinese {

	const CString TYPE_COLUMN_NAME       = "规则类型";       //"Rule Type";
	const CString APPLIED_COLUMN_NAME    = "状态";           //"Status";
	const CString ALARMTYPE_COLUMN_NAME  = "警报类型";       //"Alarm Type";
	const CString ENTITYTYPE_COLUMN_NAME = "实体类型";       //"Entity Type";
	const CString ENTITY_COLUMN_NAME     = "实体";           //"Entity";
	const CString TRIGGER_COLUMN_NAME    = "触发器";         //"Trigger";
	const CString DESCRIPTION_COLUMN_NAME = "警报描述";      //"Alarm Description";
    const CString NEW_ALARM_RULE_DIALOG_TITLE ="警报规则类型"; //"Alarm Rule Type";

	const std::string AUTO_ACK_RULE ="自动确认规则";     // "Automatic Acknowledgement Rule";
	const std::string AVA_SUPPRE_RULE ="雪崩规则";     //"Avalanche Suppression Rule";
	const std::string PLAN_EXEC_RULE ="预案执行规则";     //"Plan Execution Rule";
	const std::string SUPPRE_BY_TYPE_RULE ="报警禁止 — 类型";     //"Suppression By Type Rule";
	const std::string SUPPRE_BY_DESC_RULE ="报警禁止 — 描述";     //"Suppression By Description Rule";
	const std::string MMS_RULE ="MMS规则";     //MMSRuleData::RULE_TYPE + " Rule";
	const std::string CCTV_RULE ="CCTV规则";      //"CCTV Rule";

	const std::string AUTO_ACK_RULE_EN ="AutomaticAcknowledgement";   //? these to be confirmed by DB
	const std::string AVA_SUPPRE_RULE_EN ="AvalancheSuppression";
	const std::string PLAN_EXEC_RULE_EN ="PlanExecution";
	const std::string SUPPRE_BY_TYPE_RULE_EN ="SuppressionByType";
	const std::string SUPPRE_BY_DESC_RULE_EN ="SuppressionByDescription";
	const std::string MMS_RULE_EN ="MMS";
	const std::string CCTV_RULE_EN ="CCTV";


	 // These are the possible values for the status of each alarm rule
	const CString NEW = "新建";            //"New";
	const CString MODIFIED ="修改";        // "Modified";
	const CString UPTODATE = "";           //""

	 // The titles for the change dialog box
	const CString CHANGE_ENTITYTYPE_DIALOG_TITLE ="更改实体类型";  //"Change Entity Type";
	const CString CHANGE_ALARMTYPE_DIALOG_TITLE  ="更改警报类型";   // "Change Alarm Type";

	const std::string TRIGGER_SUBMIT = "提交";
	const std::string TRIGGER_CLOSE = "关闭";
	const std::string TRIGGER_ACKNOWLEDGE = "确认";  

	const std::string TRIGGER_SUBMIT_EN = "submit";
	const std::string TRIGGER_CLOSE_EN = "close";
	const std::string TRIGGER_ACKNOWLEDGE_EN = "acknowledge";
 

	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S"; 

	// this one set in file "TA_AlarmRulesConfigPlugin.rc"
	// "雪崩子规则" "Avalanche Suppression Rule Children"

}