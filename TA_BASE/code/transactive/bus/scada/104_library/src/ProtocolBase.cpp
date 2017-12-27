#include "ProtocolBase.h"
#include "Iec104Api.h"
#include "Iec104Event.h" 
CProtocolBase::CProtocolBase()
:m_iChannelIndex(0),
m_pChannel(NULL),
m_bInitFinished(true),
m_iExpFrameNum(0),
//m_pRxBuf(NULL),
//m_pTxBuf(NULL),
m_rxBuf(NULL),
m_txBuf(NULL)
{
	memset(m_msgFrame,0,MAXPACKFRAME);
	m_SourceCon=false;
	m_handler = NULL;
	//m_log = LogThread::instance();
}

CProtocolBase::~CProtocolBase()
{
}

void CProtocolBase::ResetLink()
{
	m_pChannel->Reset();
}


void CProtocolBase::SetHandler(IEC104_Event_Handler *handler)
{
	m_handler = handler;
}

IEC104_Event_Handler* CProtocolBase::GetHandler()
{
	return m_handler;
}

//get the length of received data in buffer
int CProtocolBase::GetBufDataLen(int link)
{
	int len=0;	
	if(m_rxBuf == NULL)
		return -1;
	len= len = m_rxBuf->getLength();
	return len;
}
//if bFlag equal true,get data and delete it from buffer;else just only get data
b_bool CProtocolBase::GetBufData(b_ui8 *pBuf,int iNum,b_bool bFlag)
{
	if( iNum>m_rxBuf->getLength()|| iNum<0) 
		return b_false;
	if(pBuf == NULL) 
		return b_false;
	
	m_rxBuf->deQueue(pBuf, iNum,bFlag);
	
	return b_true;
}

b_bool CProtocolBase::GetBufData(b_ui8 *pBuf)
{
	
	return b_true;
}

b_bool CProtocolBase::MoveBufHeadp(int iOffset)
{
	
	return b_true;
}

b_ui32 CProtocolBase::GetBufFree()
{
	return 0;
}

b_bool CProtocolBase::WriteBuff( b_ui8 *pBuf, int iNum )
{
	PrintInfo("Send:",pBuf,iNum);
	if( iNum > m_txBuf->getFreeLength() || iNum <=0)
		return b_false;
	if(pBuf == NULL)
		return b_false;
	
	m_txBuf->inQueue(pBuf, iNum);

	char tmp[100];
	sprintf(tmp,"put data to tx buffer");
	return b_true;
}

void CProtocolBase::PostFrameMessage(int channelID,int iDataType, int iDataLen, void *pBuf)
{
}

void CProtocolBase::AddMsgFrame( SMsgFrame *pMsg,int iNum)
{
}

/*
*setting the parameters of protocol,sharing the caching of sending and receiving of CChannel
*/
b_bool CProtocolBase::SetChanAttr(CChannel *pChannel)
{
	if(pChannel == NULL)
		return b_false;
	
	m_pChannel = pChannel;
	m_iChannelIndex = pChannel->m_iChannelIndex;
	m_rxBuf  = m_pChannel->m_rxBuf;
	m_txBuf = m_pChannel->m_txBuf;
	
	Init();
	return b_true;
}

b_bool CProtocolBase::ConnectSource()
{
	return m_SourceCon;
}

time_t CProtocolBase::ConversionTime(SFepTime ftime)
{
	struct tm timeps ;
	timeps.tm_yday = ftime.year;
	timeps.tm_mon  = ftime.month;
	timeps.tm_yday = ftime.day;
	timeps.tm_hour = ftime.hour;
	timeps.tm_min  = ftime.minute;
	timeps.tm_sec  = ftime.second;
	timeps.tm_isdst= ftime.millisecond;

	return mktime(&timeps);
}

void CProtocolBase::PrintInfo(char *para, b_ui8 *buf, int iLen)
{
	char tmp[100];
	std::string logString;
	logString += para;
	for(int j=0;j<iLen;j++)
	{
		sprintf(tmp,"%02x ",buf[j]);
		logString += tmp;
	}
	this->m_pChannel->logChannelMsg(LOG_LEVEL_INFO,logString);
}



