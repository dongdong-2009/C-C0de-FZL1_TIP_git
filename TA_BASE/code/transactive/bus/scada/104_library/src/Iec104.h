#ifndef _PROTOCOL_IEC104_H
#define _PROTOCOL_IEC104_H


#include "Iec104Def.h"
#include "ProtocolBase.h"
#include <deque>


using namespace std;

#define SPI_OFFSET 0
#define DPI_OFFSET 0
#define OV_OFFSET  0
#define CA_OFFSET  6
#define CY_OFFSET 5
#define EI_OFFSET 3
#define SRD_OFFSET 5 
#define SIE_OFFSET 4
#define SL3_OFFSET 3
#define SL2_OFFSET 2
#define SL1_OFFSET 1
#define CL3_OFFSET 3
#define CL2_OFFSET 2
#define CL1_OFFSET 1
#define GC_OFFSET 0
#define LPC_OFFSET 6
#define POP_OFFSET 7

#define IV_OFFSET  7
#define NT_OFFSET 6
#define SB_OFFSET 5
#define BL_OFFSET 4

#define BIT_SIZE1  0X01
#define BIT_SIZE2  0X03
#define BIT_SIZE5  0X1F
#define BIT_SIZE6  0X3F

struct SFileTransfer
{
	b_ui8   cmd_state;   
	b_ui16 file_name;   
	b_ui8   section_name; 
	b_ui8   segment_name;
	b_bool finish;                
	SFileTransfer()
	{
		memset(this,0,sizeof(SFileTransfer))	;
	}
};

struct SResult
{
	int iRtu;
	int iOffset;
	int iResult;
	union{
		 int iVal;
	 	float fVal;
	 	b_ui8 xVal[4];
	}value;

};
//call data of fixed value
struct SValueData
{
	b_ui8 bAreaNo;
	b_ui8 bValueNo;
	float   fVal;
};
//result of calling fixed value
struct SExCallValueResult
{
	b_ui8 bResult;
	b_ui8 bCount;
	b_ui8 bTotalNo;
	SValueData sData[24];
};
//result of modifying fixed value
struct SExResult
{
	int iRtu;
	b_ui8 bAreaNo;
	b_ui8 bResult;
	SExResult()
	{
		memset(this,0,sizeof(SExResult));
	}
		
};
//Record configuration parameters of 104 protocol

struct SHostPara
{
	b_ui8 groupId;
	std::string hostAddr;
	b_ui16   port;
	SHostPara()
	{
		groupId=0;
		hostAddr="127.0.0.1";
		port=2404;
	};
};

struct SIec104_LINK_PARA
{
	b_ui8		T0; 
	b_ui8		T1;
	b_ui8		T2;
	b_ui8		T3;
	b_i32		T4;
	b_ui8		K;
	b_ui8		W;
	b_ui8          hostNum;
	std::string   strLocalAddr;
	SHostPara  *m_hostPara;
	SIec104_LINK_PARA()
	{
		K= IEC104_STD_K;
		W= IEC104_STD_W;
		T0= IEC104_STD_T0 ;
		T1= IEC104_STD_T1;
		T2= IEC104_STD_T2;
		T3= IEC104_STD_T3;
		T4 = IEC104_EX_T4;
		hostNum = 0;
		strLocalAddr="0.0.0.0";
		m_hostPara= new SHostPara[4];
	}
	~SIec104_LINK_PARA()
	{
		delete[] m_hostPara;
	}
};
//parameter structure at application layer
struct SIec104_APP_PARA
{
	b_ui8		CotLen;//Cause Of Transfer
	b_ui8		AsduAddrLen;//the length of common address
	b_ui8		InfAddrLen;//the length of information address
	b_ui16           rtuAddr;
	SIec104_APP_PARA()
	{
		CotLen = IEC104_STD_COT_LEN;
		AsduAddrLen =IEC104_STD_COA_LEN;
		InfAddrLen = IEC104_STD_IAD_LEN;
		rtuAddr = 1;
	};
};
struct SIec104Para
{
	SIec104_LINK_PARA LinkPara;
	SIec104_APP_PARA	 AppPara;
};

/**
*class CIec104_Apci used to operate the control field of 104 protocol packets
*the control field of 104 protocol packet, 6 bytes
*/
class CIec104_Apci
{
public:
	CIec104_Apci(const b_ui8* pBuf);
	~CIec104_Apci(void);
	CIec104_Apci(int iFrType, int iRxSeq=0, int iTxSeq=0, int iLen=4 );
//return the received number
	int GetRxSeq();
//return the sent unmber
	int GetTxSeq();	
//return the type
	int GetFrType();
	b_ui8 * GetApciAddr();
	b_ui8	m_pBuf[IEC104_APCI_SIZE];
};

/**
*class CIec104_Asdu used to operate the data of 104 protocol packets
*Just the I format takes data
*/
class CIec104_Asdu
{
public:
	CIec104_Asdu():m_pBuf(NULL),m_iLen(0){};
	CIec104_Asdu(const b_ui8* pBuf, int iLen);
	CIec104_Asdu(SIec104_APP_PARA para,int iTI,int iVSQ,int iCOT, int iCOA,int iInfAddr, b_ui8* pBuf, int iLen);
	~CIec104_Asdu(void);
	b_ui8 * GetAsduAddr(){
		return m_pBuf;
	};
	b_ui8*	m_pBuf;
	int		m_iLen;
};
/**
*class CIec104_Apdu
*APDU is composed by one APDU and zero to one ASDU
*/

class CIec104_Apdu
{
public:
	int GetFrType();
	int GetTxSeq();
	int GetRxSeq();
	int GetSize();
	CIec104_Apdu():m_pBuf(NULL){};
	CIec104_Apdu(const b_ui8* pApci, const b_ui8* pAsdu,int iAsduLen);
	~CIec104_Apdu(void);
	b_ui8 * m_pBuf;
};

//record the packets which are sent, but not be confirmed
class CIec104TxItem
{
public:
	b_ui8 m_iCnt;
	b_ui8* m_pBuf;
	CIec104TxItem();
	CIec104TxItem(const b_ui8* pBuf,int iLen,int iCnt=15);
	~CIec104TxItem(void);
	b_ui8 DeCount(); 
	int GetTxSeq();
};

typedef deque< CIec104TxItem * > TXDEQUE;

struct SIec104Data
{
	SIec104_LINK_PARA	*LinkPara;
	SIec104_APP_PARA 	AppPara;
	
	b_i32		T1Cnt; //T1-T3
	b_i32		T2Cnt;
	b_i32		T3Cnt;
	b_i32		T4Cnt;
	b_i32		T1Start;
	b_i32		T2Start;
	b_i32		T3Start;
	b_i32		T4Start;
	
	int		RxSeq;	// Recieve sequence
	int		TxSeq;	// Transfer sequence
	int		AckSeq;	// Ackknowlodge sequence for send I frame 
	
	b_ui8		LinkState;	//UNRESET=0; CONN; DBCONN; DBWAITPEER;WAITPEER; 
	b_ui8		LinkCmd;
	b_ui8		AppState;	//WAITALLDATA...
	b_ui8		NextCmd;	//TestAct,TestCon,StartDtAct,StartDtCon,StopDtAct,StopDtCon...
	b_ui8		CtrlCmd;	//Select, Exec, Cancel
	SFileTransfer     FileTransfer;

	TXDEQUE*	pTxQueue; //hold the transfered but not acked items' queue
	int			LastAckSeq;			// the current not acknowledged I Frame Sequence for received 
	
	b_bool	StartFlag;	//the flag identify the first enter the loop
   	 b_ui16	btRtuAddr;
	
	SIec104Data()
	{
		pTxQueue = new TXDEQUE;
		LinkPara=NULL;
		LinkState=IEC104_LK_STATE_UNRESET;
		LinkCmd  =IEC104_LK_CMD_NONE;
		T1Cnt= IEC104_STD_T1;
		T2Cnt= IEC104_STD_T2;
		T3Cnt= IEC104_STD_T3;
		T4Cnt = IEC104_EX_T4;
		T1Start = IEC104_TIME_STOP;
		T2Start = IEC104_TIME_STOP;
		T3Start = IEC104_TIME_STOP;
		T4Start = IEC104_TIME_STOP;
		AckSeq = 0;
		RxSeq = 0;
		TxSeq = 0;
		
		LastAckSeq = -1;
		
		StartFlag=b_true;
		NextCmd=IEC104_CMD_NON;
		CtrlCmd = IEC104_CTRL_NON;
		btRtuAddr = 1;
		
		AppPara.AsduAddrLen = IEC104_STD_COA_LEN;
		AppPara.CotLen = IEC104_STD_COT_LEN;
		AppPara.InfAddrLen = IEC104_STD_IAD_LEN;
		
		AppState = IEC104_APP_STATE_IDLE;

		
	};
	
	~SIec104Data()
	{
		if ( pTxQueue != NULL)
			if ( pTxQueue->empty() )
			{
				delete pTxQueue;
				int a=0;
			}
			else
			{
				TXDEQUE::iterator pi;
				CIec104TxItem* txItem;
				for(pi= pTxQueue->begin();  pi !=pTxQueue->end(); pi++)
				{
					txItem = *pi;				
					pTxQueue->pop_front();
					delete txItem; // delete the stored Tx frame
				}
				pTxQueue->clear();
				delete pTxQueue;
			}
	};
};

 /**
 *class CIec104
 *
 */
class CIec104 : public CProtocolBase  
{
public:
	void SendCommand();
	int ExpBufData();
	int Explain();
	void Init();
	CIec104();
	virtual ~CIec104();
public:
	int AppTxAsduType_45(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bSco);
	int AppTxAsduType_46(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bDco);
	int AppTxAsduType_47(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bRco);
	int AppTxAsduType_48(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui16 Nva,b_ui8 bQos);
	int AppTxAsduType_49(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_i16 Sva,b_ui8  bQos);
	int AppTxAsduType_50(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, float Sfa,b_ui8  bQos);
	int AppTxAsduType_51(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui32 bitstring32);
	int AppTxAsduType_58(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bSco,b_ui8 *bTimeTag);
	int AppTxAsduType_59(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bDco,b_ui8 *bTimeTag);
	int AppTxAsduType_60(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bRco,b_ui8 *bTimeTag);
	int AppTxAsduType_61(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui16 Nva,b_ui8  bQos,b_ui8 *bTimeTag);
	int AppTxAsduType_62(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_i16 Sva,b_ui8  bQos,b_ui8 *bTimeTag);
	int AppTxAsduType_63(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, float Sfa,b_ui8  bQos,b_ui8 *bTimeTag);
	int AppTxAsduType_64(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui32 bitstring32,b_ui8 *bTimeTag);
	int AppTxAsduType_100(int rtuAddr, b_ui8 bCot=6, b_ui8 btQoi=20);	
	int AppTxAsduType_101(int rtuAddr ,b_ui8 bCot=6,b_ui8 btQcc=1);
	int AppTxAsduType_102(int rtuAddr,int infoAddr);
	int AppTxAsduType_103(int rtuAddr);
	int AppTxAsduType_105(int rtuAddr,b_ui8  btQrp);
	int AppTxAsduType_107(int rtuAddr, int infoAddr, b_ui16 bTsc );
	int  AppTxAsduType_110(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm);
	int  AppTxAsduType_111(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm);
	int  AppTxAsduType_112(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm);
	int  AppTxAsduType_113(int rtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui8 bQpa);


	int AppRxAsdu_1(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_3(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_5(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_7(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_9(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_11(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_13(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_15(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_20(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_21(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_30(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_31(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_32(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_33(const b_ui8 *asdu, b_ui8 size);			
	int AppRxAsdu_34(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_35(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_36(const b_ui8 *asdu, b_ui8 size);	
	int AppRxAsdu_37(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_38(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_39(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_40(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_45(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_46(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_47(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_48(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_49(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_50(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_51(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_58(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_59(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_60(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_61(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_62(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_63(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_64(const b_ui8 *asdu, b_ui8 size);
	int AppRxAsdu_110(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_111(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_112(const b_ui8 * asdu, b_ui8 size);
	int AppRxAsdu_113(const b_ui8 * asdu, b_ui8 size);
	int checkAsduSize(b_ui8 bTi,const b_ui8 bVsq,b_ui8 size);
//--------------------------follows are used by 103 extension----------------------------//

	int  AppTxExCallValue(int comnAddr,b_ui8 prtectSerialNo,b_ui8 ngd,b_ui8 group,b_ui8 areaFlag);//145

	int  AppTxExModifyValueAreaNo(int comnAddr,b_ui8 prtecSerialNo,b_ui8 areaNo,b_ui8 cmdType);//147

	int     AppTxExModifyValue(int comnAddr,b_ui8 prtecSerialNo,b_ui8 areaNo,b_ui8 cmdType,float* fBuf,b_ui8 ngd,b_ui8 beginNo);//147

	void  AppRxCallValueConf(const b_ui8 *asdu, b_ui8 size); //146

	void   AppRxModifyValueConf(const b_ui8 *asdu, b_ui8 size);//148

	void  AppRxProtectAction(const b_ui8 *asdu, b_ui8 size);//140
	
//------------------------follows are used to file transfer---------------------------//
	int   AppTxCallFile(int rtuAddr,int infoAddr, b_ui8 cmdType,int fileName,b_ui8 secName);//122

	void  AppRxDir(const b_ui8 *asdu,b_ui8 size);//126

	void  AppRxFileReady(const b_ui8 *asdu,b_ui8 size);//120

	void  AppRxSectionReady(const b_ui8 *asdu,b_ui8 size);//121

	void AppRxSegment(const b_ui8 *asdu,b_ui8 size);//125

	void  AppRxLastSe(const b_ui8 *asdu,b_ui8 size);//123

	int  AppTxAffirmFile(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag);//124

	void AppRxSvConf(const b_ui8 *apdu, b_ui8 size);//48 49 50 set point command confirmation


	int AppTxCallGrpKwh(int group);       //101

	int AppTxCallGrpData( int iGroup );//  100

	void AppSyncTimeCoding( b_ui8 *const buf );
	int AppCheckCommand();
	int AppCheckCmdFromAgent();
	int  LinkCheckCommand();
	int SendLinkCmd();
	int LinkTxFrameI( const b_ui8 *pAsdu, b_ui8 size, b_ui8 frmtype );
	int LinkTxFrameU(b_ui8 iFrameType);
	int LinkTxFrameS();
	int AppRxAsdu_70( const b_ui8 *pAsdu, b_ui8 iSize);//70

	int AppRxAsdu_107(const b_ui8* apdu, b_ui8 size);//107

	int AppRxAsdu_105(const b_ui8* pBuf, b_ui8 iSize);//105

	int AppRxAsdu_103(const b_ui8  *apdu, b_ui8 size);//103
	
	int AppRxAsdu_102(const b_ui8  *asdu, b_ui8 size );//100

	int AppRxAsdu_101(const b_ui8  *asdu, b_ui8 size );//101

	int AppRxAsdu_100(const b_ui8  *asdu, b_ui8 size );//100

	void AppRxAsdu(const  b_ui8* pBuf, b_ui8 iSize);

	b_bool GetCurTm(SFepTime &  t);
	b_bool GetCurTm( char * pBuf);
	b_bool GetTmStmp(const b_ui8 *pBuf,SFepTime *pTmStmp);

	int LinkRxFrameI(SIec104Data *p, b_ui8* pApdu);
	int LinkRxFrameU(SIec104Data *p, const b_ui8 *pApdu);
	b_bool LinkRxFrameS(SIec104Data *p,  const b_ui8* pApdu);
	int LinkRxApdu();
	b_bool GetSendDataPackage(char * buff,int *len);
	b_bool LoadProtocolCfg(SIec104Data* pPara);
	void setStartFlag(){
		if(m_pIec104Data)
			m_pIec104Data->StartFlag=b_true;
	};
	void setStopFlag(){
		if(m_pIec104Data)
			m_pIec104Data->StartFlag=b_false;
	};
	SIec104Data	*	m_pIec104Data;
	b_ui8			m_bRecvFinish;
private:
	b_ui16 p104tohost16(const b_ui16 src);
	b_ui32 p104tohost32(const b_ui32 src);
	//int checkAsdu(b_ui8 vsq ,b_ui8 infoSize,b_ui8 size);
	void Close();
	void initPtr();
	void adjustSeq(b_ui8 *buf,int len);
	void printDebug(char* buf);
	int getRtuAddr(const b_ui8 *pBuf);
	b_i32 getTimeCnt();
	void  resetTimer(int timer);
	void updateTimer();
	void stopTimer(int Timer);
};

#endif // !defined(AFX_IEC104_H__A56F4F46_9AA3_4CA4_9682_40F0D23D1A45__INCLUDED_)
