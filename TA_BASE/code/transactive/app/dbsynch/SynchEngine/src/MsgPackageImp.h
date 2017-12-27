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
#ifndef  __CMSG_FRAME_IMP_HH_
#define  __CMSG_FRAME_IMP_HH_

#include <list>

#include "CommonDef.h"
#include "CommonData.h"

#include "IMsgItem.h"
#include "IMsgPackage.h"

NS_BEGIN(TA_DB_Sync)

class IMsgItem;

class CMsgPackageImp :	public IMsgPackage
{
public:
	CMsgPackageImp();
	~CMsgPackageImp();
	CMsgPackageImp(const CMsgPackageImp& rhs);
	CMsgPackageImp& operator=(const CMsgPackageImp& rhs);

public:

	virtual void  setMsgFrameSTX(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgFrameSTX()const;

	virtual void  setMsgFramePLArray(const unsigned char* pSrc, int nLen);
	virtual void  setMsgFramePL(int nMsgPageLen);
	virtual int getMsgFramePL() const;
	
	virtual void  setMsgFramePUUID16(const unsigned char* pSrc, int nLen);
	virtual void  setMsgFramePUUID32(const std::string& strUUID32);
	virtual unsigned char* getMsgFramePUUID() const;

	virtual void  setMsgFramePT(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgFramePT() const;

	virtual void  setMsgFrameDC(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgFrameDC() const;


	virtual void  setMsgFrameDLArray(const unsigned char* pSrc, int nLen);
	virtual void  setMsgFrameDL(int nFrameDataSrclen);
	virtual int getMsgFrameDL() const;


	virtual void  setMsgFrameETX(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgFrameETX() const;

	//last set msgData 
	virtual void  setMsgFrameData(const unsigned char* pSrc, int nLen);
	virtual unsigned char* getMsgFrameData() const;
	virtual int getMsgFrameDataLen() const;

	virtual unsigned char*  getFrameStartPtr() const;

public:
	virtual std::list<IMsgItem*>*            getLstFrameMsgs() ;	
	virtual int								 parseLstFrameMsgs() ;
	virtual int							     freeLstFrameMsgs() ;
	virtual unsigned char							 getMsgType() ;
	virtual int								 setMsgType(unsigned char ucMsgType) ;

private:
	PMSG_PACKAGE_HEADER              m_pMsgFrameHeader;
private:
	std::list<IMsgItem*>*            m_pLstMessages;
	unsigned char							 m_ucMsgType;//defMsg_PayloadType	
};

NS_END(TA_DB_Sync)

#endif  // __CMSG_FRAME_IMP_HH_
