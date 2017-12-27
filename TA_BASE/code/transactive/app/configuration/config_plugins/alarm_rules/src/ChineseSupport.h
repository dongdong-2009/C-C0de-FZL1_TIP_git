#pragma once
#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include <string>


namespace alarmrule_chinese {

	const CString TYPE_COLUMN_NAME       = "��������";       //"Rule Type";
	const CString APPLIED_COLUMN_NAME    = "״̬";           //"Status";
	const CString ALARMTYPE_COLUMN_NAME  = "��������";       //"Alarm Type";
	const CString ENTITYTYPE_COLUMN_NAME = "ʵ������";       //"Entity Type";
	const CString ENTITY_COLUMN_NAME     = "ʵ��";           //"Entity";
	const CString TRIGGER_COLUMN_NAME    = "������";         //"Trigger";
	const CString DESCRIPTION_COLUMN_NAME = "��������";      //"Alarm Description";
    const CString NEW_ALARM_RULE_DIALOG_TITLE ="������������"; //"Alarm Rule Type";

	const std::string AUTO_ACK_RULE ="�Զ�ȷ�Ϲ���";     // "Automatic Acknowledgement Rule";
	const std::string AVA_SUPPRE_RULE ="ѩ������";     //"Avalanche Suppression Rule";
	const std::string PLAN_EXEC_RULE ="Ԥ��ִ�й���";     //"Plan Execution Rule";
	const std::string SUPPRE_BY_TYPE_RULE ="������ֹ �� ����";     //"Suppression By Type Rule";
	const std::string SUPPRE_BY_DESC_RULE ="������ֹ �� ����";     //"Suppression By Description Rule";
	const std::string MMS_RULE ="MMS����";     //MMSRuleData::RULE_TYPE + " Rule";
	const std::string CCTV_RULE ="CCTV����";      //"CCTV Rule";

	const std::string AUTO_ACK_RULE_EN ="AutomaticAcknowledgement";   //? these to be confirmed by DB
	const std::string AVA_SUPPRE_RULE_EN ="AvalancheSuppression";
	const std::string PLAN_EXEC_RULE_EN ="PlanExecution";
	const std::string SUPPRE_BY_TYPE_RULE_EN ="SuppressionByType";
	const std::string SUPPRE_BY_DESC_RULE_EN ="SuppressionByDescription";
	const std::string MMS_RULE_EN ="MMS";
	const std::string CCTV_RULE_EN ="CCTV";


	 // These are the possible values for the status of each alarm rule
	const CString NEW = "�½�";            //"New";
	const CString MODIFIED ="�޸�";        // "Modified";
	const CString UPTODATE = "";           //""

	 // The titles for the change dialog box
	const CString CHANGE_ENTITYTYPE_DIALOG_TITLE ="����ʵ������";  //"Change Entity Type";
	const CString CHANGE_ALARMTYPE_DIALOG_TITLE  ="���ľ�������";   // "Change Alarm Type";

	const std::string TRIGGER_SUBMIT = "�ύ";
	const std::string TRIGGER_CLOSE = "�ر�";
	const std::string TRIGGER_ACKNOWLEDGE = "ȷ��";  

	const std::string TRIGGER_SUBMIT_EN = "submit";
	const std::string TRIGGER_CLOSE_EN = "close";
	const std::string TRIGGER_ACKNOWLEDGE_EN = "acknowledge";
 

	const std::string DATE_FORMAT_STR = "%Y/%m/%d %H:%M:%S"; 

	// this one set in file "TA_AlarmRulesConfigPlugin.rc"
	// "ѩ���ӹ���" "Avalanche Suppression Rule Children"

}