#if !defined(__C_COM_PROTOCOL_HH__)
#define __C_COM_PROTOCOL_HH__


#include "Buffer.h"
#include "CommonDef.h"
#include "CommonData.h"


NS_BEGIN(TA_DB_Sync)

class IMsgPackage;


class CComProtocol
{
public:
	CComProtocol();
	~CComProtocol();
	
	
public:
	int	processSendData(MSGPKG* pMsgPkgSrc, IMsgPackage*& pFrameDest);
	int processRecvData(CBuffer* pRecvBuffer, lstMSGPKGConT& lstMsgPkg);
	bool isBufferHaveOneFrame(CBuffer* pRecvBuffer);

	
private:	
	int _GetBufferFromlstMsgItemConT(lstMsgItemConT& lstMsgItemSrc, CBuffer* pBufferDest);
	int	_GetBufferFromlstMsgItemsConT(lstMsgItemsConT& pMsgPkgSrc, CBuffer* pBuffDest);
	int _ZipBuffer(CBuffer* pBufferSrc, CBuffer* pBufferDest);
	int _UnZipBuffer(CBuffer* pBufferSrc, CBuffer* pBufferDest);
	int _GetOneFrameFromBuffer(CBuffer* pRecvBuffer, IMsgPackage*& pMsgFrame);
	int _GetMsgPkgFromFrame(IMsgPackage* pMsgFrame, MSGPKG*& pMsgPkg);
};//class 


NS_END(TA_DB_Sync)




#endif//__C_COM_PROTOCOL_HH__




















