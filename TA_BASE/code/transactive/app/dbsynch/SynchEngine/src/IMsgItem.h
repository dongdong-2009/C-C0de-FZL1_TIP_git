//////////////////////////////////////////////////////////////////////
///		@file		IMessage.h
///		@author		zhilin,ouyang
///		@date		2012-01-19 14:44:41
///
///		@brief	    the interface to encapsulate the message in the  
///                 database synchronization module. the implementation 
///                 can various based on the underlying database.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  IMESSAGE_H
#define  IMESSAGE_H

#include "CommonDef.h"
	
NS_BEGIN(TA_DB_Sync)

class IMsgItem
{
	

/*
MT		每 type of the message.  
			Value: 0 每 message contains payload; 
			1 每 message only contains message ID.
			Length: 1BYTE
QN		每 queue name.  Value: the target queue name.  Length: 32BYTE
OMI		每 the source/original message ID.  Value: unique ID.  Length: 36BYTE
SID
PR		---Priority: priority of the message. Value: 0每immediately; 1- fix time. Length: 1BYTE
PYTP
DL		---Length: the length of the message. Value: unsigned integer size.  Length: 4BYTE
*/
	
public:
	virtual void setMsgMT(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgMT()const = 0;

	virtual void  setMsgQN(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgQN()const = 0;

	virtual void  setMsgOMI(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgOMI()const = 0;
	
	virtual void setOServerIDArray( const unsigned char* pSrc, int nLen ) = 0;
	virtual void setMsgOServerID( int nOServerID ) = 0;
	virtual int getMsgOServerID() const  = 0;

	virtual void  setMsgSID(ulong64 nMsgSID) = 0;
	virtual void  setMsgSID(const unsigned char* pSrc, int nLen) = 0;
	virtual ulong64 getMsgSID()const = 0;

	virtual void  setMsgPR(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgPR()const = 0;
	
	virtual void  setMsgPYTP(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgPYTP()const = 0;

	virtual void  setMsgDLArray(const unsigned char* pSrc, int nLen) = 0;
	virtual void  setMsgDL(int nMsgDL) = 0;
	virtual int getMsgDL()const = 0;

	//last set msgData 
	virtual void  setMsgData(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgData()const = 0;

	virtual unsigned char*  getMsgStartPtr() const = 0;
	virtual int  getMsgAllLen() const = 0;

	virtual ~IMsgItem(void) {}
};

NS_END(TA_DB_Sync)


#endif // IMESSAGE_H





