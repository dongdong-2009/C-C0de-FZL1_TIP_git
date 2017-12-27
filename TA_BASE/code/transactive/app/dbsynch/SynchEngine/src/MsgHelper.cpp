#include "MsgHelper.h"
#include "CommonDef.h"
#include "CommonData.h"

#include "Buffer.h"

#include "IMsgItem.h"
#include "IMsgPackage.h"
#include "MsgItemImp.h"
#include "MsgPackageImp.h"
#include "Message.h"


#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)



CMsgItemHelper::CMsgItemHelper()
{
}

CMsgItemHelper::~CMsgItemHelper()
{
}
	  

int CMsgItemHelper::_GetConACKByItem(const IMsgItem* pMsgItem, CONACK*& pData)
{
	int nFunRes = 0;   					   
	unsigned char ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int nInt32Value = 0;
	CBuffer* pBuffer = NULL;

	nInt32Value = 0;
	if (NULL == pMsgItem)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//conack not have list type data, just delete
	DEF_DELETE(pData);	

	if (NULL == pData)
	{
		pData = new CONACK();
	}

	pData->m_strQName = (char *)pMsgItem->getMsgQN();

	nInt32Value =   pMsgItem->getMsgDL();	
	if (MSGITEM_INT32_VALUE_LEN == nInt32Value)
	{
		pBuffer = new CBuffer(pMsgItem->getMsgDL() + BUFFER_LEN_ADDEX);	
		pBuffer->write(pMsgItem->getMsgData(), pMsgItem->getMsgDL());

		//get ConReqACK
		memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
		pBuffer->read(&ucszInt32Value, MSGITEM_INT32_VALUE_LEN);
		pData->m_nConReqAck = DEF_INT4KORR(ucszInt32Value);

	}

	DEF_DELETE(pBuffer);	

	return nFunRes;
}



int CMsgItemHelper::_GetEnqReqByItem(lstMsgItemConT* pGetMsgItemConT, ENQREQEX*& pData)
{ 
	int				nFunRes = 0;   					   
	unsigned char			ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int				nInt32Value = 0;
	lstMsgItemConIterT iterLstMsgItem;
	CMessage*		pMessage = NULL;
	IMsgItem*		pMsgItem = NULL;
	char*			pszTemp = NULL;		
	std::string		strQName;
	std::string		strMsgID;
	int             nOServerID;
	ulong64			nSIDValue = 0;
	unsigned char			ucPR = 0;
	unsigned char			ucPYTP = 0;
	std::string		strPayload;
	std::string		strServerAgtName; 
								 
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	nInt32Value = 0;
	if (NULL == pGetMsgItemConT)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//ENQREQEX have list type data, free list data first
	_FreeEnqReqEx(&pData);
	if (NULL == pData)
	{
		pData = new ENQREQEX();
	}

	//get ServerAgtName eg. TRANSACT
	strServerAgtName = g_system_variables.m_LocalAgent.getName();

	iterLstMsgItem =  pGetMsgItemConT->begin();
	while (iterLstMsgItem != pGetMsgItemConT->end())
	{
		pMsgItem = (*iterLstMsgItem);
		strQName = (char *)pMsgItem->getMsgQN();
		strMsgID = (char *)pMsgItem->getMsgOMI();
		nOServerID = pMsgItem->getMsgOServerID();
		nSIDValue = pMsgItem->getMsgSID();
		ucPR =  (*(pMsgItem->getMsgPR()));
		ucPYTP = (*(pMsgItem->getMsgPYTP()));
		
		nInt32Value = pMsgItem->getMsgDL() + 1;
		pszTemp = new char[nInt32Value];
		memset(pszTemp, 0, nInt32Value);
		memcpy(pszTemp, pMsgItem->getMsgData(), pMsgItem->getMsgDL());
		strPayload = pszTemp;
		DEF_DELETE_ARRAY(pszTemp);		

		pMessage = new CMessage();
		pMessage->setQName(strQName);
		pMessage->setMsgID(strMsgID);
		pMessage->setMsgSeqID(nSIDValue);
		pMessage->setMsgPriority((int)ucPR);
		pMessage->setPayloadType((int)ucPYTP);
		pMessage->setPayload(strPayload);	
		pMessage->setOServerID(nOServerID);
	    
		pData->m_lstDBMsg.push_back(pMessage);
		pMessage = NULL;

		iterLstMsgItem++;
	}	

	return nFunRes;
}

int CMsgItemHelper::_GetEnqACKByItem(const IMsgItem* pMsgItem, ENQACK*& pData)
{
	int nFunRes = 0;   					   
	unsigned char ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int nInt32Value = 0;
	CBuffer* pBuffer = NULL;

	nInt32Value = 0;
	if (NULL == pMsgItem)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//ENQACK not have list type data, just delete
	DEF_DELETE(pData);
	if (NULL == pData)
	{
		pData = new ENQACK();
	}
	
	pData->m_strQName = (char *)pMsgItem->getMsgQN();

	nInt32Value =   pMsgItem->getMsgDL();	
	if (MSGITEM_INT32_VALUE_LEN == nInt32Value)
	{
		pBuffer = new CBuffer(pMsgItem->getMsgDL() + BUFFER_LEN_ADDEX);	
		pBuffer->write(pMsgItem->getMsgData(), pMsgItem->getMsgDL());
		
		//get EnqReqACK
		memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
		pBuffer->read(&ucszInt32Value, MSGITEM_INT32_VALUE_LEN);
		pData->m_nEnqReqACK = DEF_INT4KORR(ucszInt32Value);

	}
	
	DEF_DELETE(pBuffer);	

	return nFunRes;
}

int CMsgItemHelper::_GetConReqByItem(const IMsgItem* pMsgItem, CONREQ*& pData)
{
	int nFunRes = 0;   					   
	unsigned char ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int nInt32Value = 0;
	char* pStrTmp = NULL;
	CBuffer* pBuffer = NULL;

	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	nInt32Value = 0;
	if (NULL == pMsgItem)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//CONREQ not have list type data, just delete 
	DEF_DELETE(pData);
	if (NULL == pData)
	{
		pData = new CONREQ();
	}

	//get QName
	pData->m_strQName = (char *)pMsgItem->getMsgQN();
	//get OServerID
	pData->m_nOServerID = pMsgItem->getMsgOServerID();
	pBuffer = new CBuffer(pMsgItem->getMsgDL());	
	pBuffer->write(pMsgItem->getMsgData(), pMsgItem->getMsgDL());

	//get AgtName
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	nInt32Value = 0;
	memcpy(ucszInt32Value, pBuffer->getReadPos(), MSGITEM_INT32_VALUE_LEN);
	pBuffer->moveReadPos(MSGITEM_INT32_VALUE_LEN);
	nInt32Value = DEF_INT4KORR(ucszInt32Value);
	pStrTmp = new char[nInt32Value + 1];
	memset(pStrTmp, 0, nInt32Value + 1);
	pBuffer->read(pStrTmp, nInt32Value);
	pData->m_strAgtName = pStrTmp;
	DEF_DELETE_ARRAY(pStrTmp);

	//get HostName
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	nInt32Value = 0;
	memcpy(ucszInt32Value, pBuffer->getReadPos(), MSGITEM_INT32_VALUE_LEN);
	pBuffer->moveReadPos(MSGITEM_INT32_VALUE_LEN);
	nInt32Value = DEF_INT4KORR(ucszInt32Value);	
	pStrTmp = new char[nInt32Value + 1];
	memset(pStrTmp, 0, nInt32Value + 1);
	pBuffer->read(pStrTmp, nInt32Value);
	pData->m_strHostName = pStrTmp;
	DEF_DELETE_ARRAY(pStrTmp);

	//get IP
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	nInt32Value = 0;
	memcpy(ucszInt32Value, pBuffer->getReadPos(), MSGITEM_INT32_VALUE_LEN);
	pBuffer->moveReadPos(MSGITEM_INT32_VALUE_LEN);
	nInt32Value = DEF_INT4KORR(ucszInt32Value);	
	pStrTmp = new char[nInt32Value + 1];
	memset(pStrTmp, 0, nInt32Value + 1);
	pBuffer->read(pStrTmp, nInt32Value);
	pData->m_strIP = pStrTmp;
	DEF_DELETE_ARRAY(pStrTmp);

	/////////////////////////////////////	
	
	DEF_DELETE(pBuffer);	

	return nFunRes;
}



int CMsgItemHelper::_SetEnqReq(unsigned char ucMsgItemType, PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg)
{
	int				nFunRes = 0;
	unsigned char	ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int				nDL = 0;
	IMsgItem*		pNewMsgItem = NULL;

	unsigned char	ucMT = 0;
	unsigned char	ucPR = 0;
	lstMsgItemConT* pNewLstmsgItem = NULL;
	lstDBMsgConIterT lstDBMsgIter;
	CMessage*		pGetMessage = NULL;	

	int              nMsgPriority;     // the message priority
	unsigned char    nPayloadType;     // the type of the payload. 1: RAW      2: OBJ
	int              nSubCnt;          // indicate the message belong to.  0: local  non-zero(!0): remote 
	ulong64          ulSeqID;          // the sequence ID of the message. 64bit.
	int              uDataLen;         // the size of the payload user data.
	std::string      strMsgID;         // the message UUID
	int              nOServerID;
	std::string      strQName;         // the queue name in which case the message belong to.
	std::string      strSender;        // the message sender
	std::string      strRecipientLst;  // the recipient list for the message will be sent to.
	std::string      objPayload;       // the payload of the message. it's keeps the payload in a key-value pair manner



	if (NULL == pEnqReqEx)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//MSGPKG have list type data, free list data first
	_FreeMsgPkg(&pMsgPkg);
	if (NULL == pMsgPkg)
	{
		pMsgPkg = new MSGPKG();
	}

	ucMT = ucMsgItemType;// ComProtocol_CTOS_ENQ_REQ;
	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_REQUEST;

	//new One MsgItem list
	pNewLstmsgItem = new lstMsgItemConT();


	lstDBMsgIter = pEnqReqEx->m_lstDBMsg.begin();
	while (lstDBMsgIter != pEnqReqEx->m_lstDBMsg.end())
	{
		pGetMessage = *lstDBMsgIter;

		nMsgPriority = 0;
		nPayloadType = 0;
		nSubCnt = 0;
		ulSeqID = 0;
		uDataLen = 0;
		strMsgID.clear();
		strQName.clear();
		strSender.clear();
		strRecipientLst.clear();
		objPayload.clear();
		nOServerID = 0;

		//MT
		//QN
		strQName = pGetMessage->getQName();
		//OMI
		strMsgID = pGetMessage->getMsgID();	
		//nOServerID
		nOServerID = pGetMessage->getOServerID();	
		//SID
		ulSeqID = pGetMessage->getMsgSeqID();
		//PR
		nMsgPriority = pGetMessage->getMsgPriority();
		//PYTP
		nPayloadType = (unsigned char)pGetMessage->getPayloadType();
		//MSgItemData
		pGetMessage->getPayload(objPayload);
		//MDL
		nDL = (int)objPayload.size();

		//print log  payload lenth
		LOG1(SourceInfo, DebugUtil::DebugMsg,"[nDL=objPayload.size()=%d]", nDL); 
		//print log  payload 
		LOGLARGESTRING(SourceInfo, DebugUtil::DebugMsg, objPayload);  

	
		//new One MsgItem
		pNewMsgItem = new CMsgItemImp();
		pNewMsgItem->setMsgMT((unsigned char *)&ucMT, MSGITEM_LEN_MT);
		pNewMsgItem->setMsgQN((unsigned char *)strQName.c_str(), (int)strQName.size());//	MSGITEM_LEN_QN
		pNewMsgItem->setMsgOMI((unsigned char *)strMsgID.c_str(), (int)strMsgID.size());//	MSGITEM_LEN_QN
		pNewMsgItem->setMsgOServerID(nOServerID);
		pNewMsgItem->setMsgSID(ulSeqID);

		ucPR = MSGITEM_PR_NORMAL;// (u_char)(nMsgPriority)
		pNewMsgItem->setMsgPR((unsigned char *)&ucPR, MSGITEM_LEN_PR);

		pNewMsgItem->setMsgPYTP((unsigned char *)&nPayloadType, MSGITEM_LEN_PYTP);

		memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
		DEF_INT4STORE(ucszInt32Value, nDL);

		pNewMsgItem->setMsgDLArray((unsigned char *)&ucszInt32Value, MSGITEM_LEN_MDL);
		pNewMsgItem->setMsgData((unsigned char *)objPayload.c_str(), (int)objPayload.size());


		//add one MsgItem to msgItem list
		pNewLstmsgItem->push_back(pNewMsgItem);
		pNewMsgItem = NULL;

		lstDBMsgIter++;
	}



	pMsgPkg->m_strPkgUUID32 = pEnqReqEx->m_strPkgID;
	pMsgPkg->m_bZip = g_system_variables.m_MsgCompress;
	//pMsgPkg->m_lstAllMsgItems.clear();
	//add one msgItem list to msgItems list
	pMsgPkg->m_lstAllMsgItems.push_back(pNewLstmsgItem);

	pNewLstmsgItem = NULL;
	pNewMsgItem = NULL;
	return nFunRes;
}

int CMsgItemHelper::_SetConReq(unsigned char ucMsgItemType, PCONREQ pConReq, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	unsigned char ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int nInt32Value = 0;
	int nDL = 0;
	CBuffer* pBuffer = NULL;
	IMsgItem* pNewMsgItem = NULL;

	unsigned char ucMT = 0;
	unsigned char ucPR = 0;
	lstMsgItemConT* pNewLstmsgItem = NULL;
	int nDataLen = 0;

	if (NULL == pConReq)
	{
		nFunRes = -1;
		return nFunRes;
	}

	//MSGPKG have list type data, free list data first
	_FreeMsgPkg(&pMsgPkg); 
	if (NULL == pMsgPkg)
	{
		pMsgPkg = new MSGPKG();
	}

	//ucMT = ComProtocol_CTOS_CON_REQ;
	ucMT = ucMsgItemType;
	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_CONREQUEST;
	
	nDataLen += MSGITEM_INT32_VALUE_LEN;
	nDataLen += (int)pConReq->m_strAgtName.size();
	nDataLen += MSGITEM_INT32_VALUE_LEN;
	nDataLen += (int)pConReq->m_strHostName.size();
	nDataLen += MSGITEM_INT32_VALUE_LEN;
	nDataLen += (int)pConReq->m_strIP.size();
	
	

	pBuffer = new CBuffer(nDataLen + BUFFER_LEN_ADDEX);
	//get agtName and agtname string len
	nInt32Value = (int)pConReq->m_strAgtName.size();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nInt32Value);

	pBuffer->write(ucszInt32Value, MSGITEM_INT32_VALUE_LEN);
	pBuffer->write(pConReq->m_strAgtName.c_str(), nInt32Value);

	//get m_strHostName and m_strHostName string len
	nInt32Value = (int)pConReq->m_strHostName.size();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nInt32Value);

	pBuffer->write(ucszInt32Value, MSGITEM_INT32_VALUE_LEN);
	pBuffer->write(pConReq->m_strHostName.c_str(), nInt32Value);

	//get m_strIP and m_strIP string len
	nInt32Value = (int)pConReq->m_strIP.size();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nInt32Value);

	pBuffer->write(ucszInt32Value, MSGITEM_INT32_VALUE_LEN);
	pBuffer->write(pConReq->m_strIP.c_str(), nInt32Value);

	//new One MsgItem
	pNewMsgItem = new CMsgItemImp();
	//ucMT = ComProtocol_CTOS_CON_REQ;
	pNewMsgItem->setMsgMT((unsigned char *)&ucMT, MSGITEM_LEN_MT);
	pNewMsgItem->setMsgOServerID(pConReq->m_nOServerID);  
	pNewMsgItem->setMsgQN((unsigned char *)pConReq->m_strQName.c_str(), (int)pConReq->m_strQName.size());//	MSGITEM_LEN_QN

	//pNewMsgItem->setMsgOMI((u_char *)readpos, MSGITEM_LEN_OMI);
	ucPR = MSGITEM_PR_NORMAL; 
	pNewMsgItem->setMsgPR((unsigned char *)&ucPR, MSGITEM_LEN_PR);

	nDL = pBuffer->remainDataLength();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nDL);


	pNewMsgItem->setMsgDLArray((unsigned char *)&ucszInt32Value, MSGITEM_LEN_MDL);
	pNewMsgItem->setMsgData((unsigned char *)pBuffer->getReadPos(), pBuffer->remainDataLength());

	//new One MsgItem list
	pNewLstmsgItem = new lstMsgItemConT();
	//add one MsgItem to msgItem list
	pNewLstmsgItem->push_back(pNewMsgItem);


	pMsgPkg->m_strPkgUUID32 = pConReq->m_strPkgID;
	//pMsgPkg->m_nPkgType = MSGPACKAGE_PT_REQUEST;
	pMsgPkg->m_bZip = g_system_variables.m_MsgCompress;
	//pMsgPkg->m_lstAllMsgItems.clear();
	//add one msgItem list to msgItems list
	pMsgPkg->m_lstAllMsgItems.push_back(pNewLstmsgItem);
	
	DEF_DELETE(pBuffer);	

	pNewLstmsgItem = NULL;
	pNewMsgItem = NULL;
	return nFunRes;
}
int CMsgItemHelper::_SetEnqACK(unsigned char ucMsgItemType, PENQACK pEnqACK, MSGPKG*& pMsgPkg)
{
	int				nFunRes = 0;
	unsigned char			ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int				nInt32Value = 0;
	int				nDL = 0;
	CBuffer*		pBuffer = NULL;
	IMsgItem*		pNewMsgItem = NULL;
	unsigned char			ucMT = 0;
	unsigned char			ucPR = 0;
	lstMsgItemConT* pNewLstmsgItem = NULL;
	int             nOServerID = 0;

	if (NULL == pEnqACK)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//MSGPKG have list type data, free list data first
	_FreeMsgPkg(&pMsgPkg); 

	if (NULL == pMsgPkg)
	{
		pMsgPkg = new MSGPKG();
	}
	//ucMT = ComProtocol_CTOS_ENQ_ACK;
	ucMT = ucMsgItemType;
	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_ACK;


	pBuffer = new CBuffer(BUFFER_LEN_ADDEX);

	//get len
	nInt32Value = pEnqACK->m_nEnqReqACK;
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nInt32Value);
	pBuffer->bufferSizePrepare(MSGITEM_INT32_VALUE_LEN);
	pBuffer->write(ucszInt32Value, MSGITEM_INT32_VALUE_LEN);

	//new One MsgItem
	pNewMsgItem = new CMsgItemImp();
	//ucMT = ComProtocol_CTOS_ENQ_ACK;
	pNewMsgItem->setMsgMT((unsigned char *)&ucMT, MSGITEM_LEN_MT);
	pNewMsgItem->setMsgQN((unsigned char *)pEnqACK->m_strQName.c_str(), (int)pEnqACK->m_strQName.size());//	MSGITEM_LEN_QN

	pNewMsgItem->setMsgOServerID(nOServerID);
	ucPR = MSGITEM_PR_NORMAL; 
	pNewMsgItem->setMsgPR((unsigned char *)&ucPR, MSGITEM_LEN_PR);

	nDL = pBuffer->remainDataLength();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nDL);


	pNewMsgItem->setMsgDLArray((unsigned char *)&ucszInt32Value, MSGITEM_LEN_MDL);
	pNewMsgItem->setMsgData((unsigned char *)pBuffer->getReadPos(), pBuffer->remainDataLength());

	//new One MsgItem list
	pNewLstmsgItem = new lstMsgItemConT();
	//add one MsgItem to msgItem list
	pNewLstmsgItem->push_back(pNewMsgItem);


	pMsgPkg->m_strPkgUUID32 = pEnqACK->m_strPkgID;
	pMsgPkg->m_bZip = g_system_variables.m_MsgCompress;
	//	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_ACK;
	//pMsgPkg->m_lstAllMsgItems.clear();
	//add one msgItem list to msgItems list
	pMsgPkg->m_lstAllMsgItems.push_back(pNewLstmsgItem);
	
	DEF_DELETE(pBuffer);

	pNewLstmsgItem = NULL;
	pNewMsgItem = NULL;
	return nFunRes;
}


int CMsgItemHelper::_SetConACK(unsigned char ucMsgItemType, PCONACK pConACK, MSGPKG*& pMsgPkg)
{
	int				nFunRes = 0;
	unsigned char			ucszInt32Value[MSGITEM_INT32_VALUE_LEN];
	int				nInt32Value = 0;
	int				nDL = 0;
	CBuffer*		pBuffer = NULL;
	IMsgItem*		pNewMsgItem = NULL;
	unsigned char			ucMT = 0;
	unsigned char			ucPR = 0;
	lstMsgItemConT* pNewLstmsgItem = NULL;
	int             nOServerID = 0;

	if (NULL == pConACK)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//MSGPKG have list type data, free list data first
	_FreeMsgPkg(&pMsgPkg); 
	if (NULL == pMsgPkg)
	{
		pMsgPkg = new MSGPKG();
	}
	//ucMT = ComProtocol_STOC_CON_ACK;
	ucMT = ucMsgItemType;
	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_ACK;


	pBuffer = new CBuffer(BUFFER_LEN_ADDEX);
		
	//get len
	nInt32Value = pConACK->m_nConReqAck;
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nInt32Value);
	
	pBuffer->bufferSizePrepare(MSGITEM_INT32_VALUE_LEN);
	pBuffer->write(ucszInt32Value, MSGITEM_INT32_VALUE_LEN);

	//new One MsgItem
	pNewMsgItem = new CMsgItemImp();
	//ucMT = ComProtocol_CTOS_ENQ_ACK;
	pNewMsgItem->setMsgMT((unsigned char *)&ucMT, MSGITEM_LEN_MT);
	pNewMsgItem->setMsgQN((unsigned char *)pConACK->m_strQName.c_str(), (int)pConACK->m_strQName.size());//	MSGITEM_LEN_QN

	pNewMsgItem->setMsgOServerID(nOServerID);

	ucPR = MSGITEM_PR_NORMAL; 
	pNewMsgItem->setMsgPR((unsigned char *)&ucPR, MSGITEM_LEN_PR);

	nDL = pBuffer->remainDataLength();
	memset(ucszInt32Value, 0, MSGITEM_INT32_VALUE_LEN);
	DEF_INT4STORE(ucszInt32Value, nDL);


	pNewMsgItem->setMsgDLArray((unsigned char *)&ucszInt32Value, MSGITEM_LEN_MDL);
	pNewMsgItem->setMsgData((unsigned char *)pBuffer->getReadPos(), pBuffer->remainDataLength());

	//new One MsgItem list
	pNewLstmsgItem = new lstMsgItemConT();
	//add one MsgItem to msgItem list
	pNewLstmsgItem->push_back(pNewMsgItem);


	pMsgPkg->m_strPkgUUID32 = pConACK->m_strPkgID;
	pMsgPkg->m_bZip = g_system_variables.m_MsgCompress;
	//	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_ACK;
	//pMsgPkg->m_lstAllMsgItems.clear();
	//add one msgItem list to msgItems list
	pMsgPkg->m_lstAllMsgItems.push_back(pNewLstmsgItem);
	
	DEF_DELETE(pBuffer);	

	pNewLstmsgItem = NULL;
	pNewMsgItem = NULL;
	return nFunRes;
}




int CMsgItemHelper::setCTOSEnqReq(PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetEnqReq(ComProtocol_CTOS_ENQ_REQ, pEnqReqEx, pMsgPkg);
	return nFunRes;
}
int CMsgItemHelper::setCTOSConReq(PCONREQ pConReq, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetConReq(ComProtocol_CTOS_CON_REQ, pConReq, pMsgPkg);
	return nFunRes;

}
int CMsgItemHelper::setCTOSEnqACK(PENQACK pEnqACK, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetEnqACK(ComProtocol_CTOS_ENQ_ACK, pEnqACK, pMsgPkg);
	return nFunRes;
}

int CMsgItemHelper::setSTOCEnqReq(PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetEnqReq(ComProtocol_STOC_ENQ_REQ, pEnqReqEx, pMsgPkg);
	return nFunRes;

}
int CMsgItemHelper::setSTOCConACK(PCONACK pConACK, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetConACK(ComProtocol_STOC_CON_ACK, pConACK, pMsgPkg);
	return nFunRes;
}
int CMsgItemHelper::setSTOCEnqACK(PENQACK pEnqACK, MSGPKG*& pMsgPkg)
{
	int nFunRes = 0;
	_SetEnqACK(ComProtocol_STOC_ENQ_ACK, pEnqACK, pMsgPkg);
	return nFunRes;
}




int CMsgItemHelper::getCTOSEnqReq(MSGPKG* pMsgPkg, PENQREQEX& pEnqReqEx)
{	
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;		  
	

	_FreeEnqReqEx(&pEnqReqEx);

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;
		_GetEnqReqByItem(pGetMsgItemConT, pEnqReqEx);		
	   	
		iterMsgItems++;
	}

	if (NULL != pEnqReqEx)
	{
		pEnqReqEx->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}



int CMsgItemHelper::getCTOSConReq(MSGPKG* pMsgPkg, PCONREQ& pConReq)
{
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;
	lstMsgItemConIterT	 iterMsgItem;
	IMsgItem*		     pMsgItem = NULL;
	
	DEF_DELETE(pConReq);	

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;		
		iterMsgItem = pGetMsgItemConT->begin();
		while (iterMsgItem != pGetMsgItemConT->end())
		{
			pMsgItem = *iterMsgItem; 			
			_GetConReqByItem(pMsgItem, pConReq);		
			iterMsgItem++;	
		} 		   	
		iterMsgItems++;
	}

	if (NULL != pConReq)
	{
		pConReq->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}



int CMsgItemHelper::getCTOSEnqACK(MSGPKG* pMsgPkg, PENQACK& pEnqAck)
{
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;
	lstMsgItemConIterT	 iterMsgItem;
	IMsgItem*		     pMsgItem = NULL;
	
	DEF_DELETE(pEnqAck);	

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;		
		iterMsgItem = pGetMsgItemConT->begin();
		while (iterMsgItem != pGetMsgItemConT->end())
		{
			pMsgItem = *iterMsgItem; 			
			_GetEnqACKByItem(pMsgItem, pEnqAck);		
			iterMsgItem++;	
		} 		   	
		iterMsgItems++;
	}

	if (NULL != pEnqAck)
	{
		pEnqAck->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}


int CMsgItemHelper::getSTOCEnqReq(MSGPKG* pMsgPkg, PENQREQEX& pEnqReqEx)
{
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;
	
	
	_FreeEnqReqEx(&pEnqReqEx);	

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;
		_GetEnqReqByItem(pGetMsgItemConT, pEnqReqEx);		

		iterMsgItems++;
	}

	if (NULL != pEnqReqEx)
	{
		pEnqReqEx->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}

int CMsgItemHelper::getSTOCConACK(MSGPKG* pMsgPkg, PCONACK& pConACK)
{
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;
	lstMsgItemConIterT	 iterMsgItem;
	IMsgItem*		     pMsgItem = NULL;
	
	DEF_DELETE(pConACK);	

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;		
		iterMsgItem = pGetMsgItemConT->begin();
		while (iterMsgItem != pGetMsgItemConT->end())
		{
			pMsgItem = *iterMsgItem; 			
			_GetConACKByItem(pMsgItem, pConACK);		
			iterMsgItem++;	
		} 		   	
		iterMsgItems++;
	}

	if (NULL != pConACK)
	{
		pConACK->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}


int CMsgItemHelper::getSTOCEnqACK(MSGPKG* pMsgPkg, PENQACK& pEnqAck)
{
	int                  nFunRes = 0;
	lstMsgItemsConIterT  iterMsgItems;
	lstMsgItemConT*		 pGetMsgItemConT = NULL;
	lstMsgItemConIterT	 iterMsgItem;
	IMsgItem*		     pMsgItem = NULL;
	
	DEF_DELETE(pEnqAck);		

	iterMsgItems = pMsgPkg->m_lstAllMsgItems.begin();
	while (iterMsgItems != pMsgPkg->m_lstAllMsgItems.end())
	{
		pGetMsgItemConT = *iterMsgItems;		
		iterMsgItem = pGetMsgItemConT->begin();
		while (iterMsgItem != pGetMsgItemConT->end())
		{
			pMsgItem = *iterMsgItem; 			
			_GetEnqACKByItem(pMsgItem, pEnqAck);		
			iterMsgItem++;	
		} 		   	
		iterMsgItems++;
	}

	if (NULL != pEnqAck)
	{
		pEnqAck->m_strPkgID =   pMsgPkg->m_strPkgUUID32;
	}

	return nFunRes;
}


//////////////////////////////////////////////////////////////////////////


int  CMsgItemHelper::_FreeMsgVec(lstDBMsgConT& rVecMsg)
{
	int nFunRes = 0;
	lstDBMsgConIterT  iter = rVecMsg.begin();
	TA_AQ_Lib::CMessage* pGet = NULL;

	while ( iter != rVecMsg.end() )
	{
		pGet = *iter;
		DEF_DELETE(pGet);
		iter++;		
	}
	rVecMsg.clear();
	
	return nFunRes;
}

int	CMsgItemHelper::_FreeEnqReqEx(ENQREQEX** ppEnqReqEx)
{
	int nFunRes = 0;

	if (NULL != ppEnqReqEx && NULL != (*ppEnqReqEx))
	{
		_FreeMsgVec((*ppEnqReqEx)->m_lstDBMsg);
		DEF_DELETE(*ppEnqReqEx);
	}
	return nFunRes;
}



int CMsgItemHelper::_FreeLstMsgItem(lstMsgItemConT& lstMsgItem)
{
	int nFunRes = 0;
	lstMsgItemConIterT iterlstMsgItem;
	IMsgItem* pGetMsgItem = NULL;

	iterlstMsgItem = lstMsgItem.begin();
	while (iterlstMsgItem != lstMsgItem.end())
	{
		pGetMsgItem = *iterlstMsgItem;			
		DEF_DELETE(pGetMsgItem);				
		iterlstMsgItem++;
	}
	lstMsgItem.clear();	
	return nFunRes;
}

int CMsgItemHelper::_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems)
{
	int nFunRes = 0;
	lstMsgItemsConIterT iterLstAllMsgItems;
	lstMsgItemConT* pGetLstMsgItem = NULL;

	iterLstAllMsgItems = lstMsgItems.begin();
	while (iterLstAllMsgItems != lstMsgItems.end())
	{
		pGetLstMsgItem = *iterLstAllMsgItems;
		_FreeLstMsgItem(*pGetLstMsgItem);
		DEF_DELETE(pGetLstMsgItem);
		iterLstAllMsgItems++;
	}

	lstMsgItems.clear();

	return nFunRes;
}


int	CMsgItemHelper::_FreeMsgPkg(MSGPKG** ppMsgPkg)
{
	int nFunRes = 0;

	if (NULL != ppMsgPkg && NULL != (*ppMsgPkg))
	{
		_FreeLstMsgItems((*ppMsgPkg)->m_lstAllMsgItems);
		DEF_DELETE(*ppMsgPkg);
	}
	return nFunRes;
}



//////////////////////////////////////////////////////////////////////////

NS_END(TA_DB_Sync)






























