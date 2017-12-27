#if !defined(__C_TCP_COM_TYPE_HELPER_HH__)
#define __C_TCP_COM_TYPE_HELPER_HH__

#include <string>
#include <set>

#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"
#include "core/sockets/src/TcpServerSocket.h"



NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;

class IServerTcpComType;

class TcpConnectionActor  
{

public:
	TcpConnectionActor(TcpSocket* pNewClient, unsigned long ID);
	~TcpConnectionActor();


public:
	bool										isHealth();
	int											getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet);	
		
	std::string									getClientAgentName();	
	std::string									getClientID();
	void   								        serverStartComWork(); 
	void   								        serverStartEnqWork(int nConReqACk); 
	void   								        serverStartDeqWork(int nConReqACk); 
    unsigned long                               getID();
	void   								        serverstopWork(int nMode); 

	void                                        setEnqueueStatusBad(bool bad); //Enqueue has problem
    void                                        setDequeueStatusBad(bool bad); //Dequeue has problem
private:
	IServerTcpComType*							_InitNewServer( TcpSocket* pNewClient);
	IServerTcpComType*							_InitOracleNewServer( TcpSocket* pNewClient);
	IServerTcpComType*							_InitMysqlNewServer( TcpSocket* pNewClient);
private:
	IServerTcpComType*							m_pServerTcpComType;

	
	unsigned long                               m_ID;
	bool   m_enqueueStatusBad;
	bool   m_dequeueStatusBad;
 
};
NS_END(TA_DB_Sync)




#endif//__C_TCP_COM_TYPE_HELPER_HH__
















