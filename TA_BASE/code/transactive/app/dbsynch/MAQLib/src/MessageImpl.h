//////////////////////////////////////////////////////////////////////
///		@file		Message.h
///		@author		zhilin,ouyang
///		@date		2012-08-19 14:44:41
///
///		@brief	    the purpose of this class is to implement the class   
///                 CMessage by using the composition pattern.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef MESSAGEIMPL_H
#define MESSAGEIMPL_H
#include "AQCommonData.h"
#include "Agent.h"

NS_BEGIN(TA_AQ_Lib)

class CMessageImpl
{
private:
	typedef  std::list<CAgent>					LstRectT; 
	typedef  std::list<CAgent>::const_iterator	LstRectIterT;
	typedef  mapPayloadT::iterator				mapPayloadIterT;
	typedef  mapPayloadT::value_type			mapPayloadValT;

public:
	CMessageImpl();
	~CMessageImpl();

public:
	std::string      getMsgID() const;
	void             setMsgID(const std::string& strMsgID);

	ulong64          getMsgSeqID() const;
	void             setMsgSeqID(ulong64 ulSeqID);

	int              getMsgState() const;
	void             setMsgState(int nState);

	int              getMsgPriority() const;
	void             setMsgPriority(int nPriority);

	int              getPayload(const std::string& strKey, std::string& strPayload);
	int              getPayload(const std::string& strKey, const char*& pPayload);
	int              getPayload(const std::string& strKey, int& nPayloadVal);
	int              getPayload(const std::string& strKey, ulong64& n64PayloadVal);
	int              getPayload(std::string& strPayload);

	void             setPayload(int nPayloadType, const std::string& strKey, const std::string& strData);
	void             setPayload(const std::string& strKey, const std::string& strPayload);
	void             setPayload(const std::string& strKey, int nValue);
	void             setPayload(const std::string& strKey, ulong64 u64Value);
	void             setPayload(const std::string& strPayload);

	int              getPayloadType() const;
	void             setPayloadType(int nPayloadType);

	void             setRecipientList(const LstRectT& rLstRecipient);
	void             setRecipientList(const CAgent& rAgent);
	std::string      getRecipientList() const; 

	void             setSender(const CAgent& rSender);
	CAgent           getSender() const;

	void             setQName(const std::string& strQName);
	std::string      getQName() const;

	void             setSubCnt(int nSubCnt);
	int              getSubCnt() const;

	u_int            getSize() const;
	u_int            getPayloadSize()const;

	void             setOServerID(int nOServerID);
	int              getOServerID() const;

private:
	inline
	int              _GetPayload(const std::string& strKey, std::string& strPayload);
	int              _GetPayload(const std::string& strKey, const char*& pPayload);
	int              _GetPayload(const std::string& strKey, int& nPayloadVal);
	int              _GetPayload(const std::string& strKey, ulong64& n64PayloadVal);
	int              _GetPayload(std::string& strPayload);
	void             _ParseDBPayload(const std::string& strPayload);
	void             _CheckSrvID();
	void             _ParseUserData1(const std::string& strKey, const std::string& strData);
	void             _CheckCenterSID();
	void             _GetKey(int nIndex, std::string& strKey);

private:
	DBMSG*           m_pDBMsg;

};


NS_END(TA_AQ_Lib)

#endif  // MESSAGEIMPL_H


























