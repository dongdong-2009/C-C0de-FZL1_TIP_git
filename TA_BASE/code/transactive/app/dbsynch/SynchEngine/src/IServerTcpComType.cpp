
 
#include "CommonDef.h"
#include <string>
#include "UtilityFun.h"
#include "IServerTcpComType.h"
#include "IObserverRcvMsg.h"
#include "IDbData.h"
#include "TcpConnectionPool.h"
#include "DatabaseImpl.h"
#include "TcpConnectionActor.h"
#include "MsgDispatcher.h"
NS_BEGIN(TA_DB_Sync)

 

IServerTcpComType::IServerTcpComType(TcpSocket* pNewTcpSocket)
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Construction IServerTcpComType"); 								   
	//m_ServerStream = pNewTcpSocket;	  //TcpSocket is not thread safety object, cannot use it in different thread at the same time.
	m_strClientID = "";
	m_pMsgPackageDispatch = NULL;
	m_ReceivedHandshark = false;
	m_nThreadIDSrvComWorker = 0;	
	m_nLogCount = 0; 
	m_ID = 0;
	m_pMsgPackageDispatch = new CMsgDispatcher();
	m_pSrvComWorker = new CSrvComWorker(pNewTcpSocket);
	m_pSrvInitWorker = new CSrvInitWorker(m_pSrvComWorker);
	_InitAgentName();									   	
	_InitSrvStream(pNewTcpSocket);	

}
 
void IServerTcpComType::setID(unsigned long ID)
{	 
	m_ID=ID;
}


int IServerTcpComType::_CheckConReq( PCONREQ pConReq)
{
	int nFunRes = 0;
	
	if (NULL == pConReq)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() NULL == ConReq");
  		nFunRes = -1;
		return nFunRes;
	}
	
	if (pConReq->m_strAgtName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() AgtName is empty");

		nFunRes = -1;
		return nFunRes;
	}
	
	if (pConReq->m_strQName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() QName is empty");

		nFunRes = -1;
		return nFunRes;
	}
	
	if (pConReq->m_strHostName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() HostName is empty");

		nFunRes = -1;
		return nFunRes;
	}

	//client will not send ip to server
// 	if (pConReq->m_strIP.empty())
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() IP is empty");
// 
// 		nFunRes = -1;
// 		return nFunRes;
// 	}
	
	if (pConReq->m_strPkgID.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() PkgID is empty");

		nFunRes = -1;
		return nFunRes;
	}
	
	if (pConReq->m_nOServerID < 0 || pConReq->m_nOServerID > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: _CheckConReq() OServerID=%d value is error",
				pConReq->m_nOServerID);

		nFunRes = -1;
		return nFunRes;
	}
	
	return nFunRes;
}


int IServerTcpComType::processRcvFrame( PCONREQ pConReq)
{	 
	int nFunRes = 0;
	std::string strDefMMSQName;
	std::string strQNameForCheck;
	
	PAQWorkInfo pDeqEnqInfo = NULL;
	
	if (0 != _CheckConReq(pConReq))
	{
		DEF_DELETE(pConReq);	
		nFunRes = -1;
		return nFunRes;
	}
	
	CUtilityFun::strtoupper((char *)pConReq->m_strAgtName.c_str());
	CUtilityFun::strtoupper((char *)pConReq->m_strQName.c_str());
	
	nFunRes = -1;
	{
		ThreadGuard guard(m_dataLock);
		//save ConReq value to	SrvInitWorker class
		if (NULL != m_pSrvInitWorker)
		{
			m_pSrvInitWorker->setConReq(pConReq);
			m_ReceivedHandshark=true;
			//must init clientID first   value eg.: AgtName_QName_IP
			//CTcpConnectionPool::getInstance()->releaseInvaildAction(m_ID) will use ClientID to find and release old one 
			nFunRes = _InitClientID(m_pSrvInitWorker); 
		}
	}
	
	//check initClientID Res
	if (0 != nFunRes)
	{
		DEF_DELETE(pConReq);	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "_InitClientID() error!");
		return nFunRes;
	}
	

    //find and release old invaild ConnectionActors
	CTcpConnectionPool::getInstance()->releaseInvaildAction(m_ID); 
	
	pDeqEnqInfo = new AQWorkInfo();
	pDeqEnqInfo->m_strQName	 = m_strQName;
	pDeqEnqInfo->m_strClientAgtName	 = m_strClientAgtName;
	pDeqEnqInfo->m_strServerAgtName	 = m_strServerAgtName;
	pDeqEnqInfo->m_nINSRVID = m_nINSRVID;
	pDeqEnqInfo->m_strClientID = m_strClientID;
	pDeqEnqInfo->m_nEnqMode = m_nEnqMode;

	//set AQWorkInfo to ComWorker
	m_pSrvComWorker->setAgtInfo(pDeqEnqInfo);


	//Start dequeue
	//defDefault_MMSQueueName  "MMS_DATA_QUEUE"
	//check if QueueName is "MMS_DATA_QUEUE" no need to start Deq thread
	strDefMMSQName = defDefault_MMSQueueName;
	strQNameForCheck = m_strQName;
	strDefMMSQName = CUtilityFun::strtoupper((char*)strDefMMSQName.c_str());
	strQNameForCheck = CUtilityFun::strtoupper((char*)strQNameForCheck.c_str());
	if (strQNameForCheck == strDefMMSQName)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "QueueName is MMS_DATA_QUEUE no need to start Deq thread");
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start Deq");
		serverStartDeqWork(ConReqAckCode_Succeed);
	}
	
	
	//Start Enqueue 
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start Enq");
	serverStartEnqWork(ConReqAckCode_Succeed);
	
		
	//send handshark acknowledge message
	if (NULL != m_pSrvInitWorker)
	{
		nFunRes = m_pSrvInitWorker->sendConReqAcK(ConReqAckCode_Succeed);
	}
	
	if (0 != nFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "send handshark ACK error!");
	}
	else
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "send handshark ACK ok!");
	}


	DEF_DELETE(pDeqEnqInfo);
	DEF_DELETE(pConReq);	

	return nFunRes;
}

int IServerTcpComType::processTcpEvent( int nStateCode )
{
	int nFunRes = 0;
	return nFunRes;
}


int IServerTcpComType::_InitClientID(CSrvInitWorker* pSrvInitWorker)
{
	//	get lock m_dataLock before call _InitClientID()
	// update m_strClientID value but no need to get lock m_dataLock  here
	//	ThreadGuard guard(m_dataLock);

	int nFunRes = 0;
	std::string strClientIDTmp;

	m_strAgtName.clear();
	m_strQName.clear();
	m_strClientAgtName.clear();	

	m_strAgtName = pSrvInitWorker->getAgtName();
	m_strQName = pSrvInitWorker->getQName();
	m_nINSRVID = pSrvInitWorker->getOServerID();
	m_nEnqMode = CTcpConnectionPool::getInstance()->getEnqModeByQueueName(m_strQName);
	m_strClientAgtName = m_strAgtName;

	if (m_strAgtName.empty() || m_strQName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	strClientIDTmp = m_strAgtName + "_";	
	strClientIDTmp += m_strQName + "_";	
	strClientIDTmp += m_strClientIP;
	
	m_strClientID = strClientIDTmp;

	LOG6(SourceInfo, DebugUtil::DebugInfo,
		"_InitClientID[ClientID=%s][AgtName=%s][QName=%s][ClientIP=%s]OServerID=%d][EnqMode=%d]",	
		m_strClientID.c_str(), m_strAgtName.c_str(), m_strQName.c_str(), 
		m_strClientIP.c_str(), m_nINSRVID, m_nEnqMode);

	return nFunRes;
}

std::string IServerTcpComType::getClientID()
{
	ThreadGuard guard(m_dataLock);

	return m_strClientID;
}

std::string IServerTcpComType::getClientAgentName()
{
	return m_strClientAgtName;
}


bool IServerTcpComType::serverStartComWork()
{
	bool nFunRes = true;	

	if (NULL != m_pMsgPackageDispatch)
	{
		m_pMsgPackageDispatch->registerHandler(defDefault_ServerIDName, ComProtocol_CTOS_CON_REQ, this);
		m_pSrvComWorker->registerObserver(m_pMsgPackageDispatch);  
		m_pSrvComWorker->start();
		m_nThreadIDSrvComWorker = m_pSrvComWorker->getId();
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"serverStartComWork[ClientID=%s][nThreadIDSrvComWorker=%d]", 
		m_strClientID.c_str(), m_nThreadIDSrvComWorker); 


	return nFunRes;
}


 
int IServerTcpComType::_InitSrvStream(TcpSocket* pSocket)
{
	int nFunRes = -1;

	try
	{
		m_strClientHostName = pSocket->getClientName();//"Lenovo-THINKR82MYWH (127.0.0.1)"
		m_strClientIP = _GetClientIpByClientName(m_strClientHostName);
		
		//m_strClientID will be inited later
		m_strClientID.clear();
	}
	catch(...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "m_ServerStream getClientName() error");
	}

	LOG1(SourceInfo, DebugUtil::DebugDebug,	"new client connected:[ClientHostName=%s]", m_strClientHostName.c_str());

	return nFunRes;	
}


std::string IServerTcpComType::_GetClientIpByClientName(const std::string& strClientName)
{
	std::string strClientIp("");
	size_t nFindLeft = 0;
	size_t nFindRight = 0;

	nFindLeft = strClientName.find("(");
	nFindRight = strClientName.rfind(")");

	if (nFindLeft > 0 && nFindRight > 0 && nFindRight > nFindLeft)
	{
		strClientIp = strClientName.substr(nFindLeft + 1, nFindRight - nFindLeft - 1);
	}
	return strClientIp;
}


int IServerTcpComType::_InitAgentName()
{
	int nRes = 0;
	std::string strSrcDbConStr;
	std::string strServerAgtName;
	TA_AQ_Lib::IDatabase*	pDataBaseGetAgentInfo = NULL; 

	//get Server Agent Name
	strServerAgtName.clear();
	strServerAgtName = g_system_variables.m_LocalAgent.getName();
	if (true == strServerAgtName.empty())
	{
		//SrcDbConStr = Oracle:TRA_OCC:IT271350_5:IT271350_5
		strSrcDbConStr = g_system_variables.m_SrcDbConStr;
		pDataBaseGetAgentInfo = new CDatabaseImpl(strSrcDbConStr.c_str());	

		//get Server Agent Name from Oracle DB
		_GetServerAgentInfo(pDataBaseGetAgentInfo, strServerAgtName); 

		DEF_DELETE(pDataBaseGetAgentInfo);
	}

	//check server Agent Name
	if (true == strServerAgtName.empty())
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "_InitAgentName [ServerAgtName=NULL]"); 
	}
	else
	{
		m_strServerAgtName = strServerAgtName;
		LOG1(SourceInfo, DebugUtil::DebugDebug,	"_InitAgentName [ServerAgtName=%s]", strServerAgtName.c_str()); 
	}
		
	return nRes;
}


bool IServerTcpComType::isTcpConnected() const
{
	bool bConnected = true;

	if (NULL != m_pSrvComWorker)
	{
		bConnected = m_pSrvComWorker->isTCPConnected();
	}
	return bConnected;
}







int IServerTcpComType::_GetServerAgentInfo(IDatabase *pDataBase, std::string& strServerAgtName )
{
	int					nFunRes = 0;
	IDbData*			pData = NULL;
	std::string			strSql;

	//=SELECT   NAME   FROM   V$DATABASE
	pDataBase->prepareSQLStatement(strSql, ORACLESYSTABLE_SELECT_5000);
	LOG1(SourceInfo, DebugUtil::DebugDebug,	"_GetServerAgentInfo()[ORACLESYSTABLE_SELECT_5000:strSql=%s]", strSql.c_str());

	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("NAME");//NAME TRANSACT

	try
	{
		pDataBase->connect();
		pData = pDataBase->executeQuery(strSql, columnNames);
		
		
		if (NULL != pData && pData->getNumRows() > 0)
		{
			strServerAgtName = pData->getStringData(0, columnNames[0]);
		}

		DEF_DELETE(pData);

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,	"DebugError! _GetServerAgentInfo()[ORACLESYSTABLE_SELECT_5000:strSql=%s]",
			strSql.c_str());
		DEF_DELETE(pData);	
	}


	return nFunRes;		
}


int IServerTcpComType::_GetClientAgentInfo(IDatabase *pDataBase, const std::string& strClientName, std::string& strClientAgtName )
{
	int nRes = 0;
	std::string strClientIp;
	IDbData*	pData = NULL;
	std::string strSql;	

	strClientIp = _GetClientIpByClientName(strClientName); 	

	//select name from recipient_info where (address like '%s%%') and (upper(NAME) != (SELECT   NAME   FROM   V$DATABASE)) 
	//ORACLERECIPIENT_SELECT_5001=select name from recipient_info where (address like '%s%%') and (upper(NAME) != upper(database())) 

	pDataBase->prepareSQLStatement(strSql, ORACLERECIPIENT_SELECT_5001, strClientIp.c_str());
	LOG1(SourceInfo, DebugUtil::DebugDebug, "_GetClientAgentInfo()[strSql=%s]", strSql.c_str());
	std::vector<std::string> columnNames;
	columnNames.push_back("NAME");//NAME TRANSACT 

	try
	{
		pDataBase->connect();
		pData = pDataBase->executeQuery(strSql, columnNames);
		

		if (pData->getNumRows() > 0)
		{
			strClientAgtName = pData->getStringData(0, columnNames[0]);
		}

		DEF_DELETE(pData);
	}
	catch (...)
	{
		nRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
			"DebugError! _GetClientAgentInfo()[ORACLERECIPIENT_SELECT_5001:strSql=%s]",
			strSql.c_str());
		DEF_DELETE(pData);	
	}


	return nRes;
}





 
NS_END(TA_DB_Sync)
 








