#ifndef  __CMSGHELPER_HH_
#define  __CMSGHELPER_HH_
#include "CommonDef.h"
#include "CommonData.h"


NS_BEGIN(TA_DB_Sync)



class CMsgItemHelper
{
public:
	CMsgItemHelper();
	~CMsgItemHelper();
	
public:
	int setCTOSEnqReq(PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg);
	int setCTOSConReq(PCONREQ pConReq, MSGPKG*& pMsgPkg);
	int setCTOSEnqACK(PENQACK pEnqACK, MSGPKG*& pMsgPkg);

	int setSTOCEnqReq(PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg);
	int setSTOCConACK(PCONACK pConACK, MSGPKG*& pMsgPkg);
	int setSTOCEnqACK(PENQACK pEnqACK, MSGPKG*& pMsgPkg);
	
public:
	int getCTOSEnqReq(MSGPKG* pMsgPkg, PENQREQEX& pEnqReqEx);
	int getCTOSConReq(MSGPKG* pMsgPkg, PCONREQ& pConReq);
	int getCTOSEnqACK(MSGPKG* pMsgPkg, PENQACK& pEnqACK);

	int getSTOCEnqReq(MSGPKG* pMsgPkg, PENQREQEX& pEnqReqEx);
	int getSTOCConACK(MSGPKG* pMsgPkg, PCONACK& pConACK);
	int getSTOCEnqACK(MSGPKG* pMsgPkg, PENQACK& pEnqAck);

private:	
	int _SetEnqACK(unsigned char ucMsgItemType, PENQACK pEnqACK, MSGPKG*& pMsgPkg);
	int _SetConReq(unsigned char ucMsgItemType, PCONREQ pConReq, MSGPKG*& pMsgPkg);
	int _SetConACK(unsigned char ucMsgItemType, PCONACK pConACK, MSGPKG*& pMsgPkg);
	int _SetEnqReq(unsigned char ucMsgItemType, PENQREQEX pEnqReqEx, MSGPKG*& pMsgPkg);
private:	
	int _GetEnqACKByItem(const IMsgItem* pMsgItem, ENQACK*& pData);
	int _GetConReqByItem(const IMsgItem* pMsgItem, CONREQ*& pData);
	int _GetConACKByItem(const IMsgItem* pMsgItem, CONACK*& pData);
	int _GetEnqReqByItem(lstMsgItemConT* pGetMsgItemConT, ENQREQEX*& pData);

private:
	//free EnqReqEx data
	int	_FreeEnqReqEx(ENQREQEX** ppEnqReqEx);
	int _FreeMsgVec(lstDBMsgConT& rVecMsg);
	
	//free MsgPkg data
	int _FreeMsgPkg(MSGPKG** ppMsgPkg);
	int _FreeLstMsgItems(lstMsgItemsConT& lstMsgItems);
	int _FreeLstMsgItem(lstMsgItemConT& lstMsgItem);
};

NS_END(TA_DB_Sync)

#endif  // __CMSGHELPER_HH_










