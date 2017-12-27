

#ifndef  _PROTOCOLBASE_H
#define  _PROTOCOLBASE_H

#include "FepDataStruct.h"
class IEC104_Event_Handler;
#define SRC_ADDR  1 

// define received frame type from channel
#define RX_FRAME_CHECKERR      1
#define RX_FRAME_CORRECT       2
#define RX_FRAME_SYNC          3

// defines the formats which are handled by protocol
#define RX_FRAME               1
#define TX_FRAME               2
#define EXP_FRAME              3

#define MAXPACKANA		       400
#define MAXPACKDIG		       400
#define MAXPACKPUL		       400
#define MAXPACKSOE		       400
#define MAXPACKALARM	       400

#define MAXPACKFRAME              100  // the number of packets which are explained at one loop

/*
*class  CProtocolBase
*the base class of protoco, defining the fuctiong which are not related with the protocol
*/
class LogThread;
class  CProtocolBase
{
public:
	CProtocolBase();
	virtual ~CProtocolBase();

	virtual void Init()=0;
	virtual int Explain() = 0;
	virtual int ExpBufData() = 0;   // reserved 
	virtual void SendCommand() = 0;	 // reserved 
	virtual void setStartFlag()=0;
	virtual void setStopFlag()=0;

	void ResetLink();
	void SetHandler(IEC104_Event_Handler *handler);
	//return the process pointer of the responding protocol

	IEC104_Event_Handler* GetHandler();
	void ProcessLinkReset(){};
	//get the bytes number of the receive buffer

	int  GetBufDataLen(int link=0);
	//get data from receive buffer

	b_bool GetBufData(b_ui8 *pBuf,int iNum,b_bool bFlag=true);
	b_bool GetBufData(b_ui8 *pBuf);
	//moving the head pointer of the buffer

	b_bool MoveBufHeadp(int iOffset);
	//get the free space of send buffer

	b_ui32 GetBufFree();
	// writting data to the send buffer

	b_bool WriteBuff( b_ui8 *pBuf, int iNum );

   	void  PostFrameMessage(int channelID,int iDataType, int iDataLen, void *pBuf);
    void AddMsgFrame( SMsgFrame *pMsg,int iNum);
//the functions are used to testing


public:	
	void PrintInfo(char * para,b_ui8 *buf, int iLen);
	time_t ConversionTime(SFepTime ftime);
	b_bool ConnectSource();
	b_bool SetChanAttr( CChannel *pChannel);
	int m_iChannelIndex;
	CChannel* m_pChannel;
	//define the processor of the protocol

	IEC104_Event_Handler *m_handler;
	//the objects are used to record

	//LogThread * m_log;
	
	b_bool m_bInitFinished;
	b_bool m_SourceCon;
//	C_Source m_Source;
	

	SMsgFrame m_msgFrame[MAXPACKFRAME];  // defines message number explained per loop.
    int m_iExpFrameNum;
protected:
	CCycleQueue *m_txBuf;
	CCycleQueue *m_rxBuf;

};

#endif //_PROTOCOLBASE_H
