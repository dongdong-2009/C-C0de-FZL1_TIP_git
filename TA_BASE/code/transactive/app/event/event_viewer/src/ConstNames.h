#pragma once
#include "app/event/event_viewer/src/stdafx.h"
#include <string>


namespace constnames {



static const CString EVENT_COUNT = "事件数 :"; 
static const CString ON_LOADING = "加载中...";
static const CString PRINT_WITH_SPACE = " 打印";
static const CString EXPORT_WITH_SPACE = " 输出";
static const CString FILTER_WITH_SPACE = "过滤器 ";
static const CString PREVIOUS = "向前";
static const CString NEXT = "向后";
static const CString EVENT_REPORT = "事故报告";
static const CString ALARM_MANAGER = "告警管理器";
static const CString EVENT_VIEWER = "事件浏览器";

static const std::string EXPORT_MSG1 = "Set and save to Local Directory";
static const std::string SLASH_FILTER_ON = " - 过滤: ";

static const unsigned int TWENTY_FOUR_HOURS( 24*60*60 );

static const std::string ERROR_LOAD( "载入" );
static const std::string ERROR_LOADED( "已载入" );
static const std::string ERROR_SAVE( "保存" );
static const std::string ERROR_SAVED( "已保存" );
static const std::string ERROR_DELETE( "删除" );
static const std::string ERROR_DELETED( "已删除" );
static const std::string ANGULAR_BRACKETS_ALL("<全部>");
static const std::string OVERWRITE_FILTER("用当前过滤器设置重写");
static const char* USER_DEFINED_FILTER("自定义过滤器");
static const char* DEFAULT_FILTER("默认过滤器");
static const std::string SAVE_ERR("保存无效的名字 ");
static const std::string SAVE_AS("保存为 ");

static const CString MULTIPLE_DLG_SUBSYSTEM_TITLE = "选择子系统...";
static const CString MULTIPLE_DLG_LOCATION_TITLE = "选择位置...";
static const CString MULTIPLE_DLG_OPERATOR_TITLE = "选择操作员...";
static const CString MULTIPLE_DLG_EVTYPE_TITLE = "选择事件类型...";
static const CString MULTIPLE_DLG_ALARMTYPE_TITLE = "选择报警类型...";


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

static const CString EVENT_LEVEL_LABEL_TEXT = "事件等级";
static const CString NAMED_FILTER_LABEL_TEXT = "当前过滤器 ";
static const CString IDLE_TEXT = "准备就绪";
static const CString FONT_NAME = "宋体";
static const char* EVENT_ONLY_STRING = "事件";
static const std::string ALARM_FILTER_MESSAGE("无报警过滤");
static const std::string TEXT_NO_ALARMID( "无报警过滤" );
static const std::string NAMEDFILTER_AVALANCHE( "雪崩事件过滤器" );

static const std::string STATUS_QUERYING( "执行数据库查询..." );
static const std::string STATUS_DUTY( "获取职责..." );


///////////from CombinedEventFilter.cpp


	const std::string COLUMN_DATETIME("日期/时间");
	const std::string COLUMN_SEVERITY("级别");
	const std::string COLUMN_ASSET("设备");
	const std::string COLUMN_DESCRIPTION("描述");
	const std::string COLUMN_VALUE("值");
	const std::string COLUMN_MMS("M");
	const std::string COLUMN_DSS("D");
	const std::string COLUMN_AVL("A");
	const std::string COLUMN_T("T");
	const std::string COLUMN_OPERATOR("操作员");
	const std::string COLUMN_COMMENT("C");
	const std::string COLUMN_ALARM_STATUS("警报状态");
	

	// Assign the required values to the hidden column constants.
	const std::string HIDDEN_COLUMN_ALARMID("警报ID");
	const std::string HIDDEN_COLUMN_ACKED("已确认");
	const std::string HIDDEN_COLUMN_STATUS("状态");

	// Assign the required values to the filter constants.
	const std::string FILTER_APPLICATION_SUBSYSTEM("应用程序子系统");
	const std::string FILTER_PHYSICAL_SUBSYSTEM("物理子系统");
	const std::string FILTER_LOCATION("位置");
	const std::string FILTER_OPERATOR("操作员");
	const std::string FILTER_EVENTTYPE("事件类型");
	const std::string FILTER_ASSET("设备");
	const std::string FILTER_DESCRIPTION("描述");
	const std::string FILTER_SEVERITY("级别");
	const std::string FILTER_ALARMTYPE("警报类型");
	const std::string FILTER_ALARMID("警报编号");

	// The name of the default filter.
	const std::string NAMEDFILTER_DEFAULT( "默认过滤器" );

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
