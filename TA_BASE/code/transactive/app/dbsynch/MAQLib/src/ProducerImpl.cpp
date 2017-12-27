//////////////////////////////////////////////////////////////////////
///		@file		ProducerImpl.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-31 14:44:41
///
///		@brief	    the message producer implementation body of 
///                 interface of producer.              
///                 
//////////////////////////////////////////////////////////////////////
#include "ProducerImpl.h"
#include "IDatabase.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DatabaseException.h"

NS_BEGIN(TA_AQ_Lib)

CProducerImpl::CProducerImpl(IDatabase* pDatabase)
: m_pDatabase(pDatabase)
{
	m_pEnqMode = new MAQ_ENQ_OPT();
	m_setErrCode.clear();
}

CProducerImpl::~CProducerImpl(void)
{
	DEF_DELETE(m_pEnqMode);
	m_setErrCode.clear();
	m_pDatabase = NULL;
}

int CProducerImpl::enqueueMsg(CMessage* pEnqMsg)
{
	FUNCTION_ENTRY("enqueueMsg");	

	int nRetCode = Ret_Success;
	
	nRetCode = _EnqMsg(pEnqMsg);
	
	FUNCTION_EXIT;
	return nRetCode;
}

int CProducerImpl::enqueueMsg(std::list<CMessage*>& EnqMsgList)
{
	FUNCTION_ENTRY("enqueueMsg");	

	int nRetCode = Ret_Success;

	nRetCode = _EnqMsg(EnqMsgList);

	FUNCTION_EXIT;
	return nRetCode;
}

void  CProducerImpl::setSkipErrorCode(const std::string& strErrorCode)
{
	FUNCTION_ENTRY("setSkipErrorCode");	

	_ParseSkipErrorsCode(strErrorCode);

	FUNCTION_EXIT;
}

void  CProducerImpl::setEnqMode(int nEnqMode)
{
	FUNCTION_ENTRY("setEnqMode");	

	if (m_pEnqMode)
	{
		m_pEnqMode->m_nEnqMode = nEnqMode;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	FUNCTION_EXIT;
}



int  CProducerImpl::_EnqMsg(std::list<CMessage*>& EnqMsgList)
{
	FUNCTION_ENTRY("_EnqMsg");
	int nRetCode = Ret_Success;
	int nErrCode = 0;

	try
	{	
		m_pDatabase->connect();
		nRetCode = m_pDatabase->enqMsg(EnqMsgList, m_pEnqMode);
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		nErrCode = e.getNativeCode();		
		if (_CheckSkipErrors(nErrCode))  nRetCode = Ret_Success;
		else  nRetCode = nErrCode;
	}	
	catch(const TA_Base_Core::DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another db
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );
		nRetCode = Ret_Failed;
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );
		nRetCode = 2006;		
	}

	FUNCTION_EXIT;
	return nRetCode;
}

int  CProducerImpl::_EnqMsg(CMessage* pEnqMsg)
{
	FUNCTION_ENTRY("_EnqMsg");
	int nRetCode = Ret_Success;
	int nErrCode = 0;

	try
	{	
		m_pDatabase->connect();
		nRetCode = m_pDatabase->enqMsg(pEnqMsg, m_pEnqMode);
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		nErrCode = e.getNativeCode();		
		if (_CheckSkipErrors(nErrCode))  nRetCode = Ret_Success;
		else  nRetCode = nErrCode;
	}	
	catch(const TA_Base_Core::DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another db
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );
		nRetCode = Ret_Failed;
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );
		nRetCode = 2006;		
	}

	FUNCTION_EXIT;
	return nRetCode;
}

void   CProducerImpl::_ParseSkipErrorsCode(const std::string& strErrorCode)
{
	FUNCTION_ENTRY("_ParseSkipErrorsCode");
	size_t uSize = strErrorCode.size();
	if (strErrorCode.empty()) return;

	int    nErrCode = 0;
	char*  pDelimPos = NULL;
	char*  pcErrCode = new char[uSize + 1];
	memset(pcErrCode, 0, uSize + 1);
	memcpy(pcErrCode, strErrorCode.c_str(), uSize);	

	pDelimPos = strchr(pcErrCode, defMsg_DelimitTag);
	if (!pDelimPos)
	{
		nErrCode = atoi( pcErrCode );
		m_setErrCode.insert(nErrCode);
	}
	else
	{
		char* pBegin = pcErrCode;
		char* pEnd   = pDelimPos;
		pDelimPos++;
		(*pEnd) = 0;
		nErrCode = atoi( pBegin );
		m_setErrCode.insert( nErrCode );
		while (pDelimPos)
		{
			pBegin    = pDelimPos;
			pDelimPos = strchr(pDelimPos, defMsg_DelimitTag);
			if ( pDelimPos )
			{
				pEnd = pDelimPos;
				pDelimPos++;
				(*pEnd) = 0;				
			}
			nErrCode = atoi( pBegin );
			m_setErrCode.insert( nErrCode );
		}
	}

	DEF_DELETE_ARRAY(pcErrCode);

	FUNCTION_EXIT;
}

inline
bool  CProducerImpl::_CheckSkipErrors(int nErr)
{
	setErrCodeIterT iter = m_setErrCode.find(nErr);
	if ( iter != m_setErrCode.end() )
		return true;
	else
		return false;
}

NS_END(TA_AQ_Lib)













