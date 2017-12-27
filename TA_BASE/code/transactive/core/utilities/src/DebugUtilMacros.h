#ifndef DEBUGUTILMACROS_H_INCLUDED
#define DEBUGUTILMACROS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DebugUtilMacros.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */


/** 
  * LOG. 
  * 
  * This macro is used to log a debug message.
  *
  * *** NOTE: *** 
  *     Any strings passed in MUST be a const char* NOT a std::string.
  *
  * Usage: 
  *      LOG(SourceInfo, DebugUtil::GenericLog, <Debug level>, <format string>, ...)
  *      LOG(SourceInfo, <Preformatted log type>, ...)
  */
#define LOG          TA_Base_Core::DebugUtil::getInstance().log

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

#define LOGBEFORECOTS(cotsname) \
	TA_Base_Core::DebugUtil::getInstance().logGeneric(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "COTS %s is going to be used.", cotsname);

#define LOGAFTERCOTS(cotsname) \
	TA_Base_Core::DebugUtil::getInstance().logGeneric(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "COTS %s is used.", cotsname);
/** 
  * LOGMORE. 
  * 
  * This macro is used to log an indented debug message without the date, time,
  * debug level, thread and location information.
  * This must be called after a LOG as its level is related to the last LOG.
  *
  * *** NOTE: *** 
  *     Any strings passed in MUST be a const char* NOT a std::string.
  *
  * Usage: 
  *      LOGMORE(SourceInfo, DebugUtil::GenericLog, <Debug level>, <format string>, ...)
  */
#define LOGMORE     TA_Base_Core::DebugUtil::getInstance().logMore

#define LOGLARGESTRING TA_Base_Core::DebugUtil::getInstance().logLargeString

/** 
  * Source Info. 
  *
  * This macro needs to be the first argument to 
  * every LOG call so that the file name and line number
  * can be logged.
  */
#define SourceInfo  __FILE__,__LINE__


#define LOG_EXCEPTION_DETAILS    TA_Base_Core::gLogExceptionDetails
#define LOG_EXCEPTION_CATCH      TA_Base_Core::gLogExceptionCatch


namespace TA_Base_Core
{
    // Necessary to support LOG_XXX_XXX macros above.
    inline void gLogExceptionDetails(const std::string& file, int line, const std::string& name, const std::string& details)
    {
        TA_Base_Core::DebugUtil::getInstance().log(file, line, DebugUtil::ExceptionDetails, name.c_str(), details.c_str());
    }

    inline void gLogExceptionCatch(const std::string& file, int line, const std::string& name, const std::string& description)
    {
        TA_Base_Core::DebugUtil::getInstance().log(file, line, DebugUtil::ExceptionCatch, name.c_str(), description.c_str());
    }

}   // namespace TA_Base_Core

// DEPRECATED.  Use FUNCTION_ENTRY and FUNCTION_EXIT macros instead.       
#define LOG_FUNCTION_ENTRY(sourceInfo, name)    FUNCTION_ENTRY(name)
#define LOG_FUNCTION_EXIT(sourceInfo, name)     FUNCTION_EXIT


#endif // DEBUGUTILMACROS_H_INCLUDED

