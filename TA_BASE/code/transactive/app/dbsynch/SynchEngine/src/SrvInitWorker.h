#ifndef  _C_SRV_INIT_WORKER_H
#define  _C_SRV_INIT_WORKER_H


#include <string>

#include "CommonDef.h"
#include "CommonData.h"

#include "SrvComWorker.h"
#include "MsgHelper.h"

NS_BEGIN(TA_DB_Sync)

class CAWorkTime;

class CSrvInitWorker  
{	
public:
	CSrvInitWorker(	CSrvComWorker* pComWorker);
	~CSrvInitWorker();

public:
	int   setConReq(PCONREQ pConReq);
	int	  getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet);

public:
	int         sendConReqAcK(int nConReqAcK);
	
public:
	std::string getAgtName();
	std::string getQName();
	std::string getClientHostName();
	std::string getClientIP();
	int			getOServerID();

private:
	std::string         m_strPkgUUID32;
	std::string			m_strAgtName;
	std::string			m_strQName;
	std::string			m_strClientHostName;
	std::string			m_strClientIP;
	int    			    m_nINSRVID;
public:
	PCONACK				m_pConACK;             
	CMsgItemHelper*     m_pMsgItemHelper;
	CSrvComWorker*		m_pComWorkerCopy;
private:
	//handshark	msg time
	CAWorkTime*			m_pMonitorHandSharkTime;
		
};


NS_END(TA_DB_Sync)

#endif // _C_SERVER_INIT_WORKER_H



