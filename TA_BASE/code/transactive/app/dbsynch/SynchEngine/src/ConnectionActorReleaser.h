/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/dbsynch/SynchEngine/src/ConnectionActorReleaser.h $
* @author:  LuoHuirong
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2013/02/19 09:15:14 $
* Last modified by:  $Author: LuoHuirong $
* 
* This class is used to release invaild ConnectionActor object.
* It is a singleton.
*/


#if !defined(__C_ConnectionActorReleaser_HH__)
#define __C_ConnectionActorReleaser_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list> 
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "Buffer.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "TcpConnectionActor.h"

NS_BEGIN(TA_DB_Sync)

class CAWorkTime;

using namespace TA_Base_Core;

class ConnectionActorReleaser: public Thread 
{
private:
	typedef std::list<TcpConnectionActor*>               vecAcotrT;
	typedef std::list<TcpConnectionActor*>::iterator     vecActorIterT;
	ConnectionActorReleaser(void);

public:
	
	~ConnectionActorReleaser(void);

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
	static ConnectionActorReleaser*  getInstance();
   
public:
	/**
	* push invalid actor to release.	 
	*	 
	*/
	void push(TcpConnectionActor* invaildActor);
	
	/*
	* for Monitor Release Actor WorkTime
	*/
	int	 getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet);


private:
	void _releaseInvaildActors();

private:
	bool m_toTerminate;

	/**
	* the one and only classInstance
	*/
	static ConnectionActorReleaser* m_theClassInstance;

	/**
	* protect singleton creation
	*/
	static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

    vecAcotrT     m_invaildActors;  // list of invaild actors

	TA_Base_Core::ReEntrantThreadLockable          m_datalock;
	TA_Base_Core::ReEntrantThreadLockable          m_monitorTimerLock; //use to lock monitor timer

 private:
	CAWorkTime* m_pMonitorReleaseActorWorkTime;
	std::string m_strClientID;// Release server's clientID, just for log
};

NS_END(TA_DB_Sync)




#endif//__C_ConnectionActorReleaser_HH__



