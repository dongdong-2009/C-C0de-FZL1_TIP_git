//////////////////////////////////////////////////////////////////////
///		@file		MySQLMessage.h
///		@author		zhilin,ouyang
///		@date		2012-01-19 14:44:41
///
///		@brief	    implementation of IMessage interface.  
///                  
///                 
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  __CMESSAGE_IMP_HH_
#define  __CMESSAGE_IMP_HH_
#include "CommonDef.h"
#include "CommonData.h"

#include "IMsgItem.h"

NS_BEGIN(TA_DB_Sync)

class CMsgItemImp :	public IMsgItem
{
public:
	CMsgItemImp();
	~CMsgItemImp();
	CMsgItemImp(const CMsgItemImp& rhs);
	CMsgItemImp& operator=(const CMsgItemImp& rhs);

public:	
	virtual void setMsgMT(const unsigned char* pSrc, int nLen) ;
	virtual unsigned char* getMsgMT()const ;

	virtual void  setMsgQN(const unsigned char* pSrc, int nLen) ;
	virtual unsigned char* getMsgQN()const ;

	virtual void  setMsgOMI(const unsigned char* pSrc, int nLen) ;
	virtual unsigned char* getMsgOMI()const ;
	
	virtual void setOServerIDArray( const unsigned char* pSrc, int nLen );
	virtual void setMsgOServerID( int nOServerID );
	virtual int getMsgOServerID() const;
	
	virtual void  setMsgSID(ulong64 nMsgSID);
	virtual void  setMsgSID(const unsigned char* pSrc, int nLen);
	virtual ulong64 getMsgSID()const;

	virtual void  setMsgPR(const unsigned char* pSrc, int nLen) ;
	virtual unsigned char* getMsgPR()const ;

	virtual void  setMsgPYTP(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgPYTP() const ;


	virtual void  setMsgDLArray(const unsigned char* pSrc, int nLen) ;
	virtual void  setMsgDL(int nMsgDL) ;
	virtual int getMsgDL()const;

	//last set msgData 
	virtual void  setMsgData(const unsigned char* pSrc, int nLen) ;
	virtual unsigned char* getMsgData()const ;

	virtual unsigned char*  getMsgStartPtr() const;
	virtual int  getMsgAllLen() const ;
		  
private:
	PMSGITEM_HEADER                m_pMsgHeader;


};

NS_END(TA_DB_Sync)

#endif  // __CMESSAGE_IMP_HH_










