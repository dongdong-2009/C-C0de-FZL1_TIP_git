#include "Adaptee.h"
#include <stdio.h>
#include "Iec104Api.h"
#include "Threads.h"
CAdaptee::CAdaptee()
:m_ade_conthr(NULL),
m_workMode(IEC104_WORKMODE_STANDBY)
{
#ifdef __OS_WIN32_
	WSADATA wsaData;
	if(WSAStartup(0x202,&wsaData) < 0)
	{
		std::string logStr="WSAStartup For Win32 Error!";
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_ERROR,logStr);
	}
#endif
	
}


CAdaptee::~CAdaptee()
{
	if(m_ade_conthr)
		delete[] m_ade_conthr;
	while (!m_messages.empty())
	{
		esca::CMessage msg = m_messages.front();
		m_messages.pop();
		delete msg.m_data;
	}
}
void CAdaptee::setWorkMode(int mode)
{
	if((m_workMode==IEC104_WORKMODE_STANDBY)
		&&(mode==IEC104_WORKMODE_NOMAL))
	{
		b_ui8 *message=new b_ui8[3];
		message[0] = CAUSE_WORKMODE_MONITOR2CONTROLER;
		message[1] = -1;
		message[2] = -1;
		m_workMode = mode;
		PostMessage(3, (void *)message);
	}
	else if((m_workMode==IEC104_WORKMODE_NOMAL)
		&&(mode==IEC104_WORKMODE_STANDBY))
	{
		b_ui8 *message=new b_ui8[3];
		message[0] = CAUSE_WORKMODE_CONTROLER2MONITOR;
		message[1] = -1;
		message[2] = -1;
		m_workMode = mode;
		PostMessage(3, (void *)message);
	}
}

int CAdaptee::chooseWorkThread(const b_ui8 *data,b_ui8 len)
{
	if(m_workMode==IEC104_WORKMODE_STANDBY)
	{
		m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_STANDBY);
		return 0;
	}
	if(len<3)
		return -2;
	int chooseIndex=-1,firstIndex=-1,i=0;
	IEC104_API *api=IEC104_API::getInstance();
	int connectNum = api->m_linkPara.hostNum;
	b_ui8 bCause = data[0];//causes of events which may lead to link change
	b_ui8 bProblemLinkId = data[1];//the linkID of the link who report event
	b_ui8 bProblemGrpId = data[2];//the groupID of the link who report event
	int grpId;

	switch(bCause)
	{
		case CAUSE_LINK_DISCONNECT:
			if(connectNum==1)
			{
				m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_NOMAL);
				return 0;
			}
			for(i=0;i<connectNum;i++)
			{
				if(m_ade_conthr[i].GetChannel()->isConnect())//find  lowest linkID of the link which is connected
				{
					firstIndex = i;
					break;
				}
			}
			if(firstIndex==-1)//all thread is disconnect
			{
				return -1;
			}
			chooseIndex = -1;
			//find the groupID of current link
			grpId = IEC104_API::getInstance()->m_linkPara.m_hostPara[m_currentWorkIndex].groupId;
			for(i=firstIndex;i<connectNum;i++)
			{
				if((m_ade_conthr[i].GetChannel()->isConnect())&&
					(m_ade_conthr[i].GetChannel()->getGroupId() ==grpId)&&
					(i!=m_currentWorkIndex))//find the other linkID which has the same groupID with current link
					chooseIndex = i;
			}
			if(chooseIndex==-1)
			{
				for(i=firstIndex;i<connectNum;i++)
				{
					if((m_ade_conthr[i].GetChannel()->isConnect())&&
						(m_ade_conthr[i].GetChannel()->getGroupId() !=grpId)&&
						(i!=m_currentWorkIndex))//find the linkID which is not in the same group of current link
					{
						chooseIndex = i;
						break;
					}
				}
			}
			if(chooseIndex==-1)//can not find the other link which is connected
				return -1;
			changeWorkThread(chooseIndex, bCause);//change link
			return 0;
			break;
		case CAUSE_STATUS_SLAVE2MASTER:
			//the current link's groupID equal to the link's linkID who report event
			if(bProblemGrpId==api->m_linkPara.m_hostPara[m_currentWorkIndex].groupId) 
			{
				if(m_ade_conthr[m_currentWorkIndex].GetChannel()->isConnect())//nothing to do
					return 0;
				if((chooseIndex = chooseConnectLinkInGrp(bProblemGrpId))<0)
					return -1;
				changeWorkThread(chooseIndex, bCause);
				return 0;
			}
			else
			{
				if((chooseIndex = chooseConnectLinkInGrp(bProblemGrpId))<0)
					return -1;
				changeWorkThread(chooseIndex, bCause);
				return 0;
			}	
		case CAUSE_STATUS_MASTER2SLAVE:
			//the current link's groupID does not equal to the link's linkID who report event
			if(bProblemGrpId!=api->m_linkPara.m_hostPara[m_currentWorkIndex].groupId) 
			{
				if(m_ade_conthr[m_currentWorkIndex].GetChannel()->isConnect())//link has been changed using the other link at the same group,nothing to do
					return 0;
				if((chooseIndex = chooseConnectLinkNotInGrp(bProblemGrpId))<0)
					return -1;
				changeWorkThread(chooseIndex, bCause);
				return 0;
			}
			else
			{
				if((chooseIndex = chooseConnectLinkNotInGrp(bProblemGrpId))<0)
					return -1;
				changeWorkThread(chooseIndex, bCause);
				return 0;
			}
			break;
		case CAUSE_LINK_CONNECT:
			if((m_ade_conthr[m_currentWorkIndex].GetChannel()->isConnect())==b_true)
			{
				m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_NOMAL);
				return 0;
			}
			else
			{			
				chooseIndex = bProblemLinkId;
				changeWorkThread(chooseIndex, bCause);
				return 0;
			}
			break;
		case CAUSE_WORKMODE_CONTROLER2MONITOR:
			m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_STANDBY);
			break;
		case CAUSE_WORKMODE_MONITOR2CONTROLER:
			chooseIndex=-1;
			for(i=0;i<connectNum;i++)
			{
				if(m_ade_conthr[i].GetChannel()->isConnect())
				{
					chooseIndex=i;
					break;
				}
			}
			if(chooseIndex==-1)
				return -1;
			changeWorkThread(chooseIndex, CAUSE_WORKMODE_MONITOR2CONTROLER);
			break;
		default:
			return -1;
				
	}
	return 0;
}
int CAdaptee::reset()
{

		return 0;
}
int CAdaptee::start_main(IEC104_Event_Handler *handler)
{

	init();
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;
	std::string ipAddr;
	int port;
	b_ui8 grpId;
	int ret=0;
	IEC104_API*pApi=IEC104_API::getInstance();
	for(int i=0;i<connectNum;i++)
	{
 		ipAddr= pApi->m_linkPara.m_hostPara[i].hostAddr;
		port   = pApi->m_linkPara.m_hostPara[i].port;
		grpId = pApi->m_linkPara.m_hostPara[i].groupId;
		
		m_ade_conthr[i].GetChannel()->setHost(ipAddr, port,grpId);
		m_ade_conthr[i].GetChannel()->setLinkId(i);
		m_ade_conthr[i].GetChannel()->setLocaAddr(pApi->m_linkPara.strLocalAddr);
		m_ade_conthr[i].Start();
	}
	//m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(1);
	while(1)
	{
		esca::CMessage msg(0,NULL);
		this->GetMessage(msg.m_id, msg.m_data);
		if(msg.m_data)
		{
			chooseWorkThread((b_ui8*)msg.m_data,msg.m_id);
			//if((ret = chooseWorkThread((b_ui8*)msg.m_data,msg.m_id))<0)
			//	IEC104_API::getInstance()->handler_->handleAllLinkDown();
			delete[] msg.m_data;
		}
	}
	return 0;
}
int CAdaptee::init()
{
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;
	m_ade_conthr = new ConnectThread[connectNum];
	for(int i=0;i<connectNum;i++)
		m_ade_conthr[i].setManager(this);
	m_currentWorkIndex = 0;
	//m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_NOMAL);
	return 0;
}

int CAdaptee::chooseConnectLinkInGrp(b_ui8 grpID)
{
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;
	for(int i=0;i<connectNum;i++)
	{
		if((m_ade_conthr[i].GetChannel()->isConnect())&&
			(m_ade_conthr[i].GetChannel()->getGroupId() ==grpID))
			return i;
	}
	return -1;
}
int CAdaptee::chooseConnectLinkNotInGrp(b_ui8 grpID)
{
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;
	for(int i=0;i<connectNum;i++)
	{
		if((m_ade_conthr[i].GetChannel()->isConnect())&&
			(m_ade_conthr[i].GetChannel()->getGroupId() !=grpID))
			return i;
	}
	return -1;
}
void CAdaptee::changeWorkThread(int chooseLinkID,int causeOfChange)
{
	b_ui8 oldWorkLinkID=m_currentWorkIndex;
	b_ui8 oldGrpID,newGrpID;
	string oldLinkIP,newLinkIP;
	m_ade_conthr[m_currentWorkIndex].GetChannel()->setWorkMode(IEC104_WORKMODE_STANDBY);
	m_currentWorkIndex = chooseLinkID;
	m_ade_conthr[chooseLinkID].GetChannel()->setWorkMode(IEC104_WORKMODE_NOMAL);
	oldGrpID=m_ade_conthr[oldWorkLinkID].GetChannel()->getGroupId();
	newGrpID=m_ade_conthr[m_currentWorkIndex].GetChannel()->getGroupId();
	oldLinkIP=m_ade_conthr[oldWorkLinkID].GetChannel()->m_host->hostAddr;
	newLinkIP=m_ade_conthr[m_currentWorkIndex].GetChannel()->m_host->hostAddr;
	IEC104_API::getInstance()->handler_->handleDataTransferLinkChanged( newGrpID, newLinkIP,oldGrpID, oldLinkIP, causeOfChange);
}

void CAdaptee::PostMessage(b_ui32 id,void *data)
{
	esca::CLock lock(&m_messageMutex);
	m_messages.push(CMessage(id,data));
	m_messageSem.Incr();//increase 1
}

void CAdaptee::GetMessage(b_ui32 &id,void *&data)
{
	m_messageSem.Decr();//decrease 1

	esca::CLock lock(&m_messageMutex);
	esca::CMessage &msg = m_messages.front();
	id = msg.m_id;
	data = msg.m_data;
	m_messages.pop();
}

void CAdaptee::killALLConnectThread()
{
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;	
	for(int i=0;i<connectNum;i++)
	{
		if(m_ade_conthr)
			m_ade_conthr[i].Kill();
	}
}

void CAdaptee::resetLink()
{
	int connectNum = IEC104_API::getInstance()->m_linkPara.hostNum;
	std::string logStr = "CAdaptee::resetLink, prepare to reset current link";
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_INFO, logStr);
	if (m_ade_conthr != NULL && (m_currentWorkIndex >= 0 && m_currentWorkIndex < connectNum))
	{
		m_ade_conthr[m_currentWorkIndex].reconnectLink();
	}
}
