//////////////////////////////////////////////////////////////////////
///		@file		Listener.h
///		@author		zhilin,ouyang
///		@date		2012-08-20 14:44:41
///
///		@brief	    the message listener for specific queue table.   
///                 currently the listener only support one subscriber
///                 vs one listener. the relationship is one to one.
///                 when there is a message enqueue in the queue table
///                 for the specific subscriber, the listener will return
///                 otherwise it will always keep listen the queue table for
///                 next coming messages.
//////////////////////////////////////////////////////////////////////
#include "Listener.h"
#include "IDatabase.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"

NS_BEGIN(TA_AQ_Lib)

CListener::CListener(IDatabase* pDatabase)
: m_pDatabase(pDatabase)
, m_nWaitTime(defDefaultListenTime)
, m_ulTimeOut(MSG_LISTEN_NOWAIT)
, m_nSubID(0)
{

}

CListener::CListener(IDatabase* pDatabase, const CAgent& rhs)
: m_pDatabase(pDatabase)
, m_Agent(rhs)
, m_nWaitTime(defDefaultListenTime)
, m_ulTimeOut(MSG_LISTEN_NOWAIT)
, m_nSubID(0)
{

}

CListener::CListener(IDatabase* pDatabase, const std::string& strQName)
: m_pDatabase(pDatabase)
, m_strQName(strQName)
, m_nWaitTime(defDefaultListenTime)
, m_ulTimeOut(MSG_LISTEN_NOWAIT)
, m_nSubID(0)
{

}

CListener::CListener(IDatabase* pDatabase, const std::string& strQName, const CAgent& rhs)
: m_pDatabase(pDatabase)
, m_Agent(rhs)
, m_nWaitTime(defDefaultListenTime)
, m_ulTimeOut(MSG_LISTEN_NOWAIT)
, m_nSubID(0)
, m_strQName(strQName)
{

}

CListener::~CListener(void)
{
	m_pDatabase = NULL;
}

bool  CListener::listen()
{
	bool bRet = false;
	std::string strSubName(m_Agent.getName());
	
	if (!m_pDatabase)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_strQName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d, Message: %s", Err_QName_NotFount, defErrMsg(Err_QName_NotFount));
		return bRet;
	}
	
	if (0 != m_nSubID)
	{
		m_pDatabase->connect();
		bRet = m_pDatabase->listen(m_strQName, m_nSubID);
	}
	else
	{
		if (strSubName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Error Code: %d, Message: %s", Err_SubName_NotFount, defErrMsg(Err_SubName_NotFount));
			return bRet;
		}
		m_pDatabase->connect();
		bRet = m_pDatabase->listen(m_strQName, m_Agent.getName());
	}

	return bRet;
	
}

void    CListener::setAgent(const CAgent& rhs)
{
	m_Agent = rhs;
}

CAgent  CListener::getAgent() const
{
	return m_Agent;
}

void    CListener::setQName(const std::string& strQName)
{
	m_strQName = strQName;
}

std::string CListener::getQName() const
{
	return m_strQName;
}

void    CListener::setSubID(u_int nSubID)
{
	m_nSubID = nSubID;
}

u_int   CListener::getSubID() const
{
	return m_nSubID;
}

void    CListener::setWaitTimeForListen(int nWaitTime)
{
	m_nWaitTime = nWaitTime;
}

int     CListener::getWaitTimeForListen()const
{
	return m_nWaitTime;
}

void    CListener::setListenTimeOut(u_long ulTimeOut)
{
	m_ulTimeOut = ulTimeOut;
}

u_long  CListener::getListenTimeOut()const
{
	return m_ulTimeOut;
}

NS_END(TA_AQ_Lib)














