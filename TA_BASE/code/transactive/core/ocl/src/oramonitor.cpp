/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oramonitor.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:21 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  OraMonitor support
//  Last modified:      25.12.02
//////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include "ocl.conf"
#include "oramonitor.h"
#include "ora.h"

#ifdef OCL_WIN32
#  include "orawinmonitor.h"
#endif

#include <map>

namespace ocl {
	
	std::auto_ptr<OraMonitor> ds_ocl_sql_monitor;
	bool OraMonitor::mIsMonitorEnable = false; 
	MonitorEventCallback OraMonitor::beforeMonitorEvent;
	MonitorEventCallback OraMonitor::afterMonitorEvent;
	
#ifdef MOCL
	int OraMonitor::mLastEventType;
	CRString OraMonitor::mLastEventDesc;
	CRString OraMonitor::mLastEventParams;
#endif
	
	OraMonitor::MonitorMsg::MonitorMsg() {
		
		msgID        = 0;
		monitorEvent = meConnect;
		tracePoint   = tpBefor;
		ownerID      = 0;
		objectID     = 0;
	}  
	
	OraMonitor::MonitorParam::MonitorParam() {
		
		dataType = dtUnknown;
		type     = ocl::pdUnknown;
	}
	
	CRString OraMonitor::MonitorParam::toString() const {
		
		static std::map<int, CRString> dataTypeString;
		static std::map<int, CRString> paramTypeString;
		
		if (dataTypeString.size() == 0) {
			dataTypeString[dtUnknown]        = "Unknown";
			dataTypeString[dtString]         = "String";
			dataTypeString[dtInt32]          = "Integer";
			dataTypeString[dtInt]            = "Integer";
			dataTypeString[dtInteger]        = "Integer";
			dataTypeString[dtInt64]          = "Int64";
			dataTypeString[dtFloat]          = "Float";
			dataTypeString[dtDouble]         = "Double";
			dataTypeString[dtDate]           = "Date";
			dataTypeString[dtBlob]           = "Blob";
			dataTypeString[dtClob]           = "Clob";
			dataTypeString[ocl::dtObject]    = "Object";
			dataTypeString[ocl::dtTable]     = "Table";
			dataTypeString[ocl::dtRowId]     = "RowId";
			dataTypeString[ocl::dtCursor]    = "Cursor";
			dataTypeString[ocl::dtBFile]     = "BFile";
			dataTypeString[ocl::dtBoolean]   = "Boolean";
			dataTypeString[ocl::dtLabel]     = "Label";
		}
		
		if (paramTypeString.size() == 0) {
			paramTypeString[ocl::pdUnknown]     = "Unknown";
			paramTypeString[ocl::pdInput]       = "Input";
			paramTypeString[ocl::pdOutput]      = "Output";
			paramTypeString[ocl::pdInputOutput] = "InputOutput";
		}
		
		CRString str = "Name: " + name +
			" Value: " + value + " Data Type: " + dataTypeString[dataType] +
			" Direction: " + paramTypeString[type];
		
		return str;
	}
	
	void OraMonitor::start() {
		
		if (!mIsMonitorEnable) {
#ifdef OCL_WIN32
			if (!ds_ocl_sql_monitor.get()) {
				std::auto_ptr<OraMonitor> m(new OraWinMonitor);
				ds_ocl_sql_monitor = m;
			}
#endif
			mIsMonitorEnable = true;
		}
	}
	
	void OraMonitor::stop() {
		
		if (mIsMonitorEnable) {
			if (ds_ocl_sql_monitor.get()) {
				OraMonitor* monitor = ds_ocl_sql_monitor.release();
				delete monitor;
			}
			mIsMonitorEnable = false;
		}
	}
	
	void OraMonitor::setBeforeMonitorEvent(MonitorEventCallback event) {
		
		beforeMonitorEvent = event;
	}
	
	void OraMonitor::setAfterMonitorEvent(MonitorEventCallback event) {
		
		afterMonitorEvent = event;
	}
	
	void OraMonitor::raiseBeforeEvent(int eventType, const CRString& desc,
		const CRString& params)
	{
		if (beforeMonitorEvent) {
#ifdef MOCL
			mLastEventType = eventType;
			mLastEventDesc = desc;
			mLastEventParams = params;
			beforeMonitorEvent();
#else
			beforeMonitorEvent(eventType, desc, params);
#endif
		}
	}
	
	void OraMonitor::raiseAfterEvent(int eventType, const CRString& desc,
		const CRString& params)
	{
		if (afterMonitorEvent) {
#ifdef MOCL
			mLastEventType = eventType;
			mLastEventDesc = desc;
			mLastEventParams = params;
			afterMonitorEvent();
#else
			afterMonitorEvent(eventType, desc, params);
#endif
		}
	}
	
} // namespace ocl
