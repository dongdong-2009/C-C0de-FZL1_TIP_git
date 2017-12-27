//////////////////////////////////////////////////////////////////////
///		@file		Consumer.h
///		@author		zhilin,ouyang
///		@date		2012-08-26 17:44:41
///
///		@brief	    the message consumer is get message from the source   
///                 database/local database based on the consumer name
///                 the routine is that the listener listen for messages
///                 for specified consumer and then fetch the message(s) 
///                 from the local database.
//////////////////////////////////////////////////////////////////////
#ifndef CONSUMER_H
#define CONSUMER_H
#include "AQCommonData.h"
#include <list>

NS_BEGIN(TA_AQ_Lib)

class CConsumerImpl;
class CAgent;
class IDatabase;
class CMessage;

class CConsumer	
{
private:
	typedef std::list<CMessage*>         lstMsgConT;

public:
	CConsumer(IDatabase* pDatabase);
	CConsumer(IDatabase* pDatabase, const CAgent& rAgent);
	CConsumer(IDatabase* pDatabase, const std::string& strConsumerName);
	CConsumer(IDatabase* pDatabase, int nPayloadType);
	~CConsumer(void);

public:
	void  setAgent(const CAgent& rAgent);
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

	int  deqMsg(lstMsgConT& rLstMsg,  u_int nDeqSize);
	int  deqMsg(LstMsgConT& rLstMsg,  u_int nDeqSize, u_int nSubID);
	int  getMsgSeqIds(nPairT& nSeqIDPair);

private:
	CConsumerImpl*     m_pConsumerImpl;

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CConsumer);
};

NS_END(TA_AQ_Lib)

#endif // CONSUMER_H























