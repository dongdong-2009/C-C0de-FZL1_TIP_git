//////////////////////////////////////////////////////////////////////
///		@file		Producer.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-31 14:44:41
///
///		@brief	    the message producer provides the interfaces to
///                 enq message to database queue. it use the composition 
///                 pattern to implement the class by using class 
///                 CProducerImpl
//////////////////////////////////////////////////////////////////////
#include "Producer.h"
#include "ProducerImpl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"

NS_BEGIN(TA_AQ_Lib)

CProducer::CProducer(IDatabase* pDatabase)
{
	m_pProducerImpl = new CProducerImpl(pDatabase);
}

CProducer::~CProducer(void)
{
	DEF_DELETE(m_pProducerImpl);
}

int   CProducer::enqueueMsg(CMessage* pEnqMsg)
{
	if (!m_pProducerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pProducerImpl->enqueueMsg(pEnqMsg);
}

int   CProducer::enqueueMsg(std::list<CMessage*>& EnqMsgList)
{
	if (!m_pProducerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pProducerImpl->enqueueMsg(EnqMsgList);
}



void  CProducer::setSkipErrorCode(const std::string& strErrorCode)
{
	if (!m_pProducerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pProducerImpl->setSkipErrorCode(strErrorCode);
}

void  CProducer::setEnqMode(int nEnqMode)
{
	if (!m_pProducerImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pProducerImpl->setEnqMode(nEnqMode);
}

NS_END(TA_AQ_Lib)
