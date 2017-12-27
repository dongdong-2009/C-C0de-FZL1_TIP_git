/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/globals.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 14:24:00 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
#ifndef GLOBALS_H
#define GLOBALS_H

static const ULONG IDC_ACK =			WM_USER + 1;
static const ULONG IDC_CLOSE_ALARM =    WM_USER + 2;
static const ULONG IDC_COMMENT =		WM_USER + 3;
static const ULONG IDC_ACKALL = 		WM_USER + 4;
static const ULONG IDC_DEFAULT_SORT = 	WM_USER + 5;
static const ULONG IDC_ALARM_VIEW = 	WM_USER + 6;
static const ULONG IDC_PRINT = 			WM_USER + 7;
static const ULONG IDC_EXPORT = 		WM_USER + 8;
static const ULONG IDC_SHOW_FILTERS = 	WM_USER + 9;
static const ULONG WM_ON_CHANGE_MSG = 	WM_USER + 10;
static const ULONG START_EXPORT = 		WM_USER + 11;
static const ULONG START_PRINT  = 		WM_USER + 12;
static const ULONG IDC_RELATED_EVENTS = WM_USER + 13;
static const ULONG IDC_INHIBIT_ALARMS =    WM_USER + 14;
static const ULONG IDC_SHOW_RULES =	        WM_USER + 15;
static const ULONG WM_ON_FILTER_DLG_APPLY =	WM_USER + 16;
static const ULONG WM_ON_FILTER_DLG_DELETE =	WM_USER + 17;
static const ULONG WM_ON_SYSTEM_DEGRADED =	WM_USER + 18;
static const ULONG WM_ON_SYSTEM_RECOVERED =	WM_USER + 19;
static const ULONG WM_ON_FILTER_DLG_UPDATE = WM_USER + 20;

static const ULONG PROFILE_LOCAL   = 0;
static const ULONG PROFILE_CENTRAL = 1;
#endif
