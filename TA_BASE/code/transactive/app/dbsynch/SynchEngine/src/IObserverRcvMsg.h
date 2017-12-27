//////////////////////////////////////////////////////////////////////
///		@file		IObserverRcvMsg.h
///		@author		zhilin,ouyang
///		@date		2012-01-19 10:44:41
///
///		@brief	    the main thread to synch data for the system. 
///                 it gather the data changes from the source database
///                 in its queue table.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  IOBSERVERRCVMSG_H
#define  IOBSERVERRCVMSG_H
#include <string>
#include <list>

#include "CommonDef.h"
#include "CommonData.h"

NS_BEGIN(TA_DB_Sync)

class IPropagateHandler;

class IObserverRcvMsg
{
public:
	virtual int   processRcvFrame(const std::string& strServerID, PMSGPKG pRecvMsgPakage) = 0;
	virtual int   processTcpEvent(const std::string& strServerID, int nStateCode) = 0;

	virtual int   registerHandler(const std::string& strServerID, unsigned char ucMsgTMask, IPropagateHandler* pHandler) = 0;
	virtual int   unRegisterHandler(const std::string& strServerID, unsigned char ucMsgTMask) = 0;
	virtual int   removeAllHandler() = 0;

	virtual ~IObserverRcvMsg(){}
};


NS_END(TA_DB_Sync)

#endif // IOBSERVERRCVMSG_H
