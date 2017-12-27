//////////////////////////////////////////////////////////////////////
///		@file		MsgDispatcher.cpp
///		@author		zhilin,ouyang
///		@date		2012-09-01 10:44:41
///
///		@brief	    implementation of interface IObserverRcvMsg. 
///                 the purpose of this class is to dispatch the 
///                 message which received from remote.
///                 
//////////////////////////////////////////////////////////////////////
#include "MsgDispatcher.h"
#include "Message.h"
#include "MsgHelper.h"
#include "IMsgItem.h"
#include "IPropagateHandler.h"
#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_DB_Sync)

CMsgDispatcher::CMsgDispatcher()
: m_pMsgHelper(NULL)
{
	m_mapAllObservers.clear();
	m_pMsgHelper = new CMsgItemHelper();
}

CMsgDispatcher::~CMsgDispatcher()
{
	_CleanObservers();
	DEF_DELETE(m_pMsgHelper);
}

int   CMsgDispatcher::processRcvFrame(const std::string& strServerID, PMSGPKG pRecvMsgPakage)
{
	FUNCTION_ENTRY("processRcvFrame");
	int nRetCode = Ret_Success;
	
	ThreadGuard  guard(m_lockMapAllObservers);

	mapAllObserverIterT iterAll = m_mapAllObservers.find(strServerID);
	if (iterAll != m_mapAllObservers.end())
	{
		nRetCode = _DispatchMsg(iterAll->second, pRecvMsgPakage);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 	
			"m_mapAllObservers cann't find strServerID [strServerID=%s][RcvEnqReplyUUID32=%s]", 	
			strServerID.c_str(), pRecvMsgPakage->m_strPkgUUID32.c_str());

		//this pRecvMsgPakage cann't find   DispatchMsg then free it
		//_FreeMSGPKG(pRecvMsgPakage);  
		//pRecvMsgPakage = NULL;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, ServerID: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr), strServerID.c_str());
		nRetCode = Ret_Failed;
	}
	
	
	//check	 DispatchMsg Result and free memory
	if (Ret_Failed == nRetCode && NULL != pRecvMsgPakage)
	{
		//this pRecvMsgPakage have error! then free memory
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 	
			"err: processRcvFrame have error! [strServerID=%s][RcvEnqReplyUUID32=%s] delete this Frame ", 	
			strServerID.c_str(), pRecvMsgPakage->m_strPkgUUID32.c_str());

		_FreeLstMsgItems(pRecvMsgPakage->m_lstAllMsgItems);	
		DEF_DELETE(pRecvMsgPakage);
 
		pRecvMsgPakage = NULL;
	}

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMsgDispatcher::processTcpEvent(const std::string& strServerID, int nStateCode)
{
	FUNCTION_ENTRY("processTcpEvent");
	int nRetCode = Ret_Success;
	ThreadGuard  guard(m_lockMapAllObservers);

	mapAllObserverIterT iterAll = m_mapAllObservers.find(strServerID);
	if (iterAll != m_mapAllObservers.end())
	{
		mapObserverIterT iterFound = (iterAll->second).begin();
		while (iterFound != (iterAll->second).end())
		{
			iterFound->second->processTcpEvent(nStateCode);
			iterFound++;
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, ServerID: %s", Err_Propagate_Handler_Not_Found, defErrMsg(Err_Propagate_Handler_Not_Found), strServerID.c_str());
		nRetCode = Ret_Failed;
	}

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMsgDispatcher::registerHandler(const std::string& strServerID, unsigned char ucMsgTMask, IPropagateHandler* pHandler)
{
	FUNCTION_ENTRY("registerHandler");
	ThreadGuard  guard(m_lockMapAllObservers);

	int nRetCode = Ret_Failed;
	if ( strServerID.empty() )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, ServerID: %s", Err_Create_TCPCon_Failed, defErrMsg(Err_Create_TCPCon_Failed), strServerID.c_str());
		return nRetCode;
	}
	mapAllObserverIterT iterAll = m_mapAllObservers.find(strServerID);
	if (iterAll != m_mapAllObservers.end())
	{
		mapObserverT& rExistOne = iterAll->second;
		rExistOne.insert(mapObserverValT(ucMsgTMask, pHandler));
	}
	else
	{
		mapObserverT mapObserver;
		mapObserver.insert(mapObserverValT(ucMsgTMask, pHandler));
		m_mapAllObservers.insert(mapAllObserverValT(strServerID, mapObserver));
	}	

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMsgDispatcher::unRegisterHandler(const std::string& strServerID, unsigned char ucMsgTMask)
{
	FUNCTION_ENTRY("unRegisterHandler");
	int nRetCode = Ret_Success;
	bool bDelete = false;
	ThreadGuard  guard(m_lockMapAllObservers);


	mapAllObserverIterT iterAll = m_mapAllObservers.find(strServerID);
	if ( iterAll != m_mapAllObservers.end() )
	{
		mapObserverT& rExistOne = iterAll->second;
		mapObserverIterT iterInner = rExistOne.find(ucMsgTMask);
		if ( iterInner != rExistOne.end())
		{
			rExistOne.erase(iterInner);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, ServerID: %s", Err_Propagate_Handler_Not_Found, defErrMsg(Err_Propagate_Handler_Not_Found), strServerID.c_str());
			nRetCode = Ret_Failed;
		}
		if ( rExistOne.empty() )		
			bDelete = true;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, ServerID: %s", Err_Propagate_Handler_Not_Found, defErrMsg(Err_Propagate_Handler_Not_Found), strServerID.c_str());
		nRetCode = Ret_Failed;
	}

	if ( bDelete )
		m_mapAllObservers.erase( iterAll );

	FUNCTION_EXIT;
	return nRetCode;
}

int   CMsgDispatcher::removeAllHandler()
{
	FUNCTION_ENTRY("unRegisterHandler");
	int nRetCode = Ret_Success;

	_CleanObservers();

	FUNCTION_EXIT;
	return nRetCode;
}

int  CMsgDispatcher::_DispatchMsg(mapObserverT& rMapObserver, PMSGPKG pRecvMsgPakage)
{
	FUNCTION_ENTRY("_DispatchMsg");
	int nRetCode = Ret_Success;
	unsigned char ucMT = 0;
	

	lstMsgItemConT* pGetLstMsgItem = NULL;
	IMsgItem* pMsgItem = NULL;
	
	if (NULL == pRecvMsgPakage)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: DispatchMsg but pRecvMsgPakage handle is NULL");
		nRetCode = Ret_Failed;
		return nRetCode;
	}
	if (pRecvMsgPakage->m_lstAllMsgItems.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"err: DispatchMsg Msg.Size = 0, PkgType = %d, PkgID=%s",
			pRecvMsgPakage->m_nPkgType, pRecvMsgPakage->m_strPkgUUID32.c_str());
		nRetCode = Ret_Failed;
		return nRetCode;
	}  
	
	pGetLstMsgItem = pRecvMsgPakage->m_lstAllMsgItems.front();
	
	if (pGetLstMsgItem->empty())
	{
		nRetCode = Ret_Failed;
		return nRetCode;
	}
	pMsgItem = pGetLstMsgItem->front();
	
	ucMT = (*(pMsgItem->getMsgMT()));


	_DispatchMsg(ucMT, rMapObserver, pRecvMsgPakage);
	_FreeLstMsgItems(pRecvMsgPakage->m_lstAllMsgItems);//free	
	DEF_DELETE(pRecvMsgPakage);
 


	FUNCTION_EXIT;
	return nRetCode;
}

void  CMsgDispatcher::_DispatchMsg(unsigned char ucMT, mapObserverT& rMapObserver, PMSGPKG pRecvMsgPakage)
{
	if (!m_pMsgHelper || rMapObserver.empty() || NULL == pRecvMsgPakage)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		return;
		//TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	mapObserverIterT  iterObserver;
	//IPropagateHandler* processHandle = NULL;
	
	bool    bDispatched = false;
	PCONACK pConAck = NULL;
	PENQREQEX pENQREQEX = NULL;
	PENQACK pEnqAck = NULL;
	PCONREQ pConReq = NULL;
	iterObserver = rMapObserver.begin();
	while (iterObserver != rMapObserver.end())
	{	

		switch (ucMT)
		{
		case ComProtocol_STOC_CON_ACK:
			if (iterObserver->first & ComProtocol_STOC_CON_ACK)
			{
				pConAck = NULL;
				m_pMsgHelper->getSTOCConACK(pRecvMsgPakage, pConAck);
				iterObserver->second->processRcvFrame(pConAck);	
				bDispatched = true;
			}
			break;
		case ComProtocol_STOC_ENQ_ACK:
			if (iterObserver->first & ComProtocol_STOC_ENQ_ACK)
			{
				pEnqAck = NULL;
				m_pMsgHelper->getSTOCEnqACK(pRecvMsgPakage, pEnqAck);
				iterObserver->second->processRcvFrame(pEnqAck);	
				bDispatched = true;
			}
			break;
		case ComProtocol_STOC_ENQ_REQ:
			if (iterObserver->first & ComProtocol_STOC_ENQ_REQ)
			{
				pENQREQEX = NULL;
				m_pMsgHelper->getSTOCEnqReq(pRecvMsgPakage, pENQREQEX);
				iterObserver->second->processRcvFrame(pENQREQEX);	
				bDispatched = true;
			}
			break;
		case ComProtocol_CTOS_CON_REQ:
			if (iterObserver->first & ComProtocol_CTOS_CON_REQ)
			{
				pConReq = NULL;
				m_pMsgHelper->getCTOSConReq(pRecvMsgPakage, pConReq);
				iterObserver->second->processRcvFrame(pConReq);	
				bDispatched = true;
			}
			break;
		case ComProtocol_CTOS_ENQ_ACK:
			if (iterObserver->first & ComProtocol_CTOS_ENQ_ACK)
			{
				pEnqAck = NULL;
				m_pMsgHelper->getCTOSEnqACK(pRecvMsgPakage, pEnqAck);
				iterObserver->second->processRcvFrame(pEnqAck);	
				bDispatched = true;
			}
			break;
		case ComProtocol_CTOS_ENQ_REQ:
			if (iterObserver->first & ComProtocol_CTOS_ENQ_REQ)
			{  		
				pENQREQEX = NULL;
				m_pMsgHelper->getCTOSEnqReq(pRecvMsgPakage, pENQREQEX);
				iterObserver->second->processRcvFrame(pENQREQEX);
				bDispatched = true;

			}
			break;
		default:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Invalid_Con_Protocol, defErrMsg(Err_Invalid_Con_Protocol));
			}		
			break;	
		}	
	
		iterObserver++;
	}

	if (!bDispatched)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"The message package has not handled by any observer and will be aborted. Package ID: %s", 
			pRecvMsgPakage->m_strPkgUUID32.c_str());
	}
}

void  CMsgDispatcher::_CleanObservers()
{
	ThreadGuard  guard(m_lockMapAllObservers);

	mapAllObserverIterT iterAll = m_mapAllObservers.begin();
	while (iterAll != m_mapAllObservers.end())
	{
		(iterAll->second).clear();

		iterAll++;
	}
	m_mapAllObservers.clear();
}




int CMsgDispatcher::_FreeLstMsgItem(lstMsgItemConT& lstMsgItem)
{
	int nFunRes = 0;
	lstMsgItemConIterT iterlstMsgItem;
	IMsgItem* pGetMsgItem = NULL;

	iterlstMsgItem = lstMsgItem.begin();
	while (iterlstMsgItem != lstMsgItem.end())
	{
		pGetMsgItem = *iterlstMsgItem;			
		DEF_DELETE(pGetMsgItem);				
		iterlstMsgItem++;
	}
	lstMsgItem.clear();	
	return nFunRes;
}

int CMsgDispatcher::_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems)
{
	int nFunRes = 0;
	lstMsgItemsConIterT iterLstAllMsgItems;
	lstMsgItemConT* pGetLstMsgItem = NULL;

	iterLstAllMsgItems = lstMsgItems.begin();
	while (iterLstAllMsgItems != lstMsgItems.end())
	{
		pGetLstMsgItem = *iterLstAllMsgItems;
		_FreeLstMsgItem(*pGetLstMsgItem);
		//fix memory problem
		DEF_DELETE(pGetLstMsgItem);
		iterLstAllMsgItems++;
	}

	lstMsgItems.clear();

	return nFunRes;
}




NS_END(TA_DB_Sync)