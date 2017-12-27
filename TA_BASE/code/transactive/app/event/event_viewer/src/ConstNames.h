#pragma once
#include "app/event/event_viewer/src/stdafx.h"
#include <string>


namespace constnames {



static const CString EVENT_COUNT = "�¼��� :"; 
static const CString ON_LOADING = "������...";
static const CString PRINT_WITH_SPACE = " ��ӡ";
static const CString EXPORT_WITH_SPACE = " ���";
static const CString FILTER_WITH_SPACE = "������ ";
static const CString PREVIOUS = "��ǰ";
static const CString NEXT = "���";
static const CString EVENT_REPORT = "�¹ʱ���";
static const CString ALARM_MANAGER = "�澯������";
static const CString EVENT_VIEWER = "�¼������";

static const std::string EXPORT_MSG1 = "Set and save to Local Directory";
static const std::string SLASH_FILTER_ON = " - ����: ";

static const unsigned int TWENTY_FOUR_HOURS( 24*60*60 );

static const std::string ERROR_LOAD( "����" );
static const std::string ERROR_LOADED( "������" );
static const std::string ERROR_SAVE( "����" );
static const std::string ERROR_SAVED( "�ѱ���" );
static const std::string ERROR_DELETE( "ɾ��" );
static const std::string ERROR_DELETED( "��ɾ��" );
static const std::string ANGULAR_BRACKETS_ALL("<ȫ��>");
static const std::string OVERWRITE_FILTER("�õ�ǰ������������д");
static const char* USER_DEFINED_FILTER("�Զ��������");
static const char* DEFAULT_FILTER("Ĭ�Ϲ�����");
static const std::string SAVE_ERR("������Ч������ ");
static const std::string SAVE_AS("����Ϊ ");

static const CString MULTIPLE_DLG_SUBSYSTEM_TITLE = "ѡ����ϵͳ...";
static const CString MULTIPLE_DLG_LOCATION_TITLE = "ѡ��λ��...";
static const CString MULTIPLE_DLG_OPERATOR_TITLE = "ѡ�����Ա...";
static const CString MULTIPLE_DLG_EVTYPE_TITLE = "ѡ���¼�����...";
static const CString MULTIPLE_DLG_ALARMTYPE_TITLE = "ѡ�񱨾�����...";


static const std::string ERROR_DELETE_EN( "delete" );
static const std::string ERROR_DELETED_EN( "deleted" );

// RunParam constants.
static const std::string RPARAM_FILTER_SYSTEM("FilterSystem"); 
static const std::string RPARAM_FILTER_SUBSYSTEM( "FilterSubsystem" );
static const std::string RPARAM_FILTER_LOCATION( "FilterLocation" );
static const std::string RPARAM_FILTER_OPERATOR( "FilterOperator" );
static const std::string RPARAM_FILTER_EVENTTYPE( "FilterEventtype" );
static const std::string RPARAM_FILTER_ASSET( "FilterAsset" );
static const std::string RPARAM_FILTER_DESCRIPTION( "FilterDescription" );
static const std::string RPARAM_FILTER_FROM( "FilterFrom" );
static const std::string RPARAM_FILTER_TO( "FilterTo" );
static const std::string RPARAM_FILTER_SEVERITY( "FilterSeverity" );
static const std::string RPARAM_FILTER_ALARMTYPE( "FilterAlarmtype" );
static const std::string RPARAM_FILTER_ALARMID( "AlarmId" );
static const std::string RPARAM_FILTER_ALARMCHILD( "AlarmChild");
static const std::string RPARAM_FILTER_DEFAULT( "FilterDefault" );

static const CString EVENT_LEVEL_LABEL_TEXT = "�¼��ȼ�";
static const CString NAMED_FILTER_LABEL_TEXT = "��ǰ������ ";
static const CString IDLE_TEXT = "׼������";
static const CString FONT_NAME = "����";
static const char* EVENT_ONLY_STRING = "�¼�";
static const std::string ALARM_FILTER_MESSAGE("�ޱ�������");
static const std::string TEXT_NO_ALARMID( "�ޱ�������" );
static const std::string NAMEDFILTER_AVALANCHE( "ѩ���¼�������" );

static const std::string STATUS_QUERYING( "ִ�����ݿ��ѯ..." );
static const std::string STATUS_DUTY( "��ȡְ��..." );


///////////from CombinedEventFilter.cpp


	const std::string COLUMN_DATETIME("����/ʱ��");
	const std::string COLUMN_SEVERITY("����");
	const std::string COLUMN_ASSET("�豸");
	const std::string COLUMN_DESCRIPTION("����");
	const std::string COLUMN_VALUE("ֵ");
	const std::string COLUMN_MMS("M");
	const std::string COLUMN_DSS("D");
	const std::string COLUMN_AVL("A");
	const std::string COLUMN_T("T");
	const std::string COLUMN_OPERATOR("����Ա");
	const std::string COLUMN_COMMENT("C");
	const std::string COLUMN_ALARM_STATUS("����״̬");
	

	// Assign the required values to the hidden column constants.
	const std::string HIDDEN_COLUMN_ALARMID("����ID");
	const std::string HIDDEN_COLUMN_ACKED("��ȷ��");
	const std::string HIDDEN_COLUMN_STATUS("״̬");

	// Assign the required values to the filter constants.
	const std::string FILTER_APPLICATION_SUBSYSTEM("Ӧ�ó�����ϵͳ");
	const std::string FILTER_PHYSICAL_SUBSYSTEM("������ϵͳ");
	const std::string FILTER_LOCATION("λ��");
	const std::string FILTER_OPERATOR("����Ա");
	const std::string FILTER_EVENTTYPE("�¼�����");
	const std::string FILTER_ASSET("�豸");
	const std::string FILTER_DESCRIPTION("����");
	const std::string FILTER_SEVERITY("����");
	const std::string FILTER_ALARMTYPE("��������");
	const std::string FILTER_ALARMID("�������");

	// The name of the default filter.
	const std::string NAMEDFILTER_DEFAULT( "Ĭ�Ϲ�����" );

	////// moved from Filter.cpp:
	const std::string FILTER_TOTIME("ToTime");
	const std::string FILTER_FROMTIME("FromTime");

}

// class ConstNames
// {
// public:
// 
// 
// 
// 	ConstNames(void);
// public:
// 	~ConstNames(void);
// };
