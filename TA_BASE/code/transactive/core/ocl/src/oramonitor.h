/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oramonitor.h
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
//  Last modified:      23.12.02
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __OCLMONITOR_H__
#define __OCLMONITOR_H__

#include "crdb.h"

#include <vector>
#include <memory>

namespace ocl {
	
#ifdef MOCL
	typedef void (*MonitorEventCallback)();
#else
	typedef void (*MonitorEventCallback)(int eventType,
		const CRString& desc, const CRString& params);
#endif
	
	using std::vector;
	using std::string;
	
	class OraMonitor {
	public:
		
		enum MonitorEvent {
			meConnect = 2,
				meDisconnect,
				meCommit,
				meRollback,
				mePrepare,
				meUnprepare,
				meExecute,
				meBlob,
				meError,
				meMisc
		};
		
		enum TracePoint {
			tpBefor,
				tpAfter
		};
		
		struct MonitorMsg {
			MonitorMsg();
			long                 msgID;
			MonitorEvent         monitorEvent;
			TracePoint           tracePoint;
			int                  objectID;
			int                  ownerID;
			CRString             objectName;
			CRString             ownerName;
			CRString             description;
		};
		
		struct MonitorParam {
			MonitorParam();
			CRString   name;
			CRString   value;
			CRDataType dataType;
			int        type;
			
			CRString toString() const;
		};
		
		OraMonitor() {};
		virtual ~OraMonitor() {}
		
		static bool isActive() { return mIsMonitorEnable; }
		static void start();
		static void stop();
		
		static void setBeforeMonitorEvent(MonitorEventCallback event);
		static void setAfterMonitorEvent(MonitorEventCallback event);
		
		static void raiseBeforeEvent(int eventType, const CRString& desc,
			const CRString& params = "");
		static void raiseAfterEvent(int eventType, const CRString& desc,
			const CRString& params = "");
		
		virtual void onConnect(MonitorMsg& msg,
			const CRString& userName,
			const CRString& server) = 0;
		
		virtual void onTransact(MonitorMsg& msg) = 0;
		
		virtual void onPrepare(MonitorMsg& msg, const CRString& sql) = 0;
		
		virtual void onExecute(MonitorMsg& msg,
			const CRString& sql,
			unsigned long rowsProcessed,
			const vector<MonitorParam>& params) = 0;
		
		virtual void onBlob(MonitorMsg& msg) = 0;
		
		virtual void onError(MonitorMsg& msg, const CRString& errorMsg) = 0;
		
		protected:
			static MonitorEventCallback beforeMonitorEvent;
			static MonitorEventCallback afterMonitorEvent;
			static bool mIsMonitorEnable;
#ifdef MOCL
			static int mLastEventType;
			static CRString mLastEventDesc;
			static CRString mLastEventParams;
#endif
	};
	
	extern std::auto_ptr<OraMonitor> ds_ocl_sql_monitor;
	
} // namespace ocl

#endif // __OCLMONITOR_H__
