//////////////////////////////////////////////////////////////////////
///		@file		Producer.h
///		@author		zhilin,ouyang
///		@date		2012-08-31 14:44:41
///
///		@brief	    the message producer provides the interfaces to
///                 enq message to database queue. it use the composition 
///                 pattern to implement the class by using class 
///                 CProducerImpl
//////////////////////////////////////////////////////////////////////
#ifndef PRODUCER_H
#define PRODUCER_H
#include "AQCommonData.h"
#include <string>

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CProducerImpl;
class CMessage;

class CProducer
{
public:
	CProducer(IDatabase* pDatabase);
	~CProducer(void);

public:
	/*
	*Oracle DB:
	*enqMsg Return Value:
	*Return	0: enqMsg Ok
	*Return -1: enqMsg have Error
	*Return ER_ORACLE_EnqueueFailed: oracle queue has been stopped to prevent any further enqueueing. 
	*Return	Err_EnqReqACkCode_PLTypeInvalid, Err_EnqReqACkCode_OCILibErr, Err_EnqReqACkCode_DataSrcErr: Create EnqObject Error
	*EnqMsg handle is NULL, nErrorCode value: Err_EnqReqACkCode_PLTypeInvalid
	*EnqMsg's object handle is NULL, nErrorCode value:Err_EnqReqACkCode_OCILibErr
	*msg data have error, nErrorCode value: Err_EnqReqACkCode_DataSrcErr
	*/ 
	int   enqueueMsg(CMessage* pEnqMsg);
	int   enqueueMsg(std::list<CMessage*>& EnqMsgList);

	void  setSkipErrorCode(const std::string& strErrorCode);

	/*
	return: 0
	param: nEnqMode (the enqueue mode which load from database configuration)
	*/
	void setEnqMode(int nEnqMode);

private:
	CProducerImpl*    m_pProducerImpl;

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CProducer);

};

NS_END(TA_AQ_Lib)


#endif /// PRODUCER_H





















