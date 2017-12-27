//////////////////////////////////////////////////////////////////////
///		@file		Message.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-19 14:44:41
///
///		@brief	    the interface to encapsulate the message in the  
///                 database.
///                 
//////////////////////////////////////////////////////////////////////
#include "Message.h"
#include "MessageImpl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"

NS_BEGIN(TA_AQ_Lib)

CMessage::CMessage()
{
	m_pMsgImpl = new CMessageImpl();
}

CMessage::~CMessage()
{
	DEF_DELETE(m_pMsgImpl);
}

std::string   CMessage::getMsgID() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getMsgID();
}

void  CMessage::setMsgID(const std::string& strMsgID)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setMsgID(strMsgID);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

ulong64  CMessage::getMsgSeqID() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getMsgSeqID();
}

void  CMessage::setMsgSeqID(ulong64 ulSeqID)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setMsgSeqID(ulSeqID);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int   CMessage::getMsgState() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getMsgState();
}

void  CMessage::setMsgState(int nState)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setMsgState(nState);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int  CMessage::getMsgPriority() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getMsgPriority();
}

void   CMessage::setMsgPriority(int nPriority)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setMsgPriority(nPriority);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int   CMessage::getPayload(const std::string& strKey, std::string& strPayload)
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayload(strKey, strPayload);
}

int  CMessage::getPayload(const std::string& strKey, const char*& pPayload)
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayload(strKey, pPayload);
}

int  CMessage::getPayload(const std::string& strKey, int& nPayloadVal)
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayload(strKey, nPayloadVal);
}

int   CMessage::getPayload(const std::string& strKey, ulong64& n64PayloadVal)
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayload(strKey, n64PayloadVal);
}

int   CMessage::getPayload(std::string& strPayload)
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayload(strPayload);
}

void   CMessage::setPayload(int nPayloadType, const std::string& strKey, const std::string& strData)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayload(nPayloadType, strKey, strData);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void   CMessage::setPayload(const std::string& strKey, const std::string& strPayload)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayload(strKey, strPayload);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void    CMessage::setPayload(const std::string& strKey, int nValue)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayload(strKey, nValue);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void    CMessage::setPayload(const std::string& strKey, ulong64 u64Value)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayload(strKey, u64Value);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void   CMessage::setPayload(const std::string& strPayload)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayload( strPayload );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int   CMessage::getPayloadType() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayloadType();
}

void   CMessage::setPayloadType(int nPayloadType)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setPayloadType( nPayloadType );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void    CMessage::setRecipientList(const LstRectT& rLstRecipient)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setRecipientList( rLstRecipient );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

void   CMessage::setRecipientList(const CAgent& rAgent)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setRecipientList( rAgent );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}


std::string  CMessage::getRecipientList() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getRecipientList();
}


void    CMessage::setSender(const CAgent& rSender)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setSender( rSender );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

CAgent   CMessage::getSender() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getSender();
}

void    CMessage::setQName(const std::string& strQName)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setQName( strQName );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

std::string    CMessage::getQName() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getQName();
}

void   CMessage::setSubCnt(int nSubCnt)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setSubCnt( nSubCnt );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}

int  CMessage::getSubCnt() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getSubCnt();
}

void  CMessage::setOServerID(int nOServerID)
{
	if (m_pMsgImpl)
	{
		m_pMsgImpl->setOServerID( nOServerID );
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
}
int   CMessage::getOServerID() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getOServerID();
}

u_int  CMessage::getSize() const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getSize();
}


u_int  CMessage::getPayloadSize()const
{
	if (!m_pMsgImpl)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	return m_pMsgImpl->getPayloadSize();
}
NS_END(TA_AQ_Lib)