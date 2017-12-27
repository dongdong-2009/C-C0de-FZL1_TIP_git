/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orawinmonitor.cpp
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

#if defined _WIN32 && defined(_DEBUG)
#pragma warning(disable:4786)
#endif

#include "ocl.conf"
#include "orawinmonitor.h"
#include "ora.h"

#include <map>
#include <stdio.h>

namespace ocl {
	
#ifdef MOCL
	const char SProductName[] = "OraDirect .NET";
#else
	const char SProductName[] = "OCL";
#endif
	
	long OraWinMonitor::mTlsIndex;
	
	// Convert OCL MonitorMsg to internal TMonitorMessage
	void convertMessage(const OraMonitor::MonitorMsg& msg, TMonitorMessage& outMsg);
	// Convert OCL MonitorParam to internal TSQLParam
	void convertParam(const OraMonitor::MonitorParam& prm, TSQLParam& outPrm);
	
	OraWinMonitor::OraWinMonitor() {
		
		CoInitialize(0);
		
		mDBMonitorPtr.CreateInstance(CLSID_DBMonitor);
		
		if (mDBMonitorPtr) {
			mDBMonitorPtr->SetCaption((LPSTR)SProductName);
			mTlsIndex = TlsAlloc();
		}
	}
	
	OraWinMonitor::~OraWinMonitor() {
		
		if (mDBMonitorPtr) {
			mDBMonitorPtr.Release();
			TlsFree(mTlsIndex);
		}
		
		CoUninitialize();
	}
	
	void OraWinMonitor::onConnect(MonitorMsg& msg,
		const CRString& userName, const CRString& server)
	{
		assert(msg.monitorEvent == OraMonitor::meConnect ||
			msg.monitorEvent == OraMonitor::meDisconnect);
		
		if (!isDBMonitorActive())
			return;
		
		if (msg.tracePoint == OraMonitor::tpBefor) {
			
			if (msg.monitorEvent == OraMonitor::meConnect)
				msg.description = "Connect: " + userName + '@' + server;
			else
				msg.description = "Disconnect: " + userName + '@' + server;
			
			raiseBeforeEvent(msg.monitorEvent, msg.description);
			
			char paramStr[256];
			sprintf(paramStr, "UserName=%s\nServer=%s", userName.c_str(), server.c_str());
			long msgID = writeDBMonitorMsg(msg, paramStr);
			TlsSetValue(mTlsIndex, (void*)msgID);
		}
		else {
			msg.msgID = (long)TlsGetValue(mTlsIndex);
			raiseAfterEvent(msg.monitorEvent, msg.description);
			writeDBMonitorMsg(msg, "");
		}
	}
	
	void OraWinMonitor::onTransact(MonitorMsg& msg) {
		
		assert(msg.monitorEvent == OraMonitor::meCommit ||
			msg.monitorEvent == OraMonitor::meRollback);
		
		if (!isDBMonitorActive())
			return;
		
		if (msg.tracePoint == OraMonitor::tpBefor) {
			
			if (msg.monitorEvent == OraMonitor::meCommit)
				msg.description = "Commit";
			else
				msg.description = "Rollback";
			
			raiseBeforeEvent(msg.monitorEvent, msg.description);
			long msgID = writeDBMonitorMsg(msg, "");
			TlsSetValue(mTlsIndex, (void*)msgID);
		}
		else {
			msg.msgID = (long)TlsGetValue(mTlsIndex);
			raiseAfterEvent(msg.monitorEvent, msg.description);
			writeDBMonitorMsg(msg, "");
		}
	}
	
	void OraWinMonitor::onPrepare(MonitorMsg& msg, const CRString& sql) {
		
		assert(msg.monitorEvent == OraMonitor::mePrepare ||
			msg.monitorEvent == OraMonitor::meUnprepare);
		
		if (!isDBMonitorActive())
			return;
		
		vector<MonitorParam> dummy;
		if (msg.tracePoint == OraMonitor::tpBefor) {
			
			if (msg.monitorEvent == OraMonitor::mePrepare)
				msg.description = "Prepare: " + sql;
			else
				msg.description = "Unprepare: " + sql;
			
			raiseBeforeEvent(msg.monitorEvent, msg.description);
			long msgID = writeDBMonitorParams(msg, sql, dummy, 0);
			TlsSetValue(mTlsIndex, (void*)msgID);
		}
		else {
			msg.msgID = (long)TlsGetValue(mTlsIndex);
			raiseAfterEvent(msg.monitorEvent, msg.description);
			writeDBMonitorParams(msg, sql, dummy, 0);
		}
	}
	
	void OraWinMonitor::onExecute(MonitorMsg& msg,
		const CRString& sql, unsigned long rowsProcessed,
		const vector<MonitorParam>& params)
	{
		assert(msg.monitorEvent == OraMonitor::meExecute);
		
		if (!isDBMonitorActive())
			return;
		
		CRString paramStr;
		for (unsigned i = 0; i < params.size(); i++)
			paramStr += params[i].toString() + '\n';
		
		if (msg.tracePoint == OraMonitor::tpBefor) {
			msg.description = "Exequte: " + sql;
			raiseBeforeEvent(msg.monitorEvent, msg.description, paramStr);
			long msgID = writeDBMonitorParams(msg, sql, params, rowsProcessed);
			TlsSetValue(mTlsIndex, (void*)msgID);
		}
		else {
			msg.msgID = (long)TlsGetValue(mTlsIndex);
			raiseAfterEvent(msg.monitorEvent, msg.description, paramStr);
			writeDBMonitorParams(msg, sql, params, rowsProcessed);
		}
	}
	
	void OraWinMonitor::onBlob(MonitorMsg& msg) {
		
	}
	
	void OraWinMonitor::onError(MonitorMsg& msg, const CRString& errorMsg) {
		
		assert(msg.monitorEvent == OraMonitor::meError);
		assert(msg.tracePoint == OraMonitor::tpAfter);
		
		raiseAfterEvent(OraMonitor::meError, errorMsg);
		
		if (!isDBMonitorActive())
			return;
		
		msg.msgID = (long)TlsGetValue(mTlsIndex);
		writeDBMonitorMsg(msg, errorMsg.c_str());
	}
	
	bool OraWinMonitor::isDBMonitorActive() {
		
		long isActive = 0;
		if (mDBMonitorPtr)
			mDBMonitorPtr->IsMonitorActive(&isActive);
		
		return isActive != 0;
	}
	
	long OraWinMonitor::writeDBMonitorMsg(const MonitorMsg& msg, const char* paramStr) {
		
		TMonitorMessage outMsg;
		if (mDBMonitorPtr) {
			convertMessage(msg, outMsg);
			mDBMonitorPtr->OnEvent(&outMsg, (LPSTR)paramStr);
		}
		else
			outMsg.MessageID = 0;
		
		return outMsg.MessageID;
	}
	
	long OraWinMonitor::writeDBMonitorParams(const MonitorMsg& msg,
		const CRString& sql, const vector<MonitorParam>& params, unsigned long rowsProcessed) {
		
		TMonitorMessage outMsg;
		if (mDBMonitorPtr) {
			convertMessage(msg, outMsg);
			TSQLParam* outPrm = new TSQLParam[params.size()];
			for (unsigned i = 0; i < params.size(); i++)
				convertParam(params[i], outPrm[i]);
			mDBMonitorPtr->OnExecute(&outMsg, (LPSTR)sql.c_str(), outPrm,
				params.size(), rowsProcessed);
		}
		else
			outMsg.MessageID = 0;
		
		return outMsg.MessageID;
	}
	
	void convertMessage(const OraMonitor::MonitorMsg& msg, TMonitorMessage& outMsg) {
		
		outMsg.MessageID = msg.msgID;
		outMsg.EventType = (TEventType)msg.monitorEvent;
		outMsg.TracePoint = (TTracePoint)msg.tracePoint;
		outMsg.ObjectID = msg.objectID;
		outMsg.OwnerID = msg.ownerID;
		outMsg.ObjectName = (LPSTR)msg.objectName.c_str();
		outMsg.OwnerName = (LPSTR)msg.ownerName.c_str();
		outMsg.Description = (LPSTR)msg.description.c_str();
	}
	
	void convertParam(const OraMonitor::MonitorParam& prm, TSQLParam& outPrm) {
		
		static std::map<int, CRString> dataTypeCode;
		static std::map<int, CRString> paramTypeCode;
		
		if (dataTypeCode.size() == 0) {
			dataTypeCode[dtUnknown]        = "UNKNOWN";
			dataTypeCode[dtString]         = "VARCHAR";
			dataTypeCode[dtFixedString]    = "CHAR";
			dataTypeCode[dtInt32]          = "INTEGER";
			dataTypeCode[dtInt]            = "INTEGER";
			dataTypeCode[dtInteger]        = "INTEGER";
			dataTypeCode[dtInt64]          = "INTEGER";
			dataTypeCode[dtFloat]          = "NUMBER";
			dataTypeCode[dtDouble]         = "NUMBER";
			dataTypeCode[dtDate]           = "DATE";
			dataTypeCode[dtBlob]           = "BLOB";
			dataTypeCode[dtClob]           = "CLOB";
			dataTypeCode[ocl::dtObject]    = "OBJECT";
			dataTypeCode[ocl::dtTable]     = "TABLE";
			dataTypeCode[ocl::dtRowId]     = "ROWID";
			dataTypeCode[ocl::dtCursor]    = "CURSOR";
			dataTypeCode[ocl::dtBFile]     = "BFILE";
			dataTypeCode[ocl::dtBoolean]   = "BOOLEAN";
			dataTypeCode[ocl::dtLabel]     = "LABEL";
		}
		
		if (paramTypeCode.size() == 0) {
			paramTypeCode[ocl::pdUnknown]     = "UNKNOWN";
			paramTypeCode[ocl::pdInput]       = "IN";
			paramTypeCode[ocl::pdOutput]      = "OUT";
			paramTypeCode[ocl::pdInputOutput] = "IN/OUT";
		}
		
		outPrm.Name = (LPSTR)prm.name.c_str();
		outPrm.DataType = (LPSTR)dataTypeCode[prm.dataType].c_str();
		outPrm.Paramtype = (LPSTR)paramTypeCode[prm.type].c_str();
		outPrm.Value = (LPSTR)prm.value.c_str();
	}
	
} // namespace ocl