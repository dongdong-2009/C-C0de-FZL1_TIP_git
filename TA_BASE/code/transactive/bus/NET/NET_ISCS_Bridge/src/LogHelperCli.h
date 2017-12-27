/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This class is C++/CLI Wrapper for Log Utiltiy in ISCS.
  */

#pragma once

using namespace System;
using namespace System::Diagnostics;

#include "core\utilities\src\DebugUtil.h"


namespace NET_ISCS_Bridge {

	public enum class EDebugLevelManaged
	{
		 DebugFatal		= 0L,	// fatal error conditions
             DebugError,		    // error conditions
             DebugWarn,		        // warning conditions
             DebugNorm,		        // normal conditions
             DebugInfo,		        // informational messages
             DebugDebug,		    // debug-level messages
             DebugSQL,              // SQL query strings
             DebugCORBA,            // for CORBA debug messages (only if other flags are enabled - see CorbaUtil)
             DebugMsg,              // for Messaging library debug messages
             DebugMsgPub,           // for Messaging library message sending and receiving debug messages
             DebugPeriodic,         // messages logged periodically by keepalives, etc
             DebugDatabase,      	// data access (SimpleDB) messages (other than SQL query strings)
             DebugExternal,      	// 3rd party (non-transactive) messages
             DebugTrace,		    // Function Entry/Exit Messages

	};


	public enum class EDebugTypeManaged
	{
		GenericLog,
		FunctionEntry,
		FunctionExit,
		ExceptionConstruction,
		ExceptionDetails,
		ExceptionCatch
	};


	public ref class LogHelperCli
	{
	public :

		static LogHelperCli^ GetInstance(); 
		static void ReleaseInstance();
		
		void Log(const String^ file, Int32 line, EDebugTypeManaged type,... array<Object^>^ params );
		void Log_Generic(String^ file, Int32 line, ... array<Object^>^ params);
		void LogBeforeCOTS(String^ cotsname);
		void LogAfterCOTS(String^ cotsname);
		void SetFile(String^ name);
		void SetMaxSize(Int32 maxSizeInBytes);
		EDebugLevelManaged  GetDebugLevelFromString(String^ levelStr);
		String^ GetStringDebugLevel(EDebugLevelManaged level);
		void SetLevel(EDebugLevelManaged level);
		String ^FormatAString(String ^format, ...array<Object^> ^args);
		 String^ GetFileName();
		 Int32 GetLineNumber();
	
	private:
		LogHelperCli();
		static  LogHelperCli^  m_instance = nullptr;

		
	};
}
