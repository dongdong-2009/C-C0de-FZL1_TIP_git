//////////////////////////////////////////////////////////////////////
///		@file		IPropagateHandler.h
///		@author		zhilin,ouyang
///		@date		2012-09-05 10:44:41
///
///		@brief	    the interface to propagate or observe incoming messages. 
///                 it setup the propagate env and gather the data changes 
///                 from the source database in its queue table.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef IPROPAGATEHANDLER_H
#define IPROPAGATEHANDLER_H
#include "CommonData.h"

NS_BEGIN(TA_DB_Sync)

class IPropagateHandler
{
public:
	virtual int   processTcpEvent( int nStateCode ) = 0;

	virtual int   setPropagateTHD(PRO_THD* pProThd){ return Ret_Success;}	
	virtual int   processRcvFrame(PENQREQEX pEnqReq){ return Ret_Success;}
	virtual int   processRcvFrame(PCONACK pConAck){ return Ret_Success;}
	virtual int   processRcvFrame(PENQACK pEnqAck){ return Ret_Success;}
	virtual int   processRcvFrame(PCONREQ pConReq){ return Ret_Success;}


	virtual ~IPropagateHandler(){}
};

NS_END(TA_DB_Sync)

#endif /// IPROPAGATEHANDLER_H




