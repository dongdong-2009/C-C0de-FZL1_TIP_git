
#ifndef _FEPDATASTRUCT_H
#define _FEPDATASTRUCT_H
#include "Btype.h"
//#include "Datadef.h"

#if defined(WIN32) || defined(WINDOWS)
#define OS_WIN
#endif

#if defined(__unix) || defined(__linux)
#define OS_UNIX
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#ifdef OS_WIN
#include <winsock2.h>
#include <windows.h>

#include <time.h>
#include <memory.h>
// #define TRACE printf
#endif

#ifdef OS_UNIX
#include <unistd.h>
#include <signal.h>
#include <memory.h>
#include <sched.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <netdb.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <netinet/in.h>

#endif

#include "Btype.h"
#if defined(__sparc__) || defined(SOLARIS) 
#define INADDR_NONE         0xFFFFFFFF 

#include <sys/param.h>			/* MAXHOSTNAMELEN    */

#endif
//#include "osi/osidatetime.h"

#define MAX_PATH          260
#include "IecSocket.h"
#include "Threads.h"

//class CMutex;

//  #undef FALSE
//  #define FALSE	0
//  //#ifndef b_true
//  #undef b_true
//  #define b_true	1
//  //#endif
//  
//  #undef	ULONG
//  #undef	ULONG32
//  #undef	LONG64
//  #undef	ULONG64
//  #undef	FLOAT32
//  #undef	FLOAT64
//  
//  #undef	b_bool
//  #undef	b_ui8
// // 
//  //typedef int	    	b_bool;
//  typedef unsigned char		b_ui8;
//  typedef unsigned short WORD;
// // 
//  #undef	UINT64
// #ifdef OS_WIN
// typedef unsigned __int64     UINT64;
// typedef int socklen_t;   
// #else
// typedef unsigned long long     UINT64;
// #endif
  
#define CHAN_UNKNOW			0
#define CHAN_RUN			1
#define CHAN_STOP			2
#define CHAN_UNSYNC			3
#define CHAN_CHECKERR		4

#define RTU_UNKNOW			0
#define RTU_RUN				1
#define RTU_STOP			2
#define RTU_UNSYNC			3
#define RTU_CHECKERR		4
#define RTU_SEARCH			5


#define NORMAL_NAME_LEN     32

//通信方式
#define PTPMODE			    0
#define MULTICASTMODE	    1
#define BROADCASTMODE       2

// define channel buffer 
#define RXBUFLEN           10240
#define TXBUFLEN            1024
#define MAX_BUF_SIZE        10240   // define max buffer size for each channel

#define MAX_PROTOCOL_NUM	100
#define MAX_CHAN_NUM		256
#define MAX_RTU_NUM			256


#define  DEF_FILENAME_LEN   1024

//#define   DEF_CONFIG_FILE "../config/iec104.conf"
// define debug code 
enum FEP_DEBUG_CODE
{
	CHANNEL_EVENT = 1,
	RTU_EVENT,
	STATUS_CHANGE,
	STATUS_SOE,

	DEBUG_LASTMSG
};

//class CDevice;
//class CReadThread;
//class CWriteThread;
//class CExplainThread;
//class CDataTranThread;
class CProtocolBase;
//class CComParaBase;

#ifdef OS_WIN
//  #if defined FEP_EXPORT
//  #define _COMBASE_DLL _declspec(dllexport)
//  #else
//  #define _COMBASE_DLL _declspec(dllimport)  
//  #endif
//#else
//	#define _COMBASE_DLL	
#endif

typedef CProtocolBase*( *ProtocolInit)();
typedef void( *ProtocolFree)(CProtocolBase* pProtocolBase); 

#ifdef OS_UNIX
extern int GetLastError();
#endif


/////////////////////////////////////////////////////////////////
//data structs used inside
struct SFepTime
{
	short    year;
	unsigned char  month;
	unsigned char  day;
	unsigned char  hour;
	unsigned char  minute;
	unsigned char  second;
	short millisecond;
};


struct SChanStatus
{
	int iChanID;
	int iChanStatus;
	int iIsSrc;
};


/********************************************************************
    following define raw data structure which are needed to be returned
	analog ,status, pulse,SOE,protection info etc.

********************************************************************/
struct SCp32Time
{
	b_ui16 milisecond;
	b_ui8   minute;
	b_ui8   hour;
};
struct SFailEq
{
	int iOffset;
	float fVal; 
};
struct SProtectActionRaw
{
	int      iTotalNo;
	b_ui8 bProtectActionNo;
	b_ui8 bDpointVal;
	int     iRelTime;
	int     iFailNo;
	struct SCp32Time time;
	b_ui8 bFailCount;
	struct SFailEq sFailEq[255];
	
};

struct SFileInfo
{
	b_ui16 fileName;
	b_ui32 fileLen;
	b_ui8   fileState;
	SFepTime timeStamp;	
	SFileInfo()
	{
		memset(this,0,sizeof(SFileInfo));
	}
};

struct SProtectEventRaw
{
	int iOffset;	
	b_ui8 bValue;
	b_ui8 bFlag;
	b_ui16 mSec;
	SFepTime timeStamp;
	SProtectEventRaw()
	{
		memset(this,0,sizeof(SProtectEventRaw));
	}
};
struct SPointRaw
{
	int iOffset;
	b_ui8 btVal;
	int iFlag;
	b_ui8 bFlag;
//	int iIndex; // point tag index
	SPointRaw()
	{	
	 memset(this,0,sizeof( SPointRaw));
	}
};
struct SGroupSpRaw
{
	b_ui8 grpFlag;
	struct SPointRaw points[16];	
	SGroupSpRaw()
	{
		memset(this,0,sizeof(SGroupSpRaw));
	}
};
struct SBiaoDuRaw
{
	int iOffset;
	b_i16  iVal;
	b_ui8    bFlag;
	SBiaoDuRaw()
	{
		memset(this,0,sizeof(SBiaoDuRaw));
	}		
};
struct SBiaoDuRawTimeStamp
{
	int iOffset;
	b_i16 iVal;
	b_ui8    bFlag;
	SFepTime timeStamp;
	SBiaoDuRawTimeStamp()
	{
		memset(this,0,sizeof(SBiaoDuRawTimeStamp));
	}
};

struct S32btsRaw
{
	int iOffset;
	b_ui8 btBuf[4];
	b_ui8 bFlag;
	S32btsRaw()
	{
		memset(this,0,sizeof(S32btsRaw));
	}
};

struct S32btsRawTimeStamp
{
	int iOffset;
	b_ui8 btBuf[4];
	b_ui8 bFlag;
	SFepTime timeStamp;
	S32btsRawTimeStamp()
	{
		memset(this,0,sizeof(S32btsRawTimeStamp));
	}
};
struct SStRaw //step position information
{
	int iOffset;
	b_ui8 bT;
	b_ui8 bVal;
	b_ui8  bFlag;
	SStRaw()
	{	
	 memset(this,0,sizeof(SStRaw));
	}
};

struct SStRawTimeStamp
{
	int iOffset;
	b_ui8 bVal;
	b_ui8 bFlag;
	b_ui8 bT;
//	int iIndex;
	SFepTime timeStamp;

	SStRawTimeStamp()
	{	
	 memset(this,0,sizeof( SStRawTimeStamp));
	}
};

struct SAnalogRaw
{
	int  iOffset;     //relative offset of RTU
	float fVal;
	int  iFlag;    //quality flag
//	int  iIndex;   // analog tag index
	SAnalogRaw()
	{
	  memset(this,0,sizeof(SAnalogRaw));
	}
};

struct SAnalogRawTimeStamp
{
	int iOffset;
	float fVal;
	int iFlag;
//	int iIndex;  // analog tag index
	SFepTime timeStamp;

    SAnalogRawTimeStamp()
	{
		memset(this,0,sizeof(SAnalogRawTimeStamp));
	}
};



struct SPointRawTimeStamp
{
	int iOffset;
	b_ui8 btVal;
	int iFlag;
	b_ui8 bFlag;
//	int iIndex;
	SFepTime timeStamp;

	SPointRawTimeStamp()
	{	
	 memset(this,0,sizeof( SPointRawTimeStamp));
	}
};


struct SCountRaw
{
	int iOffset;
	unsigned int dwVal;
	int iFlag;
//	int iIndex;
	SCountRaw()
	{	
	 memset(this,0,sizeof( SCountRaw));
	}
};

struct SCountRawTimeStamp
{
	int iOffset;
    unsigned int dwVal;
	int iFlag;
	int iIndex;
	SFepTime timeStamp;

	SCountRawTimeStamp()
	{	
	 memset(this,0,sizeof( SCountRawTimeStamp));
	};
};


// define data quality flag
#define CHANGEDATA      (1 << 1)

#define RTUERROR        0x40
#define OLDDATA		    0x80
#define OperatorBlocked 0x400



// defines message structure used to debug
#define MAX_FRAME_LEN   1024
#define MAX_FRAME_NUM   2000

struct SMsgFrame
{
	int iChanID;
	int iDataType;   // 1 :rx 2 :tx 3 :exp
	int iDataLen;
	SFepTime timeStamp;
	b_ui8 btBuf[MAX_FRAME_LEN];
};

struct SMsgRecord
{
  int iWritep;
  SMsgFrame msgFrame[MAX_FRAME_NUM];

};

struct SDownCommand
{
	b_bool bSyncTimeFlag;
	b_bool bAllDataFlag;
	b_bool bAllKwhFlag;

};

///////////////////////////////////////////////////////////////////
//parameters definition 


//REMOTEUNIT definition,they are distinguished according to  TYPE
struct SRtuPara
{
	int    iID;
	char   strName[NORMAL_NAME_LEN];
	b_ui8   iType;
	char   strSubStationName[NORMAL_NAME_LEN];
	b_bool   bUseFlag;
	int    iAnaNum;
	int    iDigNum;
	int    iPulNum;
	int    iSyncTimeInterval;
	int    iAllDataInterval;
	int    iKwhDataInterval;

};

//channel parameters
struct SChanPara
{
	int	  iID;		                	// channel ID
	char  szName[NORMAL_NAME_LEN];		// channel name
	int   iPhyType;						// media type 
	int   iProtocolID;           		// protocol ID
	char  strPhyAddrA[256];	            // device address 1
	char  strPhyAddrB[256];	            // device address 2
	int   iPort;					    // 端口号
	int   bPoll;                      // poll mode flag
	int   bAvailable;					// enable flag:0不使用，1:使用

};

//protocol parameters 
struct SProtocolPara
{
	int  	iID;                  				//protocol ID
	char  	szName[NORMAL_NAME_LEN];			//protocol name
	char  	szDLLName[NORMAL_NAME_LEN];       	//protocol library name
};


struct STxBuf
{
	int iHeadP;
	int iTailP;
	b_ui8 btBuf[TXBUFLEN];
	STxBuf(){ iHeadP=0;iTailP=0;}
	int GetLength()
	{
		return (iTailP - iHeadP + TXBUFLEN)%TXBUFLEN;
	}
};

struct SRxBuf
{
	int iHeadP;
	int iTailP;
	b_ui8 btBuf[RXBUFLEN];
	SRxBuf(){ iHeadP=0;iTailP=0;}
};

struct SProtocollFunc
{
#ifdef OS_WIN
	HINSTANCE libHandle;
#endif
#ifdef OS_UNIX
	void * libHandle;
#endif
	ProtocolInit pFuncInit;		//prrtocol data initialized
	ProtocolFree pFuncFree;		//release protocol data
};

			
////////////////////////////////////////////////////////////////////////////
//cycle queue definition

////////////////////////////////////////////////////////////////////////////
class CCycleQueue
{
public:
	CCycleQueue(int size);
	 ~ CCycleQueue();
	 int getLength();
	 int getFreeLength();
	 int inQueue(b_ui8 * buf, int len);
	 int deQueue(b_ui8* buf,int len, b_bool bMovHead=true);
	 b_bool isFull();
	 b_bool isEmpty();
	 void resetQueue();
private:
	b_ui8 *m_buf;
	int  m_head;
	int m_tail;
	int m_maxSize;
	esca::CMutex m_headMutex;
	esca::CMutex m_tailMutex;

};

/**
 * class CChannel 
 *CChannel compose the link and protocol
 *It used to send and receive data,maintain link and configure protocol etc.
 */
struct SHostPara;
class CAdaptee;
class  /*_COMBASE_DLL*/ CChannel
{
public:
	CChannel();
	~CChannel();
public:
    
	bool init();

	
	int  Connect();


	b_bool Disconnect();

	//read data from the socket,put data into rx buffer
	int  Read();


	int  Write();


	void Reset() { m_bReset = b_true;}


	//load 104 protocol,and init the parameter
	b_bool LoadProtocol();

	//free 104protocol.
	void FreeProtocol();


	CProtocolBase* getProtocol();

	
	void Explain();	

	void logChannelMsg(int level,std::string &str);

public:
	SChanPara *m_pStaticPara;
	int m_iChannelIndex;
	
	b_bool m_bConnected;
	volatile b_bool m_bReset;		    	        
	b_bool m_bCanConnect;
	volatile bool m_bRunFlag;
	void setWorkMode(int mode);
	int  getWorkMode();
	int setHost(std::string ipAddr,int port,b_ui8 groupId);
	void setLocaAddr(std::string localAddr);
	b_ui8 getGroupId();
	void setLinkId(b_ui8 id);
	b_ui8 getLinkId();
	b_ui8 getHostStatus();
	void   setHostStatus(b_ui8 hostStatus);
	b_bool isConnect();
	int setManager(CAdaptee *pManager);


	CCycleQueue *m_rxBuf;
	CCycleQueue *m_txBuf;
	CProtocolBase* m_pProtocol;
	SHostPara *m_host;
	std::string  m_localAddr;
	
private:
	int       m_workMode;
	b_ui8   m_hostStatus;
	b_ui8   m_linkId;
	CAdaptee*m_pManager;
	CIecSocket mCommandSocket;

public:
	bool loadConfig();
	struct _internetStruct {
#ifdef OS_WIN

		SOCKET sock;
#else
        int sock;
#endif

		sockaddr_in sockAddr;
		char netAddr[MAX_PATH];
		int m_port;
		struct timeval timeout;
	}TCP_para;
	
};

//--------------------------------baseDatade-begin -----------------------------------------

struct _SIQ
{
	b_ui8 IV;
	b_ui8 NT;
	b_ui8 SB;
	b_ui8 BL;
	b_ui8 SPI;
};



//7.2.6.2 Double-point information

struct _DIQ
{
	b_ui8 DPI;
	b_ui8 BL;
	b_ui8 SB;
	b_ui8 NT;
	b_ui8 IV;
};


//7.2.6.3 Quality descriptor

struct _QDS
{
	b_ui8 OV;
	b_ui8 BL;
	b_ui8 SB;
	b_ui8 NT;
	b_ui8 IV;
};



//7.2.6.4 Quality descriptor for events of protection equipment
struct _QDP
{
	b_ui8 EI;
	b_ui8 BL;
	b_ui8 SB;
	b_ui8 NT;
	b_ui8 IV;
};


//7.2.6.5 Value with transient state indication
struct _VTI
{
	b_ui8 Value;
	b_ui8 Transient;
};

//7.2.6.6 Normalized value

typedef b_ui16 _NVA;

//7.2.6.7 Scaled value

typedef b_i16 _SVA;
//7.2.6.8 Short floating point number

typedef float _SFA;

//7.2.6.9 Binary counter reading

struct _SequenceNotation
{
	b_ui8 SQ;
	b_ui8 CY;
	b_ui8 CA;
	b_ui8 IV;
};


struct _BCR
{
	b_i32 Counter;
	_SequenceNotation SequenceNotation;
};

//7.2.6.40 Status and status change detection
struct _SCD
{
	b_ui16 ST;
	b_ui16 CD;	
};


//7.2.6.10 Single event of protection equipment
struct _SEP
{
	b_ui8 EventState;
	b_ui8 EI;
	b_ui8 BL;
	b_ui8 SB;
	b_ui8 NT; 
	b_ui8 IV;
};


//7.2.6.11 Start events of protection equipment

struct _SPE
{
	b_ui8 GeneralStart:1;
	b_ui8 SL1;
	b_ui8 SL2;
	b_ui8 SL3;
	b_ui8 SIE;
	b_ui8 SRD;
};

//7.2.6.12 Output circuit information of protection equipment
struct _OCI
{
	b_ui8 GC;
	b_ui8 CL1;
	b_ui8 CL2;
	b_ui8 CL3;
};



//7.2.6.13 Binary state information (IEV 371-02-03) 32 bit
typedef b_ui32 _BSI;
/*struct _BSI
{
	b_ui8 Bt1;
	b_ui8 Bt2;
	b_ui8 Bt3;
	b_ui8 Bt4;
};*/

//7.2.6.14 Fixed test bit pattern, two octets

typedef b_ui16 FBP;

//7.2.6.15 Single command (IEV 371-03-02)
struct SCO
{
	b_ui8 SCS:1;
	b_ui8 :1;
	b_ui8 QU:5;
	b_ui8 SE:1;
	SCO()
	{
		memset(this,0,sizeof(SCO));
	}
};

//7.2.6.16 Double command (IEV 371-03-03)

struct DCO
{
	b_ui8 DCS:2;
	b_ui8 QU:5;
	b_ui8 SE:1;
	DCO()
	{
		memset(this,0,sizeof(DCO));
	}
};

//7.2.6.17 Regulating step command (IEV 371-03-13)
struct RCO
{
	b_ui8 RCS:2;
	b_ui8 QU:5;
	b_ui8 SE:1;
	RCO()
	{
		memset(this,0,sizeof(RCO));
	}
};

//7.2.6.18 Seven octet binary time


//7.2.6.21 Cause of initialization
typedef b_ui8 _COI;


//7.2.6.22 Qualifier of interrogation

typedef b_ui8 _QOI;

//7.2.6.23 Qualifier of counter interrogation command
#ifdef _x86_
struct _QCC
{
	b_ui8 RQT:6;
	b_ui8 FRZ:2;
};
#else
struct _QCC
{
	b_ui8 RQT:6;
	b_ui8 FRZ:2;
};
#endif

//7.2.6.24 Qualifier of parameter of measured values
struct _QPM
{
	b_ui8 KPA;
	b_ui8 LPC;
	b_ui8 POP;
};

//7.2.6.25 Qualifier of parameter activation

typedef b_ui8 _QPA;

//7.2.6.27 Qualifier of reset process command
typedef b_ui8 _QRP;
//~~~~~~~~~~~~~~~~~~~~~~~~~baseDatadef-end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//--------------------------------datadef-begin-------------------------------
//7.3.1.1 TYPE IDENT 1: M_SP_NA_1 Single-point information without time tag
struct SSinglePoint
{
	int iOffset;
	_SIQ SIQ;
	SSinglePoint()
	{
		memset(this,0,sizeof(SSinglePoint));
	}
};

//7.3.1.3 TYPE IDENT 3: M_DP_NA_1 Double-point information without time tag
struct SDoublePoint
{
	int iOffset;
	_DIQ DIQ;
	SDoublePoint()
	{
		memset(this,0,sizeof(SDoublePoint));
	}
};

//7.3.1.5 TYPE IDENT 5: M_ST_NA_1 Step position information
struct SStepPosition
{
	int iOffset;
	_VTI VTI;
	_QDS QDS;
	SStepPosition()
	{
		memset(this,0,sizeof(SStepPosition));
	}
};

//TYPE IDENT 7: M_BO_NA_1 Bitstring of 32 bit
struct  SBitstring32
{
	int iOffset;
	_BSI BSI;
	_QDS QDS;
	SBitstring32()
	{
		memset(this,0,sizeof(SBitstring32));
	}
};

//TYPE IDENT 9: M_ME_NA_1 Measured value, normalized value

struct SMeasuredValueNVA
{
	int iOffset;
	_NVA NVA;
	_QDS QDS;
	SMeasuredValueNVA()
	{
		memset(this,0,sizeof(SMeasuredValueNVA));
	}
};

//TYPE IDENT 11: M_ME_NB_1 Measured value, scaled value

struct SMeasuredValueSVA
{
	int iOffset;
	_SVA SVA;
	_QDS QDS;
	SMeasuredValueSVA()
	{
		memset(this,0,sizeof(SMeasuredValueSVA));
	}
};

//TYPE IDENT 13: M_ME_NC_1 Measured value, short floating point number
struct SMeasuredValueSFA
{
	int iOffset;
	_SFA SFA;
	_QDS QDS;
	SMeasuredValueSFA()
	{
		memset(this,0,sizeof(SMeasuredValueSFA));
	}
};

//TYPE IDENT 15: M_IT_NA_1 Integrated totals
struct SCounterValue
{
	int iOffset;
	_BCR BCR;
	SCounterValue()
	{
		memset(this,0,sizeof(SCounterValue));
	}
};

//TYPE IDENT 20:Packed single-point information with status change detection
struct SPackedSinglePoint
{
	int iOffset;
	_SCD SCD;
	_QDS QDS;
	SPackedSinglePoint()
	{
		memset(this,0,sizeof(SPackedSinglePoint));
	}	
};

//TYPE IDENT 21:Measured value, normalized value without quality descriptor
struct SMeasuredValueNVA2
{
	int iOffset;
	_NVA NVA;
	SMeasuredValueNVA2()
	{
		memset(this,0,sizeof(SMeasuredValueNVA2));
	}
};

//TYPE IDENT 30: M_SP_TB_1 Single-point information with time tag CP56Time2a
struct SSinglePointTimeTag
{
	int iOffset;
	_SIQ SIQ;
	SFepTime cp56TimeTag;
	SSinglePointTimeTag()
	{
		memset(this,0,sizeof(SSinglePointTimeTag));
	}
};

//TYPE IDENT 31: M_DP_TB_1 Double-point information with time tag CP56Time2a

struct SDoublePointTimeTag
{
	int iOffset;
	_DIQ DIQ;
	SFepTime cp56TimeTag;
	SDoublePointTimeTag()
	{
		memset(this,0,sizeof(SDoublePointTimeTag));
	}
};

//TYPE IDENT 32: M_ST_TB_1 Step position information with time tag CP56Time2a

struct SStepPositionTimeTag
{
	int iOffset;
	_VTI VTI;
	_QDS QDS;
	SFepTime cp56TimeTag;
	SStepPositionTimeTag()
	{
		memset(this,0,sizeof(SStepPositionTimeTag));
	}
};

//TYPE IDENT 33: M_BO_TB_1 Bitstring of 32 bits with time tag CP56Time2a
struct  SBitstring32TimeTag
{
	int iOffset;
	_BSI BSI;
	_QDS QDS;
	SFepTime cp56TimeTag;
	SBitstring32TimeTag()
	{
		memset(this,0,sizeof(SBitstring32TimeTag));
	}
};

//TYPE IDENT 34: M_ME_TD_1 Measured value, normalized value with time tag CP56Time2a
struct SMeasuredValueNVATimeTag
{
	int iOffset;
	_NVA NVA;
	_QDS QDS;
	SFepTime cp56TimeTag;
	SMeasuredValueNVATimeTag()
	{
		memset(this,0,sizeof(SMeasuredValueNVATimeTag));
	}
};

//TYPE IDENT 35: M_ME_TE_1 Measured value, scaled value with time tag CP56Time2a
struct SMeasuredValueSVATimeTag
{
	int iOffset;
	_SVA SVA;
	_QDS QDS;
	SFepTime cp56TimeTag;
	SMeasuredValueSVATimeTag()
	{
		memset(this,0,sizeof(SMeasuredValueSVATimeTag));
	}
};

//TYPE IDENT 36: M_ME_TF_1 Measured value, short floating point number with time tag CP56Time2a
struct SMeasuredValueSFATimeTag
{
	int iOffset;
	_SFA SFA;
	_QDS QDS;
	SFepTime cp56TimeTag;
	SMeasuredValueSFATimeTag()
	{
		memset(this,0,sizeof(SMeasuredValueSFATimeTag));
	}
};

//TYPE IDENT 37: M_IT_TB_1 Integrated totals with time tag CP56Time2a

struct SCounterValueTimeTag
{
	int iOffset;
	_BCR BCR;
	SFepTime cp56TimeTag;
	SCounterValueTimeTag()
	{
		memset(this,0,sizeof(SCounterValueTimeTag));
	}
};

//TYPE IDENT 38: M_EP_TD_1 Event of protection equipment with time tag CP56Time2a
struct SEventProtectionTimeTag
{
	int iOffset;
	_SEP SEP;
	b_ui16 cp16TimeTag;
	SFepTime cp56TimeTag;
	SEventProtectionTimeTag()
	{
		memset(this,0,sizeof(SEventProtectionTimeTag));
	}
};

//TYPE IDENT 39: M_EP_TE_1 Packed start events of protection equipment with time tag CP56Time2a
struct SStartEventsTimeTag
{
	int iOffset;
	_SPE SPE;
	_QDP QDP;
	b_ui16 cp16TimeTag;
	SFepTime cp56TimeTag;
	SStartEventsTimeTag()
	{
		memset(this,0,sizeof(SStartEventsTimeTag));
	}
};

//TYPE IDENT 40: M_EP_TF_1 Packed output circuit information of protection equipment with time tag
struct SCircuitInformationTimeTag
{
	int iOffset;
	_OCI OCI;
	_QDP QDP;
	b_ui16 cp16TimeTag;
	SFepTime cp56TimeTag;
	SCircuitInformationTimeTag()
	{
		memset(this,0,sizeof(SCircuitInformationTimeTag));
	}
};
//TYPE IDENT 110: P_ME_NA_1 Parameter of measured values, normalized value
struct SParameterNVA
{
	int iOffset;
	_NVA NVA;
	_QPM QPM;
	SParameterNVA()
	{
		memset(this,0,sizeof(SParameterNVA));
	}
};
//TYPE IDENT 111: P_ME_NB_1 Parameter of measured values, scaled value
struct SParameterSVA
{
	int iOffset;
	_SVA SVA;
	_QPM QPM;
	SParameterSVA()
	{
		memset(this,0,sizeof(SParameterSVA));
	}
};
//TYPE IDENT 112: P_ME_NC_1 Parameter of measured values, short floating point number
struct SParameterSFA
{
	int iOffset;
	_SFA SFA;
	_QPM QPM;
	SParameterSFA()
	{
		memset(this,0,sizeof(SParameterSFA));
	}
};
//TYPE IDENT 113: P_AC_NA_1 Parameter activation
struct SParameterAct
{
	int iOffset;
	_QPA QPA;
	SParameterAct()
	{
		memset(this,0,sizeof(SParameterAct));
	}
};
//~~~~~~~~~~~~~~~~~~~~~~~~~datadef-end~~~~~~~~~~~~~~~~~~~~~~~~
#endif // _FEPDATASTRUCT_H
