/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\ora.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:19 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2004 Core Lab. All right reserved.
//  Ora classes
//  Created:            02.07.98
//  Last modified:      05.11.04
//////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>

#include "ocl.conf"
#include "oracall.h"
#include "orapars.h"
#include "ora.h"

#ifdef MOCL
// for avoid type loading exception
struct OCIEnv {};
struct OCIError {};
struct OCISvcCtx {};
struct OCIStmt {};
struct OCIBind {};
struct OCIDefine {};
struct OCIDescribe {};
struct OCIServer {};
struct OCISession {};
struct OCIComplexObject {};
struct OCITrans {};
struct OCISecurity {};
struct OCISubscription {};

struct OCISnapshot {};
struct OCIResult {};
struct OCILobLocator {};
struct OCIParam {};
struct OCIComplexObjectComp {};
struct OCIInterval {};
struct OCIDateTime {};
#endif

namespace ocl {
	
	const char SCheckOCI[]               = "OCI is not inizialized";
	const char SCheckOCI73[]             = "Need Oracle 7.3 Call Inteface";
	const char SCheckOCI80[]             = "Need Oracle 8 Call Inteface";
	const char SUnknownDataType[]        = "Unknown data type: [%d]";
	const char SArrayParam[]             = "%s parameter must be array with length %d";
	const char SUnsupportedDataType[]    = "Unsupported data type";
	const char SNotEnoughInputParams[]   = "Not enough Input params";
	const char STooManyInputParams[]     = "Too many Input params";
	const char STooNanyOutputParams[]    = "Too many Output params";
	const char SNotEnoughOutputParams[]  = "Not enough Output params";
	const char SNeedParamType[]          = "Lob locator should be Input or Output";
	const char SNotRows[]                = "SQL statement doesn't return rows";
	const char SCannotConvert[]          = "Cannot convert to %s";
	const char SObjectNotExist[]         = "Object not exist: %s";
	const char SRecSetNotClosed[]        = "Previously opened Recordset was not closed";
	const char SStillExecuting[]         = "Previously query still executing";
	const char SRowsNotReturn[]          = "SQL statement doesn't return rows";
	const char SBufferMustBeNull[]       = "For reference BufferSize must be 0";
	const char SBufferCannotChange[]     = "BufferSize connot be changed for this type";
	const char SIndexIsNotRange[]        = "Index of array is not range";
	const char STypeCanNotConsistTable[] = "This type cannot consist in table";
	const char SSessionNotDef[]          = "Session is not defined";
	const char SSessionNotConnected[]    = "Session is not connected";
	const char SCursorNotOpened[]        = "Cursor must be opened";
	const char SCursorOpened[]           = "Cursor has been opened";
	const char SUnkParamType[]           = "Parameter % is of unknown type";
	const char SLOBNotAllocated[]        = "LOB locator not allocatted";
	const char SLOBNotInited[]           = "LOB was not inited";
	const char SLOBSvcCtxNotInit[]       = "LOB service context was not initalized";
	const char SRefreshNotAllowed[]      = "Refresh is not allowed";
	const char SCannotChange[]           = "Can not change while connection is open";
	const char SConnAlreadyAttach[]      = "Connection already attached to transaction";
	const char SConnNotAttach[]          = "Connection not attach to transaction";
	const char SGTransIdToLarge[]        = "Global transaction ID length too large";
	const char SBufferTooLarge[]         = "Buffer too large";
	const char TnsNamesFileNotFound[]    = "File tnsnames.ora not found";
	const char InvalidHomeIndex[]        = "Invalid Oracle home index";
	const char InvalidHomeName[]         = "Invalid Oracle home name";
	const char InvalidDatatype[]         = "Invalid data type was passed";
	const char TimeStampTypeNotSpec[]    = "TIMESTAMP type was not specified";
	const char IntervalTypeNotSpec[]     = "INTERVAL type was not specified";
	const char InvalidTimeStampField[]   = "Invalid TIMESTAMP field was specified";
	const char InvalidIntervalField[]    = "Invalid TIMESTAMP field was specified";
	const char STableNameNotDef[]        = "Table name must be defined";
	const char SStoredProcNameNotDef[]   = "Stored procedure name must be defined";
	
	const long maxBlobSize = 2147483647;
	const int rowIdSize = 18;
	
#if !defined(OCL_DYNAMIC) && defined(OCL_OCI8)
	OCIVersion OraConnection::mOCICallStyle = OCI80;
	OCIVersion OraCommand::mOCICallStyle    = OCI80;
	OCIVersion OraCursor::mOCICallStyle     = OCI80;
#elif !defined(OCL_DYNAMIC) && defined(OCL_OCI7)
	OCIVersion OraConnection::mOCICallStyle = OCI73;
	OCIVersion OraCommand::mOCICallStyle    = OCI73;
	OCIVersion OraCursor::mOCICallStyle     = OCI73;
#endif
	
	// Helper functions for SQL parsing
	int getColumnsInfo(const CRString& sql, ColumnInfo columnsInfo[]);
	int getTablesInfo(const CRString& sql, TableInfo tablesInfo[]);
	
	// Date convertion
	
	void oraDateToCRDate(void* oraDate, CRDate& date) {
		byte* d = (byte*)oraDate;
		
		date = CRDate((d[0] - 100) * 100 + d[1] - 100, d[2],
			d[3], d[4] - 1, d[5] - 1, d[6] - 1);
	}
	
	void CRDateToOraDate(CRDate date, void* oraDate) {
		byte* d = (byte*)oraDate;
		
		d[0] = (byte)(date.year() / 100 + 100);
		d[1] = (byte)(date.year() % 100 + 100);
		d[2] = (byte)date.month();
		d[3] = (byte)date.day();
		d[4] = (byte)(date.hour() + 1);
		d[5] = (byte)(date.minute() + 1);
		d[6] = (byte)(date.second() + 1);
	}
	
	// RowId convertion
	
	long shift(long value) {
		long result = value;
		
		if (result != 0)
			if ((result & 0xFF) == 0)
				result = result >> 8;
			
			return result;
	}
	
	word reverse2(word value) {
		word result;
		
		((byte*)&result)[0] = ((byte*)&value)[1];
		((byte*)&result)[1] = ((byte*)&value)[0];
		return result;
	}
	
	long reverse4(long value) {
		long result;
		
		((byte*)&result)[0] = ((byte*)&value)[3];
		((byte*)&result)[1] = ((byte*)&value)[2];
		((byte*)&result)[2] = ((byte*)&value)[1];
		((byte*)&result)[3] = ((byte*)&value)[0];
		return result;
	}
	
	CRString intToSym(long value, int len) {
		const int base = 64;
		char st[10];
		int i,n;
		
		st[len] = 0;
		i = len - 1;
		
		while (value) {
			n = value % base;
			
			if (n < 26)
				st[i--] = (char)(n + 65); // + St;
			else
				if (n < 52)
					st[i--] = (char)(n - 26 + 97);
				else
					if (n < 62)
						st[i--] = (char)(n - 52 + 48);
					else
						if (n == 62)
							st[i--] = 43;
						else
							if (n == 63)
								st[i--] = 47;
							else
								assert(false);
							
							value = value / base;
		};
		
		while (i >= 0)
			st[i--]= 65;
		
		return CRString(st);
	}
	
	//////////////////  OraException  ///////////////////////
	
	OraException::OraException(const CRString& msg) :
	CRException(msg)
	{
		mErrorCode = 0;
	}
	
	OraException::OraException(int errorCode, const CRString& msg) :
	CRException(msg)
	{
		mErrorCode = errorCode;
	}
	
	int OraException::errorCode() {
		
		return mErrorCode;
	}
	
	
	///////////////////////   OraTransaction   /////////////////////
	
#ifdef OCL_OCI8
	
	unsigned char OraTransaction::mNextIdValue = 1;
	
	OraTransaction::OraTransaction() 
	{
		
		_init();
		
		mXid.formatID = 1000;
		mXid.gtridLength = 3;
		mXid.bqualLength = 1;
		
		mXid.data[0] = 1;
		mXid.data[1] = 2;
		mXid.data[2] = 3;
	}
	
	OraTransaction::OraTransaction(char gId[], int gIdLength) {
		
		_init();
		
		if (gIdLength > 128)
			throw CRException(SGTransIdToLarge);
		
		mXid.formatID = 1000;
		mXid.gtridLength = gIdLength;
		mXid.bqualLength = 1;
		
		for (int i = 0; i < gIdLength; i++)
			mXid.data[i] = gId[i];
	}
	
	OraTransaction::~OraTransaction() {
		
		for (unsigned i = 0; i < mConnections.size(); i++) {
			if (mConnections[i]->getOCISvcCtx()) {
				check(OCITransDetach(mConnections[i]->getOCISvcCtx(),
					OraEnvironment::getOCIError(), OCI_DEFAULT));
				check(OCIHandleFree(mOCITrans[i], OCI_HTYPE_TRANS));
			}
			mConnections[i]->mTransaction = 0;
		}
	}
	
	void OraTransaction::_init() {
		memset(&mXid, 0, sizeof(XID));
		mTimeout = 60;
	}
	
	
	void OraTransaction::setTimeout(int timeout) {
		mTimeout = timeout;
	}
	
	int OraTransaction::timeout() const {
		return mTimeout;
	}
	
	void OraTransaction::attachConnection(OraConnection* conn) {
		
#ifdef _DEBUG
		for (unsigned i = 0; i < mConnections.size(); i++)
			if (mConnections[i] == conn)
				throw CRException(SConnAlreadyAttach);
#endif
			
			OCITrans* trans;
			
			check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&trans,
				OCI_HTYPE_TRANS, 0, 0));
			
			// Create new branch within this transaction    
			mXid.data[3] = mNextIdValue;
			mNextIdValue++;
			
			check(OCIAttrSet(trans, OCI_HTYPE_TRANS, &mXid, sizeof(XID),
				OCI_ATTR_XID, OraEnvironment::getOCIError()));
			
			check(OCIAttrSet(conn->getOCISvcCtx(), OCI_HTYPE_SVCCTX, trans, 0,
				OCI_ATTR_TRANS, OraEnvironment::getOCIError()));
			
			mOCITrans.push_back(trans);
			mConnections.push_back(conn);
	}
	
	void OraTransaction::detachConnection(OraConnection* conn) {
		
		if (mConnections.empty())
		{
			return;
		}
		std::vector<OraConnection*>::iterator connIter  = mConnections.begin();
		std::vector<OCITrans*>::iterator      transIter = mOCITrans.begin();
		
		for (; connIter != mConnections.end(); connIter++, transIter++) {
			if (*connIter == conn && conn->getOCISvcCtx()) {
				check(OCITransDetach(conn->getOCISvcCtx(),
					OraEnvironment::getOCIError(), OCI_DEFAULT));
				check(OCIHandleFree(*transIter, OCI_HTYPE_TRANS));
				
				mOCITrans.erase(transIter);
				mConnections.erase(connIter);
				break;
			}
		}
	}
	
	void OraTransaction::startTransaction(OraConnection* conn) {
		
		bool found = false;
		
		for (unsigned i = 0; i < mConnections.size(); i++)
			if (mConnections[i] == conn)
				found = true;
			
			if (!found)
				throw CRException(SConnNotAttach);
			
			check(OCITransStart(conn->getOCISvcCtx(),
				OraEnvironment::getOCIError(), mTimeout, OCI_TRANS_NEW));
			
			if (mCoordName.empty())
				mCoordName = conn->server();
	}
	
	void OraTransaction::commit() {
		
		bool doCommit = true;
		CRString errorMsg;
		sword res, errorCode;
		
		for (unsigned i = 0; i < mConnections.size(); i++) {
			if (mConnections[i]->server() != mCoordName)
				res = OCITransPrepare(mConnections[i]->getOCISvcCtx(), OraEnvironment::getOCIError(), 0);
			else
				res = OCI_SUCCESS;
			
			if (res != OCI_SUCCESS && res != OCI_SUCCESS_WITH_INFO) {
				doCommit = false;
				errorCode = OraEnvironment::getOraError(res, errorMsg, OraEnvironment::getOCIError());
			}
		}
		
		if (doCommit) {
			ub4 commitType = OCI_DEFAULT;
			
			for (unsigned ii = 0; ii < mConnections.size(); ii++) {
				if (mConnections[ii]->server() != mCoordName)
					commitType = OCI_TRANS_TWOPHASE;
				
				check(OCITransCommit(mConnections[ii]->getOCISvcCtx(),
					OraEnvironment::getOCIError(), commitType));
			}
		}
		else
			for (unsigned ii = 0; ii < mConnections.size(); ii++)
				check(OCITransRollback(mConnections[ii]->getOCISvcCtx(),
				OraEnvironment::getOCIError(), 0));
			
			// Reset global coordinator name
			mCoordName.erase();
			
			if (!doCommit)
				throw OraException(errorCode, errorMsg);
	}
	
	void OraTransaction::rollback() {
		
		try {
			for (unsigned i = 0; i < mConnections.size(); i++)
				check(OCITransRollback(mConnections[i]->getOCISvcCtx(),
				OraEnvironment::getOCIError(), 0));
			
			// Reset global coordinator name
			mCoordName.erase();
		}
		catch (OraException&) {
			// Reset global coordinator name
			mCoordName.erase();
			
			throw;
		}
	}
	
#endif // OCL_OCI8
	
	///////////////////////   OraConnection   /////////////////////
	
	// OraConnections existing in the application
	CRMutex mutexConnections;
	CRList<OraConnection> connections;
	
	int OraConnection::mHome = -1;
	
	OraConnection::OraConnection() {
		
		_init();
	}
	
	OraConnection::OraConnection(const CRString& connectString) {
		
		_init();
		
		setConnectString(connectString);
	}
	
	OraConnection::OraConnection(const CRString& username,
		const CRString& password, const CRString& server) {
		
		_init();
		
		mUsername = username;
		mPassword = password;
		mServer   = server;
	}
	
	void OraConnection::_init() {
		
		mIsAttachedServer=false;
		
		mLDA = 0;
		mHDA = 0;
		
#ifdef OCL_OCI8
		hOCISvcCtx  = 0;
		hOCIServer  = 0;
		hOCISession = 0;
		mConnectMode = cmNormal;
		mTransaction  = 0;
#endif
		
		mConnected      = false;
		mNativeSvcCtx   = true;
		mThreadSafety   = true;
		mAutoCommit     = false;
		mInTransaction  = false;
		mIsolationLevel = ilReadCommited;
		mCharsetId      = 0;
		
		mOCICallStyle = OraEnvironment::OCICallStyle();
		mOCIVersion   = OraEnvironment::OCICallStyle();
		
		// Add this object to existing connection array
		{
			CRThreadGuard theGuard( mutexConnections );
			connections.add(this);
		}
		
#ifdef OCL_WIN32
		hMutex = CreateMutex(0, false, 0);
#endif
		
		/*
		mBeforeConnect = 0;
		mAfterConnect = 0;
		mBeforeDisconnect = 0;
		mAfterDisconnect = 0;
		*/
	}
	
	OraConnection::~OraConnection() {
		
#ifdef OCL_OCI8
		if (mTransaction) 
		{
			mTransaction->detachConnection(this);
			mTransaction = 0;
		}
#endif
		
		close();
		
		// Remove this object from existing connection array
		{
			CRThreadGuard theGuard( mutexConnections );
			connections.remove(this);
		}
		
#ifdef OCL_WIN32
			CloseHandle(hMutex);
#endif
	}
	
	CRString OraConnection::home() {
		
		if (mHome == -1)
			return OraEnvironment::oracleHomeName(OraEnvironment::defaultOracleHome());
		
		return OraEnvironment::oracleHomeName(mHome);
	}
	
	CRString OraConnection::homePath() {
		
		if (mHome == -1)
			return OraEnvironment::oracleHomePath(OraEnvironment::defaultOracleHome());
		
		return OraEnvironment::oracleHomePath(mHome);
	}
	
	void OraConnection::setHome(int homeIndex) {
		
		if (homeIndex < 0 || homeIndex >= OraEnvironment::oracleHomeCount())
			throw CRException(InvalidHomeIndex);
		
		if (homeIndex != mHome) 
		{
			mHome = homeIndex;
			
			// Disconnect all connections
			{
				CRThreadGuard theGuard( mutexConnections );

				try 
				{
					for (int i = 0; i < connections.count(); i++)
						connections[i].close();
				}
				catch (CRException) 
				{
					; // Suppress all OCL exceptions
				}
			}
			
#ifdef OCL_DYNAMIC
			if (OraEnvironment::isOCIInited())
				OraEnvironment::freeOCI();
#endif
			
			OraEnvironment::setOracleHome(mHome);
		}
	}
	
	void OraConnection::setHome(const CRString& homeName) {
		
		bool isFounded = false;
		int homeIndex;
		for (int i = 0; i < OraEnvironment::oracleHomeCount(); i++)
			if (homeName == OraEnvironment::oracleHomeName(i)) {
				isFounded = true;
				homeIndex = i;
				break;
			}
			
			if (!isFounded)
				throw CRException(InvalidHomeName);
			
			setHome(homeIndex);
	}
	
	void OraConnection::setDefaultHome() {
		
		// Disconnect all connections
		{
			CRThreadGuard theGuard( mutexConnections );

			try 
			{
				for (int i = 0; i < connections.count(); i++)
					connections[i].close();
			}
			catch (CRException) 
			{
				; // Suppress all OCL exceptions
			}
		}
		
#ifdef OCL_DYNAMIC
		if (OraEnvironment::isOCIInited())
			OraEnvironment::freeOCI();
#endif
		
		mHome = -1;
		OraEnvironment::useDefaultOracleHome();
	}
	
	void OraConnection::setUsername(const CRString& value) {
		
		if (mConnected)
			close();
		
		mUsername = value;
	}
	
	void OraConnection::setPassword(const CRString& value) {
		
		if (mConnected)
			close();
		
		mPassword = value;  
	}
	
	void OraConnection::setServer(const CRString& value) {
		
		if (mConnected)
			close();
		
		mServer = value;  
	}
	
	CRString OraConnection::serverVersion() {
		
		CRString serverVersion;
		
		if (!mConnected)
			return "";
		
#ifdef OCL_OCI8
		char version[255];
		
		if (mOCICallStyle >= OCI80) {
			
			check(OCIServerVersion(hOCISvcCtx, OraEnvironment::getOCIError(), (text*)version, 255,
				OCI_HTYPE_SVCCTX));
			
			CRString rawVersion = version;
			
			CRString::size_type dotPos = rawVersion.find('.');
			
			if (dotPos != CRString::npos) {
				CRString::size_type startVerPos = rawVersion.find_last_of(' ', dotPos);
				CRString::size_type endVerPos = rawVersion.find(' ', startVerPos + 1);
				if (startVerPos != CRString::npos && endVerPos != CRString::npos)
					serverVersion = rawVersion.substr(startVerPos + 1, endVerPos - startVerPos - 1);
			}
		}
#endif // OCL_OCI8
		
		return serverVersion;
	}
	
	void OraConnection::setConnectString(const CRString& value) {
		const char* st;
		const char* ch;
		char username[30], password[30], server[30];
		
		username[0] = 0;
		password[0] = 0;
		server[0] = 0;
		
		if (value.empty())
			return;
		
		st = value.c_str();
		ch = strchr(st, '/');
		
		if (ch) {
			strncpy(username, st, ch - st);
			username[ch - st] = 0;
			st = ch + 1;
		}
		
		ch = strchr(st, '@');
		if (ch) {
			if (!username[0]) {
				strncpy(username, st, ch - st);
				username[ch - st] = 0;
			}
			else {
				strncpy(password, st, ch - st);
				password[ch - st] = 0;
			}
			
			st = ch + 1;
			strcpy(server, st);
		}
		else
			if (!username[0])
				strcpy(username, st);
			else
				strcpy(password, st);
			
			mUsername = username;
			mPassword = password;
			mServer   = server;
	}
	
#ifdef OCL_OCI8
	void OraConnection::setConnectMode(CRConnectMode connectMode) {
		mConnectMode = connectMode;
	}
#endif
	
	void OraConnection::open(const CRString& connectString) {
		
		if (mConnected)
			return;
		
		if (!OraEnvironment::isOCIInited())
			OraEnvironment::initOCI();
		
		if (!connectString.empty())
			setConnectString(connectString);
		
			/*
			if (mBeforeConnect)
			mBeforeConnect(*this);
		*/
		
		if (mOCICallStyle == None)
			mOCICallStyle = OraEnvironment::OCICallStyle();
		
		if (mOCIVersion == None)
			mOCIVersion = mOCICallStyle;
		
		trace(OraMonitor::meConnect, OraMonitor::tpBefor);
		//bool attachedServer=false; //replace to member variable	 mIsAttachedServer
		
		switch (mOCICallStyle) {
#ifdef OCL_OCI7    
		case OCI73:
			if (!mLDA)
				mLDA = (LDA*)malloc(sizeof(LDA));
			
			if (!mHDA)
				mHDA = (HDA*)malloc(sizeof(HDA));
			
			memset(mHDA, 0, sizeof(HDA));
			
			try {
				check(olog(mLDA, (unsigned char*)mHDA, (text*)mUsername.c_str(), -1,
					(text*)mPassword.c_str(), -1, (text*)mServer.c_str(), -1, OCI_LM_DEF));
			}
			catch (OraException &e )//jianghp, added
			{
				free(mHDA);
				free(mLDA);
				
				mLDA = 0;
				mHDA = 0;
				
				throw e;
			}
			catch (...) {
				free(mHDA);
				free(mLDA);
				
				mLDA = 0;
				mHDA = 0;
				
				throw;
			}
			break;
#endif
#ifdef OCL_OCI8
		case OCI80:
			try {
				check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hOCIServer,
					OCI_HTYPE_SERVER, 0, 0));
				
				check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hOCISvcCtx,
					OCI_HTYPE_SVCCTX, 0, 0));
				
				check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hOCISession,
					OCI_HTYPE_SESSION, 0, 0));
				
				check(OCIServerAttach(hOCIServer, OraEnvironment::getOCIError(), (text*)mServer.c_str(), mServer.length(),
					OCI_DEFAULT));

				mIsAttachedServer = true;
				
				if (mTransaction) {
					CRString internalName;
					
					if (mInternalName.empty())
						internalName = server();
					
					check(OCIAttrSet(hOCIServer, OCI_HTYPE_SERVER, (void*)internalName.c_str(),
						internalName.length(), OCI_ATTR_INTERNAL_NAME, OraEnvironment::getOCIError()));
					
					if (!mExternalName.empty())
						check(OCIAttrSet(hOCIServer, OCI_HTYPE_SERVER, (void*)mExternalName.c_str(),
						mExternalName.length(), OCI_ATTR_EXTERNAL_NAME, OraEnvironment::getOCIError()));
				}
				
				check(OCIAttrSet(hOCISvcCtx, OCI_HTYPE_SVCCTX, hOCIServer, 0,
					OCI_ATTR_SERVER, OraEnvironment::getOCIError()));
				
				ub4 cred;
				if (!mUsername.empty()) {
					check(OCIAttrSet(hOCISession, OCI_HTYPE_SESSION, (void*)mUsername.c_str(),
						mUsername.length(), OCI_ATTR_USERNAME, OraEnvironment::getOCIError()));
					check(OCIAttrSet(hOCISession, OCI_HTYPE_SESSION, (void*)mPassword.c_str(),
						mPassword.length(), OCI_ATTR_PASSWORD, OraEnvironment::getOCIError()));
					cred = OCI_CRED_RDBMS;
				}
				else
					cred = OCI_CRED_EXT;
				
				ub4 mode;
				switch (mConnectMode) {
				case cmSysOper:
					mode = OCI_SYSOPER;
					break;
				case cmSysDBA:
					mode = OCI_SYSDBA;
					break;
				default:
					mode = OCI_DEFAULT;
				}
				
				check(OCISessionBegin(hOCISvcCtx, OraEnvironment::getOCIError(), hOCISession,
					cred, mode));
				
				check(OCIAttrSet(hOCISvcCtx, OCI_HTYPE_SVCCTX, hOCISession,
					0, OCI_ATTR_SESSION, OraEnvironment::getOCIError()));
				
				mConnected = true; // for serverVersion();
				
				CRString sVer = serverVersion();
				if (sVer[0] == '7')
					mOCIVersion = OCI73;
				else
					if (mTransaction)
						mTransaction->attachConnection(this);
			}
			catch (OraException &e )//jianghp, added
			{
		
				if(mIsAttachedServer)
				{
					OCIServerDetach(hOCIServer, OraEnvironment::getOCIError(), (ub4)OCI_DEFAULT);
					mIsAttachedServer = false;
				}
				
				if (hOCISvcCtx)
				{
					OCIHandleFree(hOCISvcCtx, OCI_HTYPE_SVCCTX);
				}
				if (hOCISession)
				{
					OCIHandleFree(hOCISession, OCI_HTYPE_SESSION);
				}
				
				if (hOCIServer)
				{
					
					OCIHandleFree(hOCIServer, OCI_HTYPE_SERVER);
				}				
				
				hOCIServer  = 0;
				hOCISvcCtx  = 0;
				hOCISession = 0;
				
				throw e;
			}
			catch (...) {
			 
				if(mIsAttachedServer)
				{
					OCIServerDetach(hOCIServer, OraEnvironment::getOCIError(), (ub4)OCI_DEFAULT);
					mIsAttachedServer = false;
				}

				if (hOCISvcCtx)
				{
					OCIHandleFree(hOCISvcCtx, OCI_HTYPE_SVCCTX);
				}
				
				if (hOCISession)
				{
					OCIHandleFree(hOCISession, OCI_HTYPE_SESSION);
				}
				
				if (hOCIServer)
				{
				 
					OCIHandleFree(hOCIServer, OCI_HTYPE_SERVER);
				}				
				
				hOCIServer  = 0;
				hOCISvcCtx  = 0;
				hOCISession = 0;
				
				throw;
			}
#endif
  }
  
  trace(OraMonitor::meConnect, OraMonitor::tpAfter);  
  
  mConnected = true;
}

void OraConnection::close() {
	
	if (!mConnected)
		return;

    {
        // Thread safe mCommands
        CRThreadGuard theGurad( mCommandsMutex );
	    for (int i = 0; i < mCommands.count(); i++) {
		    try {
			    mCommands[i].disconnect();
		    }
		    catch (OraException& e) { 
			    if (e.errorCode() != OCI_NOT_CONNECTED  &&
				    e.errorCode() != OCI_NOTLOGGEDON    &&
				    e.errorCode() != OCI_SESSION_KILLED &&
				    e.errorCode() != OCI_NO_INTERFACE)    
				    throw;
		    }
	    }
    }
	
	//  mCommands.clear();
	
	mConnected = false;
	
	trace(OraMonitor::meDisconnect, OraMonitor::tpBefor);
	
	if (!inTransaction())
		commit();
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
    case OCI73:
		try {
			if (mNativeSvcCtx) {
				check(ologof(mLDA));          
				
				free(mHDA);
				free(mLDA);
				
				mLDA = 0;
				mHDA = 0;
			}
		}
		catch (OraException& e) {
			if (mNativeSvcCtx) {
				free(mHDA);
				free(mLDA);
				
				mLDA = 0;
				mHDA = 0;
			}
			
			if (e.errorCode() != OCI_NOT_CONNECTED &&
				e.errorCode() != OCI_SESSION_KILLED &&
				e.errorCode() != OCI_NO_INTERFACE)    
				throw;
        }
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		try {
			if (mNativeSvcCtx) {
				check(OCISessionEnd(hOCISvcCtx, OraEnvironment::getOCIError(), hOCISession, 0));
				
				if (mIsAttachedServer)
				{
					check(OCIServerDetach(hOCIServer, OraEnvironment::getOCIError(), (ub4)OCI_DEFAULT));
					mIsAttachedServer = false; 			
				}
				
				if (hOCISvcCtx)
					OCIHandleFree(hOCISvcCtx, OCI_HTYPE_SVCCTX);
				
				if (hOCIServer)
					OCIHandleFree(hOCIServer, OCI_HTYPE_SERVER);
				
				if (hOCISession)
					OCIHandleFree(hOCISession, OCI_HTYPE_SESSION);
				
				hOCIServer =  0;
				hOCISvcCtx =  0;
				hOCISession = 0;          
			}
		}
		catch (OraException& e) {
			if (mNativeSvcCtx) {
			
			
				if(mIsAttachedServer)
				{
					OCIServerDetach(hOCIServer, OraEnvironment::getOCIError(), (ub4)OCI_DEFAULT);
					mIsAttachedServer = false;
				}
				
				if (hOCISession)
					OCIHandleFree(hOCISession, OCI_HTYPE_SESSION);
				
				if (hOCISvcCtx && mNativeSvcCtx)
					OCIHandleFree(hOCISvcCtx, OCI_HTYPE_SVCCTX);
				
				if (hOCIServer)
					OCIHandleFree(hOCIServer, OCI_HTYPE_SERVER);
				
				hOCIServer =  0;
				hOCISvcCtx =  0;
				hOCISession = 0;
			}
			if (e.errorCode() != OCI_NOT_CONNECTED &&
				e.errorCode() != OCI_SESSION_KILLED &&
				e.errorCode() != OCI_NO_INTERFACE)    
				throw;
		}
		break;
#endif
	}
	
	trace(OraMonitor::meDisconnect, OraMonitor::tpAfter);  
}

#ifdef OCL_OCI8
void OraConnection::changePassword(const CRString& newPassword) {
	
	if (!OraEnvironment::isOCIInited())
		OraEnvironment::initOCI();
	
	if (mOCICallStyle == None)
		mOCICallStyle = OraEnvironment::OCICallStyle();
	
	checkOCI80();
	
	OCISvcCtx* hSvcCtx;
	OCIServer* hServer;
	OCISession* hSession;
	
	if (mConnected) {
		hSvcCtx = hOCISvcCtx;
		hServer = hOCIServer;
		hSession = hOCISession;
	}
	else {
		check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hSvcCtx, OCI_HTYPE_SVCCTX, 0, 0));
		check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hServer, OCI_HTYPE_SERVER, 0, 0));
		check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void**)&hSession, OCI_HTYPE_SESSION, 0, 0));
	}
	
	try {
		if (!mConnected)
		{
			check(OCIServerAttach(hServer, OraEnvironment::getOCIError(), (OraText*)mServer.c_str(), mServer.length(), OCI_DEFAULT));
			mIsAttachedServer = true;
		}
		try {
			if (!mConnected) {
				check(OCIAttrSet(hSvcCtx, OCI_HTYPE_SVCCTX, hServer, 0, OCI_ATTR_SERVER, OraEnvironment::getOCIError()));
				check(OCIAttrSet(hSvcCtx, OCI_HTYPE_SVCCTX, hSession, 0, OCI_ATTR_SESSION, OraEnvironment::getOCIError()));
			}
			
			check(OCIPasswordChange(hSvcCtx, OraEnvironment::getOCIError(), (OraText*)mUsername.c_str(),
				mUsername.length(), (OraText*)mPassword.c_str(), mPassword.length(),
				(OraText*)newPassword.c_str(), newPassword.length(), OCI_AUTH));
			
			if (!mConnected)
				check(OCISessionEnd(hSvcCtx, OraEnvironment::getOCIError(), hSession, OCI_DEFAULT));
			
			//finnaly
			if (!mConnected)
			{
				if (mIsAttachedServer)
				{
					check(OCIServerDetach(hServer, OraEnvironment::getOCIError(), OCI_DEFAULT));
				    mIsAttachedServer = false;
				}
			}
		}
		catch (OraException&) {
			//finnaly
			if (!mConnected)
			{
				if (mIsAttachedServer)
				{
					check(OCIServerDetach(hServer, OraEnvironment::getOCIError(), OCI_DEFAULT));
					mIsAttachedServer = false;
				}
			}
			throw;
		}
		
		//finnaly
		if (!mConnected) {
			check(OCIHandleFree(hSession, OCI_HTYPE_SESSION));
			check(OCIHandleFree(hServer, OCI_HTYPE_SERVER));
			check(OCIHandleFree(hSvcCtx, OCI_HTYPE_SVCCTX));
		}
	}
	catch (OraException&) {
		
		//finnaly
		if (!mConnected) {
			check(OCIHandleFree(hSession, OCI_HTYPE_SESSION));
			check(OCIHandleFree(hServer, OCI_HTYPE_SERVER));
			check(OCIHandleFree(hSvcCtx, OCI_HTYPE_SVCCTX));
		}
		
		throw;
	}
}
#endif // OCL_OCI8

// Transaction control

#ifdef OCL_OCI8

OraTransaction* OraConnection::transaction() const {
	
	checkOCI80();
	
	return mTransaction;
}

void OraConnection::setTransaction(OraTransaction* trans) {
	
	// checkOCI80(); May be OCI not inited
	
	if (mTransaction != trans) {
		// Detach from previous transaction
		if (mTransaction)
			mTransaction->detachConnection(this);
		
		mTransaction = trans;
	}
}

CRString OraConnection::internalName() const {
	
	checkOCI80();
	
	return mInternalName;
}

void OraConnection::setInternalName(const CRString& internalName) {
	
	checkOCI80();
	
	if (mConnected)
		throw CRException(SCannotChange);
    
	mInternalName = internalName;
}

CRString OraConnection::externalName() const {
	
	checkOCI80();
	
	return mExternalName;
}

void OraConnection::setExternalName(const CRString& externalName) {
	
	checkOCI80();
	
	if (mConnected)
		throw CRException(SCannotChange);
    
	mExternalName = externalName;
}

#endif // OCL_OCI8

void OraConnection::startTransaction() {
	
	if (!mConnected)
		throw CRException(SSessionNotConnected);
	
#ifdef OCL_OCI8
	if (mTransaction)
		mTransaction->startTransaction(this);
	else
#endif
		commit();
    
	// SET TRANSACTION
	mInTransaction = true;
}

void OraConnection::startTransaction(CRIsolationLevel level) {
	
	if (mConnected && (level != mIsolationLevel)) {
		
		if (mInTransaction)
			commit();
		
		OraCommand oraSql(*this);
		
		CRString sql("SET TRANSACTION ");
		CRString strLevel;
		
		switch (level) {
		case ilReadCommited:
			strLevel = CRString("ISOLATION LEVEL ") + "READ COMMITTED";
			mIsolationLevel = level;
			break;
			
		case ilReadOnly:
			strLevel = "READ ONLY";
			mIsolationLevel = level;
			break;
			
		case ilSerializable:
			strLevel = CRString("ISOLATION LEVEL ") + "SERIALIZABLE";
			mIsolationLevel = level;
			break;
			
		default:
			strLevel = CRString("ISOLATION LEVEL ") + "READ COMMITTED";
			mIsolationLevel = ilReadCommited;
		}
		
		sql += strLevel;
		
		oraSql.setSQL(sql.c_str());
		oraSql.execute();
	}  
}

void OraConnection::commit() {
	
	if (!mConnected)
		return;
	
	trace(OraMonitor::meCommit, OraMonitor::tpBefor);  
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
		
    case OCI73:
		check(ocom(mLDA));
		break;
		
#endif
#ifdef OCL_OCI8
		
    case OCI80:
#ifdef OCL_OCI8
		if (mTransaction)
			mTransaction->commit();
		else
#endif
			check(OCITransCommit(hOCISvcCtx, OraEnvironment::getOCIError(), OCI_DEFAULT));
		break;
		
#endif
	}
	
	trace(OraMonitor::meCommit, OraMonitor::tpAfter);
	mInTransaction = false;
}

void OraConnection::rollback() {
	
	if (!mConnected)
		return;
	
	trace(OraMonitor::meRollback, OraMonitor::tpBefor);  
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
		
    case OCI73:
		check(orol(mLDA));
		break;
		
#endif
#ifdef OCL_OCI8
		
    case OCI80:
#ifdef OCL_OCI8
		if (mTransaction)
			mTransaction->rollback();
		else
#endif
			check(OCITransRollback(hOCISvcCtx, OraEnvironment::getOCIError(), OCI_DEFAULT));
		break;
		
#endif
	}
	
	trace(OraMonitor::meRollback, OraMonitor::tpAfter);
	mInTransaction = false;
}

void OraConnection::savepoint(const CRString& savepoint) {
	
	if (mConnected) {
		OraCommand sql(*this, "SAVEPOINT " + savepoint);
		sql.execute();
	}
}

void OraConnection::rollbackToSavepoint(const CRString& savepoint) {
	
	if (mConnected) {
		OraCommand sql(*this, "ROLLBACK TO SAVEPOINT " + savepoint);
		sql.execute();
	}  
}

void OraConnection::setIsolationLevel(CRIsolationLevel value) {
	
	if (mConnected && (value != mIsolationLevel)) {
		
		if (mInTransaction)
			commit();
		
		mInTransaction = false;
		
		OraCommand oraSql(*this);
		
		CRString sql("ALTER SESSION SET ISOLATION_LEVEL = ");
		CRString strLevel;
		
		switch (value) {
		case ilReadOnly:
			strLevel = "READ ONLY";
			mIsolationLevel = value;
			break;
			
		case ilSerializable:
			strLevel = "SERIALIZABLE";
			mIsolationLevel = value;
			break;
			
		default:
			strLevel = "READ COMMITTED";
			mIsolationLevel = ilReadCommited;
		}
		
		sql += strLevel;
		
		oraSql.setSQL(sql.c_str());
		oraSql.execute();
	}
}

OraCommand* OraConnection::createCommand() {
	
	OraCommand* command = new OraCommand(*this);
	
	return command;
}

void OraConnection::addCommand( OraCommand* pCommand )
{
    if ( NULL == pCommand )
    {
        return;
    }

    {
        // Thread safe mCommands
        CRThreadGuard theGuard( mCommandsMutex );
        mCommands.add( pCommand );
    }
}

void OraConnection::removeCommand( OraCommand* pCommand )
{
    if ( NULL == pCommand )
    {
        return;
    }

    {
        // Thread safe mCommands
        CRThreadGuard theGuard( mCommandsMutex );
        mCommands.remove( pCommand );
    }
}

// Multi Thread

void OraConnection::busy() {
	
#ifdef OCL_WIN32
	
	if (mThreadSafety) {
		switch (WaitForSingleObject(hMutex, 0)) {
		case WAIT_OBJECT_0:
			return;
			
		case WAIT_TIMEOUT:
			throw OraException(OCI_CONNECTION_BUSY, "ORA-03127a");
			
		default:
			assert(false);
		}
	}
	
#endif
	
}

void OraConnection::busyWait() {
	
#ifdef OCL_WIN32
	if (mThreadSafety) {
		switch (WaitForSingleObject(hMutex, INFINITE)) {
		case WAIT_OBJECT_0:
			return;
			
		case WAIT_TIMEOUT:
			throw OraException(OCI_CONNECTION_BUSY, "ORA-03127a");
			
		case WAIT_ABANDONED:
			return;
			
		default:
			assert(false);
		}
	}
#endif
}

void OraConnection::release() {
	
#ifdef OCL_WIN32
	if (mThreadSafety)
	{
		ReleaseMutex(hMutex);
	}
#endif
}

void OraConnection::breakExec() {
	
	if (!mConnected)
		return;
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
		
    case OCI73:
		check(obreak(mLDA));
		break;
		
#endif
#ifdef OCL_OCI8
		
    case OCI80:
		check(OCIBreak(hOCISvcCtx, OraEnvironment::getOCIError()));
		break;
		
#endif
	}
}

LDA* OraConnection::getLDA() {
	
#ifdef OCL_OCI8
	if (mOCICallStyle == OCI80) {
		if (!mLDA)
			mLDA = new LDA;
		
		if (hOCISvcCtx)
			check(OCISvcCtxToLda(hOCISvcCtx, OraEnvironment::getOCIError(), mLDA));
	}
#endif
	
	return mLDA;
}

void OraConnection::setLDA(LDA* value)
{
	if (value != mLDA) {
		close();
		
		mLDA = value;
		
		//mNativeSvcCtx = hOCISvcCtx == 0;
		
		if (mNativeSvcCtx) {
			mAutoCommit = false;
			mInTransaction = false;
		}
		
		//mConnected = hOCISvcCtx != 0;
	}
}

#ifdef OCL_OCI8

OCISvcCtx* OraConnection::getOCISvcCtx() const {
	
	return hOCISvcCtx;
}

void OraConnection::setOCISvcCtx(OCISvcCtx* value) {
	
	if (value != hOCISvcCtx) {
		close();
		
		hOCISvcCtx = value;
		
		mNativeSvcCtx = hOCISvcCtx == 0;
		
		if (!mNativeSvcCtx) {
			mAutoCommit = false;
			mInTransaction = false;
		}
		
		mConnected = hOCISvcCtx != 0;
	}
}

#endif

OCIVersion OraConnection::OCICallStyle() const {
	return mOCICallStyle;
}

void OraConnection::setOCICallStyle(OCIVersion value) {
	if (mOCICallStyle != value) {
		close();
		mOCICallStyle = value;
	}
}

void OraConnection::checkOCI() const {
	
	throwError(SCheckOCI);
}

void OraConnection::checkOCI73() const {
	
	if (mOCICallStyle != OCI73)
		throwError(SCheckOCI73);
}

void OraConnection::checkOCI80() const {
	
	if (mOCICallStyle != OCI80)
		throwError(SCheckOCI80);
}

void OraConnection::check(sword errorCode) {
	if (errorCode != OCI_SUCCESS)
		error(mOCICallStyle, errorCode);
}

void OraConnection::error(OCIVersion callStyle, sword errorCode) {
	CRString errorMsg;
	
	switch (callStyle) {
#ifdef OCL_OCI7    
    case OCI73:
        char msg[512];
		oerhms(mLDA, errorCode, (text*)msg, sizeof(msg));
		errorMsg = msg;
		errorCode = abs(errorCode);
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		sword code = OraEnvironment::getOraError(errorCode, errorMsg);
		errorCode = code;
		
		if (errorCode == OCI_SUCCESS_WITH_INFO && errorCode == 24344)
			return;
		
		break;
#endif
	}
	
	trace(OraMonitor::meError, OraMonitor::tpAfter, errorMsg);
	
	// Handle lost Connection
	if ((errorCode == OCI_NOT_CONNECTED  ||
		errorCode == OCI_NOTLOGGEDON    ||      
		errorCode == OCI_SESSION_KILLED ||
		errorCode == OCI_NO_INTERFACE ||
		errorCode == 12571 /*TNS:packet writer failure*/) && mConnected)
	{
		close();
	}
	
	throw OraException(errorCode, errorMsg);
}


void OraConnection::trace(OraMonitor::MonitorEvent monitorEvent,
						  OraMonitor::TracePoint tracePoint, const string& errorMsg) {
	
	if (!OraMonitor::isActive())
		return;
	
	OraMonitor::MonitorMsg msg;
	
	msg.monitorEvent = monitorEvent;
	msg.tracePoint = tracePoint;
	msg.objectName = mName;
	msg.objectID = reinterpret_cast<int>(this);
	
	switch (monitorEvent) {
    case OraMonitor::meConnect:
    case OraMonitor::meDisconnect:
		ds_ocl_sql_monitor->onConnect(msg, string(mUsername), string(mServer));
		break;
		
    case OraMonitor::meCommit:
    case OraMonitor::meRollback:      
		ds_ocl_sql_monitor->onTransact(msg);
		break;
		
    case OraMonitor::meError:
		ds_ocl_sql_monitor->onError(msg, errorMsg);
		break;
	}
}

int OraConnection::getTNSAliases(CRString aliases[], int arrCount) {
	
	const CRString tnsNames("tnsnames.ora");
	const CRString networkDir("\\network\\admin\\");
	const CRString net80Dir("\\net80\\admin\\");
	const int BUFFER_LENGTH = 256;
	
	CRString tnsFileName;
	FILE* tnsFile;
	char buf[BUFFER_LENGTH];
	int aliasesCount = 0;
	
#ifndef OCL_DYNAMIC
	// Set Oracle home path
	OraEnvironment::detectOCI();
#else
	OraEnvironment::initOCI();
#endif
	
	// Check if TNS_ADMIN environment variable is set
	char* tnsAdmEnv = getenv("TNS_ADMIN");
	
	if (tnsAdmEnv) {
		tnsFileName = tnsAdmEnv;
		tnsFileName += "\\" + tnsNames;
	}
#ifdef OCL_WIN32
	else {
		// Look for TNS_ADMIN variable in registry
		HKEY hOraKey = 0;
		sprintf(buf, "SOFTWARE\\ORACLE\\HOME%i", OraEnvironment::defaultOracleHome());
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, buf, 0, KEY_READ, &hOraKey)
			== ERROR_SUCCESS)
		{
			int bufLen = BUFFER_LENGTH;
			if (RegQueryValueEx(hOraKey, "TNS_ADMIN", 0, 0, (LPBYTE)buf, (LPDWORD)&bufLen) ==
				ERROR_SUCCESS)
			{
				buf[bufLen] = 0;
				tnsFileName += CRString(buf) + "\\" + tnsNames;
			}
			
			RegCloseKey(hOraKey);
		}
	}
#endif // OCL_WIN32
	
	CRString oraHomePath = OraConnection::homePath();
	
	if (tnsFileName.empty()) {
		// Try to find file in Oracle net directories
		tnsFileName = oraHomePath + networkDir + tnsNames;
		
		tnsFile = fopen(tnsFileName, "r");
		if (!tnsFile) {
			tnsFileName = oraHomePath + net80Dir + tnsNames;
			tnsFile = fopen(tnsFileName, "r");
			
			if (!tnsFile) {
				throw CRException(TnsNamesFileNotFound);
			}
		}
	}
	else {
		tnsFile = fopen(tnsFileName, "r");
		
		if (!tnsFile) {
			throw CRException(TnsNamesFileNotFound);
		}
	}
	
	CRString data;
	char temp[256];
	
	// Read file
	while (!feof(tnsFile)) {
		fgets(temp, 256, tnsFile);
		data += CRString(temp);
	}
	
	fclose(tnsFile);
	
	// Set up parser
	OraParser parser(data);
	parser.setOmitBlank(true);
	parser.setOmitComment(true);
	parser.setInlineComment("#");
	
	int code = 0;
	CRString st, oldSt, alias;
	int bracket = 0;
	
	// Parse tnsnames.ora file
	do {
		if ((bracket == 0) && ((code == lcIdent) || (st == "-")))
			if (oldSt == ".") {
				if (st.toUpper() != "WORLD")
					alias += "." + st;
			}
			else
				alias += st;
			
			oldSt = st;
			
			code = parser.next(st);
			if (st == "(")
				bracket++;
			else
				if (st == ")")
					bracket--;
				else
					if ((st == "=") && (bracket == 0)) {
						if (aliasesCount < arrCount) {
							aliases[aliasesCount] = alias;
							aliasesCount++;
						}
						else
							break;
						
						alias.erase();
					}
	} while (code != lcEnd);
	
	return aliasesCount;
}

/*
void OraConnection::setBeforeConnect(TCRSessionEvent* value) {

 mBeforeConnect = value;
 }
 
  void OraConnection::setAfterConnect(TCRSessionEvent* value) {
  
   mAfterConnect = value;
   }
   
	void OraConnection::setBeforeDisconnect(TCRSessionEvent* value) {
	
	 mBeforeDisconnect = value;
	 }
	 
	  void OraConnection::setAfterDisconnect(TCRSessionEvent* value) {
	  
	   mAfterDisconnect = value;
	   }
	   */
	   
	   ////////////////////  OraCursor  ////////////////////////
	   
	   OraCursor::OraCursor() : CRSharedObject()
	   {
		   
#ifdef OCL_OCI7
		   mCDA = 0;
#endif
		   
#ifdef OCL_OCI8
		   hOCIStmt = 0;
#endif
		   
		   mState = csInactive;
		   mOCICallStyle = OraEnvironment::OCICallStyle();
	   }
	   
	   OraCursor::~OraCursor() {
		   freeCursor();
	   }
	   
	   void OraCursor::checkOCI() const {
		   
		   if (!(mOCICallStyle == OCI73 || mOCICallStyle == OCI80))
			   throwError(SCheckOCI);
	   }
	   
	   void OraCursor::checkOCI73() const {
		   
		   if (mOCICallStyle != OCI73)
			   throwError(SCheckOCI73);
	   }
	   
	   void OraCursor::checkOCI80() const {
		   
		   if (mOCICallStyle != OCI80)
			   throwError(SCheckOCI80);
	   }
	   
	   void OraCursor::allocCursor() {
		   
		   switch (mOCICallStyle) {
#ifdef OCL_OCI7
		   case OCI73:
			   if (!mCDA)
				   mCDA = (CDA*)malloc(sizeof(CDA));
			   
			   memset(mCDA, 0, sizeof(CDA));
			   break;
#endif
#ifdef OCL_OCI8
		   case OCI80:
			   if (!hOCIStmt)
				   check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void **)&hOCIStmt, OCI_HTYPE_STMT, 0, 0));
			   break;
#endif
		   }
	   }
	   
	   void OraCursor::freeCursor() {
		   
		   if (refCount() == 1) {
			   switch (mOCICallStyle) {
#ifdef OCL_OCI7
			   case OCI73:
				   if (mCDA) {
					   //checkOCI73();
					   free(mCDA);
					   mCDA = 0;
				   }
				   break;
#endif
#ifdef OCL_OCI8
			   case OCI80:
				   if (hOCIStmt) {
					   check(OCIHandleFree(hOCIStmt, OCI_HTYPE_STMT));
					   hOCIStmt = 0;
				   }
#endif
			   }
		   }
	   }
	   
	   bool OraCursor::canFetch() const {
		   
		   return mState >= csExecuted && mState < csCanceled;
	   }
	   
#ifdef OCL_OCI7
	   CDA* OraCursor::getCDA() {
		   
		   //  checkOCI73();
		   
		   if (!mCDA)
			   allocCursor();
		   
		   return mCDA;
	   }
#endif // OCL_OCI7
	   
#ifdef OCL_OCI8
	   OCIStmt* OraCursor::getOCIStmt() {
		   
		   return hOCIStmt;
	   }
#endif
	   
	   OCIVersion OraCursor::OCICallStyle() const {
		   
		   return mOCICallStyle;
	   }
	   
	   void OraCursor::setOCICallStyle(OCIVersion value) {
		   
		   if (value != mOCICallStyle) {
			   freeCursor();
			   mOCICallStyle = value;
		   }
	   }
	   
	   CRCursorState OraCursor::state() const {
		   
		   return mState;
	   }
	   
	   ////////////////////  OraParam  ////////////////////////
	   
	   OraParam::OraParam() {
		   
		   mValue = 0;
		   mIndicator = &mDefIndicator;
		   mDefIndicator = -1;
		   mDataType = mSubDataType = dtUnknown;
		   mDataSize = 0;
		   mTable = false;
		   mTableLength = 1;
		   mActualLength = 0;
		   mDirection = pdUnknown;
		   mHandle = 0;
		   mBlobPieceCount = 0;
	   }
	   
	   OraParam::~OraParam() {
		   freeValue();
	   }
	   
	   // Memory management
	   
#if defined(OCL_OCI90) || defined(OCL_OCI8)
	   const int stringSize = 4001;
#else
	   const int stringSize = 2001;
#endif
	   
	   void OraParam::reallocBuffers(int newDataSize, int newTableLength) {
		   
		   switch (mDataType) {
		   case dtLongRaw: case dtLong: case dtCursor: case dtBlob: case dtClob:
			   if (newDataSize > 0)
				   throwError(SBufferMustBeNull);
			   break;
		   }
		   
		   if (newTableLength < 1)
			   newTableLength = 1;
		   
		   
		   if (mDataSize > 0) {
			   free(mValue);
			   mValue = 0;
		   }
		   
		   mDataSize = newDataSize;
		   mTableLength = newTableLength;
		   
		   if (mDataSize > 0) {
			   mValue = malloc(mDataSize*mTableLength);
			   memset(mValue, 0, mDataSize*mTableLength);
		   }
		   
		   
		   if (mIndicator != &mDefIndicator) {
			   free(mIndicator);
			   mIndicator = &mDefIndicator;
		   }
		   
		   if (mTableLength > 1) {
			   mIndicator = (sb2*)malloc(sizeof(sb2)*mTableLength);
			   memset(mIndicator, -1, sizeof(sb2)*mTableLength);
		   }
		   else
			   mDefIndicator = -1;
	   }
	   
	   void OraParam::freeBuffers() {
		   
		   if (mValue != 0) {
			   free(mValue);
			   mValue = 0;
		   }
		   
		   if (mIndicator != &mDefIndicator) {
			   free(mIndicator);
			   mIndicator = &mDefIndicator;
		   }
		   
		   mDataSize = 0;
		   mDefIndicator = -1;
	   }
	   
	   void OraParam::createValue() {
		   
		   long bufSize;
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   bufSize = stringSize;
			   break;
		   case dtInteger:
			   bufSize = sizeof(int);
			   break;
		   case dtFloat:
		   case dtDouble:
			   bufSize = sizeof(double);
			   break;
		   case dtDate:
			   bufSize = 7;
			   break;
		   case dtBinary:
#ifdef OCL_OCI8
			   bufSize = 2000;
#else
			   bufSize = 255;
#endif
			   break;
		   case dtCursor:
			   bufSize = 0;
			   break;
		   case dtLongRaw: case dtLong:
			   bufSize = 0;
			   break;
#ifdef OCL_OCI8
		   case dtBlob: case dtClob: case dtBFile: case dtCFile:
			   bufSize = 0;
			   break;
#endif
#ifdef OCL_OCI90
		   case dtTimeStamp: case dtTimeStampTZ: case dtTimeStampLTZ:
		   case dtIntervalYM: case dtIntervalDS:
			   bufSize = 0;
			   break;
#endif
		   default:
			   assert(false);
		   }
		   
		   reallocBuffers(bufSize, mTableLength);
		   
		   // Create objects
		   switch (mDataType) {
		   case dtLongRaw: case dtLong:
			   if (!mValue)
				   mValue = new OraLong();
			   break;
		   case dtCursor:
			   if (!mValue)
				   mValue = new OraCursor();
			   break;
#ifdef OCL_OCI8
		   case dtBlob: case dtClob:
			   if (!mValue)
				   mValue = new OraLob();
			   break;
		   case dtBFile: case dtCFile:
			   if (!mValue)
				   mValue = new OraFile();
			   break;
#endif
#ifdef OCL_OCI90
		   case dtTimeStamp:
			   if (!mValue)
				   mValue = new OraTimeStamp(dtTimeStamp);
			   break;
		   case dtTimeStampTZ:
			   if (!mValue)
				   mValue = new OraTimeStamp(dtTimeStampTZ);
			   break;
		   case dtTimeStampLTZ:
			   if (!mValue)
				   mValue = new OraTimeStamp(dtTimeStampLTZ);
			   break;
		   case dtIntervalYM:
			   if (!mValue)
				   mValue = new OraInterval(dtIntervalYM);
			   break;
		   case dtIntervalDS:
			   if (!mValue)
				   mValue = new OraInterval(dtIntervalDS);
			   break;
#endif
		   }
		   
		   initValue();
	   }
	   
	   void OraParam::initValue() {
		   
		   int i;
		   
		   switch (mDataType) {
			   
		   case dtString:
		   case dtFixedString:
			   for (i = 1; i <= mTableLength; i++)
				   setString(i, "");
			   break;
			   
		   case dtInteger: case dtDouble:
			   for (i = 1; i <= mTableLength; i++)
				   setInt(i, 0);
			   break;
			   
		   case dtDate:
		   case dtLongRaw: case dtLong:
			   setString("");
			   break;
		   }
		   
		   for (i = 1; i <= mTableLength; i++)
			   setNull(i);
	   }
	   
	   void OraParam::freeValue() {
		   
		   switch (mDataType) {
		   case dtLongRaw:
		   case dtLong:
		   case dtCursor:
		   case dtBlob:
		   case dtClob:
		   case dtBFile:
		   case dtCFile:
			   if (mValue) {
				   ((CRSharedObject*)mValue)->release();
				   mValue = 0;
			   }
			   break;
#ifdef OCL_OCI90
		   case dtTimeStamp: case dtTimeStampTZ: case dtTimeStampLTZ:
			   if (mValue) {
				   delete (OraTimeStamp*)mValue;
				   mValue = 0;
			   }
			   break;
		   case dtIntervalYM: case dtIntervalDS:
			   if (mValue) {
				   delete (OraInterval*)mValue;
				   mValue = 0;
			   }
			   break;
#endif
		   }
		   
		   freeBuffers();
		   mDataType = dtUnknown;
	   }
	   
	   void* OraParam::valuePtr() {
		   
		   return mValue;
	   }
	   
	   void* OraParam::indicatorPtr() {
		   
		   return mIndicator;
	   }
	   
	   // Properties
	   
	   CRString OraParam::name() const {
		   
		   return mName;
	   }
	   
	   void OraParam::setName(const CRString& value) {
		   
		   mName = value;
	   }
	   
	   CRDataType OraParam::dataType() const {
		   
		   return mDataType;
	   }
	   
	   void OraParam::setDataType(CRDataType value) {
		   
		   CRParamDirection oldDirection;
		   
		   if (mDataType != value) {
			   oldDirection = mDirection;
			   freeValue();
			   
			   mDataType = value;
			   
			   if (mDataType != dtUnknown) {
				   createValue();
				   if (oldDirection != pdUnknown && mDataType != dtCursor)
					   mDirection = oldDirection;
			   }
		   }
	   }
	   
	   void OraParam::setDirection(CRParamDirection value) {
		   
		   mDirection = value;
	   }
	   
	   int OraParam::size() const {
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   return strlen((char*)mValue);
		   case dtInteger: case dtDouble: case dtDate:
			   return mDataSize;
		   case dtLongRaw: case dtLong: case dtBlob: case dtClob: case dtBFile: case dtCFile:
			   return ((CRLob*)mValue)->size();
		   case dtCursor:
			   return 0;
		   default:
			   assert(false);
			   return 0;
		   }
	   }
	   
	   void OraParam::setSize(int value) {
		   
		   if (value == mDataSize || mDataType == dtUnknown)
			   return;
		   
		   if (mDataType == dtLongRaw || mDataType == dtLong ||
			   mDataType == dtBlob || mDataType == dtClob || mDataType == dtBFile ||
			   mDataType == dtCFile)
		   {
			   throwError(SBufferCannotChange);
		   }
		   
		   reallocBuffers(value, mTableLength);
	   }
	   
	   void OraParam::checkRange(int index) const {
		   
		   if (1 > index || index > mTableLength)
			   throwError(SIndexIsNotRange);
	   }
	   
	   void OraParam::refreshIndicator() {
		   
		   switch (mDataType) {
		   case dtLongRaw: case dtLong:
			   if (!mValue || ((OraLong*)mValue)->size() == 0)
				   mIndicator[0] = -1;
			   else
				   mIndicator[0] = 0;
			   break;
#ifdef OCL_OCI8
		   case dtBlob: case dtClob:
			   if (!mValue || ((OraLob*)mValue)->size() == 0)
				   mIndicator[0] = -1;
			   else
				   mIndicator[0] = 0;
			   break;      
		   case dtBFile: case dtCFile:
			   if (!mValue || ((OraFile*)mValue)->fileDir() == "" &&
				   ((OraFile*)mValue)->fileName() == ""
				   )
				   mIndicator[0] = -1;
			   else
				   mIndicator[0] = 0;
			   break;
#endif
#ifdef OCL_OCI90
		   case dtTimeStamp:
		   case dtTimeStampTZ:
		   case dtTimeStampLTZ:
			   if (((OraTimeStamp*)mValue)->isNull())
				   mIndicator[0] = -1;
			   else
				   mIndicator[0] = 0;
			   break;
		   case dtIntervalYM:
		   case dtIntervalDS:
			   if (((OraInterval*)mValue)->isNull())
				   mIndicator[0] = -1;
			   else
				   mIndicator[0] = 0;
			   break;
#endif
		   }
	   }
	   
	   sb2 OraParam::indicator(int index) const {
		   
		   switch (mDataType) {
		   case dtCursor:
			   return 0; // strict for binding
		   default:
			   checkRange(index);
			   return mIndicator[index - 1];
		   }
	   }
	   
	   void OraParam::setIndicator(int index, sb2 value) {
		   
		   checkRange(index);
		   mIndicator[index - 1] = value;
	   }
	   
	   bool OraParam::isTable() const {
		   
		   return mTable;
	   }
	   
	   void OraParam::setTable(bool value) {
		   
		   if (value != mTable) {
			   if (value) {
				   if (mDataType == dtLongRaw || mDataType == dtLong || mDataType == dtCursor ||
					   mDataType == dtBlob || mDataType == dtClob || mDataType == dtBFile ||
					   mDataType == dtCFile
					   )
					   throwError(STypeCanNotConsistTable);
			   }
			   else
				   mTableLength = 1;
			   
			   mTable = value;
		   }
	   }
	   
	   int OraParam::tableLength() const {
		   
		   return mTableLength;
	   }
	   
	   void OraParam::setTableLength(int value) {
		   
		   //length can not be less then one
		   assert(value >= 1);
		   
		   //if length not changed exit
		   if (value == mTableLength)
			   return;
		   
		   mTableLength = value;
		   
		   if (mDataType != dtUnknown) {
			   reallocBuffers(mDataSize, mTableLength);
			   initValue();
		   }
	   }
	   
	   // Data
	   
	   bool OraParam::isNull(int index) {
		   
		   return indicator(index) < 0;
	   }
	   
	   void OraParam::setNull(int index /* = 1 */) {
		   
		   setIndicator(index, -1);
		   
		   if (mDataType == dtLongRaw || mDataType == dtLong ||
			   mDataType == dtBlob || mDataType == dtClob)
		   {
			   ((CRLob*)mValue)->clear();
		   } 
	   }
	   
	   CRDate OraParam::getDate(int index) {
		   void* VPtr;
		   CRDate date;
		   
		   checkRange(index);
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtDate:
			   oraDateToCRDate(VPtr, date);
			   break;
			   
		   default:
			   throwError(SCannotConvert, "Date");
		   }
		   return date;
	   }
	   
	   void OraParam::setDate(const CRDate& value) {
		   
		   setDate(1, value);
	   }
	   
	   void OraParam::setDate(int index, const CRDate& value) {
		   void* VPtr;
		   
		   checkRange(index);
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtDate);
		   
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtDate:
			   CRDateToOraDate(value, VPtr);
			   break;
		   default:
			   throwError(SCannotConvert, "Date");
		   }
		   
		   setIndicator(index, 0);
	   }
	   
	   float OraParam::getFloat(int index){
		   return (float)getDouble(index);
	   }
	   
	   double OraParam::getDouble(int index) {
		   void* VPtr;
		   
		   checkRange(index);
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtDouble:
			   return *(double*)VPtr;
		   case dtInteger:
			   return *(int*)VPtr;
		   default:
			   throwError(SCannotConvert, "Float");
			   return 0;
		   }
	   }
	   
	   void OraParam::setFloat(float value) {
		   if (mDataType == dtUnknown)
			   setDataType(dtFloat);
		   
		   setDouble(1, value);
	   }
	   
	   void OraParam::setDouble(double value) {
		   
		   setDouble(1, value);
	   }
	   
	   void OraParam::setDouble(int index, double value) {
		   void* VPtr;
		   
		   checkRange(index);
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtDouble);
		   
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtFloat:
		   case dtDouble:
			   *(double*)VPtr = value;
			   break;
		   case dtInteger:
			   *(int*)VPtr = (int)value;
			   break;
		   default:
			   throwError(SCannotConvert, "Float");
		   }
		   
		   setIndicator(index, 0);
	   }    
	   
	   int OraParam::getInt(int index) {
		   void* VPtr;
		   
		   checkRange(index);
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtInteger:
			   return *(int*)VPtr;
		   case dtDouble:
			   return (int)*(double*)VPtr;
		   default:
			   throwError(SCannotConvert, "Int");
			   return 0;
		   }
	   }
	   
	   void OraParam::setInt(int value) {
		   
		   setInt(1, value);
	   }
	   
	   void OraParam::setInt(int index, int value) {
		   void* VPtr;
		   
		   checkRange(index);
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtInteger);
		   
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
			   
		   case dtInteger:
			   *(int*)VPtr = value;
			   break;
			   
		   case dtDouble:
			   *(double*)VPtr = value;
			   break;
			   
		   default:
			   throwError(SCannotConvert, "Int");
		   }
		   
		   setIndicator(index, 0);
	   }
	   
	   bool OraParam::getBoolean(int index /*= 1*/) {
		   void* valuePtr;
		   
		   checkRange(index);
		   
		   if (mSubDataType != dtBoolean)
			   throwError(SCannotConvert, "Boolean");
		   
		   valuePtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   return *(int*)valuePtr == 1 ? true : false;
	   }
	   
	   void OraParam::setBoolean(bool value) {
		   setBoolean(1, value);
	   }
	   
	   void OraParam::setBoolean(int index, bool value) {
		   void* valuePtr;
		   
		   checkRange(index);
		   
		   if (mSubDataType != dtBoolean)
			   throwError(SCannotConvert, "Boolean");
		   
		   valuePtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   *(int*)valuePtr = value == true ? 1 : 0;
		   
		   setIndicator(index, 0);
	   }
	   
	   const char* OraParam::getChars(int index) {
		   
		   checkRange(index);
		   
		   if (isNull(index))
			   return "";
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   return (char*)mValue + (index - 1)*mDataSize;
			   
		   case dtLongRaw: case dtLong:
			   return "";
			   
		   default:
			   return getChars(index, mBuf);
		   }
	   }
	   
	   char* OraParam::getChars(char* result) {
		   
		   return getChars(1, result);
	   }
	   
	   char* OraParam::getChars(int index, char* result) {
		   void* dataPtr;
		   
		   checkRange(index);
		   
		   if (isNull(index)) {
			   *result = 0;
			   return "";
		   }
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   dataPtr = (char*)mValue + (index - 1)*mDataSize;      
			   return strcpy(result, (char*)dataPtr);
			   
		   case dtDate:
			   return strcpy(result, getDate().format().c_str());
			   
		   case dtDouble:
			   sprintf(result, "%f", getDouble());
			   return result;
			   
		   case dtInteger:
			   sprintf(result, "%i", getInt());
			   return result;
			   
		   case dtLongRaw: case dtLong:
		   case dtClob: case dtBlob:
			   return getLob().getChars(result);
			   
		   default:
			   throwError(SCannotConvert, "String");
			   return 0;
		   }
	   }
	   
	   CRString OraParam::getString(int index /* = 1 */) {
		   
		   checkRange(index);
		   
		   if (isNull(index))
			   return "";
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   return CRString((char*)mValue + (index - 1)*mDataSize);
			   
		   case dtDate:
			   return CRString(getDate().format());
			   
		   case dtDouble: case dtInteger:
			   return CRString(getChars(index));
			   
		   case dtLongRaw: case dtLong:
		   case dtClob: case dtBlob:
			   return getLob().getString();
			   
		   default:
			   throwError(SCannotConvert, "String");
			   return 0;
		   }
		   
	   }
	   
	   void OraParam::setChars(const char* value) {
		   
		   setString(1, value);
	   }
	   
	   void OraParam::setChars(int index, const char* value) {
		   void* VPtr;
		   int len;
		   
		   checkRange(index);
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtString);
		   
		   if (!value || !value[0]) {
			   setNull(index);
			   return;
		   }
		   
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   switch (mDataType) {
		   case dtString:
		   case dtFixedString:
			   len = strlen(value) + 1; // null terminate
			   if (len > mDataSize)
				   if (!mTable)
					   setSize(len);
				   else
					   len = mDataSize - 1;
				   strncpy((char*)VPtr, value, len);
				   *((char*)VPtr + mDataSize - 1) = 0;  // terminator
				   break;
		   case dtInteger:
			   *(int*)VPtr = atoi(value);
			   break;
		   case dtDouble:
			   *(double*)VPtr = atof(value);
			   break;
		   case dtLong: case dtClob:
			   ((CRLob*)mValue)->clear();
			   ((CRLob*)mValue)->write_blob(0, strlen(value) + 1, (void*)value);
			   break;
		   case dtLongRaw: case dtBlob: case dtBFile: case dtCFile:
			   ((CRLob*)mValue)->clear();
			   ((CRLob*)mValue)->write_blob(0, strlen(value), (void*)value);
			   break;
		   default:
			   throwError(SCannotConvert, "String");
		   }
		   
		   setIndicator(index, 0);
	   }
	   
	   void OraParam::setString(const CRString& value) {
		   
		   setString(1, value);
	   }
	   
	   void OraParam::setString(int index, const CRString& value) {
		   
		   setChars(index, value.c_str());
	   }
	   
	   OraCursor& OraParam::getCursor() {
		   
		   if (mDataType != dtCursor)
			   throwError(SCannotConvert, "Cursor");
		   
		   return *(OraCursor*)mValue;
	   }
	   
	   void OraParam::setCursor(OraCursor& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtCursor)
				   if (mDataType == dtUnknown)
					   setDataType(dtCursor);
				   else
					   throwError(SCannotConvert, "Cursor");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   if (mValue)
					   value.addRef();
				   
				   setIndicator(1, 0);
		   }
	   }
	   
	   
	   CRLob& OraParam::getLob() {
		   
		   if (mDataType != dtLong && mDataType != dtLongRaw &&
			   mDataType != dtBlob && mDataType != dtClob && mDataType != dtBFile)
		   {
			   throwError(SCannotConvert, "Blob");
		   }
		   
		   return *(CRLob*)mValue;
	   }
	   
	   void OraParam::setLob(CRLob& value) {
		   
		   if (mValue != (void*)&value) {
			   
			   if (mDataType != dtLong && mDataType != dtLongRaw && 
				   mDataType != dtBlob && mDataType != dtClob && mDataType != dtBFile)
			   {  
				   throwError(SCannotConvert, "Blob");
			   }
			   else
				   freeValue();
			   
			   mValue = (void*)&value;
			   if (mValue)
				   value.addRef();
			   
			   setIndicator(1, 0);
		   }
	   }
	   
	   
	   OraLong& OraParam::getLong() {
		   
		   if (mDataType != dtLong && mDataType != dtLongRaw)
			   if (mDataType == dtUnknown)
				   setDataType(dtLong);
			   else
				   throwError(SCannotConvert, "Long");
			   
			   return *(OraLong*)mValue;
	   }
	   
	   void OraParam::setLong(OraLong& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtLong && mDataType != dtLongRaw)      
				   if (mDataType == dtUnknown)
					   setDataType(dtLong);
				   else
					   throwError(SCannotConvert, "Long");
				   else
					   freeValue();
				   
				   
				   mValue = (void*)&value;
				   if (mValue)
					   value.addRef();
				   
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraLong& OraParam::getLongRaw() {
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtLongRaw);
		   
		   return getLong();
	   }
	   
	   unsigned char* OraParam::getRaw(unsigned char* result) {
		   return getRaw(1, result);
	   }
	   
	   unsigned char* OraParam::getRaw(int index, unsigned char* result) {
		   
		   void* dataPtr;
		   
		   checkRange(index);
		   
		   if (isNull(index)) {
			   *result = 0;
			   return 0;
		   }
		   
		   if (mDataType != dtBinary)
			   throwError(SCannotConvert, "RAW");
		   
		   dataPtr = (char*)mValue + (index - 1)*mDataSize;      
		   memcpy(result, dataPtr, mDataSize);
		   
		   return result;
	   }
	   
	   void OraParam::setRaw(unsigned char* value, int length) {
		   setRaw(1, value, length);
	   }
	   
	   void OraParam::setRaw(int index, unsigned char* value, int length) {
		   void* VPtr;
		   
		   checkRange(index);
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtBinary);
		   
		   if (mDataType != dtBinary)
			   throwError(SCannotConvert, "RAW");
		   
		   if (mDataSize < length)
			   throwError(SBufferTooLarge);
		   
		   if (mDataSize != length)
			   setSize(length);
		   
		   if (!value || !value[0]) {
			   setNull(index);
			   return;
		   }
		   
		   VPtr = (char*)mValue + (index - 1)*mDataSize;
		   
		   memcpy(VPtr, value, length);
		   
		   setIndicator(index, 0);
	   }
	   
	   void OraParam::setLongRaw(OraLong& value) {
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtLongRaw);
		   
		   setLong(value);
	   }
	   
#ifdef OCL_OCI8
	   
	   OraLob& OraParam::getBlob() {
		   
		   if (mDataType != dtBlob && mDataType != dtClob)
			   if (mDataType == dtUnknown)
				   setDataType(dtBlob);
			   else
				   throwError(SCannotConvert, "Lob");
			   
			   return *(OraLob*)mValue;
	   }
	   
	   void OraParam::setBlob(OraLob& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtBlob && mDataType != dtClob)
				   if (mDataType == dtUnknown)
					   setDataType(dtBlob);
				   else
					   throwError(SCannotConvert, "Lob");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   if (mValue)
					   value.addRef();
				   
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraLob& OraParam::getClob() {
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtClob);
		   
		   return getBlob();
	   }
	   
	   void OraParam::setClob(OraLob& value) {
		   
		   if (mDataType == dtUnknown)
			   setDataType(dtClob);
		   
		   setBlob(value);
	   }
	   
	   OraFile& OraParam::getBFile() {
		   
		   if (mDataType != dtBFile)
			   if (mDataType == dtUnknown)
				   setDataType(dtBFile);
			   else
				   throwError(SCannotConvert, "File");
			   
			   return *(OraFile*)mValue;
	   }
	   
	   void OraParam::setBFile(OraFile& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtBFile)
				   if (mDataType == dtUnknown)
					   setDataType(dtBFile);
				   else
					   throwError(SCannotConvert, "File");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   if (mValue)
					   value.addRef();
				   
				   setIndicator(1, 0);
		   }
	   }
	   
#endif //OCL_OCI8
	   
#ifdef OCL_OCI90
	   OraTimeStamp& OraParam::getTimeStamp() {
		   
		   if (mDataType != dtTimeStamp)
			   if (mDataType != dtUnknown)
				   setDataType(dtTimeStamp);
			   else
				   throwError(SCannotConvert, "TIMESTAMP");
			   
			   return *(OraTimeStamp*)mValue;
	   }
	   
	   void OraParam::setTimeStamp(const OraTimeStamp& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtTimeStamp)
				   if (mDataType == dtUnknown)
					   setDataType(dtTimeStamp);
				   else
					   throwError(SCannotConvert, "TIMESTAMP");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraTimeStamp& OraParam::getTimeStampTZ() {
		   
		   if (mDataType != dtTimeStampTZ)
			   if (mDataType != dtUnknown)
				   setDataType(dtTimeStampTZ);
			   else
				   throwError(SCannotConvert, "TIMESTAMP WITH TIME ZONE");
			   
			   return *(OraTimeStamp*)mValue;
	   }
	   
	   void OraParam::setTimeStampTZ(const OraTimeStamp& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtTimeStampTZ)
				   if (mDataType == dtUnknown)
					   setDataType(dtTimeStampTZ);
				   else
					   throwError(SCannotConvert, "TIMESTAMP WITH TIME ZONE");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraTimeStamp& OraParam::getTimeStampLTZ() {
		   
		   if (mDataType != dtTimeStampLTZ)
			   if (mDataType != dtUnknown)
				   setDataType(dtTimeStampLTZ);
			   else
				   throwError(SCannotConvert, "TIMESTAMP WITH LOCAL TIME ZONE");
			   
			   return *(OraTimeStamp*)mValue;
	   }
	   
	   void OraParam::setTimeStampLTZ(const OraTimeStamp& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtTimeStampLTZ)
				   if (mDataType == dtUnknown)
					   setDataType(dtTimeStampLTZ);
				   else
					   throwError(SCannotConvert, "TIMESTAMP WITH LOCAL TIME ZONE");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraInterval& OraParam::getIntervalYM() {
		   
		   if (mDataType != dtIntervalYM)
			   if (mDataType != dtUnknown)
				   setDataType(dtIntervalYM);
			   else
				   throwError(SCannotConvert, "INTERVAL YEAR TO MONTH");
			   
			   return *(OraInterval*)mValue;
	   }
	   
	   void OraParam::setIntervalYM(const OraInterval& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtIntervalYM)
				   if (mDataType == dtUnknown)
					   setDataType(dtIntervalYM);
				   else
					   throwError(SCannotConvert, "INTERVAL YEAR TO MONTH");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   setIndicator(1, 0);
		   }
	   }
	   
	   OraInterval& OraParam::getIntervalDS() {
		   
		   if (mDataType != dtIntervalDS)
			   if (mDataType != dtUnknown)
				   setDataType(dtIntervalDS);
			   else
				   throwError(SCannotConvert, "INTERVAL DAY TO SECOND");
			   
			   return *(OraInterval*)mValue;
	   }
	   
	   void OraParam::setIntervalDS(const OraInterval& value) {
		   
		   if (mValue != (void*)&value) {
			   if (mDataType != dtIntervalDS)
				   if (mDataType == dtUnknown)
					   setDataType(dtIntervalDS);
				   else
					   throwError(SCannotConvert, "INTERVAL DAY TO SECOND");
				   else
					   freeValue();
				   
				   mValue = (void*)&value;
				   setIndicator(1, 0);
		   }
	   }
	   
#endif // OCL_OCI90
	   
	   CRValue OraParam::getValue(int index) {
		   
		   checkRange(index);
		   
		   switch (mDataType) {
		   case dtInteger:
			   return getInt(index);
		   case dtDouble:
			   return getInt(index);
		   default:
			   throwError(SCannotConvert, "CRValue");
			   return CRValue();
		   };
	   }
	   
	   void OraParam::setValue(const CRValue& value) {
		   
		   setValue(1, value);
	   }
	   
	   void OraParam::setValue(int index, const CRValue& value) {
		   
		   checkRange(index);
		   
		   switch (mDataType) {
		   case dtInteger:
			   setInt(value.getInt());
			   break;
		   default:
			   throwError(SCannotConvert, "CRValue");
		   }
		   
		   if (value.isNull())
			   setNull(index);
		   else
			   setIndicator(index, 0);
	   }
	   
	   void OraParam::assign(/*const*/ OraParam& value) {
		   int i;
		   
		   if (&value) {
			   mName = value.mName;
			   setDataType(value.mDataType);
			   mDirection = value.mDirection;
			   mTable = value.mTable;
			   mTableLength = value.mTableLength;
			   setSize(value.mDataSize);
			   
			   for (i = 1; i <= mTableLength; i++) {
				   setIndicator(i, value.indicator(i));
				   
				   switch (mDataType) {
				   case dtDate:
					   setDate(i, value.getDate(i));
					   break;
				   case dtDouble:
					   setDouble(i, value.getDouble(i));
					   break;
				   case dtInteger:
					   setInt(i, value.getInt(i));
					   break;
				   case dtString:
				   case dtFixedString:
					   setChars(i, value.getChars(i));
					   break;
				   case dtLongRaw:
					   setLongRaw(value.getLongRaw());
					   break;
				   case dtLong:
					   setLong(value.getLong());
					   break;
				   case dtCursor:
					   setCursor(value.getCursor());
					   break;
				   }
			   }
		   }
	   }
	   
	   void OraParam::operator =(const CRValue& value) {
		   
		   switch (value.type()) {
		   case CRValue::vtInt:
			   setInt(value.getInt());
			   break;
		   case CRValue::vtFloat:
		   case CRValue::vtDouble:
			   setDouble(value.getDouble());
			   break;
		   case CRValue::vtString:
			   setString(value.getString());
			   break;
		   case CRValue::vtDate:
			   setDate(value.getDate());
			   break;
		   default:
			   assert(0);
		   }
	   }
	   
	   void OraParam::disconnectObject() {
		   
#ifdef OCL_OCI8
		   switch (mDataType) {
		   case dtClob:
			   getClob().freeLob();
			   break;
		   case dtBlob:
			   getBlob().freeLob();
			   break;
#ifdef OCL_OCI90
		   case dtTimeStamp:
			   getTimeStamp().free();
			   break;
		   case dtTimeStampTZ:
			   getTimeStampTZ().free();
			   break;
		   case dtTimeStampLTZ:
			   getTimeStampLTZ().free();
			   break;
		   case dtIntervalDS:
			   getIntervalDS().free();
			   break;
		   case dtIntervalYM:
			   getIntervalYM().free();
			   break;
#endif
		   }
#endif // OCL_OCI8
	   }
	   
	   /////////////////  OraCommand  /////////////////////
	   
	   OraCommand::OraCommand() {
		   
		   _init();
	   }
	   
	   OraCommand::OraCommand(OraConnection& connection) {
		   _init();
		   setConnection(connection);
	   }
	   
	   OraCommand::OraCommand(OraConnection& connection, const CRString& sql) {
		   _init();
		   setConnection(connection);
		   setCommandText(sql);
	   }
	   
	   void OraCommand::_init() {
		   
		   mCursor           = new OraCursor();
		   mCursorRef        = mCursor;
		   mConnection       = 0;
		   mRecordset        = 0;
		   mNonBlocking      = false;
		   mAutoCommit       = true;
		   mSQLType          = SQL_UNKNOWN;
		   mOCICallStyle     = OraEnvironment::OCICallStyle();
		   mErrorOffset      = 0;
		   mRowsProcessed    = 0;
		   mFetchedRows      = 0;
		   mItersCount       = 1;
		   mCommandType      = ctSQL;
		   mImplicitPrepared = false;
	   }
	   
	   OraCommand::~OraCommand() {
		   
		   if (cursorState() > csInactive)
			   unprepare();
		   
		   assert(mCursorRef);
		   
		   if (mRecordset)
			   mRecordset->close();
		   
		   delete mRecordset;
		   
		   if (mCursorRef != mCursor)
			   delete mCursorRef;
		   
		   delete mCursor;
		   
		   mParams.clear();

           if (mConnection)
               mConnection->removeCommand(this);
	   }
	   
	   void OraCommand::checkOCI() const {
		   
		   if (!(mOCICallStyle == OCI73 || mOCICallStyle == OCI80))
			   throwError(SCheckOCI);
	   }
	   
	   void OraCommand::checkOCI73() const {
		   
		   if (mOCICallStyle != OCI73)
			   throwError(SCheckOCI73);
	   }
	   
	   void OraCommand::checkOCI80() const {
		   
		   if (mOCICallStyle != OCI80)
			   throwError(SCheckOCI80);
	   }
	   
	   void OraCommand::check(sword status) const {
		   
		   if (status != OCI_SUCCESS)
			   mConnection->error(mOCICallStyle, status);
	   }
	   
	   void OraCommand::checkSession() const {
		   
		   if (!mConnection)
			   throwError(SSessionNotDef);
		   
		   if (!mConnection->isConnected())
			   throwError(SSessionNotConnected);
	   }
	   
	   void OraCommand::checkActive() const {
		   
		   checkSession(); // ???
		   
		   if (cursorState() == csInactive)
			   throwError(SCursorNotOpened);
	   }
	   
	   void OraCommand::checkActiveRecordset() const
	   {
		   if (mRecordset && mRecordset->isActive())
			   throw OraException(SRecSetNotClosed);
	   }
	   
	   void OraCommand::checkInactive() const {
		   
		   if (cursorState() != csInactive)
			   throwError(SCursorOpened);
	   }
	   
	   // Types
	   
	   int OraCommand::getOraType(int dataType, int subDataType) {
		   
		   int oraType;  
		   
		   switch (mOCICallStyle) {
#ifdef OCL_OCI7    
		   case OCI73:
			   switch (dataType) {
			   case dtString:
				   oraType = STRING_TYPE;  // return with terminator
				   break;
			   case dtFixedString:
				   oraType = CHAR_TYPE;  // return with terminator
				   break;          
			   case dtInteger:
				   oraType = INTEGER_TYPE;
				   break;
			   case dtDouble:
				   oraType = FLOAT_TYPE;
				   break;
			   case dtDate:
				   oraType = DATE_TYPE;
				   break;
			   case dtLong:
				   oraType = LONG_TYPE;
				   break;
			   case dtBinary:
				   oraType = RAW_TYPE;
				   break;
			   case dtLongRaw:
				   oraType = LONGRAW_TYPE;
				   break;
			   case dtRowId:
				   oraType = ROWID_TYPE;
				   break;
			   case dtCursor:
				   oraType = CURSOR_TYPE;
				   break;
			   default:
				   throwError(SUnknownDataType, dataType);
			   }
			   break;
#endif
#ifdef OCL_OCI8
			   case OCI80:
				   switch (dataType) {
				   case dtString:
					   oraType = SQLT_STR;
					   break;
				   case dtFixedString:
					   oraType = SQLT_AVC;
					   break;          
				   case dtInteger:
					   oraType = SQLT_INT;
					   break;
				   case dtDouble:
					   oraType = SQLT_FLT;
					   break;
				   case dtDate:
					   oraType = SQLT_DAT;
					   break;
				   case dtBoolean:
					   oraType = SQLT_INT;
					   break;
				   case dtLong:
					   if (subDataType == dtString)
						   oraType = SQLT_STR;
					   else
						   oraType = SQLT_LNG;
					   break;
				   case dtBinary:
					   oraType = SQLT_BIN;
					   break;
				   case dtLongRaw:
					   oraType = SQLT_LBI;
					   break;
				   case dtBlob:
					   oraType = SQLT_BLOB;
					   break;
				   case dtClob:
					   oraType = SQLT_CLOB;
					   break;
				   case dtBFile:
					   oraType = SQLT_BFILE;
					   break;
				   case dtCFile:
					   oraType = SQLT_CFILE;
					   break;
				   case dtRowId:
					   oraType = SQLT_RDD;
					   break;
				   case dtCursor:
					   oraType = SQLT_RSET;
					   break;
#ifdef OCL_OCI90
				   case dtTimeStamp:
					   oraType = SQLT_TIMESTAMP;
					   break;
				   case dtTimeStampTZ:
					   oraType = SQLT_TIMESTAMP_TZ;
					   break;
				   case dtTimeStampLTZ:
					   oraType = SQLT_TIMESTAMP_LTZ;
					   break;
				   case dtIntervalYM:
					   oraType = SQLT_INTERVAL_YM;
					   break;
				   case dtIntervalDS:
					   oraType = SQLT_INTERVAL_DS;
					   break;
				   case dtFloat:
					   oraType = SQLT_FLT;
					   if ((OCIVersion() >= OCI100) && (mConnection->serverVersion() >= "10"))
					   {
						   if (subDataType = dtBFloat)
						   {//haipeng commented
							   //oraType = SQLT_IBFLOAT;
							   assert(false);
						   }
						   else if (subDataType = dtBDouble)
						   {//haipeng commented
							   //oraType = SQLT_IBDOUBLE;
							   assert(false);
						   }
					   }
					   break;
#endif // OCL_OCI90
					   /*
					   case dtObject:
					   oraType = SQLT_NTY;
					   break;
					   case dtReference:
					   oraType = SQLT_REF;
					   break;
					   case dtArray:
					   oraType = SQLT_NTY;
					   break;
					   case dtTable:
					   oraType = SQLT_NTY;
					   break;
					   case dtLabel:
					   oraType = SQLT_OSL;
					   */
				   default:
					   throwError(SUnknownDataType, dataType);
				   }
				   break;
#endif
  }
  return oraType;
}

// OCI functions

#ifdef OCL_OCI7

void OraCommand::internalOpen() {
	sword res;
	
	//  checkOCI73();
	checkSession();
	
	busy();
	try {
		mConnection->busyWait();
		res = oopen(mCursorRef->getCDA(), mConnection->getLDA(), 0, -1, -1, 0, -1);
		mConnection->release();
		
		check(res);
		
		setCursorState(csOpen);
		// finally
		release();
	}
	catch (OraException &e )//jianghp, added
	{
		release();
		throw e;
	}
	catch (...) {
		release();
		throw;
	}
}

void OraCommand::internalParse() {
	
	sword res;
	CRString SQLText;
	
	//  checkOCI73();
	checkActive();
	
	// for PL-SQL
	SQLText = mSQL;
	
	for (int i = 0; i < (int)SQLText.size(); i++)
		if (SQLText.at(i) == 13)
			SQLText.at(i) = 32;
		
		busy();
		
		try {
			mConnection->busyWait();
			
			res = oparse(mCursorRef->getCDA(), (text*)SQLText.c_str(), SQLText.length() + 1, OCI_PARSE_NODEFER, OCI_LANG_V7);  // DEFER ???
			mConnection->release();
			
			check(res);
			
			mSQLType = mCursorRef->getCDA()->ft;
			
			setCursorState(csParsed);
			// finally
			mErrorOffset = mCursorRef->getCDA()->peo;
			release();
		}
		catch (OraException &e )//jianghp, added
		{
			mErrorOffset = mCursorRef->getCDA()->peo;
			release();
			throw e;
		}
		catch (...) {
			mErrorOffset = mCursorRef->getCDA()->peo;
			release();
			throw;
		}
}
#endif // OCL_OCI7

#ifdef OCL_OCI8
void OraCommand::internalPrepare() {
	sword res;
	CRString SQLText;
	ub2 stmtType = 0;
	
	// checkOCI73();
	checkActive();
	
	// for PL-SQL
	SQLText = mSQL;
	
	for (int i = 0; i < (int)SQLText.size(); i++) {
		if (SQLText.at(i) == 13)
			SQLText.at(i) = 32;
	}
	
	busy();
	
	try {
		mConnection->busyWait();
		
		res = OCIStmtPrepare(mCursorRef->getOCIStmt(), OraEnvironment::getOCIError(), (text*)SQLText.c_str(),
			SQLText.length() + 1, OCI_NTV_SYNTAX, OCI_DEFAULT);
		
		mConnection->release();
		
		check(res);
		
		check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, (ub2*)&stmtType,
			0, OCI_ATTR_STMT_TYPE, OraEnvironment::getOCIError()));
		
		switch(stmtType) {
		case OCI_STMT_SELECT:
			mSQLType = SQL_SELECT;
			break;
		case OCI_STMT_UPDATE:
			mSQLType = SQL_UPDATE;
			break;
		case OCI_STMT_DELETE:
			mSQLType = SQL_DELETE;
			break;
		case OCI_STMT_INSERT:
			mSQLType = SQL_INSERT;
			break;
		case OCI_STMT_DROP:
			mSQLType = SQL_DROP;
			break;
		case OCI_STMT_CREATE:
			mSQLType = SQL_CREATE;
			break;
		case OCI_STMT_BEGIN:
		case OCI_STMT_DECLARE:
			mSQLType = SQL_PLSQL;
			break;
			
		default:
			mSQLType = SQL_UNKNOWN;
		}
		
		setCursorState(csPrepared);
	}
	catch (OraException &e )//jianghp, added
	{
		throw e;
	}
	catch (...) {
		throw;
	}
}

sb4 callbackInBind(dvoid* ictxp, OCIBind* bindp, ub4 iter,
				   ub4 index, dvoid** bufpp, ub4* alenp, ub1* piecep, dvoid** indpp)
{
	return ((OraCommand*)ictxp)->callbackInBind(bindp, iter, index,
		bufpp, alenp, piecep, indpp);
}

sb4 callbackOutBind(dvoid* octxp, OCIBind* bindp, ub4 iter, ub4 index, 
					dvoid** bufpp, ub4** alenpp, ub1* piecep, dvoid** indpp, ub2** rcodepp)
{
	*rcodepp = 0;
	return ((OraCommand*)octxp)->callbackOutBind(bindp, iter, index,
		bufpp, alenpp, piecep, indpp);
}

#endif // OCL_OCI8

void OraCommand::bindParam(OraParam* param) {
	char nullBuf = 0;
	int oraType;
	void* valuePtr;
	int bufferSize;
	
	if (param->mDataType == dtUnknown) {
		if (param->isNull())
			oraType = VARCHAR2_TYPE;
		else
			throwError(SUnkParamType, param->mName.c_str());
	}
	else
		oraType = getOraType(param->mDataType, dtUnknown);
	
	param->refreshIndicator();
	bufferSize = param->mDataSize;  
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		switch (param->mDataType) {
        case dtCursor:
			param->getCursor().setOCICallStyle(mOCICallStyle);
			param->getCursor().allocCursor();
			valuePtr = param->getCursor().getCDA();
			break;
			
        case dtUnknown:
			valuePtr = &nullBuf;
			bufferSize = 1;
			break;
			
        default:
			valuePtr = param->mValue;
		}
		
		if (!param->mTable) {
			if (!(param->mDataType == dtLongRaw || param->mDataType == dtLong))
				check(obndrv(mCursorRef->getCDA(), (text*)param->mName.c_str(), -1, (ub1*)valuePtr,
				bufferSize, oraType, -1, param->mIndicator, 0, -1, -1));
			else
				// Piecewise
				check(obindps(mCursorRef->getCDA(), 0, (text*)param->mName.c_str(), -1, 0,
				maxBlobSize, oraType, 0, param->mIndicator, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0));
			
			param->mActualLength = 1;
		}
		else {
			// Table
			param->mActualLength = param->mTableLength;
			
			check(obindps(mCursorRef->getCDA(), 1, (text*)param->mName.c_str(), -1, (ub1*)valuePtr,
				param->mDataSize, oraType, 0, param->mIndicator, 0, 0, param->mDataSize,
				sizeof(sb2), 0, 0, param->mTableLength, &(param->mActualLength), 0, 0, 0));
		}
		break;
		
#endif
#ifdef OCL_OCI8
		case OCI80:
			OCIBind* hBind = 0;
			
			switch (param->mDataType) {
			case dtCursor:
				param->getCursor().allocCursor();
				valuePtr = param->getCursor().getOCIStmtPtr();
				break;
			case dtBlob:
			case dtClob:
				param->getBlob().allocLob();
				param->getBlob().setOCISvcCtx(mConnection->getOCISvcCtx());
				valuePtr = param->getBlob().getOCILobLocatorPtr();
				break;
			case dtBFile:
			case dtCFile:
				param->getBFile().allocLob();
				param->getBFile().setOCISvcCtx(mConnection->getOCISvcCtx());
				valuePtr = param->getBFile().getOCILobLocatorPtr();
				break;
#ifdef OCL_OCI90
			case dtTimeStamp:
				valuePtr = param->getTimeStamp().getOCIDateTimePtr();
				break;
			case dtTimeStampTZ:
				valuePtr = param->getTimeStampTZ().getOCIDateTimePtr();
				break;
			case dtTimeStampLTZ:
				valuePtr = param->getTimeStampLTZ().getOCIDateTimePtr();
				break;
			case dtIntervalYM:
				valuePtr = param->getIntervalYM().getOCIIntervalPtr();
				break;
			case dtIntervalDS:
				valuePtr = param->getIntervalDS().getOCIIntervalPtr();
				break;
#endif
			case dtUnknown:
				valuePtr = &nullBuf;
				bufferSize = 1;
				break;
			default:
				valuePtr = param->valuePtr();
			}
			
			if (!param->mTable) {
				if (mItersCount == 1) {
					if (param->dataType() != dtLongRaw && param->dataType() != dtLong) {
						check(OCIBindByName(mCursorRef->getOCIStmt(), (OCIBind **)&hBind, OraEnvironment::getOCIError(), (text*)param->mName.c_str(),
							param->mName.length(), valuePtr, bufferSize, oraType, param->indicatorPtr(),
							0, 0, 0, 0, OCI_DEFAULT));
							/*
							switch (param.dataType()) {
							case dtObject: case dtArray: case dtTable:
							dtReference:
						*/              
					}
					else {
						if (param->mDirection == pdInput) {
							bufferSize = param->getLob().size();
							if (bufferSize == 0)
								bufferSize++;
						}
						else {
							bufferSize = maxBlobSize;
							if (param->mDirection == pdOutput)
								param->getLob().clear();
						}
						
						check(OCIBindByName(mCursorRef->getOCIStmt(), &hBind, OraEnvironment::getOCIError(), (text*)param->mName.c_str(),
							param->mName.length(), 0, bufferSize, oraType, param->indicatorPtr(),
							0, 0, 0, 0, OCI_DATA_AT_EXEC));
						
						check(OCIBindDynamic(hBind, OraEnvironment::getOCIError(), this,
							ocl::callbackInBind, this, ocl::callbackOutBind));
						
						param->mHandle = hBind;
					}
					param->mActualLength = 1;
				}
				else {
					if (mItersCount > param->mTableLength) {
						char msg[512];
						sprintf(msg, SArrayParam, param->mName.c_str(), mItersCount);
						throwError(msg);
					}
					
					check(OCIBindByName(mCursorRef->getOCIStmt(), &hBind, OraEnvironment::getOCIError(), (text*)param->mName.c_str(),
						param->mName.length(), valuePtr, bufferSize, oraType, param->indicatorPtr(),
						0, 0, 0, 0, OCI_DEFAULT));
					
					check(OCIBindArrayOfStruct(hBind, OraEnvironment::getOCIError(), bufferSize, sizeof(sb2), 0, 0));
				}
			}
			else {
				param->mActualLength = param->mTableLength;
				
				check(OCIBindByName(mCursorRef->getOCIStmt(), &hBind, OraEnvironment::getOCIError(), (text*)param->mName.c_str(),
					param->mName.length(), valuePtr, bufferSize, oraType, param->indicatorPtr(),
					0, 0, param->mTableLength, &(param->mActualLength), OCI_DEFAULT));
				
				check(OCIBindArrayOfStruct(hBind, OraEnvironment::getOCIError(), bufferSize, sizeof(sb2), 0, 0));
			}
			
			if ((param->dataType() == dtString || param->dataType() == dtFixedString) && (mConnection->mCharsetId > 0))
				check(OCIAttrSet(hBind, OCI_HTYPE_BIND, &(mConnection->mCharsetId), 0, 
				OCI_ATTR_CHARSET_ID, OraEnvironment::getOCIError()));
			
#endif
  }
}

sword OraCommand::internalExecute(int mode) {
	CRCursorState oldState;
	sword res;
	
	checkActive();
	busy();
	
	if (mode == OCI_DEFAULT)
		trace(OraMonitor::meExecute, OraMonitor::tpBefor);  
	
	oldState = cursorState();
	setCursorState(csExecuting);
	
	try { // For Busy
		try {
			switch (mOCICallStyle) {
#ifdef OCL_OCI7        
			case OCI73:
				if (mode == OCI_DEFAULT) {
					mConnection->busyWait();
					res = oexn(mCursorRef->getCDA(), mItersCount, 0);
					mConnection->release();
					
					if (res != OCI_SUCCESS &&
						res != OCI_STILL_IS_PIECE &&
						res != OCI_STILL_IS_PIECE1 &&
						res != OCI_BLOCKED) {
						mErrorOffset = mCursorRef->getCDA()->peo;
						check(res);
					}
					if (res == OCI_SUCCESS)
						mRowsProcessed = mCursorRef->getCDA()->rpc;
				}
				break;
#endif
#ifdef OCL_OCI8
			case OCI80:
				int iters;
				
				if (mSQLType == SQL_SELECT)
					iters = 0;
				else
					iters = mItersCount;
				
				mConnection->busyWait();
				res = OCIStmtExecute(mConnection->getOCISvcCtx(), mCursorRef->getOCIStmt(),
					OraEnvironment::getOCIError(), iters, 0, 0, 0, mode);
				mConnection->release();
				
				if ((res != OCI_SUCCESS) && (res != OCI_NEED_DATA))
					check(res);
				
				if (mSQLType == SQL_UNKNOWN)
					check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, &mSQLType, 0,
					OCI_ATTR_SQLFNCODE, OraEnvironment::getOCIError()));
				
				if ((res == OCI_SUCCESS) && (mode != OCI_DESCRIBE_ONLY))
					check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, &mRowsProcessed, 0,
					OCI_ATTR_ROW_COUNT, OraEnvironment::getOCIError()));
				break;
#endif
			}
		}

		catch (...) {
			setCursorState(oldState);
			
			throw;
		};
		
		if (mode == OCI_DEFAULT) {
			setCursorState(csExecuted);
			
			for (int i = 0; i < paramCount(); i++)
				if (mParams[i].mDataType == dtCursor)
					mParams[i].getCursor().mState = csExecuted;
		}
		else
			setCursorState(oldState);
		
		//finally
		release();
	}		
	catch (OraException &e )//jianghp, added
	{
#ifdef OCL_OCI8    
		if (mOCICallStyle == OCI80 && OraEnvironment::getOCIVersion() >= OCI81 && mConnection->isConnected()) {
			sb2 errorOffset;
			check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, &errorOffset, 0,
				OCI_ATTR_PARSE_ERROR_OFFSET, OraEnvironment::getOCIError()));
			mErrorOffset = errorOffset;
		}
#endif
		
		release();
		
		throw e;
	}
	catch (...) {
		
#ifdef OCL_OCI8    
		if (mOCICallStyle == OCI80 && OraEnvironment::getOCIVersion() >= OCI81 && mConnection->isConnected()) {
			sb2 errorOffset;
			check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, &errorOffset, 0,
				OCI_ATTR_PARSE_ERROR_OFFSET, OraEnvironment::getOCIError()));
			mErrorOffset = errorOffset;
		}
#endif
		
		release();
		
		throw;
	};
	
	if (mode == OCI_DEFAULT)
		trace(OraMonitor::meExecute, OraMonitor::tpAfter);
	
	mFetchedRows = 0;
	
	return res;
}

int OraCommand::getFieldDesc(int fieldNo, OraField* field, bool longString) {
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
    case OCI73: {
		const int maxFieldNameSize = 40;
		sb4 dbsize;
		sb2 dbtype;
		char cbuf[maxFieldNameSize + 1];
		sb4 cbufl;
		sb4 dsize;
		sb2 prec;
		sb2 nullok;
		sb2 scale;
		sword res;
		
		cbufl = maxFieldNameSize;
		
		res = odescr(mCursorRef->getCDA(), fieldNo, &dbsize, &dbtype, (sb1*)cbuf,
			&cbufl, &dsize, &prec, &scale, &nullok);
		
		if (res == 0) {
			cbuf[cbufl] = 0;
			field->mName.assign(cbuf);
			field->mFieldNo = fieldNo;
			field->mPrecision = 0;
			field->mScale = 0;
			
			switch (dbtype) {
			case VARCHAR2_TYPE:
				field->mDataType = dtString;
				field->mDataSize = dbsize + 1;  // for terminator
				break;
			case CHAR_TYPE:
				field->mDataType = dtFixedString;
				field->mDataSize = dbsize + 1;  // for terminator
				break;            
			case NUMBER_TYPE:
				field->mPrecision = prec;
				field->mScale = abs(scale);
				
				if (abs(scale) > 0 || prec > 9 || prec == 0) {
					field->mDataType = dtDouble;
					field->mDataSize = 8;
				}
				else {
					field->mDataType = dtInteger;
					field->mDataSize = 4;
				};
				break;
			case DATE_TYPE:
				field->mDataType = dtDate;
				field->mDataSize = 7;
				break;
			case ROWID_TYPE:
				field->mDataType = dtString; //dtRowId;  //WAR
				field->mSubDataType = dtRowId;
				field->mDataSize = rowIdSize + 1; // for terminator //dbsize;
				break;
			case RAW_TYPE:
				field->mDataType = dtBinary;
				field->mDataSize = dbsize;
				break;
			case LONG_TYPE:
				field->mDataType = dtLong;
				field->mDataSize = sizeof(OraLong*);
				break;
			case LONGRAW_TYPE:
				field->mDataType = dtLongRaw;
				field->mDataSize = sizeof(OraLong*);
				break;
			default:
				throwError(SUnknownDataType, dbtype);
			}
			
			field->mRequired = nullok == 0;
			return 1;
		}
		else {
			if (res != OCI_VAR_NOT_IN_LIST)
				check(res);
			
			return 0;
		}
				}
#endif
#ifdef OCL_OCI8
    case OCI80:
		OCIParam* hOCIParam = (OCIParam *)0;
		ub4 columnCount = 0;
		text* name;
		ub4 len = 0;
		ub2 oraType = 0;
		ub2 dataSize = 0;
		//ub1 precision = 0; //change to sb2
		ub1 isNull = 0;
		sb1 scale;
		
		//get the number of columns in the select list
		check(OCIAttrGet((dvoid *)mCursorRef->getOCIStmt(), (ub4)OCI_HTYPE_STMT, (dvoid *)&columnCount,
			(ub4 *)0, (ub4)OCI_ATTR_PARAM_COUNT, OraEnvironment::getOCIError()));
		
		if (fieldNo > (int)columnCount)
			return 0;
		
		//get parameter descriptor for current column
		check(OCIParamGet((dvoid *)mCursorRef->getOCIStmt(), (ub4)OCI_HTYPE_STMT, OraEnvironment::getOCIError(),
			(dvoid **)&hOCIParam, (ub4)fieldNo));
		
		//get name of the current column
		check(OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid *)&name,
			(ub4 *)&len, (ub4)OCI_ATTR_NAME, OraEnvironment::getOCIError()));
		
		char buf[512];
		memset((void*)buf, '\0', 512);
		
		field->mName = (const char*)strncpy(buf, (const char*)name, len);
		field->mFieldNo = fieldNo;
		field->mScale = 0;
		
		OCIError* error = OraEnvironment::getOCIError();
		assert(error != NULL);
		try
		{
			//get data type of the column
			/*
			check(OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid **)&oraType,
			(ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE, error));*/
			
			
			OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid **)&oraType,
				(ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE, error);
			
			//get data size of the column
			check(OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid **)&dataSize,
				(ub4 *)0, (ub4)OCI_ATTR_DATA_SIZE, error));
		}
		catch(OraException &e)
		{
			throw e;
		}
		catch(...)
		{
			char errorMsg[130];
			memset( errorMsg, 0, sizeof( errorMsg ) );
			sb4 nativeCode = 0;
			
			if ( error != NULL )
			{
				// Have a closer look at the error. Retrieve more info from Oracle.
				
				OCIErrorGet( error, 1, 0, &nativeCode, 
					(unsigned char*)&errorMsg, sizeof( errorMsg ), 
					OCI_HTYPE_ERROR );
			}
			
			std::string errorMsgStr( errorMsg );
			printf("oracle error is: [%s ]\n", errorMsgStr.c_str());
		}
		/*
		if ((oraType == SQLT_NTY) || (oraType == SQLT_REF)) {
        check(OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid **)&name,
		(ub4 *)len, (ub4)OCI_ATTR_TYPE_NAME, OraEnvironment::getOCIError()));
		
		 strcpy(typeName, (const char *)name);
		 
		  check(OCIAttrGet((dvoid *)hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid **)&name,
          (ub4 *)len, (ub4)OCI_ATTR_SCHEMA_NAME, OraEnvironment::getOCIError()));
		  
		   strcpy(schemaName, (const char *)name);
		   
			if (objectTypes != 0)
			field.objectType = objectTypes.findType(schemaName + "." + typeName)
			else
			}
		*/
		
		switch (oraType) {
        case SQLT_CHR:
        case SQLT_AFC:
			if (dataSize <= 255 || longString) {
				field->mDataType = oraType == SQLT_CHR ? dtString : dtFixedString;
				field->mDataSize = dataSize + 1;
			}
			else {
				field->mDataType = dtLong;
				field->mSubDataType = dtString;
				field->mDataSize = sizeof(void*);
			}
			field->mLength = dataSize;
			field->mFixed = oraType == SQLT_AFC;
			break;
        case SQLT_NUM:
			{
				sb2 precision = 0;
				try
				{
					ub4 size = sizeof(precision);
					OCIAttrGet(hOCIParam, (ub4)OCI_DTYPE_PARAM, /*(dvoid**)*/&precision,
						&size, (ub4)OCI_ATTR_PRECISION, OraEnvironment::getOCIError());
				}
				catch(OraException &e)
				{
					throw e;
				}
				catch(...)
				{
					char errorMsg[130];
					memset( errorMsg, 0, sizeof( errorMsg ) );
					sb4 nativeCode = 0;
					
					if ( OraEnvironment::getOCIError() != NULL )
					{
						// Have a closer look at the error. Retrieve more info from Oracle.
						
						OCIErrorGet( error, 1, 0, &nativeCode, 
							(unsigned char*)&errorMsg, sizeof( errorMsg ), 
							OCI_HTYPE_ERROR );
					}
					
					std::string errorMsgStr( errorMsg );
					printf("oracle error is: [%s ]\n", errorMsgStr.c_str());
				}
				
				check(OCIAttrGet(hOCIParam, (ub4)OCI_DTYPE_PARAM, (dvoid**)&scale,
					(ub4 *)0, (ub4)OCI_ATTR_SCALE, OraEnvironment::getOCIError()));
				
				field->mPrecision = precision;
				field->mScale = abs(scale);
				
				//  fix scale and precision according to datatype
				if (-127 == scale && precision > 0) //FLOAT
					field->mLength = (int)ceil(precision * 0.30103);
				else{
					if (0 == precision)
						field->mPrecision = 38;
					if (-127 == scale)
						field->mScale = 0;
					field->mLength = precision;
				}
				
				if (38 >= field->mPrecision && 0 == field->mScale) {
					//  integer data type
					field->mDataType = dtInteger;
					field->mDataSize = 4;
				}
				else {
					field->mDataType = dtDouble;
					field->mDataSize = 8;
				}
			}
			break;
        case SQLT_DAT:
			field->mDataType = dtDate;
			field->mDataSize = 7;
			break;
        case SQLT_RDD:
        case SQLT_RID:
			field->mDataType = dtString;
			field->mSubDataType = dtRowId;
			field->mDataSize = rowIdSize + 1;
			break;
        case SQLT_BIN:
			field->mDataType = dtBinary;
			field->mDataSize = dataSize;
			break;
        case SQLT_LNG:
			field->mDataType = dtLong;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_LBI:
			field->mDataType = dtLongRaw;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_CLOB:
			field->mDataType = dtClob;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_BLOB:
			field->mDataType = dtBlob;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_BFILEE:
			field->mDataType = dtBFile;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_CFILEE:
			field->mDataType = dtCFile;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_RSET:
			field->mDataType = dtCursor;
			field->mDataSize = sizeof(void*);
			break;
#ifdef OCL_OCI90
        case SQLT_TIMESTAMP:
			field->mDataType = dtTimeStamp;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_TIMESTAMP_TZ:
			field->mDataType = dtTimeStampTZ;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_TIMESTAMP_LTZ:
			field->mDataType = dtTimeStampLTZ;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_INTERVAL_YM:
			field->mDataType = dtIntervalYM;
			field->mDataSize = sizeof(void*);
			break;
        case SQLT_INTERVAL_DS:
			field->mDataType = dtIntervalDS;
			field->mDataSize = sizeof(void*);
			break;
#endif
#ifdef OCL_OCI100	
        case SQLT_IBFLOAT:
			field->mDataType = dtFloat;
			field->mSubDataType = dtBFloat;
			field->mDataSize = sizeof(double);
			break;
        case SQLT_IBDOUBLE:
			field->mDataType = dtFloat;
			field->mSubDataType = dtBDouble;
			field->mDataSize = sizeof(double);
			break;
#endif
			/*
			case SQLT_NTY:
			//field->mDataType = TOraType(Field.ObjectType).DataType;
			field->mDataSize = sizeof(void*);
			break;
			case SQLT_REF:
			field->mDataType = dtReference;
			field->mDataSize = sizeof(void*);
			break;
			case SQLT_OSL:
			field->mDataType = dtLabel;
			field->mDataSize = dataSize;
			break;
			*/
        default:
			throwError(SUnknownDataType, oraType);
			field->mDataType = dtUnknown;
      }
	  
      //get if NULL required for column
      check(OCIAttrGet(hOCIParam, OCI_DTYPE_PARAM, (dvoid **)&isNull,
		  0, OCI_ATTR_IS_NULL, OraEnvironment::getOCIError()));
	  
      field->mRequired = (isNull ? true : false);
      break;
#endif
  }
  return 1;
}

void OraCommand::defineData(OraField* field, void* buf, sb2* ind) {
	
	int oraType;
	
	oraType = getOraType(field->mDataType, field->mSubDataType);
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
    case OCI73:
		check(odefin(mCursorRef->getCDA(), field->mFieldNo, (ub1*)buf, field->mDataSize, oraType,
			-1, ind, 0, -1, -1, 0, 0));
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		OCIDefine* hDefine = 0;
		void* valuePtr = 0;
		word size;
		
		if (field->mSubDataType == dtString)
			size = field->size() + 1;
		else
			size = field->size();
		
		switch (field->dataType()) {
        case dtObject:
        case dtReference:
        case dtArray:
        case dtTable:
			break;
			
        default:
			valuePtr = buf;
		}
		
		check(OCIDefineByPos(mCursorRef->getOCIStmt(), &hDefine, OraEnvironment::getOCIError(),
			field->fieldNo(), valuePtr, size, oraType, ind, 0, 0, OCI_DEFAULT));
		
		switch (field->dataType()) {
        case dtString:
        case dtFixedString:
			if (mConnection->mCharsetId > 0)
				check(OCIAttrSet(hDefine, OCI_HTYPE_DEFINE, &mConnection->mCharsetId, 0, OCI_ATTR_CHARSET_ID, OraEnvironment::getOCIError()));
			break;
		}
		
		field->mHandle = hDefine;
		break;
#endif
	}
}

#ifdef OCL_OCI8
void OraCommand::defineDynamic(OraField* field, void* owner, OCICallbackDefine proc) {
	word oraType;
	OCIDefine *hDefine = 0;
	
	oraType = getOraType(field->dataType(), field->mSubDataType);
	
	check(OCIDefineByPos(mCursorRef->getOCIStmt(), &hDefine, OraEnvironment::getOCIError(),
		field->mFieldNo, 0, maxBlobSize, oraType, 0, 0, 0, OCI_DYNAMIC_FETCH));
	
	check(OCIDefineDynamic(hDefine, OraEnvironment::getOCIError(), owner, proc));
	
	field->mHandle = hDefine;
}
#endif // OCL_OCI8

void OraCommand::defineArrayData(OraField* field, void* buf,
								 sb2* ind, int bufSkip, int indSkip)
{
	int oraType;
	int size;
	
	oraType = getOraType(field->mDataType, field->mSubDataType);
	
	if (field->mSubDataType == dtString)
		size = field->mLength +1;
	else
		size = field->mDataSize;
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		check(odefinps(mCursorRef->getCDA(), 1, field->mFieldNo, (ub1*)buf, size,
			oraType, 0, ind, 0, 0, 0, 0, 0, bufSkip, indSkip, 0, 0));
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		OCIDefine* hOCIDefine = 0;
		void* valuePtr = 0;
		
		switch (field->mDataType) {
        case dtObject:
        case dtReference:
        case dtArray:
        case dtTable:
			break;
			
        default:
			valuePtr = buf;
		}
		
		check(OCIDefineByPos(mCursorRef->getOCIStmt(), &hOCIDefine, OraEnvironment::getOCIError(),
			field->fieldNo(), valuePtr, size, oraType, ind, 0, 0, OCI_DEFAULT));
		
		switch (field->dataType()) {
        case dtString:
        case dtFixedString:
			if (mConnection->mCharsetId > 0)
				check(OCIAttrSet(hOCIDefine, OCI_HTYPE_DEFINE, &mConnection->mCharsetId, 0, OCI_ATTR_CHARSET_ID, OraEnvironment::getOCIError()));
			break;
			
			/*
			dtObject,dtReference,dtArray,dtTable:
            Check(OCIDefineObject(hDefine, OraEnvironment::getOCIError(), TOraType(Field.ObjectType).TDO,
			Buf, nil, nil, nil));
			*/
		}
		
		check(OCIDefineArrayOfStruct(hOCIDefine, OraEnvironment::getOCIError(), bufSkip, indSkip, 0, 0));
		
		field->setHandle(hOCIDefine);
		break;
#endif
	}
}

void OraCommand::definePieceData(OraField* field, void* buf, sb2* ind) {
	int oraType;
	
	oraType = getOraType(field->mDataType, field->mSubDataType);
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		check(odefinps(mCursorRef->getCDA(), 0, field->mFieldNo, (ub1*)buf, maxBlobSize,
			oraType, 0, ind, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		OCIDefine* hOCIDefine = (OCIDefine *)0;      
		
		check(OCIDefineByPos(mCursorRef->getOCIStmt(), &hOCIDefine, OraEnvironment::getOCIError(), field->fieldNo(),
			buf, maxBlobSize, oraType, ind, 0, 0, OCI_DYNAMIC_FETCH));
		
		field->mHandle = hOCIDefine;   
#endif
	}
}

// Fetch
int OraCommand::internalFetch(int rows) {
	sword res;
	int fetched;  
	
	checkActive();
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		// In PtrCDA^.rpc  problem with cursor ???
		mConnection->busyWait();
		res = ofen(mCursorRef->getCDA(), rows);
		mConnection->release();
		
		fetched = mCursorRef->getCDA()->rpc - mFetchedRows;
		mFetchedRows = mCursorRef->getCDA()->rpc;
		
		if (res) {
			if (res == OCI_NO_DATA_FOUND)
				internalCancel();
			else
				check(res);
		}
		else
			setCursorState(csFetching);
		
		return fetched;
#endif
#ifdef OCL_OCI8
    case OCI80:
		int rowsProc;
		
		mConnection->busyWait();
		res = OCIStmtFetch(mCursorRef->getOCIStmt(), OraEnvironment::getOCIError(), rows, OCI_FETCH_NEXT, OCI_DEFAULT);
		mConnection->release();
		
		check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, &rowsProc, 0, OCI_ATTR_ROW_COUNT, OraEnvironment::getOCIError()));
		
		fetched = rowsProc - mFetchedRows;
		mFetchedRows = rowsProc;
		
		if (res) {
			if (res == OCI_NO_DATA)
				setCursorState(csCanceled);
			else
				check(res);
		}
		else
			setCursorState(csFetching);
		
		return fetched;
#endif
	}
	return 0;
}

// For piecewise

int OraCommand::internalFetchPiece() {
	
	sword res;
	
	checkActive();
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		mConnection->busyWait();
		res = ofen(mCursorRef->getCDA(), 1);
		mConnection->release();
		
		if (res) {
			if (res == OCI_NO_DATA_FOUND)
				internalCancel();
			else
				if (res != OCI_STILL_IS_PIECE)
					check(res);
		}
		else
			setCursorState(csFetching);
		
		return res;
#endif
#ifdef OCL_OCI8
    case OCI80:
		mConnection->busyWait();
		res = OCIStmtFetch(mCursorRef->getOCIStmt(), OraEnvironment::getOCIError(), 1, OCI_FETCH_NEXT, OCI_DEFAULT);
		mConnection->release();
		
		if (res != 0) {
			if (res == OCI_NO_DATA)
				setCursorState(csCanceled);
			else
				if (res != OCI_NEED_DATA)
					check(res);
		}
		return res;
#endif
	}
	
	return 0;
}

#ifdef OCL_OCI7
void OraCommand::internalCancel() {
	sword res;
	
	checkActive();
	
	mConnection->busyWait();
    res = ocan(mCursorRef->getCDA());
	mConnection->release();
	
	check(res);
	
	setCursorState(csCanceled);
}

void OraCommand::internalClose() {
	sword res;
	
	checkActive();
	
	busy();
	try { // For Busy
		mConnection->busyWait();
		res = oclose(mCursorRef->getCDA());
		mConnection->release();
		
		check(res);
		
		setCursorState(csInactive);
		// finally
		release();
	}
	catch(OraException &e)
	{
		release();
		throw e;
	}
	catch (...) {
		release();
		throw;
	}
}
#endif // OCL_OCI7

void OraCommand::getPI(void** Handle, byte* Piece, void** Buf, long* Iteration,
					   long* Index, CRParamDirection* mode)
{
	sword res;
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7
    case OCI73:
		mConnection->busyWait();
		res = ogetpi(mCursorRef->getCDA(), Piece, Buf, (ub4*)Iteration, (ub4*)Index);
		mConnection->release();
		
		check(res);
		*Handle = 0;
		break;
#endif
#ifdef OCL_OCI8
    case OCI80:
		ub1 IOMode;
		ub4 HType;
		
		mConnection->busyWait();
		res = OCIStmtGetPieceInfo(mCursorRef->getOCIStmt(), OraEnvironment::getOCIError(), (void**)Handle, &HType,
			&IOMode, (ub4*)Iteration, (ub4*)Index, Piece);
		mConnection->release();
		
		check(res);
		
		switch (IOMode) {
        case OCI_PARAM_IN:
			*mode = pdInput;
			break;
        case OCI_PARAM_OUT:
			*mode = pdOutput;
			break;
        default:
			*mode = pdUnknown;
		}
		break;
#endif
	}
}

void OraCommand::setPI(void* Handle, long HType, byte Piece, void* Buf, long* BufLen, sb2* Ind) {
	sword res;
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		mConnection->busyWait();
		res = osetpi(mCursorRef->getCDA(), Piece, Buf, (ub4*)BufLen);
		mConnection->release();
		
		check(res);
		break;
#endif
#ifdef OCL_OCI8    
    case OCI80:
		mConnection->busyWait();
		res = OCIStmtSetPieceInfo(Handle, HType, OraEnvironment::getOCIError(), Buf, (ub4 *)BufLen, (ub1)Piece, Ind, 0);
		mConnection->release();
		
		check(res);
		break;
#endif
	}
}

const unsigned maxParams = 80;

void OraCommand::initProcParams7(const char* Name, int overload) {
	ub2 ovrld[maxParams];
	ub2 pos[maxParams];
	ub2 level[maxParams];
	char argnm[maxParams][30];
	ub2 arnlen[maxParams];
	ub2 dtype[maxParams];
	ub1 defsup[maxParams];
	ub1 mode[maxParams];
	ub4 dtsize[maxParams];
	sb2 prec[maxParams];
	sb2 scale[maxParams];
	ub1 radix[maxParams];
	ub4 spare[maxParams];
	ub4 arrSize;
	OraParam* param;
	
	arrSize = maxParams;
	
	// cleare previous parameters
	mParams.clear();
	
	check(odessp(mConnection->getLDA(), (text*)Name, strlen(Name), 0, 0, 0, 0,
		ovrld, pos, level, (text**)argnm, arnlen, dtype, defsup, mode, dtsize,
		prec, scale, radix, spare, &arrSize));
	
	assert(arrSize < maxParams);
	
	for (int i = 0; i < (int)arrSize; i++)
		if (level[i] == 0 && dtype[i] != UNKNOWN_TYPE) {
			param = new OraParam();
			if (pos[i] == 0)
				param->mName = "RESULT";
			else {
				argnm[i][arnlen[i]] = 0;
				param->mName = argnm[i];
			};
			
			switch (dtype[i]) {
			case VARCHAR2_TYPE:
				param->mDataType = dtString;
				break;        
			case CHAR_TYPE:
				param->mDataType = dtFixedString;
				break;
			case NUMBER_TYPE:
				if (abs(scale[i]) > 0 || prec[i] > 9 || prec[i] == 0)
					param->mDataType = dtDouble;
				else
					param->mDataType = dtInteger;
				break;
			case INTEGER_TYPE:
				param->mDataType = dtInteger;
				break;
			case DATE_TYPE:
				param->mDataType = dtDate;
				break;
			case ROWID_TYPE:
				param->mDataType = dtString;
				param->setSize(rowIdSize + 1);
				break;
			case RAW_TYPE:
				param->mDataType = dtBinary;
				break;
			case LONG_TYPE:
				param->mDataType = dtLong;
				break;
			case LONGRAW_TYPE:
				param->mDataType = dtLongRaw;
				break;
			case CURSOR_TYPE:
				param->mDataType = dtCursor;
				break;
			case BOOLEAN_TYPE:
				param->mDataType = dtInt;
				param->mSubDataType = dtBoolean;
				break;
			default:
				char msg[256];
				sprintf(msg, SUnsupportedDataType, dtype[i]);
				throwError(msg);
			};
			
			if (param->mDataType == dtCursor)
				param->mDirection = pdOutput;
			else
				switch (mode[i]) {
			case 0:
				param->mDirection = pdInput;
				break;
			case 1:
				param->mDirection = pdOutput;
				break;
			case 2:
				param->mDirection = pdInputOutput;
				break;
			};
			
			param->createValue();
			
			mParams.add(param);
		}
}
#ifdef OCL_OCI8
void OraCommand::initProcParams8(const char* name, int overload) {
	sword res;
	int i;
	OraParam *param;
	OCIDescribe* hDescribe = 0;
	OCIParam *hParam = 0,
		*hArgList = 0,
		*hArgList1,
		*hArg = 0,
		*hProcList = 0;
	word argCount = 0;
	char* pName = 0;
	ub4 len = 0;
	word oraType = 0;
	word size = 0;
	byte scale = 0;
	word prec = 0;
	unsigned mode = 0;
	word procCount = 0;
	unsigned listType = 0;
	word lOverload;
	int d1, d2;
	char st[256], st1[256], procName[256];
	byte objType = 0;
	byte b1 = 0;
	int offset;
	
	memset(st, '\0', sizeof(st));
	memset(st1, '\0', sizeof(st1));
	
	// cleare previous parameters
	mParams.clear();
	
	check(OCIHandleAlloc(OraEnvironment::getOCIEnv(), (void **)&hDescribe, OCI_HTYPE_DESCRIBE, 0, 0));
	
	b1 = 1;
	check(OCIAttrSet(hDescribe, OCI_HTYPE_DESCRIBE, &b1, 0,
		OCI_ATTR_DESC_PUBLIC, OraEnvironment::getOCIError()));
	
	try {
		if (overload == 1)
			overload = 0;
		
		lOverload = 0;
		
		strcpy(procName, name);
		
		strupr(procName);
		
		while (true) {
			char* pos1, *pos2;
			pos1 = strchr(procName, '.');
			pos1 ? d1 = pos1 - procName + 1 : d1 = 0;
			
			if (d1 > 0) {
				pos2 = strchr(pos1 + 1, '.');
				pos2 ? d2 = pos2 - pos1 : d2 = 0;
				
				if (d2) {
					d1 = d1 + d2;
					d2 = d1 - d2;
				}
			}
			else
				d2 = 0;
			
			if (d1 > 0)
				strncpy(st, procName, d1 - 1);
			else
				strcpy(st, procName);
			
			res = OCIDescribeAny(mConnection->getOCISvcCtx(), OraEnvironment::getOCIError(), st, strlen(st),
				OCI_OTYPE_NAME, 0, OCI_PTYPE_UNK, hDescribe);
			
			if (res != 0 && d1 > 0 && d2 == 0) {
				strcpy(st, procName);
				check(OCIDescribeAny(mConnection->getOCISvcCtx(), OraEnvironment::getOCIError(), st, strlen(st),
					OCI_OTYPE_NAME, 0, OCI_PTYPE_UNK, hDescribe));
			}
			else
				check(res);
			
			check(OCIAttrGet(hDescribe, OCI_HTYPE_DESCRIBE, &hParam, 0,
				OCI_ATTR_PARAM, OraEnvironment::getOCIError()));
			
			check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &objType, 0,
				OCI_ATTR_PTYPE, OraEnvironment::getOCIError()));
			
			switch (objType) {
			case OCI_PTYPE_PROC:
			case OCI_PTYPE_FUNC:
				break;
				
			case OCI_PTYPE_PKG:
			case OCI_PTYPE_TYPE:
				check(OCIAttrGet(hDescribe, OCI_HTYPE_DESCRIBE, &hParam, 0,
					OCI_ATTR_PARAM, OraEnvironment::getOCIError()));
				
				if (objType == OCI_PTYPE_PKG) {
					//get the subprogramm list
					check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &hProcList, 0,
						OCI_ATTR_LIST_SUBPROGRAMS, OraEnvironment::getOCIError()));
					
					offset = 0;
				}
				else {
					check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &hProcList, 0,
						OCI_ATTR_LIST_TYPE_METHODS, OraEnvironment::getOCIError()));
					
					offset = 1;
				}
				
				//get the number of procs
				check(OCIAttrGet(hProcList, OCI_DTYPE_PARAM, &procCount, 0,
					OCI_ATTR_NUM_PARAMS, OraEnvironment::getOCIError()));
				
					/*
					if (!d2)
					strcpy(st, procName + d1);
					else
					strcpy(st, procName + d2);
				*/
				
				strcpy(st, procName + d1);
				i = offset;
				
				while (i < procCount + offset) {
					//get the parametr handler
					check(OCIParamGet(hProcList, OCI_DTYPE_PARAM, OraEnvironment::getOCIError(), (void **)&hParam, i));
					
					check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &pName, &len, OCI_ATTR_NAME, OraEnvironment::getOCIError()));
					check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &overload, 0, OCI_ATTR_OVERLOAD_ID, OraEnvironment::getOCIError()));
					
					if (lOverload == 1)
						lOverload = 0;
					
					memset(st1, '\0', sizeof(st1));
					strncpy(st1, pName, len);
					
					if ((stricmp(st, st1) == 0) && (lOverload == overload))
						break;
					
					i++;
				}
				
				if (i == procCount + offset) {
					char msg[512];
					sprintf(msg, SObjectNotExist, procName);
					throwError(msg);
				}
				break;
				
			case OCI_PTYPE_SYN:
				char buf[256];
				if (d1 > 0) {
					strcpy(buf, ".");
					strcat(buf, procName + d1);
					strcpy((char*)procName, buf);
				}
				else
					*procName = 0;
				
				check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &pName, &len, OCI_ATTR_NAME,
					OraEnvironment::getOCIError()));
				strcpy(buf, pName);
				strcat(buf, procName);
				strcpy(procName, buf);
				check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &pName, &len, OCI_ATTR_SCHEMA_NAME,
					OraEnvironment::getOCIError()));
				strcpy(buf, pName);
				strcat(buf, ".");
				strcat(buf, procName);
				strcpy(procName, buf);
				
				continue;
				
			default:
				char msg[512];
				sprintf(msg, SObjectNotExist, st);
				throwError(msg);
			} //switch
			
			//get the arg list
			check(OCIAttrGet(hParam, OCI_DTYPE_PARAM, &hArgList, 0, OCI_ATTR_LIST_ARGUMENTS,
				OraEnvironment::getOCIError()));
			
			//get the number of arguments
			check(OCIAttrGet(hArgList, OCI_DTYPE_PARAM, &argCount, 0, OCI_ATTR_NUM_PARAMS,
				OraEnvironment::getOCIError()));
			
			if (OraEnvironment::getOCIVersion() < OCI81) {
				if (objType == OCI_PTYPE_FUNC)
					listType = OCI_LTYPE_ARG_FUNC;
			}
#ifdef OCL_OCI81
			else
				//get the type of subprogramm (8.1.5 and above)
				check(OCIAttrGet(hArgList, OCI_DTYPE_PARAM, &listType, 0, OCI_ATTR_LTYPE,
				OraEnvironment::getOCIError()));
#endif
			
			assert(argCount < maxParams);
			
			param = 0;
			hArgList1 = 0;
			
			if (listType == OCI_LTYPE_ARG_FUNC ||
				listType == OCI_LTYPE_TYPE_ARG_FUNC) {
				
				i = 0;
				argCount--;
			}
			else
				i = 1;
			
			while (i <= argCount) {
				if (hArgList1 == 0)
					check(OCIParamGet(hArgList, OCI_DTYPE_PARAM, OraEnvironment::getOCIError(), (void **)&hArg, i));
				else
					check(OCIParamGet(hArgList1, OCI_DTYPE_PARAM, OraEnvironment::getOCIError(), (void **)&hArg, 1));
				
				check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &oraType, 0, OCI_ATTR_DATA_TYPE,
					OraEnvironment::getOCIError()));
				
				if ((oraType != dtUnknown) && (lOverload == overload)) {
					if (hArgList1 == 0) {
						if (i > 0)
							check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &pName, &len, OCI_ATTR_NAME,
							OraEnvironment::getOCIError()));
						
						check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &mode, 0, OCI_ATTR_IOMODE,
							OraEnvironment::getOCIError()));
						
						param = new OraParam();
						
						char buf[512];
						memset((void*)buf, '\0', 512);
						
						if (i > 0)
							param->mName = (const char*)strncpy(buf, (const char*)pName, len); 
						else
							param->mName.assign("RESULT");
						
						switch (mode) {
						case 0:
							param->mDirection = pdInput;
							break;
							
						case 1:
							param->mDirection = pdOutput;
							break;
							
						case 2:
							param->mDirection = pdInputOutput;
							break;
						}
					}
					
					assert(param != 0);
					
					if (oraType != SQLT_NTY) {
						check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &size, 0, OCI_ATTR_DATA_SIZE,
							OraEnvironment::getOCIError()));
						check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &scale, 0, OCI_ATTR_SCALE,
							OraEnvironment::getOCIError()));
						check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &prec, 0, OCI_ATTR_PRECISION,
							OraEnvironment::getOCIError()));                  
					}
					
					// Use default value OCI_ATTR_HAS_DEFAULT
					switch (oraType) {
					case SQLT_CHR:
						param->mDataType = dtString;
						break;            
					case SQLT_AFC:
						param->mDataType = dtFixedString;
						break;
					case SQLT_NUM:
						if ((abs(scale) > 0) || (prec > 9) || (prec == 0))
							param->mDataType = dtDouble;
						else
							param->mDataType = dtInteger;
						break;
					case INTEGER_TYPE:
						param->mDataType = dtInteger;
						break;
					case SQLT_DAT:
						param->mDataType = dtDate;
						break;
					case SQLT_RID:
					case SQLT_RDD:
						param->mDataType = dtString;
						param->mDataSize = rowIdSize + 1;
						break;
					case SQLT_BIN:
						param->mDataType = dtBinary;
						break;
					case SQLT_LNG:
						param->mDataType = dtLong;
						break;
					case SQLT_LBI:
						param->mDataType = dtLongRaw;
						break;
					case SQLT_CUR:
						param->mDataType = dtCursor;
						param->mDirection = pdOutput;
						break;
					case BOOLEAN_TYPE:
						param->mDataType = dtInt;
						param->mSubDataType = dtBoolean;
						break;
						
						/*
						case SQLT_TAB:
						check(OCIAttrGet(hArg, OCI_DTYPE_PARAM, &hArgList1, 0, OCI_ATTR_LIST_ARGUMENTS,
						OraEnvironment::getOCIError()));
						param->mTable = true;
						param->mLength = 1;
						continue;
						*/
						
						//Oracle 8 types
					case SQLT_CLOB:
						param->mDataType = dtClob;
						break;
					case SQLT_BLOB:
						param->mDataType = dtBlob;
						break;
					case SQLT_BFILE:
						param->mDataType = dtBFile;
						break;
					case SQLT_CFILE:
						param->mDataType = dtCFile;
						break;
						
#ifdef OCL_OCI90
						// Oracle 9i types
					case SQLT_TIMESTAMP:
						param->mDataType = dtTimeStamp;
						break;
					case SQLT_TIMESTAMP_TZ:
						param->mDataType = dtTimeStampTZ;
						break;
					case SQLT_TIMESTAMP_LTZ:
						param->mDataType = dtTimeStampLTZ;
						break;
					case SQLT_INTERVAL_YM:
						param->mDataType = dtIntervalYM;
						break;
					case SQLT_INTERVAL_DS:
						param->mDataType = dtIntervalDS;
						break;
#endif
						
						//  TODO it was commented maybe not worked
						// need check
					case SQLT_NTY:
						param->mDataType = dtObject;
						break;
					case SQLT_REF:
						param->mDataType = dtReference;
						break;
					case SQLT_NCO:
						param->mDataType = dtTable;
						break;
						//            case SQLT_VARRAY:
						//            param->mDataType = dtArray;
						//            break;
						
#ifdef OCL_OCI100	
					case SQLT_IBFLOAT:
						param->mDataType = dtFloat;
						break;
					case SQLT_IBDOUBLE:
						param->mDataType = dtFloat;
						break;
#endif
						
					default:
						char msg[256];
						sprintf(msg, SUnsupportedDataType, oraType);
						throwError(msg);
          }
            }
            
            hArgList1 = 0;
            i++;
            
            param->createValue();
            mParams.add(param);
          }  
          break;
        } //while
      }
	  catch(OraException &e)
	  {
		  OCIHandleFree(hDescribe, OCI_HTYPE_DESCRIBE);
		  throw e;
	  }
      catch(...) {
		  
		  OCIHandleFree(hDescribe, OCI_HTYPE_DESCRIBE);
		  
		  throw;
		  
      }
      
      OCIHandleFree(hDescribe, OCI_HTYPE_DESCRIBE);  
}
#endif // OCL_OCI8

void OraCommand::initProcParams(const char* name, int overload) {
	CRString St;
	
	checkSession();
	setOCICallStyle(mConnection->OCICallStyle());
	
	if (mOCICallStyle == OCI73)
		initProcParams7(name, overload);
	else
		// gcc 2.96 basic_string not support compare for two pointers
		if (OraEnvironment::getOCIVersion() < OCI81 || strncmp(mConnection->serverVersion().c_str(), "7", 1) == 0)
			initProcParams7(name, overload);
#ifdef OCL_OCI8
		else
			initProcParams8(name, overload);
#endif
}

// Open/close

void OraCommand::prepare() {
	
	checkSession();
	
	if (!isPrepared()) {
		checkInactive();
		
		mRowsProcessed = 0;
		mErrorOffset = 0;
		
		setOCICallStyle(mConnection->mOCIVersion);
		
		mCursorRef->allocCursor();
		
		createSQL();
		
		trace(OraMonitor::mePrepare, OraMonitor::tpBefor);
		
		try {
			switch (mOCICallStyle) {
#ifdef OCL_OCI7
			case OCI73:
				internalOpen();
				
				try {
					internalParse();
				}
				catch (...) {
					internalClose();
					
					throw;
				}
				break;
#endif
#ifdef OCL_OCI8
			case OCI80:
				setCursorState(csOpen);
				internalPrepare();
#endif
			}
		}
		catch(OraException &e)
		{
			mCursorRef->freeCursor();
			
			throw e;
		}
		catch (...) {
			mCursorRef->freeCursor();
			throw;
		}
		
		setCursorState(csPrepared);
		
		trace(OraMonitor::mePrepare, OraMonitor::tpAfter);
	}
}

void OraCommand::unprepare() {
	
	if (isActive() && isPrepared()) {
		trace(OraMonitor::meUnprepare, OraMonitor::tpBefor);
		try {
			switch (mOCICallStyle) {
#ifdef OCL_OCI7
			case OCI73:
				try {
					internalClose();
					
					// finally
					mCursorRef->freeCursor();
				}
				catch (...) {
					mCursorRef->freeCursor();
					throw;
				};
				break;
#endif
#ifdef OCL_OCI8
			case OCI80:
				busy();
				
				try {
					mConnection->busyWait();
					try {
						if (mCursor != mCursorRef){
							mCursorRef->freeCursor();
							mCursorRef->release();
						}
						mCursor->freeCursor();
						mCursorRef = mCursor;
						mConnection->release();
					}
					catch(...) {
						mConnection->release();
						throw;
					}
					release();
				}
				catch(...) {
					release();
					throw;
				}
#endif
			}
			setCursorState(csInactive);
			// finally
		}
		catch(OraException &e)
		{
			setCursorState(csInactive);
			throw e;
		}
		catch (...) {
			setCursorState(csInactive);
			throw;
		}
		
		trace(OraMonitor::meUnprepare, OraMonitor::tpAfter);    
	}
}

bool OraCommand::isActive() const {
	
	return cursorState() != csInactive;
}

bool OraCommand::isPrepared() const {
	
	return cursorState() >= csPrepared;
}

int OraCommand::rowsReturn() {
	
	checkActive();
	
	return (mSQLType == SQL_SELECT || mCursorRef != mCursor &&
		mCursorRef->mState >= csExecuted);
}

// Check return SQL statement rows
void OraCommand::checkRowsReturn() {
	
	if (!rowsReturn())
		throwError(SRowsNotReturn);
}

// Executing

void OraCommand::exec() {
	const int BufSize = 100;
	sword res;
	int i;
	ub1 PieceStatus;
	CRLob* Blob;
	PieceHeader* Piece;
	long BufLen;
	long Iteration;
	long index;
	byte Buf[BufSize];
	
	res = internalExecute(OCI_DEFAULT);
	
	if (res == OCI_SUCCESS) {
		
#ifdef OCL_OCI8
		// Reading/Writing LOBs
		
		OraParam *param;
		
		for (i = 0; i < mParams.count(); i++) {
			param = &mParams[i];
			if ((param->dataType() == dtBlob || param->dataType() == dtClob) &&
				!param->isNull())
				
				if (param->direction() == pdInput)
					(param->getBlob()).writeLob();
				else
					if (param->direction() == pdOutput)
						(param->getBlob()).readLob();
					else
						throwError(SNeedParamType);
		}
#endif
	}
	else {
		// Read/Write Piecewise
		
		switch (mOCICallStyle) {
#ifdef OCL_OCI8     
		case OCI80:
			void* hBind;
			CRParamDirection mode;
			
			getPI(&hBind, &PieceStatus, 0, &Iteration, &index, &mode);
			
			do {
				for (i = 0; i < mParams.count(); i++)
					if (mParams[i].mHandle == hBind)
						break;
					
					assert(i < mParams.count());
					
					Blob = &(mParams[i].getLob());
					
					if (mode == pdInput) {
						Piece = ((OraLob*)Blob)->firstPiece();
						
						if (Piece != 0)
							do {
								BufLen = Piece->used;
								
								if (Piece->next == 0)
									PieceStatus = OCI_LAST_PIECE;
								
								
								setPI(hBind, OCI_HTYPE_BIND, PieceStatus, (char*)Piece + sizeof(PieceHeader), &BufLen, 0);
								
								res = internalExecute(OCI_DEFAULT);
								
								getPI(&hBind, &PieceStatus, 0, &Iteration, &index, &mode);
								
								Piece = Piece->next;
							} while (!((res == OCI_SUCCESS) || (PieceStatus == OCI_FIRST_PIECE) || (PieceStatus == OCI_ONE_PIECE)));
							else {
								BufLen = 0;
								
								setPI(hBind, OCI_HTYPE_BIND, OCI_LAST_PIECE, Buf, &BufLen, 0);
								
								res = internalExecute(OCI_DEFAULT);
								
								getPI(&hBind, &PieceStatus, 0, &Iteration, &index, &mode);
							}
					}
					else
						if (mode == pdOutput) {
							Blob->clear();
							
							do {
								BufLen = ((OraLob*)Blob)->defPieceSize();
								
								Piece = ((OraLob*)Blob)->allocPiece(BufLen);
								
								setPI(hBind, OCI_HTYPE_BIND, PieceStatus, (char*)Piece + sizeof(PieceHeader), &BufLen, 0);
								
								res = internalExecute(OCI_DEFAULT);
								
								Piece->used = BufLen;
								
								if (BufLen < ((OraLob*)Blob)->defPieceSize())
									((OraLob*)Blob)->reallocPiece(&Piece, BufLen);
								
								((OraLob*)Blob)->appendPiece(Piece);
								
								getPI(&hBind, &PieceStatus, 0, &Iteration, &index, &mode);
								
							} while (!((res == OCI_SUCCESS) || (PieceStatus == OCI_FIRST_PIECE) || (PieceStatus == OCI_ONE_PIECE)));
						}
			} while (res != OCI_SUCCESS);
			break;
#endif
#ifdef OCL_OCI7      
		case OCI73:
			int NeedData = OCI_STILL_IS_PIECE1;
			int IsData = OCI_STILL_IS_PIECE;
			void* Ptr;
			void* hDefine;
			long HType = 0;
			
			// IN params
			for (i = 0; i < paramCount(); i++) {
				if ((mParams[i].mDataType == dtLongRaw || mParams[i].mDataType == dtLong) &&
					(mParams[i].mDirection == pdInput || mParams[i].mDirection == pdInputOutput) &&
					!mParams[i].isNull())
				{
					if (res != NeedData) {
						throwError(STooNanyOutputParams);
						break;
					};
					
					getPI(&hDefine, &PieceStatus, &Ptr, &Iteration, &index, 0);
					
					Blob = &(mParams[i].getLob());
					assert(Blob);
					
					Piece = ((OraLong*)Blob)->firstPiece();
					if (Piece != 0)
						do {
							BufLen = Piece->used;
							if (!Piece->next)
								PieceStatus = OCI_LAST_PIECE;
							
							setPI(hDefine, HType, PieceStatus, (char*)Piece + sizeof(PieceHeader), &BufLen, 0);
							
							res = internalExecute(OCI_DEFAULT);
							
							getPI(&hDefine, &PieceStatus, &Ptr, &Iteration, &index, 0);
							
							Piece = Piece->next;
						}
						while (res != OCI_SUCCESS && PieceStatus != OCI_FIRST_PIECE);
						else {
							BufLen = 0;
							setPI(hDefine, HType, OCI_LAST_PIECE, &Buf, &BufLen, 0);
							res = internalExecute(OCI_DEFAULT);
						}
				}
				
				if (res == NeedData && mOCICallStyle == OCI73) {
					throwError(SNotEnoughInputParams);
					while (res == NeedData) {
						BufLen = 0;
						setPI(hDefine, HType, OCI_LAST_PIECE, &Buf, &BufLen, 0);
						res = internalExecute(OCI_DEFAULT);
					}
				}
			}
			
			// OUT params
			for (i = 0; i < paramCount(); i++)
				if ((mParams[i].mDataType == dtLongRaw || mParams[i].mDataType == dtLong) &&
					(mParams[i].mDirection == pdOutput || mParams[i].mDirection == pdInputOutput))
				{
					if (res != IsData) {
						throwError(STooNanyOutputParams);
						break;
					}
					
					getPI(&hDefine, &PieceStatus, &Ptr, &Iteration, &index, 0);
					
					Blob = &(mParams[i].getLob());
					assert(Blob);
					
					Blob->clear();
					
					do {
						BufLen = ((OraLong*)Blob)->defPieceSize();
						Piece = ((OraLong*)Blob)->allocPiece(BufLen);
						
						setPI(hDefine, HType, PieceStatus, (char*)Piece + sizeof(PieceHeader), &BufLen, 0);
						
						res = internalExecute(OCI_DEFAULT);
						
						Piece->used = BufLen;
						if (BufLen < ((OraLong*)Blob)->defPieceSize())
							((OraLong*)Blob)->reallocPiece(&Piece, BufLen);
						((OraLong*)Blob)->appendPiece(Piece);
						
						getPI(&hDefine, &PieceStatus, &Ptr, &Iteration, &index, 0);
						
					} while (res != OCI_SUCCESS && PieceStatus != OCI_FIRST_PIECE);
				}
				
				if (res == IsData) {
					throwError(SNotEnoughOutputParams);
					while (res == IsData) {
						BufLen = BufSize;
						setPI(hDefine, HType, OCI_LAST_PIECE, &Buf, &BufLen, 0);
						res = internalExecute(OCI_DEFAULT);
					}
				}
				break;
#endif
    }
  }
  
  // AutoCommit
  if (res == OCI_SUCCESS &&
	  mSQLType != SQL_SELECT &&
	  mAutoCommit &&
	  mConnection->isAutoCommit() &&
	  !mConnection->inTransaction())
  {
	  mConnection->commit();
  }
}

#ifdef OCL_OCI8

sb4 OraCommand::callbackInBind(OCIBind* bind, ub4 iter, ub4 index,
							   void** buf, ub4* bufLen, ub1* pieceStatus, void** ind)
{
	OraParam* param;
	CRLob* lob;
	PieceHeader* piece;
	
	int iBreakPos = 0;
	for (int i = 0; i < mParams.count(); i++)
	{
		if (mParams[i].mHandle == bind)
		{
			iBreakPos = i;
			break;
		}
	}
	assert(iBreakPos < mParams.count());
	
	param = &mParams[iBreakPos];
	lob = &param->getLob();
	
	piece = lob->firstPiece();
	if (*pieceStatus == OCI_FIRST_PIECE)
		param->mBlobPieceCount = 0;
	else {
		int i = 0;
		while ((piece != 0) && (i < param->mBlobPieceCount)) {
			piece = piece->next;
			i++;
		}
	}
	
	if (piece != 0) {
		*buf = (char*)piece + sizeof(PieceHeader);
		*bufLen = piece->used;
		*ind = param->indicatorPtr();
		if (piece->next == 0)
			*pieceStatus = OCI_LAST_PIECE;
		else
			if (param->mBlobPieceCount == 0)
				*pieceStatus = OCI_FIRST_PIECE;
			else
				*pieceStatus = OCI_NEXT_PIECE;
			param->mBlobPieceCount++;
	}
	else {
		*buf = 0;
		*bufLen = 0;
		*ind = 0;
		*pieceStatus = OCI_LAST_PIECE;
	}
	
	return OCI_CONTINUE;
}

sb4 OraCommand::callbackOutBind(OCIBind* bind, ub4 iter, ub4 index,
								void** buf, ub4** bufLen, ub1* pieceStatus, void** ind)
{
	CRLob* lob;
	PieceHeader* piece;
	long len;
	
	int iBreakPos = 0;
	for (int i = 0; i < mParams.count(); i++)
		if (mParams[i].mHandle == bind)
		{
			iBreakPos = i;
			break;
		}
		assert(iBreakPos < mParams.count());
		
		lob = &mParams[iBreakPos].getLob();
		
		if (*pieceStatus == OCI_ONE_PIECE)
			lob->clear();
		
		len = ((OraLob*)lob)->defPieceSize();
		piece = ((OraLob*)lob)->allocPiece(len);
		((OraLob*)lob)->appendPiece(piece);
		
		*buf = (char*)piece + sizeof(PieceHeader);
		
		piece->used = len;
		*bufLen = (ub4*)&piece->used;
		
		*ind = mParams[index].indicatorPtr();
		
		*pieceStatus = OCI_NEXT_PIECE;
		
		return OCI_CONTINUE;
}

#endif // OCL_OCI8

void OraCommand::doExecute() {
	
	int oldPrepared;
	
	oldPrepared = isPrepared();
	
	if (!oldPrepared)
		prepare();
	
	try {
		
		bindParams();
		
		exec();
		
		// finally
		if (!oldPrepared)
			unprepare();
	}
	catch(OraException &e)
	{
		if (!oldPrepared)
			unprepare();
		
		throw e;
	}
	catch (...) {
		
		if (!oldPrepared)
			unprepare();
		
		throw;
	}
}

int OraCommand::execute(int iters /* = 1 */) {
	
	if (iters <= 0)
		iters = 1;
	
	mItersCount = iters;
	
	return execute((OraRecordset**)0);
}

int OraCommand::execute(OraRecordset** rs, bool fieldsInfoOnly /* = false */) {
	
	if (rs)
		checkActiveRecordset();
	
	if (cursorState() == csExecuting)
		return 0;
	
	
	switch (mCommandType) {
    case ctTable:
		if (mCommandText.empty())
			throw CRException(STableNameNotDef);
		break;
    case ctStoredProc:
		if (mCommandText.empty())
			throw CRException(SStoredProcNameNotDef);
		break;
	}
	
	if (!isPrepared()) {
		prepare();
		mImplicitPrepared = true;
	}
	else
		mImplicitPrepared = false;
	
	try {
		if (rs)
			*rs = 0;
		
		if (!fieldsInfoOnly)
			doExecute();
		
		if (isRowsReturn() && rs != 0) {
			if (!mRecordset)
				mRecordset = new OraRecordset(*this);
			
			mRecordset->initFields();
			*rs = mRecordset;
			
			if (fieldsInfoOnly) {
				if (mImplicitPrepared)
					unprepare();
				return 0;
			}
			
			mRecordset->open();
		}
		
		//finnaly
		if (mImplicitPrepared && !isRowsReturn())
			unprepare();
		
		endExecute(true);
	}
	catch(OraException &e)
	{
		endExecute(false);
		
		//finnaly
		if (mImplicitPrepared)
			unprepare();
		
		throw e;
	}
	catch (...) {
		endExecute(false);
		
		//finnaly
		if (mImplicitPrepared)
			unprepare();
		
		throw;
	}
	
	return rowsProcessed();  
}

OraRecordset& OraCommand::executeQuery() {
	
	checkActiveRecordset();
	
	if (cursorState() == csExecuting)
		throw OraException(SStillExecuting);
	
	OraRecordset* rs;
	
	execute(&rs);
	
	if (!rs)
		throw OraException(SRowsNotReturn);
	
	return *rs;
}

void OraCommand::cancel() {
	
	if (cursorState() != csInactive)
		mConnection->breakExec();
}

void OraCommand::endExecute(bool result) {
}

// Params

void OraCommand::scanParams() {
	OraParam* param;
	OraParser parser(mSQL.c_str());
	int codeLexem;
	CRString st;
	
	mParams.clear();
	parser.setOmitBlank(false);
	parser.toBegin();
	
	do {
		
		do {
			codeLexem = parser.next(st);
			if (codeLexem == lxTRIGGER)
				codeLexem = lcEnd;
		} while (codeLexem != lcEnd && st[0] != ':');
		
		if (st[0] == ':') {
			
			codeLexem = parser.next(st);
			
			if (codeLexem == lcIdent || codeLexem == lcNumber) {
				
				param = mParams.findItem(st);
				
				if (!param) {
					param = new OraParam();
					param->mName = st;
					param->mDataType = dtUnknown;
					mParams.add(param);
				}
			}
		}
	} while (codeLexem != lcEnd);
}

void OraCommand::bindParams() {
	
	mConnection->busyWait();
	
	try {
		for (int i = 0; i < paramCount(); i++)
			bindParam(&mParams[i]);
		
		// finally
		mConnection->release();
	}
	catch(OraException &e)
	{
		mConnection->release();
		throw e;
	}
	catch (...) {
		mConnection->release();
		throw;
	}
	
	setCursorState(csBound);
}

int OraCommand::paramCount() {
	checkParams();
	return mParams.count();
}

OraParams& OraCommand::params() {
	checkParams();
	return mParams;
}

OraParam& OraCommand::param(int index) {
	checkParams();
	return mParams.item(index);
}

OraParam& OraCommand::param(const CRString& name) {
	checkParams();
	return mParams.item(name);
}

OraParam& OraCommand::param(const char* name) {
	checkParams();
	return mParams.item(name);
}

OraParam* OraCommand::findParam(int index) {
	checkParams();
	return mParams.findItem(index);
}

OraParam* OraCommand::findParam(const CRString& name) {
	checkParams();
	return mParams.findItem(name.c_str());
}

// Stored proc

#ifdef OCL_DEPRECATED
void OraCommand::createProcCall(const char* name) {
	setCommandType(ctStoredProc);
	setCommandText(name);
}
#endif

void OraCommand::generateProcCall(const char* name) {
	CRString st, boolConvertSt;
	bool added;
	OraParam* prm;
	
	if (name == 0 || *name == '\0')
		return;
	
	initProcParams(name, 0);
	
	st = "begin\n  ";
	if ((prm = findParam("RESULT")) != 0) {
		if (prm->mSubDataType != dtBoolean)
			st = st + ":RESULT := ";
		else
			st = st + "RESULT := ";
	}
	st = st + name;
	
	added = false;
	for (int i = 0; i < paramCount(); i++) {
		if (param(i).mSubDataType == dtBoolean) {
			if (param(i).direction() != pdOutput)
				boolConvertSt += "  " + param(i).name() + " " + "boolean := sys.diutil.int_to_bool(:"
				+ param(i).name() + ");\n";
			else
				boolConvertSt += "  " + param(i).name() + " boolean;\n";
		}
		if (param(i).name() != "RESULT") {
			if (!added)
				st = st + "(";
			else
				st = st + ", ";
			if (param(i).mSubDataType != dtBoolean)
				st = st + ":" + param(i).name();
			else
				st = st + param(i).name();
			added = true;
		}
	}
	if (added)
		st = st + ");";
	else
		st += ';';
	
	if (!boolConvertSt.empty())
		st.insert(0, "declare\n" + boolConvertSt);
	
	for (int ii = 0; ii < paramCount(); ii++)
		if (param(ii).mSubDataType == dtBoolean)
			if (param(ii).direction() == pdOutput || param(ii).direction() == pdInputOutput)
				st += "\n  :" + param(ii).name() + " := sys.diutil.bool_to_int("
				+ param(ii).name() + ");";
			
			st = st + "\nend;";
			
			mSQL = st;
}

void OraCommand::createSQL(){
	
	if (mSQL.empty())
		switch (mCommandType) {
      case ctSQL:
		  mSQL = mCommandText;
		  scanParams();
		  break;
      case ctTable:
		  if (!mCommandText.empty())
			  mSQL = "SELECT * FROM " + mCommandText;
		  break;
      case ctStoredProc:
		  // if mSQL will not bee assigned then recursion
		  // and stack overflow take place
		  mSQL = mCommandText;
		  try {
			  generateProcCall(mCommandText);
		  }
		  catch(OraException &e)
		  {
			  mSQL = "";
			  throw e; 
		  }
		  catch (...) {
			  mSQL = "";
			  throw;
		  }
		  break;
    }
}

OraConnection* OraCommand::connection() const {
	
	return mConnection;
}

int OraCommand::errorOffeset() const {
	
	return mErrorOffset;
}

void OraCommand::setConnection(OraConnection& value) {
	
	setConnection(&value);
}

void OraCommand::setConnection(OraConnection* value) {
	
	if (value && value != mConnection) {
		
		checkActiveRecordset();
		
		if (mConnection)
            mConnection->removeCommand(this);

		unprepare();
		mConnection = value;
		setOCICallStyle(value->OCICallStyle());

		if (mConnection)
            mConnection->addCommand(this);
	}
}

void OraCommand::setCommandText(const CRString& value) {
	
	checkActiveRecordset();
	
	if (mCommandText != value) {
		mCommandText = value;
		mSQL = "";
		unprepare();
	}
}

void OraCommand::setCommandType(CRCommandType value) {
	
	checkActiveRecordset();
	
	if (mCommandType != value) {
		mCommandType = value;
		mSQL = "";
		unprepare();
	}
}

CRCursorState OraCommand::cursorState() const {
	
	return mCursorRef->mState;
}

void OraCommand::setCursorState(CRCursorState value) {
	
	if (mNonBlocking) {
		busy();
		mCursorRef->mState = value;
		release();
	}
	else
		mCursorRef->mState = value;
}

OraCursor* OraCommand::cursor() const {
	
	return mCursorRef;
}

void OraCommand::setCursor(OraCursor& value){
	setCursor(&value);
}  

void OraCommand::setCursor(OraCursor* value) {
	
	if (value != mCursorRef) {
		if (mCursorRef && mCursorRef != mCursor)
			mCursorRef->release();
		
		if (value && value != mCursor) {
			mCursorRef = value;
			mCursorRef->addRef();
			mOCICallStyle = mCursorRef->mOCICallStyle;
			mFetchedRows = 0;
		}
		else
			mCursorRef = mCursor;
	}
}

bool OraCommand::nativeCursor() {
	
	return mCursorRef == mCursor;
}

CRString OraCommand::SQL() const {
	if (mCommandType == ctSQL)
		return mCommandText;
	else
		return mSQL;
}

void OraCommand::setSQL(const CRString& value) {
	
	if (!value.empty()) {
		setCommandType(ctSQL);
		setCommandText(value);
	}
}

void OraCommand::busy() {
}

void OraCommand::release() {
}

const char* OraCommand::rowId() const {
	
	checkActive();
	
	switch (mOCICallStyle) {
#ifdef OCL_OCI7    
    case OCI73:
		return "";
#endif
#ifdef OCL_OCI8
    case OCI80:
		return "";
#endif
	}
	return 0;
}

int OraCommand::rowsProcessed() const {
	int result;
	
	//checkActive;
	
	if (mFetchedRows > 0)
		result = mFetchedRows;
	else
		if (mRowsProcessed > 0)
			result = mRowsProcessed;
		else
			if (cursorState() != csInactive) {
				
				switch (mOCICallStyle) {
#ifdef OCL_OCI7
				case OCI73:
					mConnection->busyWait();
					result = mCursorRef->getCDA()->rpc;
					mConnection->release();
					break;
#endif
#ifdef OCL_OCI8
				case OCI80:
					check(OCIAttrGet(mCursorRef->getOCIStmt(), OCI_HTYPE_STMT, (dvoid *)&result,
						0, OCI_ATTR_ROW_COUNT, OraEnvironment::getOCIError()));
#endif
				}
			}
			else
				result = 0;
			
			return result;
}

OCIVersion OraCommand::OCICallStyle() const {
	
	return mOCICallStyle;
}

void OraCommand::setOCICallStyle(OCIVersion value) {
	
	if (value != mOCICallStyle) {
		unprepare();
		
		mOCICallStyle = value;
		mCursorRef->setOCICallStyle(mOCICallStyle);
	}
}

int OraCommand::SQLType() const {
	
	return mSQLType;
}

bool OraCommand::isQuery() const {
	
	return mSQLType == SQL_SELECT;
}

bool OraCommand::isPLSQL() const {
	
	return mSQLType == SQL_PLSQL;
}

bool OraCommand::isRowsReturn() const {
	
	bool res = false;
	
	if (mSQLType == SQL_SELECT)
		res = true;
	else
		for (int i = 0; i < mParams.count(); i++) {
			if (mParams[i].dataType() == dtCursor && mParams[i].direction() == pdOutput) {
				res = true;
				break;
			}
		}
		
		return res;
}

void OraCommand::setArrayLength(int newLength)
{
	for (int i = 0; i < paramCount(); i++) {
		param(i).setTableLength(newLength);
	}
}

void OraCommand::disconnect()
{
	if (cursorState() > csInactive)
		unprepare();
	
	if (mCursorRef != mCursor)
		mCursorRef->freeCursor();
	
	if (mRecordset)
		mRecordset->close();
	
	mCursor->freeCursor();
	
	for (OraParams::size_type i = 0; i < mParams.size(); i++)
		mParams[i].disconnectObject();
}

void OraCommand::trace(OraMonitor::MonitorEvent monitorEvent,
					   OraMonitor::TracePoint tracePoint)
{
	if (!OraMonitor::isActive())
		return;
	
	OraMonitor::MonitorMsg msg;
	
	msg.monitorEvent = monitorEvent;
	msg.tracePoint   = tracePoint;
	msg.ownerName    = mConnection->name();
	msg.objectName   = name();
	msg.ownerID      = reinterpret_cast<int>(mConnection);
	msg.objectID     = reinterpret_cast<int>(this);  
	
	switch (monitorEvent) {
		
	case OraMonitor::meExecute: {
		
		int prmSize = mParams.size();
		
		vector<OraMonitor::MonitorParam> params(prmSize);
		
		if (prmSize != 0) {
			
			for (int i = 0; i < prmSize; i++) {
				params[i].name     = mParams[i].name();
				params[i].dataType = mParams[i].dataType();
				
				if (mParams[i].isTable()) {
					char buf[40];
					sprintf(buf, "PL/SQL Table with %d elements", mParams[i].tableLength());
					params[i].value = buf;
				}
				else if (params[i].dataType == dtLongRaw || params[i].dataType == dtLong) {
					char buf[40];
					sprintf(buf, "Large object %d size", mParams[i].getLob().size());
					params[i].value = buf;
				}
#ifdef OCL_OCI8
				else if (params[i].dataType == dtClob) {
					char buf[40];
					sprintf(buf, "Large object %d size", mParams[i].getClob().size());
					params[i].value = buf;
				}
				else if (params[i].dataType == dtBlob) {
					char buf[40];
					sprintf(buf, "Large object %d size", mParams[i].getBlob().size());
					params[i].value = buf;
				}
#endif
				else if (params[i].dataType == dtCursor)
					params[i].value = "Unavailable";
				else
					params[i].value = mParams[i].getChars();
				
				params[i].type = mParams[i].direction();
			}
			
		}
		
		ds_ocl_sql_monitor->onExecute(msg, mSQL, rowsProcessed(), params);
		break;
								}
    case OraMonitor::mePrepare:
    case OraMonitor::meUnprepare:
		ds_ocl_sql_monitor->onPrepare(msg, mSQL);
		break;
	}
	
}

#ifdef OCL_OCI90

OraTimeStamp::OraTimeStamp() {
	
	OraEnvironment::checkOCI90();
	
	hOCIDateTime = 0;
	mType        = 0;
}

OraTimeStamp::OraTimeStamp(OraDataType type) {
	
	OraEnvironment::checkOCI90();
	
	hOCIDateTime = 0;
	mType        = 0;
	mIsNull      = true;
	
	setTimeStampType(type);
}

OraTimeStamp::OraTimeStamp(const OraTimeStamp& src) {
	
	OraEnvironment::checkOCI90();
	
	hOCIDateTime = 0;
	mType        = 0;
	
	if (src.hOCIDateTime) {
		setTimeStampType(src.timeStampType());
		
		check(OCIDateTimeAssign(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), src.getOCIDateTime(), hOCIDateTime));
		mIsNull = false;
	}
}

OraTimeStamp::OraTimeStamp(short year, unsigned char month, unsigned char day,
						   unsigned char hour, unsigned char min, unsigned char sec, unsigned fsec,
						   const CRString& timeZone /* = "" */)
{
	OraEnvironment::checkOCI90();
	
	hOCIDateTime = 0;
	mType        = 0;
	
	setTimeStampType(dtTimeStampTZ);
	
	OraText* tz = (OraText*)(timeZone.empty() ? 0 : timeZone.c_str());
	
	check(OCIDateTimeConstruct(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, year, month, day,
		hour, min, sec, fsec, tz, timeZone.length()));
	
	// Validate timestamp
	checkValid();
	
	mIsNull = false;
}

OraTimeStamp& OraTimeStamp::operator=(const OraTimeStamp& r) {
	
	// Check for assignment to self
	if ((hOCIDateTime != r.getOCIDateTime()) && r.hOCIDateTime) {
		setTimeStampType(r.timeStampType());
		
		check(OCIDateTimeAssign(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), r.getOCIDateTime(), hOCIDateTime));
		
		mIsNull = false;
	}
	
	return *this;
}

OraTimeStamp::~OraTimeStamp() {
	
	free();
}

void OraTimeStamp::alloc() {
	
	assert(mType != 0); // Type must be specified befor alloc
	
	if (!hOCIDateTime)
		check(OCIDescriptorAlloc(OraEnvironment::getOCIEnv(),
		(void**)&hOCIDateTime, mType, 0, 0));
}

void OraTimeStamp::free() {
	
	if (hOCIDateTime) {
		check(OCIDescriptorFree(hOCIDateTime, mType));
		hOCIDateTime = 0;
		mType        = 0;
		mIsNull      = true;
	}
}

OCIDateTime* OraTimeStamp::getOCIDateTime() const {
	
	return hOCIDateTime;
}

OCIDateTime** OraTimeStamp::getOCIDateTimePtr() {
	
	return &hOCIDateTime;
}

OraDataType OraTimeStamp::timeStampType() const {
	
	OraDataType timeStampType;
	switch (mType) {
    case OCI_DTYPE_TIMESTAMP:
		timeStampType = dtTimeStamp;
		break;
    case OCI_DTYPE_TIMESTAMP_TZ:
		timeStampType = dtTimeStampTZ;
		break;
    case OCI_DTYPE_TIMESTAMP_LTZ:
		timeStampType = dtTimeStampLTZ;
		break;
    default:
		timeStampType = dtUnknown;
	}
	
	return timeStampType;
}

void OraTimeStamp::setTimeStampType(OraDataType type) {
	
	unsigned newType;
	switch (type) {
    case dtTimeStamp:
		newType = OCI_DTYPE_TIMESTAMP;
		break;
    case dtTimeStampTZ:
		newType = OCI_DTYPE_TIMESTAMP_TZ;
		break;
    case dtTimeStampLTZ:
		newType = OCI_DTYPE_TIMESTAMP_LTZ;
		break;
    default:
		throw CRException(InvalidDatatype);
	}
	
	if (newType != mType) {
		// If type was specified clear timestamp data
		if (mType)
			free();
		
		mType = newType;
		
		if (!hOCIDateTime)
			alloc();
	}
}

void OraTimeStamp::getDate(short& year, unsigned char& month, unsigned char& day) const {
	
	checkAlloc();
	
	check(OCIDateTimeGetDate(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, &year, &month, &day));
}

void OraTimeStamp::setDate(short year, unsigned char month, unsigned char day) {
	
	checkAlloc();
	
	unsigned char hour, min, sec;
	unsigned fsec;
	getTime(hour, min, sec, fsec);
	
	CRString timeZone;
	OraText* tz = 0;
	
	if (mType != OCI_DTYPE_TIMESTAMP) {
		// supress exception raised when timezone absent
		try {
			timeZone = timeZoneName();
			tz = (OraText*)(timeZone.empty() ? 0 : timeZone.c_str());
		}
		catch (OraException) {
			;
		}
	}
	
	check(OCIDateTimeConstruct(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, year, month, day,
		hour, min, sec, fsec, tz, timeZone.length()));
	
	// Validate timestamp
	checkValid();
	
	mIsNull = false;
}

void OraTimeStamp::getTime(unsigned char& hour, unsigned char& min,
						   unsigned char& sec, unsigned& fsec) const
{
	checkAlloc();
	
	check(OCIDateTimeGetTime(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, &hour, &min, &sec, &fsec));
}

void OraTimeStamp::setTime(unsigned char hour, unsigned char min,
						   unsigned char sec, unsigned fsec)
{
	checkAlloc();
	
	short year;
	unsigned char month, day;
	getDate(year, month, day);
	
	CRString timeZone;
	OraText* tz = 0;
	
	if (mType != OCI_DTYPE_TIMESTAMP) {
		timeZone = timeZoneName();
		tz = (OraText*)(timeZone.empty() ? 0 : timeZone.c_str());
	}
	
	check(OCIDateTimeConstruct(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, year, month, day,
		hour, min, sec, fsec, tz, timeZone.length()));
	
	// Validate timestamp
	checkValid();
	
	mIsNull = false;
}

void OraTimeStamp::timeZoneOffset(signed char& hour, signed char& min) const {
	
	checkAlloc();
	
	check(OCIDateTimeGetTimeZoneOffset(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, &hour, &min));
}

void OraTimeStamp::setTimeZoneOffset(signed char hour, signed char min) {
	
	checkAlloc();
	
	short year;
	unsigned char month, day;
	getDate(year, month, day);
	
	unsigned char lhour, lmin, sec;
	unsigned fsec;
	getTime(lhour, lmin, sec, fsec);
	
	CRString timeZone;
	char buf[5];
	
	sprintf(buf, "%i", (int)hour);
	timeZone += buf;
	timeZone += ':';
	
	sprintf(buf, "%i", (int)min);
	timeZone += buf;
	
	OraText* tz = (OraText*)(timeZone.empty() ? 0 : timeZone.c_str());
	
	check(OCIDateTimeConstruct(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, year, month, day,
		lhour, lmin, sec, fsec, tz, timeZone.length()));
	
	checkValid();
	
	mIsNull = false;
}

CRString OraTimeStamp::timeZoneName() const {
	
	checkAlloc();
	
	OraText buffer[256];
	size_t bufferSize = sizeof(buffer);
	
	check(OCIDateTimeGetTimeZoneName(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, buffer, &bufferSize));
	
	return (const char*)buffer;
}

CRString OraTimeStamp::toString(const CRString& format /* = "" */,
								unsigned char fsprecision /* = 6 */) const
{
	checkAlloc();
	
	// If TimeStamp not inited return empty string
	if (!hOCIDateTime)
		return CRString();
	
	OraText* fmt = (OraText*)(format.empty() ? 0 : format.c_str());
	
	OraText buffer[256];
	unsigned bufferSize = sizeof(buffer);
	
	// NLS not used
	check(OCIDateTimeToText(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, fmt, format.length(),
		fsprecision, 0, 0, &bufferSize, buffer));
	
	return (const char*)buffer;
}

void OraTimeStamp::fromString(const CRString& value, const CRString& format /* = "" */) {
	
	checkAlloc();
	
	OraText* fmt = (OraText*)(format.empty() ? 0 : format.c_str());
	
	check(OCIDateTimeFromText(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), (OraText*)value.c_str(), value.length(),
		fmt, format.length(), 0, 0, hOCIDateTime));
	
	checkValid();
	
	mIsNull = false;
}

void OraTimeStamp::setSysTimeStamp() {
	
	checkAlloc();
	
	check(OCIDateTimeSysTimeStamp(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime));
	
	mIsNull = false;
}

bool OraTimeStamp::isNull() const {
	
	return mIsNull;
}

void OraTimeStamp::setNull() {
	
	mIsNull = true;
}

int OraTimeStamp::compare(const OraTimeStamp& value) const {
	
	checkAlloc();
	value.checkAlloc();
	
	sword result;
	check(OCIDateTimeCompare(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, value.hOCIDateTime, &result));
	
	return result;
}

void OraTimeStamp::checkValid() const {
	
	assert(hOCIDateTime != 0); // Descriptor must be already allocated
	
	unsigned checkResult = 0;
	check(OCIDateTimeCheck(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIDateTime, &checkResult));
	
	if (checkResult != 0)
		throw CRException(InvalidTimeStampField);
}

void OraTimeStamp::checkAlloc() const {
	
	if (!hOCIDateTime)
		throw CRException(TimeStampTypeNotSpec);
}

OraTimeStamp& OraTimeStamp::operator+(const OraInterval& value) {
	
	if (value.hOCIInterval && hOCIDateTime) {
		check(OCIDateTimeIntervalAdd(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), hOCIDateTime, value.hOCIInterval, hOCIDateTime));
		
		mIsNull = false;
	}
	
	return *this;
}

OraTimeStamp& OraTimeStamp::operator-(const OraInterval& value) {
	
	if (value.hOCIInterval && hOCIDateTime) {
		check(OCIDateTimeIntervalSub(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), hOCIDateTime, value.hOCIInterval, hOCIDateTime));
		
		mIsNull = false;
	}
	
	return *this;
}

OraInterval operator-(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	l.checkAlloc();
	
	OraInterval interval;
	interval.setIntervalType(dtIntervalYM);
	
	if (r.hOCIDateTime) {
		check(OCIDateTimeSubtract(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), r.hOCIDateTime, l.hOCIDateTime, interval.hOCIInterval));
		
		interval.mIsNull = false;
	}
	
	return interval;
}

bool operator==(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) == 0;
}

bool operator>(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) == 1;
}

bool operator<(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) == -1;
}

bool operator!=(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) != 0;
}

bool operator>=(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) == 0 || l.compare(r) == 1;
}

bool operator<=(const OraTimeStamp &l, const OraTimeStamp &r) {
	
	return l.compare(r) == 0 || l.compare(r) == -1;
}

OraInterval::OraInterval() {
	
	hOCIInterval = 0;
	mType        = 0;
	mIsNull      = true;
}

OraInterval::OraInterval(OraDataType type) {
	
	hOCIInterval = 0;
	mType        = 0;
	mIsNull      = true;
	
	setIntervalType(type);
}

OraInterval::OraInterval(int year, int month) {
	
	hOCIInterval = 0;
	mType        = 0;
	
	setIntervalType(dtIntervalYM);
	
	check(OCIIntervalSetYearMonth(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), year, month, hOCIInterval));
	
	checkValid();
	
	mIsNull = false;
}

OraInterval::OraInterval(int day, int hour, int minute, int second, int fsecond) {
	
	hOCIInterval = 0;
	mType        = 0;
	
	setIntervalType(dtIntervalDS);
	
	check(OCIIntervalSetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), day, hour, minute, second, fsecond, hOCIInterval));
	
	checkValid();
	
	mIsNull = false;
}

OraInterval::OraInterval(const OraInterval& src) {
	
	hOCIInterval = 0;
	mType        = 0;
	
	if (src.hOCIInterval) {
		setIntervalType(src.intervalType());
		
		check(OCIIntervalAssign(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), src.hOCIInterval, hOCIInterval));
		
		mIsNull = false;
	}
}

OraInterval& OraInterval::operator=(const OraInterval& r) {
	
	if ((hOCIInterval != r.hOCIInterval) && r.hOCIInterval) {
		setIntervalType(r.intervalType());
		
		check(OCIIntervalAssign(OraEnvironment::getOCIEnv(),
			OraEnvironment::getOCIError(), r.hOCIInterval, hOCIInterval));
		
		mIsNull = false;
	}
	
	return *this;
}

OraInterval::~OraInterval() {
	
	free();
}

void OraInterval::alloc() {
	
	assert(mType != 0); // Type must be specified befor alloc
	
	if (!hOCIInterval)
		check(OCIDescriptorAlloc(OraEnvironment::getOCIEnv(),
		(void**)&hOCIInterval, mType, 0, 0));
}

void OraInterval::free() {
	
	if (hOCIInterval) {
		check(OCIDescriptorFree(hOCIInterval, mType));
		hOCIInterval = 0;
		mType        = 0;
		mIsNull      = true;
	}
}

OCIInterval* OraInterval::getOCIInterval() const {
	
	return hOCIInterval;
}

OCIInterval** OraInterval::getOCIIntervalPtr() {
	
	return &hOCIInterval;
}

OraDataType OraInterval::intervalType() const {
	
	OraDataType intervalType;
	switch (mType) {
    case OCI_DTYPE_INTERVAL_YM:
		intervalType = dtIntervalYM;
		break;
    case OCI_DTYPE_INTERVAL_DS:
		intervalType = dtIntervalDS;
		break;
    default:
		intervalType = dtUnknown;
	}
	
	return intervalType;
}

void OraInterval::setIntervalType(OraDataType type) {
	
	unsigned newType;
	switch (type) {
    case dtIntervalYM:
		newType = OCI_DTYPE_INTERVAL_YM;
		break;
    case dtIntervalDS:
		newType = OCI_DTYPE_INTERVAL_DS;
		break;
    default:
		throw CRException(InvalidDatatype);
	}
	
	if (newType != mType) {
		// If type was specified clear timestamp data
		if (mType)
			free();
		
		mType = newType;
		
		if (!hOCIInterval)
			alloc();
	}
}

void OraInterval::setYearMonth(int year, int month) {
	
	checkAlloc();
	
	check(OCIIntervalSetYearMonth(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), year, month, hOCIInterval));
	
	checkValid();
	
	mIsNull = false;
}

void OraInterval::setDaySecond(int day, int hour, int minute,
							   int second, int fsecond)
{
	checkAlloc();
	
	check(OCIIntervalSetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), day, hour, minute, second, fsecond, hOCIInterval));
	
	checkValid();
	
	mIsNull = false;
}

int OraInterval::year() const {
	
	checkAlloc();
	
	int year, month;
	
	if (mType == OCI_DTYPE_INTERVAL_YM)
		check(OCIIntervalGetYearMonth(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &year, &month, hOCIInterval));
	else
		year = 0;
	
	return year;
}

int OraInterval::month() const {
	
	checkAlloc();
	
	int year, month;
	
	if (mType == OCI_DTYPE_INTERVAL_YM)
		check(OCIIntervalGetYearMonth(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &year, &month, hOCIInterval));
	else
		month = 0;
	
	return month;
}

int OraInterval::day() const {
	
	checkAlloc();
	
	int day, hour, minute, second, fsecond;
	check(OCIIntervalGetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &day, &hour, &minute, &second, &fsecond, hOCIInterval));
	
	return day;
}

int OraInterval::hour() const {
	
	checkAlloc();
	
	int day, hour, minute, second, fsecond;
	check(OCIIntervalGetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &day, &hour, &minute, &second, &fsecond, hOCIInterval));
	
	return hour;
}

int OraInterval::minute() const {
	
	checkAlloc();
	
	int day, hour, minute, second, fsecond;
	check(OCIIntervalGetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &day, &hour, &minute, &second, &fsecond, hOCIInterval));
	
	return minute;
}

int OraInterval::second() const {
	
	checkAlloc();
	
	int day, hour, minute, second, fsecond;
	check(OCIIntervalGetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &day, &hour, &minute, &second, &fsecond, hOCIInterval));
	
	return second;
}

int OraInterval::fracSecond() const {
	
	checkAlloc();
	
	int day, hour, minute, second, fsecond;
	check(OCIIntervalGetDaySecond(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), &day, &hour, &minute, &second, &fsecond, hOCIInterval));
	
	return fsecond;
}

CRString OraInterval::toString(unsigned char lfprec /* = 0 */,
							   unsigned char fsprec /* = 6 */) const
{
	checkAlloc();
	
	OraText buffer[256];
	size_t bufferSize = sizeof(buffer);
	
	check(OCIIntervalToText(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIInterval, lfprec, fsprec, buffer,
		bufferSize, &bufferSize));
	
	return (const char*)buffer;
}

void OraInterval::fromString(const CRString& val) {
	
	checkAlloc();
	
	check(OCIIntervalFromText(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), (OraText*)val.c_str(), val.length(), hOCIInterval));
	
	checkValid();
	
	mIsNull = false;
}

bool OraInterval::isNull() const {
	
	return mIsNull;
}

void OraInterval::setNull() {
	
	mIsNull = true;
}

int OraInterval::compare(const OraInterval& value) const {
	
	checkAlloc();
	value.checkAlloc();
	
	sword result;
	check(OCIIntervalCompare(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIInterval, value.hOCIInterval, &result));
	
	return result;
}

void OraInterval::checkValid() const {
	
	assert(hOCIInterval != 0); // Descriptor must be already allocated
	
	unsigned checkResult = 0;
	check(OCIIntervalCheck(OraEnvironment::getOCIEnv(),
		OraEnvironment::getOCIError(), hOCIInterval, &checkResult));
	
	if (checkResult != 0)
		throw CRException(InvalidIntervalField);
}

void OraInterval::checkAlloc() const {
	
	if (!hOCIInterval)
		throw CRException(IntervalTypeNotSpec);
}


OraInterval& OraInterval::operator+=(const OraInterval& r) {
	
	checkAlloc();
	
	if (r.hOCIInterval)
		check(OCIIntervalAdd(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(),
		hOCIInterval, r.hOCIInterval, hOCIInterval));
	
	checkValid();
	
	return *this;
}

OraInterval& OraInterval::operator-=(const OraInterval& r) {
	
	checkAlloc();
	
	if (r.hOCIInterval)
		check(OCIIntervalSubtract(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(),
		hOCIInterval, r.hOCIInterval, hOCIInterval));
	
	checkValid();
	
	return *this;
}

OraInterval operator+(const OraInterval& l, const OraInterval& r) {
	
	l.checkAlloc();
	r.checkAlloc();
	
	OraInterval interval(l.intervalType());
	
	check(OCIIntervalAdd(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(),
		l.hOCIInterval, r.hOCIInterval, interval.hOCIInterval));
	
	interval.checkValid();
	
	return interval;
}

OraInterval operator-(const OraInterval& l, const OraInterval& r) {
	
	l.checkAlloc();
	r.checkAlloc();
	
	OraInterval interval(l.intervalType());
	
	check(OCIIntervalSubtract(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(),
		l.hOCIInterval, r.hOCIInterval, interval.hOCIInterval));
	
	interval.checkValid();
	
	return interval;
}

bool operator==(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) == 0;
}

bool operator>(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) == 1;
}

bool operator<(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) == -1;
}

bool operator!=(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) != 0;
}

bool operator>=(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) == 1 || l.compare(r) == 0;;
}

bool operator<=(const OraInterval& l, const OraInterval& r) {
	
	return l.compare(r) == -1 || l.compare(r) == 0;;
}

#endif // OCL_OCI90

#ifdef OCL_OCI8 //LOB types are specific for Oracle 8

OraLob::OraLob() {
	hOCISvcCtx     = 0;
	hOCILobLocator = 0;
}

OraLob::OraLob(OraConnection& connection) : CRLob() {
	hOCISvcCtx     = connection.getOCISvcCtx();
	hOCILobLocator = 0;
}

OraLob::~OraLob() {
	freeLob();
}

void OraLob::allocLob() {
	if (hOCILobLocator)
		return;
	
	if (!OraEnvironment::isOCIInited())
		OraEnvironment::initOCI();
	
	check(OCIDescriptorAlloc(OraEnvironment::getOCIEnv(), (dvoid **)&hOCILobLocator, OCI_DTYPE_LOB, 0, 0));
	
	init();
}

void OraLob::freeLob() {
	if (hOCILobLocator) {
		check(OCIDescriptorFree(hOCILobLocator, OCI_DTYPE_LOB));
		hOCILobLocator = 0;
	}
}

void OraLob::enableBuffering() {
	checkAlloc();
	checkSession();
	
	check(OCILobEnableBuffering(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator));
}

void OraLob::disableBuffering() {
	checkAlloc();
	checkSession();
	
	check(OCILobDisableBuffering(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator));
}

void OraLob::init() {
	
	check(OCIAttrSet(hOCILobLocator, OCI_DTYPE_LOB, (ub4 *)0, 0, OCI_ATTR_LOBEMPTY, OraEnvironment::getOCIError()));
	
}

bool OraLob::isInited() {
	boolean isInited = 0;
	
	check(OCILobLocatorIsInit(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(), hOCILobLocator, (boolean* )&isInited));
	
	return isInited != 0;
}

unsigned int OraLob::length() {
	ub4 length = 0;
	
	checkAlloc();
	checkSession();
	
	check(OCILobGetLength(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, &length));
	
	return length;
}

void OraLob::readLob() {
	
	PieceHeader *piece = 0;
	ub4 remainder = 0, offset = 0, bufLen = 0;
	
	checkAlloc();
	checkSession();
	
	clear();
	
	if (!isInited())
		return;
	
	remainder = length();
	offset = 1;
	
	while (remainder > 0) {
		
		if (remainder > (unsigned)mDefPieceSize)
			bufLen = mDefPieceSize;
		else
			bufLen = remainder;
		
		piece = allocPiece(bufLen);
		
		
		try {
			check(OCILobRead(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator,
				&bufLen, offset, (void*)((char*)piece + sizeof(PieceHeader)), bufLen, 0, 0, 0, 0));
		}
		catch(OraException &e)
		{
			freePiece(piece);
			throw e;
			
		}
		catch(...) {
			freePiece(piece);
			
			throw;
		}
		
		piece->used = bufLen;
		
		appendPiece(piece);
		
		remainder -= bufLen;
		offset += bufLen;
	}
}

void OraLob::writeLob() {
	
	PieceHeader *piece;
	ub4 bufLen, offset;
	
	checkAlloc();
	checkSession();
	checkInit();
	
	check(OCILobTrim(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, 0));
	
	piece = mFirstPiece;
	offset = 1;
	
	while (piece) {
		
		bufLen = piece->used;
		
		check(OCILobWrite(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, &bufLen, offset,
			(void*)((char*)piece + sizeof(PieceHeader)), bufLen, OCI_ONE_PIECE, 0, 0, 0, 0));
		
		piece = piece->next;
		
		offset += bufLen;
	}
}

OCISvcCtx* OraLob::getOCISvcCtx() const {
	
	return hOCISvcCtx;
}

void OraLob::setOCISvcCtx(OCISvcCtx *hSvcCtx) {
	hOCISvcCtx = hSvcCtx;
}

OCILobLocator* OraLob::getOCILobLocator() const {
	
	return hOCILobLocator;
}

void OraLob::checkAlloc() {
	
	if (!hOCILobLocator)
		throwError(SLOBNotAllocated);
}

void OraLob::checkInit() {
	
	if (!isInited())
		throwError(SLOBNotInited);
}


void OraLob::checkSession() {
	if (!hOCISvcCtx)
		throwError(SLOBSvcCtxNotInit);
}

OraFile::OraFile() {
}

OraFile::OraFile(OraConnection& connection) : OraLob(connection) {
	
}

OraFile::~OraFile() {
	freeLob();
}

void OraFile::allocLob() {
	if (hOCILobLocator)
		return;
	
	if (!OraEnvironment::isOCIInited())
		OraEnvironment::initOCI();
	
	check(OCIDescriptorAlloc(OraEnvironment::getOCIEnv(), (dvoid **)&hOCILobLocator, OCI_DTYPE_FILE, 0, 0));
}

void OraFile::freeLob() {
	if (hOCILobLocator) {
		check(OCIDescriptorFree(hOCILobLocator, OCI_DTYPE_FILE));
		hOCILobLocator = 0;
	}
}

void OraFile::open() {
	checkAlloc();
	checkSession();
	
	if (isInited())
		// 1 == OCI_FILE_READ_ONLY only on oci8i exist
		check(OCILobFileOpen(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, 1));
}

void OraFile::close() {
	checkAlloc();
	checkSession();
	
	if (isInited())
		check(OCILobFileClose(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator));
}

void OraFile::refresh() {
	checkAlloc();
	checkSession();
	
	clear();
	
	if (exist()) {
		open();
		
		try {
			readLob();
		}
		catch(OraException &e)
		{
			close();
			throw e;
		}
		catch(...) {
			close();
			
			throw;
		}
	}
}

bool OraFile::isOpen() {
	bool res = false;
	
	if (hOCILobLocator && isInited()) {
		checkSession();
		
		check(OCILobFileIsOpen(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, (boolean *)&res));
	}
	
	return res != 0;
}

bool OraFile::exist() {
	
	bool val = false;
	
	if (hOCILobLocator && isInited()) {
		checkSession();
		
		if (mFileName.empty())
			return false;
		
		check(OCILobFileExists(hOCISvcCtx, OraEnvironment::getOCIError(), hOCILobLocator, (boolean *)&val));
	}
	
	return val != 0;
}

CRString OraFile::fileDir() const {
	return mFileDir;
}

void OraFile::setFileDir(const CRString& value) {
	checkAlloc();
	
	if (mFileDir != value) {
		
		mFileDir = value;
		
		int dirLength = 1;
		
		if (!value.empty())
			dirLength = value.length();
		
		int fileNameLength = 1;
		
		if (!mFileName.empty())
			fileNameLength = mFileName.length();
		
		check(OCILobFileSetName(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(), &hOCILobLocator, (OraText*)mFileDir.c_str(),
			dirLength, (OraText*)mFileName.c_str(), fileNameLength));
	}
}

CRString OraFile::fileName() const {
	return mFileName;
}

void OraFile::setFileName(const CRString& value) {
	checkAlloc();
	
	if (mFileName != value) {
		
		mFileName = value;
		
		int fileNameLength = 1;
		
		if (!mFileName.empty())
			fileNameLength = mFileName.length();
		
		int dirLength = 1;
		
		if (!mFileDir.empty())
			dirLength = mFileDir.length();
		
		check(OCILobFileSetName(OraEnvironment::getOCIEnv(), OraEnvironment::getOCIError(), &hOCILobLocator, (OraText*)mFileDir.c_str(),
			dirLength, (OraText*)mFileName.c_str(), fileNameLength));    
	}
}

#endif // OCL_OCI8

/////////////////  OraField    /////////////////

OraField::OraField() {
	mHandle = 0;
	
	mIsUnique      = false;
	mIsKey         = false;
	mIsExpression  = false;  
}

OraField::~OraField() {
}

int OraField::size() {
	
	switch (mDataType) {
    case dtClob: case dtBlob: case dtBFile:
    case dtLong: case dtLongRaw:
		return 0;
    default:
		return CRField::size();
	}
}

bool OraField::isComplex() const {
	
	return mDataType == dtLongRaw      ||
		mDataType == dtLong         ||
		mDataType == dtBlob         ||
		mDataType == dtClob         ||
		mDataType == dtBFile        ||
		mDataType == dtCursor       ||
		mDataType == dtTimeStamp    ||
		mDataType == dtTimeStampTZ  ||
		mDataType == dtTimeStampLTZ ||
		mDataType == dtIntervalDS   ||
		mDataType == dtIntervalYM;
}

bool OraField::hasParent() const {
	return CRField::hasParent();
}

const char* OraField::getChars() {
	
#ifdef OCL_OCI8
	switch (mDataType) {
    case dtClob: case dtBlob:
		return "";
    default:
#endif
		return CRField::getChars();
#ifdef OCL_OCI8
	}
#endif
}

char* OraField::getChars(char* result) {
	
	switch (mDataType) {
    case dtClob: case dtBlob:
    case dtLong: case dtLongRaw:
		return getLob().getChars(result);
    default:
		return CRField::getChars(result);
	}
}

char* OraField::getChars(char* result, int length) {
	
	switch (mDataType) {
    case dtClob: case dtBlob:
    case dtLong: case dtLongRaw:
		return getLob().getChars(result);
    default:
		return CRField::getChars(result, length);
	}
}


CRString OraField::getString() {
	
	switch (mDataType) {
    case dtClob: case dtBlob:
    case dtLong: case dtLongRaw:      
		return getLob().getString();
#ifdef OCL_OCI90
    case dtTimeStamp:
    case dtTimeStampTZ:
    case dtTimeStampLTZ:
		return getTimeStamp().toString();
    case dtIntervalYM:
    case dtIntervalDS:
		return getInterval().toString();
#endif
    default:
		return CRField::getString();
	}
}

bool OraField::isLob() const {
	switch (mDataType) {
    case dtLong: case dtLongRaw:
		return true;
    default:
		return CRField::isLob();
	}
}

CRLob& OraField::getLob() {
	
#ifdef OCL_OCI8
	OraLob* result = 0;
#else                   // War C4700
	OraLong* result = 0;
#endif
	
	switch (mDataType) {
#ifdef OCL_OCI8
    case dtBlob: case dtClob:
		getData(&result);
		break;
#endif
    case dtLongRaw: case dtLong:
		getData(&result);
		break;
    default:
		throwError(SCannotConvert, "LOB");
	}
	
	return *result;
}

void OraField::setLob(const CRLob& value) {
	
	switch (mDataType) {
#ifdef OCL_OCI8
    case dtBlob: case dtClob:
		setData((void*)&value);
		break;
#endif
    case dtLongRaw: case dtLong:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "LOB");
	}
}

OraLong& OraField::getLong() {
	
	OraLong* result = 0; // War C4700
	
	switch (mDataType) {
    case dtLongRaw: case dtLong:
		getData(&result);
		break;
    default:
		throwError(SCannotConvert, "LONG");
	}
	
	return *result;
}

void OraField::setLong(OraLong& value) {
	
	switch (mDataType) {
    case dtLongRaw: case dtLong:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "LONG");
	}
}

unsigned char* OraField::getRaw(unsigned char* result) {
	return getBinary(result);
}

void OraField::setRaw(unsigned char* value, int length) {
	setBinary(value, length);
}

OraLong& OraField::getLongRaw() {
	
	return getLong();
}

void OraField::setLongRaw(OraLong& value) {
	
	setLong(value);
}

#ifdef OCL_OCI8

OraLob* OraField::getBlob() 
{//change the interface to OraLob& to *, since the object does not have the deep copy! 
	OraLob *lob = 0;// = new OraLob;
	
	if(!((OraRecordset*)mDataset)->validBuffer())
		throwError(SObjectNotExist);
	
	switch (mDataType) {
    case dtBlob: case dtClob: case dtBFile:
		getData(&lob);
		break;
    default:
		throwError(SCannotConvert, "BLOB");
	}
	
	return lob;
}

void OraField::setBlob(const OraLob& value) {
	
	switch (mDataType) {
    case dtBlob: case dtClob: case dtBFile:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "BLOB");
	}
}

OraLob* OraField::getClob() {
	
	return getBlob();
}

void OraField::setClob(const OraLob& value) {
    
	setBlob(value);
}

OraFile& OraField::getBFile() {
	OraFile* file = 0;
	
	if(!((OraRecordset*)mDataset)->validBuffer())
		throwError(SObjectNotExist);
	
	switch (mDataType) {
    case dtBFile:
		getData(&file);
		break;
    default:
		throwError(SCannotConvert, "BFILE");
	}
	
	return *file;
}

void OraField::setBFile(const OraFile& value) {
	
	switch (mDataType) {
    case dtBFile:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "BFILE");
	}
}

#endif // OCL_OCI8

#ifdef OCL_OCI90

const OraTimeStamp& OraField::getTimeStamp() {
	
	OraTimeStamp* timeStamp = 0;
	
	switch (mDataType) {
    case dtTimeStamp:
    case dtTimeStampTZ:
    case dtTimeStampLTZ:
		getData(&timeStamp);
		break;
    default:
		throwError(SCannotConvert, "TIMESTAMP");
	}
	
	return *timeStamp;
}

void OraField::setTimeStamp(const OraTimeStamp& value) {
	
	switch (mDataType) {
    case dtTimeStamp:
    case dtTimeStampTZ:
    case dtTimeStampLTZ:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "TIMESTAMP");
	}
}

const OraInterval& OraField::getInterval() {
	
	OraInterval* interval = 0;
	
	switch (mDataType) {
    case dtIntervalYM:
    case dtIntervalDS:
		getData(&interval);
		break;
    default:
		throwError(SCannotConvert, "INTERVAL");
	}
	
	return *interval;
}

void OraField::setInterval(const OraInterval& value) {
	
	switch (mDataType) {
    case dtIntervalYM:
    case dtIntervalDS:
		setData((void*)&value);
		break;
    default:
		throwError(SCannotConvert, "INTERVAL");
	}
}

#endif // OCL_OCI90


/////////////////  OraRecordset  /////////////////

OraRecordset::OraRecordset(OraCommand& command)
: CRMemDataset(), mCommand(command) {
	
	_init();
}

void OraRecordset::_init() {
	
	mFetchAll           = false;
	mFetchRows          = 25;
	mCached             = false;
	mPieceFetch         = false;
	mAutoClose          = false;
	mFetchBlock         = 0;
	mFetchBlockItemSize = 0;
	mFetchCursor        = 0;
	mFetchItem          = 0;
	mLongString         = true;
}

OraRecordset::~OraRecordset() {
	
	CRDataset::close();
	mCommand.mRecordset = 0;
}

// Items / data

void OraRecordset::prepareData() {
	
	CRMemDataset::prepareData();
	
	if (rowsReturn())
		checkRowsReturn();
	
	mEOF = false;
	
	execFetch();
}

// Open / close

void OraRecordset::open() {
	
	CRDataset::open();
}

void OraRecordset::internalOpen() {
	
#ifdef OCL_OCI7
	mPieceFetch = false;
	
	if (mCommand.OCICallStyle() == OCI73) {
		for (int i = 0; i < mFields.count(); i++)
			if (mFields[i].dataType() == dtLongRaw || mFields[i].dataType() == dtLong) {
				mPieceFetch = true;
				break;
			}
	}
#endif //OCL_OCI7
}

void OraRecordset::internalClose() {
	
	if (!mCommand.nativeCursor()) {
		if (mCommand.cursorState() != csInactive)
			mCommand.unprepare();
		mCommand.setCursor(0);
		mFetchCursor->freeCursor(); // When RefCount = 1
	}
	
	if (mCommand.cursorState() > csPrepared)
		mCommand.setCursorState(csPrepared);
	
	if (mCommand.mImplicitPrepared)
		mCommand.unprepare();
	
	if (mFetchBlock) {
		free(mFetchBlock);
		mFetchBlock = 0;
	}
}

void OraRecordset::internalRefresh() {
	
	if (mCommand.cursorState() == csInactive)
		open();
	else
		if (mCommand.nativeCursor()) {
			mCommand.bindParams();  // for reallocated params
			
			freeData();
			initData();
			prepareData();
		}
		else {
			if (mCommand.SQLType() == SQL_UNKNOWN)
				DBError(SRefreshNotAllowed);
			
			close();
			open();
		};
}

bool OraRecordset::rowsReturn() {
	
	mCommand.checkActive();
	
	return (mCommand.SQLType() == SQL_SELECT || mCommand.mCursorRef != mCommand.mCursor &&
		mCommand.mCursorRef->state() >= csExecuted ||
		mCommand.SQLType() == SQL_PLSQL && mFetchCursor);
}

void OraRecordset::checkRowsReturn() {
	
	if (!rowsReturn())
		DBError(SNotRows);
}

// Fields

void OraRecordset::getFieldsInfo() {
	ColumnInfo columnsInfo[300];
	TableInfo  tablesInfo[30];
	int i;
	OraRecordset* rs;
	
	int columnsCount = getColumnsInfo(mCommand.SQL(), columnsInfo);
	int tableCount = getTablesInfo(mCommand.SQL(), tablesInfo);
	
	// when parser failed
	if (columnsCount == 0)
		return;
	
	OraCommand cmd(*mCommand.connection());
	
	for (i = 0; i < tableCount; i++) {
		if (tablesInfo[i].alias.empty())
			tablesInfo[i].alias = tablesInfo[i].name;
	}
	
	CRString defaultTable;
	
	if (tableCount > 0)
		defaultTable = tablesInfo[0].name;
	
	bool needAddInfo = false;
	int asteriskCount = 0;
	ColumnInfo columnInfo;
	std::vector<ColumnInfo> columns(columnsCount);
	
	for (i = 0; i < columnsCount; i++) {
		columnInfo.name = columnsInfo[i].name;
		columnInfo.table = columnsInfo[i].table;
		
		if (!columnInfo.table.empty())
			for (int j = 0; j < tableCount; j++)
				if (columnInfo.table == tablesInfo[j].alias) {
					columnInfo.table = tablesInfo[j].name;
					break;
				}
				
				columnInfo.expr = columnsInfo[i].expr;
				columnInfo.alias = columnsInfo[i].alias;
				
				if (columnInfo.name == "*") {
					asteriskCount++;
					if (!columnInfo.table.empty())
						defaultTable = columnInfo.table;
					else
						needAddInfo = true;
				}
				else {
					if (columnInfo.alias.empty())
						if (!columnInfo.name.empty())
							columnInfo.alias = columnInfo.name;
						else
							columnInfo.alias = columnInfo.expr;
						
						if (!columnInfo.name.empty() && columnInfo.table.empty())
							needAddInfo = true;
				}
				
				columns[i] = columnInfo;
	}
	
	if (needAddInfo || asteriskCount) {
		CRString querySql, name, table, owner;
		
		for (i = 0; i < tableCount; i++) {
			table = tablesInfo[i].name;
			
			CRString::size_type j = table.find('.');
			
			if (j != CRString::npos) {
				name = table.substr(j + 1);
				owner = table.substr(0, table.size() - name.size() - 1);
				table = name;
			}
			else
				owner = mCommand.connection()->username().toUpper();
			
			if (i > 0)
				querySql += "\n  UNION (\n";
			
			char ibuf[10];
			sprintf(ibuf, "%i", i + 1);
			
			querySql += "SELECT table_name, column_name, " + CRString(ibuf) + " table_id, column_id\nFROM "
				"all_tab_columns\nWHERE owner = '" + owner + "' and table_name = '"
				+ table + "'";
			
			if (i > 0)
				querySql += "\n)\n";
		}
		
		if (tableCount > 1)
			querySql = "SELECT table_name, column_name, table_id\nFROM\n(\n" + querySql +
			"\n)\nORDER BY table_id, column_id";
		
		cmd.setCommandText(querySql);
		cmd.execute(&rs);
		
		// if table or view not exist in current schema
		// check for public synonim
		if (rs->isEOF()) {
			rs->close();
			CRString synonimSQL = "SELECT table_owner\n"
				"FROM all_synonyms WHERE synonym_name = '" + table + "' AND owner = 'PUBLIC'";
			cmd.setCommandText(synonimSQL);
			cmd.execute(&rs);
			if (!rs->isEOF()) {
				CRString actualOwner = rs->field("table_owner").getString();
				querySql.replace(querySql.find(owner), owner.length(), actualOwner);
				rs->close();
				cmd.setCommandText(querySql);
				cmd.execute(&rs);
			}
			else
				return; // Throw Exception 'object dose not exist'
		}
		
		i = 0;
		
		if (tableCount > 1 && columns.size() == 1 && columns[0].name == "*") {
			for (i = 1; i < tableCount; i++) {
				columnInfo.name = "*";
				columns.push_back(columnInfo);
			}
		}
		
		int i = 0;
		std::vector<ColumnInfo>::iterator iter = columns.begin();
		std::vector<ColumnInfo> tempColumns;
		
		int tableId = 0;
		rs->first();
		while (i < (signed int)columns.size()) {
			columnInfo = columns[i];
			iter = columns.begin();
			
			if (columnInfo.name == "*") {
				table = columnInfo.table;
				iter = columns.erase(iter + i);
				//rs->first();
				while (!rs->isEOF()) {
					if ((table.empty() || rs->field(0).getString() == table) &&
						(tableId == 0  || rs->field(2).getInt()    == tableId))
					{
						columnInfo.table = rs->field(0).getString();
						columnInfo.name  = rs->field(1).getString();
						int colCount = 0;
						for (unsigned k = 0; k < columns.size(); k++)
							if (columnInfo.name == columns[k].name)
								colCount++;
							if (colCount != 0) {
								char t[20];
								sprintf(t, "_%i", colCount);
								columnInfo.alias = columnInfo.name + CRString(t);
							}
							else
								columnInfo.alias = columnInfo.name;
							tableId = rs->field(2).getInt();
							columnInfo.schema = owner;
							tempColumns.push_back(columnInfo);
							i++;
					}
					else {
						tableId = rs->field(2).getInt();
						break;
					}
					rs->next();
				}
				columns.insert(iter, tempColumns.begin(), tempColumns.end());
				tempColumns.clear();
			}
			else {
				if (columnInfo.name.empty() == false && columnInfo.table.empty() == true) {
					bool isFound = false;
					rs->first();
					while (!rs->isEOF()) {
						if (rs->field(1).getString() == columnInfo.name) {
							columns[i].table = rs->field(0).getString();
							columns[i].schema = owner;
							isFound = true;
							break;
						}
						rs->next();
					}
					// For pseudo-columns, for example ROWID
					if (!isFound) {
						// If default table consist of schema
						CRString::size_type j = defaultTable.find('.');
						
						if (j != CRString::npos) {
							columns[i].table = defaultTable.substr(j + 1);
							columns[i].schema = defaultTable.substr(0, defaultTable.size() - columns[i].table.size() - 1);
						}
						else
							columns[i].table = defaultTable;
					}
				}
				i++;
			}
		}
		
		rs->close();
  }
  
  char querySQL1[] =  "SELECT cc.column_name Name,\n"
	  "  DECODE(c.constraint_type, 'P', 1, 0) Is_Key,\n"
	  "  DECODE(c.constraint_type, 'U', 1, 0) Is_Unique\n"
	  "FROM  all_cons_columns cc, all_constraints c\n"
	  "WHERE cc.constraint_name = c.constraint_name\n"
	  "  AND\n"
	  "  (c.table_name = '";
  
  char querySQL2[] =  "  )\n"
	  "  AND\n"
	  "  (c.constraint_type = 'P' OR c.constraint_type = 'U')\n";
  
  CRString sql(querySQL1);
  
  for (i = 0; i < tableCount; i++) {
	  
	  CRString table = tablesInfo[i].name;
	  
	  CRString::size_type j = table.find('.');
	  
	  if (j != CRString::npos)
		  table = table.substr(j + 1);
	  else
		  table = tablesInfo[i].name;
	  
	  if (i > 0)
		  sql += "  OR\n"
		  "  c.table_name = '" + table + "'";
	  else
		  sql += table + "'";
  }
  
  sql += querySQL2;
  
  cmd.setCommandText(sql);
  cmd.execute(&rs);
  
  while (!rs->isEOF()) {
	  for (unsigned k = 0; k < columns.size(); k++)
		  if (rs->field(0).getString() == columns[k].name) {
			  OraField* ffield = findField(columns[k].alias);
			  if (ffield) {
				  if (rs->field(1).getInt() == 1)
					  field(columns[k].alias).mIsKey = true;
				  else
					  field(columns[k].alias).mIsKey = false;
				  
				  if (rs->field(2).getInt() == 1)
					  field(columns[k].alias).mIsUnique = true;
				  else
					  field(columns[k].alias).mIsUnique = false;
			  }
		  }
		  rs->next();
  }
  
  rs->close();
  
  for (i = 0; i < (int)columns.size(); i++) {
	  CRString alias;
	  // If alias is quoted
	  if (columns[i].alias[0] == '\"')
		  alias = columns[i].alias.substr(1, columns[i].alias.size() - 2);
	  else
		  alias = columns[i].alias;
	  field(alias).mOriginalName = columns[i].name;
	  field(alias).mTableName = columns[i].table;
	  
	  if (columns[i].schema.empty())
		  field(alias).mSchemaName = mCommand.connection()->username().toUpper();
	  else
		  field(alias).mSchemaName = columns[i].schema;
	  
	  if (columns[i].expr.empty())
		  field(alias).mIsExpression = false;
	  else {
		  field(alias).mIsExpression = true;
		  field(alias).mExpression = columns[i].expr;
	  }
  }
}

void OraRecordset::internalInitFields() {
	
	OraField* field;
	CRCursorState oldCursorState;
	CRString alias;
	int aliasNum;
	char stAliasNum[4];
	bool implicitPrepared = false;
	
	oldCursorState = mCommand.cursorState();
	
	mFetchCursor = mCommand.cursor();
	
	try {
		if (mCommand.nativeCursor()) {
			
			if (!mCommand.isPrepared()) {
				mCommand.prepare();
				implicitPrepared = true;
			}
			
			if (mCommand.SQLType() == SQL_SELECT) {
#ifdef OCL_OCI8
				if (mCommand.cursorState() < csExecuted)
					mCommand.internalExecute(OCI_DESCRIBE_ONLY);
#endif
			}
			else
				if (mCommand.SQLType() == SQL_PLSQL) {
					mFetchCursor = 0;
					for (int i = 0; i < mCommand.paramCount(); i++)
						if (mCommand.mParams[i].mDataType == dtCursor) {
							mFetchCursor = &(mCommand.mParams[i].getCursor());
							break;
						}
						
						if (!mFetchCursor) {
							if (oldCursorState == csInactive)
								mCommand.unprepare();
							return;
						}
						
						if (mCommand.cursorState() < csBound)
							mCommand.bindParams();
						
						if (mCommand.cursorState() < csExecuted)
							mCommand.exec();   // need for describe
						
						mCommand.setCursor(mFetchCursor);
				}
				else {
					if (oldCursorState == csInactive)
						mCommand.unprepare();
					return;
				}
		}
		else
			mCommand.checkActive();
		
		mCommand.mConnection->busyWait();
		try {
			int i = 1;
			while (true) {
				field = new OraField();
				if (mCommand.getFieldDesc(i, field, mLongString)) {
					field->mActualName = field->mName;
					field->mFieldNo = mFields.count() + 1;
					
					alias = field->mName;
					aliasNum = 1;
					while (findField(alias.c_str())) {
						sprintf(stAliasNum, "%i", aliasNum);
						alias = field->mName + '_' + stAliasNum;
						aliasNum++;
					}
					if (aliasNum > 1)
						field->mName = alias;
					
					mFields.add(field);
					i++;
				}
				else {
					delete field;
					break;
				}
			}
			// finally
			mCommand.mConnection->release();
		}
		catch (...) {
			mCommand.mConnection->release();
			delete field; // when getFieldDesc throw exception
			throw;
		}
		
		if (implicitPrepared)
			mCommand.unprepare();
		
		// finally
		if (oldCursorState == csInactive)
			mCommand.unprepare();
	}
	catch(OraException &e)
	{
		if (implicitPrepared)
			mCommand.unprepare();
		
		if (oldCursorState == csInactive)
			mCommand.unprepare();
		throw e;
	}
	catch (...) {
		if (implicitPrepared)
			mCommand.unprepare();
		
		if (oldCursorState == csInactive)
			mCommand.unprepare();
		throw;
	}
	
	mIndicatorsSize = mFields.count()*sizeof(short);
}

void OraRecordset::getDateFromBuf(void* buf, void* date) {
	
	oraDateToCRDate(buf, *((CRDate*)date));
}

void OraRecordset::putDateToBuf(void* buf, void* date) {
}

void OraRecordset::createComplexFields(void* recBuf, bool withBlob) {
	
	//CRMemDataset::createComplexFields(recBuf, withBlob);
	
	void** ptr;
	OraCursor* cursor;
	OraLong* oraLong;  
	
#ifdef OCL_OCI8
	OraLob*  oraLob;
	OraFile* oraFile;
#endif
	
#ifdef OCL_OCI90
	OraTimeStamp* oraTimeStamp, *oraTimeStampTZ, *oraTimeStampLTZ;
	OraInterval* intervalYM, *intervalDS;
#endif
	
	for (int i = 0; i < fieldCount(); i++) {
		ptr = (void**)((char*)recBuf + ((OraField&)mFields[i]).mDataOffset);
		
		if (!((OraField&)mFields[i]).hasParent() && (!(((OraField&)mFields[i]).isComplex()) || withBlob)) {
			
			switch (mFields[i].dataType()) {
			case dtLong:
			case dtLongRaw:
				oraLong = new OraLong;
				*(OraLong**)ptr = oraLong;
				break;
			case dtCursor:
				cursor = new OraCursor();
				*(OraCursor**)ptr = cursor;
				break;          
#ifdef OCL_OCI8        
			case dtBlob:
			case dtClob:
				oraLob = new OraLob(*mCommand.mConnection);
				oraLob->allocLob();
				*(OraLob**)ptr = oraLob;
				break;
			case dtBFile:
			case dtCFile:
				oraFile = new OraFile(*mCommand.mConnection);
				oraFile->allocLob();
				*(OraFile**)ptr = oraFile;
				break;
#endif
#ifdef OCL_OCI90
			case dtTimeStamp:
				oraTimeStamp = new OraTimeStamp(dtTimeStamp);
				*(OraTimeStamp**)ptr = oraTimeStamp;
				break;
			case dtTimeStampTZ:
				oraTimeStampTZ = new OraTimeStamp(dtTimeStampTZ);
				*(OraTimeStamp**)ptr = oraTimeStampTZ;
				break;
			case dtTimeStampLTZ:
				oraTimeStampLTZ = new OraTimeStamp(dtTimeStampLTZ);
				*(OraTimeStamp**)ptr = oraTimeStampLTZ;
				break;
				
			case dtIntervalYM:
				intervalYM = new OraInterval(dtIntervalYM);
				*(OraInterval**)ptr = intervalYM;
				break;
			case dtIntervalDS:
				intervalDS = new OraInterval(dtIntervalDS);
				*(OraInterval**)ptr = intervalDS;
				break;
#endif
				
			} //switch
		} //if
	} //for
}

void OraRecordset::freeComplexFields(void* recBuf, bool withBlob) {
	
	for (int i = 0; i < fieldCount(); i++) {
		if (!((OraField&)mFields[i]).hasParent() && (!(((OraField&)mFields[i]).isComplex()) || withBlob)) {
			
			switch (mFields[i].dataType()) {
			case dtLong:
			case dtLongRaw:
			case dtBlob:
			case dtClob:
			case dtBFile:
			case dtCFile:
			case dtCursor:
			case dtObject:
			case dtReference:
			case dtArray:
			case dtTable:
				(*(CRSharedObject**)((char*)recBuf + ((OraField&)mFields[i]).mDataOffset))->release();
				break;
			} 
		} 
	}
}

// Records / buffers

void OraRecordset::getNextRecord(void* recBuf) {
	bool found;
	ItemHeader* item;
	
	if (!isEOF()) {
		if (mCurrentItem) {
			mCurrentItem = mCurrentItem->next;
			mBOF = false;
		}
		
		do {
			if (!mCurrentItem) {
				item = mLastItem;
				if (!fetch()) {
					mCurrentItem = mLastItem;
					mEOF = true;
				}
				else
					if (!item || !mCached)
						mCurrentItem = mFirstItem;
					else {
						mCurrentItem = item->next;
						mBOF = false;
					}
			}
			
			found = true;
		} while (!found);
		
		getRecord(recBuf);
	}
}

void OraRecordset::getPrevRecord(void* RecBuf) {
	
	CRMemDataset::getPrevRecord(RecBuf);
}

void OraRecordset::close() {
	try {
		CRDataset::close();
		mFields.clear();
	}
	catch(OraException &e)
	{
		mFields.clear();
		
		throw e;
	}
	catch (...) {
		mFields.clear();
		
		throw;
	}
}

// Navigation

void OraRecordset::internalLast() {
	
	if (mCommand.cursorState() < csCanceled)
		fetchAll();
	
	CRMemDataset::internalLast();
}

// Fetch

bool OraRecordset::fetch() {
	bool result = false;
	
	if (mCommand.cursorState() < csCanceled) {
		if (mPieceFetch)
			result = fetchPiece();
		else
			result = fetchArray();
		
		if (mCommand.cursorState() == csCanceled && mAutoClose) {
			mCommand.setCursor(0);
			mFetchCursor->freeCursor(); // When RefCount = 1
			mCommand.unprepare();
		}
	}
	
	return result;
}

#ifdef OCL_OCI8

sb4 callbackDefine(void* octxp, OCIDefine* definp, ub4 iter,
				   void** bufp, ub4** alenpp, ub1* piecep, void** indpp, ub2** rcodepp)
{
	sb4 res = ((OraRecordset*)octxp)->callbackDefine(definp, iter, bufp, alenpp, piecep, indpp);
	
	*rcodepp = 0;
	
	return res;
}

sb2 OraRecordset::callbackDefine(OCIDefine* definp, ub4 iter, void** bufp,
								 ub4** alenpp, ub1* piecep, void** indpp)
{
	OraLong* blob;
	PieceHeader* piece;
	ub4 len;
	int i = 0;
	
	while ((i < fieldCount()) && (((OraField*)&mFields[i])->mHandle != definp))
		i++;
	
	assert(i < fieldCount());
	
	blob = *(OraLong**)((char*)mFetchItems + (mRecordSize + sizeof(ItemHeader))*iter +
		sizeof(ItemHeader) + ((OraField&)mFields[i]).mDataOffset);
	
	len = ((OraLob*)blob)->defPieceSize();
	piece = ((OraLob*)blob)->allocPiece(len);
	((OraLob*)blob)->appendPiece(piece);
	
	*bufp = PieceHeader::pieceData(piece);
	piece->used = len;
	*alenpp = (ub4*)PieceHeader::pieceUsedPtr(piece);
	
	*indpp = (char*)mFetchItems + (mRecordSize + sizeof(ItemHeader))*iter +
		sizeof(ItemHeader) + mDataSize + i * sizeof(sb2);
	
	*piecep = OCI_NEXT_PIECE;
	
    return OCI_CONTINUE;
}

#endif // OCL_OCI8

void OraRecordset::allocFetchBlock() {
	
	if (mFetchBlock != 0)
		freeFetchBlock();
	
	mFetchBlockItemSize = 0;
	
	for (int i = 0; i < fieldCount(); i++)
		if (!((OraField&)mFields[i]).hasParent() && ((OraField&)mFields[i]).isComplex())
			if (mFields[i].dataType() == dtLong && ((OraField&)mFields[i]).mSubDataType == dtString)
				mFetchBlockItemSize  += ((OraField&)mFields[i]).mLength + 1;
			else
				mFetchBlockItemSize += sizeof(void*);
			
			if (mFetchBlockItemSize > 0)
				mFetchBlock = malloc(mFetchBlockItemSize * mFetchRows);
}

void OraRecordset::freeFetchBlock() {
	
	if (mFetchBlock != 0)
		free(mFetchBlock);
	
	mFetchBlock = 0;
	mFetchBlockItemSize = 0;  
}

void OraRecordset::initBlock(BlockHeader *block) {
	
    void* ptr;
#ifdef OCL_OCI8
    void* objPtr; // warning remove
#endif
	
    int itemSize = mRecordSize + sizeof(ItemHeader);
    int complexFieldOffset = 0;
	
    for (int i = 0; i < mFetchRows; i++) {
		
		ptr = (char*)block + sizeof(BlockHeader) + i*itemSize + sizeof(ItemHeader);
		createComplexFields(ptr, true);
		
		for (int j = 0; j < fieldCount(); j++) {
			
			if (!((OraField&)mFields[j]).hasParent() && ((OraField&)mFields[j]).isComplex()) {
				
#ifdef OCL_OCI8 // BCB W8004 remove
				objPtr = *(void**)((char*)ptr + ((OraField&)mFields[j]).mDataOffset);
#endif
				
				switch (mFields[j].dataType()) {
				case dtLong:
					if (((OraField&)mFields[j]).mSubDataType == dtString)
						*((char*)mFetchBlock + complexFieldOffset) = '\0';
					break;
#ifdef OCL_OCI8            
				case dtCursor:
					((OraCursor*)objPtr)->mState = csExecuted;
					*(void**)((char*)mFetchBlock + complexFieldOffset) = ((OraCursor*)objPtr)->getOCIStmt();
					break;
					
				case dtBlob: case dtClob: case dtBFile: case dtCFile:
					*(void**)((char*)mFetchBlock + complexFieldOffset) = ((OraLob*)objPtr)->getOCILobLocator();
					break;
#endif
#ifdef OCL_OCI90
				case dtTimeStamp:
				case dtTimeStampTZ:
				case dtTimeStampLTZ:
					*(void**)((char*)mFetchBlock + complexFieldOffset) = ((OraTimeStamp*)objPtr)->getOCIDateTime();
					break;
				case dtIntervalYM:
				case dtIntervalDS:
					*(void**)((char*)mFetchBlock + complexFieldOffset) = ((OraInterval*)objPtr)->getOCIInterval();
					break;
#endif
				}
				
				if (mFields[j].dataType() == dtLong && ((OraField&)mFields[j]).mSubDataType == dtString)
					complexFieldOffset += ((OraField&)mFields[j]).mDataSize;
				else
					complexFieldOffset += sizeof(void*);
			} //if
		} //for
    } //for
}

void OraRecordset::prepareBlock(BlockHeader* block, int fetched) {
	
    void* ptr;
    void* objPtr;
    PieceHeader* piece;
    int len;
	
    int complexFieldOffset = 0;
    int itemSize = mRecordSize + sizeof(ItemHeader);
	
    for (int i = 0; i < fetched; i++) {
		
		ptr = (char*)block + sizeof(BlockHeader) + i*itemSize + sizeof(ItemHeader);
		
		for (int j = 0; j < fieldCount(); j++) {
			
			if (!((OraField&)mFields[j]).hasParent() && ((OraField&)mFields[j]).isComplex()) {
				
				objPtr = *(void**)((char*)ptr + ((OraField&)mFields[j]).mDataOffset);
				
				switch (mFields[j].dataType()) {
				case dtLong:
					if (((OraField&)mFields[j]).mSubDataType == dtString) {
						
						len = strlen((char*)mFetchBlock + complexFieldOffset);
						
						if (len > 0) {
							piece = ((OraLong*)objPtr)->allocPiece(len); // for term
							
							((OraLong*)objPtr)->appendPiece(piece);
							
							strncpy((char*)((OraLong*)objPtr)->mFirstPiece + sizeof(PieceHeader),
								(char*)mFetchBlock + complexFieldOffset, len);
							
							((OraLong*)objPtr)->mFirstPiece->used = len;
						}
					}
				}
				
				if (mFields[j].dataType() == dtLong && ((OraField&)mFields[j]).mSubDataType == dtString) // String as memo
					complexFieldOffset += ((OraField&)mFields[j]).mLength + 1;
				else
					complexFieldOffset += sizeof(void*);        
				
			}
		} //for
    } //for
}


void OraRecordset::disconnectFromSession() {
	close();
}

void OraRecordset::clearBlock(BlockHeader* block) {
	
	int itemSize = mRecordSize + sizeof(ItemHeader);
	
	// Free complex filds
	for (int i = 0; i < mFetchRows; i++)
		freeComplexFields((char*)block + sizeof(BlockHeader) + i*itemSize + sizeof(ItemHeader), true);
	
}

bool OraRecordset::fetchArray() {
	bool result = false;
	int i;
	void* pRec;
	void* pInd;
	int fetched;
	BlockHeader* block;
	ItemHeader* item;
	int itemSize;
	bool newBlock;
	void* bufferPtr;
	int complexFieldOffset;
	int bufSkip;
	void* indPtr;
	CRLob* blob;
	
	mCommand.busy();
	// for close in time fetch all
	if (mCommand.cursorState() < csBound) {   // For Non Blocking
		mCommand.release();
		return false;
	}
	
	try { // For Busy
		itemSize = mRecordSize + sizeof(ItemHeader);
		newBlock = mCached || !mBlockman.firstBlock();
		
		if (newBlock) {
			block = mBlockman.allocBlock(mFetchRows);
			
			if (hasComplexFields()) {
				if (!mFetchBlock)
					allocFetchBlock();
				initBlock(block);
			}
		}
		else
			block = mBlockman.firstBlock();
		
		try {    // For free memory
			if (newBlock) {
				pRec = (char*)block + sizeof(BlockHeader) + sizeof(ItemHeader);
				pInd = (char*)pRec + mDataSize;
				
				mCommand.mConnection->busyWait();
				try { // For Session.Busy
					// DefineData
					complexFieldOffset = 0;
					for (int i= 0; i < fieldCount(); i++){
						if (!((OraField&)mFields[i]).hasParent()) {
							
							if ((mFields[i].dataType() == dtLong || mFields[i].dataType() == dtLongRaw) &&
								((OraField&)mFields[i]).mSubDataType != dtString) {
#ifdef OCL_OCI8
								mCommand.defineDynamic((OraField*)&mFields[i], this, ocl::callbackDefine);
								complexFieldOffset += sizeof(void*);
#else
								;
#endif
							}
							else {
								if (((OraField&)mFields[i]).isComplex()) {
									
									bufferPtr = (char*)mFetchBlock + complexFieldOffset;
									bufSkip = mFetchBlockItemSize;
									
									if (mFields[i].dataType() == dtLong && ((OraField&)mFields[i]).mSubDataType == dtString) // String as memo
										complexFieldOffset += ((OraField&)mFields[i]).mLength + 1;
									else
										complexFieldOffset += sizeof(void*);
									
								}
								else {
									bufferPtr = (char*)pRec + ((OraField&)mFields[i]).mDataOffset;
									bufSkip = itemSize;
								}
								
								if (mFields[i].dataType() == dtObject/*,dtReference,dtArray,dtTable*/)
									indPtr = 0; // WAR Oracle bug - indicator skip is ignored for Objects
								else
									indPtr = (char*)pInd + i * sizeof(sb2);
								
								mCommand.defineArrayData((OraField*)&mFields[i], bufferPtr,
									(sb2*)indPtr, bufSkip, itemSize);
							}
						}
					}
					// finally
					mCommand.mConnection->release();
				}
				catch (...) {
					mCommand.mConnection->release();
					throw;
				}
			}
			
			mFetchItems = (char*)block + sizeof(BlockHeader);
			
			fetched = mCommand.internalFetch(mFetchRows);
		}
		catch (OraException &e )//jianghp, added
		{
			if (newBlock) {
				if (hasComplexFields())
					clearBlock(block);
				mBlockman.freeBlock(block);
			}
			throw e;
		}
		catch (...) {
			if (newBlock) {
				if (hasComplexFields())
					clearBlock(block);
				mBlockman.freeBlock(block);
			}
			throw;
		}  // try
		
		result = fetched > 0;
		
		if (result) {
			
			for (i = 0; i < fieldCount(); i++)
				if (mFields[i].dataType() == dtLong || mFields[i].dataType() == dtReference)
					prepareBlock(block, fetched);
				
				// Create Items
				item = (ItemHeader*)((char*)block + sizeof(BlockHeader));
				if (!mFirstItem)
					mFirstItem = item;
				
				for (i = 0; i < fetched; i++) {
					// Prepare comlex field
					
#ifdef OCL_OCI8
					OraLob* lob;
					OraFile* file;
					//COraRef* ref;        
#endif
					
					if (hasComplexFields())
						for (int j = 0; j < fieldCount(); j++)
							if (!((OraField&)mFields[j]).hasParent()) {
								switch (mFields[j].dataType()) {
								case dtLong:
								case dtLongRaw:
									if (((OraField&)mFields[j]).mSubDataType != dtString) {
										blob = *(OraLong**)((char*)item + sizeof(ItemHeader) + ((OraField&)mFields[j]).mDataOffset);
										blob->compress();
									}
									break;
									
#ifdef OCL_OCI8
								case dtBlob: case dtClob:
									lob = *(OraLob**)((char*)item + sizeof(ItemHeader) + ((OraField&)mFields[j]).mDataOffset);
									lob->readLob();
									break;
									
								case dtBFile: case dtCFile:
									file = *(OraFile**)((char*)item + sizeof(ItemHeader) + ((OraField&)mFields[j]).mDataOffset);
									if (file->exist()) {
										file->open();
										try {
											file->readLob();
										}     
										catch (OraException &e )//jianghp, added
										{
											file->close();
											throw e;
										}
										catch (...) {
											file->close();
											throw;
										}
									}
									break;
#endif
								}
							}
							
							item->prev = mLastItem;
							item->next = 0;
							item->block = block;
							item->flag = flUsed;
							item->rollback = 0;
							item->status = isUnmodified;
							
							if (mLastItem) {
								mLastItem->next = item;
								item->order = mLastItem->order + 1;
							}
							else
								item->order = 1;
							
							mLastItem = item;
							item = (ItemHeader*)((char*)item + itemSize);
				}
				mFirstItem->prev = 0;
				mLastItem->next = 0;
				
				mRecordCount += fetched;
				
				// Free items
				for (i = 0; i < mFetchRows - fetched; i++) {
					item->prev = 0;
					item->next = mBlockman.firstFree();
					item->block = block;
					item->flag = flFree;
					item->rollback = 0;
					item->status = isUnmodified;
					
					if (mBlockman.firstFree())
						mBlockman.firstFree()->prev = item;
					mBlockman.setFirstFree(item);
					
					if (hasComplexFields())
						// Free complex fields
						freeComplexFields((char*)item + sizeof(ItemHeader)/*, true*/);
					
					item = (ItemHeader*)((char*)item + itemSize);
				}
				
				block->usedItems = (word)fetched;
		}
		else {
			if (newBlock) {
				if (hasComplexFields())
					clearBlock(block);
				mBlockman.freeBlock(block);
			};
			
			if (hasComplexFields())
				freeFetchBlock();
		}
		// finally
		mCommand.release();
  }
  catch (OraException &e )//jianghp, added
  {
	mCommand.release();
	throw e;
  }
  catch (...) {
	  mCommand.release();
  }
  return result;
}

bool OraRecordset::fetchPiece() {
	bool result = false;
	int i;
	void* pRec;
	void* pInd;
	int res = 0;
	ItemHeader* item;
	ub1 pieceStatus = 0;
	PieceHeader* piece;
	long bufLen = 0;
	void* bufPtr;
	int needData = 0;
	int noData = 0;
	long iteration = 0;
	long index = 0;
	void* hDefine;
	void* ptr;
	void* objPtr;
	static long temp = 0;
	CRLob* blob;
	CRParamDirection mode = pdUnknown;
	
#ifdef OCL_OCI8
	OraFile* oraFile;
#endif
	
	mCommand.busy();
	if (mCommand.cursorState() < csBound) {   // For Non Blocking
		mCommand.release();
		return false;
	}
	
	try {  // For Busy
		if (!mCached && (mFirstItem != 0)) {
			item = mFirstItem;
			
			freeComplexFields((char*)item + sizeof(ItemHeader), true);
			
			mFirstItem = 0;
			mLastItem = 0;
			mCurrentItem = 0;
		}
		else
			item = mBlockman.allocItem();
		
		pRec = (char*)item + sizeof(ItemHeader);
		pInd = (char*)pRec + mDataSize;
		bufLen = maxBlobSize;
		mFetchItem = item;
		
		createComplexFields((char*)item + sizeof(ItemHeader), true);
		
		mCommand.mConnection->busyWait();
		
		try {    // For free memory and Session.Busy
			// Picewise fetch as one operation
			
			// DefineData  // OPTIM - Once
			for (i = 0; i < fieldCount(); i++) {
				if (!((OraField&)mFields[i]).hasParent()) {
					bufPtr = (char*)pRec + ((OraField&)mFields[i]).mDataOffset;
					
					switch (mFields[i].dataType()) {
#ifdef OCL_OCI8
					case dtCursor:
						(*(OraCursor**)bufPtr)->mState = csExecuted;
						bufPtr = (*(OraCursor**)bufPtr)->getOCIStmt();
						break;
						
					case dtBlob:
					case dtClob:
					case dtCFile:
					case dtBFile:
						bufPtr = (*((OraLob**)bufPtr))->getOCILobLocator();
						break;
#endif // OCL_OCI8
#ifdef OCL_OCI90
					case dtTimeStamp:
					case dtTimeStampTZ:
					case dtTimeStampLTZ:
						bufPtr = (*((OraTimeStamp**)bufPtr))->getOCIDateTime();
						break;
						
					case dtIntervalYM:
					case dtIntervalDS:
						bufPtr = (*((OraInterval**)bufPtr))->getOCIInterval();
						break;
#endif
						
					case dtLong:
						if (((OraField&)mFields[i]).mSubDataType == dtString) {
							piece = (*(OraLong**)bufPtr)->allocPiece(((OraField&)mFields[i]).mDataSize);
							(*(OraLong**)bufPtr)->appendPiece(piece);
							bufPtr = (char*)piece + sizeof(PieceHeader);
						}
					}
					
					if (((mFields[i].dataType() == dtLongRaw) ||
						(mFields[i].dataType() == dtLong)) &&
						(((OraField&)mFields[i]).mSubDataType != dtString))
					{
						switch (mCommand.mOCICallStyle) {
#ifdef OCL_OCI7                
						case OCI73:
							mCommand.definePieceData((OraField*)&mFields[i], 0, (sb2*)((char*)pInd + i*sizeof(sb2)));
							break;
#endif
#ifdef OCL_OCI8                  
						case OCI80:
							mCommand.defineDynamic((OraField*)&mFields[i], this, ocl::callbackDefine);
#endif
						}
					}
					else
						mCommand.defineData((OraField*)&mFields[i], bufPtr, (sb2*)((char*)pInd + i*sizeof(sb2)));
				}
			}
			mFetchItems = item;
			res = mCommand.internalFetchPiece();
			
			switch (mCommand.OCICallStyle()) {
#ifdef OCL_OCI7        
			case OCI73:
				needData = OCI_STILL_IS_PIECE;
				noData = OCI_NO_DATA_FOUND;
				break;    
#endif
#ifdef OCL_OCI8          
			case OCI80:
				needData = OCI_NEED_DATA;
				noData = OCI_NO_DATA;
				break;    
#endif
			}
			if (res == needData)  {
				for (i = 0; i < fieldCount(); i++)
					if (mFields[i].dataType() == dtLongRaw || mFields[i].dataType() == dtLong) {
						
						mCommand.getPI(&hDefine, &pieceStatus, &ptr, &iteration, &index, &mode);
						
						blob = 0; // anti warning
						if (pieceStatus == OCI_FIRST_PIECE || pieceStatus == OCI_ONE_PIECE)
							blob = *(OraLong**)((char*)item + sizeof(ItemHeader) + ((OraField&)mFields[i]).mDataOffset);
						else
							assert(false); // DEBUG
						
						do {
							bufLen = ((OraLong*)blob)->defPieceSize();
							piece = ((OraLong*)blob)->allocPiece(bufLen);
							
							mCommand.setPI(hDefine, 6 /*=OCI_HTYPE_DEFINE*/, pieceStatus, (char*)piece + sizeof(PieceHeader),
								&bufLen, (sb2*)((char*)pInd + i*sizeof(sb2)));
							
							res = mCommand.internalFetchPiece();
							
							if (bufLen > 0) {
								piece->used = bufLen;
								if (bufLen + 1 < ((OraLong*)blob)->defPieceSize())
									((OraLong*)blob)->reallocPiece(&piece, bufLen + 1); // for terminator
								((OraLong*)blob)->appendPiece(piece);
							}
							else
								((OraLong*)blob)->freePiece(piece);
							
							mCommand.getPI(&hDefine, &pieceStatus, &ptr, &iteration, &index, &mode);
							
							if (pieceStatus == OCI_FIRST_PIECE) {
								bufLen = sizeof(long);
								mCommand.setPI(hDefine, 6 /*=OCI_HTYPE_DEFINE*/, pieceStatus, &temp, &bufLen, 0);
							}
						} while (!(res == 0 || res == noData || pieceStatus == OCI_FIRST_PIECE));
					}
			}
			else
#ifdef OCL_OCI7
				//          if (res != noData)
				//            assert(false);   // DEBUG
#endif
				
				// finally
				mCommand.mConnection->release();
    }
	catch (OraException &e )//jianghp, added
	{
		mCommand.mConnection->release();
		
		freeComplexFields((char*)item + sizeof(ItemHeader), true);
		mBlockman.freeItem(item);
		throw e;
	}
    catch (...) {
		mCommand.mConnection->release();
		
		freeComplexFields((char*)item + sizeof(ItemHeader), true);
		mBlockman.freeItem(item);
		throw;
    }
	
    result = (res == 0);
	
    if (result)  {
		// Prepare comlex field
		if (hasComplexFields())
			for (i = 0; i < fieldCount(); i++) {
				objPtr = *(void**)((char*)item + sizeof(ItemHeader) + ((OraField&)mFields[i]).mDataOffset);
				
				switch (mFields[i].dataType()) {
				case dtLongRaw:
				case dtLong:
					if (((OraField&)mFields[i]).mSubDataType == dtString)
						((OraLong*)objPtr)->firstPiece()->used = strlen((char*)(((OraLong*)objPtr)->firstPiece()) + sizeof(PieceHeader));
					else
						((OraLong*)objPtr)->compress();
					break;
					
#ifdef OCL_OCI8
				case dtBlob:
				case dtClob:
					((OraLob*)objPtr)->readLob();
					break;
					
				case dtBFile:
				case dtCFile: {
					oraFile = (OraFile*)objPtr;
					if (oraFile->exist())  {
						oraFile->open();
						try {
							oraFile->readLob();
						}
						catch (OraException &e )//jianghp, added
						{
							oraFile->close();
							throw e;
						}
						catch (...) {
							oraFile->close();
						}
					}
				 }
#endif //OCL_OCI8
				}
			}
			
			// Create Items
			if (!mFirstItem)
				mFirstItem = item;
			
			item->prev = mLastItem;
			item->next = 0;
			item->flag = flUsed;
			
			if (mLastItem) {
				mLastItem->next = item;
				item->order = mLastItem->order + 1;
			}
			else
				item->order = 1;
			
			mLastItem = item;
			
			item->rollback = 0;
			item->status = isUnmodified;
			item->updateResult = urNone;
			
			mRecordCount++;
    }
    else {
		freeComplexFields((char*)item + sizeof(ItemHeader), true);
		
		mBlockman.freeItem(item);
    }
	
	// finally
    mCommand.release();
  }
  catch (OraException &e )//jianghp, added
  {
	  mCommand.release();
	  throw e;
  }
  catch (...) {
	  mCommand.release();
	  throw;
  }
  
  return result;
}

void OraRecordset::doExecFetch () {
	
	if (mCommand.cursorState() < csBound)
		mCommand.bindParams();
	
	if (mCommand.cursorState() < csExecuted)  // for PL/SQL
		mCommand.exec();
	
	if (!rowsReturn())
		checkRowsReturn();
	
	mCommand.setCursor(mFetchCursor);
	
	if (mCommand.mNonBlocking)
		fetch();
}

void OraRecordset::endExecFetch(bool result) {
	
	if (mCommand.mNonBlocking && mFirstItem) {
		mCurrentItem = mFirstItem;
		mBOF = false;
	}
	
	if (result && mFetchAll)
		fetchAll();
}

void OraRecordset::execFetch() {
	
	if (mCommand.mNonBlocking) {
	}
	else {
		doExecFetch();
		if (mFetchAll)
			fetchAll();
	}
}

void OraRecordset::doFetchAll() {
	CRCursorState OldCursorState;
	
	if (mCommand.cursorState() < csFetchingAll) {
		OldCursorState = mCommand.cursorState();
		mCommand.setCursorState(csFetchingAll);
		try {
			while (fetch());
		}
		catch (OraException &e )//jianghp, added
		{
			mCommand.setCursorState(OldCursorState);
			throw e;
		}			
		catch (...) {
			mCommand.setCursorState(OldCursorState);
			throw;
		}
	}
}

void OraRecordset::doFetchAllPulse() {
}

void OraRecordset::endFetchAll(bool result) {
}

void OraRecordset::fetchAll() {
	
	if (mCommand.mNonBlocking) {
	}
	else
		doFetchAll();
}

int OraRecordset::fetchRows() const {
	
	return mFetchRows;
}

void OraRecordset::setFetchRows(int value) {
	
	mFetchRows = value;
}

bool OraRecordset::isCached() const {
	
	return mCached;
}

void OraRecordset::setCached(bool value) {
	
	mCached = value;
}

void OraRecordset::setLongString(bool newValue) {
	mLongString = newValue;
}

bool OraRecordset::longString() const {
	return mLongString;
}

//////////////////  OraQuery  ///////////////////

//'this' : used in base member initializer list
#ifdef WIN32
#pragma warning(disable : 4355)
#endif

OraQuery::OraQuery() : OraRecordset((OraCommand&)*this) {
	_init();
}

OraQuery::OraQuery(OraConnection& connection)
: OraRecordset((OraCommand&)*this), OraCommand(connection)
{
	_init();
}

OraQuery::OraQuery(OraConnection& connection, const CRString& sql)
: OraRecordset((OraCommand&)*this), OraCommand(connection, sql)
{
	_init();
}

#ifdef WIN32
#pragma warning(default : 4555)
#endif

OraQuery::~OraQuery() {
	close();
}

void OraQuery::_init() {
	mImplicitPrepared = true;
}

void OraQuery::prepare() {
	if (!isPrepared()) {
		if (!OraCommand::isPrepared())
			OraCommand::prepare();
		
		initFields();
		mImplicitPrepared = false;
	}
}

void OraQuery::unprepare() {
	
	if (isPrepared()) {
		OraCommand::unprepare();
		mImplicitPrepared = true;
	}
	
	mFields.clear();
}

void OraQuery::open() {
	
	if (!mImplicitPrepared)
		CRDataset::open();
	else {
		bool doCommandUnprepare = false;
		try {
			if (!OraCommand::isPrepared()) {
				OraCommand::prepare();
				doCommandUnprepare = true;
			}
			
			if ( nativeCursor() ){
				OraCommand::execute();
			}
			
			initFields();
			CRDataset::open();
		}
		catch (OraException &e )//jianghp, added
		{
			if (doCommandUnprepare)
				OraCommand::unprepare();
			
			throw e;
		}
		catch (...) {
			if (doCommandUnprepare)
				OraCommand::unprepare();
			
			throw;
		}
	}
}

void OraQuery::close() {
	if (isActive()) {
		try {
			CRDataset::close();
			
			//finnaly
			if(mImplicitPrepared)
				unprepare();
		}
		catch (OraException &e )//jianghp, added
		{
			if(mImplicitPrepared) {
				unprepare();
				mImplicitPrepared = false;
			}
			throw e;
		}
		catch (...) {
			//finnaly
			if(mImplicitPrepared) {
				unprepare();
				mImplicitPrepared = false;
			}
			
			throw;
		}
	}
}

bool OraQuery::isActive() const {
	return OraRecordset::isActive();
}

int OraQuery::execute() {
	
	if (cursorState() == csExecuting)
		return 0;
	
	if (!mImplicitPrepared) {
		if (isRowsReturn())
			CRDataset::open();
	}
	else {
		bool doCommandUnprepare = false;
		try {
			if (!OraCommand::isPrepared()) {
				OraCommand::prepare();
				doCommandUnprepare = true;
			}
			
			OraCommand::execute();
			
			if (isRowsReturn()) {
				initFields();
				CRDataset::open();
			}
			else if (doCommandUnprepare)
				OraCommand::unprepare();
		}
		catch (OraException &e )//jianghp, added
		{
			if (doCommandUnprepare)
				OraCommand::unprepare();
			throw e;
		}
		catch (...) {
			if (doCommandUnprepare)
				OraCommand::unprepare();
			
			throw;
		}
	}
	
	return rowsProcessed();  
}

int getColumnsInfo(const CRString& sql, ColumnInfo columnsInfo[]) {
	OraParser parser(sql);
	
	parser.setOmitBlank(true);
	
	if (!parser.toLexeme(lxSELECT))
		return 0;
	
	char lexeme[256];
	int code = 0, count = 0, prevCode;
	CRString name, table, schema, alias, prevLexeme, expr;
	ColumnInfo columnInfo;
	bool isExpr = false;
	
	do {
		
		prevCode = code;
		prevLexeme = lexeme;
		
		code = parser.next(lexeme);
		
		// keywords after SELECT
		if (!(strcmp(lexeme, "ALL") && strcmp(lexeme, "DISTINCT") && strcmp(lexeme, "UNIQUE")))
			continue;
		
		int bracketCount = 0;
		
		if ((code == lcIdent || *lexeme == '*') && !isExpr) {
			name = lexeme;
			code = parser.next(lexeme);
			if (*lexeme == '.') {
				code = parser.next(lexeme);
				if (code == lcIdent || *lexeme == '*') {
					table = name;
					name = lexeme;
					code = parser.next(lexeme);
					if (*lexeme == '.') {
						code = parser.next(lexeme);
						if (code == lcIdent || *lexeme == '*') {            
							schema = table;
							table = name;
							name = lexeme;
							code = parser.next(lexeme);
						}
					}
				}
			}
			
			if (code == lxAS)
				code = parser.next(lexeme);
			
			if (code == lcIdent) {
				alias = lexeme;
				code = parser.next(lexeme);
			}
		}
		
		if (*lexeme == ',' && bracketCount == 0 || (code == lcEnd || code == lxFROM)) {
			if (!isExpr && (name == "NULL" || name == "SYSDATE" || name == "USER" || name == "UID")) {
				isExpr = true;
				expr = name;
			}
			
			if (!isExpr) {
				columnsInfo[count].name = name;
				columnsInfo[count].table = table;
			}
			else
				columnsInfo[count].expr = expr;
			
			columnsInfo[count].alias = alias;
			
			count++;
			
			if (*lexeme == ',') {
				name = "";
				table = "";
				alias = "";
				expr = "";
				isExpr = false;        
			}
			else
				break;
		}
		else {
			if (!isExpr) {
				expr = name;
				if (table.empty())
					expr = name;
				else
					expr = table + '.' + name;
				alias = "";
				isExpr = true;
			}
			else
				if (!alias.empty())
					expr += alias;
				
				if (*lexeme == '(')
					bracketCount++;
				else if (*lexeme == ')')
					bracketCount--;
				
				if (code == lxAS)
					continue;
				
				if (bracketCount == 0 && code == lcIdent && 
					((prevCode == lcIdent || prevCode == lxAS || (*prevLexeme == ')'))))
					alias = lexeme;
				else
					expr += lexeme;
		}
	} while (code != lcEnd && code != lxFROM);
	
	return count;
}

int getTablesInfo(const CRString& sql, TableInfo tablesInfo[]) {
	OraParser parser(sql);
	
	parser.setOmitBlank(true);
	
	if (!parser.toLexeme(lxSELECT))
		return 0;
	
	char lexeme[256];
	int bracketCount = 0, count = 0, code;
	CRString name, alias;
	
	do {
		code = parser.next(lexeme);
		
		if (*lexeme == '(')
			bracketCount++;
		else if (*lexeme == ')')
			bracketCount--;
		
	} while (!((code == lxFROM && bracketCount == 0) || code == lcEnd));
	
	if (code != lcEnd) {
		do {
			code = parser.next(lexeme);
			
			if (code == lcIdent) {
				name = lexeme;
				parser.next(lexeme);
				
				if (*lexeme == '.') {
					name += lexeme;
					
					if (parser.next(lexeme) == lcIdent)
						name += lexeme;
					else
						break;
				}
				else
					parser.back();
				
				code = parser.next(lexeme);
				
				// check for remote server
				if (*lexeme == '@') {
					if (parser.next(lexeme) == lcIdent) {
						name += '@' + lexeme;
						code = parser.next(lexeme);
					}
					else
						parser.back();
				}
				
				if (code == lcIdent) {
					alias = lexeme;
					parser.next(lexeme);
				}
				else
					alias.erase();
				
				tablesInfo[count].name = name;
				tablesInfo[count].alias = alias;
				count++;
			}
			else {
				if (*lexeme == '"') {
					int fC = parser.currPos();
					do {
						parser.next(lexeme);
					} while (*lexeme != '"');
					int sC = parser.currPos();
					char tName[256];
					strncpy(tName, parser.getText() + fC - 1, sC - fC + 1);
					tName[sC - fC + 1] = '\0';
					
					code = parser.next(lexeme);
					
					if (code == lcIdent) {
						alias = lexeme;
						parser.next(lexeme);
					}
					else
						alias.erase();
					
					tablesInfo[count].name = tName;
					tablesInfo[count].alias = alias;
					count++;
				}
				else
					break;
			}
		} while (*lexeme == ',');
	}
	
	return count;
}

} // namespace ocl
