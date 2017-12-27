/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\ora.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:22 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  Ora classes
//  Last modified:      22.03.00
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORA_H__
#define __ORA_H__

#include "crdef.h"
#include "crdb.h"
#ifdef OCL_PROF
#include "oraprof.h"
#else
#include "oracall.h"
#endif
#include "oramonitor.h"

namespace ocl {
	
	class OraException;
	class OraTransaction;
	class OraConnection;
	class OraCursor;
	class OraParam;
	class OraCommand;
	class OraField;
	class OraRecordset;
	class OraQuery;
	class OraLong;
	
#ifdef OCL_OCI8
	class OraLob;
	class OraFile;
#endif
	
#ifdef OCL_OCI90
	class OraTimeStamp;
	class OraInterval;
#endif
	
	
	//////////////////  Data types  /////////////////
	
	typedef CRDataType OraDataType;
	
	const OraDataType dtVarChar   = dtString;
	const OraDataType dtChar      = dtFixedString;
	
	const OraDataType dtCursor    = 20;
	const OraDataType dtRowId     = 21;
	const OraDataType dtBoolean   = 22;
	const OraDataType dtLabel     = 23;
	
	const OraDataType dtLong      = 24;
	const OraDataType dtLongRaw   = 25;
	const OraDataType dtBFile     = 26;
	const OraDataType dtCFile     = 27;
	
	const OraDataType dtObject    = 28;
	const OraDataType dtReference = 29;
	const OraDataType dtArray     = 30;
	const OraDataType dtTable     = 31;
	
	// Oracle 9i only
	const OraDataType dtTimeStamp    = 32; // TIMESTAMP
	const OraDataType dtTimeStampTZ  = 33; // TIMESTAMP WITH TIME ZONE
	const OraDataType dtTimeStampLTZ = 34; // TIMESTAMP WITH LOCAL TIME ZONE
	const OraDataType dtIntervalYM   = 35; // INTERVAL YEAR TO MONTH
	const OraDataType dtIntervalDS   = 36; // INTERVAL DAY TO SECOND
	
	///////////////  OraException  /////////////////
	
	class OraException : public CRException {
	public:
		OraException(const CRString& msg);
		OraException(int errorCode, const CRString& msg);
		
		int errorCode();
	private:
		int mErrorCode;
	};
	
	
	/////////////////  OraXID  /////////////////
	
	class OraXID
	{
	public:
		OraXID();
		OraXID(int fID, char gID[], int gIDLength, char bID[], int bIDLength);
		
		int formatID() const;
		void setFormaID(int fID);
		
		void globalTransactionID(char gID[], int length) const;
		void setGlobalTransactionID(char gID[], int length);
		
		void branchQualifier(char gID[], int gIDLength) const;
		void setBranchQualifier(char gID[], int gIDLength);
	};
	
	/////////////////  OraTransaction  /////////////////
	
#ifdef OCL_OCI8
	
	const int MaxGtrIdSize = 64;
	const int MaxBqualSize = 64;
	
	struct XID {
		long formatID;
		long gtridLength;
		long bqualLength;
		char data[MaxGtrIdSize + MaxBqualSize];
	};
	
	class OraTransaction {
	public:
		OraTransaction();
		OraTransaction(char gId[], int gIdLength);
		~OraTransaction();
		
		void setTimeout(int timeout);
		int timeout() const;
		
	private:
		OraTransaction(const OraTransaction&);
		
		void _init();
		
		void attachConnection(OraConnection* conn);
		void detachConnection(OraConnection* conn);
		
		void startTransaction(OraConnection* conn);
		void commit();
		void rollback();
		
		// Array of branch handles
		vector<OCITrans*> mOCITrans;
		
		// Array of attached connections
		vector<OraConnection*> mConnections;
		
		// XID of transaction
		XID mXid;
		
		// Name of the server where transaction started
		CRString mCoordName;
		
		// Timeout while detached transaction is alive
		int mTimeout;
		
		// For transId and branchId generating 
		static unsigned char mNextIdValue;
		
		friend class OraConnection;
	};
	
#endif // OCL_OCI8
	
	/////////////////  OraConnection  /////////////////
	
	enum CRIsolationLevel {ilReadCommited, ilReadOnly, ilSerializable};
	enum CRConnectMode { cmNormal, cmSysOper, cmSysDBA};
	
	class OraConnection {
	public:
		OraConnection();
		OraConnection(const CRString& connectString);
		OraConnection(const CRString& username, const CRString& password,
			const CRString& server);
		~OraConnection();
		
		static CRString home();
		static CRString homePath();
		static void setHome(int homeIndex);
		static void setHome(const CRString& homeName);
		static void setDefaultHome();
		
		CRString username() const { return mUsername; }
		void setUsername(const CRString& value);
		
		CRString password() const { return mPassword; }
		void setPassword(const CRString& value);
		
		CRString server() const { return mServer; }
		void setServer(const CRString& value);
		
		CRString serverVersion();
		
		void setConnectString(const CRString& connectString);
		
#ifdef OCL_OCI8
		void setConnectMode(CRConnectMode connectMode);
		CRConnectMode connectMode() const { return mConnectMode; }
#endif
		
		void open(const CRString& conectString = "");
		void close();
		bool isConnected() const { return mConnected; }
		
#ifdef OCL_OCI8
		void changePassword(const CRString& newPassword);
#endif
		
		// Transaction control
		
#ifdef OCL_OCI8 // Global transactions are supported only in OCI8
		OraTransaction* transaction() const;
		void setTransaction(OraTransaction* trans);
		
		CRString internalName() const;
		void setInternalName(const CRString& internalName);
		
		CRString externalName() const;
		void setExternalName(const CRString& externalName);
#endif
		
		void startTransaction();
		void startTransaction(CRIsolationLevel level);
		
		void commit();
		void rollback();
		void savepoint(const CRString& savepoint);
		void rollbackToSavepoint(const CRString& savepoint);
		
		bool inTransaction() const { return mInTransaction; }
		bool isAutoCommit() const { return mAutoCommit; }
		void setAutoCommit(bool value) { mAutoCommit = value; }
		
		CRIsolationLevel isolationLevel() const { return mIsolationLevel; }
		void setIsolationLevel(CRIsolationLevel value);
		
		OraCommand* createCommand();

        void addCommand( OraCommand* pCommand );
        void removeCommand( OraCommand* pCommand );
		
		void breakExec();
		
		LDA* getLDA();
		void setLDA(LDA* value);
		
#ifdef OCL_OCI8
		OCISvcCtx* getOCISvcCtx() const;
		void setOCISvcCtx(OCISvcCtx* value);
#endif
		
		OCIVersion OCICallStyle() const;
		void setOCICallStyle(OCIVersion value);
		
		static int getTNSAliases(CRString aliases[], int arrCount);
		
		CRString name() const { return mName; }
		void setName(const CRString& name) { mName = name; }
		
#ifdef OCL_DEPRECATED
		void connect(const char* connectString = "") { open(connectString); }
		void disconnect() { close(); }
#endif
		
	private:
		void checkOCI() const;
		void checkOCI73() const;
		void checkOCI80() const;
		
		void check(sword errorCode);
		void error(OCIVersion callStyle, sword errorCode);
		
		// Multi Thread
		void busy();
		void busyWait();
		void release();  
		
		void trace(OraMonitor::MonitorEvent monitorEvent,
			OraMonitor::TracePoint tracePoint, const string& errorMsg = "");
		
		void _init();
		
		static int mHome;
		
		bool mConnected;
		CRString mUsername;
		CRString mPassword;
		CRString mServer;
		
		//add for fixing DB Fail over and connection leak 
		bool mIsAttachedServer;
		
		LDA* mLDA;
		HDA* mHDA;
		
#ifdef OCL_OCI8
		OCISvcCtx*  hOCISvcCtx;
		OCIServer*  hOCIServer;
		OCISession* hOCISession;
		CRConnectMode mConnectMode;
#endif
		
		bool mNativeSvcCtx;
		bool mThreadSafety;
		bool mAutoCommit;
		bool mInTransaction;
		word mCharsetId;
		CRIsolationLevel mIsolationLevel;
		
#ifdef OCL_DYNAMIC
		OCIVersion mOCICallStyle;
#else
		static OCIVersion mOCICallStyle;
#endif
		
		OCIVersion mOCIVersion;
        CRMutex mCommandsMutex;
		CRList<OraCommand> mCommands;
		
#ifdef OCL_OCI8
		OraTransaction* mTransaction;
#endif
		
		// For global transaction
		CRString mInternalName;
		CRString mExternalName;
		
#ifdef OCL_WIN32
		//mutex for thread synchronization
		HANDLE hMutex;
#endif
		
		CRString mName;
		
		friend class OraCommand;
		friend class OraRecordset;
		friend class OraTransaction;
		
		
};


/////////////////  OraCursor  //////////////////

enum CRCursorState {csInactive, csOpen, csParsed, csPrepared, csBound,
csExecuting, csExecuted, csFetching, csFetchingAll, csCanceled};

class OraCursor : public CRSharedObject {
public:
	virtual ~OraCursor();
	
	void allocCursor();
	void freeCursor();
	
	bool canFetch() const;
	
	CRCursorState state() const;
	
	CDA* getCDA();
	
#ifdef OCL_OCI8
	OCIStmt* getOCIStmt();
	OCIStmt** getOCIStmtPtr() { return &hOCIStmt; }
#endif
	
protected:
	OraCursor();  
	
private:
	
#ifdef OCL_OCI7
	CDA* mCDA;
#endif
	
#ifdef OCL_OCI8
	OCIStmt* hOCIStmt;
#endif
	
	CRCursorState mState;
	
#ifdef OCL_DYNAMIC
	OCIVersion mOCICallStyle;
#else
	static OCIVersion mOCICallStyle;
#endif  
	
	void checkOCI() const;
	void checkOCI73() const;
	void checkOCI80() const;
	
	OCIVersion OCICallStyle() const;
	void setOCICallStyle(OCIVersion OCICallStyle);
	
	friend class OraCommand;
	friend class OraRecordset;
	friend class OraParam;
};

//////////////////  COraParam  /////////////////////

#ifdef OCL_DEPRECATED
enum TCRParamType {ptUnknown, ptInput, ptOutput, ptInputOutput};  
#endif

enum CRParamDirection {pdUnknown, pdInput, pdOutput, pdInputOutput};  

class OraParam {
public:
	OraParam();
	~OraParam();
	
	CRString name() const;
	void setName(const CRString& value);
	CRDataType dataType() const;
	void setDataType(CRDataType value);
	CRParamDirection direction() const { return mDirection; }
	void setDirection(CRParamDirection value);
	int size() const;
	void setSize(int value);
	bool isTable() const;
	void setTable(bool value);
	int tableLength() const;
	void setTableLength(int value);  // Length of table
	
	bool isNull(int index = 1);
	void setNull(int index = 1);
	
	CRDate getDate(int index = 1);
	void setDate(const CRDate& value);
	void setDate(int index, const CRDate& value);
	
	float getFloat(int index = 1);
	void setFloat(float value);
	
	double getDouble(int index = 1);
	void setDouble(double value);
	void setDouble(int index, double value);
	
	int getInt(int index = 1);
	void setInt(int value);
	void setInt(int index, int value);
	
	bool getBoolean(int index = 1);
	void setBoolean(bool value);
	void setBoolean(int index, bool value);
	
	const char* getChars(int index = 1);
	char* getChars(char* result);
	char* getChars(int index, char* result);
	CRString getString(int index = 1);
	
	void setChars(const char* value);
	void setChars(int index, const char* value);
	void setString(const CRString& value);
	void setString(int index, const CRString& value);
	
	CRLob& getLob();
	void setLob(CRLob& value);
	
	OraLong& getLong();
	void setLong(OraLong& value);
	
	unsigned char* getRaw(unsigned char* result);
	unsigned char* getRaw(int index, unsigned char* result);
	
	void setRaw(unsigned char* value, int length);
	void setRaw(int index, unsigned char* value, int length);
	
	OraLong& getLongRaw();
	void setLongRaw(OraLong& value);
	
	OraCursor& getCursor();
	void setCursor(OraCursor& value);
	
#ifdef OCL_OCI8
	OraLob& getBlob();
	void setBlob(OraLob& value);
	
	OraLob& getClob();
	void setClob(OraLob& value);
	
	OraFile& getBFile();
	void setBFile(OraFile& value);
#endif
	
#ifdef OCL_OCI90
	OraTimeStamp& getTimeStamp();
	void setTimeStamp(const OraTimeStamp& value);
	
	OraTimeStamp& getTimeStampTZ();
	void setTimeStampTZ(const OraTimeStamp& value);
	
	OraTimeStamp& getTimeStampLTZ();
	void setTimeStampLTZ(const OraTimeStamp& value);
	
	OraInterval& getIntervalYM();
	void setIntervalYM(const OraInterval& value);
	
	OraInterval& getIntervalDS();
	void setIntervalDS(const OraInterval& value);
#endif
	
	CRValue getValue(int index = 1);
	void setValue(const CRValue& value);
	void setValue(int index, const CRValue& value);
	
	void assign(const CRDate& value) { setDate(value); };
	void assign(int value) { setInt(value); };
	void assign(double value) { setDouble(value); };
	void assign(const char* value) { setString(value); };
	void assign(const CRString& value) { setString(value); };
	
	void assign(OraParam& value);
	
	void operator =(const CRValue& value);
	
#ifdef OCL_DEPRECATED
	TCRParamType paramType() { return (TCRParamType)direction(); }
	void setParamType(TCRParamType type) { setDirection((CRParamDirection)type); }
	int length() const { return tableLength(); }
	void setLength(int value) { setTableLength(value); }
	CRLob& getMemo() { return (CRLob&)getLong(); }
	void setMemo(CRLob& value) { setLong((OraLong&)value); }
#endif  
	
protected:
	void reallocBuffers(int newDataSize, int newTableLength);
	void freeBuffers();
	
	void createValue();
	void freeValue();
	void initValue();
	
	void* valuePtr();
	void* indicatorPtr();
	
	short indicator(int index) const;
	void setIndicator(int index, short value);  
	
	void refreshIndicator();
	
	void disconnectObject();
	
	void checkRange(int index) const;
	
private:
	CRString mName;
	CRDataType mDataType;
	CRDataType mSubDataType;
	int mDataSize;
	void* mValue;
	sb2 mDefIndicator;
	sb2* mIndicator;
	bool mTable;
	long mTableLength;  // table length  
	ub4 mActualLength;
	CRParamDirection mDirection;
	char mBuf[bufSize];
	void* mHandle;
	int mBlobPieceCount;
	
	friend class OraCommand;
	friend class OraRecordset;
	friend class CRNamedList<OraParam>;
};


/////////////////  OraParams  //////////////////

typedef CRNamedList<OraParam> OraParams;

/////////////////  OraCommand  /////////////////////

enum CRCommandType {ctSQL, ctTable, ctStoredProc};

class OraCommand {
public:
	OraCommand();
	OraCommand(OraConnection& connection);
	OraCommand(OraConnection& connection, const CRString& sql);
	virtual ~OraCommand();
	
	// Open/close
	void prepare();
	void unprepare();
	bool isPrepared() const;
	
	// Executing
	int execute(int iters = 1);
	int execute(OraRecordset** rs, bool fieldsInfoOnly = false);
	OraRecordset& executeQuery();
	void cancel();
	
	// Params
	void bindParams();
	
	int paramCount();
	OraParams& params();
	
	OraParam& param(int index);
	OraParam& param(const CRString& name);
	OraParam& param(const char* name);
	OraParam* findParam(int index);
	OraParam* findParam(const CRString& name);
	
	// Info
	int SQLType() const;
	bool isQuery() const;
	bool isPLSQL() const;
	
	//is SQL statement can rows return
	bool isRowsReturn() const;
	
	const char* rowId() const;
	int rowsProcessed() const;
	
	void setArrayLength(int newLength);
	
	OraConnection* connection() const;
	void setConnection(OraConnection& value);
	void setConnection(OraConnection* value);
	
	CRString commandText() const { return mCommandText; }
	void setCommandText(const CRString& value);
	
	CRCommandType commandType() const { return mCommandType; }
	void setCommandType(CRCommandType value);  
	
	CRString SQL() const;
	void setSQL(const CRString& value);
	
	OraCursor* cursor() const;
	void setCursor(OraCursor &value);  
	CRCursorState cursorState() const;
	
	OraParam& operator [](int index) {  return param(index); };
	OraParam& operator [](const CRString& name) {  return param(name); };
	
	int errorOffeset() const;
	
	// for SQL Monitoring purpose
	CRString name() const { return mName; }
	void setName(const CRString& name) { mName = name; }
	
#ifdef OCL_DEPRECATED
	void createProcCall(const char* name);
	OraConnection* session() { return mConnection; }
	void setSession(OraConnection& session) { setConnection(session); }
	void setSession(OraConnection* session) { setConnection(session); }
#endif
	
	
protected:
	OraConnection* mConnection;
	
	void checkOCI() const;
	void checkOCI73() const;
	void checkOCI80() const;
	
	OCIVersion OCICallStyle() const;
	void setOCICallStyle(OCIVersion value);  
	
	void check(sword status) const;
	void checkSession() const;
	void checkActive() const;
	void checkActiveRecordset() const;
	void checkInactive() const;
	
	int getOraType(int dataType, int subDataType);
	
	void setCursor(OraCursor* value);
	// Stored proc
	void generateProcCall(const char* name);
	
	void checkParams() { createSQL(); }
	void createSQL();
	
#ifdef OCL_OCI7
	void internalOpen();
	void internalParse();
	void internalClose();
	void internalCancel();  
#endif
	
#ifdef OCL_OCI8
	void internalPrepare();
	void defineDynamic(OraField* field, void* owner, OCICallbackDefine proc);
	
	sb4 callbackInBind(OCIBind* bind, ub4 iter, ub4 index,
		void** buf, ub4* bufLen, ub1* pieceStatus, void** ind);
	
	sb4 callbackOutBind(OCIBind* bind, ub4 iter, ub4 index,
		void** buf, ub4** bufLen, ub1* pieceStatus, void** ind);
#endif
	
	void bindParam(OraParam* param);
	sword internalExecute(int mode);
	void exec();
	int getFieldDesc(int fieldNo, OraField* field, bool longString);
	void defineData(OraField* field, void* buf, sb2* ind);
	
	void defineArrayData(OraField* field, void* buf, sb2* ind, int bufSkip, int indSkip);
	void definePieceData(OraField* field, void* buf, sb2* ind);
	int internalFetch(int rows);
	int internalFetchPiece();
	
	// out parameter buf valid only for OCI7 and in OCI8 may be NULL
	void getPI(void** handle, byte* piece, void** buf, long* iteration, long* index, CRParamDirection* mode);
	void setPI(void* handle, long hType, byte piece, void* buf, long* bufLen, sb2* ind);
	
	void initProcParams(const char* name, int overload);
	void initProcParams7(const char* name, int overload);
#ifdef OCL_OCI8
	void initProcParams8(const char* name, int overload);
#endif
	
	// Executing
	void doExecute();
	void endExecute(bool result);
	
	// Params
	void scanParams();
	
	// Multi Thread
	void busy();
	void release();
	
	bool isActive() const;
	
	bool nativeCursor();
	void setCursorState(CRCursorState cursorState);
	
	int rowsReturn();
	void checkRowsReturn();
	
	virtual void disconnect();
	
	void trace(OraMonitor::MonitorEvent monitorEvent, OraMonitor::TracePoint tracePoint);
	
	bool mImplicitPrepared;  
	CRString mName;
	
private:
	OraCursor* mCursor;
	OraCursor* mCursorRef;
	OraRecordset* mRecordset;
	CRString mSQL;
	CRString mCommandText;
	CRCommandType mCommandType;
	OraParams mParams;
	
#ifdef OCL_DYNAMIC
	OCIVersion mOCICallStyle;
#else
	static OCIVersion mOCICallStyle;
#endif
	
	word mSQLType;
	bool mNonBlocking;
	bool mAutoCommit;
	int mErrorOffset;
	int mRowsProcessed;
	int mFetchedRows;
	int mItersCount;
	
	void _init();
	
#ifdef OCL_OCI8
	
	friend sb4 callbackInBind(dvoid* ictxp, OCIBind* bindp, ub4 iter,
		ub4 index, dvoid** bufpp, ub4* alenp, ub1* piecep, dvoid** indpp);
	
	friend sb4 callbackOutBind(dvoid* octxp, OCIBind* bindp, ub4 iter, ub4 index, 
		dvoid** bufpp, ub4** alenpp, ub1* piecep, dvoid** indpp, ub2** rcodepp);
	
#endif // OCL_OCI8
	
	friend class OraRecordset;
	friend class OraConnection;
};


/////////////////  OraField  ///////////////////

class OraField : public CRField {
public:
	OraField();
	virtual ~OraField();
	
	virtual int size();
	
	virtual const char* getChars();
	virtual char* getChars(char* result);
	virtual char* getChars(char* result, int length);
	virtual CRString getString();
	
	virtual bool isLob() const;
	
	bool isUnique() const { return mIsUnique; };
	bool isKey() const { return mIsKey; };
	bool isExpression() const { return mIsExpression; };
	CRString schemaName() const { return  mSchemaName; };
	CRString tableName() const { return mTableName; };
	CRString originalName() const { return mOriginalName; };
	CRString expression() const { return mExpression; };
	
	virtual CRLob& getLob();
	virtual void setLob(const CRLob& value);
	
	OraLong& getLong();
	void setLong(OraLong& value);
	
	inline unsigned char* getRaw(unsigned char* result);
	inline void setRaw(unsigned char* value, int length);
	
	OraLong& getLongRaw();
	void setLongRaw(OraLong& value);
	
#ifdef OCL_OCI8  
	OraLob* getBlob();
	void setBlob(const OraLob& value);
	
	OraLob* getClob();
	void setClob(const OraLob& value);  
	
	OraFile& getBFile();
	void setBFile(const OraFile& value);
#endif
	
#ifdef OCL_OCI90
	const OraTimeStamp& getTimeStamp();
	void setTimeStamp(const OraTimeStamp& value);
	
	const OraInterval& getInterval();
	void setInterval(const OraInterval& value);
#endif
	
protected:  
	void* mHandle;
	
	bool mIsUnique;
	bool mIsKey;
	bool mIsExpression;
	CRString mSchemaName;
	CRString mTableName;
	CRString mOriginalName;
	CRString mExpression;
	
	void* handle() const { return mHandle; }
	void  setHandle(void* handle) { mHandle = handle;}
	
	virtual bool isComplex() const;
	virtual bool hasParent() const;
	
	friend class OraCommand;
	friend class OraRecordset;
};


////////////////  OraRecordset  //////////////////

class OraRecordset : public CRMemDataset {
public:
	virtual ~OraRecordset();
	
	// Open / close
	bool isActive() const { return CRDataset::isActive(); }
	void close();
	
	// Fetch
	void fetchAll();
	
	int fetchRows() const;
	void setFetchRows(int value);
	bool isCached() const;
	void setCached(bool value);
	
	void setLongString(bool newValue);
	bool longString() const;
	
	void getFieldsInfo();  
	
	OraField& field(int index) const;
	OraField& field(const CRString& name) const;
	OraField* findField(int index) const;
	OraField* findField(const CRString& name) const;
	
	OraField& operator [](int index) const { return (OraField&)field(index); }
	OraField& operator [](const CRString& name) const { return (OraField&)field(name); }
	
protected:
	OraRecordset(const OraRecordset&);
	OraRecordset(OraCommand& command);
	
	void prepare();
	void unprepare();
	void open();  
	
	// Items / data
	virtual void prepareData();
	
	// Open / close
	virtual void internalOpen();
	virtual void internalClose();
	
	virtual void internalRefresh();
	
	bool rowsReturn();
	void checkRowsReturn();
	
	// Fields
	virtual void internalInitFields();
	
	virtual void getDateFromBuf(void* buf, void* date);
	virtual void putDateToBuf(void* buf, void* date);
	
	virtual void createComplexFields(void* recBuf, bool withBlob = true);
	virtual void freeComplexFields(void* recBuf, bool withBlob = true);
	
	// Records / buffers
	virtual void getNextRecord(void* recBuf);
	virtual void getPrevRecord(void* recBuf);
	
	// Navigation
	virtual void internalLast();
	
	// Fetch
	bool fetch();
	bool fetchPiece();
	
	void doExecFetch();
	void endExecFetch(bool result);
	void execFetch();
	
	void doFetchAll();
	void doFetchAllPulse();
	void endFetchAll(bool result);
	
	bool fetchArray();    
	void initBlock(BlockHeader* block);
	void prepareBlock(BlockHeader* block, int fetched);
	void clearBlock(BlockHeader* block);
	
	virtual void disconnectFromSession();
	
#ifdef OCL_OCI8  
	sb2 callbackDefine(OCIDefine* definp, ub4 iter,
		void** bufp, ub4** alenpp, ub1* piecep, void** indpp);      
	
	friend sb4 callbackDefine(void* octxp, OCIDefine* definp, ub4 iter,
		void** bufp, ub4** alenpp, ub1* piecep, void** indpp, ub2** rcodepp);
#endif
	
private:
	OraCursor* mFetchCursor;
	ItemHeader* mFetchItem;
	bool mFetchAll;
	int mFetchRows;
	bool mCached;
	void* mFetchBlock;
	int mFetchBlockItemSize;
	bool mPieceFetch;
	bool mLongString;
	bool mAutoClose;
	void* mFetchItems;
	OraCommand& mCommand;
	
	void _init();
	
	void allocFetchBlock();
	void freeFetchBlock();
	
	friend class OraField;
	friend class OraConnection;
	friend class OraCommand;
	friend class OraQuery;
};


//inline functions

inline OraField& OraRecordset::field(int index) const {
	
	return (OraField&)CRDataset::field(index);
}

inline OraField& OraRecordset::field(const CRString& name) const {
	
	return (OraField&)CRDataset::field(name);
}

inline OraField* OraRecordset::findField(int index) const {
	
	return (OraField*)CRDataset::findField(index);
}

inline OraField* OraRecordset::findField(const CRString& name) const {
	
	return (OraField*)CRDataset::findField(name);
}


//////////////////  OraQuery  ///////////////////

class OraQuery : public OraRecordset, public OraCommand {
public:
	OraQuery();
	OraQuery(OraConnection& connection);
	OraQuery(OraConnection& connection, const CRString& sql);
	virtual ~OraQuery();
	
	// Open / close
	void prepare();
	void unprepare();
	
	void open();
	void close();
	
	bool isActive() const;
	
	int execute();
	
	OraField& operator [](int index) const { return (OraField&)OraRecordset::field(index); }
	OraField& operator [](const CRString& name) const { return (OraField&)OraRecordset::field(name); }
	
private:
	void _init();
	
	bool mImplicitPrepared;
};


#ifdef OCL_OCI90
class OraTimeStamp {
public:
	OraTimeStamp();
	
	OraTimeStamp(const OraTimeStamp& src);
	
	OraTimeStamp(short year, unsigned char month, unsigned char day,
		unsigned char hour, unsigned char min, unsigned char sec, unsigned fsec,
		const CRString& timeZone = "");
	
	OraTimeStamp& operator=(const OraTimeStamp& r);
	
	~OraTimeStamp();
	
	OraDataType timeStampType() const;
	void setTimeStampType(OraDataType type);
	
	void getDate(short& year, unsigned char& month, unsigned char& day) const;
	void setDate(short year, unsigned char month, unsigned char day);
	
	void getTime(unsigned char& hour, unsigned char& min,
		unsigned char& sec, unsigned& fsec) const;
	void setTime(unsigned char hour, unsigned char min,
		unsigned char sec, unsigned fsec);
    
	void timeZoneOffset(signed char& hour, signed char& min) const;
	void setTimeZoneOffset(signed char hour, signed char min);
	
	CRString timeZoneName() const;
	
	CRString toString(const CRString& format = "", unsigned char fsprecision = 6) const;
	void fromString(const CRString& value, const CRString& format = "");
	
	void setSysTimeStamp();
	
	bool isNull() const;
	void setNull();
	
	int compare(const OraTimeStamp& value) const;
	
	OraTimeStamp& operator+(const OraInterval& value);
	OraTimeStamp& operator-(const OraInterval& value);
	
	friend OraInterval operator-(const OraTimeStamp &l, const OraTimeStamp &r);
	
	friend bool operator==(const OraTimeStamp &l, const OraTimeStamp &r);
	friend bool operator>(const OraTimeStamp &l, const OraTimeStamp &r);
	friend bool operator<(const OraTimeStamp &l, const OraTimeStamp &r);
	friend bool operator!=(const OraTimeStamp &l, const OraTimeStamp &r);
	friend bool operator>=(const OraTimeStamp &l, const OraTimeStamp &r);
	friend bool operator<=(const OraTimeStamp &l, const OraTimeStamp &r);
	
private:
	OraTimeStamp(OraDataType type);
	void alloc();
	void free();
	
	OCIDateTime* getOCIDateTime() const;
	OCIDateTime** getOCIDateTimePtr();
	
	void checkValid() const;
	void checkAlloc() const;
	
	OCIDateTime* hOCIDateTime;
	unsigned mType; // OCI descriptor type
	bool mIsNull;
	
	friend class OraParam;
	friend class OraCommand;
	friend class OraRecordset;
};

OraInterval operator-(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator==(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator>(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator<(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator!=(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator>=(const OraTimeStamp &l, const OraTimeStamp &r);
bool operator<=(const OraTimeStamp &l, const OraTimeStamp &r);

class OraInterval {
public:
	// Create INTERVAL without type
	OraInterval();
	// Create dtIntervalYM type
	OraInterval(int year, int month);
	// Create dtIntervalDS type
	OraInterval(int day, int hour, int minute, int second, int fsecond);
	
	OraInterval(const OraInterval& src);
	OraInterval& operator=(const OraInterval& r);
	
	~OraInterval();
	
	OraDataType intervalType() const;
	void setIntervalType(OraDataType type);
	
	void setYearMonth(int year, int month);
	void setDaySecond(int day, int hour, int minute, int second, int fsecond);
	
	int year() const;
	int month() const;
	int day() const;
	int hour() const;
	int minute() const;
	int second() const;
	int fracSecond() const;
	
	CRString toString(unsigned char lfprec = 0, unsigned char fsprec = 6) const;
	void fromString(const CRString& val);
	
	bool isNull() const;
	void setNull();
	
	int compare(const OraInterval& value) const;
	
	OraInterval& operator+=(const OraInterval& r);
	OraInterval& operator-=(const OraInterval& r);
	
	friend OraInterval operator+(const OraInterval& l, const OraInterval& r);
	friend OraInterval operator-(const OraInterval& l, const OraInterval& r);
	
	friend bool operator==(const OraInterval& l, const OraInterval& r);
	friend bool operator>(const OraInterval& l, const OraInterval& r);
	friend bool operator<(const OraInterval& l, const OraInterval& r);
	friend bool operator!=(const OraInterval& l, const OraInterval& r);
	friend bool operator>=(const OraInterval& l, const OraInterval& r);
	friend bool operator<=(const OraInterval& l, const OraInterval& r);
	
private:
	OraInterval(OraDataType type);
	void alloc();
	void free();
	
	OCIInterval* getOCIInterval() const;
	OCIInterval** getOCIIntervalPtr();
	
	void checkValid() const;
	void checkAlloc() const;
	
	OCIInterval* hOCIInterval; // OCI descriptor type
	unsigned mType;
	bool mIsNull;
	
	friend OraInterval operator-(const OraTimeStamp &l, const OraTimeStamp &r);
	friend class OraTimeStamp;
	friend class OraParam;
	friend class OraCommand;
	friend class OraRecordset;
};

OraInterval operator+(const OraInterval& l, const OraInterval& r);
OraInterval operator-(const OraInterval& l, const OraInterval& r);
bool operator==(const OraInterval& l, const OraInterval& r);
bool operator>(const OraInterval& l, const OraInterval& r);
bool operator<(const OraInterval& l, const OraInterval& r);
bool operator!=(const OraInterval& l, const OraInterval& r);
bool operator>=(const OraInterval& l, const OraInterval& r);
bool operator<=(const OraInterval& l, const OraInterval& r);

#endif // OCL_OCI90


//////////////////  OraLong  ///////////////////

class OraLong : public CRLob {
public:
	virtual ~OraLong() {}
	
protected:
	OraLong() {}
	
	friend class OraCommand;
	friend class OraRecordset;
	friend class OraParam;
};


#ifdef OCL_OCI8 

//////////////////  OraLob  ///////////////////

//global callback define function
sb4 callbackDefine(void* octxp, OCIDefine* definp, ub4 iter,
                   void** bufp, ub4** alenpp, ub1* piecep, void** indpp, ub2** rcodepp);

class OraLob : public CRLob {
public:
	virtual ~OraLob();
	
	virtual void allocLob();
	virtual void freeLob();
	
	void enableBuffering();
	void disableBuffering();
	
	void init();
	bool isInited();
	
	unsigned int length();
	
	void readLob();
	void writeLob();
	
	OCISvcCtx* getOCISvcCtx() const;
	void setOCISvcCtx(OCISvcCtx* hSvcCtx);
	
	OCILobLocator* getOCILobLocator() const;
	
protected:
	OraLob();
	OraLob(OraConnection& connection);
	
	void checkInit();
	void checkAlloc();
	void checkSession();
	
	OCILobLocator** getOCILobLocatorPtr() { return &hOCILobLocator; }  
	
	OCILobLocator* hOCILobLocator;
	OCISvcCtx*     hOCISvcCtx;
	
	friend class OraCommand;
	friend class OraRecordset;
	friend class OraParam;
};


//////////////////  OraFile  ///////////////////

class OraFile : public OraLob {
public:
	virtual ~OraFile();
	
	virtual void allocLob();
	virtual void freeLob();
	
	void open();
	void close();
	
	void refresh();
	
	bool isOpen();
	bool exist();
	
	CRString fileDir() const;
	void setFileDir(const CRString& value);
	
	CRString fileName() const;
	void setFileName(const CRString& value);
	
protected:
	OraFile();
	OraFile(OraConnection& connection);
	
	CRString mFileName;
	CRString mFileDir;
	
	friend class OraParam;
	friend class OraRecordset;
};

#endif // OCL_OCI8

#ifdef OCL_DEPRECATED
typedef OraException COraException;
typedef OraConnection COraSession;
typedef OraCursor COraCursor;
typedef OraParam COraParam;
typedef OraParams COraParams;
typedef OraCommand COraSQL;
typedef OraField COraField;
typedef OraQuery COraDataset;
#ifdef OCL_OCI8
typedef OraLob COraLob;
typedef OraFile COraFile;
#endif
#endif


struct ColumnInfo {
	CRString name;
	CRString table;
	CRString schema; 
	CRString expr;
	CRString alias;
};

struct TableInfo {
	CRString name;
	CRString alias;
};

int getColumnsInfo(const CRString& sql, ColumnInfo*);
int getTablesInfo(const CRString& sql, TableInfo*);

} // namespace ocl

#endif // __ORA_H__
