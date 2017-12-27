//////////////////////////////////////////////////////////////////////
///		@file		ConsumerImpl.h
///		@author		zhilin,ouyang
///		@date		2012-08-26 10:04:41
///
///		@brief	    the purpose of this class is to implement the class   
///                 Consumer by using the composition pattern.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef CONSUMERIMPL_H
#define CONSUMERIMPL_H
#include "AQCommonData.h"
#include <list>

NS_BEGIN(TA_AQ_Lib)

class CAgent;
class IDatabase;
class CMessage;

class CConsumerImpl	
{
public:
	CConsumerImpl(IDatabase* pDatabase);
	CConsumerImpl(IDatabase* pDatabase, const CAgent& rAgent);
	CConsumerImpl(IDatabase* pDatabase, const std::string& strConsumerName);
	CConsumerImpl(IDatabase* pDatabase, int nPayloadType);
	~CConsumerImpl(void);

public:
	void  setAgent(const CAgent& rSub);
	void  setConsumberName(const std::string& strConName);
	std::string  getConsumberName() const;

	void  setDeqMode(DequeueMode mode);
	DequeueMode  getDeqMode() const;

	void  setMsgIdToDeq(const std::string& strMsgID);
	std::string getMsgIdToDeq() const;

	void  setPositionOfMsg(Navigation pos);
	Navigation getPositionOfMsg() const;

	void  setVisibility(Visibility op);
	Visibility getVisibility() const;

	void  setQueueName(const std::string& strQueue);
	std::string getQeueuName() const;

	void  setPayloadType(int nPayloadType);
	int   getPayloadType() const;

	/*for Oracle use*/
	int  deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize);
	/*for Mysql use*/
	int  deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize, u_int nSubID);

	int  getMsgSeqIds(nPairT& nSeqIDPair);

private:
	void  _FreeDBMsg(LstMsgConT& crDBData);
	

private:
	IDatabase*     m_pDatabase;
	CAgent*        m_pAgent;
	MAQ_DEQ_OPT*   m_pDeqOpt;
	std::string    m_strQueueName;
	int            m_nPayloadType;
	nPairT         m_nPairSeqIDs;

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CConsumerImpl);
};

NS_END(TA_AQ_Lib)

#endif // CONSUMERIMPL_H


























