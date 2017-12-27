#include <sstream>
#include "Iec104.h"
#include "FepDataStruct.h"
//#include "Profile.h"
#include <stdio.h>
#include "Iec104Api.h"
CCycleQueue::CCycleQueue(int size)
:m_head(0),
m_tail(0),
m_maxSize(size),
m_headMutex(false),
m_tailMutex(false)
{
	m_buf = new b_ui8[size];
}

CCycleQueue::~CCycleQueue()
{
	delete m_buf;
}
int CCycleQueue::getLength()
{
	return ( m_tail + m_maxSize - m_head) % m_maxSize;
}
 int CCycleQueue::getFreeLength()
 {
 	return m_maxSize -1-this->getLength();
 }
 int CCycleQueue::inQueue(b_ui8 * buf, int count)
 {
	 esca::CLock lock(&m_tailMutex);
	if( count < m_maxSize- m_tail)
	{
		memcpy(&m_buf[m_tail], buf, count);
	}
	else
	{
		int tmp = m_maxSize  - m_tail;
		memcpy(&m_buf[m_tail], buf, tmp);
		memcpy(m_buf, buf+tmp, count - tmp);
	}
	m_tail =(m_tail +  count) % m_maxSize;
	return 0;
 }
int CCycleQueue::deQueue(b_ui8* buf,int count, b_bool bMovHead)
{
	CLock lock(&m_headMutex);
	if(count <=m_maxSize  - m_head)
	{
		memcpy(buf, &m_buf[m_head], count);
	}
	else
	{
		int tmp = m_maxSize  - m_head;
		memcpy(buf, &m_buf[m_head], tmp);
		memcpy(buf + tmp, m_buf, count - tmp);
	}
	if(bMovHead)
		m_head = (m_head + count )% m_maxSize;
	return count;
	
}
b_bool CCycleQueue::isFull()
{
	return (m_tail+1) % m_maxSize == m_head;
}
b_bool CCycleQueue::isEmpty()
 {
 	return m_tail == m_head;
 }
void CCycleQueue::resetQueue()
{
	CLock lockHead(&m_headMutex);
	CLock lockTail(&m_tailMutex);
	m_tail = m_head = 0;
}



//-----------------------------------------------------------------------
CChannel::CChannel()
:m_bRunFlag(false),
m_bConnected (false),
m_bReset(false),
m_bCanConnect(true)
{
	m_rxBuf =NULL;
	m_txBuf = NULL;
	m_pProtocol=NULL;
	m_pStaticPara= NULL;
	m_workMode = IEC104_WORKMODE_STANDBY;
	m_host = new SHostPara();
	m_localAddr="0.0.0.0";
}

CChannel::~CChannel()
{
	if(m_rxBuf)
		delete m_rxBuf;
	if(m_txBuf)
		delete m_txBuf;
	if(m_pProtocol)
		delete m_pProtocol;
	if(m_pStaticPara)
		delete m_pStaticPara;
	if(m_host)
		delete m_host;
}

int CChannel::Connect()
{
	int ret;
	try{
		mCommandSocket.Create();
		mCommandSocket.Bind(0, m_localAddr);
		ret = mCommandSocket.Connect(m_host->hostAddr,m_host->port);
		return ret;
	}
	catch(CFtpException&cfe)
	{
		std::string logStr;
		logStr+=cfe.SysText();
		this->logChannelMsg(LOG_LEVEL_ERROR, logStr);
		return -1;
	}
	m_bReset=false;
	return ret;
}

int CChannel::Read()
{
	unsigned char buff[IEC104_BUFF_SIZE];
	int ifreeLen=0;
//To determine whether the queue is  full
	while(1)
	{
	
		ifreeLen = m_rxBuf->getFreeLength();
		if(ifreeLen==0)// is full
		{
#ifdef WIN32
			Sleep(100);
#else
			usleep(100);
#endif
		}	
		else if(ifreeLen>0)
			break;
		else
		{
			std::string logStr="Read data when buffer free len < 0";
			this->logChannelMsg(LOG_LEVEL_ERROR, logStr);
		}
	}

	int recvLen = mCommandSocket.Receive((char*)buff, ifreeLen>IEC104_BUFF_SIZE?IEC104_BUFF_SIZE:ifreeLen);
	if (recvLen<0)
	{
		return recvLen;
	}
	m_rxBuf->inQueue(buff, recvLen);
	return 0;
}

b_bool CChannel::Disconnect()
{
	try
	{
		mCommandSocket.Close(false);
	}
	catch(CFtpException&cfe)
	{
		std::string logStr;
		logStr+=cfe.SysText();
		this->logChannelMsg(LOG_LEVEL_ERROR, logStr);
		return b_false;
	}
	m_bConnected=false;
	return b_true;
}

int CChannel::Write()
{
	char buff[IEC104_BUFF_SIZE];
    int ret=0;
	CIec104 * mp = (CIec104*)m_pProtocol;
	int len;
	len = m_txBuf->getLength();
	if(len>0)
	{
		if(m_txBuf->deQueue((b_ui8*)buff, len<IEC104_BUFF_SIZE?len:IEC104_BUFF_SIZE))
		{
			try
			{
				ret=mCommandSocket.Send((const char*)buff,len);
				return ret;
			}
			catch(CFtpException& cfe)
			{
				std::string logStr;
				logStr+=cfe.SysText();
				this->logChannelMsg(LOG_LEVEL_ERROR, logStr);
				return -2;
			}
		}
	}
	return ret;
}

bool CChannel::init()
{

	TCP_para.sockAddr.sin_port = TCP_para.m_port;//
	int mAddr = inet_addr(TCP_para.netAddr);
	TCP_para.sockAddr.sin_addr.s_addr = mAddr;
	TCP_para.sockAddr.sin_family  = AF_INET;
	TCP_para.timeout.tv_sec = 1;
	TCP_para.timeout.tv_usec= 0;


	m_txBuf = new CCycleQueue(TXBUFLEN);
	m_rxBuf = new CCycleQueue(RXBUFLEN);

	m_pProtocol = new CIec104;

	m_bRunFlag = true;
	m_hostStatus = -1;

	return true;
}

b_bool CChannel::LoadProtocol( )
{
	m_pProtocol = new CIec104;
	m_pProtocol->Init();
	return b_true;
}
CProtocolBase*CChannel:: getProtocol()
{
	return m_pProtocol;
}

void CChannel::FreeProtocol()
{
	delete m_pProtocol;
}

void CChannel::Explain()
{
	// explanation
}

bool CChannel::loadConfig()
{
	 return true;
}

void CChannel::setLocaAddr(std::string localAddr)
{
	m_localAddr = localAddr;
}

void CChannel::setLinkId(b_ui8 id)
{
	m_linkId = id;
}
b_ui8 CChannel::getLinkId()
{
	return m_linkId;
}

b_ui8  CChannel::getHostStatus()
{
	return m_hostStatus;
}
void   CChannel::setHostStatus(b_ui8 hostStatus)
{
	if((m_hostStatus==HOST_STATUS_SLAVE) &&(hostStatus==HOST_STATUS_MASTER))
	{
		b_ui8 *message=new b_ui8[3];
		message[0]= CAUSE_STATUS_SLAVE2MASTER;
		message[1]=getLinkId();
		message[2]= getGroupId();
		m_pManager->PostMessage(3, (void *)message);
		
	}
	else if((m_hostStatus==HOST_STATUS_MASTER) &&(hostStatus==HOST_STATUS_SLAVE))
	{
		b_ui8 *message=new b_ui8[3];
		message[0]= CAUSE_STATUS_MASTER2SLAVE;
		message[1]=getLinkId();
		message[2]= getGroupId();
		m_pManager->PostMessage(3, (void *)message);
	}
	m_hostStatus =hostStatus;
}

int CChannel::setManager(CAdaptee *pManager)
{
	if(pManager==NULL)
		return -1;
	m_pManager = pManager;
	return 0;
}

void CChannel::setWorkMode(int mode)
{
	if((m_workMode==IEC104_WORKMODE_NOMAL)&&(mode==IEC104_WORKMODE_STANDBY))
	{
		getProtocol()->setStopFlag();
	}
	else if((m_workMode==IEC104_WORKMODE_STANDBY)&&(mode==IEC104_WORKMODE_NOMAL))
	{
		getProtocol()->setStartFlag();
	}
	m_workMode = mode;
	IEC104_API*api=IEC104_API::getInstance();
	api->handler_->handleLinkModeStatus(getGroupId(),m_host->hostAddr,mode);
}
int  CChannel::getWorkMode()
{
	return m_workMode;
}
int  CChannel::setHost(std::string ipAddr,int port,b_ui8 groupId)
{
	m_host->hostAddr=ipAddr;
	m_host->port = port;
	m_host->groupId=groupId;
	return 0;
}
b_ui8  CChannel::getGroupId()
{
	return m_host->groupId;
}
b_bool CChannel::isConnect()
{
	return m_bConnected;
}

void CChannel::logChannelMsg(int level,std::string &msg)
{
	std::ostringstream logStream;

	logStream<<"[ Grp_"<<static_cast<int>(this->m_host->groupId)<<":";
	logStream<<this->m_host->hostAddr<<":";
	logStream<<this->m_host->port<<" ] ";
	logStream<<msg;

	std::string logString = logStream.str();
	IEC104_API::getInstance()->logRecord(level, logString);
}

