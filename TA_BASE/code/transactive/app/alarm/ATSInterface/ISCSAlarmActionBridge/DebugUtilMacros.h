#ifndef DEBUGUTILMACROS_H_INCLUDED
#define DEBUGUTILMACROS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/alarm/ATSInterface/ISCSAlarmActionBridge/DebugUtilMacros.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/31 15:10:17 $
  * Last modified by:  $Author: lei.chen $
  *
  * <description>
  *
  */

/**
  * LOG_GENERIC
  *
  * This macro is used to make generic log messages (ie a message not fitting
  * one of the preformatted message types).
  *
  * Usage:
  *      LOG_GENERIC(SourceInfo, <Debug level>, <format string>, ...)
  */
#define LOG_GENERIC  TA_Base_Core::DebugUtil::getInstance().logGeneric


// Macros that are equivalent to LOG_GENERIC, but avoid evaluating arguments if
// the current debug level is less than the message's log level.
#define LOG0(sourceInfo, debugLevel, formatStr) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr); \
    } 

#define LOG1(sourceInfo, debugLevel, formatStr, arg1) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1); \
    } 

#define LOG2(sourceInfo, debugLevel, formatStr, arg1, arg2) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2); \
    } 

#define LOG3(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3); \
    } 

#define LOG4(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4); \
    } 

#define LOG5(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5); \
    } 

#define LOG6(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5, arg6) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5, arg6); \
    } 

/** 
  * Source Info. 
  *
  * This macro needs to be the first argument to 
  * every LOG call so that the file name and line number
  * can be logged.
  */
#define SourceInfo  __FILE__,__LINE__


#endif // DEBUGUTILMACROS_H_INCLUDED

