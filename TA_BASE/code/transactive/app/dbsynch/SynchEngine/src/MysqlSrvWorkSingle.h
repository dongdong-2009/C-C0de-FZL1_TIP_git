#if !defined(__C_MYSWQ_SERVER_WORK_SINGLE_HH__)
#define __C_MYSWQ_SERVER_WORK_SINGLE_HH__


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
#include "IDatabase.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;
using namespace TA_AQ_Lib;

class IMsgItem;
class CSrvComWorker;

class IObserverRcvMsg;
class CSrvInitWorker;
class CMysqlSrvEnqWorker;


class CMysqlSrvWorkSingle :  public IServerTcpComType
{
public:
	CMysqlSrvWorkSingle(TcpSocket* pNewTcpSocket);
	~CMysqlSrvWorkSingle();

public:
	virtual int		getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet);
	virtual bool	serverTerminateAndWait(int nMode);
	virtual bool	serverStartDeqWork(int nConReqACk);  
	virtual bool	serverStartEnqWork(int nConReqACk);  
	

	
private: 
	int												m_nClientIndex;	
	CMysqlSrvEnqWorker*								m_pSrvEnqWorker; 	
	unsigned int                                    m_nThreadIDSrvEnqWorker;
	
};


NS_END(TA_DB_Sync)

#endif//__C_MYSWQ_SERVER_WORK_SINGLE_HH__




















