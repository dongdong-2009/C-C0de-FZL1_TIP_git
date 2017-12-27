/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oracall.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:20 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  OCI
//  Created:            09.09.99
//  Last modified:      28.08.99
//////////////////////////////////////////////////

#include "ocl.conf"
#include "oracall.h"
#undef OCL_PROF
#include "ora.h"

#include <stdio.h>
#include <stdlib.h>

#include "core/utilities/src/DebugUtil.h"

#ifdef OCL_LINUX
#include <dlfcn.h> // Linux DL API
#endif

#ifdef OCL_SOLARIS
#include <dlfcn.h> // Solaris DL API
#endif

namespace ocl {
	
#ifdef OCL_WIN32
	DWORD (__stdcall *_GetFileVersionInfoSizeA)(LPSTR lptstrFilename, LPDWORD lpdwHandle);
	
	BOOL (__stdcall *_GetFileVersionInfoA)(LPSTR lptstrFilename,
		DWORD dwHandle, DWORD dwLen, LPVOID lpData);
	
	BOOL (__stdcall *_VerQueryValueA)(const LPVOID pBlock, LPSTR lpSubBlock,
		LPVOID * lplpBuffer, PUINT puLen);
#endif
	
	
	const char InalidIndex[]        = "Invalid home index";
	const char InvalidHomeName[]    = "Invalid Oracle home name";
	const char NeedOCI80Interface[] = "Need OCI80 interface";
	const char NeedOCI81Interface[] = "Need OCI8i interface";
	const char NeedOCI9Interface[]  = "Need OCI9i interface";
	
	
	
	// ************************ OCI8 functions *************************
	
#ifdef OCL_DYNAMIC
	
#ifdef OCL_OCI8
	
	_OCIInitialize OCIInitialize;
	_OCITerminate OCITerminate;
	_OCIEnvCreate OCIEnvCreate;
	_OCIHandleAlloc OCIHandleAlloc;
	_OCIHandleFree OCIHandleFree;
	_OCIDescriptorAlloc OCIDescriptorAlloc;
	_OCIDescriptorFree OCIDescriptorFree;
	_OCIEnvInit OCIEnvInit;
	_OCIServerAttach OCIServerAttach;
	_OCIServerDetach OCIServerDetach;
	_OCISessionBegin OCISessionBegin;
	_OCISessionEnd OCISessionEnd;
	_OCIPasswordChange OCIPasswordChange;
	_OCIServerVersion OCIServerVersion;
	_OCISvcCtxToLda OCISvcCtxToLda;
	_OCIStmtPrepare OCIStmtPrepare;
	_OCIBindByName OCIBindByName;
	_OCIBindArrayOfStruct OCIBindArrayOfStruct;
	_OCIBindDynamic OCIBindDynamic;
	_OCIStmtGetPieceInfo OCIStmtGetPieceInfo;
	_OCIStmtSetPieceInfo OCIStmtSetPieceInfo;
	_OCIStmtExecute OCIStmtExecute;
	_OCIDefineByPos OCIDefineByPos;
	_OCIDefineDynamic OCIDefineDynamic;
	_OCIDefineArrayOfStruct OCIDefineArrayOfStruct;
	_OCIStmtFetch OCIStmtFetch;
	_OCIDescribeAny OCIDescribeAny;
	_OCIParamGet OCIParamGet;
	_OCITransStart OCITransStart;
	_OCITransPrepare OCITransPrepare;
	_OCITransCommit OCITransCommit;
	_OCITransRollback OCITransRollback;
	_OCITransDetach OCITransDetach;
	_OCIErrorGet OCIErrorGet;
	_OCILobDisableBuffering OCILobDisableBuffering;
	_OCILobEnableBuffering OCILobEnableBuffering;
	_OCILobFileClose OCILobFileClose;
	_OCILobFileExists OCILobFileExists;
	_OCILobFileIsOpen OCILobFileIsOpen;
	_OCILobFileOpen OCILobFileOpen;
	_OCILobFileSetName OCILobFileSetName;
	_OCILobFileGetName OCILobFileGetName;
	_OCILobGetLength OCILobGetLength;
	_OCILobIsEqual OCILobIsEqual;
	_OCILobIsOpen OCILobIsOpen;
	_OCILobLocatorIsInit OCILobLocatorIsInit;
	_OCILobOpen OCILobOpen;
	_OCILobRead OCILobRead;
	_OCILobTrim OCILobTrim;
	_OCILobWrite OCILobWrite;
	_OCIBreak OCIBreak;
	_OCIAttrGet OCIAttrGet;
	_OCIAttrSet OCIAttrSet;
	_OCIExtProcGetEnv OCIExtProcGetEnv;
	_OCIExtProcRaiseExcpWithMsg OCIExtProcRaiseExcpWithMsg;
	_OCIExtProcRaiseExcp OCIExtProcRaiseExcp;
	
#ifdef OCL_OCI81
	_OCIDirPathFinish OCIDirPathFinish;
	_OCIDirPathPrepare OCIDirPathPrepare;
	_OCIDirPathLoadStream OCIDirPathLoadStream;
	_OCIDirPathColArrayEntrySet OCIDirPathColArrayEntrySet;
	_OCIDirPathColArrayReset OCIDirPathColArrayReset;
	_OCIDirPathColArrayToStream OCIDirPathColArrayToStream;
	_OCIDirPathStreamReset OCIDirPathStreamReset;
#endif // OCL_OCI81
	
#ifdef OCL_OCI90
	_OCIDateTimeAssign OCIDateTimeAssign;
	_OCIDateTimeCheck OCIDateTimeCheck;
	_OCIDateTimeCompare OCIDateTimeCompare;
	_OCIDateTimeConstruct OCIDateTimeConstruct;
	_OCIDateTimeConvert OCIDateTimeConvert;
	_OCIDateTimeFromArray OCIDateTimeFromArray;
	_OCIDateTimeFromText OCIDateTimeFromText;
	_OCIDateTimeGetDate OCIDateTimeGetDate;
	_OCIDateTimeGetTime OCIDateTimeGetTime;
	_OCIDateTimeGetTimeZoneName OCIDateTimeGetTimeZoneName;
	_OCIDateTimeGetTimeZoneOffset OCIDateTimeGetTimeZoneOffset;
	_OCIDateTimeIntervalAdd OCIDateTimeIntervalAdd;
	_OCIDateTimeIntervalSub OCIDateTimeIntervalSub;
	_OCIDateTimeSubtract OCIDateTimeSubtract;
	_OCIDateTimeSysTimeStamp OCIDateTimeSysTimeStamp;
	_OCIDateTimeToArray OCIDateTimeToArray;
	_OCIDateTimeToText OCIDateTimeToText;
	
	_OCIIntervalAdd OCIIntervalAdd;
	_OCIIntervalAssign OCIIntervalAssign;
	_OCIIntervalCheck OCIIntervalCheck;
	_OCIIntervalCompare OCIIntervalCompare;
	_OCIIntervalDivide OCIIntervalDivide;
	_OCIIntervalFromNumber OCIIntervalFromNumber;
	_OCIIntervalFromText OCIIntervalFromText;
	_OCIIntervalFromTZ OCIIntervalFromTZ;
	_OCIIntervalGetDaySecond OCIIntervalGetDaySecond;
	_OCIIntervalGetYearMonth OCIIntervalGetYearMonth;
	_OCIIntervalMultiply OCIIntervalMultiply;
	_OCIIntervalSetDaySecond OCIIntervalSetDaySecond;
	_OCIIntervalSetYearMonth OCIIntervalSetYearMonth;
	_OCIIntervalSubtract OCIIntervalSubtract;
	_OCIIntervalToNumber OCIIntervalToNumber;
	_OCIIntervalToText OCIIntervalToText;
#endif // OCL_OCI90
	
#endif // OCL_OCI8
	
	// ************************ OCI7 functions *************************
	
	_obindps obindps;
	_obreak obreak;
	_ocan ocan;
	_oclose oclose;
	_ocof ocof;
	_ocom ocom;
	_ocon ocon;
	_odefinps odefinps;
	_odessp odessp;
	_odescr odescr;
	_oerhms oerhms;
	_oermsg oermsg;
	_oexec oexec;
	_oexfet oexfet;
	_oexn oexn;
	_ofen ofen;
	_ofetch ofetch;
	_oflng oflng;
	_ogetpi ogetpi;
	_oopt oopt;
	_opinit opinit;
	_olog olog;
	_ologof ologof;
	_oopen oopen;
	_oparse oparse;
	_orol orol;
	_osetpi osetpi;
	_sqlld2 sqlld2;
	_sqllda sqllda;
	_onbset onbset; 
	_onbtst onbtst; 
	_onbclr onbclr; 
	_ognfd ognfd;
	_obndra obndra;
	_obndrn obndrn;
	_obndrv obndrv;
	_odefin odefin;
	_oname oname;
	_orlon orlon;
	_olon olon;
	_osql3 osql3;
	_odsc odsc;
	
#endif // OCL_DYNAMIC
	
	///////////////  OraEnvironment  /////////////////
	
	const int OraEnvironment::MaxHomes = 6;
	CRMutex OraEnvironment::mLock;
	
#ifdef OCL_DYNAMIC
	OCIVersion OraEnvironment::mOCICallStyle = None;
#else
#ifdef OCL_OCI7
    OCIVersion OraEnvironment::mOCICallStyle = OCI73;
#endif
#ifdef OCL_OCI8
    OCIVersion OraEnvironment::mOCICallStyle = OCI80;
#endif
#endif
	
#ifdef OCL_DYNAMIC
	OCIVersion OraEnvironment::mOCIVersion = None;
#else
#if defined(OCL_OCI81) && !defined(OCL_OCI90)
	OCIVersion OraEnvironment::mOCIVersion = OCI81;
#endif
#ifdef OCL_OCI90
    OCIVersion OraEnvironment::mOCIVersion = OCI90;
#endif
#if !defined(OCL_OCI81) && !defined(OCL_OCI90)
    OCIVersion OraEnvironment::mOCIVersion = OCI80;
#endif
#endif
	
#ifdef OCL_DYNAMIC
#ifdef OCL_LINUX
	CRString OraEnvironment::mOCILibrary= "libclntsh.so";
#endif
	
#ifdef OCL_SOLARIS
	CRString OraEnvironment::mOCILibrary= "libclntsh.so";
#endif
#endif
	
	
	bool OraEnvironment::mOCIInited  = false;
	
#ifdef OCL_OCI8
	OCIEnv* OraEnvironment::hOCIEnv;
	OCIError* OraEnvironment::hOCIError;
#endif
	
	int OraEnvironment::mDefaultOraHome;
	int OraEnvironment::mOraHomeCount;
	int OraEnvironment::mActiveOraHome = -1;
	
	vector<CRString> OraEnvironment::mOraHomePaths(OraEnvironment::MaxHomes);
	vector<CRString> OraEnvironment::mOraHomeNames(OraEnvironment::MaxHomes);
	
#ifdef OCL_DYNAMIC
	bool OraEnvironment::mOCILoaded  = false;
	void* OraEnvironment::mLibHandle = 0;
	//Default OCI DLL name
#ifdef OCL_WIN32
	CRString OraEnvironment::mOCILibrary;
#endif
#else
	bool OraEnvironment::mOCILoaded  = true;
#endif
	
	unsigned OraEnvironment::mInitMode = OraEnvironment::imDefault;
	
	
	void OraEnvironment::setInitMode(InitMode mode) {
		FUNCTION_ENTRY("OraEnvironment::setInitMode");
		mInitMode = mode;
		FUNCTION_EXIT;
	}
	
	void OraEnvironment::initOCI() {
		FUNCTION_ENTRY("OraEnvironment::initOCI()");
		//CRMutex mutex(true);
        CRThreadGuard theGuard( mLock );

		if (mOCIInited)
		{
			FUNCTION_EXIT;
			return;
		}
		
#ifdef OCL_DYNAMIC
		if (!mOCILoaded)
			loadOCI();    
#endif
		
		switch (mOCICallStyle) {
#ifdef OCL_OCI7
		case OCI73:
			if (mInitMode & imThreaded)
				opinit(OCI_EV_TSF);
			else
				opinit(OCI_EV_DEF);
			break;
#endif
#ifdef OCL_OCI8        
		case OCI80:
			if (mOCIVersion == OCI80) {
				// Filter OCI80 init modes
				unsigned initMode = 0;
				
				if (mInitMode & imThreaded)
					initMode = OCI_THREADED;
				
				if (mInitMode & imObject)
					initMode |= OCI_OBJECT;
				
				check(OCIInitialize(initMode, 0, 0, 0, 0));
				
				// Clear init mode for OCIEnvInit
				initMode = 0;
				
				if (mInitMode & imThreadedUnmutexed)
					initMode = 0x80; // OCI_NO_MUTEX in OCI805 not declared
				
				check(OCIEnvInit(&hOCIEnv, initMode, 0, 0));
				check(OCIHandleAlloc(hOCIEnv, (void**)&hOCIError, OCI_HTYPE_ERROR, 0, 0));
			}
#ifdef OCL_OCI81
			else if (mOCIVersion >= OCI81) {
				unsigned initMode = 0;
				
				if (mInitMode & imThreaded)
					initMode = OCI_THREADED;
				
				if (mInitMode & imObject)
					initMode |= OCI_OBJECT;
				
				if (mInitMode & imThreadedUnmutexed)
					initMode |= OCI_THREADED | OCI_ENV_NO_MUTEX;
				
				if (mInitMode & imShared)
					initMode |= OCI_SHARED;
				
				if (mInitMode & imNoUserCallbacks)
					initMode |= OCI_NO_UCB;
				
				if (mInitMode & imEvents)
					initMode |= OCI_EVENTS;
				
				check(OCIEnvCreate((OCIEnv**)&hOCIEnv, initMode, 0, 0, 0, 0, 0, 0));
				check(OCIHandleAlloc(hOCIEnv, (void**)&hOCIError, OCI_HTYPE_ERROR, 0, 0));
			}
#endif
			break;
#endif
		}
		
		mOCIInited = true;
		FUNCTION_EXIT;
	}
	
	extern 	CRMutex mutexConnections;
	extern CRList<OraConnection> connections;
	
	void OraEnvironment::freeOCI() {
		
#ifdef OCL_OCI8
		if (!mOCIInited)
			return;
		
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
		
#ifdef OCL_OCI81
		if (mOCIVersion >= OCI81)
			OCITerminate(OCI_DEFAULT);
#endif
		
		if (hOCIError)
			OCIHandleFree(hOCIError, OCI_HTYPE_ERROR);
		
		if (hOCIEnv)
			OCIHandleFree(hOCIEnv, OCI_HTYPE_ENV);
		
		hOCIEnv   = 0;
		hOCIError = 0;
#endif
		
#ifdef OCL_DYNAMIC
		if (mLibHandle)
#ifdef OCL_WIN32
			FreeLibrary((HMODULE)mLibHandle);
#endif
#ifdef OCL_LINUX 
		dlclose(mLibHandle);
#endif
#ifdef OCL_SOLARIS
		dlclose(mLibHandle);
#endif
		mOCILoaded = false;
		mOCIVersion = None;
		mOCILibrary = "";
#endif
		
		mOCIInited = false;
	}
	
	bool OraEnvironment::isThreaded() {
		
		return mInitMode & imThreaded;
	}
	
	void OraEnvironment::setThreaded(bool value) {
		FUNCTION_ENTRY("OraEnvironment::setThreaded");
		value ? mInitMode |= imThreaded : mInitMode &= mInitMode^imThreaded;
		FUNCTION_EXIT;
	}
	
#ifdef OCL_OCI8
	bool OraEnvironment::isEvents() {
		
		return (mInitMode & imEvents) != 0;
	}
	
	void OraEnvironment::setEvents(bool value) {
		
		value ? mInitMode |= imEvents : mInitMode &= imEvents;
	}
	
	OCIEnv* OraEnvironment::getOCIEnv() {
		
		return hOCIEnv;
	}
	
	OCIError* OraEnvironment::getOCIError() {
		
		return hOCIError;
	}
#endif
	
	OCIVersion OraEnvironment::getOCIVersion() {
		
		return mOCIVersion;
	}
	
	OCIVersion OraEnvironment::OCICallStyle() {
		FUNCTION_ENTRY("OraEnvironment::OCICallStyle()");
		FUNCTION_EXIT;
		return mOCICallStyle;
	}
	
	void OraEnvironment::setOCICallStyle(OCIVersion value) {
		
		mOCICallStyle = value;
	}
	
	bool OraEnvironment::isOCIInited() {
		FUNCTION_ENTRY("OraEnvironment::isOCIInited()");
		FUNCTION_EXIT;
		return mOCIInited;
	}
	
#ifdef OCL_DYNAMIC
	bool OraEnvironment::isOCILoaded() {
		
		return mOCILoaded;
	}
	
	CRString OraEnvironment::OCILibrary() {
		
		return mOCILibrary;
	}
	
	void OraEnvironment::setOCILibrary(const CRString& name) {
		
		mOCILibrary = name;
	}
#endif
	
	int OraEnvironment::oracleHomeCount() {
		
		if (!mOraHomeCount)
			detectOCI();
		
		return mOraHomeCount;
	}
	
	int OraEnvironment::defaultOracleHome() {
		
		if (!mOraHomeCount)
			detectOCI();
		
		return mDefaultOraHome;
	}
	
	CRString OraEnvironment::oracleHomeName(int index) {
		
		if (!mOraHomeCount)
			detectOCI();
		
		if (index >= MaxHomes)
			throw CRException(InalidIndex);
		
		return mOraHomeNames[index];
	}
	
	CRString OraEnvironment::oracleHomePath(int index) {
		
		if (!mOraHomeCount)
			detectOCI();
		
		if (index >= MaxHomes)
			throw CRException(InalidIndex);
		
		return mOraHomePaths[index];
	}
	
	void OraEnvironment::setOracleHome(int index) {
		
		if (!mOraHomeCount)
			detectOCI();
		
		if (index >= mOraHomeCount)
			throw CRException(InalidIndex);
		
		mActiveOraHome = index;
	}
	
	void OraEnvironment::setOracleHome(const CRString& homeName) {
		
		if (!mOraHomeCount)
			detectOCI();
		
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
			
			setOracleHome(homeIndex);
	}
	
	void OraEnvironment::useDefaultOracleHome() {
		
		if (!mOraHomeCount)
			detectOCI();
		
		mActiveOraHome = -1;
#ifdef OCL_DYNAMIC
		mOCILibrary = "";
#endif
	}
	
	void OraEnvironment::resetOracleHomes() {
		
		mOraHomeCount = 0;
	}
	
#ifdef OCL_DYNAMIC
	void* OraEnvironment::getSymbol(void* dllHandle, const char* name) {
		void* symbol;
		
#ifdef OCL_WIN32
		symbol = GetProcAddress((HMODULE)dllHandle, name);
#endif
		
#ifdef OCL_LINUX
		symbol = dlsym(dllHandle, name);
#endif
		
#ifdef OCL_SOLARIS
		symbol = dlsym(dllHandle, name);
#endif
		
		if (symbol == 0)
			throw CRException("Can not find symbol.");
		
		return symbol;
	}
	
	void* OraEnvironment::loadDl(const CRString& name) {
		void* dllHandle;
		
#ifdef OCL_LINUX
		dllHandle = dlopen(name.c_str(), RTLD_LAZY);
#endif
		
#ifdef OCL_SOLARIS
		dllHandle = dlopen(name.c_str(), RTLD_LAZY);
#endif
		
#ifdef OCL_WIN32
		dllHandle = LoadLibrary(name);
#endif
		
		return dllHandle;
	}
	
#endif // OCL_DYNAMIC
	
	void OraEnvironment::detectOCI() {
		
#ifdef OCL_WIN32
		const int BUFFER_LENGTH = 256;
		char buf[BUFFER_LENGTH];
		
		// Search for all oracle homes, their names and paths
		HKEY hOraKey = 0;
		mOraHomeCount = 0; 
		unsigned long cntSubKey, MaxKeyLen, keyLen;
		
		mOraHomeNames.clear();
		mOraHomePaths.clear();
		
		// find 10g homes
		if ((ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\ORACLE", 0, KEY_READ, &hOraKey)) &&
			(ERROR_SUCCESS == RegQueryInfoKey(hOraKey, NULL, NULL, NULL, &cntSubKey, &MaxKeyLen, NULL, NULL, NULL, NULL, NULL, NULL))){
			
			MaxKeyLen++;
			char *key = new char[MaxKeyLen];
			memset(key, 0, MaxKeyLen);
			
			for (unsigned long i = 0, hc = 0; i < cntSubKey && hc < MaxHomes; ++i) {
				keyLen = MaxKeyLen;
				RegEnumKeyEx(hOraKey, i, key, &keyLen, NULL, NULL, NULL, NULL);
				if ((strstr(key, "HOME") != NULL) || 
					(strstr(key, "KEY_") != NULL)){
					if ( AddToOracleHomes(key) )
						hc++;
				}
			}
			delete [] key;
			RegCloseKey(hOraKey);
			hOraKey = 0;
			mOraHomeCount = mOraHomeNames.size();
			mDefaultOraHome = mOraHomeCount-1;
		}
		else
			throw CRException(InvalidHomeName);
		
		// Search for default Oracle home
		if (mOraHomeCount > 1) {
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\ORACLE\\ALL_HOMES",
				0, KEY_READ, &hOraKey) == ERROR_SUCCESS)
			{
				int bufLen = BUFFER_LENGTH;
				if (RegQueryValueEx(hOraKey, "DEFAULT_HOME", 0, 0, (LPBYTE)buf, (LPDWORD)&bufLen)
					== ERROR_SUCCESS)
				{
					buf[bufLen] = 0;
					
					for (int i = 0; i <= mDefaultOraHome; i++)
						if (mOraHomeNames[i] == buf) {
							mDefaultOraHome = i;
							break;
						}
				}
			}
		}
		
		// Close registry key
		if (hOraKey)
			RegCloseKey(hOraKey);
		
#endif // OCL_WIN32
	}
	
	bool
		OraEnvironment::AddToOracleHomes(const char *regKey){
		
		bool res = false;
#ifdef OCL_WIN32
		HKEY hOraReg;
		char HomeName[100];
		char HomePath[255];
		char keyBuf[255];
		unsigned long len, len1;
		
		strcpy(keyBuf, "SOFTWARE\\ORACLE\\"); 
		len = strlen(keyBuf);
		len1 = strlen(regKey);
		if (len + len1 >= sizeof(keyBuf))
			//  shouldn't happen ever
			return res;
		
		strncat(keyBuf, regKey, len1);
		keyBuf[sizeof(keyBuf)-1] = '\0';
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyBuf, 0, KEY_READ, &hOraReg)){
			len = 100;
			if (ERROR_SUCCESS == RegQueryValueEx(hOraReg, "ORACLE_HOME_NAME", NULL, NULL, (unsigned char*)&HomeName, &len)){
				len = 255;
				if (ERROR_SUCCESS == RegQueryValueEx(hOraReg, "ORACLE_HOME", NULL, NULL, (unsigned char*)&HomePath, &len)){
					mOraHomePaths.push_back(HomePath);
					//        OracleHomeKeys[OracleHomeCount - 1] := StrKey;
					mOraHomeNames.push_back(HomeName);
					res = true;
				}
			}
			RegCloseKey(hOraReg);
		}
#endif // OCL_WIN32
		return res;
	}
	
#ifdef OCL_DYNAMIC
	void OraEnvironment::loadOCI() {
		FUNCTION_ENTRY("OraEnvironment::loadOCI()");
		CRString ociLibName = mOCILibrary;
		
		// on Linux always "libclntsh.so"
		if (ociLibName.empty()) {
			detectOCI();
			
			// if oracle home not specified by the user
			if (mActiveOraHome == -1)
				// assume that default home not empty
				ociLibName = mOraHomePaths[mDefaultOraHome];
			else
				ociLibName = mOraHomePaths[mActiveOraHome];
			
			ociLibName += "\\bin\\oci.dll";
		}
		
		// try to load OCI library
		mLibHandle = loadDl(ociLibName);
#ifdef OCL_WIN32
		if (!mLibHandle) {
			//  check for instant client on windows platform
			if (mActiveOraHome == -1)
				// assume that default home not empty
				ociLibName = mOraHomePaths[mDefaultOraHome];
			else
				ociLibName = mOraHomePaths[mActiveOraHome];
			
			ociLibName += "\\oci.dll";
			mLibHandle = loadDl(ociLibName);
		}
#endif
		
#ifdef OCL_LINUX 
		if (mLibHandle) {
			if (mOCICallStyle == None)
				mOCICallStyle = OCI80;
			
			mOCIVersion = OCI80;
		}
#endif
		
#ifdef OCL_SOLARIS
		if (mLibHandle) {
			if (mOCICallStyle == None)
				mOCICallStyle = OCI80;
			
			mOCIVersion = OCI80;
		}
#endif
		
		// if load failed and OCI library was specified by the user
		// or user set home by hands throw exception
		if (!mLibHandle && (!mOCILibrary.empty() || mActiveOraHome != -1)) {
			mOCIVersion = None;
			throw CRException("Can not load " + ociLibName);
		}
		
		// if load failed try to load OCI library in PATH
		if (!mLibHandle) {
			ociLibName = "oci.dll";
			mLibHandle = loadDl(ociLibName);
			
			if (mLibHandle) {
				// if call style was specified early don't change it
				if (mOCICallStyle == None)
					mOCICallStyle = OCI80;
				
				// because exactly oci.dll version not known
				// set minimum subset of OCI8 functions
				mOCIVersion = OCI80;
			}
			else {
				ociLibName = "ociw32.dll";
				mLibHandle = loadDl(ociLibName);
				
				if (mLibHandle) {
					// set single possible values
					mOCICallStyle = OCI73;
					mOCIVersion = OCI73;
				}
				// all attempts to load OCI failed
				else {
					mOCILibrary = "";
					mOCIVersion = None;
					throw CRException("Can not load OCI library");
				}
			}
		}
		else {
			// if call style was specified early don't change it
			if (mOCICallStyle == None)
				mOCICallStyle = OCI80;
			
			// WAR: getOCIVersion may throw exception
			mOCIVersion = getOCIVersion(ociLibName);
		}
		
		// change OCI lib name with loaded lib name
		mOCILibrary = ociLibName;
		
		
		void* hDll = mLibHandle;
		
		switch (mOCICallStyle) {
#ifdef OCL_OCI8
		case OCI80:
			OCIInitialize              = (_OCIInitialize)getSymbol(hDll, "OCIInitialize");
			OCIHandleAlloc             = (_OCIHandleAlloc)getSymbol(hDll, "OCIHandleAlloc");
			OCIHandleFree              = (_OCIHandleFree)getSymbol(hDll, "OCIHandleFree");
			OCIDescriptorAlloc         = (_OCIDescriptorAlloc)getSymbol(hDll, "OCIDescriptorAlloc");
			OCIDescriptorFree          = (_OCIDescriptorFree)getSymbol(hDll, "OCIDescriptorFree");
			OCIEnvInit                 = (_OCIEnvInit)getSymbol(hDll, "OCIEnvInit");
			OCIServerAttach            = (_OCIServerAttach)getSymbol(hDll, "OCIServerAttach");
			OCIServerDetach            = (_OCIServerDetach)getSymbol(hDll, "OCIServerDetach");
			OCISessionBegin            = (_OCISessionBegin)getSymbol(hDll, "OCISessionBegin");    
			OCISessionEnd              = (_OCISessionEnd)getSymbol(hDll, "OCISessionEnd");
			OCIPasswordChange          = (_OCIPasswordChange)getSymbol(hDll, "OCIPasswordChange");;
			OCIServerVersion           = (_OCIServerVersion)getSymbol(hDll, "OCIServerVersion");
			OCISvcCtxToLda             = (_OCISvcCtxToLda)getSymbol(hDll, "OCISvcCtxToLda");
			OCIStmtPrepare             = (_OCIStmtPrepare)getSymbol(hDll, "OCIStmtPrepare");
			OCIBindByName              = (_OCIBindByName)getSymbol(hDll, "OCIBindByName");
			OCIBindArrayOfStruct       = (_OCIBindArrayOfStruct)getSymbol(hDll, "OCIBindArrayOfStruct");
			OCIBindDynamic             = (_OCIBindDynamic)getSymbol(hDll, "OCIBindDynamic");
			OCIStmtGetPieceInfo        = (_OCIStmtGetPieceInfo)getSymbol(hDll, "OCIStmtGetPieceInfo");
			OCIStmtSetPieceInfo        = (_OCIStmtSetPieceInfo)getSymbol(hDll, "OCIStmtSetPieceInfo");
			OCIStmtExecute             = (_OCIStmtExecute)getSymbol(hDll, "OCIStmtExecute");
			OCIDefineByPos             = (_OCIDefineByPos)getSymbol(hDll, "OCIDefineByPos");
			OCIDefineDynamic           = (_OCIDefineDynamic)getSymbol(hDll, "OCIDefineDynamic");
			OCIDefineArrayOfStruct     = (_OCIDefineArrayOfStruct)getSymbol(hDll, "OCIDefineArrayOfStruct");
			OCIStmtFetch               = (_OCIStmtFetch)getSymbol(hDll, "OCIStmtFetch");
			OCIDescribeAny             = (_OCIDescribeAny)getSymbol(hDll, "OCIDescribeAny");
			OCIParamGet                = (_OCIParamGet)getSymbol(hDll, "OCIParamGet");
			OCITransStart              = (_OCITransStart)getSymbol(hDll, "OCITransStart");
			OCITransPrepare            = (_OCITransPrepare)getSymbol(hDll, "OCITransPrepare");
			OCITransCommit             = (_OCITransCommit)getSymbol(hDll, "OCITransCommit");
			OCITransRollback           = (_OCITransRollback)getSymbol(hDll, "OCITransRollback");
			OCITransDetach             = (_OCITransDetach)getSymbol(hDll, "OCITransDetach");
			OCIErrorGet                = (_OCIErrorGet)getSymbol(hDll, "OCIErrorGet");
			OCILobDisableBuffering     = (_OCILobDisableBuffering)getSymbol(hDll, "OCILobDisableBuffering");
			OCILobEnableBuffering      = (_OCILobEnableBuffering)getSymbol(hDll, "OCILobEnableBuffering");
			OCILobFileClose            = (_OCILobFileClose)getSymbol(hDll, "OCILobFileClose");
			OCILobFileExists           = (_OCILobFileExists)getSymbol(hDll, "OCILobFileExists");
			OCILobFileIsOpen           = (_OCILobFileIsOpen)getSymbol(hDll, "OCILobFileIsOpen");
			OCILobFileOpen             = (_OCILobFileOpen)getSymbol(hDll, "OCILobFileOpen");
			OCILobFileSetName          = (_OCILobFileSetName)getSymbol(hDll, "OCILobFileSetName");
			OCILobFileGetName          = (_OCILobFileGetName)getSymbol(hDll, "OCILobFileGetName");
			OCILobGetLength            = (_OCILobGetLength)getSymbol(hDll, "OCILobGetLength");
			OCILobIsEqual              = (_OCILobIsEqual)getSymbol(hDll, "OCILobIsEqual");
			OCILobLocatorIsInit        = (_OCILobLocatorIsInit)getSymbol(hDll, "OCILobLocatorIsInit");
			OCILobRead                 = (_OCILobRead)getSymbol(hDll, "OCILobRead");
			OCILobTrim                 = (_OCILobTrim)getSymbol(hDll, "OCILobTrim");
			OCILobWrite                = (_OCILobWrite)getSymbol(hDll, "OCILobWrite");
			OCIBreak                   = (_OCIBreak)getSymbol(hDll, "OCIBreak");
			OCIAttrGet                 = (_OCIAttrGet)getSymbol(hDll, "OCIAttrGet");
			OCIAttrSet                 = (_OCIAttrSet)getSymbol(hDll, "OCIAttrSet");
			OCIExtProcGetEnv           = (_OCIExtProcGetEnv)getSymbol(hDll, "ociepgoe");
			OCIExtProcRaiseExcpWithMsg = (_OCIExtProcRaiseExcpWithMsg)getSymbol(hDll, "ociepmsg");
			OCIExtProcRaiseExcp        = (_OCIExtProcRaiseExcp)getSymbol(hDll, "ocieperr");
			
#ifdef OCL_OCI81
			if (mOCIVersion >= OCI81) {
				OCITerminate               = (_OCITerminate)getSymbol(hDll, "OCITerminate");
				OCIEnvCreate               = (_OCIEnvCreate)getSymbol(hDll, "OCIEnvCreate");
				OCILobIsOpen               = (_OCILobIsOpen)getSymbol(hDll, "OCILobIsOpen");
				OCILobOpen                 = (_OCILobOpen)getSymbol(hDll, "OCILobOpen");
				OCIDirPathFinish           = (_OCIDirPathFinish)getSymbol(hDll, "OCIDirPathFinish");
				OCIDirPathPrepare          = (_OCIDirPathPrepare)getSymbol(hDll, "OCIDirPathPrepare");
				OCIDirPathLoadStream       = (_OCIDirPathLoadStream)getSymbol(hDll, "OCIDirPathLoadStream");
				OCIDirPathColArrayEntrySet = (_OCIDirPathColArrayEntrySet)getSymbol(hDll, "OCIDirPathColArrayEntrySet");
				OCIDirPathColArrayReset    = (_OCIDirPathColArrayReset)getSymbol(hDll, "OCIDirPathColArrayReset");
				OCIDirPathColArrayToStream = (_OCIDirPathColArrayToStream)getSymbol(hDll, "OCIDirPathColArrayToStream");
				OCIDirPathStreamReset      = (_OCIDirPathStreamReset)getSymbol(hDll, "OCIDirPathStreamReset");
			}
#endif // OCL_OCI81
#ifdef OCL_OCI90
			if (mOCIVersion >= OCI90) {
				OCIDateTimeAssign            = (_OCIDateTimeAssign)getSymbol(hDll, "OCIDateTimeAssign");
				OCIDateTimeCheck             = (_OCIDateTimeCheck)getSymbol(hDll, "OCIDateTimeCheck");
				OCIDateTimeCompare           = (_OCIDateTimeCompare)getSymbol(hDll, "OCIDateTimeCompare");
				OCIDateTimeConstruct         = (_OCIDateTimeConstruct)getSymbol(hDll, "OCIDateTimeConstruct");
				OCIDateTimeConvert           = (_OCIDateTimeConvert)getSymbol(hDll, "OCIDateTimeConvert");
				OCIDateTimeFromArray         = (_OCIDateTimeFromArray)getSymbol(hDll, "OCIDateTimeFromArray");
				OCIDateTimeFromText          = (_OCIDateTimeFromText)getSymbol(hDll, "OCIDateTimeFromText");
				OCIDateTimeGetDate           = (_OCIDateTimeGetDate)getSymbol(hDll, "OCIDateTimeGetDate");
				OCIDateTimeGetTime           = (_OCIDateTimeGetTime)getSymbol(hDll, "OCIDateTimeGetTime");
				OCIDateTimeGetTimeZoneName   = (_OCIDateTimeGetTimeZoneName)getSymbol(hDll, "OCIDateTimeGetTimeZoneName");
				OCIDateTimeGetTimeZoneOffset = (_OCIDateTimeGetTimeZoneOffset)getSymbol(hDll, "OCIDateTimeGetTimeZoneOffset");
				OCIDateTimeIntervalAdd       = (_OCIDateTimeIntervalAdd)getSymbol(hDll, "OCIDateTimeIntervalAdd");
				OCIDateTimeIntervalSub       = (_OCIDateTimeIntervalSub)getSymbol(hDll, "OCIDateTimeIntervalSub");
				OCIDateTimeSubtract          = (_OCIDateTimeSubtract)getSymbol(hDll, "OCIDateTimeSubtract");
				OCIDateTimeSysTimeStamp      = (_OCIDateTimeSysTimeStamp)getSymbol(hDll, "OCIDateTimeSysTimeStamp");
				OCIDateTimeToArray           = (_OCIDateTimeToArray)getSymbol(hDll, "OCIDateTimeToArray");
				OCIDateTimeToText            = (_OCIDateTimeToText)getSymbol(hDll, "OCIDateTimeToText");
				
				OCIIntervalAdd          = (_OCIIntervalAdd)getSymbol(hDll, "OCIIntervalAdd");
				OCIIntervalAssign       = (_OCIIntervalAssign)getSymbol(hDll, "OCIIntervalAssign");
				OCIIntervalCheck        = (_OCIIntervalCheck)getSymbol(hDll, "OCIIntervalCheck");
				OCIIntervalCompare      = (_OCIIntervalCompare)getSymbol(hDll, "OCIIntervalCompare");
				OCIIntervalDivide       = (_OCIIntervalDivide)getSymbol(hDll, "OCIIntervalDivide");
				OCIIntervalFromNumber   = (_OCIIntervalFromNumber)getSymbol(hDll, "OCIIntervalFromNumber");
				OCIIntervalFromText     = (_OCIIntervalFromText)getSymbol(hDll, "OCIIntervalFromText");
				OCIIntervalFromTZ       = (_OCIIntervalFromTZ)getSymbol(hDll, "OCIIntervalFromTZ");
				OCIIntervalGetDaySecond = (_OCIIntervalGetDaySecond)getSymbol(hDll, "OCIIntervalGetDaySecond");
				OCIIntervalGetYearMonth = (_OCIIntervalGetYearMonth)getSymbol(hDll, "OCIIntervalGetYearMonth");
				OCIIntervalMultiply     = (_OCIIntervalMultiply)getSymbol(hDll, "OCIIntervalMultiply");
				OCIIntervalSetDaySecond = (_OCIIntervalSetDaySecond)getSymbol(hDll, "OCIIntervalSetDaySecond");
				OCIIntervalSetYearMonth = (_OCIIntervalSetYearMonth)getSymbol(hDll, "OCIIntervalSetYearMonth");
				OCIIntervalSubtract     = (_OCIIntervalSubtract)getSymbol(hDll, "OCIIntervalSubtract");
				OCIIntervalToNumber     = (_OCIIntervalToNumber)getSymbol(hDll, "OCIIntervalToNumber");
				OCIIntervalToText       = (_OCIIntervalToText)getSymbol(hDll, "OCIIntervalToText");
			}
#endif // OCL_OCI90
#endif // OCL_OCI8
    case OCI73:
		obindps  = (_obindps)getSymbol(hDll, "obindps");
		obreak   = (_obreak)getSymbol(hDll, "obreak");
		ocan     = (_ocan)getSymbol(hDll, "ocan");
		oclose   = (_oclose)getSymbol(hDll, "oclose");
		ocof     = (_oclose)getSymbol(hDll, "ocof");
		ocom     = (_ocom)getSymbol(hDll, "ocom");
		ocon     = (_ocon)getSymbol(hDll, "ocon");
		odefinps = (_odefinps)getSymbol(hDll, "odefinps");
		odessp   = (_odessp)getSymbol(hDll, "odessp");
		odescr   = (_odescr)getSymbol(hDll, "odescr");
		oerhms   = (_oerhms)getSymbol(hDll, "oerhms");
		oermsg   = (_oermsg)getSymbol(hDll, "oermsg");
		oexec    = (_oexec)getSymbol(hDll, "oexec");
		oexfet   = (_oexfet)getSymbol(hDll, "oexfet");
		oexn     = (_oexn)getSymbol(hDll, "oexn");
		ofen     = (_ofen)getSymbol(hDll, "ofen");
		ofetch   = (_ofetch)getSymbol(hDll, "ofetch");
		oflng    = (_oflng)getSymbol(hDll, "oflng");
		ogetpi   = (_ogetpi)getSymbol(hDll, "ogetpi");
		oopt     = (_oopt)getSymbol(hDll, "oopt");
		opinit   = (_opinit)getSymbol(hDll, "opinit");
		olog     = (_olog)getSymbol(hDll, "olog");
		ologof   = (_ologof)getSymbol(hDll, "ologof");
		oopen    = (_oopen)getSymbol(hDll, "oopen");
		oparse   = (_oparse)getSymbol(hDll, "oparse");
		orol     = (_orol)getSymbol(hDll, "orol");
		osetpi   = (_osetpi)getSymbol(hDll, "osetpi");
		onbset   = (_onbset)getSymbol(hDll, "onbset"); 
		onbtst   = (_onbtst)getSymbol(hDll, "onbtst"); 
		onbclr   = (_onbclr)getSymbol(hDll, "onbclr"); 
		ognfd    = (_ognfd)getSymbol(hDll, "ognfd");
		obndra   = (_obndra)getSymbol(hDll, "obndra");
		obndrn   = (_obndrn)getSymbol(hDll, "obndrn");
		obndrv   = (_obndrv)getSymbol(hDll, "obndrv");
		odefin   = (_odefin)getSymbol(hDll, "odefin");
		oname    = (_oname)getSymbol(hDll, "oname");
		orlon    = (_orlon)getSymbol(hDll, "orlon");
		olon     = (_olon)getSymbol(hDll, "olon");
		osql3    = (_osql3)getSymbol(hDll, "osql3");
		odsc     = (_odsc)getSymbol(hDll, "odsc");
  }
  
  mOCILoaded = true;
  FUNCTION_EXIT;
}

#endif //OCL_DYNAMIC

OCIVersion OraEnvironment::getOCIVersion(const CRString& fileName) { 
	
#ifdef OCL_WIN32
	HMODULE hDll = LoadLibrary("version.dll");
	
	_GetFileVersionInfoSizeA = (DWORD (__stdcall *)(LPSTR, LPDWORD))
		GetProcAddress(hDll, "GetFileVersionInfoSizeA");
	
	_GetFileVersionInfoA = (BOOL (__stdcall *)(LPSTR, DWORD, DWORD, LPVOID))
		GetProcAddress(hDll, "GetFileVersionInfoA");
	
	_VerQueryValueA  = (BOOL (__stdcall *)(const LPVOID, LPSTR, LPVOID*, PUINT))
		GetProcAddress(hDll, "VerQueryValueA");
	
	CRString version;  
	
	DWORD infoSize = _GetFileVersionInfoSizeA("oci.dll", 0);
	
	if (infoSize <= 0)
		throw CRException("Can not retrive OCI version");
	
	void* versionBuffer = new BYTE[infoSize];
	char query[256];
	char* fileVersion;
	
	if (_GetFileVersionInfoA("oci.dll", 0, infoSize, versionBuffer)) {
		LPVOID trns;
		UINT   len;
		if (_VerQueryValueA(versionBuffer, "\\VarFileInfo\\Translation", &trns, &len) && len >= 4)
			sprintf(query, "\\StringFileInfo\\%04x%04x\\FileVersion",
			LOWORD(*(DWORD*)trns), HIWORD(*(DWORD*)trns));
		else
			sprintf(query, "\\StringFileInfo\\%04x%04x\\FileVersion", 0x0409, 0x04B0);
		
		if (_VerQueryValueA(versionBuffer, query, (LPVOID*)&fileVersion, &len))
			version = fileVersion;
	}
	else
		throw CRException("Can not retrive OCI version");
	
	delete [] versionBuffer;
	
	FreeLibrary(hDll);
	
	if (version.find("7.3") == 0)
		return OCI73;
	else if (version.find("8.0") == 0)
		return OCI80;
	else if (version.find("8.1") == 0)
		return OCI81;
	else if (version.find("9.") == 0)
		return OCI90;
	else if (version.find("10.") == 0)
		return OCI100;
	else
		throw CRException(CRString("OCI version " + version + " is not supported").c_str());
	
#endif
	
#ifdef OCL_LINUX
	return OCI80;
#endif
	
#ifdef OCL_SOLARIS
	return OCI80;
#endif
	
}

void OraEnvironment::checkOCI80() {
	
	if (getOCIVersion() < OCI80)
		throw CRException(NeedOCI80Interface);
}

void OraEnvironment::checkOCI81() {
	
	if (getOCIVersion() < OCI81)
		throw CRException(NeedOCI81Interface);
}

void OraEnvironment::checkOCI90() {
	
	if (getOCIVersion() < OCI90)
		throw CRException(NeedOCI9Interface);
}

#ifdef OCL_OCI8
int OraEnvironment::getOraError(sword errorCode, CRString& errorMsg,
								OCIError* errorHandle)
{
	if (hOCIError)
		errorHandle = hOCIError;
	
	char msg[256];
	
	sb4 code;
	sword res;
	
	switch (errorCode) {
    case OCI_SUCCESS:
		return errorCode;
    case OCI_ERROR:
    case OCI_SUCCESS_WITH_INFO:
    case OCI_NO_DATA:
		res = OCIErrorGet(errorHandle, 1, 0, &code, (text*)msg, 256, OCI_HTYPE_ERROR);
		errorMsg = msg;
		
		switch (res) {
        case OCI_SUCCESS:
			if (errorMsg.empty()) {
				sprintf(msg, "ORA-%i", abs(code));
				errorMsg = msg;
			}
			break;
        case OCI_SUCCESS_WITH_INFO:
			errorMsg = "OCI_SUCCESS_WITH_INFO";
			break;
        case OCI_NO_DATA:
			errorMsg = "OCI_NO_DATA";
			break;
		}
		break;
		case OCI_INVALID_HANDLE:
			errorMsg = "OCI_INVALID_HANDLE";
			code = errorCode;
			break;
		case OCI_NEED_DATA:
			errorMsg = "OCI_NEED_DATA";
			code = errorCode;
			break;
		default:
			sprintf(msg, "Unknown error: ORA-%i", abs(errorCode));
			errorMsg = msg;
			code = errorCode;
	}
	
	return code;
}

void OraEnvironment::oraError(sword errorCode, OCIError* errorHandle) {
	FUNCTION_ENTRY("OraEnvironment::oraError");
	CRString msg;
	
	sword orgErrorCode = errorCode;
	errorCode = getOraError(errorCode, msg, errorHandle);
	
	if (orgErrorCode != OCI_SUCCESS_WITH_INFO)
		throw OraException(errorCode, msg);
	FUNCTION_EXIT;
}

void check(sword errorCode) {
	if (errorCode != OCI_SUCCESS)
		OraEnvironment::oraError(errorCode, OraEnvironment::hOCIError);
}
#endif // OCL_OCI8

} // namespace ocl
