/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/dbsynch/SynchEngine/src/MonitorWorker.h $
* @author:  shenglong.lin
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2013/02/28 09:15:14 $
* Last modified by:  $Author: shenglong.lin $
* 
* This class is used to monitor threads is hang or not.
* It is a singleton.
*/


#if !defined(__C_MONITOR_WORKER_HH__)
#define __C_MONITOR_WORKER_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list> 
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "TcpConnectionActor.h"

NS_BEGIN(TA_DB_Sync)

class CAWorkTime;

using namespace TA_Base_Core;

class CMonitorWorker: public Thread 
{
private:
	CMonitorWorker(void);

public:	
	~CMonitorWorker(void);

public:
	virtual void	run(); 
	virtual void	terminate();

	/**
	* getInstance
	*
	* Public accessor to the singleton
	*
	* @return   ConnectionActorReleaser    One and only ConnectionActorReleaser
	*/
	static CMonitorWorker*  getInstance();
	
private:
	/*
	* Monitor ConnectionActorReleaser thread 
	* if ConnectionActorReleaser thread hang, program will exit(0) after sleep(TIME_MIN_FIVE)
	*/
	void _MonitorThreads();	 	

private:
	/**
	* the one and only classInstance
	*/
	static CMonitorWorker* m_theClassInstance;

	/**
	* protect singleton creation
	*/
	static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
  
	TA_Base_Core::ReEntrantThreadLockable          m_datalock;

	bool m_toTerminate;
	
	

};

NS_END(TA_DB_Sync)




#endif//__C_MONITOR_WORKER_HH__



