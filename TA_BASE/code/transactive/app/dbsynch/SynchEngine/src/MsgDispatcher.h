//////////////////////////////////////////////////////////////////////
///		@file		MsgDispatcher.h
///		@author		zhilin,ouyang
///		@date		2012-09-01 10:44:41
///
///		@brief	    implementation of interface IObserverRcvMsg. 
///                 the purpose of this class is to dispatch the 
///                 message which received from remote.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef MSGDISPATCH_H
#define MSGDISPATCH_H
#include "IObserverRcvMsg.h"
#include <map>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


using namespace TA_Base_Core;

NS_BEGIN(TA_DB_Sync)

class IPropagateHandler;
class CMsgItemHelper;
class CMessage;

class CMsgDispatcher : public IObserverRcvMsg
{
private:
	typedef  std::multimap<unsigned char, IPropagateHandler*>			      mapObserverT;
	typedef  std::multimap<unsigned char, IPropagateHandler*>::iterator       mapObserverIterT;
	typedef  std::multimap<unsigned char, IPropagateHandler*>::value_type     mapObserverValT;
	typedef  std::map<std::string, mapObserverT>							  mapAllObserverT;
	typedef  std::map<std::string, mapObserverT>::iterator					  mapAllObserverIterT;
	typedef  std::map<std::string, mapObserverT>::value_type				  mapAllObserverValT;
	typedef  lstMsgItemConT::iterator										  LstMsgItemIterT;
	typedef  lstMsgItemsConT::iterator										  LstAllMsgItemIterT;

public:
	CMsgDispatcher();
	~CMsgDispatcher();
public:
	int   processRcvFrame(const std::string& strServerID, PMSGPKG pRecvMsgPakage);
	int   processTcpEvent(const std::string& strServerID, int nStateCode);
	int   registerHandler(const std::string& strServerID, unsigned char ucMsgTMask, IPropagateHandler* pHandler);
	int   unRegisterHandler(const std::string& strServerID, unsigned char ucMsgTMask);
	int   removeAllHandler();

private:
	int      _DispatchMsg(mapObserverT& rMapObserver, PMSGPKG pRecvMsgPakage);
	void	 _DispatchMsg(unsigned char ucMT, mapObserverT& rMapObserver, PMSGPKG pRecvMsgPakage);
	void     _CleanObservers();
private:	
	int _FreeLstMsgItems(lstMsgItemsConT& lstMsgItems);
	int _FreeLstMsgItem(lstMsgItemConT& lstMsgItem);

private:
	ReEntrantThreadLockable	m_lockMapAllObservers; 

	mapAllObserverT         m_mapAllObservers;
	CMsgItemHelper*         m_pMsgHelper;

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CMsgDispatcher);
	
};

NS_END(TA_DB_Sync)

#endif //MSGDISPATCH_H






