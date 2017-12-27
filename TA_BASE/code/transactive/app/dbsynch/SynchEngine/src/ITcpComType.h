
#ifndef  __I_TCP_COM_TYPE__HH__
#define  __I_TCP_COM_TYPE__HH__
#include "CommonDef.h"
#include "CommonData.h"


NS_BEGIN(TA_DB_Sync)
					 
class IObserverRcvMsg;




class ITcpComType
{
public:

	virtual void  managerStart() = 0;
	virtual void  managerTerminateAndWait() = 0;
	virtual int	  registerObserver(IObserverRcvMsg* Handler) = 0;
	virtual int   removeObserver() = 0;
	virtual int	  clientSendData(const std::string& strServerID, PMSGPKG pSendMsgPakage) = 0;
	virtual int   connectToServer(int nTimeOut) = 0;

	virtual ~ITcpComType(void) {}
};

NS_END(TA_DB_Sync)

#endif // __I_TCP_COM_TYPE__HH__