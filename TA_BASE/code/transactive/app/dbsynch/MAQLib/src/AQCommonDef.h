//////////////////////////////////////////////////////////////////////
///		@file		AQCommonDef.h
///		@author		zhilin,ouyang
///		@date		2012-08-25 10:44:41
///
///		@brief	    to define some common use macro or constant static 
///                 variables used in the AQ library.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  AQCOMMONDEF_H
#define  AQCOMMONDEF_H

// easy usage of the namespace identifier
#define  NS_BEGIN(name) namespace name {
#define  NS_END(name)   };


NS_BEGIN(TA_AQ_Lib)


///////////////////////////////////////////////////
// for simply use the build-in type
typedef		unsigned char		byte;
typedef		const byte			c_byte;
typedef     const char          c_char;
typedef		unsigned char		u_char;
typedef		unsigned int		u_int;
typedef		unsigned short		u_short;
typedef		unsigned long		u_long;
//
typedef		unsigned char       uint8;
typedef		unsigned int        uint32;
typedef		int                 int32;

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64       ulong64;
typedef signed   __int64       long64;
#else
typedef unsigned long long     ulong64;
typedef signed long long       long64;
typedef void*                  HANDLE;
#endif
////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// define the error code and the error messages
// which may occur in this module
enum Err_Code
{
	Err_Begin                                   = 1000,
	Err_Out_Of_Mem,
	Err_Invalid_Index,
	Err_Invalid_Ptr,
	Err_File_NotExist,
	Err_NoData_inFile,
	Err_CreateFile_Failed,
	Err_LockFile_Failed,
	Err_Deq_Failed,
	Err_QData_NotFound,
	Err_HostName_notFound,
	Err_DbException,
	Err_DbConnectionStr_Invalid,
	Err_QName_NotFount,
	Err_SubName_NotFount,
	Err_Payload_NotFount,
	Err_Payload_Format_Invalid,
	Err_SenderStr_Format_Invalid,
	Err_Propagate_Handler_Not_Found,
	Err_Invalid_Con_Protocol,
	Err_Invalid_Propagate_Code,
	Err_Invalid_RcvMsg_Process_Code,
	Err_Invalid_Addr,
	Err_Create_TCPCon_Failed,
	Err_Failed_GetHostName,
	Err_Server_IP_Addr_Not_Found,
	Err_Client2Server_ID_Invalid,
	Err_Tcp_Disconnected_Enq,
	Err_EnqAck_Data_Inconsistence,
	Err_NewThread_Not_Start,
	Err_Register_Observer_Failed,
	Err_DBServer_DiskFull,
	Err_DBOffLine,
	Err_DBEnqOpt_Disable,
	Err_DBQ_PLT_Invalid,
	Err_DeqOption_Invalid,
	Err_Receiver_Lst_Null,
	Err_Config_Invalid,
	Err_DEQ_Message_Lst_NotNull,

	Err_Unknown,
	Err_End
};

static c_char* g_ErrMsg[] = 
{
	("The OS is running out of memory."),
	("Invalid index of array or container."),
	("Invalid pointer or Null pointer."),
	("File is not exist or access denied."),
	("Empty config file or no valid data found in config file."),
	("Create file failed.Please check permission for creating file."),
	("Lock file failed. please ensure file permission."),
	("Deq exception or failed!"),
	("The data inconsistency between dispatch and queue table."),
	("source database host name not found."),
	("database server exception or crash down."),
	("The database connection string invalid."),
	("The queue name is empty or null."),
	("Subscriber name is empty."),
	("The payload data cannot found cause invalid key value."),
	("The format of payload data is invalid."),
	("The sender string format invalid."),
	("The propagation handler refer to server ID not found."),
	("The protocol is invalid."),
	("The propagate code is invalid."),
	("The receive message enq state code is invalid."),
	("The server IP address or Port is invalid."),
	("Failed to create a new TCP client."),
	("Failed to get local host name."),
	("The server IP address is empty or null."),
	("The client to server ID is empty or null."),
	("The Tcp connection disconnected during enqueue."),
	("The Enq Ack return by remote is in a inconsistence state."),
	("There are still some new threads not start up properly."),
	("Register the observer handler failed."),
	("The target DB server hard disk is full."),
	("The target DB server offline."),
	("The target DB server enq option disabled."),
	("The payload type between both sides mismatched."),
	("The Dequeue options are invalid, subid or consumer name is invalid."),
	("The receiver list is empty."),
	("Fatal error: the config info is invalid."),
	("The dequeue message buffer still has messages to sent to server."),
	
	("Unknown error or exception occurred.")
};

// for easy to extract the error message base on the error code
#define  defErrMsg(A)                  g_ErrMsg[(A) - Err_Begin - 1]

//////////////////////////////////////////////////////
enum  Ret_Code
{
	Ret_Success                                 = 0,
	Ret_Failed,
	Ret_Exit
};

enum Src_DB_Type
{
	enumOracleDb = 1,   // Oracle database  (Support)
	enumMysqlDb  = 2,   // Mysql database   (Support)
	enumMSSvrDb  = 3,   // Microsoft SQL Server  (Not support currently, just list here)
	enumReserve         // For other database system should add below
};

enum MessageState
{
	MSG_READY = 1,
	MSG_WAITING,
	MSG_PROCESSED,
	MSG_EXPIRED
};

// enumeration for enqueue message
enum EnqueueMode
{
	ENQ_AQ_API          = 1,
	ENQ_SQL_PRC_NoPrepare = 2,
	ENQ_SQL_PRC_Prepare_One = 3,
	ENQ_SQL_PRC_Prepare_Batch = 4  
};

// enumeration for connection session options
enum DBConnectionOptCode
{
	CON_BACKSLASH_OFF          = 0,
	CON_BACKSLASH_ON
};


// enumeration for dequeue message
enum DequeueMode
{
	DEQ_REMOVE      = 1,
	DEQ_REMOVE_NODATA,
	DEQ_REMOVE_DELAY,
	DEQ_REMOVE_FIXED,
	DEQ_BROWSE
};
enum Navigation
{
	DEQ_FIRST_MSG   = 1,
	DEQ_NEXT_MSG
};
enum Visibility
{
	DEQ_IMMEDIATE   = 1,
	DEQ_ON_COMMIT
};

enum MsgListenMode
{
	MSG_LISTEN_NOWAIT  = 0,
	MSG_LISTEN_FOREVER = 0xFFFF
};

enum MsgPayloadType
{
	PAYLOAD_TYPE_RAW    = 1,
	PAYLOAD_TYPE_OBJ    = 0
};

enum ReqAckCode
{	
	Err_EnqReqACkCode_Base				   = -1000,
	Err_EnqReqACkCode_DiskFull			   = Err_EnqReqACkCode_Base + 1,
	Err_EnqReqACkCode_DBOffLine			   = Err_EnqReqACkCode_Base + 2,
	Err_EnqReqACkCode_EnqDisable		   = Err_EnqReqACkCode_Base + 3,
	Err_EnqReqACkCode_PLTypeInvalid		   = Err_EnqReqACkCode_Base + 4,
	Err_EnqReqACkCode_DataSrcErr		   = Err_EnqReqACkCode_Base + 5,
	Err_EnqReqACkCode_OCILibErr		       = Err_EnqReqACkCode_Base + 6,
	Err_EnqReqACkCode_Disconnected	       = Err_EnqReqACkCode_Base + 7,


	ConReqAckCode_Succeed                  = 0,
	Err_ConReqACkCode_Base                 = -2000,
	Err_ConReqACkCode_InvalidQName         = Err_ConReqACkCode_Base + 1,

	
	Err_EnqReqACkCode_End,

	//Succed  enq count
};

enum DeqAckCode
{	
	Err_DEQACkCode_Base					   = -10000,
	Err_DEQACkCode_NOTFIND                 = Err_DEQACkCode_Base + 1,

	Err_DEQACkCode_Ease,

	//Succed  enq count
};

// some database error code which we are 
// intrested in.
enum MYSQL_DB_Error_Code
{
	ER_DUP_KEYNAME			 = 1061,
	ER_DUP_ENTRY			 = 1062,
	ER_MYSQL_READ_ONLY		 = 1290,
	ER_MYSQL_CANNT_CONNECT   = 2003, //Can't connect to MySQL server on '192.168.123.41'
	ER_MYSQL_GONE_AWAY		 = 2006, //MySQL server has gone away
	ER_MYSQL_PRC_NOEXIST     = 1305,

};


enum ORACLE_DB_Error_Code
{  
	/*OCI_ERR_AQ_DEQUEUE_TIMEOUT*/
    ER_ORACLE_AQ_DEQUEUE_TIMEOUT  =   25228,

	/*
	Oracle Database Error Code ORA-25207 Description :
	enqueue failed, queue string.string is disabled from enqueueing
	Error Cause:
	The queue has been stopped to prevent any further enqueueing.
	*/
	ER_ORACLE_EnqueueFailed	 = 25207,
	/*
	Oracle Database Error Code ORA-25226 Description :
	dequeue failed, queue string.string is not enabled for dequeue
	Error Cause:
	The queue has not been enabled for dequeue.
	*/
	ER_ORACLE_DequeueFailed	 = 25226,
	
	/*
	Oracle Database Error Code ORA-1089 Description :
	immediate shutdown in progress - no operations are permitted
	Error Cause:
	sqlplus "/ as sysdba"
	shutdown immediate
	*/
	ER_ORACLE_ImmediateShutDown	 = 1089,
	
	
	/*
	Oracle Database Error Code ORA-1033 Description :
	ORACLE initialization or shutdown in progress
	Error Cause:
	sqlplus "/ as sysdba"
	shutdown immediate
	startup	
	*/
	ER_ORACLE_InitOrShutDownInProgress = 1033, 


	/*
	Oracle Database Error Code ORA-1034 Description :
	ORACLE not available
	Error Cause:
	Oracle was not started up. 
	*/
	ER_ORACLE_NotAvailable	 = 1034,
	
	
	/*
	Oracle Database Error Code ORA-12537 Description :
	TNS:connection closed
	Error Cause:
	Oracle was not started up. 
	*/
	ER_ORACLE_TNSConnectionClosed = 12537,
	
	


	
};


enum EXESQLAckCode
{	
	Err_EXESQLAckCode_Base					   = -20000,
	Err_EXESQLAckCode_NotGetData			   = Err_EXESQLAckCode_Base + 1, //select not get data


};


//===============================================
#define defDefault_Deq_WaitTime   2	 //deq time out default value is 2 seconds 




//===========configfile======================================================
// define the identifier for the system config file
//default value
#define  defDefault_Config_DbMySQL            "mysql"
#define  defDefault_Config_DbOracle           "oracle"

//===========================================================================

#define  defConfig_DbConStrTag		  0x3A                   // ':'
#define  defConfig_srcIpDelimit       0x3B                   // ';'
#define  defConfig_srcIpDelimitEx     0x7C                   // '|'
#define  defMsg_DelimitTag            0x2C                   // ','

#define  DEF_64BIT_INT_LEN            (20)
#define  defDefaultListenTime         (100)               // the default listen interval time
#define  defDefaultListenTime1        (50)                // the default listen interval time 2        
#define  defDefaultMsgPrintTimes      (1000)              // the default message print out times
#define  defMaxNameLenAQ              (50)                // 50 characters, if this enough?
#define  defMaxFileLen                (260)               // 260 characters, include path and file name, same with Windows
#define  defMiniSQLParaSize           (2)                 // the require parameters for SQL wrapper at least 2.
#define  defMaxSQLLen                 (2047)              // the max SQL statement length supported in stack 2047 + 1(NULL)
#define  defMaxLogSize                (2000)              // the max log message size support by the log module
#define  defMaxQueueNameLen           (32)                // 32 characters, if this enough?
#define  defMaxMsgIDLen               (37)                // 36 characters for the message ID + NULL.
#define  defDBMsgSize                 (7)                 // the column count of the select SQL statement for retrieve data from MySQL Queue table

#define  defMaxNameLen                (256)                // 50 characters, if this enough?

#define  defEnqBatchSize              (100)				   //default BatchSize 50 records
#define  defMaxSQLStringLength        (16384)			   //default Max SQL Len 16K

#define  defMsg_SrvIDTag              "DB_SRV_ID"   
#define  defMsg_CenterSIDTag          "CENTRAL_SID"
#define  defPayload_1_Tag             "MESSAGE_QUALIFIER"   // the tag of the user_data_1, refer to Oracle message_qualifier 
#define  defPayload_2_Tag             "AUDIT_DATA_ORACLE"   // the tag of the user_data_2, refer to Oracle audit_data_oracle
#define  defPayload_3_Tag             "AUDIT_DATA_MYSQL"    // the tag of the user_data_3, refer to Oracle audit_data_mysql

#define  DEF_EXIT_CHECK(condition)   if((condition))   return Ret_Exit  
#define  DEF_ERROR_CHECK(condition)  if(!(condition))  return  Ret_Failed
#define  DEF_ERROR_EXIT(condition)   if(!(condition))  goto Exit0
#define  DEF_ASSERT_EXIT(condition)  if(!(condition))  goto Exit0

#ifndef  countof
#define  countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define DEF_DELETE_ARRAY(pArray)    if (pArray) { delete [](pArray); (pArray) = NULL; }
#define DEF_DELETE(p)   if (p) { delete (p); (p) = NULL; }

#define DEF_DISABLE_COPY_AND_ASSIGNMENT(className)   private:   \
	className(const className&); \
	className& operator=(const className&);

#define DEF_RET_FAILED  -1            // for solaris file handle return code
#define DEF_INFINITE    0xFFFFFFFF 


NS_END(TA_AQ_Lib)

#endif  // AQCOMMONDEF_H














