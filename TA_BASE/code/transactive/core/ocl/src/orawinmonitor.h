/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orawinmonitor.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:23 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  OraMonitor support
//  Last modified:      25.12.02
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORAWINMONITOR_H__
#define __ORAWINMONITOR_H__

#include "oramonitor.h"

#ifdef _MSC_VER
#  include "dbmon/msvc/dbmon.h"
#elif __BORLANDC__
#  include "dbmon/bcb/dbmon.h"
#else
#  error "Your compiler is not recognized by OCL"
#endif

namespace ocl {
	
	class OraWinMonitor : public OraMonitor {
	public:
		OraWinMonitor();
		~OraWinMonitor();
		
		void onConnect(MonitorMsg& msg,
			const CRString& userName,
			const CRString& server);
		
		void onTransact(MonitorMsg& msg);
		
		void onPrepare(MonitorMsg& msg, const CRString& sql);  
		
		void onExecute(MonitorMsg& msg,
			const CRString& sql,
			unsigned long rowsProcessed,
			const vector<MonitorParam>& params);
		
		void onBlob(MonitorMsg& msg);  
		
		void onError(MonitorMsg& msg, const CRString& errorMsg);
		
	private:
		
		// COM object wrapper
		IDBMonitorPtr mDBMonitorPtr;
		
		// TLS index for thread current message ID
		static long mTlsIndex;
		
		// True if COM object exist and DBMonitor started
		bool isDBMonitorActive();
		
		// Send MonitorMsg structure to DBMonitor
		// Return message ID for current message
		long writeDBMonitorMsg(const MonitorMsg& msg, const char* pramStr);
		
		// Send MonitorMsg and additional parameters for onExecute
		// Return message ID for current message
		long writeDBMonitorParams(const MonitorMsg& msg, const CRString& sql,
			const vector<MonitorParam>& params, unsigned long rowsProcessed);
	};
	
} // namespace ocl

#endif // __ORAWINMONITOR_H__