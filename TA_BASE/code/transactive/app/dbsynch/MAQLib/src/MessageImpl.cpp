//////////////////////////////////////////////////////////////////////
///		@file		MessageImpl.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-19 14:44:41
///
///		@brief	    the purpose of this class is to implement the class   
///                 CMessage by using the composition pattern.
///                 
//////////////////////////////////////////////////////////////////////
#include "MessageImpl.h"
#include "UtilityFun.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"

NS_BEGIN(TA_AQ_Lib)

static const std::string  s_strRawKey("RAW");

CMessageImpl::CMessageImpl()
{
	m_pDBMsg = new DBMSG();
}


CMessageImpl::~CMessageImpl()
{
	if (m_pDBMsg)
	{
		m_pDBMsg->m_objPayload.clear();
	}
	DEF_DELETE(m_pDBMsg);
}

std::string  CMessageImpl::getMsgID() const
{
	FUNCTION_ENTRY("getMsgID");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_strMsgID;
}

void   CMessageImpl::setMsgID(const std::string& strMsgID)
{
	FUNCTION_ENTRY("setMsgID");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_strMsgID = strMsgID;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
}

ulong64   CMessageImpl::getMsgSeqID() const
{
	FUNCTION_ENTRY("getMsgSeqID");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_ulSeqID;
}

void   CMessageImpl::setMsgSeqID(ulong64 ulSeqID)
{
	FUNCTION_ENTRY("setMsgSeqID");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_ulSeqID = ulSeqID;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

int   CMessageImpl::getMsgState() const
{
	FUNCTION_ENTRY("getMsgState");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_nState;
}

void  CMessageImpl::setMsgState(int nState)
{
	FUNCTION_ENTRY("setMsgState");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_nState = nState;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

int  CMessageImpl::getMsgPriority() const
{
	FUNCTION_ENTRY("getMsgPriority");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_nMsgPriority;
}

void  CMessageImpl::setMsgPriority(int nPriority)
{
	FUNCTION_ENTRY("setMsgPriority");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_nMsgPriority = nPriority;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

int  CMessageImpl::getPayload(const std::string& strKey, std::string& strPayload)
{
	FUNCTION_ENTRY("getPayload");
	int nRetCode = Ret_Success;
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_pDBMsg->m_objPayload.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Message: %s", "the payload is empty or null.");
		return Ret_Failed;
	}

	nRetCode = _GetPayload(strKey, strPayload);

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMessageImpl::getPayload(const std::string& strKey, const char*& pPayload)
{
	FUNCTION_ENTRY("getPayload");
	int nRetCode = Ret_Success;
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_pDBMsg->m_objPayload.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Message: %s", "the payload is empty or null.");
		return Ret_Failed;
	}

	nRetCode = _GetPayload(strKey, pPayload);

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMessageImpl::getPayload(const std::string& strKey, int& nPayloadVal)
{
	FUNCTION_ENTRY("getPayload");
	int nRetCode = Ret_Success;
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_pDBMsg->m_objPayload.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Message: %s", "the payload is empty or null.");
		return Ret_Failed;
	}

	nRetCode = _GetPayload(strKey, nPayloadVal);

	FUNCTION_EXIT;
	return nRetCode;
}

int  CMessageImpl::getPayload(const std::string& strKey, ulong64& n64PayloadVal)
{
	FUNCTION_ENTRY("getPayload");
	int nRetCode = Ret_Success;
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_pDBMsg->m_objPayload.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Message: %s", "the payload is empty or null.");
		return Ret_Failed;
	}

	nRetCode = _GetPayload(strKey, n64PayloadVal);

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMessageImpl::getPayload(std::string& strPayload)
{
	FUNCTION_ENTRY("getPayload");
	int  nRetCode = Ret_Success;
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_pDBMsg->m_objPayload.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Message: %s", "the payload is empty or null.");
		return Ret_Failed;
	}

	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		mapPayloadIterT iter = m_pDBMsg->m_objPayload.begin();
		strPayload = iter->second;
	}
	else
	{
		nRetCode = _GetPayload(strPayload);
	}

	FUNCTION_EXIT;
	return nRetCode;
}

void   CMessageImpl::setPayload(int nPayloadType, const std::string& strKey, const std::string& strData)
{
	FUNCTION_ENTRY("setPayload");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	m_pDBMsg->m_nPayloadType = nPayloadType;
	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		m_pDBMsg->m_objPayload.clear();
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(s_strRawKey, strData));
	}
	else
	{
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKey, strData));
		_ParseUserData1(strKey,  strData);
		_CheckSrvID();
	}
	m_pDBMsg->m_uDataLen += strData.size();
	FUNCTION_EXIT;
}

void  CMessageImpl::setPayload(const std::string& strKey, const std::string& strPayload)
{
	FUNCTION_ENTRY("setPayload");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		m_pDBMsg->m_objPayload.clear();
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(s_strRawKey, strPayload));
	}
	else
	{
		std::string strKeyUpper = CUtilityFun::strtoupper((char*)strKey.c_str());
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKeyUpper, strPayload));
		_ParseUserData1(strKey,  strPayload);
		_CheckSrvID();
	}
	m_pDBMsg->m_uDataLen += strPayload.size();
	FUNCTION_EXIT;

}

void  CMessageImpl::setPayload(const std::string& strKey, int nValue)
{
	FUNCTION_ENTRY("setPayload");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	std::string strData = CUtilityFun::int2str(nValue);

	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		m_pDBMsg->m_objPayload.clear();
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(s_strRawKey, strData));
	}
	else
	{
		std::string strKeyUpper = CUtilityFun::strtoupper((char*)strKey.c_str());
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKeyUpper, strData));
		// check message original srv_id
		_CheckSrvID();
	}
	m_pDBMsg->m_uDataLen += strData.size();
	FUNCTION_EXIT;
}

void  CMessageImpl::setPayload(const std::string& strKey, ulong64 u64Value)
{
	FUNCTION_ENTRY("setPayload");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	std::string strData = CUtilityFun::u64tostr(u64Value);

	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		m_pDBMsg->m_objPayload.clear();
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(s_strRawKey, strData));
	}
	else
	{
		std::string strKeyUpper = CUtilityFun::strtoupper((char*)strKey.c_str());
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKeyUpper, strData));
		// check message original srv_id & center_sid
		_CheckSrvID();
		_CheckCenterSID();
	}
	m_pDBMsg->m_uDataLen += strData.size();
	FUNCTION_EXIT;
}

void  CMessageImpl::setPayload(const std::string& strPayload)
{
	FUNCTION_ENTRY("setPayload");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (PAYLOAD_TYPE_RAW == m_pDBMsg->m_nPayloadType)
	{
		m_pDBMsg->m_objPayload.clear();
		m_pDBMsg->m_objPayload.insert(mapPayloadValT(s_strRawKey, strPayload));
	}
	else
	{
		_ParseDBPayload(strPayload);
		// check message original srv_id
		_CheckSrvID();
	}

	m_pDBMsg->m_uDataLen += strPayload.size();

	FUNCTION_EXIT;
}

int  CMessageImpl::getPayloadType() const
{
	FUNCTION_ENTRY("getPayloadType");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_nPayloadType;
}

void  CMessageImpl::setPayloadType(int nPayloadType)
{
	FUNCTION_ENTRY("setPayloadType");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_nPayloadType = nPayloadType;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

void   CMessageImpl::setRecipientList(const LstRectT& rLstRecipient)
{
	FUNCTION_ENTRY("setRecipientList");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (rLstRecipient.empty()) return;	

	if (!m_pDBMsg->m_strRecipientLst.empty())
	{
		m_pDBMsg->m_strRecipientLst += ";";
	}

	std::string strName, strAddr;
	LstRectIterT iter = rLstRecipient.begin();
	while (iter != rLstRecipient.end())
	{
		strName = iter->getName();
		strAddr = iter->getAddr();	

		iter++;
		if ( iter != rLstRecipient.end() )
		{
			m_pDBMsg->m_strRecipientLst += (strName + "," + strAddr + ";");
		}
		else
		{
			m_pDBMsg->m_strRecipientLst += (strName + "," + strAddr );
		}
		
	}
	FUNCTION_EXIT;
}

void   CMessageImpl::setRecipientList(const CAgent& rAgent)
{
	FUNCTION_ENTRY("setRecipientList");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	std::string strName(rAgent.getName());
	std::string strAddr(rAgent.getAddr());

	if (!m_pDBMsg->m_strRecipientLst.empty())
	{
		m_pDBMsg->m_strRecipientLst += ";";
	}
	
	m_pDBMsg->m_strRecipientLst += (strName + "," + strAddr );
	
	FUNCTION_EXIT;
}

std::string CMessageImpl::getRecipientList() const
{
	FUNCTION_ENTRY("getRecipientList");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_strRecipientLst;

}

void   CMessageImpl::setSender(const CAgent& rSender)
{
	FUNCTION_ENTRY("setSender");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_strSender = (rSender.getName() + "," + rSender.getAddr());
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

CAgent CMessageImpl::getSender() const
{
	FUNCTION_ENTRY("getSender");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	u_int uPosDemli = 0;
	std::string strName, strAddr;
	uPosDemli = m_pDBMsg->m_strSender.find_first_of(",");
	if (std::string::npos == uPosDemli)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, sender: %s", Err_SenderStr_Format_Invalid, defErrMsg(Err_SenderStr_Format_Invalid), m_pDBMsg->m_strSender.c_str());
		return CAgent();
	}
	strName = m_pDBMsg->m_strSender.substr(0, uPosDemli - 1);
	strAddr = m_pDBMsg->m_strSender.substr(uPosDemli + 1);

	FUNCTION_EXIT;
	return CAgent(strName, strAddr);
}

void   CMessageImpl::setQName(const std::string& strQName)
{
	FUNCTION_ENTRY("setQName");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_strQName = strQName;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}
std::string  CMessageImpl::getQName() const
{
	FUNCTION_ENTRY("getQName");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_strQName;
}

void  CMessageImpl::setSubCnt(int nSubCnt)
{
	FUNCTION_ENTRY("setSubCnt");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_nSubCnt = nSubCnt;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

int  CMessageImpl::getSubCnt() const
{
	FUNCTION_ENTRY("getSubCnt");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_nSubCnt;
}


void  CMessageImpl::setOServerID(int nOServerID)
{
	FUNCTION_ENTRY("setOServerID");
	if (m_pDBMsg)
	{
		m_pDBMsg->m_nOServerID = nOServerID;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	FUNCTION_EXIT;
}

int  CMessageImpl::getOServerID() const
{
	FUNCTION_ENTRY("getOServerID");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_nOServerID;
}


u_int  CMessageImpl::getSize() const
{
	FUNCTION_ENTRY("getSize");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_objPayload.size();
}

u_int  CMessageImpl::getPayloadSize() const
{
	FUNCTION_ENTRY("getSize");
	if (!m_pDBMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
	return m_pDBMsg->m_uDataLen;
}

int  CMessageImpl::_GetPayload(const std::string& strKey, std::string& strPayload)
{
	int nRetCode = Ret_Success;
	std::string strKeyFind = CUtilityFun::strtoupper((char*)strKey.c_str());
	mapPayloadIterT iter = m_pDBMsg->m_objPayload.find(strKey);
	if (iter != m_pDBMsg->m_objPayload.end())
	{
		strPayload = iter->second;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, payload key: %s", Err_Payload_NotFount, defErrMsg(Err_Payload_NotFount), strKey.c_str());
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CMessageImpl::_GetPayload(const std::string& strKey, const char*& pPayload)
{
	int nRetCode = Ret_Success;
	std::string strKeyFind = CUtilityFun::strtoupper((char*)strKey.c_str());
	mapPayloadIterT iter = m_pDBMsg->m_objPayload.find(strKey);
	if (iter != m_pDBMsg->m_objPayload.end())
	{
		pPayload = iter->second.c_str();
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, payload key: %s", Err_Payload_NotFount, defErrMsg(Err_Payload_NotFount), strKey.c_str());
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CMessageImpl::_GetPayload(const std::string& strKey, int& nPayloadVal)
{
	int nRetCode = Ret_Success;
	std::string strVal;
	std::string strKeyFind = CUtilityFun::strtoupper((char*)strKey.c_str());
	mapPayloadIterT iter = m_pDBMsg->m_objPayload.find(strKey);
	if (iter != m_pDBMsg->m_objPayload.end())
	{
		strVal = iter->second;
		nPayloadVal = atoi(strVal.c_str());
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, payload key: %s", Err_Payload_NotFount, defErrMsg(Err_Payload_NotFount), strKey.c_str());
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CMessageImpl::_GetPayload(const std::string& strKey, ulong64& n64PayloadVal)
{
	int nRetCode = Ret_Success;
	std::string strVal;
	std::string strKeyFind = CUtilityFun::strtoupper((char*)strKey.c_str());
	mapPayloadIterT iter = m_pDBMsg->m_objPayload.find(strKey);
	if (iter != m_pDBMsg->m_objPayload.end())
	{
		strVal = iter->second;
		n64PayloadVal = CUtilityFun::atoui64(strVal);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, payload key: %s", Err_Payload_NotFount, defErrMsg(Err_Payload_NotFount), strKey.c_str());
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CMessageImpl::_GetPayload(std::string& strPayload)
{
	int nRetCode = Ret_Success;
	std::string strValueSize;
	strPayload.clear();
	mapPayloadIterT iter = m_pDBMsg->m_objPayload.begin();
	while (iter != m_pDBMsg->m_objPayload.end())
	{
		strValueSize = CUtilityFun::int2str(iter->second.size());
		strPayload  += (strValueSize + iter->first + "=" + iter->second);

		iter++;
	}

	return nRetCode;
}

void  CMessageImpl::_ParseDBPayload(const std::string& strPayload)
{
	int nKeyPosBegin = 0;
	int nPosDeli     = 0;
	u_int uValueSize = 0;
	u_int uRemainLen = 0, uRemainPos = 0;
	std::string strSize, strKey, strValue;
	std::string strRemainData(strPayload);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
		"CMessageImpl::_ParseDBPayload(strPayload) {strPayload is:]");
	LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, strPayload);

	while (strRemainData.size() > 0)
	{
		nKeyPosBegin = strRemainData.find_first_not_of("0123456789");
		if (std::string::npos == nKeyPosBegin ||!nKeyPosBegin)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s [strRemainData is:]", Err_Payload_Format_Invalid, defErrMsg(Err_Payload_Format_Invalid));
			LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strRemainData);
			break;
		}
		nPosDeli = strRemainData.find_first_of("=");
		if (std::string::npos == nPosDeli)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s [strRemainData is:]", Err_Payload_Format_Invalid, defErrMsg(Err_Payload_Format_Invalid));
			LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strRemainData);
			break;
		}
		strSize    = strRemainData.substr(0, nKeyPosBegin);
		strKey     = strRemainData.substr(nKeyPosBegin, nPosDeli - nKeyPosBegin);
		uValueSize = atoi(strSize.c_str());		

		uRemainPos = nPosDeli + uValueSize;
		uRemainLen = strRemainData.size();
		if (uRemainPos <= uRemainLen)
		{
			strValue   = strRemainData.substr(nPosDeli + 1, uValueSize);
			strKey     = CUtilityFun::strtoupper((char*)strKey.c_str());
			m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKey, strValue));

			if ( uRemainLen == uRemainPos || uRemainPos + 1 == uRemainLen) break;

			strRemainData = strRemainData.substr(uRemainPos + 1);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s [strRemainData is:]", Err_Payload_Format_Invalid, defErrMsg(Err_Payload_Format_Invalid));
			LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strRemainData);
			break;
		}		

	} // end of while
}

void  CMessageImpl::_CheckSrvID()
{
	if (m_pDBMsg->m_objPayload.empty()) return;

	mapPayloadIterT iterFind = m_pDBMsg->m_objPayload.find(defMsg_SrvIDTag);
	if (iterFind != m_pDBMsg->m_objPayload.end())
	{
		int nSrvID = atoi(iterFind->second.c_str());
		setOServerID(nSrvID);
	}
}

void  CMessageImpl::_ParseUserData1(const std::string& strKey, const std::string& strData)
{
	if (!CUtilityFun::stricmp_(defPayload_1_Tag, strKey.c_str()))
	{
		int nIndex = 0, nDelimPos = 0;
		std::string strKeyTag, strVal;
		std::string strRemain(strData);
		while (true)
		{	
			nIndex++;
			nDelimPos = strRemain.find_first_of(defMsg_DelimitTag);
			if (nDelimPos == std::string::npos)  break;
			_GetKey(nIndex, strKeyTag);
			strVal = strRemain.substr(0, nDelimPos);	
			m_pDBMsg->m_objPayload[strKeyTag] = strVal;			

			strRemain = strRemain.substr(nDelimPos + 1);
		}	
		if (nIndex > 1)
		{
			_GetKey(nIndex, strKeyTag);
			m_pDBMsg->m_objPayload.insert(mapPayloadValT(strKeyTag, strRemain));
		}
	}
}

void  CMessageImpl::_CheckCenterSID()
{
	if (m_pDBMsg->m_objPayload.empty()) return;

	mapPayloadIterT iterFind = m_pDBMsg->m_objPayload.find(defMsg_CenterSIDTag);
	if (iterFind != m_pDBMsg->m_objPayload.end())
	{
		ulong64 ui64SID = CUtilityFun::atoui64(iterFind->second);
		setMsgSeqID(ui64SID);
	}

}

void  CMessageImpl::_GetKey(int nIndex, std::string& strKey)
{
	switch (nIndex)
	{
	case 1:
		strKey = defPayload_1_Tag;
		break;
	case 2:
		strKey = defMsg_SrvIDTag;
		break;
	case 3:
		strKey = defMsg_CenterSIDTag;
		break;
	}
}

NS_END(TA_AQ_Lib)
















