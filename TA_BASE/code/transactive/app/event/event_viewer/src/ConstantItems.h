/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ConstantItems.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventViewerConstants is the namespace which includes const items used in this application.
  *
  */

#include <windows.h>


namespace EventViewerConstants {

	const std::string RPARAM_NO_VISIBILITY_RULE = "NoVisibilityRule";
	const std::string REFRESH_TIME = "EventRefreshTime";  // the parameter name saved in table EntityParameter
	const std::string MAX_EVENTS = "MaxEvents";
	const std::string LOG_EVENT_DETAIL_INTERVAL = "LogEventDetailInterval";

	const int ATS_EVENT_COUNT_DEFAULT = 20000; 
	const int ISCS_EVENT_COUNT_DEFAULT = 20000;
	/*const unsigned long ISCS_REFRESH_SEC = 5;    this can be retrieved from DB : table "entityparametervalue" */ 
	
	const int WORKER_REFRESH_TASK_PERIOD = 200; //"ms"; worker sleep time before check for new task 

	const int AGENT_STATUS_CHECK_INTERVAL =200; //"ms" agentStutasCheckInterval

	const unsigned long REALTIME_LOAD_BATCH = 20000;

	const int RIGHTS_CACHE_CHECK_INTERVAL = 200; //"ms", the agent sleep time if rights cache is not ready.

	const bool ENABLE_ISCS_DEFAULT = true;
	const bool ENABLE_ATS_DEFAULT = true;

	// This is the key value for "event" in "severity" (means alarm severity)on the filter dialogue.
    const int EVENT_ONLY_KEY = 0;

	const unsigned int TWENTY_FOUR_HOURS = 24*60*60;
	const std::string EMPTY_STRING("");

	const COLORREF COLOUR_BLACK( RGB(0, 0, 0) );
	const COLORREF COLOUR_BEIGE( RGB( 230, 227, 194 ) );
	const COLORREF COLOUR_DARKBEIGE( RGB( 219, 186, 119 ) );

	const unsigned long DEFAULT_EVENT_LEVEL( 5 );

	// message information which will be shown at the left bottom of the EventView dialogue
	const std::string STATUS_ATS_QUERYING( "ATS 执行数据库查询..." );
	const std::string STATUS_ISCS_QUERYING( "执行数据库查询..." );
	const std::string STATUS_ATS_READY( "ATS 准备就绪" );
	const std::string STATUS_ISCS_READY( "准备就绪" );
	const std::string STATUS_ATS_UNHEALTHY( "ATS 出现问题" );
	const std::string STATUS_ISCS_UNHEALTHY( "出现问题" );
	const std::string STATUS_ATS_DISABLED( "ATS 关闭" );
	const std::string STATUS_ICSC_DISABLED( "关闭" );
	const std::string STATUS_SEPERATE (" | ");

};
