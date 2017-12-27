/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ThreadConnResolve.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for ThreadConnResolve.
  */

#ifndef __THREAD_CONNECTION_RESOLVE_H_INCLUDED__
#define __THREAD_CONNECTION_RESOLVE_H_INCLUDED__
#pragma once

#include "core/threads/src/Thread.h"
#include "GENAHttpClient.h"
#include "CommonDef.h"
#include "GENARequest.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	// Forward declaration
	class ServerConnectionManager;

	class ThreadConnResolve
		: public TA_Base_Core::Thread
	{
	public:
		/*
		* Constructor
		*/
		ThreadConnResolve(ServerConnectionManager* pServerCon, 
			ConnectionPtr pConnection);
		
		/*
		* Destructor
		*/
		~ThreadConnResolve(void);

		/*
		* run
		*/
		virtual void run(void);

		/*
		* terminate
		*/
		virtual void terminate(void);

		/*
		* terminate
		*/
		void startResolve(boost::shared_ptr<GENARequest> objReq);

		unsigned int getLastReqResult() { return m_lastRetVal; };

	private:
		bool m_bIsTerminate;
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		ServerConnectionManager* m_pServerCon;
		ConnectionPtr m_pConnection;
		boost::shared_ptr<GENARequest> m_objReq;
		
		GENARequestStatus m_lastRetVal;
		//TA_Base_Core::Condition m_condition;
	}; //ThreadConnResolve

	typedef boost::shared_ptr<ThreadConnResolve> ThreadConnResolvePtr;
} //TA_Base_Bus

#endif //__THREAD_CONNECTION_RESOLVE_H_INCLUDED__
