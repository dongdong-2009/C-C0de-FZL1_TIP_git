//////////////////////////////////////////////////////////////////////
///		@file		MsgFrameImp.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-20 14:44:41
///
///		@brief	    implementation body of class CMySQLMessage.  
///                  
///                 
///                 
//////////////////////////////////////////////////////////////////////
#include "MsgPackageImp.h"

#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"

#include "IMsgItem.h"
#include "MsgItemImp.h"

NS_BEGIN(TA_DB_Sync)


CMsgPackageImp::CMsgPackageImp()
{
	m_pMsgFrameHeader = NULL;
	m_pMsgFrameHeader = new MSG_PACKAGE_HEADER();
	m_pLstMessages = NULL;
	m_pLstMessages = new std::list<IMsgItem*>();
	m_pLstMessages->clear();
}

CMsgPackageImp::~CMsgPackageImp()
{
	DEF_DELETE_ARRAY(m_pMsgFrameHeader->pMsgFrameStartPtr);
	DEF_DELETE(m_pMsgFrameHeader);
	freeLstFrameMsgs();
	DEF_DELETE(m_pLstMessages);
}
CMsgPackageImp::CMsgPackageImp( const CMsgPackageImp& rhs )
{
	m_pMsgFrameHeader = NULL;
	m_pMsgFrameHeader = new MSG_PACKAGE_HEADER();
	m_pLstMessages = NULL;
	m_pLstMessages = new std::list<IMsgItem*>();
	m_pLstMessages->clear();
	
	m_pMsgFrameHeader->ucMsgFrameSTX  = rhs.m_pMsgFrameHeader->ucMsgFrameSTX;
	memcpy(m_pMsgFrameHeader->ucszMsgFramePL, rhs.m_pMsgFrameHeader->ucszMsgFramePL, MSGPACKAGE_LEN_PL);
	m_pMsgFrameHeader->ucMsgFramePT = rhs.m_pMsgFrameHeader->ucMsgFramePT;
	m_pMsgFrameHeader->ucMsgFrameDC     = rhs.m_pMsgFrameHeader->ucMsgFrameDC;
	memcpy(m_pMsgFrameHeader->ucszMsgFrameDL, rhs.m_pMsgFrameHeader->ucszMsgFrameDL, MSGPACKAGE_LEN_UZDL);
	m_pMsgFrameHeader->ucszMsgFrameDLValue = rhs.m_pMsgFrameHeader->ucszMsgFrameDLValue; 
	m_pMsgFrameHeader->nMsgFrameDataLen = rhs.m_pMsgFrameHeader->nMsgFrameDataLen;
	DEF_DELETE_ARRAY(m_pMsgFrameHeader->pMsgFrameStartPtr);
	if ( m_pMsgFrameHeader->nMsgFrameDataLen )
	{
		int nMemSize = m_pMsgFrameHeader->nMsgFrameDataLen + MSGPACKAGE_TOTAL + 1;
		m_pMsgFrameHeader->pMsgFrameStartPtr = new unsigned char[nMemSize];
		memset(m_pMsgFrameHeader->pMsgFrameStartPtr, 0, nMemSize);
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr, rhs.m_pMsgFrameHeader->pMsgFrameStartPtr, nMemSize - 1);
	}	
}
CMsgPackageImp& CMsgPackageImp::operator=( const CMsgPackageImp& rhs )
{
	if (this != &rhs)
	{
		m_pMsgFrameHeader->ucMsgFrameSTX  = rhs.m_pMsgFrameHeader->ucMsgFrameSTX;
		memcpy(m_pMsgFrameHeader->ucszMsgFramePL, rhs.m_pMsgFrameHeader->ucszMsgFramePL, MSGPACKAGE_LEN_PL);
		m_pMsgFrameHeader->ucMsgFramePT = rhs.m_pMsgFrameHeader->ucMsgFramePT;
		m_pMsgFrameHeader->ucMsgFrameDC     = rhs.m_pMsgFrameHeader->ucMsgFrameDC;
		memcpy(m_pMsgFrameHeader->ucszMsgFrameDL, rhs.m_pMsgFrameHeader->ucszMsgFrameDL, MSGPACKAGE_LEN_UZDL);
		m_pMsgFrameHeader->ucszMsgFrameDLValue = rhs.m_pMsgFrameHeader->ucszMsgFrameDLValue; 
		m_pMsgFrameHeader->nMsgFrameDataLen = rhs.m_pMsgFrameHeader->nMsgFrameDataLen;
		DEF_DELETE_ARRAY(m_pMsgFrameHeader->pMsgFrameStartPtr);
		if ( m_pMsgFrameHeader->nMsgFrameDataLen )
		{
			int nMemSize = m_pMsgFrameHeader->nMsgFrameDataLen + MSGPACKAGE_TOTAL + 1;
			m_pMsgFrameHeader->pMsgFrameStartPtr = new unsigned char[nMemSize];
			memset(m_pMsgFrameHeader->pMsgFrameStartPtr, 0, nMemSize);
			memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr, rhs.m_pMsgFrameHeader->pMsgFrameStartPtr, nMemSize - 1);
		}	
	}
	return (*this);
}

void CMsgPackageImp::setMsgFrameSTX( const unsigned char* pSrc, int nLen )
{
	if (nLen > 0)
	{
		m_pMsgFrameHeader->ucMsgFrameSTX = ((unsigned char*)pSrc)[0];
	}
}

unsigned char* CMsgPackageImp::getMsgFrameSTX() const
{
	return (unsigned char *)&(m_pMsgFrameHeader->ucMsgFrameSTX);
}

void CMsgPackageImp::setMsgFramePLArray( const unsigned char* pSrc, int nLen )
{
	if (MSGPACKAGE_LEN_PL == nLen)
	{
		memset(m_pMsgFrameHeader->ucszMsgFramePL, 0, MSGPACKAGE_LEN_PL);
		memcpy(m_pMsgFrameHeader->ucszMsgFramePL, pSrc, MSGPACKAGE_LEN_PL);

		m_pMsgFrameHeader->ucszMsgFramePLValue = DEF_INT4KORR(m_pMsgFrameHeader->ucszMsgFramePL);


	}
}

void CMsgPackageImp::setMsgFramePL(int nMsgPageLen )
{
	memset(m_pMsgFrameHeader->ucszMsgFramePL, 0, MSGPACKAGE_LEN_PL);

	if (nMsgPageLen >= 0 )
	{
		m_pMsgFrameHeader->ucszMsgFramePLValue = nMsgPageLen;
		DEF_INT4STORE(m_pMsgFrameHeader->ucszMsgFramePL, m_pMsgFrameHeader->ucszMsgFramePLValue);


	}
}

int CMsgPackageImp::getMsgFramePL() const
{
	return (m_pMsgFrameHeader->ucszMsgFramePLValue);
}

void  CMsgPackageImp::setMsgFramePUUID32(const std::string& strUUID32)
{
	memset(m_pMsgFrameHeader->ucszMsgFramePUUID, 0, MSGPACKAGE_LEN_PUUID);
	memset(m_pMsgFrameHeader->ucszMsgFramePUUIDValue, 0, MSGPACKAGE_PUUID_BUFFER_LEN);

	if (MSGPACKAGE_PUUID_VALUE_LEN == strUUID32.size())
	{ 
		CUtilityFun::strUUID32ToUChar16(strUUID32, m_pMsgFrameHeader->ucszMsgFramePUUID);
		memcpy(m_pMsgFrameHeader->ucszMsgFramePUUIDValue, strUUID32.c_str(), strUUID32.size());
	}
}


void  CMsgPackageImp::setMsgFramePUUID16(const unsigned char* pSrc, int nLen)
{

	memset(m_pMsgFrameHeader->ucszMsgFramePUUID, 0, MSGPACKAGE_LEN_PUUID);
	memset(m_pMsgFrameHeader->ucszMsgFramePUUIDValue, 0, MSGPACKAGE_PUUID_BUFFER_LEN);

	if (MSGPACKAGE_LEN_PUUID == nLen)
	{ 
		memcpy(m_pMsgFrameHeader->ucszMsgFramePUUID, pSrc, nLen);

		std::string strUUID32; 
		CUtilityFun::uChar16TostrUUID32((unsigned char*)pSrc, strUUID32);

		memcpy(m_pMsgFrameHeader->ucszMsgFramePUUIDValue, strUUID32.c_str(), strUUID32.length());

	}
}



unsigned char* CMsgPackageImp::getMsgFramePUUID() const
{
	return (m_pMsgFrameHeader->ucszMsgFramePUUIDValue);
}




void CMsgPackageImp::setMsgFramePT( const unsigned char* pSrc, int nLen )
{
	if (nLen > 0)
	{
		m_pMsgFrameHeader->ucMsgFramePT = ((unsigned char*)pSrc)[0];

	}
}

unsigned char* CMsgPackageImp::getMsgFramePT() const
{
	return (unsigned char *)&(m_pMsgFrameHeader->ucMsgFramePT);
}

void CMsgPackageImp::setMsgFrameDC( const unsigned char* pSrc, int nLen )
{
	if (nLen > 0)
	{
		m_pMsgFrameHeader->ucMsgFrameDC = ((unsigned char*)pSrc)[0];
	}
}

unsigned char* CMsgPackageImp::getMsgFrameDC() const
{
	return (unsigned char *)&(m_pMsgFrameHeader->ucMsgFrameDC);
}

void CMsgPackageImp::setMsgFrameDLArray( const unsigned char* pSrc, int nLen )
{
	memset(m_pMsgFrameHeader->ucszMsgFrameDL, 0, MSGPACKAGE_LEN_UZDL);

	if (nLen > 0 && nLen <= MSGPACKAGE_LEN_UZDL)
	{
		memcpy(m_pMsgFrameHeader->ucszMsgFrameDL, pSrc, nLen);
		m_pMsgFrameHeader->ucszMsgFrameDLValue = DEF_INT4KORR(m_pMsgFrameHeader->ucszMsgFrameDL);
	}
}
void  CMsgPackageImp::setMsgFrameDL(int nFrameDataSrclen)
{
	memset(m_pMsgFrameHeader->ucszMsgFrameDL, 0, MSGPACKAGE_LEN_UZDL);
	m_pMsgFrameHeader->ucszMsgFrameDLValue = 0;
	if (nFrameDataSrclen > 0)
	{
		m_pMsgFrameHeader->ucszMsgFrameDLValue = nFrameDataSrclen;

		DEF_INT4STORE(m_pMsgFrameHeader->ucszMsgFrameDL, m_pMsgFrameHeader->ucszMsgFrameDLValue);

	}


}
int CMsgPackageImp::getMsgFrameDL() const
{
	return (m_pMsgFrameHeader->ucszMsgFrameDLValue);
}

void CMsgPackageImp::setMsgFrameData( const unsigned char* pSrc, int nLen )
{
	int nMemSize = 0;
	int nIndex = 0;
	DEF_DELETE_ARRAY(m_pMsgFrameHeader->pMsgFrameStartPtr);
	m_pMsgFrameHeader->nMsgFrameDataLen = 0;

	if (nLen >= 0)
	{
		m_pMsgFrameHeader->nMsgFrameDataLen = nLen;

		nMemSize = 0;

		nMemSize += MSGPACKAGE_TOTAL;
		nMemSize += nLen;
		nMemSize += 1;

		m_pMsgFrameHeader->pMsgFrameStartPtr = new unsigned char[nMemSize];
		memset(m_pMsgFrameHeader->pMsgFrameStartPtr, 0, nMemSize);

		//STX PL PT DC DL data ETX
		nIndex = 0;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, &(m_pMsgFrameHeader->ucMsgFrameSTX), MSGPACKAGE_LEN_STX);
		nIndex += MSGPACKAGE_LEN_STX;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, (m_pMsgFrameHeader->ucszMsgFramePL), MSGPACKAGE_LEN_PL);
		nIndex += MSGPACKAGE_LEN_PL;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, (m_pMsgFrameHeader->ucszMsgFramePUUID), MSGPACKAGE_LEN_PUUID);
		nIndex += MSGPACKAGE_LEN_PUUID;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, &(m_pMsgFrameHeader->ucMsgFramePT), MSGPACKAGE_LEN_PT);
		nIndex += MSGPACKAGE_LEN_PT;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, &(m_pMsgFrameHeader->ucMsgFrameDC), MSGPACKAGE_LEN_DC);
		nIndex += MSGPACKAGE_LEN_DC;
		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, (m_pMsgFrameHeader->ucszMsgFrameDL), MSGPACKAGE_LEN_UZDL);
		nIndex += MSGPACKAGE_LEN_UZDL;

		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, pSrc, nLen);
		nIndex += nLen;

		memcpy(m_pMsgFrameHeader->pMsgFrameStartPtr + nIndex, &(m_pMsgFrameHeader->ucMsgFrameETX), MSGPACKAGE_LEN_ETX);
		nIndex += MSGPACKAGE_LEN_ETX;


	}


}

unsigned char* CMsgPackageImp::getMsgFrameData() const
{
	return (m_pMsgFrameHeader->pMsgFrameStartPtr + MSGPACKAGE_TOTAL - MSGPACKAGE_LEN_ETX);

}
int CMsgPackageImp::getMsgFrameDataLen() const
{
	return (m_pMsgFrameHeader->nMsgFrameDataLen);

}

void CMsgPackageImp::setMsgFrameETX( const unsigned char* pSrc, int nLen )
{
	if (nLen > 0)
	{
		m_pMsgFrameHeader->ucMsgFrameETX = ((unsigned char*)pSrc)[0];
	}
}

unsigned char* CMsgPackageImp::getMsgFrameETX() const
{
	return (unsigned char *)&(m_pMsgFrameHeader->ucMsgFrameETX);

}

unsigned char* CMsgPackageImp::getFrameStartPtr() const
{
	return (m_pMsgFrameHeader->pMsgFrameStartPtr);
}

std::list<IMsgItem*>* CMsgPackageImp::getLstFrameMsgs()  
{
	return m_pLstMessages;
}


int  CMsgPackageImp::freeLstFrameMsgs()   
{
	int nFunRes = 0;
	
	std::list<IMsgItem*>::iterator  iterLst;
	IMsgItem*						pGetMsg = NULL;
	
	if (NULL == m_pLstMessages)
	{
		nFunRes = -1;
		return nFunRes;
	}	
	
	iterLst = m_pLstMessages->begin();
	while (iterLst != m_pLstMessages->end())
	{
		pGetMsg = *iterLst;

		DEF_DELETE(pGetMsg);
		pGetMsg = NULL;
		
		iterLst++;		
	}
	m_pLstMessages->clear();
	
	pGetMsg = NULL;
	return nFunRes;
}

int CMsgPackageImp::parseLstFrameMsgs()  
{
	int				nFunRes = 0;
	unsigned char			ucMsgMT = 0;
	unsigned char*			pBuffer = NULL;		
	unsigned char*			writepos = NULL;	
	unsigned char*			readpos = NULL;
	int				nCount = 0;
	int             nFrameDL = 0;
	
	if (NULL == m_pMsgFrameHeader->pMsgFrameStartPtr || NULL == m_pLstMessages)
	{
		nFunRes = -1;
		return nFunRes;
	}

	pBuffer = getMsgFrameData();
	nFrameDL = getMsgFrameDL();
	writepos = pBuffer + nFrameDL;
	readpos = pBuffer;
	freeLstFrameMsgs();

	while ( readpos < writepos )
	{
		ucMsgMT = 0;
		ucMsgMT = readpos[0];

		IMsgItem* pMsg = NULL;
		pMsg = new CMsgItemImp();
		pMsg->setMsgMT((unsigned char *)readpos, MSGITEM_LEN_MT);
		readpos += MSGITEM_LEN_MT;
		nCount += MSGITEM_LEN_MT;

		pMsg->setMsgQN((unsigned char *)readpos, MSGITEM_LEN_QN);
		readpos += MSGITEM_LEN_QN;
		nCount += MSGITEM_LEN_QN;

		pMsg->setMsgOMI((unsigned char *)readpos, MSGITEM_LEN_OMI);
		readpos += MSGITEM_LEN_OMI;
		nCount += MSGITEM_LEN_OMI;
		
		pMsg->setOServerIDArray((unsigned char *)readpos, MSGITEM_LEN_OServerID);
		readpos += MSGITEM_LEN_OServerID;
		nCount += MSGITEM_LEN_OServerID;

		pMsg->setMsgSID((unsigned char *)readpos, MSGITEM_LEN_SID);
		readpos += MSGITEM_LEN_SID;
		nCount += MSGITEM_LEN_SID;

		pMsg->setMsgPR((unsigned char *)readpos, MSGITEM_LEN_PR);
		readpos += MSGITEM_LEN_PR;
		nCount += MSGITEM_LEN_PR;
		
		pMsg->setMsgPYTP((unsigned char *)readpos, MSGITEM_LEN_PYTP);
		readpos += MSGITEM_LEN_PYTP;
		nCount += MSGITEM_LEN_PYTP;


		pMsg->setMsgDLArray((unsigned char *)readpos, MSGITEM_LEN_MDL);
		readpos += MSGITEM_LEN_MDL;
		nCount += MSGITEM_LEN_MDL;

		pMsg->setMsgData((unsigned char *)readpos, pMsg->getMsgDL());
		readpos += pMsg->getMsgDL();
		nCount += pMsg->getMsgDL();

		m_pLstMessages->push_back(pMsg);
		pMsg = NULL;
	}

	return nFunRes;
}


unsigned char	CMsgPackageImp::getMsgType() 
{
   return m_ucMsgType;
}
int		CMsgPackageImp::setMsgType(unsigned char ucMsgType)   
{
	int nFunRes = 0;
	m_ucMsgType =   ucMsgType;
	return nFunRes;
}






NS_END(TA_DB_Sync)













