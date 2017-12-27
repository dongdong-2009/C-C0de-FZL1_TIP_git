#include "ConnectThread.h"
#include "ProtocolBase.h"
#include "Iec104Api.h"

ConnectThread::ConnectThread()
	: m_resetCurrentLink(false)
{
	mChannel = new CChannel;
	if(mChannel)
	{
		if(!mChannel->init())
			exit(0);	
		mChannel->m_bRunFlag = true;
	}
	else
	{   
		//LogThread::instance()->printStringWithTime("program will exit");
		std::string logStr="Create mChannel error, program will exit";
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_ERROR, logStr);
		exit(0);
	}

}

CChannel * ConnectThread::GetChannel(){
	return mChannel;
}
ConnectThread::~ConnectThread()
{
	delete mChannel;

}
#ifdef WIN32
   DWORD ConnectThread::Main(){
#else
	b_ui64 ConnectThread::Main(){
#endif
	return run();

}
void ConnectThread::CheckLink(){
	if (mChannel->m_bReset)
	{
		mChannel->Disconnect();
	}
}

int  ConnectThread::run()
{
	IEC104_Event_Handler *handler;
	b_bool changeLink;
	if(mChannel == NULL)
		return 0;
	handler = IEC104_API::getInstance()->handler_;
	if(handler ==NULL)
		return 0;
	mChannel->init();
	if(mChannel->m_pProtocol == NULL)
		mChannel->m_bRunFlag = false;
	mChannel->m_pProtocol->SetChanAttr(mChannel);//set channel queue
	mChannel->getProtocol()->SetHandler(handler);
	std::string logStr;
	while (mChannel->m_bRunFlag)
	{
		if (!mChannel->m_bConnected)//connection is not established
		{
			int ret =mChannel->Connect();
			if (ret == 0)
			{
				mChannel->m_bConnected =true;
				IEC104_API::getInstance()->handler_->handleLinkConnectStatusChanged(mChannel->getGroupId(),mChannel->m_host->hostAddr,LINK_STATUS_CONNECT);
				b_ui8 *message=new b_ui8[3];
				message[0] = CAUSE_LINK_CONNECT;
				message[1] = this->GetChannel()->getLinkId();
				message[2] = this->GetChannel()->getGroupId();
				m_pManager->PostMessage(3, (void *)message);
			}
			else
			{
				logStr="Connection failed";
				this->mChannel->logChannelMsg(LOG_LEVEL_WARNING, logStr);
				if(mChannel->getWorkMode()==IEC104_WORKMODE_NOMAL)
				{
					//mChannel->setWorkMode(IEC104_WORKMODE_STANDBY);
					b_ui8 *message=new b_ui8[3];
					message[0] = CAUSE_LINK_DISCONNECT;
					message[1] = this->GetChannel()->getLinkId();
					message[2] = this->GetChannel()->getGroupId();
					m_pManager->PostMessage(3, (void *)message);
				}
				int sleepTime=IEC104_API::getInstance()->m_linkPara.T0;
			#ifdef WIN32
				Sleep(sleepTime*1000);
			#else
				sleep(sleepTime);
			#endif
			}
		}
		else
		{
			do
			{
				if(mChannel->Read()<0)//read data fail
				{
					//LogThread::instance()->printStringWithTime("socket read fail");
					logStr="Socket read failed";
					this->mChannel->logChannelMsg(LOG_LEVEL_WARNING, logStr);
					changeLink = b_true;
					mChannel->Disconnect();
					break;
				}
					
				if(mChannel->getProtocol()->Explain()<0)//explain data fail
				{
					//LogThread::instance()->printStringWithTime("protocol explain fail");
					changeLink = b_false;
					break;
				}
				if(mChannel->Write()<0)//send data fail
				{
					//LogThread::instance()->printStringWithTime("socket write fail");
					logStr="Socket write failed";
					this->mChannel->logChannelMsg(LOG_LEVEL_WARNING, logStr);
					changeLink = b_true;
					break;
				}

				if (m_resetCurrentLink)
				{
					changeLink = b_false;
					m_resetCurrentLink = false;
					logStr="HB--reset current link";
					this->mChannel->logChannelMsg(LOG_LEVEL_INFO, logStr);
					break;
				}
				
			}while(1);
			reset(changeLink);
		}
	}
	return 0;
}

int ConnectThread::reset(b_bool changeLink)
{ 
		mChannel->Disconnect();
		IEC104_API*api=IEC104_API::getInstance();
		api->handler_->handleLinkConnectStatusChanged(mChannel->getGroupId(),mChannel->m_host->hostAddr,LINK_STATUS_DISCONNECT);
		//std::string logStr="disconnect! ";
		//IEC104_API::getInstance()->logRecord(LOG_LEVEL_WARNING, logStr);
		if((mChannel->getWorkMode()==IEC104_WORKMODE_NOMAL)
		  &&(changeLink==b_true))
		{
			//mChannel->setWorkMode(IEC104_WORKMODE_STANDBY);
			b_ui8 *message=new b_ui8[3];
			message[0] = CAUSE_LINK_DISCONNECT;
			message[1] = this->GetChannel()->getLinkId();
			message[2] = this->GetChannel()->getGroupId();
			m_pManager->PostMessage(3, (void *)message);
		}
		mChannel->Reset();
		mChannel->m_bCanConnect = false;
		mChannel->m_pProtocol->Init();
		mChannel->m_txBuf->resetQueue();
		mChannel->m_rxBuf->resetQueue();
		mChannel->m_bCanConnect = true;
		return 0;
}

void ConnectThread::reconnectLink()
{
	m_resetCurrentLink = true;
}

int ConnectThread::setManager(CAdaptee *pManager)
{
	if(pManager==NULL)
		return -1;
	m_pManager = pManager;
	GetChannel()->setManager(pManager);
	return 0;
}
