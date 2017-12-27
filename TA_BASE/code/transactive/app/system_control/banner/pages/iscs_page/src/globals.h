/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/pages/iscs_page/src/globals.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef GLOBALS_H
#define GLOBALS_H

static const ULONG WM_ON_CHANGE_MSG		= WM_USER + 1;
static const ULONG WM_UPDATE_ALARMS_MSG = WM_USER + 2;
static const ULONG WM_ON_SYSTEM_DEGRADED =	WM_USER + 3;
static const ULONG WM_ON_SYSTEM_RECOVERED =	WM_USER + 4;

static const ULONG PROFILE_LOCAL   = 0;
static const ULONG PROFILE_CENTRAL = 1;

#endif
