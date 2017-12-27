#include "TcpConnectionPool.h"
#include "CommonDef.h"	
#include "CommonData.h"	 
#include "UtilityFun.h"
#include "core/utilities/src/DebugUtil.h"

#include "TcpConnectionActor.h"
#include "ConnectionActorReleaser.h"
#include "SQLCode.h"
#include "IDbData.h"

NS_BEGIN(TA_DB_Sync)

using  TA_Base_Core::DebugUtil;

ReEntrantThreadLockable CTcpConnectionPool::m_singletonLock;
CTcpConnectionPool* CTcpConnectionPool::m_theClassInstance = NULL;

CTcpConnectionPool* CTcpConnectionPool::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);

	// If guard acquired then make sure the singleton is still NULL
	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
		m_theClassInstance = new CTcpConnectionPool();
	}

	return  m_theClassInstance;
}

void CTcpConnectionPool::removeInstance()
{
	DEF_DELETE(m_theClassInstance);
}



CTcpConnectionPool::CTcpConnectionPool()
{	
	m_toTerminate = false;
	m_nMaxClientNum = SERVER_LIMIT_CLIENT_COUNT_MAX;	
	m_nCurrentClientNum = 0;
	m_sequenceID=0;
}


CTcpConnectionPool::~CTcpConnectionPool()
{
	_ClearAll();
}


void CTcpConnectionPool::_ClearAll()
{
	_ClearAllTcpSocket();
	_ClearAllConnectedClients();
	_ClearMapQNameAQManager();
}


void CTcpConnectionPool::_ClearAllConnectedClients()
{
	TA_Base_Core::ThreadGuard	guard(m_lockSetAllClients);
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor*			pClientGet = NULL;
	std::string					strClientID;

	iterSet = m_AllConnectedClients.begin();
	while (iterSet != m_AllConnectedClients.end())
	{
		pClientGet = iterSet->second;
		m_AllConnectedClients.erase(iterSet);

		strClientID = pClientGet->getClientID();
		pClientGet->serverstopWork(CurrentClientDestroy_Normal);
		DEF_DELETE(pClientGet);

		m_nCurrentClientNum--;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"after free one server [CurrentClientNum=%d][strClientID=%s]", 
			m_nCurrentClientNum, strClientID.c_str()); 


		iterSet = m_AllConnectedClients.begin();
	}
	m_AllConnectedClients.clear();
}


void CTcpConnectionPool::_ClearAllTcpSocket()
{
	TA_Base_Core::ThreadGuard	guard(m_lockSetAllClients);
	vecTcpSocketConIterT			iterVec;
	TcpSocket*					pClientGet = NULL;

	iterVec = m_AllNewTCPClients.begin();
	while (iterVec != m_AllNewTCPClients.end())
	{
		pClientGet = *iterVec;
		m_AllNewTCPClients.erase(pClientGet);

		DEF_DELETE(pClientGet);
		iterVec = m_AllNewTCPClients.begin();
	}
	m_AllNewTCPClients.clear();
}



void CTcpConnectionPool::run()
{  
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionPool  begin run()");

	while( false == m_toTerminate ) 
	{
		_ThreadJob();
		TA_Base_Core::Thread::sleep( SERVER_LIMIT_THREAD_SLEEP );
	}//while
	_ClearAll();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionPool  end run()");

}


void CTcpConnectionPool::terminate()
{
	m_toTerminate = true;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"CTcpConnectionPool::terminate()");
}


void CTcpConnectionPool::_ThreadJob()
{
	//change TCP socket into TCPConnectionActor
	_JobAcceptNewClient();	 

	//release TCPConnectionActor if its TCP connection is lost.
	_JobCheckConnectedClients();
}



void CTcpConnectionPool::_JobCheckConnectedClients()
{
	vecConPoolManagerConT     unHealthClients;
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor*			pClientGet = NULL;
	std::string                 strClientID;
	{ //fix dead lock problem	
		TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);	
		iterSet = m_AllConnectedClients.begin();

		while (iterSet != m_AllConnectedClients.end())
		{
			pClientGet = iterSet->second;
			if (pClientGet !=NULL && false == pClientGet->isHealth())
			{
				strClientID = pClientGet->getClientID();	 

				m_AllConnectedClients.erase(iterSet); 
				m_nCurrentClientNum--;  //why we need this count?
				unHealthClients[pClientGet->getID()]=pClientGet;		
				iterSet = m_AllConnectedClients.begin();
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "this client is unHealth [ClientID=%s][ClientNum=%d]", 
					strClientID.c_str(), m_nCurrentClientNum);

			}		 		 
			else if ( NULL == pClientGet)
			{
				m_AllConnectedClients.erase(iterSet);   //a= map[key], if map does not include an object with key 'key', map will insert an empty object with key 'key'
				iterSet = m_AllConnectedClients.begin();			 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unHealth,empty action,[ClientID=%s][ClientNum=%d]", 
					strClientID.c_str(), m_nCurrentClientNum);
			}
			else
			{
				iterSet++;
			}

		}//	while (iter_set)
	}


	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "remove unHealth");

	iterSet = unHealthClients.begin();
	while (iterSet != unHealthClients.end())
	{
		pClientGet = iterSet->second;	
		ConnectionActorReleaser::getInstance()->push(pClientGet);
		iterSet++;		 

	}//	while (iter_set)
	unHealthClients.clear();


}

bool CTcpConnectionPool::poolAcceptOneNewClient( TcpSocket* pNewClient)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);
	bool bFunRes = true;

	m_AllNewTCPClients.insert(pNewClient);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Number of New Client's Request= %d",m_AllNewTCPClients.size());
	return bFunRes;					   	
}


bool CTcpConnectionPool::_JobAcceptNewClient()
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);
	bool bFunRes = true;
	std::string SrcDbCon;
	TcpSocket* pNewClient = NULL;
	vecTcpSocketConIterT iterSet;

	iterSet = m_AllNewTCPClients.begin();
	if (iterSet == m_AllNewTCPClients.end())
	{
		bFunRes = false;
		return bFunRes;
	}
	pNewClient = *iterSet;
	m_AllNewTCPClients.erase(iterSet);

	if (!pNewClient->stillConnected())
	{

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,lost client connection");		 
		pNewClient->close(); //ok,  no other thread access this socket,so it can be closed 
		DEF_DELETE(pNewClient);
		bFunRes = false;
		return bFunRes;
	}

	if (m_AllConnectedClients.size() > m_nMaxClientNum)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,Arrived maximum allowable count of client ");	
		pNewClient->close();  
		DEF_DELETE(pNewClient);
		bFunRes = false;
		return bFunRes;

	}

	SrcDbCon = g_system_variables.m_SrcDbConStr;	
		   
	bFunRes = _AcceptNewClient(pNewClient);
			  
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"new,[AllNewTCPClients.size()=%d][SrcDbCon=%s][DBType=%d]", 
		m_AllNewTCPClients.size(), SrcDbCon.c_str(), g_system_variables.m_nDBType);

	return bFunRes;
}


bool CTcpConnectionPool::_AcceptNewClient( TcpSocket* pNewClient)
{ 
	bool						bFunRes = true;
	TcpConnectionActor*			pNeedCheckClient = NULL;
	int                         nCheckRes =	 0;

	if (NULL == pNewClient)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,NULL == pNewClient");
		bFunRes = false;
		return bFunRes;
	}	
	unsigned long id =_getNextActorID();
	pNeedCheckClient = new TcpConnectionActor(pNewClient,id);	
	m_AllConnectedClients[id]= pNeedCheckClient;
	pNeedCheckClient->serverStartComWork();	//start one TCP communication thread     
	m_nCurrentClientNum++;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection clients = %d, newID=%lu", m_AllConnectedClients.size(),id);

	return bFunRes;
}




void CTcpConnectionPool::releaseInvaildAction(unsigned long newActionID)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 


	TcpConnectionActor* pNewClient=NULL;

	vecConPoolManagerConIterT  iter=m_AllConnectedClients.find(newActionID);

	if( iter != m_AllConnectedClients.end() )
	{
		pNewClient=iter->second;
	}

	std::list<TcpConnectionActor*>  rlist;
	if (pNewClient==NULL)  //it may happen if connection is lost and TCPConnectionPool releases it.
	{
		return;
	}

	std::string					strClientIDCheck;
	std::string					strClientAgtName;
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor			*pClientGet = NULL;
	TcpConnectionActor			*pCreateNewClient = NULL; 	
	std::string					strClientIDGet;
	std::string					strClientAgtNameGet;

	//value eg.: AgtName_QName_IP 
	strClientIDCheck = pNewClient->getClientID();
	strClientAgtName = pNewClient->getClientAgentName();


	/*find and delete old */
	iterSet = m_AllConnectedClients.begin();
	while (iterSet != m_AllConnectedClients.end())
	{
		pClientGet = iterSet->second;		 
		if (pClientGet !=NULL)  
		{	 
			strClientIDGet = pClientGet->getClientID();
			strClientAgtNameGet = pClientGet->getClientAgentName();	

			if (strClientIDCheck == strClientIDGet &&  pClientGet->getID() != newActionID)
			{			 
				LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"found old server [CurrentClientNum=%d][ClientIDGet=%s][ClientAgtNameGet=%s][oldActionID=%lu][newActionID=%lu]",
					m_nCurrentClientNum, strClientIDGet.c_str(), strClientAgtNameGet.c_str(), pClientGet->getID(), newActionID); 
				rlist.push_back(pClientGet);
				pClientGet = NULL;	
			}	

		}
		iterSet++;

	}//	while (iter_set)


	std::list<TcpConnectionActor*>::iterator iterRemove=rlist.begin();

	while (iterRemove != rlist.end())
	{
		pClientGet = *iterRemove;
		m_AllConnectedClients.erase(pClientGet->getID());
		ConnectionActorReleaser::getInstance()->push(pClientGet);
		iterRemove++;
	}
	rlist.clear();

}



unsigned long CTcpConnectionPool::_getNextActorID()
{
	ThreadGuard guard(m_seqIDLock);
	m_sequenceID++;

	if( m_sequenceID == ULONG_MAX )
	{
		m_sequenceID = 1;
	}

	return m_sequenceID;
}

//Dequeue meet problem, need to release TcpConnectionActor
void  CTcpConnectionPool::setDequeueStatusBad(unsigned long  actorID)
{ 
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 
	TcpConnectionActor*			actor = NULL;  

	vecConPoolManagerConIterT  iter = m_AllConnectedClients.find(actorID);

	if( iter != m_AllConnectedClients.end() )
	{
		actor=iter->second;
		if (actor !=NULL) 
		{
			actor->setDequeueStatusBad(true);
		}	
	}


}

//Enqueue meet problem, need to release TcpConnectionActor
void    CTcpConnectionPool::setEnqueueStatusBad(unsigned long  actorID)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 
	TcpConnectionActor*			actor = NULL; 		 

	vecConPoolManagerConIterT  iter=m_AllConnectedClients.find(actorID);

	if( iter != m_AllConnectedClients.end() )
	{
		actor=iter->second;
		if (actor !=NULL) 
		{
			actor->setEnqueueStatusBad(true);
		}	
	}

}

//////////////////////////////////////////////////////////////////////////
void CTcpConnectionPool::_ClearMapQNameAQManager()
{
	mapQNameAQManagerConIterT itermap;
	AQManager* pGetData = NULL;
	ThreadGuard guard(m_MapQNameAQManagerLock);

	itermap = m_mapQNameAQManagerInfo.begin();
	while (itermap != m_mapQNameAQManagerInfo.end())
	{
		pGetData = itermap->second;
		
		DEF_DELETE(pGetData);
		
		itermap++;
	}
	m_mapQNameAQManagerInfo.clear();
	
}


int  CTcpConnectionPool::initMapQNameAQManager(IDatabase* pDatabase)
{
	int  nFunRes = 0;
	int  nRowNums = 0;
	TA_AQ_Lib::IDbData*  pData = NULL;
	int nIndex = 0;
	std::string strSql;
	std::vector<std::string> columnNames;
	ThreadGuard guard(m_MapQNameAQManagerLock);

	if (NULL == pDatabase)
	{
		nFunRes = -1;
		return nFunRes;
	}

	/* AQ_SQL_SElECT_AQSYSQUEUEM_8002=SELECT Q_NAME, ENQ_MODE FORM AQ_SYS_QUEUE_M */
	pDatabase->prepareSQLStatement(strSql, AQ_SQL_SElECT_AQSYSQUEUEM_8002);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "initMapQNameAQManager[AQ_SQL_SElECT_AQSYSQUEUEM_8002:strSql=%s]", strSql.c_str());
							 
	// Set up the columnNames vector to be passed to executeQuery()
	columnNames.push_back("Q_NAME");
	columnNames.push_back("ENQ_MODE");
							 	
	try
	{
		pDatabase->connect();
		pData = pDatabase->executeQuery(strSql, columnNames);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get AQSysQueueManagerInfo error! [strSql=%s]", strSql.c_str()); 
		DEF_DELETE(pData);	
		nFunRes = -1;
		return nFunRes;
	}
	
	if (NULL == pData)
	{  
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get AQSysQueueManagerInfo error! [strSql=%s]", strSql.c_str()); 
		nFunRes = -1;
		return nFunRes;	
	}

	nRowNums = 	pData->getNumRows();
	if (1 > nRowNums)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get AQSysQueueManagerInfo error! [strSql=%s][nRowNums=%d]", strSql.c_str(), nRowNums);
		DEF_DELETE(pData);	
		nFunRes = -1;
		return nFunRes;
	}		

	for (nIndex = 0; nIndex < nRowNums; nIndex++)
	{
		std::string strQName = pData->getStringData(nIndex, columnNames[0]);
		int nEnqMode = pData->getIntegerData(nIndex, columnNames[1]);

		AQManager* pAQManager = new AQManager();
		pAQManager->m_strQueueName = strQName;
		pAQManager->m_nEnqMode = (TA_AQ_Lib::EnqueueMode)nEnqMode;
		m_mapQNameAQManagerInfo.insert(mapQNameAQManagerValueT(strQName, pAQManager));	
	}

	DEF_DELETE(pData);

	return nFunRes;
}



TA_AQ_Lib::EnqueueMode  CTcpConnectionPool::getEnqModeByQueueName(const std::string& strQName)
{
	TA_AQ_Lib::EnqueueMode nEnqMode = ENQ_AQ_API;
	std::string strKeyQName;
	mapQNameAQManagerConIterT iterFind;
	AQManager* pGetData = NULL;
	ThreadGuard guard(m_MapQNameAQManagerLock);
									    
	strKeyQName = strQName;
	iterFind = m_mapQNameAQManagerInfo.find(strKeyQName);
	if (iterFind == m_mapQNameAQManagerInfo.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Err: cann't find QName in mapQNameAQManagerInfo, QName=%s, mapQNameAQManagerInfo.size=%d", 
			strQName.c_str(), m_mapQNameAQManagerInfo.size());
		nEnqMode = ENQ_AQ_API;
		return nEnqMode;
	}
	
	pGetData = iterFind->second;
	nEnqMode = pGetData->m_nEnqMode;
	
	LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"find QName in mapQNameAQManagerInfo, QName=%s, EnqMode=%d, mapQNameAQManagerInfo.size=%d, ", 
		strQName.c_str(), nEnqMode, m_mapQNameAQManagerInfo.size());
	

	return nEnqMode;
}


NS_END(TA_DB_Sync)








