#include "TcpConnectionActor.h"



#include "core/utilities/src/DebugUtil.h"

#include "UtilityFun.h"	 

#include "IServerTcpComType.h"
#include "SrvWorkMutil.h"
#include "MysqlSrvWorkSingle.h"

NS_BEGIN(TA_DB_Sync)

using  TA_Base_Core::DebugUtil;


TcpConnectionActor::TcpConnectionActor(TcpSocket* pNewClient, unsigned long ID)
{	
 
    m_pServerTcpComType = _InitNewServer(pNewClient);
    m_ID = ID;
	m_pServerTcpComType->setID(ID);
	m_enqueueStatusBad = false;
	m_dequeueStatusBad = false;
}

unsigned long TcpConnectionActor::getID()
{	
	return m_ID;	 
}

TcpConnectionActor::~TcpConnectionActor()
{
	if (NULL != m_pServerTcpComType)
	{
		DEF_DELETE(m_pServerTcpComType);
	}
}

IServerTcpComType* TcpConnectionActor::_InitNewServer( TcpSocket* pNewClient)
{
	IServerTcpComType*          pNewServerTcpComType = NULL;
	std::string strDbType;
	
	
	if (enumMysqlDb == g_system_variables.m_nDBType)
	{
		pNewServerTcpComType = _InitMysqlNewServer(pNewClient);
	}
	else if (enumOracleDb == g_system_variables.m_nDBType)
	{
		pNewServerTcpComType = _InitOracleNewServer(pNewClient);
	}
	else 
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "DBType Error!"); 
	}

 
    
	return pNewServerTcpComType;
}

IServerTcpComType* TcpConnectionActor::_InitMysqlNewServer( TcpSocket* pNewClient)
{ 
	IServerTcpComType*          pNewServerTcpComType = NULL;
	
	pNewServerTcpComType = new CMysqlSrvWorkSingle(pNewClient); //it has two thread objects,
  
	return pNewServerTcpComType;
}



IServerTcpComType* TcpConnectionActor::_InitOracleNewServer( TcpSocket* pNewClient)
{ 
	IServerTcpComType*          pNewServerTcpComType = NULL;
	   		
	pNewServerTcpComType = new CSrvWorkMutil(pNewClient);

	return pNewServerTcpComType;
}

bool TcpConnectionActor::isHealth()
{
	bool bServerStatus = false;
	WorkTimeInfo workTimeInfoGet;

	if (NULL != m_pServerTcpComType)
	{
		m_pServerTcpComType->getServerThreadNotWorkTime(workTimeInfoGet);
		bServerStatus = m_pServerTcpComType->isTcpConnected();		
		if (false == bServerStatus) 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "socket lost, need to be released [ClientID=%s]",
				workTimeInfoGet.m_strClientID.c_str()); 
		}
	}
	

	if (true == bServerStatus)
	{
		bServerStatus= !m_enqueueStatusBad;
		if (false == bServerStatus) 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "enqueue meet problem, need to be released [ClientID=%s]",
				workTimeInfoGet.m_strClientID.c_str()); 
		}
	}

	if (true == bServerStatus)
	{
		bServerStatus = !m_dequeueStatusBad;
		if (false == bServerStatus)
		{
		    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "dequeue meet problem, need to be released [ClientID=%s]",
				workTimeInfoGet.m_strClientID.c_str());   
		}
	}
	
	//monitor enq time
	if (workTimeInfoGet.m_nEnqNotWork > g_system_variables.m_nEnqTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "EnqNotWork %llu > %d, need to be released [ClientID=%s]",
			workTimeInfoGet.m_nEnqNotWork, g_system_variables.m_nEnqTimeOut,
			workTimeInfoGet.m_strClientID.c_str());  
		bServerStatus = false;
	}
	
	//monitor deq time
	if (workTimeInfoGet.m_nDeqNotWork > g_system_variables.m_nDeqTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "DeqNotWork %llu > %d, need to be released [ClientID=%s]",
			workTimeInfoGet.m_nDeqNotWork, g_system_variables.m_nDeqTimeOut,
			workTimeInfoGet.m_strClientID.c_str());  
		bServerStatus = false;
	}
	
	//monitor deq not get ack
	if (workTimeInfoGet.m_nDeqACKNotGet > g_system_variables.m_nDeqGetACKTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "DeqACKNotGet %llu > %d, need to be released [ClientID=%s]",
			workTimeInfoGet.m_nDeqACKNotGet, g_system_variables.m_nDeqGetACKTimeOut,
			workTimeInfoGet.m_strClientID.c_str());  
		bServerStatus = false;
	}

	
	//monitor andShark time
	if (workTimeInfoGet.m_nHandSharkNotGet > g_system_variables.m_nHandShareTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "HandSharkNotGet %llu > %d, need to be released [ClientID=%s]",
		  workTimeInfoGet.m_nHandSharkNotGet, g_system_variables.m_nHandShareTimeOut,
		  workTimeInfoGet.m_strClientID.c_str());  
		bServerStatus = false;
	}

	return bServerStatus;
}


void  TcpConnectionActor::setEnqueueStatusBad(bool bad) 
{
	m_enqueueStatusBad=bad;
}
void  TcpConnectionActor::setDequeueStatusBad(bool bad)
{
	m_dequeueStatusBad=bad;
}

int	TcpConnectionActor::getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet)
{
	int nFunRes = 0;
	if (NULL != m_pServerTcpComType)
	{
		nFunRes = m_pServerTcpComType->getServerThreadNotWorkTime(workTimeInfoGet);		
	}
	return nFunRes;
}

std::string TcpConnectionActor::getClientAgentName()
{
	std::string strRes;
	if (NULL != m_pServerTcpComType)
	{
		strRes = m_pServerTcpComType->getClientAgentName();		
	}
	return strRes;
}

std::string TcpConnectionActor::getClientID()
{
	std::string strRes;
	if (NULL != m_pServerTcpComType)
	{
		strRes = m_pServerTcpComType->getClientID();		
	}
	return strRes;
}


void TcpConnectionActor::serverstopWork(int nMode)
{
	if (NULL != m_pServerTcpComType)
	{
		m_pServerTcpComType->serverTerminateAndWait(nMode);
	}
}

void TcpConnectionActor::serverStartComWork()
{
	if (NULL != m_pServerTcpComType)
	{
		m_pServerTcpComType->serverStartComWork();		
	}
}


void TcpConnectionActor::serverStartEnqWork(int nConReqACk)
{
	if (NULL != m_pServerTcpComType)
	{
		m_pServerTcpComType->serverStartEnqWork(nConReqACk);		
	}
}

void TcpConnectionActor::serverStartDeqWork( int nConReqACk )
{
	if (NULL != m_pServerTcpComType)
	{
		m_pServerTcpComType->serverStartDeqWork(nConReqACk);		
	}
}


 


NS_END(TA_DB_Sync)











