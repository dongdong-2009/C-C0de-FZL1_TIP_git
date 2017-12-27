//////////////////////////////////////////////////////////////////////
///		@file		MySQLMessage.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-20 14:44:41
///
///		@brief	    implementation body of class CMySQLMessage.  
///                  
///                 
///                 
//////////////////////////////////////////////////////////////////////
#include "MsgItemImp.h"	
#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;


NS_BEGIN(TA_DB_Sync)



CMsgItemImp::CMsgItemImp()
{
	m_pMsgHeader = new MSGITEM_HEADER();
}


CMsgItemImp::~CMsgItemImp()
{
	DEF_DELETE_ARRAY(m_pMsgHeader->pFrameStartPtr);
	m_pMsgHeader->pMsgPos = NULL;
	DEF_DELETE(m_pMsgHeader);

}
CMsgItemImp::CMsgItemImp( const CMsgItemImp& rhs )
{
	m_pMsgHeader = new MSGITEM_HEADER();

	m_pMsgHeader->ucMsgMT  = rhs.m_pMsgHeader->ucMsgMT;
	memcpy(m_pMsgHeader->ucszMsgQN, rhs.m_pMsgHeader->ucszMsgQN, MSGITEM_LEN_QN);
	memcpy(m_pMsgHeader->ucszMsgOMI, rhs.m_pMsgHeader->ucszMsgOMI, MSGITEM_LEN_OMI);
	memcpy(m_pMsgHeader->ucszMsgOServerID, rhs.m_pMsgHeader->ucszMsgOServerID, MSGITEM_LEN_OServerID);
	m_pMsgHeader->ucszMsgOServerIDValue =   rhs.m_pMsgHeader->ucszMsgOServerIDValue;

	m_pMsgHeader->ucPriority = rhs.m_pMsgHeader->ucPriority;
	memcpy(m_pMsgHeader->ucszMsgDL, rhs.m_pMsgHeader->ucszMsgDL, MSGITEM_LEN_MDL);

	DEF_DELETE_ARRAY(m_pMsgHeader->pFrameStartPtr);
	m_pMsgHeader->pMsgPos = NULL;
	m_pMsgHeader->ucszMsgDLValue = rhs.m_pMsgHeader->ucszMsgDLValue;
	
	
	memcpy(m_pMsgHeader->ucszMsgQNValue, rhs.m_pMsgHeader->ucszMsgQNValue, MSGITEM_QN_VALUE_BUFFER_LEN);
	memcpy(m_pMsgHeader->ucszMsgOMIValue, rhs.m_pMsgHeader->ucszMsgOMIValue, MSGITEM_OMI_VALUE_BUFFER_LEN);


	if ( m_pMsgHeader->ucszMsgDLValue )
	{
		int nMemLen = m_pMsgHeader->ucszMsgDLValue  + MSGITEM_LEN_TOTAL + 1;
		m_pMsgHeader->pFrameStartPtr = new unsigned char[nMemLen];
		memset(m_pMsgHeader->pFrameStartPtr, 0, nMemLen);
		memcpy(m_pMsgHeader->pFrameStartPtr, rhs.m_pMsgHeader->pFrameStartPtr, nMemLen - 1);
	}

}
CMsgItemImp& CMsgItemImp::operator=( const CMsgItemImp& rhs )
{
	if (this != &rhs)
	{
		m_pMsgHeader->ucMsgMT  = rhs.m_pMsgHeader->ucMsgMT;
		memcpy(m_pMsgHeader->ucszMsgQN, rhs.m_pMsgHeader->ucszMsgQN, MSGITEM_LEN_QN);
		memcpy(m_pMsgHeader->ucszMsgOMI, rhs.m_pMsgHeader->ucszMsgOMI, MSGITEM_LEN_OMI);
		memcpy(m_pMsgHeader->ucszMsgOServerID, rhs.m_pMsgHeader->ucszMsgOServerID, MSGITEM_LEN_OServerID);
		m_pMsgHeader->ucszMsgOServerIDValue =   rhs.m_pMsgHeader->ucszMsgOServerIDValue;

		m_pMsgHeader->ucPriority = rhs.m_pMsgHeader->ucPriority;
		memcpy(m_pMsgHeader->ucszMsgDL, rhs.m_pMsgHeader->ucszMsgDL, MSGITEM_LEN_MDL);

		DEF_DELETE_ARRAY(m_pMsgHeader->pFrameStartPtr);
		m_pMsgHeader->pMsgPos = NULL;

		m_pMsgHeader->ucszMsgDLValue = rhs.m_pMsgHeader->ucszMsgDLValue;
		memcpy(m_pMsgHeader->ucszMsgQNValue, rhs.m_pMsgHeader->ucszMsgQNValue, MSGITEM_QN_VALUE_BUFFER_LEN);
		memcpy(m_pMsgHeader->ucszMsgOMIValue, rhs.m_pMsgHeader->ucszMsgOMIValue, MSGITEM_OMI_VALUE_BUFFER_LEN);

		if ( m_pMsgHeader->ucszMsgDLValue )
		{
			int nMemLen = m_pMsgHeader->ucszMsgDLValue  + MSGITEM_LEN_TOTAL + 1;
			m_pMsgHeader->pFrameStartPtr = new unsigned char[nMemLen];
			memset(m_pMsgHeader->pFrameStartPtr, 0, nMemLen);
			memcpy(m_pMsgHeader->pFrameStartPtr, rhs.m_pMsgHeader->pFrameStartPtr, nMemLen - 1);
			m_pMsgHeader->pMsgPos = m_pMsgHeader->pFrameStartPtr + MSGITEM_LEN_TOTAL;

		}
	}
	return (*this);
}


void CMsgItemImp::setMsgMT( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"NULL == pSrc" );
		return;
	}

	if (nLen > 0)
	{
		m_pMsgHeader->ucMsgMT = ((unsigned char*)pSrc)[0];
	}
}

unsigned char* CMsgItemImp::getMsgMT() const
{
	return (unsigned char *)&(m_pMsgHeader->ucMsgMT);
}

void CMsgItemImp::setMsgQN( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	memset(m_pMsgHeader->ucszMsgQN, 0, MSGITEM_LEN_QN);

	if (nLen > 0 && nLen <= MSGITEM_LEN_QN)
	{
		memcpy(m_pMsgHeader->ucszMsgQN, pSrc, nLen);
	}
	
	memset(m_pMsgHeader->ucszMsgQNValue, 0, MSGITEM_QN_VALUE_BUFFER_LEN);

	memcpy(m_pMsgHeader->ucszMsgQNValue, m_pMsgHeader->ucszMsgQN, MSGITEM_LEN_QN);


}

unsigned char* CMsgItemImp::getMsgQN() const
{
	return (unsigned char *)m_pMsgHeader->ucszMsgQNValue;
}

void CMsgItemImp::setMsgOMI( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	memset(m_pMsgHeader->ucszMsgOMI, 0, MSGITEM_LEN_OMI);

	if (nLen > 0 && nLen <= MSGITEM_LEN_OMI)
	{
		memcpy(m_pMsgHeader->ucszMsgOMI, pSrc, nLen);
	}
	
	memset(m_pMsgHeader->ucszMsgOMIValue, 0, MSGITEM_OMI_VALUE_BUFFER_LEN);

	memcpy(m_pMsgHeader->ucszMsgOMIValue, m_pMsgHeader->ucszMsgOMI, MSGITEM_LEN_OMI);

}

unsigned char* CMsgItemImp::getMsgOMI() const
{
	return (unsigned char *)m_pMsgHeader->ucszMsgOMIValue;
}

void  CMsgItemImp::setMsgSID(ulong64 nMsgSID)
{
	memset(m_pMsgHeader->ucszMsgSID, 0, MSGITEM_LEN_SID);

	m_pMsgHeader->uInt64SIDValue = nMsgSID;

	DEF_INT8STORE(m_pMsgHeader->ucszMsgSID, m_pMsgHeader->uInt64SIDValue);

}
void  CMsgItemImp::setMsgSID(const unsigned char* pSrc, int nLen)
{
	if (MSGITEM_LEN_SID == nLen)
	{
		memset(m_pMsgHeader->ucszMsgSID, 0, MSGITEM_LEN_SID);
		memcpy(m_pMsgHeader->ucszMsgSID, pSrc, MSGITEM_LEN_SID);

		m_pMsgHeader->uInt64SIDValue = DEF_INT8KORR(m_pMsgHeader->ucszMsgSID);
		
	}	

}
ulong64 CMsgItemImp::getMsgSID()const
{
	return m_pMsgHeader->uInt64SIDValue;
}




void CMsgItemImp::setOServerIDArray( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	if (MSGITEM_LEN_OServerID == nLen)
	{
		memset(m_pMsgHeader->ucszMsgOServerID, 0, MSGITEM_LEN_OServerID);
		memcpy(m_pMsgHeader->ucszMsgOServerID, pSrc, MSGITEM_LEN_OServerID);

		m_pMsgHeader->ucszMsgOServerIDValue = DEF_INT4KORR(m_pMsgHeader->ucszMsgOServerID);


	}


}
void CMsgItemImp::setMsgOServerID( int nOServerID )
{
	memset(m_pMsgHeader->ucszMsgOServerID, 0, MSGITEM_LEN_OServerID);

	m_pMsgHeader->ucszMsgOServerIDValue = nOServerID;

	DEF_INT4STORE(m_pMsgHeader->ucszMsgOServerID, m_pMsgHeader->ucszMsgOServerIDValue);

}

int CMsgItemImp::getMsgOServerID() const
{
	return m_pMsgHeader->ucszMsgOServerIDValue;
}

void CMsgItemImp::setMsgPR( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	if (nLen > 0)
	{
		m_pMsgHeader->ucPriority = ((unsigned char*)pSrc)[0];
	}
}

unsigned char* CMsgItemImp::getMsgPR() const
{
	return (unsigned char *)&(m_pMsgHeader->ucPriority);
}


void  CMsgItemImp::setMsgPYTP(const unsigned char* pSrc, int nLen)
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	if (nLen > 0)
	{
		m_pMsgHeader->ucPYTP = ((unsigned char*)pSrc)[0];
	}
}
unsigned char* CMsgItemImp::getMsgPYTP()const
{
	return (unsigned char *)&(m_pMsgHeader->ucPYTP); 
}


void CMsgItemImp::setMsgDLArray( const unsigned char* pSrc, int nLen )
{
	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == pSrc");
		return;
	}

	if (MSGITEM_LEN_MDL == nLen)
	{
		memset(m_pMsgHeader->ucszMsgDL, 0, MSGITEM_LEN_MDL);
		memcpy(m_pMsgHeader->ucszMsgDL, pSrc, MSGITEM_LEN_MDL);

		m_pMsgHeader->ucszMsgDLValue = DEF_INT4KORR(m_pMsgHeader->ucszMsgDL);

		
	}
	

}
void CMsgItemImp::setMsgDL( int nMsgDL )
{
	memset(m_pMsgHeader->ucszMsgDL, 0, MSGITEM_LEN_MDL);

	m_pMsgHeader->ucszMsgDLValue = nMsgDL;

	DEF_INT4STORE(m_pMsgHeader->ucszMsgDL, m_pMsgHeader->ucszMsgDLValue);

}

int CMsgItemImp::getMsgDL() const
{
	return m_pMsgHeader->ucszMsgDLValue;
}

void CMsgItemImp::setMsgData( const unsigned char* pSrc, int nLen )
{

	int nMemSize = 0;
	int nIndex = 0;

	if (NULL == pSrc)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"NULL == pSrc");
		return;
	}

	DEF_DELETE_ARRAY(m_pMsgHeader->pFrameStartPtr);
	m_pMsgHeader->pMsgPos = NULL;


	if (nLen > 0)
	{
		nMemSize = 0;
		
		nMemSize += MSGITEM_LEN_TOTAL;
		nMemSize += nLen;
		nMemSize += 1;

		m_pMsgHeader->pFrameStartPtr = new unsigned char[nMemSize];
		memset(m_pMsgHeader->pFrameStartPtr, 0, nMemSize);

		//MT  QN OMI PR  MDL  MData
		nIndex = 0;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, &(m_pMsgHeader->ucMsgMT), MSGITEM_LEN_MT);
		nIndex += MSGITEM_LEN_MT;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, (m_pMsgHeader->ucszMsgQN), MSGITEM_LEN_QN);
		nIndex += MSGITEM_LEN_QN;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, (m_pMsgHeader->ucszMsgOMI), MSGITEM_LEN_OMI);
		nIndex += MSGITEM_LEN_OMI;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, (m_pMsgHeader->ucszMsgOServerID), MSGITEM_LEN_OServerID);
		nIndex += MSGITEM_LEN_OServerID;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, (m_pMsgHeader->ucszMsgSID), MSGITEM_LEN_SID);
		nIndex += MSGITEM_LEN_SID;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, &(m_pMsgHeader->ucPriority), MSGITEM_LEN_PR);
		nIndex += MSGITEM_LEN_PR;
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, &(m_pMsgHeader->ucPYTP), MSGITEM_LEN_PYTP);
		nIndex += MSGITEM_LEN_PYTP;
		setMsgDL(nLen);
		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, (m_pMsgHeader->ucszMsgDL), MSGITEM_LEN_MDL);
		nIndex += MSGITEM_LEN_MDL;

		memcpy(m_pMsgHeader->pFrameStartPtr + nIndex, pSrc, nLen);
		m_pMsgHeader->pMsgPos = m_pMsgHeader->pFrameStartPtr + MSGITEM_LEN_TOTAL;

		

	}
}

unsigned char* CMsgItemImp::getMsgData() const
{
	m_pMsgHeader->pMsgPos = m_pMsgHeader->pFrameStartPtr + MSGITEM_LEN_TOTAL;
	return (m_pMsgHeader->pFrameStartPtr + MSGITEM_LEN_TOTAL);//
}

unsigned char* CMsgItemImp::getMsgStartPtr() const
{	
	return m_pMsgHeader->pFrameStartPtr;
}

int CMsgItemImp::getMsgAllLen() const
{
	int nMsgALlLen = 0;
	nMsgALlLen = m_pMsgHeader->ucszMsgDLValue + MSGITEM_LEN_TOTAL;
	return nMsgALlLen;
}





NS_END(TA_DB_Sync)



















