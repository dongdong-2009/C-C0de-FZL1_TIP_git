#if !defined(__C_SERVER_WORK_MUTIL_HH__)
#define __C_SERVER_WORK_MUTIL_HH__


#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <set>
#include "CommonDef.h"
#include "CommonData.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "IServerTcpComType.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;
using namespace TA_AQ_Lib;

class IMsgItem;
class CSrvComWorker;

class IObserverRcvMsg;
class CSrvInitWorker;
class CSrvDeqWorker;
class CSrvEnqWorker;

class CSrvWorkMutil :  public IServerTcpComType
{
public:
	CSrvWorkMutil(TcpSocket* pNewTcpSocket);
	~CSrvWorkMutil();



public:
	virtual int		getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet);	
    virtual bool	serverTerminateAndWait(int nMode);
	virtual bool	serverStartEnqWork(int nConReqACk);
	virtual bool	serverStartDeqWork(int nConReqACk);  
	
private: 
	int												m_nClientIndex;
	CSrvDeqWorker*									m_pSrvDeqWorker;
	CSrvEnqWorker*									m_pSrvEnqWorker;
private:
  	unsigned int                                    m_nThreadIDSrvEnqWorker;
 	unsigned int                                    m_nThreadIDSrvDeqWorker;
 		
};


NS_END(TA_DB_Sync)

#endif//__C_SERVER_WORK_SINGLE_HH__




















