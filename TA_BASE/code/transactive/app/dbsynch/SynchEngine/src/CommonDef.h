//////////////////////////////////////////////////////////////////////
///		@file		CommonDef.h
///		@author		zhilin,ouyang
///		@date		2012-01-05 10:44:41
///
///		@brief	    to define some common use macro or constant static 
///                 variables used in the DB synchronization engine.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  COMMONDEF_H
#define  COMMONDEF_H
#include "AQCommonDef.h"


NS_BEGIN(TA_DB_Sync)

using namespace TA_AQ_Lib;



//==============================================================
/*
	TCP client server Com Protocol
*/
enum enmComProtocolCode
{			   
	ComProtocol_CTOS_CON_REQ	= 0x01,
	ComProtocol_STOC_CON_ACK	= 0x02,

	ComProtocol_CTOS_ENQ_REQ	= 0x04,
	ComProtocol_STOC_ENQ_ACK	= 0x08,

	ComProtocol_STOC_ENQ_REQ	= 0x10,
	ComProtocol_CTOS_ENQ_ACK	= 0x20
};



//==============================================================

/*
	STX --- start of the text.  Value: F8H.         Length: 1BYTE
	PL --- length of the package. Value: the length of the package.  Length: 4BYTE
	PUUID--  Value:          Length: 16BYTE
	PT --- type of the package.  Value:  0 每 request; 1 每 reply; 2 每 notify. Length: 1BYTE
	DC --- data compress option.  Value: 0 每 FALSE; 1 每 TRUE.  Length: 1BYTE
	UZDL --- length on the data . Value: the length of the data.  Length: 4BYTE
	msgItems
	ETX --- end of the text.   Value: FEH.       Length: 1BYTE
*/
enum MsgPackageLenConst
{
	MSGPACKAGE_LEN_STX		= 1,
	MSGPACKAGE_LEN_PL		= 4,
	MSGPACKAGE_LEN_PUUID	= 16,
	MSGPACKAGE_LEN_PT		= 1,
	MSGPACKAGE_LEN_DC		= 1,
	MSGPACKAGE_LEN_UZDL		= 4,
	MSGPACKAGE_LEN_ETX		= 1,

	MSGPACKAGE_TOTAL     = (MSGPACKAGE_LEN_STX + MSGPACKAGE_LEN_PL + MSGPACKAGE_LEN_PUUID + MSGPACKAGE_LEN_PT + MSGPACKAGE_LEN_DC + MSGPACKAGE_LEN_UZDL + MSGPACKAGE_LEN_ETX)
};



enum MsgPackageValueConst
{
	MSGPACKAGE_STX_VALUE    =   0xF8,
	MSGPACKAGE_ETX_VALUE    =   0xFE,

	MSGPACKAGE_PT_REQUEST   =	0x00,
	MSGPACKAGE_PT_ACK		=	0x01,
	MSGPACKAGE_PT_NOTIFY    =   0x02,
	MSGPACKAGE_PT_CONREQUEST   =	0x03,

	MSGPACKAGE_DC_UNZIP     =   0x00,
	MSGPACKAGE_DC_ZIP		=   0x01,
};


enum MsgPackageValueLenConst
{
	MSGPACKAGE_PUUID_VALUE_LEN	= 32,
	MSGPACKAGE_PUUID_BUFFER_LEN	= MSGPACKAGE_PUUID_VALUE_LEN + 1,
	  
};

//==============================================
/*
	MT		每 type of the message.  
				Value: 0 每 message contains payload; 
				1 每 message only contains message ID.
				Length: 1BYTE
	QN		每 queue name.  Value: the target queue name.  Length: 32BYTE
	OMI		每 the source/original message ID.  Value: unique ID.  Length: 36BYTE
	SID     --
	PR		---Priority: priority of the message. Value: 0每immediately; 1- fix time. Length: 1BYTE
	PYTP    --
	MDL		---Length: the length of the message. Value: unsigned integer size.  Length: 4BYTE
*/

enum MsgItemLenConst
{
	MSGITEM_LEN_MT		  = 1,//enmComProtocolCode
	MSGITEM_LEN_QN        = 32,
	MSGITEM_LEN_OMI       = 36,
	MSGITEM_LEN_OServerID = 4,
	MSGITEM_LEN_SID       = 8,
	MSGITEM_LEN_PR		  = 1,
	MSGITEM_LEN_PYTP	  = 1,	
	MSGITEM_LEN_MDL       = 4,
	//msgdata
	MSGITEM_LEN_TOTAL     = (MSGITEM_LEN_MT + MSGITEM_LEN_QN + MSGITEM_LEN_OMI + MSGITEM_LEN_OServerID + MSGITEM_LEN_SID + MSGITEM_LEN_PR + MSGITEM_LEN_PYTP + MSGITEM_LEN_MDL)
};

enum MsgItemValueConst
{
	//MSGITEM_MT_VALUE		  = enmComProtocolCode
   MSGITEM_PR_HIGH        = 0x31, 
   MSGITEM_PR_NORMAL      = 0x32,    
   MSGITEM_PR_LOW         = 0x33,    

   MSGITEM_PYTP_RAW       = 0x51,    
   MSGITEM_PYTP_OBJ       = 0x52,    

};

enum MsgItemValueLenConst
{
	MSGITEM_QN_VALUE_LEN	= 32,
	MSGITEM_QN_VALUE_BUFFER_LEN	= MSGITEM_QN_VALUE_LEN + 1,	//33

	MSGITEM_OMI_VALUE_LEN	= 36,
	MSGITEM_OMI_VALUE_BUFFER_LEN	= MSGITEM_OMI_VALUE_LEN + 1,	//37

	MSGITEM_INT32_VALUE_LEN = 4,
	MSGITEM_INT64_VALUE_LEN = 8,

};

enum Timer_ID_Code
{
	PROPAGATE_TIMER_CONACK              = 1000,
	PROPAGATE_TIMER_LOG_EACHDEQWORK,
	PROPAGATE_TIMER_LOG_DEQDATA,
	PROPAGATE_TIMER_LOG_SENDDATA,
	PROPAGATE_TIMER_LOG_WAITACK,
	PROPAGATE_TIMER_LOG_DEQUPDATE,
	PROPAGATE_TIMER_LOG_DEQCOMMIT,
	PROPAGATE_TIMER_LOG_EACHENQWORK,
	PROPAGATE_TIMER_LOG_ENQDATA,
	PROPAGATE_TIMER_LOG_ENQCOMMIT,
	PROPAGATE_TIMER_LOG_SNEDACK, 
	PROPAGATE_TIMER_LOG_GETSID,
	PROPAGATE_TIMER_LOG_UPDSID,
		
	PROPAGATE_TIMER_LOG_THREAD_WORK,
	PROPAGATE_TIMER_LOG_MonitorDeqTime,
	PROPAGATE_TIMER_LOG_MonitorClientEnqSendAckTime,
	PROPAGATE_TIMER_LOG_MonitorEnqTime,
	PROPAGATE_TIMER_LOG_MonitorHandShark,
	PROPAGATE_TIMER_LOG_MonitorReleaseActor,

	HEARTBEAT_TIMER

};





//==============================================================
/*
	declare some time constant for easy to use
*/
enum TimeConstantMac
{			   
	TIME_BASE_SECOND		= 1000,
	TIME_BASE_MIN			= 60   * TIME_BASE_SECOND,
	TIME_BASE_HOUR			= 60   * TIME_BASE_MIN,
	TIME_BASE_DAY			= 24   * TIME_BASE_HOUR,

	TIME_SECOND_TWO         = 2    * TIME_BASE_SECOND,
	TIME_SECOND_THREE       = 3    * TIME_BASE_SECOND,
	TIME_SECOND_FOUR        = 4    * TIME_BASE_SECOND,
	TIME_SECOND_FIVE		= 5    * TIME_BASE_SECOND,
	TIME_SECOND_TEN         = 10   * TIME_BASE_SECOND,
	TIME_SECOND_THIRTY      = 30   * TIME_BASE_SECOND,
            
	TIME_MIN_TWO			= 2  * TIME_BASE_MIN,
	TIME_MIN_THREE			= 3  * TIME_BASE_MIN,
	TIME_MIN_FIVE           = 5  * TIME_BASE_MIN,
	TIME_MIN_EIGHT          = 8  * TIME_BASE_MIN,
	TIME_MIN_TEN            = 10 * TIME_BASE_MIN,
	TIME_MIN_FIFTEEN        = 15 * TIME_BASE_MIN,
	TIME_MIN_TWENTY         = 20 * TIME_BASE_MIN,
	TIME_MIN_THIRTY         = 30 * TIME_BASE_MIN,
	TIME_MIN_FOURTY         = 40 * TIME_BASE_MIN,
	TIME_MIN_FIFTY          = 50 * TIME_BASE_MIN,

	TIME_HOUR_TWO           = 2 * TIME_BASE_HOUR
};


//==============================================================

enum BufferLenConst	
{
	BUFFER_LEN_ADDEX        = 256,
	BUFFER_LEN_READ			= 0xFFFF,//65535
	BUFFER_LEN_MAX			= 0xFFFFFFF,//268M
};

//==============================================================

enum ServerLimitValueConst	
{
	SERVER_LIMIT_THREAD_SLEEP				= 100,
	SERVER_LIMIT_TCP_RECONNECT_SLEEP		= 5000,
	SERVER_LIMIT_TCP_LISTEN_TIMEOUT         = 100,
	SERVER_LIMIT_TCP_TIMEOUT                = 500,
	SERVER_LIMIT_TCP_RecvBuffReset          = 500,

	SERVER_LIMIT_CLIENT_COUNT_MAX			= 1024,
	SERVER_LIMIT_LOG_NOTWORK_TIME			= 120,
	SERVER_LIMIT_LOG_VLAUE_MOD				= 1024,
	SERVER_LIMIT_LOG_VLAUE_MOD_MAX			= 10240,
	SERVER_LIMIT_LOG_VLAUE_MAX				= 0xFFFFFFF,

	SERVER_LIMIT_INVAILD_CLIENT_COUNT_MAX   = 150,
};



//==============================================================


enum TCP_STATE_Code
{
	TCP_CONNECTED        = 100,
	TCP_DISCONNECTED     = 101,
	TCP_NODATAFORREAD    = 102,
};



//==============================================================

enum NewClientCheckedResultConst
{
	NewClientCheck_RefuseClient   =1,
	NewClientCheck_ReplaceOldServer ,       
	NewClientCheck_CreateNewServer,
};

enum CurrentClientDestroyReasonConst
{
	CurrentClientDestroy_TcpDisconnect = 1,	      
	CurrentClientDestroy_Replace ,	
	CurrentClientDestroy_Normal ,
	CurrentClientDestroy_CheckResError ,
	CurrentClientDestroy_NotWorkError ,

};

//==============================================================


// the run type of the SynchEngine
enum enRunTypeCode
{
	RunType_NotSet             = -1,
	RunType_Server             = 1,
	RunType_Client	    
};

// the switch for tcp connection failed monitor mechanism
enum enTcpConFailMonSwitch
{
	Tcp_ConFailMon_Switch_NotSet = -1,
	Tcp_ConFailMon_Switch_OFF  = 0,
	Tcp_ConFailMon_Switch_ON,

};

// the switch for sending heartbeat mechanism from server to client
enum enHeartBeatSwitch
{
	HeartBeat_Switch_NotSet = -1,
	HeartBeat_Switch_OFF  = 0,
	HeartBeat_Switch_ON,
};

// the heartbeat timer state for TCP Communication threads
enum HeartBeatCode
{
	enHeartBeat_Timer_ON         = 0,
	enHeartBeat_Timer_OFF
};


/*
* system state code is used 8-bit to present the following 
  code, currently, only 3-bit used, others reserved for 
  further usage, depicts below:

  STATE_CODE_STATUS       -  the system running status.
                             0 - normal; 1 - exit.
  STATE_CODE_RUNNING_MODE -  the system running mode.
                             0 - active; 1 - passive.
  STATE_CODE_RUNNING_TYPE -  the system running type.
                             0 - server; 1 - client.
*/

#define  defMaxFlagCount    8
#define  defCharBitMask     7
#define  defItemIndexMask   3
#define  defFlagNum          (defMaxFlagCount >> defItemIndexMask)


enum SysStateBitCode
{
	STATE_CODE_STATUS                  = 0,
	STATE_CODE_RUNNING_MODE,
	STATE_CODE_RUNNING_TYPE,   
	STATE_CODE_RESERVED
};

enum SynchMode
{
	SYNCH_MODE_ACTIVE           = 0,
	SYNCH_MODE_PASSIVE
};
//=========system state code==========================





// some useful macro which will use in this module
#define  defMaxPackageSize            (200 * 1024)        // 200KB
#define  defMaxUnsendMsgSize          (2 * 1024 * 1024)   // 2MB
#define  defBytesKMask                (1024)              // 1K
#define  defServerPortLen             (10)                // 10 characters for the server port
#define  defDefaultDeqTime            (150)               // the default dequeue interval time
#define  defDefaultEnqTime            (100)               // the default enqueue interval time
#define  defDefaultWaitEventTime      (10)                // the default wait event interval time
#define  defDefaultMsgPrintTimes      (1000)              // the default message print out times
#define  defChannelSrvTryTimes        (2)                 // the try times for tcp connect to target server of propagation
#define  defTCPSeqFailTimes           (20)                // the maximum times for tcp sequence out of order (can connected but fail immediately)


//===========defString==========================================
#define  defDefault_ServerIDName            "server"

//===========configfile======================================================
// define the identifier for the system config file
//default value
#define  defDefault_ConfigFileName            "dbsynconfig.cnf"
#define  defDefault_LogFileName				  "SynchEngineLog.log"
#define  defDefault_Config_DbMySQL            "mysql"
#define  defDefault_Config_DbOracle           "oracle"
#define  defDefault_config_OracleServerPort	  "2006"
#define  defDefault_config_DebugPidFilenames  "0"
#define  defConfig_RunType_Client             "client"
#define  defConfig_RunType_Srv                "server"

#define  defDefault_Config_LogFileSize        (19999999)          // the default log file size 20M
#define  defDefault_Config_LogFileCount       (100)               // the default log file count 100

#define  defConfig_DbConStrTag				  0x3A                   // ':'
#define  defConfig_srcIpDelimit               0x3B                   // ';'
#define  defMsg_DelimitTag                    0x2C                   // ','
#define  defMsg_OrderASC                      "ASC"
#define  defMsg_OrderDESC                     "DESC"
#define  defConfig_OneLineMaxLen              (256) 


//default tag
//[dbsyn_queue]
#define  defConfig_CenterDBName				  "centerdbname"
#define  defConfig_MsgCompress				  "msgcompress"
#define  defConfig_PackSize					  "packagesize"
#define  defConfig_SrcDbConStr				  "srcdbconstr"
#define  defConfig_BackSlashSwitch            "backslashswitch"
#define  defConfig_SkipErrors				  "skiperrors"
#define  defConfig_LogLevel                   "loglevel"
#define  defConfig_LogFileSize                "logfilesize"
#define  defConfig_LogFileCount               "logfilecount"
#define  defConfig_LogFileName                "logfilename"
#define  defConfig_ServerPort                 "serverport"
#define  defConfig_RunType                    "runtype"
#define  defConFailMonSwitch                  "confailmonswitch"
#define  defHeartBeatSwitch                   "heartbeatswitch"
#define  defHeartBeatInterval                 "heartbeatinterval"
#define  defHeartBeatTimeOut                  "heartbeattimeout"
#define  defConfig_EnqTimeOut                 "enqtimeout"		   //defaule value	TIME_MIN_FIVE
#define  defConfig_DeqTimeOut                 "deqtimeout"		   //defaule value	TIME_MIN_FIVE
#define  defConfig_DeqGetACKTimeOut           "deqgetacktimeout"	//defaule value	TIME_MIN_FIFTEEN
#define  defConfig_HandSharkTimeOut           "handsharktimeout"   //defaule value	TIME_SECOND_TEN
#define  defConfig_ReleaseActorTimeOut        "releaseactortimeout"   //defaule value	TIME_MIN_FIVE
#define  defConfig_DebugPidFilenames		  "debugpidfilenames"

#define  defConfig_EnqBatchSize   		      "enqbatchsize"  //defEnqBatchSize
#define  defConfig_DeqFailOverTime            "deqfailovertime"    //defaule value	TIME_SECOND_FOUR
#define  defConfig_CharSet                    "mysqlcharset"


//===========configfile======================================================

//===========command line=================================================
#define  defCmdLine_StartSynch                 "start"
#define  defCmdLine_StopSynch                  "stop"
#define  defCmdLine_DefaultFile                "DefaultFile"
#define  defCmdLine_Mode                       "Mode"
#define  defCmdLine_SynID                      "SynID"
#define  defCmdLine_Mode_Active                "Control"
#define  defCmdLine_Mode_Passive               "Monitor"
#define  defCmdLine_Mode_Stop                  "Stop"
#define  defCmdLine_StatusSynch                "Status"
#define  defCmdLine_CharSet                    "Charset"
//===========command line=================================================


#define  defDefault_MMSQueueName                   "MMS_DATA_QUEUE"
#define  defDefault_AUDITQueueName                 "AUDIT_DATA_QUEUE"




#if !defined(EXTERN_C)
#ifdef __cplusplus
#define EXTERN_C			extern "C"
#else
#define EXTERN_C
#endif
#endif

#define DEF_WAITFOR_SIGNAL(condition)   while(!(condition))   Thread::sleep(defDefaultListenTime);

#define DEF_INT4KORR(A)	(uint32) (((uint32) ((unsigned char) (A)[0])) +\
	(((uint32) ((unsigned char) (A)[1])) << 8) +\
	(((uint32) ((unsigned char) (A)[2])) << 16) +\
	(((uint32) ((unsigned char) (A)[3])) << 24))
#define DEF_INT4STORE(T,A)       do { *((char *)(T))=(char) ((A));\
	*(((char *)(T))+1)=(char) (((A) >> 8));\
	*(((char *)(T))+2)=(char) (((A) >> 16));\
	*(((char *)(T))+3)=(char) (((A) >> 24)); } while(0)
#define DEF_INT8KORR(A)	((ulong64) (((uint32) ((unsigned char) (A)[0])) +\
			(((uint32)  ((unsigned char) (A)[1])) << 8) +\
			(((uint32)  ((unsigned char) (A)[2])) << 16) +\
			(((uint32)  ((unsigned char) (A)[3])) << 24)) +\
(((ulong64) (((uint32)  ((unsigned char) (A)[4])) +\
		    (((uint32)  ((unsigned char) (A)[5])) << 8) +\
		    (((uint32)  ((unsigned char) (A)[6])) << 16) +\
		    (((uint32)  ((unsigned char) (A)[7])) << 24)))<<32))
#define DEF_INT8STORE(T,A)       do { *((char *)(T))=(char) ((A));\
	*(((char *)(T))+1)=(char) (((A) >> 8));\
	*(((char *)(T))+2)=(char) (((A) >> 16));\
	*(((char *)(T))+3)=(char) (((A) >> 24));\
	*(((char *)(T))+4)=(char) (((A) >> 32));\
	*(((char *)(T))+5)=(char) (((A) >> 40));\
	*(((char *)(T))+6)=(char) (((A) >> 48));\
	*(((char *)(T))+7)=(char) (((A) >> 56)); } while(0)

//==============================================================



NS_END(TA_DB_Sync)

#endif  // COMMONDEF_H
