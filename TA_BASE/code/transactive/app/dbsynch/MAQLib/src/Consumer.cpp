//////////////////////////////////////////////////////////////////////
///		@file		Consumer.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-26 17:44:41
///
///		@brief	    the message consumer is get message from the source   
///                 database/local database based on the consumer name
///                 the routine is that the listener listen for messages
///                 for specified consumer and then fetch the message(s) 
///                 from the local database.
//////////////////////////////////////////////////////////////////////
#include "Consumer.h"
#include "ConsumerImpl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"


NS_BEGIN(TA_AQ_Lib)

CConsumer::CConsumer(IDatabase* pDatabase)
{
	m_pConsumerImpl = new CConsumerImpl(pDatabase);
}

CConsumer::CConsumer(IDatabase* pDatabase, const CAgent& rAgent)
{
	m_pConsumerImpl = new CConsumerImpl(pDatabase, rAgent);
}

CConsumer::CConsumer(IDatabase* pDatabase, const std::string& strConsumerName)
{
	m_pConsumerImpl = new CConsumerImpl(pDatabase, strConsumerName);
}

CConsumer::CConsumer(IDatabase* pDatabase, int nPayloadType)
{
	m_pConsumerImpl = new CConsumerImpl(pDatabase, nPayloadType);
}

CConsumer::~CConsumer(void)
{
	DEF_DELETE(m_pConsumerImpl);
}

void  CConsumer::setAgent(const CAgent& rAgent)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setAgent(rAgent);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void  CConsumer::setConsumberName(const std::string& strConName)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setConsumberName(strConName);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string  CConsumer::getConsumberName() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->getConsumberName();
}

void  CConsumer::setDeqMode(DequeueMode mode)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setDeqMode(mode);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

DequeueMode  CConsumer::getDeqMode() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return  static_cast<DequeueMode>(m_pConsumerImpl->getDeqMode());
}

void  CConsumer::setMsgIdToDeq(const std::string& strMsgID)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setMsgIdToDeq(strMsgID);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string CConsumer::getMsgIdToDeq() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->getMsgIdToDeq();
}

void  CConsumer::setPositionOfMsg(Navigation pos)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setPositionOfMsg(pos);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

Navigation CConsumer::getPositionOfMsg() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return  static_cast<Navigation>(m_pConsumerImpl->getPositionOfMsg());
}

void  CConsumer::setVisibility(Visibility op)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setVisibility(op);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

Visibility CConsumer::getVisibility() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return  static_cast<Visibility>(m_pConsumerImpl->getVisibility());
}

void  CConsumer::setQueueName(const std::string& strQueue)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setQueueName(strQueue);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string CConsumer::getQeueuName() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->getQeueuName();
}

void  CConsumer::setPayloadType(int nPayloadType)
{
	if (m_pConsumerImpl)
	{
		m_pConsumerImpl->setPayloadType(nPayloadType);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int   CConsumer::getPayloadType() const
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->getPayloadType();
}

int  CConsumer::deqMsg(lstMsgConT& rLstMsg,  u_int nDeqSize)
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->deqMsg(rLstMsg, nDeqSize);
}

int  CConsumer::deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize, u_int nSubID)
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->deqMsg(rLstMsg, nDeqSize, nSubID);
}

int  CConsumer::getMsgSeqIds(nPairT& nSeqIDPair)
{
	if (!m_pConsumerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pConsumerImpl->getMsgSeqIds(nSeqIDPair);
}

NS_END(TA_AQ_Lib)















