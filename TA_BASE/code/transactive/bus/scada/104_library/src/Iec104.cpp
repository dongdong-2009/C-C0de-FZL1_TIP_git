
#include "Iec104.h"
#include "FepDataStruct.h"
#include <time.h>
#include "Command.h"
#include "Iec104Api.h"
#include <stdlib.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIec104_Apci::CIec104_Apci(const b_ui8* pBuf)
{
	if (pBuf!=NULL)
	{
		memcpy(m_pBuf,pBuf,IEC104_APCI_SIZE);
	}
}

CIec104_Apci::CIec104_Apci(int iFrType, int iRxSeq, int iTxSeq, int iLen)
{
	m_pBuf[0]=0x68;
	switch(iFrType) 
	{
	case IEC104_FRAME_I:
		m_pBuf[1] = iLen;
		m_pBuf[2] = ((iTxSeq % 128) << 1 ) & 0xfe;
		m_pBuf[3] = iTxSeq >> 7;
		m_pBuf[4] = ((iRxSeq % 128) << 1 ) & 0xfe;
		m_pBuf[5] = iRxSeq >> 7;
		break;
	case IEC104_FRAME_S:
		m_pBuf[1] = 4;
		m_pBuf[2] = 1 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = ((iRxSeq % 128) << 1 ) & 0xfe;
		m_pBuf[5] = iRxSeq >> 7;
		break;
	case IEC104_FRAME_U_START_ACT:
		m_pBuf[1] = 4 ;
		m_pBuf[2] = 7 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	case IEC104_FRAME_U_START_CON:
		m_pBuf[1] = 4 ;
		m_pBuf[2] = 0xb ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	case IEC104_FRAME_U_STOP_ACT:
		m_pBuf[1] = 4;
		m_pBuf[2] = 0x13 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	case IEC104_FRAME_U_STOP_CON:		
		m_pBuf[1] = 4 ;
		m_pBuf[2] = 0x23 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	case IEC104_FRAME_U_TEST_ACT:		
		m_pBuf[1] = 4 ;
		m_pBuf[2] = 0x43 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	case IEC104_FRAME_U_TEST_CON:		
		m_pBuf[1] = 4 ;
		m_pBuf[2] = 0x83 ;
		m_pBuf[3] = 0 ;
		m_pBuf[4] = 0 ;
		m_pBuf[5] = 0 ;
		break;
	default:
		break;
	}
}

CIec104_Apci::~CIec104_Apci(void)
{
}

int CIec104_Apci::GetFrType()
{
	b_ui8 temp = m_pBuf[2]&0x01;
	if (temp == 0)
	{
		return IEC104_FRAME_I;
	}
	else
	{
		if ((m_pBuf[2]&0x03) == 1)
		{
			return IEC104_FRAME_S;
		}
		else if((m_pBuf[2]&7) == 7)
		{
			return IEC104_FRAME_U_START_ACT;
		}
		else if ((m_pBuf[2]&0x0b) == 0x0b)
		{
			return IEC104_FRAME_U_START_CON;
		}
		else if ((m_pBuf[2]&0x13) == 0x13)
		{
			return IEC104_FRAME_U_STOP_ACT;
		}
		else if ((m_pBuf[2]&0x23) == 0x23)
		{
			return IEC104_FRAME_U_STOP_CON;
		}
		else if ((m_pBuf[2]&0x43) == 0x43)
		{
			return IEC104_FRAME_U_TEST_ACT;
		}
		else if ((m_pBuf[2]&0x83) == 0x83)
		{
			return IEC104_FRAME_U_TEST_CON;
		}
	}
	return 0;
}

int CIec104_Apci::GetRxSeq()
{
	if (GetFrType()==0)
	{
		return -1;
	}
	unsigned short ret = *((unsigned short*)&m_pBuf[4]);
	ret >>=1;
	return ret;
}

int CIec104_Apci::GetTxSeq()
{
	if (GetFrType()==0)
	{
		return -1;
	}
	unsigned short ret = *((unsigned short*)&m_pBuf[2]);
	ret>>=1;
	return ret;
}

b_ui8 * CIec104_Apci::GetApciAddr()
{
	return m_pBuf;
}
// ----------- [5/22/2008]---------- bfg -----------

CIec104_Asdu::CIec104_Asdu(const b_ui8* pBuf, int iLen)
{
	m_pBuf = new b_ui8[iLen];
	if (m_pBuf == NULL)
	{
		return;
	}
	memcpy(m_pBuf, pBuf, iLen);
	m_iLen = iLen;
}
CIec104_Asdu::~CIec104_Asdu(void)
{
	if(m_pBuf)
		delete[] m_pBuf;
}

CIec104_Asdu::CIec104_Asdu(SIec104_APP_PARA para,int iTI,int iVSQ,int iCOT, int iCOA,int iInfAddr, b_ui8* pBuf, int iLen)
{
	int i;
	m_iLen = 2 + para.CotLen + para.AsduAddrLen + para.InfAddrLen + iLen;
	m_pBuf = new b_ui8[m_iLen];
	m_pBuf[0] = iTI;
	m_pBuf[1] = iVSQ;
	m_pBuf[2] = iCOT;
	for( i=1 ;i<para.CotLen;i++)
	{
		m_pBuf[2+i] = 0;
	}
	m_pBuf[2+para.CotLen] = iCOA % 256;//4
	for( i=1 ;i<para.AsduAddrLen;i++)
	{
		m_pBuf[2+para.CotLen+i] = (iCOA >> (8*i) ) % 256 ;//5
	}
	//m_pBuf[2+para.CotLen+para.InfAddrLen ] = iInfAddr % 256;
    m_pBuf[2+para.CotLen+para.AsduAddrLen] = iInfAddr % 256;
	for( i=1 ;i<para.InfAddrLen ;i++)
	{
	//	m_pBuf[2+para.CotLen +para.InfAddrLen +i] = (iInfAddr >> (8*i)) % 256;
		m_pBuf[2+para.CotLen +para.AsduAddrLen +i] = (iInfAddr >> (8*i)) % 256;
	}
	memcpy(m_pBuf+2+para.CotLen + para.AsduAddrLen + para.InfAddrLen,pBuf,iLen);
}

CIec104_Apdu::CIec104_Apdu(const b_ui8* pApci, const b_ui8* pAsdu,int iAsduLen)
{
	m_pBuf = new b_ui8[ IEC104_APCI_SIZE + iAsduLen ];
	memcpy(m_pBuf,pApci,IEC104_APCI_SIZE);
	memcpy(m_pBuf+IEC104_APCI_SIZE, pAsdu ,iAsduLen);
}
CIec104_Apdu::~CIec104_Apdu(void)
{
	if(m_pBuf)
		delete[] m_pBuf;
}
int CIec104_Apdu::GetSize()
{
	int ret = (int)m_pBuf[1];
	return ret;
}



int CIec104_Apdu::GetRxSeq()
{
	CIec104_Apci* p  = (CIec104_Apci*)m_pBuf;
	return p->GetRxSeq();
}


int CIec104_Apdu::GetTxSeq()
{
	CIec104_Apci* p  = (CIec104_Apci*)m_pBuf;
	return p->GetTxSeq();
}

int CIec104_Apdu::GetFrType()
{
	CIec104_Apci* p  = (CIec104_Apci*)m_pBuf;
	return p->GetFrType();
}

CIec104TxItem::CIec104TxItem()
{
	m_iCnt = 15;
	m_pBuf = NULL;
}
CIec104TxItem::CIec104TxItem(const b_ui8* pBuf,int iLen,int iCnt)
{
	m_iCnt = iCnt;
	m_pBuf = new b_ui8[iLen];
	memcpy(m_pBuf,pBuf,iLen);
}
CIec104TxItem::~CIec104TxItem(void)
{
	if (m_pBuf != NULL)
		delete[] m_pBuf;
}
b_ui8 CIec104TxItem::DeCount()
{
	return (--m_iCnt);
}
int CIec104TxItem::GetTxSeq()
{
	if (m_pBuf == NULL) return 0;
	return ((int)m_pBuf[2] >> 1 )+ ((int)m_pBuf[3] << 7);
}



// iec104 class

CIec104::CIec104()
{
	m_pIec104Data = NULL;

}

CIec104::~CIec104()
{
	Close();
}

void CIec104::Init()
{
	initPtr();
	if(LoadProtocolCfg(m_pIec104Data)){
		std::string logStr="LoadProtocolCfg success";
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_DEBUG, logStr);
	}
	else{
		std::string logStr="LoadProtocolCfg fail ,program will exit";
		IEC104_API::getInstance()->logRecord(LOG_LEVEL_ERROR, logStr);
		exit(1);

	}
	
}

int CIec104::Explain()
{
	
	memset(m_msgFrame,0,sizeof(SMsgFrame) * MAXPACKFRAME);
	m_iExpFrameNum = 0;
	SendCommand();//send commands of 104 protocol
	if(ExpBufData()<0)
	{
		std::string logStr="Explain buff data error";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,logStr);
		return -1;
	}
	updateTimer();
	if(m_pIec104Data->T2Cnt<=0)
	{
		char tmpStr[100];
		sprintf(tmpStr,"T2 time out %d",m_pIec104Data->T2Cnt);
		std::string logStr=tmpStr;
		this->m_pChannel->logChannelMsg(LOG_LEVEL_DEBUG,logStr);
		LinkTxFrameS();
	}
	if(m_pIec104Data->T1Cnt<=0)
	{
		std::string logStr="T1 time out";
		char cStr[100];
		sprintf(cStr,"%d",m_pIec104Data->T1Cnt);
		logStr +=cStr;
		this->m_pChannel->logChannelMsg(LOG_LEVEL_DEBUG,logStr);
		return -1;
	}


	if(!m_pChannel->m_bConnected){
		initPtr(); 
		LoadProtocolCfg(m_pIec104Data);
		return 0;
	}
	return 0;
}

int CIec104::ExpBufData()
{
	if(m_pIec104Data == NULL)
	{
		return 0;
	}	
	SIec104Data *p104 = m_pIec104Data;
	TXDEQUE*	pTxQue = m_pIec104Data->pTxQueue;//send buffer

	int ret=0;
	while (GetBufDataLen() >= IEC104_APCI_SIZE)
	{
		ret = LinkRxApdu();
		if(ret==-1) //
		{
			return -1;
		}
		if(ret==-2)//not enough length
			return 0;
			
	}
	return 0;
}
	
int  CIec104::LinkCheckCommand()
{
	if(m_pIec104Data->LinkCmd!=IEC104_LK_CMD_NONE )//
		return 0;
	if(m_pIec104Data ->T3Cnt < 0)
	{
		std::string logStr="T3 time out";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_DEBUG,logStr);
		m_pIec104Data->LinkCmd = IEC104_LK_CMD_UTEST;
	}
	if(m_pChannel->getWorkMode()==IEC104_WORKMODE_NOMAL)//only send utest at monitor work model
	{
		if((m_pIec104Data ->LinkState== IEC104_LK_STATE_STOPPED) &&(m_pIec104Data->StartFlag ==b_true))
		{
			m_pIec104Data ->LinkCmd = IEC104_LK_CMD_STARTDT;
		}
	}
	else if(m_pChannel->getWorkMode()==IEC104_WORKMODE_STANDBY)
	{
		if((m_pIec104Data ->LinkState== IEC104_LK_STATE_STARTED) &&(m_pIec104Data->StartFlag ==b_false))
		{
			m_pIec104Data ->LinkCmd = IEC104_LK_CMD_STOPDT;
		}
	}
	return SendLinkCmd();

}

int CIec104::SendLinkCmd()
{
	switch(m_pIec104Data ->LinkCmd)
	{
		case IEC104_LK_CMD_NONE:
			return 0;
		case IEC104_LK_CMD_STARTDT:
			if ( LinkTxFrameU(IEC104_FRAME_U_START_ACT) == 1)
			{
				m_pIec104Data ->LinkCmd |= IEC104_LK_CMD_SENDFLAG;
				m_pIec104Data->LinkState= IEC104_LK_STATE_STARTED_PEND;
				return 0;
			}
		case IEC104_LK_CMD_UTEST:
			if ( LinkTxFrameU(IEC104_FRAME_U_TEST_ACT) == 1)
			{
				m_pIec104Data ->LinkCmd |= IEC104_LK_CMD_SENDFLAG;
				return 0;
			}
		case IEC104_LK_CMD_STOPDT:
			if ( LinkTxFrameU(IEC104_FRAME_U_STOP_ACT) == 1)
			{
				m_pIec104Data ->LinkCmd |= IEC104_LK_CMD_SENDFLAG;
				m_pIec104Data->LinkState= IEC104_LK_STATE_STOPPED_PEND;
				return 0;
			}
		default:
			return -1;//invalid command
			  
			
	}
	return -2;//error of sending
}

int CIec104::AppCheckCommand()
{
	SIec104Data *p104;	
   	 p104 = m_pIec104Data ;
	 if(p104->AppState!=IEC104_APP_STATE_IDLE)//commands have not finished
	 	return 0;
	 if(m_pChannel->getWorkMode()!=IEC104_WORKMODE_NOMAL)
	 	return 0;
	 int ret = 0;
	 ret = AppCheckCmdFromAgent();
	 if(ret <1)
	 	return ret;
	 if(ret ==2)//no app from agent, check t4
	 {
	 	if((p104->T4Cnt <=0)&& (p104->T4Start!=IEC104_TIME_STOP) && (p104->NextCmd==IEC104_CMD_NON))
			p104->NextCmd = IEC104_CMD_CALLALLDATA;
		else
			return 0;		
	 }
	switch(p104->NextCmd)//mostly are system commands
	{
		case IEC104_CMD_CALLALLDATA:
			if ( AppTxAsduType_100(p104->AppPara.rtuAddr)== 1)
			{
				p104->NextCmd = IEC104_CMD_NON;
				return 0;
			}
			ret = -2;
			break;
		case IEC104_CMD_CALLALLKWH:
			if (  AppTxAsduType_101(p104->AppPara.rtuAddr)== 1)
			{
				p104->NextCmd = IEC104_CMD_NON;
				return 0;
			}
			ret = -2;
			break;
		case  IEC104_CMD_SYNTIME:
			if ( AppTxAsduType_103(p104->AppPara.rtuAddr)==1 )
			{
				p104->NextCmd = IEC104_CMD_NON;
				return 0;
			}
			ret = -2;
			break;	
		case IEC104_CMD_NON:
		case  IEC104_CMD_PEND:
			return 0;
		/*case IEC104_CMD_TEST:
			if ( AppTxTs() == 1 )
			{
				p104->NextCmd = IEC104_CMD_NON;
				return 0;
			}
			ret = -2;
			break;
		case IEC104_CMD_RESETPROC:
			if ( AppTxRP() == 1)
			{
				p104->NextCmd = IEC104_CMD_NON;
				return 0;
			}
			ret = -2;		*/
		default:
			ret = -1;
	}
	return ret;
}

int CIec104::AppCheckCmdFromAgent()
{
	 SIec104Data *p104;	
   	 p104 = m_pIec104Data ;
	 if(p104->NextCmd!= IEC104_CMD_NON)
	 	return 1;     //If the system commands have not finished,the command from agent can not be sent
	 CCommand cmd;
	 float sfa;b_ui16 nva;b_i16 sva;b_ui32 bitstring32=0;
	 int ret;
	 if(!(IEC104_API::getInstance()->getCommand(cmd)))//no app cmd in queue.
	 	return 2;
	switch(cmd.getTi())
	{
		case IEC104_C_IC_NA://interrogation command
			//p104->NextCmd = IEC104_CMD_CALLALLDATA;
			ret = AppTxAsduType_100(cmd._iRtuAddr, cmd._arg[0], cmd._arg[1]);
			break;
		case IEC104_C_CI_NA://counter interrogation command
			//p104->NextCmd = IEC104_CMD_CALLALLKWH;
			ret = AppTxAsduType_101(cmd._iRtuAddr, cmd._arg[0], cmd._arg[1]);
			//return 1;
			break;
		case IEC104_C_CS_NA://clock synchronization command
			ret = AppTxAsduType_103(cmd._iRtuAddr);
			break;
		case IEC104_C_RD_NA://read command
			ret = AppTxAsduType_102(cmd._iRtuAddr, cmd._iInfoAddr);
			break;
		case IEC104_C_RP_NA://reset process command
			ret = AppTxAsduType_105(cmd._iRtuAddr, cmd._arg[0]);
			break;
		case IEC104_C_TS_TA://test command with time tag CP56Time2a
			ret = AppTxAsduType_107(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0]+cmd._arg[1]*256);
			break;
		case IEC104_C_SC_NA://single command
			ret =AppTxAsduType_45(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1]);
			break;
		case IEC104_C_DC_NA://double command
			ret =AppTxAsduType_46(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1]);
			break;
		case IEC104_C_RC_NA://regulating step command
			ret =AppTxAsduType_47(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1]);
			break;
		case IEC104_C_SC_TA://single command with time tag CP56Time2a
			ret=AppTxAsduType_58(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1],&cmd._arg[2]);
			break;
		case IEC104_C_DC_TA://double command with time tag CP56Time2a
			ret=AppTxAsduType_59(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1],&cmd._arg[2]);
			break;
		case IEC104_C_RC_TA://regulating step command with time tag CP56Time2a
			ret=AppTxAsduType_60(cmd._iRtuAddr, cmd._iInfoAddr,cmd._arg[0], cmd._arg[1],&cmd._arg[2]);
			break;
		case IEC104_C_SE_NA: //set point command, normalized value
			memcpy(&nva, &cmd._arg[1],2);
			ret = AppTxAsduType_48(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],nva,cmd._arg[3]);
			break;
		case IEC104_C_SE_NB://set point command, scaled value
			memcpy(&sva, &cmd._arg[1],2);
			ret = AppTxAsduType_49(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],sva,cmd._arg[3]);
			break;
		case IEC104_C_SE_NC://set point command, short floating point number
			memcpy(&sfa, &cmd._arg[1], 4);
			ret = AppTxAsduType_50(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],sfa,cmd._arg[5]);
			break;
		case IEC104_C_SE_TA://set point command, normalized value with time tag CP56Time2a
			memcpy(&nva, &cmd._arg[1], 2);
			ret = AppTxAsduType_61(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],nva,cmd._arg[3],&cmd._arg[4]);
			break;
		case IEC104_C_SE_TB://set point command, scaled value with time tag CP56Time2a
			memcpy(&sva, &cmd._arg[1], 2);
			ret = AppTxAsduType_62(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],sva, cmd._arg[3],&cmd._arg[4]);
			break;
		case IEC104_C_SE_TC://set point command, short floating-point number with time tag CP56Time2a
			memcpy(&sfa, &cmd._arg[1], 4);
			ret = AppTxAsduType_63(cmd._iRtuAddr, cmd._iInfoAddr, cmd._arg[0],sfa, cmd._arg[5],&cmd._arg[6]);
			break;
		case IEC104_C_BO_NA:
			memcpy(&bitstring32, &cmd._arg[1], 4);
			ret=AppTxAsduType_51(cmd._iRtuAddr,cmd._iInfoAddr ,cmd._arg[0], bitstring32);
			break;
		case IEC104_C_BO_TA:
			memcpy(&bitstring32, &cmd._arg[1], 4);
			ret=AppTxAsduType_64(cmd._iRtuAddr,cmd._iInfoAddr ,cmd._arg[0], bitstring32,&cmd._arg[5]);
			break;
		case IEC104_P_ME_NA://parameter of measured value, normalized value
			ret = AppTxAsduType_110(cmd._iRtuAddr, cmd._iInfoAddr,&cmd._arg[0],cmd._arg[2]);
			break;
		case IEC104_P_ME_NB://parameter of measured value, scaled value	
			ret = AppTxAsduType_111(cmd._iRtuAddr, cmd._iInfoAddr,&cmd._arg[0],cmd._arg[2]);
			break;
		case IEC104_P_ME_NC://parameter of measured value, short floating-point number	
			ret = AppTxAsduType_112(cmd._iRtuAddr, cmd._iInfoAddr,&cmd._arg[0],cmd._arg[4]);
			break;
		case IEC104_P_NC_NA://parameter activation
			//if(AppTxParaAct(cmd._iRtuAddr)==1)
			ret = AppTxAsduType_113(cmd._iRtuAddr, cmd._arg[0],cmd._iInfoAddr, cmd._arg[1]);
			break;
		case IEC104_F_SC_NA:
			if(AppTxCallFile(cmd._iRtuAddr,cmd._iInfoAddr,cmd._bCmdType,*(b_ui16*)&cmd._arg[0],cmd._arg[2])==1)
				return 0;
			break;
		case IEC104_F_AF_NA:
			if(AppTxAffirmFile(cmd._iRtuAddr, cmd._iInfoAddr,*(b_ui16*)&cmd._arg[0], cmd._arg[2], cmd._arg[3]))
				return 0;

		case IEC104_E_CD_NA:
			if(cmd._arg[0] == 0)//area number
			{
				if(AppTxExCallValue(cmd._iRtuAddr,cmd._arg[1],1,cmd._arg[0],1)==1)
					return 0;				
			}
			else
			{
				if(AppTxExCallValue(cmd._iRtuAddr,cmd._arg[1],1,cmd._arg[0],0)==1)
					return 0;
			}
			break;
		case IEC104_E_MD_NA:
			if(cmd._arg[2] == 0)//area number
			{
				if(AppTxExModifyValueAreaNo(cmd._iRtuAddr, cmd._arg[1],cmd._arg[0],cmd._bSeType)==1)
					return 0;
			}
			else
			{
				if(AppTxExModifyValue(cmd._iRtuAddr, cmd._arg[1], cmd._arg[0], cmd._bSeType, cmd._fBuf, cmd._arg[2],cmd._arg[3])==1)
					return 0;
			}
			break;
		default:
			return -1;
	}
	if(ret ==1)
		return 0;
	return -2;
		
}
void CIec104::SendCommand()
{
	if(m_pIec104Data == NULL)
	{
		return;
	}	
	else if (!m_pChannel->m_bConnected)
	{
		return;
	}
	SIec104Data *p104;
	p104 = m_pIec104Data;
	int ret=0;
	do
	{	
		if((ret = LinkCheckCommand())!=0)//check startact and testact command
			break;
		if(m_pIec104Data->LinkState<IEC104_LK_STATE_STARTED)//under stopped state,can't send app cmd
			return;
		if((ret = AppCheckCommand())!=0)//check system command
			break;
		return;
	}while(0);
				
	switch(ret)//deal with error 
	{
		case -1:
			printDebug("sendcommand:invalid parameter");
			break;
		case -2:
			printDebug("sendcommand: Tx error");
			break;
	}
	return ;
}

void CIec104::initPtr()
{
    if(m_pIec104Data){
		delete m_pIec104Data;
		m_pIec104Data=NULL;
	}
	IEC104_API *api=IEC104_API::getInstance();
	m_pIec104Data =new SIec104Data;
	m_pIec104Data->LinkPara = &api->m_linkPara;
	m_pIec104Data->AppPara = api->m_AppPara;
	resetTimer(3);
}

b_bool CIec104::LoadProtocolCfg(SIec104Data *pPara)
{
	
	return b_true;
}

void CIec104::Close()
{
	if (m_pIec104Data != NULL)
	{
		delete m_pIec104Data;
	}
}

b_bool CIec104::GetSendDataPackage(char *buff, int *len)
{

	return true;
}


/*
*-2:length is not enough
* 0:normal
*-1:error of explaination
*-3:error of packet format
*/

int CIec104::LinkRxApdu()
{
	SIec104Data *p104;
	b_ui8 apdu[IEC104_MAX_APDU_SIZE];

	p104=m_pIec104Data;
	int k=0;
	while(1)
	{
		if(GetBufDataLen() < 1)
		{
			
			return -2;	
		}
		GetBufData(apdu,1,false);
		if(apdu[0]==0x68)
		{
			if(GetBufDataLen()>=IEC104_APCI_SIZE)
			{
				GetBufData(apdu,2,false);
				if(apdu[1]<4)
				{
					
					return -3;//invalid apdu
				}
				else if(apdu[1]==4)
				{
					GetBufData(apdu,IEC104_APCI_SIZE,true);
					break;
				}
				else
				{
					if(GetBufDataLen()>= apdu[1] + 2)
					{
						GetBufData(apdu,apdu[1]+2,true);
						break;
					}
					int size = GetBufDataLen();
					
					return -2;//I format have not been received finished and have to wait
				}
			}
			else
			{	
				
				return -2;
			}
		}
		else
		{
			GetBufData(apdu,1,true);
			
		}
	}

	if (apdu[1] <4) 
	{
		return -3;
	}
	if (apdu[1] == 4)
	{
		PrintInfo("Recv:",apdu,6);
		if ((apdu[2] & 0x01) == 0) 
			return false;
		if ((apdu[2] & 0x03) == 1)
		{
			// s format
			LinkRxFrameS(p104,apdu);
		}
		if ((apdu[2] & 0x03) == 3)
		{
			// U format
			LinkRxFrameU(p104,apdu);
		}
	}
	else 
	{
		PrintInfo("Recv:",apdu,apdu[1]+2);
		if ((apdu[2] & 0x01) == 0)
			if(LinkRxFrameI(p104,apdu)==-1)
				return -1;
		else
			return -3;	
	}

	return 0;
}
//handel S format
b_bool CIec104::LinkRxFrameS(SIec104Data *p, const b_ui8 *pApdu)
{
	TXDEQUE* pTxQue;
	pTxQue = p->pTxQueue ;
	int iRxSeq = (pApdu[4] >> 1 )+ (pApdu[5] << 7) ;
	p->AckSeq = iRxSeq ;
	TXDEQUE::iterator pi;
	CIec104TxItem* txItem;
	//delet those data that have been confirmed from send buffer
	for(pi= pTxQue->begin();  pi !=pTxQue->end(); pi++)
	{
		txItem = *pi;
		if (txItem->GetTxSeq() <= iRxSeq)
		{
			pTxQue->pop_front();
			delete txItem; // delete the stored Tx frame
		}
		else
			break;
	}
	resetTimer(3);	
	if(iRxSeq == p->TxSeq)
		stopTimer(1);
	return true;
}
//handle U format
int CIec104::LinkRxFrameU(SIec104Data *p, const b_ui8 *pApdu)
{
	switch( pApdu[2])
	{
		case 0x83:// TESTFR_CON
			if(m_pIec104Data ->LinkCmd = IEC104_LK_CMD_SENDFLAG | IEC104_LK_CMD_UTEST)
			{
				m_pIec104Data ->LinkCmd = IEC104_CMD_NON;
				stopTimer(1);
				if(pApdu[3]>0)//host is slave
					m_pChannel->setHostStatus(HOST_STATUS_SLAVE);
				else
					m_pChannel->setHostStatus(HOST_STATUS_MASTER);
				return 0;
			}
			return -1;
		case 0x43:// TESTFR_ACT	
			if(pApdu[3]>0)
				m_pChannel->setHostStatus(HOST_STATUS_SLAVE);
			else
				m_pChannel->setHostStatus(HOST_STATUS_MASTER);
			if(LinkTxFrameU(IEC104_FRAME_U_TEST_CON)==1)
				return 0;
			return -1;
		case 0x23://stopDt con
			if(m_pIec104Data ->LinkCmd= IEC104_LK_CMD_SENDFLAG | IEC104_LK_CMD_STOPDT)
			{
				m_pIec104Data ->LinkCmd = IEC104_CMD_NON;
				m_pIec104Data ->LinkState= IEC104_LK_STATE_STOPPED;
				stopTimer(1);
				return 0;
			}
			return -1;
		case 0x0b://start con
			if(m_pIec104Data->LinkCmd= IEC104_LK_CMD_SENDFLAG | IEC104_LK_CMD_STARTDT)
			{
				m_pIec104Data ->LinkCmd= IEC104_CMD_NON;
				m_pIec104Data ->LinkState= IEC104_LK_STATE_STARTED;
				m_pIec104Data->NextCmd = IEC104_CMD_CALLALLDATA;
				m_pChannel->setHostStatus(HOST_STATUS_MASTER);
				stopTimer(1);
				return 0;
			}
			return -1;
		case 0x13:
		case 0x07:
		default:
			return -1;
	}
	

}
//handle I format
int CIec104::LinkRxFrameI(SIec104Data *p, b_ui8* pApdu)
{
	b_ui8 size = pApdu[1]+2;
	int iRxSeq=0,iTxSeq=0;
	TXDEQUE* pTxQue = p->pTxQueue;

	iRxSeq = (pApdu[4] >> 1 )+ (pApdu[5] << 7 );
	iTxSeq = (pApdu[2] >> 1 )+ (pApdu[3] << 7 );
	if(p->RxSeq==iTxSeq)
		p->RxSeq = ((++p->RxSeq)%32768);
	else
		return -1;
	if(p->TxSeq == iRxSeq )
		stopTimer(1);
	resetTimer(3);
	if((p->LastAckSeq+1)%32768== iTxSeq)
		resetTimer(2);
	if ( iRxSeq > p->AckSeq) 
	{
		p->AckSeq = iRxSeq;
		TXDEQUE::iterator pi;
		CIec104TxItem *txItem;
		//delete data that were confirmed from send buffer
		for(pi= pTxQue->begin();  pi !=pTxQue->end(); pi++)
		{
			txItem = *pi;
			if (txItem->GetTxSeq() <= iRxSeq)
			{
				pTxQue->pop_front();
				delete txItem; // delete the stored Tx frame
				if(pTxQue->size() == 0)
					break;
			}
			else
				break;
		}			
	}
	try
	{
		AppRxAsdu(pApdu+IEC104_APCI_SIZE, size-IEC104_APCI_SIZE);
	}
	catch (...)
	{
		std::string msg = "CIec104::LinkRxFrameI catched unhandled exception when calling AppRxAsdu.";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR, msg);
	}

	// When received number is w and which are not confirmed,we should send S format
	if(p->NextCmd == IEC104_CMD_PEND)
		p->NextCmd =IEC104_CMD_NON;
	if( ((p->RxSeq +32768 -  p->LastAckSeq )%32768) >= p->LinkPara->W ) 
		LinkTxFrameS();

	return 0;
}

b_bool CIec104::GetCurTm(char *pBuf)
{
	char times[9];
	time_t ti;
	ti = time(&ti);
	struct tm * gm = gmtime(&ti);
	sprintf(times,"%2d:%2d:%2d",gm->tm_hour,gm->tm_min,gm->tm_sec);
	return true;
}
//handle ASDU of I format
void CIec104::AppRxAsdu(const b_ui8 *pBuf, b_ui8 iSize)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	int ret=0;
	std::string tmpStr;
	if(iSize<2)
	{
		
		tmpStr="Wrong asdu size";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,tmpStr);
		return;
	}
	if(checkAsduSize(pBuf[0], pBuf[1], iSize)<0)
	{
		tmpStr="Wrong asdu size";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,tmpStr);
		return;
	}
	if((pBuf[2]&0x3f)==IEC104_COT_SPONT)
		LinkTxFrameS();
		
	switch (pBuf[0])
	{
	case IEC104_C_IC_NA: //100
		ret=AppRxAsdu_100(pBuf, iSize );
		break;
	case IEC104_C_CI_NA: //101
		ret=AppRxAsdu_101( pBuf, iSize );
		break;
	case IEC104_C_RD_NA:	//102 
		ret=AppRxAsdu_102( pBuf, iSize );
		break;
	case IEC104_C_CS_NA:	//103
		ret=AppRxAsdu_103( pBuf, iSize );
		break;
	case IEC104_C_RP_NA:	//105
		ret=AppRxAsdu_105( pBuf, iSize );
		break;
	case IEC104_C_TS_TA:	//107
		ret=AppRxAsdu_107( pBuf, iSize );
		break;	
	case IEC104_M_SP_NA:    
		ret = AppRxAsdu_1(pBuf, iSize);
		break;
	case IEC104_M_DP_NA:	  
		ret = AppRxAsdu_3(pBuf, iSize);
		break;
	case IEC104_M_ST_NA:	//5
		ret = AppRxAsdu_5(pBuf, iSize);
		break;
	case IEC104_M_ME_NA:	//9	
		ret = AppRxAsdu_9(pBuf, iSize);
		break;
	case IEC104_M_ME_NB:	//11
		ret = AppRxAsdu_11(pBuf, iSize);
		break;
	case IEC104_M_ME_NC:	//13
		ret = AppRxAsdu_13(pBuf, iSize);
		break;
	case IEC104_M_ME_ND://21	
		ret = AppRxAsdu_21(pBuf, iSize);
		break;
	case IEC104_M_SP_TB:    
		ret = AppRxAsdu_30(pBuf, iSize);
		break;
	case IEC104_M_DP_TB:   
		ret = AppRxAsdu_31(pBuf, iSize);
		break;
	case IEC104_M_ST_TB:	//32
		ret = AppRxAsdu_32(pBuf, iSize);
		break;
	case IEC104_M_ME_TD:	//34
		ret = AppRxAsdu_34(pBuf, iSize);
		break;
	case IEC104_M_ME_TE:	//35
		ret = AppRxAsdu_35(pBuf, iSize);
		break;
	case IEC104_M_ME_TF:	//36
		ret = AppRxAsdu_36(pBuf, iSize);
		break;	
	case IEC104_M_BO_NA:	//7
		ret = AppRxAsdu_7(pBuf, iSize);
		break;
	case IEC104_M_BO_TB:	//33
		ret = AppRxAsdu_33(pBuf, iSize);
		break;
	case IEC104_M_EI_NA:	//70	end of initialization
		//AppRxInitEnd(iLoopCnt, iRtuIndex, pBuf, iSize );
		break;
	case IEC104_M_IT_NA:	//integrated totals
		ret = AppRxAsdu_15(pBuf, iSize);
		break;
	case IEC104_M_SP_NB:
		ret = AppRxAsdu_20(pBuf,iSize);
		break;
	case IEC104_M_IT_TB:	//37
		
		ret = AppRxAsdu_37(pBuf, iSize);
		break;
	case IEC104_M_EP_TD:	//38
		ret = AppRxAsdu_38(pBuf,iSize);
		break;
	case IEC104_M_EP_TE:	//39
		ret = AppRxAsdu_39(pBuf,iSize);
		break;
	case IEC104_M_EP_TF:	//40
		ret = AppRxAsdu_40(pBuf,iSize);
		break;
	case IEC104_C_SC_NA:
		ret = AppRxAsdu_45(pBuf, iSize);
		break;
	case IEC104_C_DC_NA:
		ret = AppRxAsdu_46(pBuf, iSize);
		break;
	case IEC104_C_RC_NA:
		ret = AppRxAsdu_47(pBuf, iSize);
		break;
	case IEC104_C_SE_NA:
		ret = AppRxAsdu_48(pBuf, iSize);
		break;
	case IEC104_C_SE_NB:
		ret = AppRxAsdu_49(pBuf, iSize);
		break;
	case IEC104_C_SE_NC:
		ret = AppRxAsdu_50(pBuf, iSize);
		break;
	case IEC104_C_BO_NA:
		ret = AppRxAsdu_51(pBuf, iSize);
		break;
	case IEC104_C_SC_TA:
		ret = AppRxAsdu_58(pBuf, iSize);
		break;
	case IEC104_C_DC_TA:
		ret = AppRxAsdu_59(pBuf, iSize);
		break;
	case IEC104_C_RC_TA:
		ret = AppRxAsdu_60(pBuf, iSize);
		break;
	case IEC104_C_SE_TA:
		ret = AppRxAsdu_61(pBuf, iSize);
		break;
	case IEC104_C_SE_TB:
		ret = AppRxAsdu_62(pBuf, iSize);
		break;
	case IEC104_C_SE_TC:	
		ret = AppRxAsdu_63(pBuf, iSize);
		break;
	case IEC104_C_BO_TA:
		ret = AppRxAsdu_64(pBuf, iSize);
		break;
	case IEC104_P_ME_NA:
		ret = AppRxAsdu_110(pBuf,iSize);
		break;
	case IEC104_P_ME_NB:
		ret = AppRxAsdu_111(pBuf,iSize);
		break;
	case IEC104_P_ME_NC:
		ret = AppRxAsdu_112(pBuf,iSize);
		break;
	case IEC104_P_NC_NA:
		ret = AppRxAsdu_113(pBuf,iSize);
		break;
//file transfer
	case IEC104_F_FR_NA:	//120
		AppRxFileReady(pBuf,iSize);
		break;
	case IEC104_F_SR_NA:	//121
		AppRxSectionReady(pBuf,iSize);
		break;
	case IEC104_F_LS_NA:	//123
		AppRxLastSe(pBuf,iSize);
		break;
	case IEC104_F_SG_NA:	//125
		AppRxSegment(pBuf,iSize);
		break;
	case IEC104_F_DR_NA:   //126
		AppRxDir(pBuf,iSize);
		break;
//103 extension
	case IEC104_E_PA_NA://140
		AppRxProtectAction(pBuf,iSize);
		break;
	case IEC104_E_CR_NA:
		AppRxCallValueConf(pBuf,iSize);
		break;
	case IEC104_E_148:
		AppRxModifyValueConf(pBuf,iSize);
		break;
	default:
		tmpStr="Unknown asdu type,no need to handle";
		this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,tmpStr);
		break;
	}

}

int CIec104::AppRxAsdu_100(const b_ui8 *asdu, b_ui8 size)
{
	SIec104Data *p104= m_pIec104Data;
	int infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int infoAddrLen = p104->AppPara.InfAddrLen;
	b_ui16 rtuAddr = getRtuAddr(asdu);
	b_ui8  bCot = asdu[2]&0x3f;
	b_ui8  bQoi = asdu[infoIndex+infoAddrLen];

	if((p104->AppState == IEC104_APP_STATE_IDLE) && ((asdu[2]&0x3f) == IEC104_COT_ACT_TERM))
		p104->NextCmd = IEC104_CMD_CALLALLKWH;

	GetHandler()->handleInterrogateGeneral(rtuAddr, bCot, bQoi);
	return 0;
	
}


int CIec104::AppRxAsdu_101(const b_ui8 *asdu, b_ui8 size)
{
	SIec104Data *p104= m_pIec104Data;
	int infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int infoAddrLen = p104->AppPara.InfAddrLen;
	b_ui16 rtuAddr = getRtuAddr(asdu);
	b_ui8  bCot = asdu[2]&0x3f;
	b_ui8  bQcc = asdu[infoIndex+infoAddrLen];

	if((p104->AppState == IEC104_APP_STATE_IDLE) && ((asdu[2]&0x3f) == IEC104_COT_ACT_TERM))
		p104->NextCmd = IEC104_CMD_SYNTIME;

	GetHandler()->handleInterrogateCounter(rtuAddr, bCot, bQcc&0x3f, (bQcc>>6)&0x03);
	return 0;
}
int CIec104::AppRxAsdu_102(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot=(asdu[2]&0x3f);
	GetHandler()->handleReadCommand(iRtuAddr, bCot,  infoAddr);
	return 0;
}
int CIec104::AppRxAsdu_103(const b_ui8 *asdu, b_ui8 size)
{
	SIec104Data *p104;	
	p104 = m_pIec104Data;	
	int iStart = p104->AppPara.AsduAddrLen + p104->AppPara.CotLen + 
		p104->AppPara.InfAddrLen +2;
	b_ui8 bCot=(asdu[2]&0x3f);
	b_ui16 rtuAddr=getRtuAddr(asdu);
	SFepTime timeTag;
	GetTmStmp(&asdu[iStart], &timeTag);
	GetHandler()->handleSyncTime(rtuAddr, bCot, timeTag);
	return 0;
}

int CIec104::AppRxAsdu_105(const b_ui8 *asdu, b_ui8 iSize)
{
	SIec104Data *p104= m_pIec104Data;;
	int iStart = p104->AppPara.AsduAddrLen + p104->AppPara.CotLen + 
		p104->AppPara.InfAddrLen +2;
	b_ui8 bCot=(asdu[2]&0x3f);
	b_ui16 rtuAddr=getRtuAddr(asdu);
	b_ui8  bQrp=asdu[iStart];
	GetHandler()->handleResetProcess(rtuAddr, bCot, bQrp);
	return 0;
}

int CIec104::AppRxAsdu_107(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot=(asdu[2]&0x3f);
	b_ui16 srcTsc=0,bTsc=0;
	memcpy(&srcTsc,&asdu[infoIndex+infoAddrLen],2);
	bTsc=p104tohost16(srcTsc);
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+infoAddrLen+2], &timeTag);
	GetHandler()->handleTestWithTimeTag(iRtuAddr, bCot, infoAddr, bTsc,timeTag);
	return 0;
	
}

int CIec104::AppRxAsdu_45(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2];	// old value: asdu[2]&0x3f;
	GetHandler()->handleSingleCommand(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex]);
	return 0;
}
int CIec104::AppRxAsdu_46(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	GetHandler()->handleDoubleCommand(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex]);
	return 0;
}
int CIec104::AppRxAsdu_47(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	GetHandler()->handleStepCommand(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex]);
	return 0;
}

int CIec104::AppRxAsdu_58(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+1], &timeTag);
	GetHandler()->handleSingleCommandWithTimeTag(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex],timeTag);
	return 0;
}
int CIec104::AppRxAsdu_59(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+1], &timeTag);
	GetHandler()->handleDoubleCommandWithTimeTag(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex],timeTag);
	return 0;
}
int CIec104::AppRxAsdu_60(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+1], &timeTag);
	GetHandler()->handleStepCommandWithTimeTag(iRtuAddr,infoAddr, bCot, asdu[infoAddrLen+infoIndex],timeTag);
	return 0;
}

int CIec104::AppRxAsdu_1(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8 btSeqflag,btCot,bSiq=0;
	b_ui16 rtuAddr;
	int iNum,PointNo,iExpYxNum=0,iInf,iInfAddrLen,i;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SSinglePoint> vSinglePoint;
	SSinglePoint sp;
	iNum = asdu[1]&0x7f;
       btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btCot >= IEC104_COT_RESP_GRP1 && btCot <= IEC104_COT_RESP_GRP8 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 1;
	}
	if (btSeqflag)
	{
		PointNo = 0;
		for (i=iInfAddrLen; i>0; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}
		for (i=0,iExpYxNum=0; i<iNum; i++,PointNo++,iExpYxNum ++)
		{
			memcpy(&bSiq,&asdu[iInf + iInfAddrLen +i],1);
			sp.SIQ.SPI = (bSiq>>SPI_OFFSET)&BIT_SIZE1;
			sp.SIQ.BL  = (bSiq>>BL_OFFSET)&BIT_SIZE1;
			sp.SIQ.SB  = (bSiq>>SB_OFFSET)&BIT_SIZE1;
			sp.SIQ.NT  = (bSiq>>NT_OFFSET)&BIT_SIZE1;
			sp.SIQ.IV  = (bSiq>>IV_OFFSET)&BIT_SIZE1;
			sp.iOffset = PointNo;
			vSinglePoint.push_back(sp);
		}
	}
	else
	{
		for( i=0; i<iNum; i++,iExpYxNum ++)
		{
			PointNo = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+1)*i-1];
			}
			memcpy(&bSiq,&asdu[iInf + iInfAddrLen +(iInfAddrLen+1)*i],1);
			sp.SIQ.SPI = (bSiq>>SPI_OFFSET)&BIT_SIZE1;
			sp.SIQ.BL  = (bSiq>>BL_OFFSET)&BIT_SIZE1;
			sp.SIQ.SB  = (bSiq>>SB_OFFSET)&BIT_SIZE1;
			sp.SIQ.NT  = (bSiq>>NT_OFFSET)&BIT_SIZE1;
			sp.SIQ.IV  = (bSiq>>IV_OFFSET)&BIT_SIZE1;
			sp.iOffset = PointNo;
			sp.iOffset = PointNo;
			vSinglePoint.push_back(sp);
			
		}
	}
	if(iExpYxNum>0)
		GetHandler()->handleSinglePoint(rtuAddr,btCot,vSinglePoint);
	return 0;

}
int CIec104::AppRxAsdu_3(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8 btSeqflag,btCot,bDiq=0;
	b_ui16 rtuAddr;
	int iNum,PointNo,iExpYxNum=0,iInf,iInfAddrLen,i;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SDoublePoint> vDoublePoint;
	SDoublePoint dp;
	
	iNum = asdu[1]&0x7f;
       btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btCot >= IEC104_COT_RESP_GRP1 && btCot <= IEC104_COT_RESP_GRP8 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 1;
	}
	if (btSeqflag)
	{
		PointNo = 0;
		for (i=iInfAddrLen; i>0; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}
		for (i=0; i<iNum; i++,PointNo++,iExpYxNum ++)
		{
			memcpy(&bDiq,&asdu[iInf + iInfAddrLen +i],1);
			dp.DIQ.DPI = (bDiq>>DPI_OFFSET)&BIT_SIZE2;
			dp.DIQ.BL  =  (bDiq>>BL_OFFSET)&BIT_SIZE1;
			dp.DIQ.SB  =  (bDiq>>SB_OFFSET)&BIT_SIZE1;
			dp.DIQ.NT  =  (bDiq>>NT_OFFSET)&BIT_SIZE1;
			dp.DIQ.IV   =  (bDiq>>IV_OFFSET)&BIT_SIZE1;
			dp.iOffset = PointNo;
			vDoublePoint.push_back(dp);
		}
	}
	else
	{
		for( i=0; i<iNum; i++ ,iExpYxNum ++)
		{
			PointNo = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+1)*i-1];
			}
			memcpy(&bDiq,&asdu[iInf + iInfAddrLen +(iInfAddrLen+1)*i],1);
			dp.DIQ.DPI = (bDiq>>DPI_OFFSET)&BIT_SIZE2;
			dp.DIQ.BL  =  (bDiq>>BL_OFFSET)&BIT_SIZE1;
			dp.DIQ.SB  =  (bDiq>>SB_OFFSET)&BIT_SIZE1;
			dp.DIQ.NT  =  (bDiq>>NT_OFFSET)&BIT_SIZE1;
			dp.DIQ.IV   =  (bDiq>>IV_OFFSET)&BIT_SIZE1;
			dp.iOffset = PointNo;
			vDoublePoint.push_back(dp);	
		}
	}
	if(iExpYxNum>0)
	{
		GetHandler()->handleDoublePoint(rtuAddr, btCot, vDoublePoint);
	}
	return 0;
}
int CIec104::AppRxAsdu_5(const b_ui8 *asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,rtuAddr=0;	
	int		iInf=0,iInfAddrLen=0,iExpYxNum=0;;
	b_ui8	btSeqflag=0, btCot=0,bVti=0,bQds=0;
	b_ui8	stvalue=0,stflag=0,stt=0;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SStepPosition> vStepPosition;
	SStepPosition sp;
	
	iNum = asdu[1]&0x7f;
   	 btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	if( btCot >= IEC104_COT_RESP_GRP1 && btCot <= IEC104_COT_RESP_GRP8 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 1;
	}

	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if(btSeqflag )
	{
		PointNo = 0;
		for (i=iInfAddrLen; i>0; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}
		for(i=0; i<iNum; i++,PointNo++)
		{
			tmpIndex=iInf + iInfAddrLen ;
			sp.iOffset = PointNo;
			memcpy(&bVti,&asdu[tmpIndex],1);
			memcpy(&bQds,&asdu[tmpIndex+1],1);
			sp.VTI.Value = (bVti&0x7f);
			sp.VTI.Transient=( bVti>>7)&BIT_SIZE1;
			sp.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sp.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sp.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sp.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sp.QDS.IV= (bQds>>IV_OFFSET)&BIT_SIZE1;
			vStepPosition.push_back(sp);
			tmpIndex+=2;
			iExpYxNum ++;
		}
	}
	else
	{
		tmpIndex=iInf + iInfAddrLen;
		for( i=0; i<iNum; i++ )
		{
			PointNo = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+2)*i-1];
			}
			sp.iOffset = PointNo;
			memcpy(&bVti,&asdu[tmpIndex],1);
			memcpy(&bQds,&asdu[tmpIndex+1],1);
			sp.VTI.Value = (bVti&0x7f);
			sp.VTI.Transient=( bVti>>7)&BIT_SIZE1;
			sp.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sp.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sp.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sp.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sp.QDS.IV= (bQds>>IV_OFFSET)&BIT_SIZE1;
			vStepPosition.push_back(sp);
			tmpIndex+=5;
			iExpYxNum ++;	
		}
	}
	if(iExpYxNum>0)
	{
		GetHandler()->handleStepPosition(rtuAddr, btCot, vStepPosition);
	}
	return 0;
}
int CIec104::AppRxAsdu_30(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8 btSeqflag,btCot,bSiq=0;
	b_ui16 rtuAddr;
	int iNum,PointNo,iExpYxNum=0,iInf,iInfAddrLen,i;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SSinglePointTimeTag> vSinglePointTimeTag;
	SSinglePointTimeTag sptm;
	
	iNum = asdu[1]&0x7f;
       btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btSeqflag ) 
		return -1;
	for ( i=0; i<iNum; i++ ,iExpYxNum ++)
	{
		PointNo = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+8)*i-1];
		}
		SFepTime timeStamp;
		GetTmStmp(&asdu[iInf + iInfAddrLen + 1 + (iInfAddrLen+8)*i], &timeStamp);
		memcpy(&bSiq,&asdu[iInf + iInfAddrLen + (iInfAddrLen+8)*i],1);
		sptm.SIQ.SPI = (bSiq>>SPI_OFFSET)&BIT_SIZE1;
		sptm.SIQ.BL  = (bSiq>>BL_OFFSET)&BIT_SIZE1;
		sptm.SIQ.SB  = (bSiq>>SB_OFFSET)&BIT_SIZE1;
		sptm.SIQ.NT  = (bSiq>>NT_OFFSET)&BIT_SIZE1;
		sptm.SIQ.IV  = (bSiq>>IV_OFFSET)&BIT_SIZE1;
		sptm.iOffset = PointNo;
		sptm.cp56TimeTag = timeStamp;
		vSinglePointTimeTag.push_back(sptm);		
	}
	if(iExpYxNum>0)
	{
		GetHandler()->handleSinglePointWithTimeTag(rtuAddr, btCot, vSinglePointTimeTag);
	}
	return 0;
}
int CIec104::AppRxAsdu_31(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8 btSeqflag,btCot,bDiq;
	b_ui16 rtuAddr;
	int iNum,PointNo,iExpYxNum=0,iInf,iInfAddrLen,i;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SDoublePointTimeTag> vDoublePointTimeTag;
	SDoublePointTimeTag dptm;
	
	iNum = asdu[1]&0x7f;
       btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btSeqflag ) 
		return -1;
	for ( i=0; i<iNum; i++ )
	{
		PointNo = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+8)*i-1];
		}
		SFepTime timeStamp;
		GetTmStmp(&asdu[iInf + iInfAddrLen + 1 + (iInfAddrLen+8)*i], &timeStamp);		
		dptm.iOffset = PointNo;
		dptm.cp56TimeTag = timeStamp;
		memcpy(&bDiq,&asdu[iInf + iInfAddrLen + (iInfAddrLen+8)*i],1);
		dptm.DIQ.DPI = (bDiq>>DPI_OFFSET)&BIT_SIZE2;
		dptm.DIQ.BL  =  (bDiq>>BL_OFFSET)&BIT_SIZE1;
		dptm.DIQ.SB  =  (bDiq>>SB_OFFSET)&BIT_SIZE1;
		dptm.DIQ.NT  =  (bDiq>>NT_OFFSET)&BIT_SIZE1;
		dptm.DIQ.IV   =  (bDiq>>IV_OFFSET)&BIT_SIZE1;
		vDoublePointTimeTag.push_back(dptm);
		iExpYxNum ++;
	}
	if(iExpYxNum>0)
	{
		GetHandler()->handleDoublePointWithTimeTag(rtuAddr, btCot, vDoublePointTimeTag);
	}
	return 0;
}
int CIec104::AppRxAsdu_32(const b_ui8 *asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,rtuAddr=0, iExpYxNum=0;	
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bVti=0,bQds=0;
	SIec104Data *p104 = m_pIec104Data;
	std::vector<SStepPositionTimeTag> vStepPositionTimeTag;
	SStepPositionTimeTag sptm;
	iNum = asdu[1]&0x7f;
   	 btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	tmpIndex = iInf + iInfAddrLen;
	if(btSeqflag)
		return -1;
	SFepTime timeStamp;
	for( i=0; i<iNum; i++ )
	{
		PointNo = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+9)*i-1];
		}				
		GetTmStmp(&asdu[tmpIndex+ 2], &timeStamp);
		memcpy(&bVti,&asdu[tmpIndex],1);
		memcpy(&bQds,&asdu[tmpIndex+1],1);
		sptm.VTI.Value = (bVti&0x7f);
		sptm.VTI.Transient=( bVti>>7)&BIT_SIZE1;
		sptm.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
		sptm.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
		sptm.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
		sptm.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
		sptm.QDS.IV= (bQds>>IV_OFFSET)&BIT_SIZE1;
		sptm.iOffset = PointNo;
		sptm.cp56TimeTag = timeStamp;
		vStepPositionTimeTag.push_back(sptm);
		tmpIndex+=12;
		iExpYxNum ++;
	}
	if(iExpYxNum>0)
	{
		GetHandler()->handleStepPositionWithTimeTag(rtuAddr, btCot, vStepPositionTimeTag);
	}
	return 0;
}

b_bool CIec104::GetTmStmp(const b_ui8 *pBuf, SFepTime *pTmStmp)
{
	int iMilliSec = (pBuf[1]*256+pBuf[0])%1000;
    int iMinute = pBuf[2]&0x3f;
	int iSec = (pBuf[1]*256+pBuf[0])/1000;
	int iHour = pBuf[3]&0x1f;
	int iDay = pBuf[4]&0x1f;
	int iYear = (pBuf[6]&0x7f)+2000;
	int iMonth = (pBuf[5]&0x0f);

	pTmStmp->year = iYear;
	pTmStmp->month = iMonth;
	pTmStmp->day = iDay;
	pTmStmp->hour = iHour;
	pTmStmp->minute = iMinute;
	pTmStmp->millisecond = iMilliSec;
	pTmStmp->second = iSec;

#ifdef _DEBUG
#endif	
	return true;
}
int CIec104::AppRxAsdu_9(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8  btSeqFlag, btCot,bQds=0;
	b_ui16 bSourceNVA,bDestNVA;
	int	  i, iYcOrder,iExpYcNum = 0;
	int	iInf,iInfAddrLen;
	int   iYcNum;
	SIec104Data *p104= m_pIec104Data;
	vector<SMeasuredValueNVA> vMeasuredValueNVA;
	SMeasuredValueNVA nva;
	
	iYcNum = asdu[1]&0x7f;
	btSeqFlag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	int rtuAddr= getRtuAddr(asdu);
	if( btCot >= IEC104_COT_RESP_GRP9 && btCot <= IEC104_COT_RESP_GRP12 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
	}
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btSeqFlag )
	{
		iYcOrder = 0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			iYcOrder = iYcOrder*256 + asdu[iInf+i-1];
		}
		for( i=0; i<iYcNum; i++, iYcOrder++ )
		{
			memcpy(&bSourceNVA,&asdu[iInf + iInfAddrLen +3*i],2);
			bDestNVA = p104tohost16(bSourceNVA);
			memcpy(&nva.NVA,&bDestNVA,2);
			memcpy(&bQds,&asdu[iInf + iInfAddrLen+2+3*i],1);
			nva.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			nva.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			nva.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			nva.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			nva.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			nva.iOffset = iYcOrder;
			vMeasuredValueNVA.push_back(nva);
			iExpYcNum ++;
		}
	}
	else
	{
		for( i=0; i<iYcNum; i++ )
		{
			iYcOrder = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				iYcOrder = iYcOrder*256 + asdu[iInf+j+(iInfAddrLen+3)*i-1];
			}
			memcpy(&bSourceNVA,&asdu[iInf + iInfAddrLen+(iInfAddrLen +3)*i],2);
			bDestNVA = p104tohost16(bSourceNVA);
			memcpy(&nva.NVA,&bDestNVA,2);
			memcpy(&bQds,&asdu[iInf + iInfAddrLen+2+(iInfAddrLen +3)*i],1);
			nva.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			nva.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			nva.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			nva.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			nva.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			nva.iOffset = iYcOrder;
			vMeasuredValueNVA.push_back(nva);
			iExpYcNum ++;
			iExpYcNum ++;
		}
	}
	if(iExpYcNum>0) 
	{
		GetHandler()->handleMeasureValueNVA(rtuAddr,  btCot, vMeasuredValueNVA);
	}
	return 0;
}
int CIec104::AppRxAsdu_11(const b_ui8 *asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,rtuAddr;
	b_ui8	btSeqflag=0, btCot=0,bQds=0;
	b_i16     bSourceSVA,bDestSVA=0;
	b_ui8	flag=0;
	SIec104Data *p104=m_pIec104Data;
	vector<SMeasuredValueSVA> vMeasuredValueSVA;
	SMeasuredValueSVA sva;


	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);	
	infIndex=0;PointNo=0;
	if( btSeqflag )
	{
		PointNo=0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}		
		for( i=0; i<iNum; i++, PointNo++ )
		{
			tmpIndex = iInf + iInfAddrLen +3*i;			
			sva.iOffset = PointNo;
			memcpy(&bSourceSVA,&asdu[tmpIndex],2);
			bDestSVA = p104tohost16(bSourceSVA);
			memcpy(&sva.SVA, &bDestSVA,2);
			memcpy(&bQds,&asdu[tmpIndex+2],1);
			sva.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sva.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sva.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sva.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sva.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			vMeasuredValueSVA.push_back(sva);
			infIndex++;
		}
	}
	else
	{
		for( i=0; i<iNum; i++ )
		{
			PointNo = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo =PointNo*256 + asdu[iInf+j+(iInfAddrLen+3)*i-1];
			}
			tmpIndex = iInf + iInfAddrLen +6*i;
			sva.iOffset = PointNo;
			memcpy(&bSourceSVA,&asdu[tmpIndex],2);
			bDestSVA = p104tohost16(bSourceSVA);
			memcpy(&sva.SVA, &bDestSVA,2);
			memcpy(&bQds, &asdu[tmpIndex+2],1);
			sva.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sva.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sva.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sva.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sva.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			vMeasuredValueSVA.push_back(sva);
			infIndex ++;	
		}
	}
	if(infIndex>0) 
	{
		GetHandler()->handleMeasureValueSVA(rtuAddr,  btCot, vMeasuredValueSVA);
	}
	return 0;
}
int CIec104::AppRxAsdu_13(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8  btSeqFlag,btCot,bQds=0;
	int	  i, iYcOrder,iExpYcNum = 0;
	int	iInf,iInfAddrLen;
	int   iYcNum;
	b_ui32 bSrc,bDst;
	SIec104Data *p104= m_pIec104Data;	
	vector<SMeasuredValueSFA> vMeasuredValueSFA;
	SMeasuredValueSFA sfa;
	
	iYcNum = asdu[1]&0x7f;
	btSeqFlag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	int rtuAddr= getRtuAddr(asdu);
	if( btCot >= IEC104_COT_RESP_GRP9 && btCot <= IEC104_COT_RESP_GRP12 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
	}
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btSeqFlag )
	{
		iYcOrder = 0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			iYcOrder = iYcOrder*256 + asdu[iInf+i-1];
		}			
		for( i=0; i<iYcNum; i++, iYcOrder++ )
		{
			sfa.iOffset = iYcOrder;
			memcpy(&bSrc,&asdu[iInf + iInfAddrLen +5*i] ,4);
			bDst = p104tohost32(bSrc);
			memcpy(&sfa.SFA, &bDst,4);
			memcpy(&bQds,&asdu[iInf + iInfAddrLen+4+5*i],1);
			sfa.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sfa.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sfa.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sfa.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sfa.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			vMeasuredValueSFA.push_back(sfa);
			iExpYcNum ++;
		}
	}
	else
	{
		for( i=0; i<iYcNum; i++ )
		{
			iYcOrder = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				iYcOrder = iYcOrder*256 + asdu[iInf+j+(iInfAddrLen+5)*i-1];
			}
			sfa.iOffset = iYcOrder;
			memcpy(&bSrc,&asdu[iInf + iInfAddrLen+(iInfAddrLen +5)*i],4);
			bDst = p104tohost32(bSrc);
			memcpy(&sfa.SFA, &bDst,4);
			memcpy(&bQds, &asdu[iInf + iInfAddrLen+4+(iInfAddrLen +5)*i],1);
			sfa.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			sfa.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			sfa.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			sfa.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			sfa.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			vMeasuredValueSFA.push_back(sfa);
			iExpYcNum ++;
		}
	}
	if(iExpYcNum>0) 
	{	
		GetHandler()->handleMeasureValueSFA(rtuAddr,  btCot, vMeasuredValueSFA);
	}
	return 0;
}
int CIec104::AppRxAsdu_21(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8  btSeqFlag,btCot;
	int	  i, iYcOrder,iExpYcNum = 0;
	b_ui16 bSouceNVA,bDestNVA=0;
	int	iInf,iInfAddrLen;
	int   iYcNum;
	SIec104Data *p104= m_pIec104Data;
	vector<SMeasuredValueNVA2> vMeasuredValueNVA2;
	SMeasuredValueNVA2 nva2;
	
	iYcNum = asdu[1]&0x7f;
	btSeqFlag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	int rtuAddr= getRtuAddr(asdu);
	if( btCot >= IEC104_COT_RESP_GRP9 && btCot <= IEC104_COT_RESP_GRP12 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
	}
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btSeqFlag )
	{
		iYcOrder = 0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			iYcOrder = iYcOrder*256 + asdu[iInf+i-1];
		}		
		for( i=0; i<iYcNum; i++, iYcOrder++ )
		{			
			nva2.iOffset =  iYcOrder;
			memcpy(&bSouceNVA,&asdu[iInf + iInfAddrLen +2*i],2);
			bDestNVA = p104tohost16(bSouceNVA);
			memcpy(&nva2.NVA, &bDestNVA,2);
			vMeasuredValueNVA2.push_back(nva2);
			iExpYcNum ++;
		}			
	}
	else
	{
		for( i=0; i<iYcNum; i++ )
		{
			iYcOrder = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				iYcOrder = iYcOrder*256 + asdu[iInf+j+(iInfAddrLen+2)*i-1];
			}
			memcpy(&bSouceNVA,&asdu[iInf + iInfAddrLen+(iInfAddrLen +2)*i],2);
			bDestNVA = p104tohost16(bSouceNVA);
			memcpy(&nva2.NVA,&bDestNVA,2);
			nva2.iOffset = iYcOrder;
			vMeasuredValueNVA2.push_back(nva2);
			iExpYcNum ++;
			
		}			
	}
	if(iExpYcNum>0) 
	{	
		GetHandler()->handleMeasureValueNVAWithoutQualityDescriptor(rtuAddr, btCot,vMeasuredValueNVA2);
	}
	return 0;
}
int CIec104::AppRxAsdu_34(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8  btSeqFlag, btCot,bQds=0;
	int	  i, iYcOrder, iExpYcNum = 0;
	b_ui16 bSourceNVA,bDestNVA;
	int		iInf,iInfAddrLen;
	int   iYcNum;
	SIec104Data *p104;
	SFepTime timeStamp;		
	vector<SMeasuredValueNVATimeTag> vMeasuredValueNVATm;
	SMeasuredValueNVATimeTag nvaTm;

	p104 = m_pIec104Data;
	iYcNum = asdu[1]&0x7f;
	btSeqFlag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	int rtuAddr= getRtuAddr(asdu);

	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for( i=0; i<iYcNum; i++ )
	{
		iYcOrder = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			iYcOrder = iYcOrder*256 + asdu[iInf+j+(iInfAddrLen+10)*i-1];
		}
		GetTmStmp( &asdu[iInf + iInfAddrLen+3+(iInfAddrLen +10)*i], &timeStamp);
		nvaTm.iOffset = iYcOrder;
		nvaTm.cp56TimeTag = timeStamp;
		memcpy(&bSourceNVA,&asdu[iInf + iInfAddrLen+(iInfAddrLen +10)*i],2);
		bDestNVA = p104tohost16(bSourceNVA);
		memcpy(&nvaTm.NVA, &bDestNVA,2);
		memcpy(&bQds, &asdu[iInf + iInfAddrLen+2+(iInfAddrLen +10)*i],1);
		nvaTm.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
		nvaTm.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
		nvaTm.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
		nvaTm.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
		nvaTm.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
		vMeasuredValueNVATm.push_back(nvaTm);
		iExpYcNum ++;
	}
	if(iExpYcNum>0) 
	{	
		GetHandler()->handleMeasureValueNVAWithTimeTag(rtuAddr,  btCot, vMeasuredValueNVATm);
	}
	return 0;
}
int CIec104::AppRxAsdu_35(const b_ui8 *asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,rtuAddr;
	b_ui8	btSeqflag=0, btCot=0,bQds=0;
	b_ui16     bSrcSVA,bDestSVA=0;
	b_ui8	flag=0;
	SIec104Data *p104= m_pIec104Data;;
	vector<SMeasuredValueSVATimeTag> vMeasuredValueSVATm;
	SMeasuredValueSVATimeTag svaTm;
	SFepTime timeStamp;

	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);	
	infIndex=0;PointNo=0;
	if( btSeqflag )
		return -1;
	for( i=0; i<iNum; i++ )
	{
		PointNo=0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo =PointNo*256 + asdu[iInf+j+(iInfAddrLen+10)*i-1];
		}
		tmpIndex = iInf + iInfAddrLen+(iInfAddrLen +10)*i;
		GetTmStmp( &asdu[tmpIndex+3], &timeStamp);	
		memcpy(&bSrcSVA,&asdu[tmpIndex],2);
		bDestSVA = p104tohost16(bSrcSVA);
		memcpy(&svaTm.SVA,&bDestSVA,2);
		memcpy(&bQds, &asdu[tmpIndex+2],1);
		svaTm.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
		svaTm.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
		svaTm.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
		svaTm.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
		svaTm.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
		svaTm.iOffset = PointNo;
		svaTm.cp56TimeTag = timeStamp;
		vMeasuredValueSVATm.push_back(svaTm);
		infIndex ++;	
	}
	if(infIndex>0) 
	{
		GetHandler()->handleMeasureValueSVAWithTimeTag(rtuAddr,  btCot, vMeasuredValueSVATm);
	}
	return 0;
}
int CIec104::AppRxAsdu_36(const b_ui8 *asdu, b_ui8 size)
{
	b_ui8  btSeqFlag, btCot,bQds=0;
	b_ui32 bSrc,bDst;
	int	  i, iYcOrder, iExpYcNum = 0,rtuAddr=0;
	int		iInf,iInfAddrLen;
	int   iYcNum;
	SIec104Data *p104= m_pIec104Data;
	vector<SMeasuredValueSFATimeTag> vMeasuredValueSFATm;
	SMeasuredValueSFATimeTag sfaTm;
	SFepTime timeStamp;	
	iYcNum = asdu[1]&0x7f;
	btSeqFlag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	rtuAddr= getRtuAddr(asdu);
	if( btCot >= IEC104_COT_RESP_GRP9 && btCot <= IEC104_COT_RESP_GRP12 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPDATA )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
	}
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for( i=0; i<iYcNum; i++ )
	{
		iYcOrder = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			iYcOrder = iYcOrder*256 + asdu[iInf+j+(iInfAddrLen+12)*i-1];
		}
		GetTmStmp( &asdu[iInf + iInfAddrLen+5+(iInfAddrLen +12)*i], &timeStamp );			
		sfaTm.iOffset = iYcOrder;
		sfaTm.cp56TimeTag = timeStamp;
		memcpy(&bSrc,&asdu[iInf + iInfAddrLen+(iInfAddrLen +12)*i],4);
		bDst= p104tohost32(bSrc);
		memcpy(&sfaTm.SFA, &bDst,4);
		memcpy(&bQds, &asdu[iInf + iInfAddrLen+4+(iInfAddrLen +12)*i],1);
		sfaTm.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
		sfaTm.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
		sfaTm.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
		sfaTm.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
		sfaTm.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
		vMeasuredValueSFATm.push_back(sfaTm);
		iExpYcNum ++;
	}
	if(iExpYcNum>0) 
	{	
		GetHandler()->handleMeasureValueSFAWithTimeTag(rtuAddr,  btCot, vMeasuredValueSFATm);
	}
	return 0;
}

int CIec104::AppRxAsdu_15(const b_ui8 *pAsdu, b_ui8 size)
{
	int  iKwhNum ;
	b_ui8  btSeqFlag,btCot, iExpPulNum=0,bsqnt=0;
	int	  iKwhNo, i,iRtuAddr=0;
	b_ui32 bSrcCounter,bDstCounter;
	int		iInf,iInfAddrLen;
	SIec104Data *p104 = m_pIec104Data;
	vector<SCounterValue> vCounterValue;
	SCounterValue cv;
	iRtuAddr = getRtuAddr(pAsdu);
       btSeqFlag = pAsdu[1]&0x80;
	iKwhNum = pAsdu[1]&0x7f;
	btCot = pAsdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;	
	if( btCot >= IEC104_COT_RESP_CO_GRP1 && btCot <= IEC104_COT_RESP_CO_GRP4 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPKWH )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 1;
	}
	if( btSeqFlag )
	{
		iKwhNo = 0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			iKwhNo = iKwhNo*256 + pAsdu[iInf+i-1];
		}
		for( i=0; i<iKwhNum; i++, iKwhNo++,iExpPulNum ++ )
		{
			memcpy(&bSrcCounter, &pAsdu[iInf+iInfAddrLen+5*i],4);
			bDstCounter = p104tohost32(bSrcCounter);
			memcpy(&cv.BCR.Counter, &bDstCounter,4);
			memcpy(&bsqnt,&pAsdu[iInf+iInfAddrLen+4+5*i],1);
			cv.BCR.SequenceNotation.SQ = bsqnt&BIT_SIZE5;
			cv.BCR.SequenceNotation.CY = (bsqnt>>CY_OFFSET)&BIT_SIZE1;
			cv.BCR.SequenceNotation.CA = (bsqnt>>CA_OFFSET)&BIT_SIZE1;
			cv.BCR.SequenceNotation.IV = (bsqnt>>IV_OFFSET)&BIT_SIZE1;
			cv.iOffset = iKwhNo;
			vCounterValue.push_back(cv);
		}
	}
	else
	{
		for( i=0; i<iKwhNum; i++ )
		{
			iKwhNo = 0;//pBuf[5+7*i]*256 + pBuf[4+7*i]- 0xc01;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				iKwhNo = iKwhNo*256 + pAsdu[iInf+j+(iInfAddrLen+5)*i-1];
			}
			memcpy(&bSrcCounter,&pAsdu[iInf+iInfAddrLen+(iInfAddrLen+5)*i],4);
			bDstCounter = p104tohost32(bSrcCounter);
			memcpy(&cv.BCR.Counter, &bDstCounter,4);
			memcpy(&bsqnt,&pAsdu[iInf+iInfAddrLen+4+(iInfAddrLen+5)*i],1);
			cv.BCR.SequenceNotation.SQ = bsqnt&BIT_SIZE5;
			cv.BCR.SequenceNotation.CY = (bsqnt>>CY_OFFSET)&BIT_SIZE1;
			cv.BCR.SequenceNotation.CA = (bsqnt>>CA_OFFSET)&BIT_SIZE1;
			cv.BCR.SequenceNotation.IV = (bsqnt>>IV_OFFSET)&BIT_SIZE1;
			cv.iOffset = iKwhNo;
			vCounterValue.push_back(cv);			
			iExpPulNum ++;
		}
	}
	if(iExpPulNum>0)
	{
		GetHandler()->handleIntegratedTotal(iRtuAddr,  btCot, vCounterValue);
	}
	return 0;
}
int CIec104::AppRxAsdu_37(const b_ui8 *pAsdu, b_ui8 size)
{
	int  iKwhNum ;
	b_ui8  btSeqFlag, btCot,bsqnt=0;
	int	  iKwhNo, i,iRtuAddr=0;
	b_ui32 bSrcCounter,bDstCounter=0;
	int		iInf,iInfAddrLen;
	SFepTime timeStamp;
	SIec104Data *p104 = m_pIec104Data;
	vector<SCounterValueTimeTag> vCounterValueTm;
	SCounterValueTimeTag cvtm;
	iRtuAddr = getRtuAddr(pAsdu);
	int iExpPulNum=0;
       btSeqFlag = pAsdu[1]&0x80;
	iKwhNum = pAsdu[1]&0x7f;
	btCot = pAsdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if( btCot >= IEC104_COT_RESP_CO_GRP1 && btCot <= IEC104_COT_RESP_CO_GRP4 
		&& p104->AppState == IEC104_APP_STATE_WAITGRPKWH )
	{
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 1;
	}
	for( i=0; i<iKwhNum; i++ )
	{
		iKwhNo = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			iKwhNo = iKwhNo*256 + pAsdu[iInf+j+(iInfAddrLen+12)*i-1];
		}
		
		GetTmStmp(&pAsdu[iInf+iInfAddrLen+5+(iInfAddrLen+12)*i], &timeStamp );
		memcpy(&bSrcCounter,&pAsdu[iInf+iInfAddrLen+(iInfAddrLen+12)*i],4);
		bDstCounter = p104tohost32(bSrcCounter);
		memcpy(&cvtm.BCR.Counter,&bDstCounter,4);
		memcpy(&bsqnt,&pAsdu[iInf+iInfAddrLen+4+(iInfAddrLen+12)*i],1);
		cvtm.BCR.SequenceNotation.SQ = bsqnt&BIT_SIZE5;
		cvtm.BCR.SequenceNotation.CY = (bsqnt>>CY_OFFSET)&BIT_SIZE1;
		cvtm.BCR.SequenceNotation.CA = (bsqnt>>CA_OFFSET)&BIT_SIZE1;
		cvtm.BCR.SequenceNotation.IV = (bsqnt>>IV_OFFSET)&BIT_SIZE1;
		cvtm.cp56TimeTag = timeStamp;
		cvtm.iOffset = iKwhNo;
		vCounterValueTm.push_back(cvtm);
		iExpPulNum ++;			
	}
	if(iExpPulNum>0)
	{
		GetHandler()->handleIntegratedTotalWithTimeTag(iRtuAddr,  btCot, vCounterValueTm);
	}
	return 0;
}

b_bool CIec104::GetCurTm(SFepTime &t)
{
	time_t ti;
	ti = time(&ti);
	struct tm * gm = gmtime(&ti);
	t.year = gm->tm_year;
	t.month = gm->tm_mon;
	t.day   = gm->tm_mday;
	t.hour  = gm->tm_hour;
	t.minute= gm->tm_min;
	t.second= gm->tm_sec;
	t.millisecond = gm->tm_isdst;
	return true;
}




int CIec104::LinkTxFrameS()
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	
	CIec104_Apci apci(IEC104_FRAME_S,p104->RxSeq );
	
	if( m_pIec104Data->LinkState >= IEC104_LK_STATE_STARTED)
	{	
		if ( !WriteBuff( apci.GetApciAddr(), IEC104_APCI_SIZE ))
		{
			return -1;
		}
		m_pChannel->Write();
		p104->LastAckSeq = p104->RxSeq;	
		resetTimer(3);
		stopTimer(2);	
	}
	else 
	{
		return -1;
	}
	return 1;
}

int CIec104::LinkTxFrameU(b_ui8 iFrameType)
{
	SIec104Data *p104;
	CIec104_Apci apci(iFrameType );
 	 p104 = m_pIec104Data ;
			
	if ( !WriteBuff( apci.GetApciAddr() , IEC104_APCI_SIZE ) )
	{
		return -1;
	}		
	resetTimer(3);
	switch ( iFrameType )
	{
		case IEC104_FRAME_U_START_ACT:
		case IEC104_FRAME_U_STOP_ACT:
		case IEC104_FRAME_U_TEST_ACT:
			resetTimer(1);
			break;
		case IEC104_FRAME_U_TEST_CON:
			break;
		default:
			return -1;
	}	
	return  1;
}

int CIec104::AppTxAsduType_100(int rtuAddr, b_ui8 bCot, b_ui8 btQoi)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_IC_NA,1,bCot,rtuAddr,0,&btQoi,1);
	
	if( LinkTxFrameI( asdu.GetAsduAddr() , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_IDLE; 
		resetTimer(4);
		return 1;
	}
	else 
		return -1;
}

int CIec104::LinkTxFrameI(const b_ui8 *pAsdu, b_ui8 size, b_ui8 frmtype)
{
	SIec104Data *p104;	
	
    p104 = m_pIec104Data ;
	
	if( p104->LinkState == IEC104_LK_STATE_STARTED)
	{
		if( m_txBuf->getFreeLength()< size+IEC104_APCI_SIZE )
			return -1;
		CIec104_Apci apci(IEC104_FRAME_I,p104->RxSeq,p104->TxSeq,size+4);
		CIec104_Apdu apdu(apci.m_pBuf,pAsdu,size);
		
		if ( !WriteBuff( apdu.m_pBuf , size+IEC104_APCI_SIZE ) )
		{
			return -1;
		}
		
		p104->TxSeq =(++p104->TxSeq)%32768;
		p104->LastAckSeq = p104->RxSeq;
		stopTimer(2);
		if( ((p104->TxSeq +32768 - p104->AckSeq )%32768) >= p104->LinkPara->K )
		{
			p104->NextCmd = IEC104_CMD_PEND;
		}
		resetTimer(3);
		resetTimer(1);
		return 1;
	}
	else 
	{
		return -1;
	}

	return 0;
}


int CIec104::AppTxAsduType_103(int rtuAddr)
{
	SIec104Data *p104= m_pIec104Data;
	b_ui8	btBuf[7];
    	AppSyncTimeCoding( btBuf );
	
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_CS_NA,1,IEC104_COT_ACT,rtuAddr,0,btBuf,7);	
	if( LinkTxFrameI( asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_IDLE;		
		return 1;
	}
	else return -1;
}

void CIec104::AppSyncTimeCoding(b_ui8 *const buf)
{
	time_t ltime;
	time( &ltime );	
	struct tm *tt;
	tt = localtime(&ltime);
	int iMs = tt->tm_isdst + tt->tm_sec * 1000;
	int  iMs1 = iMs % 60000;
	b_ui8 iMinute = tt->tm_min;	
	b_ui8 iHour = tt->tm_hour %24;
	buf[0] = iMs1 %256;
	buf[1] =  iMs1 /256;
	buf[2] = iMinute &0x3f;
	buf[3] = iHour &0x1f;
	buf[4] = ((tt->tm_wday & 0x07) << 5) | ((tt->tm_mday )& 0x1f);
	buf[5] = (tt->tm_mon +1)& 0x0f;
	buf[6] = tt->tm_year - 100; 
}


int CIec104::AppTxAsduType_101(int rtuAddr ,b_ui8 bCot,b_ui8 btQcc)
{
	SIec104Data *p104= m_pIec104Data;
	b_ui8 iFlag=1;

	if(btQcc==0x41)
		iFlag=0;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_CI_NA,1,bCot,rtuAddr,0,&btQcc,1);	
	if(  LinkTxFrameI( asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		//p104->AppState = (iFlag)?IEC104_APP_STATE_WAITALLKWHCONF:IEC104_APP_STATE_WAITFRZKWHCONF;
		p104->AppState = IEC104_APP_STATE_IDLE;
		m_bRecvFinish = 0;
		return 1;
	}
	else return -1;
}

int CIec104::AppTxAsduType_107(int rtuAddr, int infoAddr, b_ui16 bTsc )
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	
	b_ui8 btBuf[9];
	memcpy(btBuf,&bTsc,2);
	AppSyncTimeCoding(btBuf+2);
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_TS_TA,1,IEC104_COT_ACT,rtuAddr,infoAddr,btBuf,9);	
	if( LinkTxFrameI( asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_IDLE; 
		return 1;
	}
	
	return -1;	
}

/********************************************************************************
*		Description:send reset process command
*		Parameters:cycloe sequence number
*		Return:1:send success;-1:send fail.
********************************************************************************/
int CIec104::AppTxAsduType_105(int rtuAddr,b_ui8  btQrp)
{
	SIec104Data *p104= m_pIec104Data;;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_RP_NA, 1,IEC104_COT_ACT,rtuAddr,0,&btQrp,1);	
	if( LinkTxFrameI( asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_IDLE; 
		return 1;
	}
	return -1;	
}

/********************************************************************************
*		Description:Interrogation grouped data
*		Parameters:cycle sequence number,grouped number
*		Return:1:send success;-1:send fail
********************************************************************************/
int CIec104::AppTxCallGrpData(int iGroup)
{
	b_ui8 btBuf = 20+iGroup;
	SIec104Data *p104;
	
	p104 = m_pIec104Data;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_IC_NA,1,IEC104_COT_REQ,p104->btRtuAddr,0,&btBuf,1);
	
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		//p104->AppState = IEC104_APP_STATE_WAITGRPDATA;
		return 1;
	}
	else return -1;
}

int CIec104::AppTxCallGrpKwh(int group)
{
	SIec104Data *p104;
	b_ui8 btBuf = 0x40 |( (group +1)&0x3f);
	
	p104 = m_pIec104Data;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_CI_NA,1,IEC104_COT_REQ,p104->btRtuAddr,0,&btBuf,1);
	
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		//p104->AppState = IEC104_APP_STATE_WAITGRPKWH;
		return 1;
	}
	
	return -1;
}



int CIec104::AppTxAsduType_45(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bSco)
{
	b_ui8 bTi = IEC104_C_SC_NA;
	b_ui8 bCmd=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bSco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,&bSco,1);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}

int CIec104::AppTxAsduType_46(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bDco)
{
	b_ui8 bTi = IEC104_C_DC_NA;
	b_ui8 bCmd=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bDco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,&bDco,1);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}


int CIec104::AppTxAsduType_47(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bRco)
{
	b_ui8 bTi = IEC104_C_RC_NA;
	b_ui8 bCmd=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bRco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,&bRco,1);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
int CIec104::AppTxAsduType_58(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bSco,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_SC_TA;
	b_ui8 bCmd=0,btBuf[8],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bSco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	btBuf[0] = bSco;
	memcpy(btBuf+1,bTimeTag,7);
	bLen = 8;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
int CIec104::AppTxAsduType_59(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bDco,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_DC_TA;
	b_ui8 bCmd=0,btBuf[8],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bDco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	btBuf[0] = bDco;
	memcpy(btBuf+1,bTimeTag,7);
	bLen = 8;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
int CIec104::AppTxAsduType_60(int rtuAddr,int infoAddr ,b_ui8 bCot,b_ui8 bRco,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_RC_TA;
	b_ui8 bCmd=0,btBuf[8],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bRco&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	btBuf[0] = bRco;
	memcpy(btBuf+1,bTimeTag,7);
	bLen = 8;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState =IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}

/*
*          C_RD_NA_1 102  read command   cot =5  InfAddr=0;
*/

int CIec104::AppTxAsduType_102(int rtuAddr,int infoAddr)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	CIec104_Asdu asdu(p104->AppPara,IEC104_C_RD_NA,1,IEC104_COT_REQ,rtuAddr,infoAddr,NULL,0);
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
	
}

/*
* parameter of measured value
   110 normalized value
   111 scaled value
   112  short floating-point number
*/
int  CIec104::AppTxAsduType_110(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;	
	b_ui8 btBuf[3];
	memcpy(btBuf,bValBuf,2);
	memcpy(btBuf+2,&bQpm,1);
	CIec104_Asdu asdu(p104->AppPara,IEC104_P_ME_NA,1,IEC104_COT_ACT,rtuAddr,infoAddr,btBuf,3);
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
}
int  CIec104::AppTxAsduType_111(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;	
	b_ui8 btBuf[3];
	memcpy(btBuf,bValBuf,2);
	memcpy(btBuf+2,&bQpm,1);
	CIec104_Asdu asdu(p104->AppPara,IEC104_P_ME_NB,1,IEC104_COT_ACT,rtuAddr,infoAddr,btBuf,3);
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
}
int  CIec104::AppTxAsduType_112(int rtuAddr, int infoAddr,b_ui8 *bValBuf,b_ui8 bQpm)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;	
	b_ui8 btBuf[5];
	memcpy(btBuf,bValBuf,4);
	memcpy(btBuf+4,&bQpm,1);
	CIec104_Asdu asdu(p104->AppPara,IEC104_P_ME_NC,1,IEC104_COT_ACT,rtuAddr,infoAddr,btBuf,5);
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
}


/*
*           P-AC-NA-1  113 parameter activation
*/
int  CIec104::AppTxAsduType_113(int rtuAddr,b_ui8 bCot,b_ui32 iInfoAddr,b_ui8 bQpa)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	CIec104_Asdu asdu(p104->AppPara,IEC104_P_NC_NA,1,bCot,rtuAddr,iInfoAddr,&bQpa,1);
	if( LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		return 1;
	}	
	return -1;
}

/*
*   se = 0 exe,
     se = 1 select  
     type = IEC104_C_SE_NA  normalized value
     type = IEC104_C_SE_NB  scaled value
     type = IEC104_C_SE_NC   short floating-point number
     type = IEC104_C-BO-NA  bitstring of 32 bits
*/
 int CIec104::AppTxAsduType_48(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui16 Nva,b_ui8 bQos)
{
	b_ui8 bTi = IEC104_C_SE_NA;
	b_ui8 bCmd=0,btBuf[5],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Nva,2);
	memcpy(btBuf+2,&bQos,1);
	bLen = 3;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
 int CIec104::AppTxAsduType_49(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_i16 Sva,b_ui8  bQos)
{
	b_ui8 bTi = IEC104_C_SE_NB;
	b_ui8 bCmd=0,btBuf[5],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Sva,2);
	memcpy(btBuf+2,&bQos,1);
	bLen = 3;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
  int CIec104::AppTxAsduType_50(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, float Sfa,b_ui8  bQos)
{
	b_ui8 bTi = IEC104_C_SE_NC;
	b_ui8 bCmd=0,btBuf[5],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Sfa,4);
	memcpy(btBuf+4,&bQos,1);
	bLen = 5;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
  int CIec104::AppTxAsduType_61(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui16 Nva,b_ui8  bQos,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_SE_TA;
	b_ui8 bCmd=0,btBuf[12],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Nva,2);
	memcpy(btBuf+2,&bQos,1);
	memcpy(btBuf+3,bTimeTag,7);
	bLen = 10;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
 int CIec104::AppTxAsduType_62(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_i16 Sva,b_ui8  bQos,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_SE_TB;
	b_ui8 bCmd=0,btBuf[12],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Sva,2);
	memcpy(btBuf+2,&bQos,1);
	memcpy(btBuf+3,bTimeTag,7);
	bLen = 10;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}
int CIec104::AppTxAsduType_63(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, float Sfa,b_ui8  bQos,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_SE_TC;
	b_ui8 bCmd=0,btBuf[12],bLen=0;
	if(bCot==IEC104_COT_DEACT)
		bCmd = IEC104_CTRL_CANCEL;
	else
	{
		bCmd = (bQos&0x80)>0?IEC104_CTRL_SELECT:IEC104_CTRL_EXECUTE;
	}
	memcpy(btBuf,&Sfa,4);
	memcpy(btBuf+4,&bQos,1);
	memcpy(btBuf+5,bTimeTag,7);
	bLen = 12;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		m_pIec104Data->AppState = IEC104_APP_STATE_IDLE;;
		m_pIec104Data->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
}

int CIec104::AppTxAsduType_51(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui32 bitstring32)
{
	b_ui8 bTi = IEC104_C_BO_NA;
	b_ui8 btBuf[12],bLen=0;
	memcpy(btBuf,&bitstring32,4);
	bLen = 4;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
}
int CIec104::AppTxAsduType_64(b_ui16 rtuAddr,b_ui32 infoAddr ,b_ui8 bCot, b_ui32 bitstring32,b_ui8 *bTimeTag)
{
	b_ui8 bTi = IEC104_C_BO_TA;
	b_ui8 btBuf[12],bLen=0;
	memcpy(btBuf,&bitstring32,4);
	memcpy(btBuf+4,bTimeTag,7);
	bLen = 11;
	CIec104_Asdu  asdu (m_pIec104Data->AppPara,bTi,1,bCot,rtuAddr,infoAddr,btBuf,bLen );
	if(LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
		return 1;
	return -1;
}



int  CIec104::AppRxAsdu_48(const b_ui8 *asdu, b_ui8 size)//nva
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui16 bSrcNVA=0,bDstNVA=0;
	_NVA nva;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcNVA,&asdu[infoAddrLen+infoIndex],2);
	bDstNVA = p104tohost16(bSrcNVA);
	memcpy(&nva,&bDstNVA,2);
	GetHandler()->handleSetPointNVA(iRtuAddr,infoAddr, bCot, nva,asdu[infoAddrLen+infoIndex+2]);
	return 0;
}
int  CIec104::AppRxAsdu_49(const b_ui8 *asdu, b_ui8 size)//sva
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui16 bSrcSVA=0,bDstSVA=0;
	_SVA sva;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcSVA,&asdu[infoAddr+infoIndex],2);
	bDstSVA = p104tohost16(bSrcSVA);
	memcpy(&sva,&bDstSVA,2);
	GetHandler()->handleSetPointSVA(iRtuAddr,infoAddr, bCot, sva,asdu[infoAddr+infoIndex+2]);
	return 0;
}
int  CIec104::AppRxAsdu_50(const b_ui8 *asdu, b_ui8 size)//sfa
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui32 bSrcSFA=0,bDstSFA=0;
	_SFA sfa;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcSFA,&asdu[infoAddrLen+infoIndex],4);
	bDstSFA = p104tohost32(bSrcSFA);
	memcpy(&sfa,&bDstSFA,4);
	GetHandler()->handleSetPointSFA(iRtuAddr,infoAddr, bCot, sfa,asdu[infoAddrLen+infoIndex+4]);
	return 0;
}
int  CIec104::AppRxAsdu_51(const b_ui8 *asdu, b_ui8 size)
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui32 bSrcBits32=0,bits32=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcBits32,&asdu[infoAddrLen+infoIndex],4);
	bits32 = p104tohost32(bSrcBits32);
	GetHandler()->handleBitstring32Command(iRtuAddr,infoAddr, bCot, bits32);
	return 0;
}
int  CIec104::AppRxAsdu_61(const b_ui8 *asdu, b_ui8 size)//nva
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui16 bSrcNVA=0,bDstNVA=0;
	_NVA nva;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcNVA,&asdu[infoAddrLen+infoIndex],2);
	bDstNVA = p104tohost16(bSrcNVA);
	memcpy(&nva,&bDstNVA,2);
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+3], &timeTag);
	GetHandler()->handleSetPointNVAWithTimeTag(iRtuAddr,infoAddr, bCot, nva,asdu[infoAddrLen+infoIndex+2],timeTag);
	return 0;
}
int  CIec104::AppRxAsdu_62(const b_ui8 *asdu, b_ui8 size)//sva
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui16 bSrcSVA=0,bDstSVA=0;
	_SVA sva;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcSVA,&asdu[infoAddrLen+infoIndex],2);
	bDstSVA = p104tohost16(bSrcSVA);
	memcpy(&sva,&bDstSVA,2);
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+3], &timeTag);
	GetHandler()->handleSetPointSVAWithTimeTag(iRtuAddr,infoAddr, bCot, sva,asdu[infoAddrLen+infoIndex+2],timeTag);
	return 0;
}
int  CIec104::AppRxAsdu_63(const b_ui8 *asdu, b_ui8 size)//sfa
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui32 bSrcSFA=0,bDstSFA=0;
	_SFA sfa;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcSFA,&asdu[infoAddrLen+infoIndex],4);
	bDstSFA = p104tohost32(bSrcSFA);
	memcpy(&sfa,&bDstSFA,4);
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+5], &timeTag);
	GetHandler()->handleSetPointSFAWithTimeTag(iRtuAddr,infoAddr, bCot, sfa,asdu[infoAddrLen+infoIndex+4],timeTag);
	return 0;
}

int  CIec104::AppRxAsdu_64(const b_ui8 *asdu, b_ui8 size)//bitsring32
{
	int i=0,infoAddr=0,infoIndex,infoAddrLen,iRtuAddr;
	b_ui8 bCot=0;
	b_ui32 bSrcBits32=0,bits32=0;
	SIec104Data *p104=m_pIec104Data;
	infoIndex= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	infoAddrLen = p104->AppPara.InfAddrLen;
	iRtuAddr = getRtuAddr(asdu);
	for (i=infoAddrLen; i>0 ; i--)
	{
		infoAddr = infoAddr*256 + asdu[infoIndex+i-1];
	}
	bCot = asdu[2]&0x3f;
	memcpy(&bSrcBits32,&asdu[infoAddrLen+infoIndex],4);
	bits32 = p104tohost32(bSrcBits32);
	SFepTime timeTag;
	GetTmStmp(&asdu[infoIndex+ infoAddrLen+4], &timeTag);
	GetHandler()->handleBitstring32CommandWithTimeTag(iRtuAddr,infoAddr, bCot,bits32,timeTag);
	return 0;
}


int CIec104::AppRxAsdu_7(const b_ui8 *asdu, b_ui8 size)
{ 	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;	
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bQds=0;
	b_ui8	flag=0;
	b_ui32    src=0;
	SIec104Data *p104 = m_pIec104Data;
	vector<SBitstring32> vBitstring32;
	SBitstring32  bts32;
	iNum = asdu[1]&0x7f; 
   	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	if(btSeqflag)
	{	
		PointNo = 0;
		for (i=iInfAddrLen; i>0; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}
		for(i=0; i<iNum; i++,PointNo++)
		{
			tmpIndex = iInf + iInfAddrLen +(5)*i;
			memcpy(&src,asdu+tmpIndex,4);
			bts32.BSI =  p104tohost32(src);
			memcpy(&bQds,asdu+tmpIndex+4,1);
			bts32.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			bts32.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			bts32.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			bts32.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			bts32.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			bts32.iOffset = PointNo;
			vBitstring32.push_back(bts32);
			infIndex ++;
		}
	}
	else
	{
		tmpIndex = 0;infIndex=0;
		for( i=0; i<iNum; i++ )
		{
			PointNo = 0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+5)*i-1];
			}
			tmpIndex = iInf + iInfAddrLen +(iInfAddrLen+5)*i;
			
			memcpy(&src,asdu+tmpIndex,4);
			bts32.BSI =  p104tohost32(src);
			memcpy(&bQds,asdu+tmpIndex+4,1);
			bts32.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			bts32.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			bts32.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			bts32.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			bts32.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			bts32.iOffset = PointNo;
			vBitstring32.push_back(bts32);
			infIndex ++;
		}
	}
	if(infIndex>0)
	{
		GetHandler()->handleBitstring32(rtuAddr,  btCot, vBitstring32);
	}
	return 0;
}
int CIec104::AppRxAsdu_33(const b_ui8 *asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;	
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bQds=0;
	b_ui8	flag=0;
	b_ui32    src=0,dst=0;
	SIec104Data *p104 = m_pIec104Data;
	vector<SBitstring32TimeTag> vBitstring32Tm;
	SBitstring32TimeTag bts32Tm;
	iNum = asdu[1]&0x7f;
   	 btSeqflag = asdu[1]&0x80 ;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;

	if(btSeqflag)
		return -1;
	tmpIndex = 0;infIndex=0;
	SFepTime timeStamp;
	for ( i=0; i<iNum; i++ )
	{
		PointNo = 0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo = PointNo*256 + asdu[iInf+j+(iInfAddrLen+12)*i-1];
		}
		tmpIndex = iInf + iInfAddrLen + (iInfAddrLen+12)*i;
		GetTmStmp(&asdu[tmpIndex+5], &timeStamp);
		memcpy(&src,asdu+tmpIndex,4);
		bts32Tm.BSI =  p104tohost32(src);
		memcpy(&bQds,asdu+tmpIndex+4,1);
		bts32Tm.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
		bts32Tm.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
		bts32Tm.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
		bts32Tm.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
		bts32Tm.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
		bts32Tm.iOffset = PointNo;
		bts32Tm.cp56TimeTag = timeStamp;
		vBitstring32Tm.push_back(bts32Tm);		
		infIndex ++;
	}
	if(infIndex>0)
	{
		GetHandler()->handleBitstring32WithTimeTag(rtuAddr,  btCot, vBitstring32Tm);
	}
	return 0;
}




int CIec104::AppRxAsdu_20(const b_ui8 * asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;	
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bQds=0;
	b_ui16 bSrc,bDst;
	SIec104Data *p104= m_pIec104Data;;	
	vector<SPackedSinglePoint> vPackedSinglePoint;
	SPackedSinglePoint psp;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);
	if(btSeqflag)
	{
		PointNo=0;
		for (i=iInfAddrLen; i>0 ; i--)
		{
			PointNo = PointNo*256 + asdu[iInf+i-1];
		}		
		for( i=0; i<iNum; i++, PointNo++ )
		{
			tmpIndex = iInf + iInfAddrLen +5*i;
			memcpy(&bSrc,&asdu[tmpIndex],2);
			bDst = p104tohost16(bSrc);
			memcpy(&psp.SCD.ST,&bDst,2);
			memcpy(&bSrc,&asdu[tmpIndex+2],2);
			bDst = p104tohost16(bSrc);
			memcpy(&psp.SCD.CD,&bDst,2);
			memcpy(&bQds,&asdu[tmpIndex+4],1);
			psp.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			psp.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			psp.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			psp.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			psp.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			psp.iOffset = PointNo;
			vPackedSinglePoint.push_back(psp);
			infIndex++;
		}
	}
	else
	{
		for( i=0; i<iNum; i++ )
		{
			PointNo=0;
			for (int j=iInfAddrLen; j>0 ; j--)
			{
				PointNo =PointNo*256 + asdu[iInf+j+(iInfAddrLen+5)*i-1];
			}
			tmpIndex = iInf + iInfAddrLen+(iInfAddrLen +5)*i;
			memcpy(&bSrc,&asdu[tmpIndex],2);
			bDst = p104tohost16(bSrc);
			memcpy(&psp.SCD.ST,&bDst,2);
			memcpy(&bSrc,&asdu[tmpIndex+2],2);
			bDst = p104tohost16(bSrc);
			memcpy(&psp.SCD.CD,&bDst,2);
			memcpy(&bQds,&asdu[tmpIndex+4],1);
			psp.QDS.OV = (bQds>>OV_OFFSET)&BIT_SIZE1;
			psp.QDS.BL= (bQds>>BL_OFFSET)&BIT_SIZE1;
			psp.QDS.SB= (bQds>>SB_OFFSET)&BIT_SIZE1;
			psp.QDS.NT= (bQds>>NT_OFFSET)&BIT_SIZE1;
			psp.QDS.IV = (bQds>>IV_OFFSET)&BIT_SIZE1;
			psp.iOffset = PointNo;
			psp.iOffset = PointNo;
			vPackedSinglePoint.push_back(psp);
			infIndex ++;	
		}
	}
	if(infIndex>0) 
	{
		GetHandler()->handlePackedSinglePoint(rtuAddr,btCot,vPackedSinglePoint);
	}
	return 0;
}
int CIec104::AppRxAsdu_38(const b_ui8 * asdu, b_ui8 size)
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bSep;
	b_ui16  bSrc,bDst;
	SIec104Data *p104 = m_pIec104Data;
	SFepTime timeStamp;
	vector<SEventProtectionTimeTag> vEventProtectionTimeTag;
	SEventProtectionTimeTag epTm;

	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);
	tmpIndex=0;
	if(btSeqflag)
		return -1;
	for( i=0; i<iNum; i++ )
	{
		PointNo=0;
		for (int j=iInfAddrLen; j>0 ; j--)
		{
			PointNo =PointNo*256 + asdu[iInf+j+(iInfAddrLen+10)*i-1];
		}
		tmpIndex = iInf + iInfAddrLen+(iInfAddrLen +10)*i;
		GetTmStmp( &asdu[tmpIndex+3], &timeStamp);
		memcpy(&bSep,&asdu[tmpIndex],1);
		epTm.SEP.EventState= (bSep)&BIT_SIZE2;
		epTm.SEP.EI= (bSep>>EI_OFFSET)&BIT_SIZE1;
		epTm.SEP.BL = (bSep>>BL_OFFSET)&BIT_SIZE1;
		epTm.SEP.SB = (bSep>>SB_OFFSET)&BIT_SIZE1;
		epTm.SEP.NT = (bSep>>NT_OFFSET)&BIT_SIZE1;
		epTm.SEP.IV = (bSep>>IV_OFFSET)&BIT_SIZE1;
		memcpy(&bSrc,&asdu[tmpIndex+1],2);
		bDst = p104tohost16(bSrc);
		memcpy(&epTm.cp16TimeTag,&bDst,2);
		epTm.cp56TimeTag = timeStamp;
		epTm.iOffset = PointNo;
		vEventProtectionTimeTag.push_back(epTm);
		infIndex ++;
	}
	if(infIndex>0) 
	{
		GetHandler()->handleEventProtectionWithTimeTag(rtuAddr, btCot,  vEventProtectionTimeTag);
	}
	return 0;
}
int CIec104::AppRxAsdu_39(const b_ui8 * asdu, b_ui8 size)
{
	int 	iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bSpe=0,bQdp=0;
	b_ui16    bSrc,bDst;
	SIec104Data *p104 = m_pIec104Data;
	SFepTime timeStamp;
	vector<SStartEventsTimeTag> vStartEventsTimeTag;
	SStartEventsTimeTag seTm;

	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);
	if(iNum!=1)
		return -1;
	tmpIndex=0;
	if(iNum!=1)
		return -1;
	tmpIndex=0;
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}
	tmpIndex=iInf+iInfAddrLen;
	GetTmStmp( &asdu[tmpIndex+4], &timeStamp);
	seTm.iOffset = PointNo;
	seTm.cp56TimeTag = timeStamp;
	memcpy(&bSpe,&asdu[tmpIndex],1);
	memcpy(&bQdp,&asdu[tmpIndex+1],1);
	seTm.SPE.GeneralStart = bSpe&0x01;
	seTm.SPE.SL1 = (bSpe>>SL1_OFFSET)&BIT_SIZE1;
	seTm.SPE.SL2 = (bSpe>>SL2_OFFSET)&BIT_SIZE1;
	seTm.SPE.SL3 = (bSpe>>SL3_OFFSET)&BIT_SIZE1;
	seTm.SPE.SIE= (bSpe>>SIE_OFFSET)&BIT_SIZE1;
	seTm.SPE.SRD= (bSpe>>SRD_OFFSET)&BIT_SIZE1;
	seTm.QDP.EI = (bQdp>>EI_OFFSET)&BIT_SIZE1;
	seTm.QDP.BL = (bQdp>>BL_OFFSET)&BIT_SIZE1;
	seTm.QDP.SB= (bQdp>>SB_OFFSET)&BIT_SIZE1;
	seTm.QDP.NT = (bQdp>>NT_OFFSET)&BIT_SIZE1;
	seTm.QDP.IV = (bQdp>>IV_OFFSET)&BIT_SIZE1;
	memcpy(&bSrc,&asdu[tmpIndex+2],2);
	bDst = p104tohost16(bSrc);
	memcpy(&seTm.cp16TimeTag,&bDst,2);
	vStartEventsTimeTag.push_back(seTm);
	infIndex ++;
	if(infIndex>0) 
	{
		GetHandler()->handleStartEventsWithTimeTag(rtuAddr, btCot, vStartEventsTimeTag);
	}
	return 0;
}
int CIec104::AppRxAsdu_40(const b_ui8 * asdu, b_ui8 size)
{
	int 	iNum=0,PointNo=0,tmpIndex=0,infIndex=0,rtuAddr=0;
	int		iInf=0,iInfAddrLen=0;
	b_ui8	btSeqflag=0, btCot=0,bOci=0,bQdp=0;
	b_ui16    bSrc,bDst;
	SIec104Data *p104 = m_pIec104Data;
	SFepTime timeStamp;
	vector<SCircuitInformationTimeTag> vCircuitInformationTimeTag;
	SCircuitInformationTimeTag ciTm;

	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	rtuAddr = getRtuAddr(asdu);
	if(iNum!=1)
		return -1;
	tmpIndex=0;
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}
	tmpIndex=iInf+iInfAddrLen;
	GetTmStmp( &asdu[tmpIndex+4], &timeStamp);
	memcpy(&bOci,&asdu[tmpIndex],1);
	memcpy(&bQdp,&asdu[tmpIndex+1],1);
	ciTm.OCI.GC=(bOci>>GC_OFFSET)&BIT_SIZE1;
	ciTm.OCI.CL1=(bOci>>CL1_OFFSET)&BIT_SIZE1;
	ciTm.OCI.CL2=(bOci>>CL2_OFFSET)&BIT_SIZE1;
	ciTm.OCI.CL3=(bOci>>CL3_OFFSET)&BIT_SIZE1;
	
	ciTm.QDP.EI = (bQdp>>EI_OFFSET)&BIT_SIZE1;
	ciTm.QDP.BL = (bQdp>>BL_OFFSET)&BIT_SIZE1;
	ciTm.QDP.SB= (bQdp>>SB_OFFSET)&BIT_SIZE1;
	ciTm.QDP.NT = (bQdp>>NT_OFFSET)&BIT_SIZE1;
	ciTm.QDP.IV = (bQdp>>IV_OFFSET)&BIT_SIZE1;
	memcpy(&bSrc,&asdu[tmpIndex+2],2);
	bDst = p104tohost16(bSrc);
	memcpy(&ciTm.cp16TimeTag,&bDst,2);
	ciTm.cp56TimeTag = timeStamp;
	ciTm.iOffset = PointNo;
	vCircuitInformationTimeTag.push_back(ciTm);
	infIndex ++;
	if(infIndex>0) 
	{
		GetHandler()->handleCircuitInformationWithTimeTag(rtuAddr, btCot,vCircuitInformationTimeTag);
	}
	return 0;
}

void CIec104::adjustSeq(b_ui8 *buf,int len)
{
	int i=0,j=len-1;
	b_ui8 tmp=0;

		while(i<j)
	{
		tmp = buf[i];
		buf[i]= buf[j];
		buf[j] = tmp;
		i++;
		j--;
	}		
}
void CIec104::printDebug(char* buf)
{
	char tmp[1024];
	sprintf(tmp,"%s",buf);
	std::string logStr=tmp;
	this->m_pChannel->logChannelMsg(LOG_LEVEL_DEBUG,logStr);
}

int CIec104::AppTxCallFile(int rtuAddr,int infoAddr, b_ui8 cmdType,int fileName,b_ui8 secName)//122
{
	SIec104Data *p104;
	p104 = m_pIec104Data;

	b_ui8 bTi=0,bCot=0,bScq=0,bLen=0,bVsq=1;
	b_ui8 btBuf[4],bCmd=0;

	bTi = IEC104_F_SC_NA;
	switch(cmdType)
	{
		case FILE_CMD_REQ_DIR:
			bCmd =  FILE_CMD_REQ_DIR;
			bCot = IEC104_COT_REQ;
			bScq = 0;//uncertain value
			break;
		case FILE_CMD_SELECT_FILE:
			bCmd =  FILE_CMD_SELECT_FILE;
			bCot = IEC104_COT_FILE_TRANS;
			bScq = 0X01;//request file
			break;
		case FILE_CMD_REQ_FILE:
			bCmd = FILE_CMD_REQ_FILE;
			bCot = IEC104_COT_FILE_TRANS;
			bScq = 0X02;//select file
			break;
		case FILE_CMD_REQ_SECTION:
			bCmd = FILE_CMD_REQ_SECTION;
			bCot = IEC104_COT_FILE_TRANS;
			bScq = 0X06;// request section
			break;
		case FILE_CMD_SELECT_SECTION:
			bCmd = FILE_CMD_SELECT_SECTION;
			bCot = IEC104_COT_FILE_TRANS;
			bScq = 0X05;//select section
			break;
		default:
			return -1;
	}

	memcpy(btBuf,&fileName,2);
	memcpy(btBuf+2,&secName,1);
	memcpy(btBuf+3,&bScq,1);
	bLen = 4;
	CIec104_Asdu  asdu (p104->AppPara,bTi,bVsq,bCot,rtuAddr,infoAddr,btBuf,bLen);

	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->FileTransfer.cmd_state= bCmd;
		p104->FileTransfer.file_name = fileName;
		p104->FileTransfer.section_name = secName;
		//p104->FileTransfer.section_name = 0;
		p104->FileTransfer.finish = 0;
		return 1;
	}
	return -1;
}

void CIec104::AppRxDir(const b_ui8 *asdu,b_ui8 size)//126
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,iRtuAddrIndex=0,iRtuAddrLen=0,iRtuAddr=0;
	b_ui8	btSeqflag=0, btCot=0;
	b_ui8 fileState=0;b_ui16 fileName=0;b_ui32 fileLen=0;


	SIec104Data *p104;
	SFileInfo FileInfo[24]; 
	SFepTime timeStamp;

	p104 = m_pIec104Data;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	//if(!btSeqflag)
	//	return;
	if((btCot!=IEC104_COT_SPONT)&& (btCot!=IEC104_COT_REQ))
		return;

	if(p104->FileTransfer.cmd_state!=FILE_CMD_REQ_DIR)
	{
		printDebug("receive Ti 126,upload dir ,without request");
		return;
	}
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for(i=iRtuAddrLen;i>0;i--)//get ASDU address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}   
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}
//	PointNo -=p104->PointBeginAddr-1;
	for (i=0; i<iNum; i++)
	{
		tmpIndex = iInf + iInfAddrLen + 12*i;
		fileName= asdu[tmpIndex]+asdu[tmpIndex+1]*256;
		fileLen    = asdu[tmpIndex+2]+asdu[tmpIndex+3]*256+asdu[tmpIndex+4]*256*256;
		fileState  =asdu[tmpIndex+5];
		GetTmStmp(&asdu[tmpIndex+6], &timeStamp);
		
		FileInfo[infIndex].fileName= fileName;
		FileInfo[infIndex].fileLen = fileLen;
		FileInfo[infIndex].fileState = fileState;
		FileInfo[infIndex].timeStamp = timeStamp;
		infIndex++;
	}
	if(infIndex>0)
	{
		GetHandler()->handle_File_Dir(iRtuAddr, PointNo, FileInfo,infIndex);
	}
	
}
void CIec104::AppRxFileReady(const b_ui8 *asdu,b_ui8 size)//120
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,iRtuAddrIndex =0,iRtuAddrLen=0,iRtuAddr=0;
	b_ui8	btSeqflag=0, btCot=0;
	b_ui8 bFrq=0;b_ui16 fileName=0;b_ui32 fileLen=0;
	SIec104Data *p104;
	
	p104 = m_pIec104Data;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	if(btSeqflag)
		return;
	if(btCot!=IEC104_COT_FILE_TRANS)
		return;

	if(p104->FileTransfer.cmd_state!=FILE_CMD_SELECT_FILE)
	{
		printDebug("receive Ti 120,file ready ,without request");
		return ;
	}
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for(i=iRtuAddrLen;i>0;i--)//get ASDU address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}   
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}

	tmpIndex = iInf + iInfAddrLen;
	fileName = asdu[tmpIndex]+asdu[tmpIndex+1]*256;
	fileLen     = asdu[tmpIndex+2]+asdu[tmpIndex+3]*256+asdu[tmpIndex+4]*256*256;
	bFrq       = asdu[tmpIndex+5];
	
	GetHandler()->handle_File_Ready(iRtuAddr, PointNo, fileName);

	
}
void CIec104::AppRxSectionReady(const b_ui8 *asdu,b_ui8 size)//121
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,iRtuAddrIndex=0,iRtuAddrLen=0,iRtuAddr =0;
	b_ui8	btSeqflag=0, btCot=0;
	b_ui8 bSrq=0,bSectionName;b_ui16 fileName=0;b_ui32 sectionLen=0;
	SIec104Data *p104;
	

	p104 = m_pIec104Data;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	if(btSeqflag)
		return;
	if(btCot!=IEC104_COT_FILE_TRANS)
		return;
	if((p104->FileTransfer.cmd_state!=FILE_CMD_REQ_FILE) &&(p104->FileTransfer.cmd_state!=FILE_CMD_SELECT_SECTION)
		&&(p104->FileTransfer.cmd_state!=FILE_CMD_REQ_SECTION))
	{
		printDebug("receive Ti 121,section ready,withou request ");
		return;
	}


	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for(i=iRtuAddrLen;i>0;i--)//get ASDU  address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}   
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}

	tmpIndex = iInf + iInfAddrLen;
	fileName = asdu[tmpIndex]+asdu[tmpIndex+1]*256;
	bSectionName =asdu[tmpIndex+2];
	sectionLen     = asdu[tmpIndex+3]+asdu[tmpIndex+4]*256+asdu[tmpIndex+5]*256*256;
	bSrq       = asdu[tmpIndex+5];

	if((p104->FileTransfer.section_name!=0) && (p104->FileTransfer.section_name!= bSectionName))
	{
		printDebug("receive Ti 121,section ready,but section name is not correct");
		return;
	}

	if(p104->FileTransfer.cmd_state==FILE_CMD_REQ_FILE)
	{
		AppTxCallFile(1,0, FILE_CMD_SELECT_SECTION,fileName,bSectionName);
		GetHandler()->handle_Section_Ready(iRtuAddr, PointNo, fileName, bSectionName);
	}
	else if(p104->FileTransfer.cmd_state==FILE_CMD_REQ_SECTION||p104->FileTransfer.cmd_state==FILE_CMD_SELECT_SECTION)
	{	
		GetHandler()->handle_Section_Ready(iRtuAddr, PointNo, fileName, bSectionName);
		AppTxCallFile(1,0, FILE_CMD_REQ_SECTION,fileName,bSectionName);
	}
}

void  CIec104::AppRxSegment(const b_ui8 *asdu,b_ui8 size)//125
{
	int 	i,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,iRtuAddrIndex=0,iRtuAddrLen=0,iRtuAddr=0;
	b_ui8	btSeqflag=0, btCot=0,btBuf[240];
	b_ui8 bSegLen=0,bSectionName;b_ui16 fileName=0;
	SIec104Data *p104;
	

	p104 = m_pIec104Data;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	

	if(btCot!=IEC104_COT_FILE_TRANS)
		return;
	if(btSeqflag)
		return;
	if(p104->FileTransfer.cmd_state!=FILE_CMD_REQ_SECTION)
	{
		printDebug("receive Ti 125, upload segment,without request");
		return;
	}
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;
	for(i=iRtuAddrLen;i>0;i--)//get ASDU address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}   
	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}

	tmpIndex = iInf + iInfAddrLen;
	fileName = asdu[tmpIndex]+asdu[tmpIndex+1]*256;
	bSectionName =asdu[tmpIndex+2];
	bSegLen    = asdu[tmpIndex+3];
	memcpy(btBuf,&asdu[tmpIndex+4],bSegLen);
	if((p104->FileTransfer.file_name!= fileName) || (p104->FileTransfer.section_name!=bSectionName))
	{
		printDebug("receive Ti 125, upload segment,filename or section name is not correct");
		return;
	}
	GetHandler()->handle_Rx_Segment(iRtuAddr,PointNo,fileName, bSectionName, btBuf, bSegLen);
}

void  CIec104::AppRxLastSe(const b_ui8 *asdu,b_ui8 size)//123
{
	int 	i=0,iNum=0,PointNo=0,tmpIndex=0,infIndex=0;
	int		iInf=0,iInfAddrLen=0,iRtuAddrIndex=0,iRtuAddrLen=0,iRtuAddr=0;
	b_ui8	btSeqflag=0, btCot=0;
	b_ui8 bSegLen=0,bLsq=0,bChs=0,bSectionName=0;
	b_ui16 fileName=0;
	SIec104Data *p104;
	
	p104 = m_pIec104Data;
	iNum = asdu[1]&0x7f;
	btSeqflag = asdu[1]&0x80;
	btCot = asdu[2]&0x3f;

	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;

	for(i=iRtuAddrLen;i>0;i--)//get ASDU address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}   

	iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;

	if(btCot!=IEC104_COT_FILE_TRANS)
		return;
	if(btSeqflag)
		return;
	if(p104->FileTransfer.cmd_state!=FILE_CMD_REQ_SECTION)
	{
		printDebug("receive Ti 123, upload last se*,without request");
		return;
	}

	for (int j=iInfAddrLen; j>0 ; j--)
	{
		PointNo =PointNo*256 + asdu[iInf+j-1];
	}

	tmpIndex = iInf + iInfAddrLen;
	fileName = asdu[tmpIndex]+asdu[tmpIndex+1]*256;
	bSectionName =asdu[tmpIndex+2];
	bLsq = asdu[tmpIndex+3];
	bChs = asdu[tmpIndex+4];
	if(bLsq==3)//last segment
	{
		GetHandler()->handle_Last_Segment(iRtuAddr, PointNo, fileName,bSectionName);
	}
	else if(bLsq ==1) //last section
	{
		GetHandler()->handle_Last_Section(iRtuAddr,PointNo,fileName,bSectionName);
		p104->FileTransfer.finish=1;
	}
}
int  CIec104::AppTxAffirmFile(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag)//124
{
	SIec104Data *p104;
	p104 = m_pIec104Data;

	b_ui8 bTi=0,bCot=0,bScq=0,bLen=0,bVsq=1;
	b_ui8 btBuf[10],bAfq=0;

	bTi = IEC104_F_AF_NA;	
	bCot = IEC104_COT_FILE_TRANS;
	bAfq = affirmFlag;
	memcpy(btBuf,&fileName,2);
	memcpy(btBuf+2,&secName,1);
	memcpy(btBuf+3,&bAfq,1);
	bLen = 4;
	CIec104_Asdu  asdu (p104->AppPara,bTi,bVsq,bCot,rtuAddr,infoAddr,btBuf,bLen);

	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		if(affirmFlag==1||affirmFlag==2)
		{
			p104->FileTransfer.cmd_state= FILE_CMD_NONE;
			p104->FileTransfer.file_name = 0;
			p104->FileTransfer.section_name = 0;
			p104->FileTransfer.finish = 1;
			return 1;
		}
		else if(affirmFlag==3||affirmFlag==4)
		{
			p104->FileTransfer.section_name ++;
		}
		return 1;
	}
	
	return -1;
}

int  CIec104::AppTxExCallValue(int comnAddr,b_ui8 prtectSerialNo,b_ui8 ngd,b_ui8 group,b_ui8 areaFlag)//145
{
	SIec104Data *p104;
	p104 = m_pIec104Data;

	b_ui8 bTi=0,bCot=0,bVsq = 1,bvalue=0,bLen=0,bRii=0,bNgd=0;
	b_ui8 bEntry=0,bGroup=0,bKod;
	b_ui8 btBuf[8],tmpInfAddr[3],bCmd=0;
	int iInfAddr=0;

	tmpInfAddr[0] = prtectSerialNo;
	tmpInfAddr[1] = 0;
	tmpInfAddr[2] = 241;
	iInfAddr =  tmpInfAddr[2]*256*256+tmpInfAddr[1]*256+tmpInfAddr[0];

	bTi = IEC104_E_CD_NA;//145
	bCot = IEC104_COT_REQ;//5
	bCmd = IEC104_CMD_EX_CALLVALUE;
	bRii=0;
	bNgd = ngd;
	bKod = 0;

	if(areaFlag)
	{
		bGroup = 0;
		bEntry  =1;
	}
	else
	{
		bGroup = group;
		bEntry  = 0;
	}
	memcpy(btBuf,&bRii,1);
	memcpy(btBuf+1,&bNgd,1);
	memcpy(btBuf+2,&bGroup,1);
	memcpy(btBuf+3,&bEntry,1);
	memcpy(btBuf+4,&bKod,1);
	bLen = 5;

	CIec104_Asdu  asdu (p104->AppPara,bTi,bVsq,bCot,comnAddr,iInfAddr,btBuf,bLen);

	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_WAITCALLVALUECONF;
		p104->CtrlCmd = bCmd;
		return 1;
	}
	
	return -1;
	
}

/*
*cmdType ={0 selecion ,1 execution,3 undoing}
*/
int  CIec104::AppTxExModifyValueAreaNo(int comnAddr,b_ui8 prtecSerialNo,b_ui8 areaNo,b_ui8 bSeType)//147
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	b_ui8 bTi=0,bCot=0,bVsq = 1,bLen=0,bRii=0,bNgd=0;
	b_ui8 bEntry=0,bGroup=0,bKod=0,bDataType=0,bDataSize=0,bNumber=0,bGid=0;
	b_ui8 btBuf[10],tmpInfAddr[3],bCmd=0;
	int   iInfAddr;

	bTi =IEC104_E_MD_NA;//147
	bCot = IEC104_COT_REQ;//5
	
	switch(bSeType)
	{
		case 0://selection
			tmpInfAddr[2]= 249;
			bCmd =  IEC104_CTRL_SELECT;
			break;
		case 1://execution
			tmpInfAddr[2] =250;
			bCmd =  IEC104_CTRL_EXECUTE;
			break;
		default://undoing
			tmpInfAddr[2] = 251;
			bCmd =  IEC104_CTRL_CANCEL;
			break;
	}
	tmpInfAddr[0] = prtecSerialNo;
	tmpInfAddr[1] = 0;
	iInfAddr =  tmpInfAddr[2]*256*256+tmpInfAddr[1]*256+tmpInfAddr[0];

	bCmd = IEC104_CMD_EX_MODIFYVALUE;
	bRii=0;
	bNgd =1;// 1:0:0(6:1:1)
	bGroup = 0;
	bEntry  = 1;
	bKod = 0;
	bDataType = 3;
	bDataSize  = 1;
	bNumber = 1;
	
	
	bNumber   = 1;
	bGid = areaNo+1;
	memcpy(btBuf,&bRii,1);
	memcpy(btBuf+1,&bNgd,1);
	memcpy(btBuf+2,&bGroup,1);
	memcpy(btBuf+3,&bEntry,1);
	memcpy(btBuf+4,&bKod,1);
	memcpy(btBuf+5,&bDataType,1);
	memcpy(btBuf+6,&bDataSize,1);
	memcpy(btBuf+7,&bNumber,1);
	memcpy(btBuf+8,&bGid,1);
	bLen = 9;

	CIec104_Asdu  asdu (p104->AppPara,bTi,bVsq,bCot,comnAddr,iInfAddr,btBuf,bLen);

	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_WAITMIDIFYVALUECONF;
		p104->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
	
}
/*
*cmdType ={0 selecion ,1 execution,3 undoing}
*ngb=Num：Count：Cont=6:1:1,count =0,cont =1 
*/
int  CIec104::AppTxExModifyValue(int comnAddr,b_ui8 prtecSerialNo,b_ui8 areaNo,b_ui8 cmdType,float* fBuf,b_ui8 ngd,b_ui8 beginNo)//147
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	b_ui8 bTi=0,bCot=0,bVsq = 1,bLen=0,bRii=0,bNgd=0;
	b_ui8 bEntry=0,bGroup=0,bKod=0,bDataType=0,bDataSize=0,bNumber=0,bGid=0;
	b_ui8 btBuf[250],tmpInfAddr[3],bCmd=0,bCount=0,bCont=0;
	int   iInfAddr,i;

	bTi =IEC104_E_MD_NA;//147
	bCot = IEC104_COT_REQ;//5
	
	switch(cmdType)
	{
		case 0://selection
			tmpInfAddr[2]= 249;
			bCmd =  IEC104_CTRL_SELECT;
			break;
		case 1://execution
			tmpInfAddr[2] =250;
			bCmd =  IEC104_CTRL_EXECUTE;
			break;
		default://undoing
			tmpInfAddr[2] = 251;
			bCmd =  IEC104_CTRL_CANCEL;
			break;
	}	
	tmpInfAddr[0] = prtecSerialNo;
	tmpInfAddr[1] = 0;
	iInfAddr =  tmpInfAddr[2]*256*256+tmpInfAddr[1]*256+tmpInfAddr[0];

	if(cmdType!=0)//not selection
	{
		bCmd = IEC104_CMD_EX_MODIFYVALUE;
		bRii=0;
		bNgd =1;// 1:0:0(6:1:1)
		bGroup = areaNo+1;
		bEntry  = 0;
		bKod = 0;
		bDataType = 3;
		bDataSize  = 1;
		bNumber = 1;
		bGid = areaNo+1;
	
		bNumber   = 1;
		bGid = areaNo+1;
		memcpy(btBuf,&bRii,1);
		memcpy(btBuf+1,&bNgd,1);
		memcpy(btBuf+2,&bGroup,1);
		memcpy(btBuf+3,&bEntry,1);
		memcpy(btBuf+4,&bKod,1);
		memcpy(btBuf+5,&bDataType,1);
		memcpy(btBuf+6,&bDataSize,1);
		memcpy(btBuf+7,&bNumber,1);
		memcpy(btBuf+8,&bGid,1);
		bLen = 9;
	}
	else //selection
	{
		bCount = ngd & 0x3f;
		if(bCount > 20)
		{
			printDebug("数值太大");
			return -1;
		}
		bRii = 0;
		bNgd = ngd;	
		memcpy(btBuf,&bRii,1);
		memcpy(btBuf+1,&bNgd,1);
		bLen=2;
		for(i=0;i<bCount;i++)
		{
			bGroup = areaNo+1; memcpy(btBuf+bLen,&bGroup,1); bLen++;
			bEntry  = beginNo+i; memcpy(btBuf+bLen,&bEntry,1); bLen++;
			bKod    = 0;                memcpy(btBuf+bLen,&bKod,1);bLen++;
			bDataType = 7;           memcpy(btBuf+bLen,&bDataType,1);bLen++;//float
			bDataSize  = 4;           memcpy(btBuf+bLen,&bDataSize,1);bLen++;
			bNumber = 0X81;	   memcpy(btBuf+bLen,&bNumber,1);bLen++;
			memcpy(btBuf+bLen,(b_ui8*)&fBuf[i],4); bLen+=4;
		}
		btBuf[bLen-4] &=0X7f; 
		
	}
	
	CIec104_Asdu  asdu (p104->AppPara,bTi,bVsq,bCot,comnAddr,iInfAddr,btBuf,bLen);
	if(  LinkTxFrameI(  asdu.m_pBuf , asdu.m_iLen, IEC104_FRMTYPE_SEND_WC ) == 1)
	{
		p104->AppState = IEC104_APP_STATE_WAITMIDIFYVALUECONF;//modifyvalue
		p104->CtrlCmd = bCmd;
		return 1;
	}
	return -1;
	
}
int CIec104::AppRxAsdu_110(const b_ui8 * asdu, b_ui8 size)
{
	b_ui8 btCot,bQpm;
	int PointNo=0;
	b_ui16 bSrc,bDst;
	b_ui16 rtuAddr;
	SIec104Data *p104 = m_pIec104Data;
	SParameterNVA paraNva;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	int iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int iInfAddrLen = p104->AppPara.InfAddrLen;
	if(asdu[1]!=1)
		return -1;
	for (int i=iInfAddrLen; i>0; i--)
	{
		PointNo = PointNo*256 + asdu[iInf+i-1];
	}
	paraNva.iOffset = PointNo;
	memcpy(&bSrc,&asdu[iInf+iInfAddrLen],2);
	bDst = p104tohost16(bSrc);
	memcpy(&paraNva.NVA,&bDst,2);
	memcpy(&bQpm,&asdu[iInf+iInfAddrLen+2],1);
	paraNva.QPM.KPA = bQpm&BIT_SIZE6;
	paraNva.QPM.LPC = (bQpm>>LPC_OFFSET)&BIT_SIZE1;
	paraNva.QPM.POP = (bQpm>>POP_OFFSET)&BIT_SIZE1;
	GetHandler()->handleParameterNVA(rtuAddr,btCot,paraNva);
	return 0;
}
int CIec104::AppRxAsdu_111(const b_ui8 * asdu, b_ui8 size)
{
	b_ui8 btCot,bQpm=0;
	int PointNo=0;
	b_ui16 rtuAddr,bSrc,bDst;
	SIec104Data *p104 = m_pIec104Data;
	SParameterSVA paraSva;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	int iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int iInfAddrLen = p104->AppPara.InfAddrLen;
	if(asdu[1]!=1)
		return -1;
	for (int i=iInfAddrLen; i>0; i--)
	{
		PointNo = PointNo*256 + asdu[iInf+i-1];
	}
	paraSva.iOffset = PointNo;
	memcpy(&bSrc,&asdu[iInf+iInfAddrLen],2);
	bDst = p104tohost16(bSrc);
	memcpy(&paraSva.SVA,&bDst,2);
	memcpy(&bQpm,&asdu[iInf+iInfAddrLen+2],1);
	paraSva.QPM.KPA = bQpm&BIT_SIZE6;
	paraSva.QPM.LPC = (bQpm>>LPC_OFFSET)&BIT_SIZE1;
	paraSva.QPM.POP = (bQpm>>POP_OFFSET)&BIT_SIZE1;
	GetHandler()-> handleParameterSVA(rtuAddr,btCot,paraSva);
	return 0;
}
int CIec104::AppRxAsdu_112(const b_ui8 * asdu, b_ui8 size)
{
	b_ui8 btCot,bQpm;
	int PointNo=0;
	b_ui16 rtuAddr;
	b_ui32 bSrc,bDst;
	SIec104Data *p104 = m_pIec104Data;
	SParameterSFA paraSfa;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	int iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int iInfAddrLen = p104->AppPara.InfAddrLen;
	if(asdu[1]!=1)
		return -1;
	for (int i=iInfAddrLen; i>0; i--)
	{
		PointNo = PointNo*256 + asdu[iInf+i-1];
	}
	paraSfa.iOffset = PointNo;
	memcpy(&bSrc,&asdu[iInf+iInfAddrLen],4);
	bDst = p104tohost32(bSrc);
	memcpy(&paraSfa.SFA,&bDst,4);
	memcpy(&bQpm,&asdu[iInf+iInfAddrLen+4],1);
	paraSfa.QPM.KPA = bQpm&BIT_SIZE6;
	paraSfa.QPM.LPC = (bQpm>>LPC_OFFSET)&BIT_SIZE1;
	paraSfa.QPM.POP = (bQpm>>POP_OFFSET)&BIT_SIZE1;
	GetHandler()->handleParameterSFA(rtuAddr,btCot,paraSfa);
	return 0;
}
int CIec104::AppRxAsdu_113(const b_ui8 * asdu, b_ui8 size)
{
	b_ui8 btCot;
	int PointNo=0;
	b_ui16 rtuAddr;
	SIec104Data *p104 = m_pIec104Data;
	SParameterAct  paraAct;
	btCot = asdu[2]&0x3f;
	rtuAddr = getRtuAddr(asdu);
	int iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int iInfAddrLen = p104->AppPara.InfAddrLen;
	if(asdu[1]!=1)
		return -1;
	for (int i=iInfAddrLen; i>0; i--)
	{
		PointNo = PointNo*256 + asdu[iInf+i-1];
	}
	paraAct.iOffset = PointNo;
	memcpy(&paraAct.QPA,&asdu[iInf+iInfAddrLen],1);
	GetHandler()->handleParameterActivation(rtuAddr,btCot,paraAct);
	return 0;
}
void CIec104::AppRxCallValueConf(const b_ui8 *asdu, b_ui8 size) //146
{
	SIec104Data *p104;
	int		i,iInfBase,iInfAddrLen,count,iInfIndex;
	int		iRtuAddrLen=0,iRtuAddrIndex=0,iRtuAddr=0;
	b_ui8   bCot =0,bInfNo=0,bTotalNo=0,bRii=0,bNgd=0,bGroup=0,bEntry=0,bKod=0;
	b_ui8   bDataType=0,bDataSize=0,bNumber=0,bGid=0;
	float    fVal;
	b_ui8   btBuf[4];
	b_ui8   erroFlag=0;
	struct SExCallValueResult sExCallValueResult;
	p104 = m_pIec104Data;
	
	iInfBase= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;

	
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	
	sExCallValueResult.bCount = 0;
	for(i=iRtuAddrLen;i>0;i--)//get rtu comman address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}

	if(p104->AppState==IEC104_APP_STATE_WAITCALLVALUECONF)
	{
		bCot = asdu[2]&0x7f;
		bTotalNo= asdu[iInfBase];
		bInfNo   = asdu[iInfBase+2];
		bRii        = asdu[iInfBase+3];
		bNgd      = asdu[iInfBase+4];
		bGroup  = asdu[iInfBase+5];
		bEntry   = asdu[iInfBase+6];
		sExCallValueResult.bTotalNo = bTotalNo;
		sExCallValueResult.bCount   = bNgd &0x3f;
		if(bCot==IEC104_COT_REQ)
		{
			p104->AppState = IEC104_APP_STATE_IDLE;			
			sExCallValueResult.bResult = 1;
			if(bGroup==0)//call area number of fix value
			{
				bGid=asdu[iInfBase+11];//area number plus 1
				sExCallValueResult.bCount = 1;
				sExCallValueResult.sData[0].bAreaNo = bGid - 1;
				sExCallValueResult.bResult |= 0x80;
			}
			else//call fix value
			{
				count = bNgd&0x3f; //the number 
				iInfBase+=5;         //offset of the first GIN
				sExCallValueResult.bCount = count;
				for(i=0;i<count;i++)
				{
					iInfIndex = iInfBase+10*i;
					bGroup = asdu[iInfIndex];
					bEntry  =asdu[iInfIndex+1];
					bKod   = asdu[iInfIndex+2];
					bDataType= asdu[iInfIndex+3];
					bDataSize= asdu[iInfIndex+4];
					bNumber = asdu[iInfIndex+5];
					memcpy(btBuf,&asdu[iInfIndex+6],4);
					fVal = *(float*)&btBuf[0];

					sExCallValueResult.sData[i].bAreaNo = bGroup - 1;
					sExCallValueResult.sData[i].bValueNo = bEntry;
					sExCallValueResult.sData[i].fVal   =  fVal;
					
				}
			}
		}
		else if((bCot== IEC104_COT_UNKNOWN) ||(bCot ==0x45))
		{
			sExCallValueResult.bResult = 0;
			p104->AppState = IEC104_APP_STATE_IDLE;
		}
		else
		{	
			erroFlag=1;
			printDebug("解析146，未知的传送原因");
			return ;
		}
	}
	else//nothing to do
	{
		erroFlag=1;
		printDebug("解析146，没有请求");
		return ;
	}

	if(!erroFlag)
 		GetHandler()->handle_Call_Value(iRtuAddr , sExCallValueResult);
	
}

void   CIec104::AppRxModifyValueConf(const b_ui8 *asdu, b_ui8 size) //148
{
	SIec104Data *p104;
	int		i,iInfBase,iInfAddrLen;
	int		iRtuAddrLen=0,iRtuAddrIndex=0,iRtuAddr=0;
	b_ui8   bCot =0,bInfNo=0,bTotalNo=0,bRii=0,bNgd=0,bGroup=0,bEntry=0,bKod=0;
	b_ui8   bDataType=0,bDataSize=0,bNumber=0,bGid=0;

	struct SExResult SResult;
	int  erroFlag=0;
	
	p104 = m_pIec104Data;
	iInfBase= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;

	
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;

	for(i=iRtuAddrLen;i>0;i--)//get RTU common address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}
	SResult.iRtu=iRtuAddr;
	bCot = asdu[2]&0x7f;
	bTotalNo= asdu[iInfBase];
	bInfNo   = asdu[iInfBase+2];
	bRii        = asdu[iInfBase+3];
	bNgd      = asdu[iInfBase+4];
	bGroup  = asdu[iInfBase+5];
	bEntry   = asdu[iInfBase+6];
	bKod     = asdu[iInfBase+7];
	bDataType = asdu[iInfBase+8];
	bDataSize = asdu[iInfBase+9];
	bNumber  = asdu[iInfBase+10];
	bGid         = asdu[iInfBase+11];
	SResult.bAreaNo= bGid - 1;

	do
	{	
		if(p104->AppState!=IEC104_APP_STATE_WAITMIDIFYVALUECONF)
		{
			printDebug("收到报文148，没有请求");
			erroFlag=1;
			break;
		}
		if(p104->CtrlCmd==IEC104_CTRL_SELECT && bInfNo!=249)
		{
			printDebug("收到报文148，预置确认，没有请求");
			erroFlag=1;
			break;
		}
		if(p104->CtrlCmd==IEC104_CTRL_EXECUTE&& bInfNo!=250)
		{
			printDebug("收到报文148，执行确认，没有请求");
			erroFlag=1;
			break;
		}
		if(p104->CtrlCmd==IEC104_CTRL_EXECUTE&& bInfNo!=250)
		{
			printDebug("收到报文148，撤销确认，没有请求");
			erroFlag=1;
			break;
		}
		switch(bInfNo)
		{	
		case 249:	//response for selection
			if(bCot==IEC104_COT_REQ)
				SResult.bResult=0x01;
			else if((bCot==IEC104_COT_UNKNOWN) ||( bCot==0x45))
				SResult.bResult=0x00;
			else
				erroFlag=1;
			break;
		case 250:	//response for selection
			if(bCot==IEC104_COT_REQ)
				SResult.bResult=0x03;
			else if((bCot==IEC104_COT_UNKNOWN) ||( bCot==0x45))
				SResult.bResult=0x02;
			else
				erroFlag=1;
			break;
		case 251:	//response for undoing
			if(bCot==IEC104_COT_REQ)
				SResult.bResult=0x05;
			else if((bCot==IEC104_COT_UNKNOWN) ||( bCot==0x45))
				SResult.bResult=0x04;
			else
				erroFlag=1;
			break;
		default:	//errors
			erroFlag=1;
			break;
		}
	}while(0);

	if(!erroFlag)
	{
		if(bGroup ==0)	//change the area number of fix value
			SResult.bResult |=0x80; 
		p104->AppState = IEC104_APP_STATE_IDLE;
		GetHandler()->handle_Modify_Value(SResult);
	}
	else
		printDebug("收到报文148，传送原因或信息序号解析出错");
	
	
}
void   CIec104::AppRxProtectAction(const b_ui8 *asdu, b_ui8 size) //140
{
	SIec104Data *p104;
	int		i,j,iInfBase,iInfAddrLen,iInfIndex,iNum;
	int		iRtuAddrLen=0,iRtuAddrIndex=0,iRtuAddr=0;
	b_ui8   bCot =0,bInfNo=0,bTotalNo=0;
	b_ui8   btBuf[4];
	int ioffset=0;

	SProtectActionRaw protectAction[45];

	iNum = asdu[1]& 0x7f;
	p104 = m_pIec104Data;
	iInfBase= 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	iInfAddrLen = p104->AppPara.InfAddrLen;

	
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;
	bCot = asdu[2]&0x3f;

	for(i=iRtuAddrLen;i>0;i--)//get RTU common address
	{
		iRtuAddr = iRtuAddr*256+asdu[iRtuAddrIndex+i-1];
	}

	if(bCot != IEC104_COT_SPONT){
		printDebug("错误的传送原因");
		return;
	}
	for(i=0;i<iNum;i++)
	{
		iInfIndex = iInfBase +i*13+ioffset;
		protectAction[i].iTotalNo = asdu[iInfIndex+1]*256 +asdu[iInfIndex];
		protectAction[i].bProtectActionNo = asdu[iInfIndex+2];
		protectAction[i].bDpointVal    =  asdu[iInfIndex+3];
		protectAction[i].iRelTime     = asdu[iInfIndex+5]*256 +  asdu[iInfIndex+4];
		protectAction[i].iFailNo       = asdu[iInfIndex+7]*256+asdu[iInfIndex+6];
		protectAction[i].time.milisecond = asdu[iInfIndex+9]*256+asdu[iInfIndex+8];
		protectAction[i].time.minute = asdu[iInfIndex+10]&0x3f;
		protectAction[i].time.hour    = asdu[iInfIndex+11]&0x1f;
		protectAction[i].bFailCount = asdu[iInfIndex+12];
		if(protectAction[i].bFailCount)
		{
			ioffset +=6*protectAction[i].bFailCount;
			for(j=0;j<protectAction[i].bFailCount;j++)
			{
				protectAction[i].sFailEq[j].iOffset = asdu[iInfIndex+13+6*j]+asdu[iInfIndex+14+6*j]*256;
				memcpy(btBuf,&asdu[iInfIndex+15+6*j],4);
				//adjustSeq(btBuf, 4);
				protectAction[i].sFailEq[j].fVal = *(float *)&btBuf[0]; 
				
			}
		}
		
	}
	if(iNum>0)
		GetHandler()->handle_Protect_Action(protectAction,iNum);

	
	
}
inline 
int CIec104::getRtuAddr(const b_ui8 *apdu)
{
	SIec104Data *p104;
	int		i=0;
	int		iRtuAddrLen=0,iRtuAddrIndex=0,iRtuAddr=0;
	
	p104 = m_pIec104Data;
	iRtuAddrIndex   =    2+ p104->AppPara.CotLen;
	iRtuAddrLen= p104->AppPara.AsduAddrLen;

	for(i=iRtuAddrLen;i>0;i--)//get ASDU address
	{
		iRtuAddr = iRtuAddr*256+apdu[iRtuAddrIndex+i-1];
	}   
	return iRtuAddr;
}
b_i32 CIec104::getTimeCnt()
{
#ifdef WIN32
	return timeGetTime() %  IEC104MAX_TIME_CNT;
#else
	struct timeval curTime;
	gettimeofday(&curTime,NULL);
	return (curTime.tv_sec *1000 + (curTime.tv_usec/1000) ) % IEC104MAX_TIME_CNT;
#endif
}
void CIec104::resetTimer(int timer)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	switch( timer)
	{
		case 1:
			p104->T1Start = getTimeCnt();
			p104->T1Cnt = p104->LinkPara->T1 *IEC104_TIME_FACTOR;
			break;
		case 2:
			p104->T2Start = getTimeCnt();
			p104->T2Cnt = p104->LinkPara->T2 *IEC104_TIME_FACTOR;
			break;
		case 3:
			p104->T3Start = getTimeCnt();
			p104->T3Cnt = p104->LinkPara->T3 *IEC104_TIME_FACTOR;
			break;
		case 4:
			if(p104->LinkPara->T4>0)
			{
				p104->T4Start = getTimeCnt();
				p104->T4Cnt = p104->LinkPara->T4 *IEC104_TIME_FACTOR;
			}
			else
			{
				p104->T4Start=IEC104_TIME_STOP;
			}
			break;
		default:
			char str[125];
			sprintf(str,"Undefined timer %d when start timer",timer);
			std::string logStr=str;
			this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,logStr);
	}
}
void CIec104::stopTimer(int timer)
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	switch( timer)
	{
		case 1:
			p104->T1Start =  IEC104_TIME_STOP;
			p104->T1Cnt =p104->LinkPara->T1 *IEC104_TIME_FACTOR;
			break;
		case 2:
			p104->T2Start =  IEC104_TIME_STOP;
			p104->T2Cnt =p104->LinkPara->T2 *IEC104_TIME_FACTOR;
			break;
		case 3:
			p104->T3Start =  IEC104_TIME_STOP;
			p104->T3Cnt =p104->LinkPara->T3 *IEC104_TIME_FACTOR;
			break;
		case 4:
			p104->T4Start =  IEC104_TIME_STOP;
			p104->T4Cnt =p104->LinkPara->T4 *IEC104_TIME_FACTOR;
			break;
		default:
			char str[100];
			sprintf(str,"Undefined timer %d when stop timer",timer);
			std::string logStr=str;
			this->m_pChannel->logChannelMsg(LOG_LEVEL_ERROR,logStr);
	}
}
void CIec104::updateTimer()
{
	SIec104Data *p104;
	p104 = m_pIec104Data;
	b_i32 curCnt = getTimeCnt();
	b_i32 differCnt=0;
	
	if(p104->T1Start !=  IEC104_TIME_STOP)
	{
		differCnt =( curCnt + IEC104MAX_TIME_CNT -  p104->T1Start ) % IEC104MAX_TIME_CNT;
		p104->T1Cnt = p104->LinkPara->T1 *IEC104_TIME_FACTOR - differCnt;
	}
	if(p104->T2Start !=  IEC104_TIME_STOP)
	{
		differCnt =( curCnt + IEC104MAX_TIME_CNT -  p104->T2Start ) % IEC104MAX_TIME_CNT;
		p104->T2Cnt =p104->LinkPara->T2 *IEC104_TIME_FACTOR - differCnt;

	}
	if(p104->T3Start !=  IEC104_TIME_STOP)
	{
		differCnt =( curCnt + IEC104MAX_TIME_CNT -  p104->T3Start ) % IEC104MAX_TIME_CNT;
		p104->T3Cnt =p104->LinkPara->T3 *IEC104_TIME_FACTOR - differCnt;
	}
	if(p104->T4Start !=  IEC104_TIME_STOP)
	{
		differCnt =( curCnt + IEC104MAX_TIME_CNT -  p104->T4Start ) % IEC104MAX_TIME_CNT;
		p104->T4Cnt =p104->LinkPara->T4 *IEC104_TIME_FACTOR - differCnt;
	}
}

int CIec104::AppRxAsdu_70( const b_ui8 *pAsdu, b_ui8 iSize)
{
	b_ui8	btSeqflag, btCot,bCoi;
	int          rtuAddr;
	SIec104Data *p104;
	p104 = m_pIec104Data;
	btSeqflag = pAsdu[1]&0x80 ;
	btCot =  pAsdu[2]&0x3f;
	rtuAddr = getRtuAddr(pAsdu);

	int iInf = 2+ p104->AppPara.CotLen + p104->AppPara.AsduAddrLen;
	int iInfAddrLen = p104->AppPara.InfAddrLen;

	bCoi = pAsdu[iInf+iInfAddrLen];
	if(btCot!=IEC104_COT_INIT)
		m_handler->logRecord("Bad cot-%02x for EndOfInitialization-70, correct is 4",btCot);
	if(btSeqflag!=1)
		m_handler->logRecord("Bad seq-%02x for EndOfInitialization-70, correct is 1",btSeqflag);
	m_handler->logRecord("Recv EndOfInitialization-70, COI is %02x",bCoi);
	return 0;
}


int CIec104::checkAsduSize(b_ui8 bTi,const b_ui8 bVsq,b_ui8 size)
{
	b_ui8 objSize=0,num,sq,tmpSize;
	switch(bTi)
	{
		case IEC104_M_SP_NA:
		case IEC104_M_DP_NA:
			objSize = 1;			break;
		case IEC104_M_ST_NA:
			objSize = 2;			break;
		case IEC104_M_BO_NA:
			objSize = 5;			break;
		case IEC104_M_ME_NA:				
		case IEC104_M_ME_NB:
			objSize = 3;			break;
		case IEC104_M_ME_NC:		
		case IEC104_M_IT_NA:					
		case IEC104_M_SP_NB:	
			objSize = 5;			break;
		case IEC104_M_ME_ND:
			objSize = 2;			break;
		case  IEC104_M_SP_TB:					
		case IEC104_M_DP_TB:	
			objSize = 8;			break;
		case  IEC104_M_ST_TB:	
			objSize = 9;			break;
		case  IEC104_M_BO_TB	:
			objSize = 12;			break;
		case IEC104_M_ME_TD:				
		case IEC104_M_ME_TE:	
			objSize = 10;			break;
		case IEC104_M_ME_TF:					
		case IEC104_M_IT_TB:	
			objSize = 12;			break;
		case IEC104_M_EP_TD:		
			objSize = 10;			break;
		case IEC104_M_EP_TE:					
		case IEC104_M_EP_TF:	
			objSize = 11;			break;
		case  IEC104_C_SC_NA:				
		case IEC104_C_DC_NA:				
		case IEC104_C_RC_NA:		
			objSize = 1;			break;
		case IEC104_C_SE_NA:					
		case IEC104_C_SE_NB:	
			objSize = 3;			break;
		case IEC104_C_SE_NC:										
			objSize = 5;			break;
		case IEC104_C_BO_NA:
			objSize = 4;			break;
		case IEC104_C_SC_TA:					
		case IEC104_C_DC_TA:					
		case IEC104_C_RC_TA:		
			objSize = 8;			break;
		case IEC104_C_SE_TA:					
		case IEC104_C_SE_TB:	
			objSize = 10;			break;
		case IEC104_C_SE_TC:	
			objSize = 12;			break;
		case IEC104_C_BO_TA:
			objSize = 11;			break;
		case IEC104_M_EI_NA:	
		case IEC104_C_IC_NA:			
		case IEC104_C_CI_NA:		
			objSize = 1;			break;
		case IEC104_C_RD_NA:		
			objSize = 0;			break;
		case IEC104_C_CS_NA:		
			objSize = 7;			break;
		case  IEC104_C_RP_NA:	
			objSize = 1;			break;
		case IEC104_C_TS_TA:	
			objSize = 9;			break;
		case IEC104_P_ME_NA:				
		case IEC104_P_ME_NB:	
			objSize = 3;			break;
		case IEC104_P_ME_NC:	
			objSize = 5;			break;
		case IEC104_P_NC_NA:		
			objSize = 1;			break;
		case IEC104_F_FR_NA:				
			objSize = 6;			break;
		case IEC104_F_SR_NA	:		
			objSize = 7;			break;
		case IEC104_F_SC_NA	:		
			objSize = 4;			break;
		case IEC104_F_LS_NA	:			
			objSize = 5;			break;
		case IEC104_F_AF_NA	:			
			objSize = 4;			break;
		case IEC104_F_DR_NA	:			
			objSize = 13;			break;
		default:
			return 0;
	}
	num= bVsq &0x7f;
	sq  = (bVsq &0x80)>0?1:0;
	tmpSize =2+m_pIec104Data->AppPara.CotLen+m_pIec104Data->AppPara.AsduAddrLen;
	if(sq)
		tmpSize+=m_pIec104Data->AppPara.InfAddrLen+num*objSize;
	else
		tmpSize+=num*(m_pIec104Data->AppPara.InfAddrLen+objSize);
	return tmpSize==size?0:-1;
}
b_ui16 CIec104::p104tohost16(const b_ui16 src)
{
	b_ui16 dest;
	dest = ((src>>8)&0x00ff)|((src<<8)&0xff00);
	return ntohs(dest);
}

b_ui32 CIec104::p104tohost32(const b_ui32 src)
{
	b_ui32 dest;
	dest=((src>>24)&0x000000ff)|((src>>8)&0x0000ff00)|((src<<8)&0x00ff0000)|((src<<24)&0xff000000);
	return ntohl(dest);
}

