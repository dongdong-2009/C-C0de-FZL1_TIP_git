#include "ComProtocol.h"


#include "Buffer.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"

#include "zlib.h"
#include "IMsgItem.h"
#include "MsgItemImp.h"
#include "IMsgPackage.h"
#include "MsgPackageImp.h" 


#include "core/utilities/src/DebugUtil.h"

//#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;


NS_BEGIN(TA_DB_Sync)

CComProtocol::CComProtocol()
{
	
}


CComProtocol::~CComProtocol()
{
}



int CComProtocol::_GetBufferFromlstMsgItemConT(lstMsgItemConT& lstMsgItemSrc, CBuffer* pBufferDest)
{
	int					nFunRes = 0;
	lstMsgItemConIterT  iter;
	int					nTotalLen = 0;
	IMsgItem*			pMessageGet = NULL;

	if (NULL == pBufferDest)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL == pBufferDest" );	
		nFunRes = -1;
		return nFunRes;
	}

	if (lstMsgItemSrc.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "lstMsgItemSrc.empty()" );	
		nFunRes = -1;
		return nFunRes;	
	}
	
	iter = lstMsgItemSrc.begin();
	nTotalLen = 0;
	while (iter != lstMsgItemSrc.end())
	{
		pMessageGet = *iter;
		nTotalLen += pMessageGet->getMsgAllLen();
		iter++;
	}
	
	pBufferDest->bufferSizePrepare(nTotalLen + BUFFER_LEN_ADDEX); 

	pMessageGet = NULL;
	iter = lstMsgItemSrc.begin();
	while (iter != lstMsgItemSrc.end() )
	{
		pMessageGet = NULL;
		pMessageGet = *iter;
		memcpy(pBufferDest->getWritePos(), pMessageGet->getMsgStartPtr(), pMessageGet->getMsgAllLen());
		pBufferDest->moveWritePos(pMessageGet->getMsgAllLen());

		iter++;
		pMessageGet = NULL;
	}
	
	nFunRes = 0;
	return nFunRes;
}
 
int	CComProtocol::_GetBufferFromlstMsgItemsConT(lstMsgItemsConT& pMsgPkgSrc, CBuffer* pBuffDest)
{ 
	int nFunRes = 0;
	lstMsgItemsConIterT  iterLstMsgItems;
	lstMsgItemConT*      pGetEachMsgItem = NULL;
	CBuffer*			 pBufferEachMsgItem = NULL;
	int                  nBufferEachLen = 0;
	
	pBufferEachMsgItem = new CBuffer(BUFFER_LEN_READ);
	pBufferEachMsgItem->clear();
	iterLstMsgItems = pMsgPkgSrc.begin();
	while (iterLstMsgItems != pMsgPkgSrc.end())
	{
		pGetEachMsgItem = *iterLstMsgItems;
		_GetBufferFromlstMsgItemConT(*pGetEachMsgItem, pBufferEachMsgItem);
		
		nBufferEachLen =  pBufferEachMsgItem->remainDataLength();
		pBuffDest->bufferSizePrepare(nBufferEachLen + BUFFER_LEN_ADDEX);
		pBuffDest->write(pBufferEachMsgItem->getReadPos(), nBufferEachLen);
		pBufferEachMsgItem->moveReadPos(nBufferEachLen);
		pBufferEachMsgItem->clear();
		
		iterLstMsgItems++;
	}
	
	DEF_DELETE(pBufferEachMsgItem);
	pGetEachMsgItem = NULL;
	pBufferEachMsgItem = NULL;
	
	return nFunRes;
}

int	CComProtocol::processSendData(MSGPKG* pMsgPkgSrc, IMsgPackage*& pFrameDest)
{
	int					nFunRes = 0;

	unsigned char		ucMsgSTX;
	int					nMsgPL = 0;
	std::string			strUUID32;
	unsigned char		ucMsgPT;// = MSGPACKAGE_PT_REQUEST;
	int					nLenBeforeZip = 0;
	unsigned char		ucMsgDC;
	unsigned char		ucMsgETX;
	CBuffer*			pBufferSendData = NULL;
	CBuffer*			pBufferZip = NULL;
	int					nLenAfterZip = 0;
	CBuffer*			pBufferTmp = NULL;


	//////////////////////////////////////////////////////////////////////////
	ucMsgSTX = MSGPACKAGE_STX_VALUE;
	nMsgPL = 0;
	strUUID32 = pMsgPkgSrc->m_strPkgUUID32;
	
	ucMsgPT = pMsgPkgSrc->m_nPkgType;//MSGPACKAGE_PT_REQUEST;
	if (pMsgPkgSrc->m_bZip)
	{
		ucMsgDC = MSGPACKAGE_DC_ZIP;
	}
	else
	{
		ucMsgDC = MSGPACKAGE_DC_UNZIP;
	}
	
	pBufferSendData = new CBuffer(BUFFER_LEN_READ);
	_GetBufferFromlstMsgItemsConT(pMsgPkgSrc->m_lstAllMsgItems, pBufferSendData);
	nLenBeforeZip = pBufferSendData->remainDataLength();//UZDL	
	nMsgPL = 0;
	nMsgPL += nLenBeforeZip;

	if (pMsgPkgSrc->m_bZip)
	{
		pBufferZip = new CBuffer(nLenBeforeZip + BUFFER_LEN_ADDEX); 
		pBufferZip->bufferSizePrepare(nLenBeforeZip + BUFFER_LEN_ADDEX);
		nFunRes = _ZipBuffer(pBufferSendData, pBufferZip);

		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "_ZipBuffer error!");
			DEF_DELETE(pFrameDest);
			DEF_DELETE(pBufferSendData);
			DEF_DELETE(pBufferZip);
			pBufferTmp = NULL;
			nFunRes = -1;				
			return nFunRes;	
		}


		nLenAfterZip = pBufferZip->remainDataLength();
		nMsgPL = 0;
		nMsgPL += nLenAfterZip;
		
		LOG2(SourceInfo, DebugUtil::DebugDebug, 
			"Before Zip DataLen=%d, After Zip DataLen=%d", nLenBeforeZip, nLenAfterZip);

		
		pBufferTmp = pBufferSendData;
		pBufferSendData = pBufferZip;
		pBufferZip = pBufferTmp;
		pBufferTmp = NULL;		
	}
	nMsgPL +=  MSGPACKAGE_TOTAL;
	
	ucMsgETX = MSGPACKAGE_ETX_VALUE;
	//////////////////////////////////////////////////////////////////////////
	 	
	if (NULL != pFrameDest)
	{
		DEF_DELETE(pFrameDest);
	}
	pFrameDest = new CMsgPackageImp();
	pFrameDest->setMsgFrameSTX(&ucMsgSTX, MSGPACKAGE_LEN_STX);	
	pFrameDest->setMsgFramePL(nMsgPL);						  
	pFrameDest->setMsgFramePUUID32(strUUID32);
	pFrameDest->setMsgFramePT(&ucMsgPT, MSGPACKAGE_LEN_PT);
	pFrameDest->setMsgFrameDC(&ucMsgDC, MSGPACKAGE_LEN_DC);
	pFrameDest->setMsgFrameDL(nLenBeforeZip);//nUZDL
	pFrameDest->setMsgFrameETX(&ucMsgETX, MSGPACKAGE_LEN_ETX);
					  
	//must last set frame data
	pFrameDest->setMsgFrameData((const unsigned char* )pBufferSendData->getReadPos(), pBufferSendData->remainDataLength());
		
	
	DEF_DELETE(pBufferSendData);
	DEF_DELETE(pBufferZip);
	pBufferTmp = NULL;							
	return nFunRes;
}




int CComProtocol::processRecvData(CBuffer* pRecvBuffer, lstMSGPKGConT& lstMsgPkg)
{
	int nFunRes = 0;
	bool bHaveOneFrame = false;
	IMsgPackage* pOneFrame = NULL;
	MSGPKG*		 pOneMsgPkg = NULL;

	
// 	bHaveOneFrame = isBufferHaveOneFrame(pRecvBuffer);
// 	if (false == bHaveOneFrame)
// 	{
// 		nFunRes = -1;
// 		return nFunRes;
// 	}
	
	pOneFrame = NULL;
	nFunRes = _GetOneFrameFromBuffer(pRecvBuffer, pOneFrame);
	pRecvBuffer->trim();
	
	pOneMsgPkg = NULL;
	nFunRes = _GetMsgPkgFromFrame(pOneFrame, pOneMsgPkg);
	DEF_DELETE(pOneFrame);
	lstMsgPkg.push_back(pOneMsgPkg);
	
	return nFunRes;
}//



int CComProtocol::_GetMsgPkgFromFrame(IMsgPackage* pMsgFrame, MSGPKG*& pMsgPkg)
{
	int						nFunRes = 0;	
	lstMsgItemConT*			pGetlstMsgItem = NULL;
	IMsgItem*				pGetMsg = NULL;
	lstMsgItemConIterT		iterLst;
	int						nMsgMTCheck = -1;
	int						nMsgMTGet = -1;
	unsigned char					nDC;		   		
	lstMsgItemConT*			pNewlst = NULL;


	if (NULL == pMsgPkg)
	{
		pMsgPkg = new MSGPKG();
	}

	pMsgFrame->parseLstFrameMsgs();
	pMsgPkg->m_strPkgUUID32 = (char *)pMsgFrame->getMsgFramePUUID();
	pMsgPkg->m_nPkgType = *(pMsgFrame->getMsgFramePT());	
	
	nDC = *(pMsgFrame->getMsgFrameDC());
	pMsgPkg->m_bZip = false;
	if (MSGPACKAGE_DC_ZIP == nDC)
	{
		pMsgPkg->m_bZip = true;
	}
	
	pGetlstMsgItem = pMsgFrame->getLstFrameMsgs();
	
	iterLst = pGetlstMsgItem->begin();
	nMsgMTCheck = -1;
	nMsgMTGet = -1;
	pNewlst = NULL;
	while (iterLst != pGetlstMsgItem->end())
	{
		pGetMsg = *iterLst;
		
		nMsgMTGet = *(pGetMsg->getMsgMT());
		if (nMsgMTGet != nMsgMTCheck)
		{
			nMsgMTCheck = nMsgMTGet;
			if (NULL != pNewlst)
			{
				pMsgPkg->m_lstAllMsgItems.push_back(pNewlst);
				pNewlst = NULL;
			}
			pNewlst = new lstMsgItemConT();
		}
		
		if (NULL != pNewlst)
		{	
			pNewlst->push_back(pGetMsg);
		}
		
		pGetMsg = NULL;

		iterLst++;		
	}
	pGetlstMsgItem->clear();
	
	
	if (NULL != pNewlst)
	{
		pMsgPkg->m_lstAllMsgItems.push_back(pNewlst);
		pNewlst = NULL;
	}
	 
	return nFunRes;
}
 

bool CComProtocol::isBufferHaveOneFrame(CBuffer* pRecvBuffer)
{
	bool			bFunRes = false;
	int             nMinBUfferDataLen = 0;
	unsigned char			ucMsgFrameSTX = 0;
	unsigned char			ucszMsgFramePL[MSGPACKAGE_LEN_PL] = {0};
	int				nMsgFramePL = 0;
	int				nRemainData = 0;  
	
	bFunRes = false;
	nMinBUfferDataLen = MSGPACKAGE_LEN_STX + MSGPACKAGE_LEN_PL;
	if (nMinBUfferDataLen >= pRecvBuffer->remainDataLength() )
	{
		bFunRes = false;
		return bFunRes;
	}

	//get MsgPackage STX
	if (!pRecvBuffer->read(&ucMsgFrameSTX, MSGPACKAGE_LEN_STX))
	{
		bFunRes = false;
		return bFunRes;
	}
	//check	 MsgPackage STX
	if ( MSGPACKAGE_STX_VALUE != ucMsgFrameSTX)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"first u_char[%d] in RecvBuffer is not Frame_STX[0xF8]!", ucMsgFrameSTX);
		bFunRes = false;
		return bFunRes;
	}
	
	//get MsgPackage PL
	if (!pRecvBuffer->read(&ucszMsgFramePL, MSGPACKAGE_LEN_PL))//4 char  size
	{
		bFunRes = false;
		return bFunRes;
	}
	//check MsgPackage PL
	nMsgFramePL = 0;
	nMsgFramePL = DEF_INT4KORR(ucszMsgFramePL);
	nRemainData = 0;
	nRemainData = nMsgFramePL - MSGPACKAGE_LEN_PL;
	nRemainData = nRemainData - MSGPACKAGE_LEN_STX;

	if (pRecvBuffer->remainDataLength() < nRemainData)
	{
		pRecvBuffer->moveReadPos(-MSGPACKAGE_LEN_PL);
		pRecvBuffer->moveReadPos(-MSGPACKAGE_LEN_STX);
		bFunRes = false;		
	}	 
	else
	{
		pRecvBuffer->moveReadPos(-MSGPACKAGE_LEN_PL);
		pRecvBuffer->moveReadPos(-MSGPACKAGE_LEN_STX);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "-->recive one frame from socket");
		bFunRes = true;		
	}
	return bFunRes;
}

int CComProtocol::_ZipBuffer(CBuffer* pBufferSrc, CBuffer* pBufferDest)
{
	int				nFunRes = 0;
	int             nLenBeforeZip = 0;
	unsigned long   nLenAfterZip = 0;
	int				nZipRes = 0; 
	
	
	nLenBeforeZip = pBufferSrc->remainDataLength();
	nLenAfterZip = pBufferDest->getSize();	
	//pBufferDest->bufferSizePrepare(nLenAfterZip);
	
	nZipRes = compress((unsigned char*)pBufferDest->getWritePos(), &nLenAfterZip, (const unsigned char*)pBufferSrc->getReadPos(), nLenBeforeZip);
	if (0 != nZipRes)
	{//zip error
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "compress() error! nZipRes=[%d]", nZipRes);	
		nFunRes = -1;
		return nFunRes;		
	}
	else
	{
		pBufferDest->moveWritePos(nLenAfterZip);
		pBufferSrc->clear();		
	}
	
	return nFunRes;
}



int CComProtocol::_UnZipBuffer(CBuffer* pBufferSrc, CBuffer* pBufferDest)
{
	int				nFunRes = 0;
	int             nBeforeUnZip = 0;
	unsigned long   nAfterUnZip = 0;

	nBeforeUnZip = pBufferSrc->remainDataLength(); 
	nAfterUnZip = pBufferDest->getSize();
// 	nAfterUnZip = nBeforeUnZip + BUFFER_LEN_ADDEX;
// 	pBufferDest->bufferSizePrepare(nAfterUnZip); 

	nFunRes = uncompress((unsigned char *)pBufferDest->getWritePos(), &nAfterUnZip, (unsigned char *)pBufferSrc->getReadPos(), pBufferSrc->remainDataLength());
	if (0 != nFunRes)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "zip error!");
		nFunRes = -1;
	}
	else
	{
		pBufferDest->moveWritePos(nAfterUnZip);
	}

	return nFunRes;

}

int CComProtocol::_GetOneFrameFromBuffer(CBuffer* pRecvBuffer, IMsgPackage*& pMsgFrame)
{
	int				nFunRes = 0;
	bool            bHaveOneFrame = false;
	unsigned char	ucMsgFrameSTX = 0;
	unsigned char	ucszMsgFramePL[MSGPACKAGE_LEN_PL] = {0};
	unsigned char	ucszMsgFramePUUID[MSGPACKAGE_LEN_PUUID] = {0};
	std::string     strUUID32; 
	unsigned char	ucMsgFramePT = 0;
	unsigned char	ucMsgFrameDC = 0;
	unsigned char	ucszMsgFrameDL[MSGPACKAGE_LEN_UZDL] = {0};
	unsigned char	ucMsgFrameETX = 0;
	int				nMsgFrameDLValue = 0;
	int				nMsgFramePL = 0;
	
	CBuffer*        pBufferSrc = NULL;
	CBuffer*        pBufferZip = NULL;
	CBuffer*        pBufferTmp = NULL;
	 	
	
// 	bHaveOneFrame = isBufferHaveOneFrame(pRecvBuffer);
// 	if (false == bHaveOneFrame)
// 	{
// 		nFunRes = -1;
// 		return nFunRes;
// 	}
	if (NULL != pMsgFrame)
	{
		DEF_DELETE(pMsgFrame);
		pMsgFrame = NULL;
	}

	//======one Frame ready==========================================
	pMsgFrame = new CMsgPackageImp();
	pRecvBuffer->read(&ucMsgFrameSTX, MSGPACKAGE_LEN_STX);
	pMsgFrame->setMsgFrameSTX((unsigned char *)&ucMsgFrameSTX, MSGPACKAGE_LEN_STX);

	pRecvBuffer->read(ucszMsgFramePL, MSGPACKAGE_LEN_PL);
	pMsgFrame->setMsgFramePLArray((unsigned char *)ucszMsgFramePL, MSGPACKAGE_LEN_PL);
	nMsgFramePL = pMsgFrame->getMsgFramePL();

	pRecvBuffer->read(ucszMsgFramePUUID, MSGPACKAGE_LEN_PUUID);
	pMsgFrame->setMsgFramePUUID16((unsigned char *)ucszMsgFramePUUID, MSGPACKAGE_LEN_PUUID);
	strUUID32.clear();
	strUUID32 =	 (char *)(pMsgFrame->getMsgFramePUUID());

	pRecvBuffer->read(&ucMsgFramePT, MSGPACKAGE_LEN_PT);
	pMsgFrame->setMsgFramePT((unsigned char *)&ucMsgFramePT, MSGPACKAGE_LEN_PT);

	pRecvBuffer->read(&ucMsgFrameDC, MSGPACKAGE_LEN_DC);
	pMsgFrame->setMsgFrameDC((unsigned char *)&ucMsgFrameDC, MSGPACKAGE_LEN_DC);

	pRecvBuffer->read(ucszMsgFrameDL, MSGPACKAGE_LEN_UZDL);
	pMsgFrame->setMsgFrameDLArray((unsigned char *)ucszMsgFrameDL, MSGPACKAGE_LEN_UZDL);
	nMsgFrameDLValue = pMsgFrame->getMsgFrameDL();//src

	pBufferSrc = new CBuffer(nMsgFramePL + BUFFER_LEN_ADDEX);
	pRecvBuffer->read(pBufferSrc->getWritePos(), nMsgFramePL - MSGPACKAGE_TOTAL);
	pBufferSrc->moveWritePos(nMsgFramePL - MSGPACKAGE_TOTAL);

	pRecvBuffer->read(&ucMsgFrameETX,MSGPACKAGE_LEN_ETX);
	pMsgFrame->setMsgFrameETX((unsigned char *)&ucMsgFrameETX, MSGPACKAGE_LEN_ETX);

	if (MSGPACKAGE_DC_ZIP  == ucMsgFrameDC)
	{
		pBufferZip = new CBuffer(nMsgFrameDLValue + BUFFER_LEN_ADDEX);		
		pBufferZip->bufferSizePrepare(nMsgFrameDLValue + BUFFER_LEN_ADDEX); 
		nFunRes = _UnZipBuffer(pBufferSrc, pBufferZip);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "_UnZipBuffer error!");
			DEF_DELETE(pBufferZip);
			DEF_DELETE(pBufferSrc);
			DEF_DELETE(pMsgFrame);
			pBufferTmp = NULL;
			nFunRes = -1; 
			return nFunRes;
		}
		
		LOG2(SourceInfo, DebugUtil::DebugDebug, 
			"Before UnZip DataLen=%d, After UnZip DataLen=%d", 
			pBufferSrc->remainDataLength(), pBufferZip->remainDataLength());

		pBufferSrc->clear();
		
		pBufferTmp = pBufferSrc;
		pBufferSrc = pBufferZip;
		pBufferZip = pBufferTmp;
		pBufferTmp = NULL;
	}	


	//must last set frame data
	pMsgFrame->setMsgFrameData((unsigned char*)pBufferSrc->getReadPos(), pBufferSrc->remainDataLength());
			 
	LOG2(SourceInfo, DebugUtil::DebugDebug, 
		"Recv one Frame [strUUID32=%s][nMsgFramePL=%d]", strUUID32.c_str(), nMsgFramePL);

	DEF_DELETE(pBufferSrc);
	DEF_DELETE(pBufferZip);
	pBufferTmp = NULL;
	
	return nFunRes;
}



NS_END(TA_DB_Sync)



























