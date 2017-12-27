// AuthenPeerTaskManager.cpp: implementation of the AuthenPeerTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/security/authentication_library/src/AuthenPeerTaskManager.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"
#include "bus/security/authentication_library/src/AuthenConst.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{

	AuthenPeerTaskManager::AuthenPeerTaskManager(AuthenticationLibrary * pAuthenLib)
	{
		TA_Assert( pAuthenLib != NULL );

		std::map<unsigned long, AuthenticationAgentNamedObj*> & authenAgentMap = 
			KeepSessionAliveThreadManager::getInstance().getAuthenAgentMap();
		TA_Assert( authenAgentMap.size() > 0 );

		std::map<unsigned long, AuthenticationAgentNamedObj*>::iterator it;
		for( it=authenAgentMap.begin(); it!=authenAgentMap.end(); it++)
		{
			unsigned long locationId = (*it).first;
//			if ( locationId > 4)
//			{
//				continue;
//			}
			AuthenticationAgentNamedObj* pAuthenAgent = (*it).second;
			TA_Assert( pAuthenAgent != NULL );
			std::auto_ptr<AuthenPeerTask> newTask(new AuthenPeerTask(pAuthenAgent, pAuthenLib, locationId));
			newTask->open(0);
			m_authenTaskMap.insert( AuthenTaskMap::value_type(locationId, newTask.release()) );
		}
	}

	AuthenPeerTaskManager::~AuthenPeerTaskManager()
	{
		AuthenTaskMap::iterator it;

		for( it = m_authenTaskMap.begin(); it != m_authenTaskMap.end(); it++ )
		{
			ACE_Message_Block* msg = new ACE_Message_Block;

			msg->msg_type( ACE_Message_Block::MB_HANGUP );

			if( AuthenLibConst::ACE_ERROR != it->second->putq(msg) )
			{
				it->second->wait();
				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finish thread for task at location %d end", it->first);
			}
			//delete it->second;
		}
		
		//no need to wait again
		//LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "all runing thread for task should finish  before manager wait");
		ACE_Thread_Manager::instance()->wait();
		LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "all running thread for task should finish after manager wait");


		for( it = m_authenTaskMap.begin(); it != m_authenTaskMap.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
	}

	void AuthenPeerTaskManager::getActiveSessions(const std::string & sessionId, std::vector<SessionInfo> & vtSessionInfo)
	{
		TA_Base_Core::ThreadGuard guard( m_sessionGetLock );
		
		AuthenTaskMap::iterator itTask;

		std::map< unsigned long, ACE_Future<bool> > resultMap;
		vtSessionInfo.clear();

		for( itTask = m_authenTaskMap.begin(); itTask != m_authenTaskMap.end(); itTask++ )
		{
			AuthenPeerTask * pTask = itTask->second;
			TA_Assert( pTask != NULL );
			unsigned long locationId = itTask->first;
			TA_Assert( locationId >= 0 );
			ACE_Future<bool> future = pTask->getActiveSessions(sessionId);
			resultMap[locationId] =  future;
		}

		TA_Base_Core::Thread::sleep(1000);

		std::map< unsigned long, ACE_Future<bool> >::iterator itResult;
		for( itResult = resultMap.begin(); itResult != resultMap.end(); itResult++ )
		{
			bool peerOk = false;
			unsigned long locationId = itResult->first;
			TA_Assert( locationId >= 0 );
			AuthenPeerTask * pTask = m_authenTaskMap[locationId];
			TA_Assert( pTask != NULL );


			// Should wait "forever" here unless we use reference counted
			// pointers for duty state. If we don't wait forever then we will
			// be deleting stuff (possibly) before the peer thread has
			// finished with it.
			if( AuthenLibConst::ACE_ERROR == itResult->second.get(peerOk) )
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "get active session failed at location %d, future return error", locationId);
				continue;
			}

			if( !peerOk )
			{
				//LOG
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "get active session failed at location %d, future return false", locationId);
				continue;
			}

			std::vector<SessionInfo> & vtActiveSessionForOneLocation = pTask->getResultSession();

			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"getActiveSessions, append %d sessions from location %d",
				vtActiveSessionForOneLocation.size(), locationId);
			for(size_t i=0; i<vtActiveSessionForOneLocation.size(); i++)
			{
				const SessionInfo & tmpSession = vtActiveSessionForOneLocation[i];
                 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getActiveSessions, find session<%s>, userid=%d, workstationId=%d", 
                    (tmpSession.SessionId).c_str(), tmpSession.UserId, tmpSession.WorkstationId);

				vtSessionInfo.push_back(tmpSession);
			}
		}
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"getActiveSessions, append total %d sessions",
			vtSessionInfo.size());
	}

}