//////////////////////////////////////////////////////////////////////
///		@file		ConsumerImpl.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-26 10:04:41
///
///		@brief	    the purpose of this class is to implement the class   
///                 Consumer by using the composition pattern.
///                 
//////////////////////////////////////////////////////////////////////
#include "ConsumerImpl.h"
#include "IDatabase.h"
#include "Agent.h"
#include "DBException.h"
#include "Message.h"
#include "UtilityFun.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DatabaseException.h"


NS_BEGIN(TA_AQ_Lib)

CConsumerImpl::CConsumerImpl(IDatabase* pDatabase)
: m_pDatabase(pDatabase)
, m_pAgent(NULL)
, m_pDeqOpt(NULL)
, m_nPayloadType(PAYLOAD_TYPE_OBJ)
{
	m_pAgent  = new CAgent();
	m_pDeqOpt = new MAQ_DEQ_OPT();
}

CConsumerImpl::CConsumerImpl(IDatabase* pDatabase, const CAgent& rAgent)
: m_pDatabase(pDatabase)
, m_pAgent(NULL)
, m_pDeqOpt(NULL)
, m_nPayloadType(PAYLOAD_TYPE_OBJ)
{
	m_pAgent = new CAgent(rAgent);
	m_pDeqOpt = new MAQ_DEQ_OPT();
}

CConsumerImpl::CConsumerImpl(IDatabase* pDatabase, const std::string& strConsumerName)
: m_pDatabase(pDatabase)
, m_pAgent(NULL)
, m_pDeqOpt(NULL)
, m_nPayloadType(PAYLOAD_TYPE_OBJ)
{
	m_pAgent  = new CAgent(strConsumerName);
	m_pDeqOpt = new MAQ_DEQ_OPT();
}

CConsumerImpl::CConsumerImpl(IDatabase* pDatabase, int nPayloadType)
: m_pDatabase(pDatabase)
, m_pAgent(NULL)
, m_pDeqOpt(NULL)
, m_nPayloadType(nPayloadType)
{
	m_pAgent  = new CAgent();
	m_pDeqOpt = new MAQ_DEQ_OPT();
}

CConsumerImpl::~CConsumerImpl(void)
{
	DEF_DELETE(m_pAgent);
	DEF_DELETE(m_pDeqOpt);
	m_pDatabase = NULL;
}

void  CConsumerImpl::setAgent(const CAgent& rAgent)
{
	if (m_pAgent)
	{
		(*m_pAgent) = rAgent;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void  CConsumerImpl::setConsumberName(const std::string& strConName)
{
	if (m_pAgent)
	{
		m_pAgent->setName(strConName);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string  CConsumerImpl::getConsumberName() const
{
	if (!m_pAgent)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	return m_pAgent->getName();
}

void  CConsumerImpl::setDeqMode(DequeueMode mode)
{
	if (m_pDeqOpt)
	{
		m_pDeqOpt->m_nDeqMode = mode;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

DequeueMode  CConsumerImpl::getDeqMode() const
{
	if (!m_pDeqOpt)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	return static_cast<DequeueMode>(m_pDeqOpt->m_nDeqMode);
	
}

void  CConsumerImpl::setMsgIdToDeq(const std::string& strMsgID)
{
	if (m_pDeqOpt)
	{
		m_pDeqOpt->m_strMsgID = strMsgID;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string CConsumerImpl::getMsgIdToDeq() const
{
	if (!m_pDeqOpt)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pDeqOpt->m_strMsgID;
}

void  CConsumerImpl::setPositionOfMsg(Navigation pos)
{
	if (m_pDeqOpt)
	{
		m_pDeqOpt->m_nDeqNav = pos;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

Navigation CConsumerImpl::getPositionOfMsg() const
{
	if (!m_pDeqOpt)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return static_cast<Navigation>(m_pDeqOpt->m_nDeqNav);
}

void  CConsumerImpl::setVisibility(Visibility op)
{
	if (m_pDeqOpt)
	{
		m_pDeqOpt->m_nDeqVis = op;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}
Visibility CConsumerImpl::getVisibility() const
{
	if (!m_pDeqOpt)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return static_cast<Visibility>(m_pDeqOpt->m_nDeqVis);
}

void  CConsumerImpl::setQueueName(const std::string& strQueue)
{
	m_strQueueName = strQueue;
}

std::string CConsumerImpl::getQeueuName() const
{
	return m_strQueueName;
}

int  CConsumerImpl::deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize)
{
	int nRetCode = Ret_Success;
	if (!m_pDeqOpt || !m_pDatabase)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	try
	{
		m_pDeqOpt->m_nDeqFetchSize = nDeqSize;
		m_pDeqOpt->m_strConsumerName = m_pAgent->getName();
		m_pDatabase->connect();
		nRetCode = m_pDatabase->deqMsg(m_strQueueName, m_pDeqOpt, rLstMsg);
		
		if (!rLstMsg.empty())
		{
			m_nPairSeqIDs.first  = (rLstMsg.front())->getMsgSeqID();
			m_nPairSeqIDs.second = (rLstMsg.back())->getMsgSeqID();
		}	
	
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		_FreeDBMsg(rLstMsg);
		nRetCode = e.getNativeCode();
	}
	catch(const TA_Base_Core::DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another node
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );	
		_FreeDBMsg(rLstMsg);
		nRetCode = Ret_Failed;
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		_FreeDBMsg(rLstMsg);
		nRetCode = Ret_Failed;
	}
	
	return nRetCode;
}

int  CConsumerImpl::deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize, u_int nSubID)
{
	int nRetCode = Ret_Success;

	if (!m_pDeqOpt || !m_pDatabase)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (!rLstMsg.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_DEQ_Message_Lst_NotNull, defErrMsg(Err_DEQ_Message_Lst_NotNull));
		_FreeDBMsg(rLstMsg);
	}

	try
	{
		m_pDeqOpt->m_nSubID = nSubID;
		m_pDeqOpt->m_nDeqFetchSize = nDeqSize;	
		m_pDeqOpt->m_nPayloadType  = m_nPayloadType;
		m_pDatabase->connect();
		nRetCode = m_pDatabase->deqMsg(m_strQueueName, m_pDeqOpt, rLstMsg);
		DEF_ERROR_CHECK(!nRetCode);
		DEF_ERROR_CHECK(!rLstMsg.empty());

		m_nPairSeqIDs.first  = (rLstMsg.front())->getMsgSeqID();
		m_nPairSeqIDs.second = (rLstMsg.back())->getMsgSeqID();

	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		_FreeDBMsg(rLstMsg);
		nRetCode = e.getNativeCode();
	}
	catch(const TA_Base_Core::DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another node
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );	
		_FreeDBMsg(rLstMsg);
		nRetCode = Ret_Failed;
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		_FreeDBMsg(rLstMsg);
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CConsumerImpl::getMsgSeqIds(nPairT& nSeqIDPair)
{
	int nRet = Ret_Success;

	nSeqIDPair.first  = m_nPairSeqIDs.first;
	nSeqIDPair.second = m_nPairSeqIDs.second;

	return nRet;
}


void CConsumerImpl::setPayloadType( int nPayloadType )
{
	m_nPayloadType = nPayloadType;
}

int CConsumerImpl::getPayloadType() const
{
   return m_nPayloadType;
}

void  CConsumerImpl::_FreeDBMsg(LstMsgConT& crDBData)
{
	LstMsgConT::iterator  iter = crDBData.begin();
	while (iter != crDBData.end())
	{
		DEF_DELETE(*iter);

		iter++;
	}

	crDBData.clear();
}


NS_END(TA_AQ_Lib)














