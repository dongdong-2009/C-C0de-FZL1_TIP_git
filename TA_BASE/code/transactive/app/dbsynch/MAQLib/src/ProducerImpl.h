//////////////////////////////////////////////////////////////////////
///		@file		ProducerImpl.h
///		@author		zhilin,ouyang
///		@date		2012-08-31 14:44:41
///
///		@brief	    the message producer implementation body of 
///                 interface of producer.              
///                 
//////////////////////////////////////////////////////////////////////
#ifndef PRODUCERIMPL_H
#define PRODUCERIMPL_H
#include "AQCommonData.h"
#include <set>

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CMessage;

class CProducerImpl
{
private:
	typedef  std::set<int>                  setErrCodeT;
	typedef  std::set<int>::iterator        setErrCodeIterT;

public:
	CProducerImpl(IDatabase* pDatabase);
	~CProducerImpl(void);

public:
	int   enqueueMsg(CMessage* pEnqMsg);
	int	  enqueueMsg(std::list<CMessage*>& EnqMsgList);
	void  setSkipErrorCode(const std::string& strErrorCode);
	void  setEnqMode(int nEnqMode);

private:
	int    _EnqMsg(CMessage* pEnqMsg);
	int    _EnqMsg(std::list<CMessage*>& EnqMsgList);
	void   _ParseSkipErrorsCode(const std::string& strErrorCode);
	inline
	bool   _CheckSkipErrors(int nErr);

private:
	IDatabase*           m_pDatabase;	
	MAQ_ENQ_OPT*         m_pEnqMode;
	setErrCodeT          m_setErrCode;

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CProducerImpl);
};

NS_END(TA_AQ_Lib)

#endif /// PRODUCERIMPL_H
























