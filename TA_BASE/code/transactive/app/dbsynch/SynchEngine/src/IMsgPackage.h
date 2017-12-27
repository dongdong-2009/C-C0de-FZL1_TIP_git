//////////////////////////////////////////////////////////////////////
///		@file		IMsgFrame.h
///		@author		zhilin,ouyang
///		@date		2012-01-19 14:44:41
///
///		@brief	    the interface to encapsulate the message in the  
///                 database synchronization module. the implementation 
///                 can various based on the underlying database.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  __IMSGFRAME__HH___
#define  __IMSGFRAME__HH___
#include "CommonDef.h"
#include <string>
#include <list>

NS_BEGIN(TA_DB_Sync)

class IMsgItem;

class IMsgPackage
{
public:
/*
	STX --- start of the text.  Value: F8H.         Length: 1BYTE
	PL --- length of the package. Value: the length of the package.  Length: 4BYTE
	PUUID---														Length: 16BYTE
	PT --- type of the package.  Value:  0 每 request; 1 每 reply; 2 每 notify. Length: 1BYTE
	DC --- data compress option.  Value: 0 每 FALSE; 1 每 TRUE.  Length: 1BYTE
	DL --- length on the data . Value: the length of the data.  Length: 4BYTE
	data
	ETX --- end of the text.   Value: FEH.       Length: 1BYTE
*/

	virtual void  setMsgFrameSTX(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgFrameSTX()const = 0;

	virtual void  setMsgFramePLArray(const unsigned char* pSrc, int nLen) = 0;
	virtual void  setMsgFramePL(int nMsgPageLen) = 0;
	virtual int getMsgFramePL() const = 0;
	
	virtual void  setMsgFramePUUID16(const unsigned char* pSrc, int nLen) = 0;
	virtual void  setMsgFramePUUID32(const std::string& strUUID32) = 0;
	virtual unsigned char* getMsgFramePUUID() const = 0;

	virtual void  setMsgFramePT(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgFramePT() const = 0;

	virtual void  setMsgFrameDC(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgFrameDC() const = 0;

	
	virtual void  setMsgFrameDLArray(const unsigned char* pSrc, int nLen) = 0;
	virtual void  setMsgFrameDL(int nFrameDataSrclen) = 0;
	virtual int getMsgFrameDL() const = 0;
 

	virtual void  setMsgFrameETX(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgFrameETX() const = 0;

	//last set msgData 
	virtual void  setMsgFrameData(const unsigned char* pSrc, int nLen) = 0;
	virtual unsigned char* getMsgFrameData() const = 0;
	virtual int getMsgFrameDataLen() const = 0;

	virtual unsigned char*  getFrameStartPtr() const = 0;
public:
	virtual std::list<IMsgItem*>*   getLstFrameMsgs()  = 0;	
	virtual int						parseLstFrameMsgs()  = 0;
	virtual int						freeLstFrameMsgs()  = 0;
	virtual unsigned char					getMsgType()  = 0;
	virtual int						setMsgType(unsigned char ucMsgType)  = 0;
	

public:
	virtual ~IMsgPackage(void) {}
};

NS_END(TA_DB_Sync)

#endif // __IMSGFRAME__HH___

















