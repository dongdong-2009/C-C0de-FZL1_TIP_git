//////////////////////////////////////////////////////////////////////
///		@file		SyncAQMsgManager.h
///		@author		zhilin,ouyang
///		@date		2012-01-09 14:44:41
///
///		@brief	    to declare the single manager for start/stop the 
///                 database synchronization routine.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef SYNCAQMSGMANAGER_H
#define SYNCAQMSGMANAGER_H
#include "CommonData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;

class CConfigInfoHelper;
class CAQMsgThreadsPool;

class CTcpConnectionListen;

class CSyncAQMsgManager
{
public:
	static CSyncAQMsgManager& getInstance();
	static void removeInstance();

	void  startSynchMsg(const std::string& strFileName);
	void  stopSynchMsg();

	void  setSynchMode(int nMode);
	void  setSynchMode(const std::string& strMode);
	int   getSynchMode()const;

private:
	int  _InitDefaultVal(pg_config_parameter pSystemVar);
	int  _InitSystemVaribles(const std::string& strFileName, pg_config_parameter pSystemVar);
	void  _StartSynchThread();

private:
	CSyncAQMsgManager(void);	
	~CSyncAQMsgManager(void);

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CSyncAQMsgManager);

private:	
	CAQMsgThreadsPool*				   m_pAQSynchPool;

	static CSyncAQMsgManager*          m_pInstance;
	static NonReEntrantThreadLockable  m_instanceLock;
	
private:
	CTcpConnectionListen*		       m_pOracleTcpServer;		
};

NS_END(TA_DB_Sync)

#endif  // SYNCAQMSGMANAGER_H
