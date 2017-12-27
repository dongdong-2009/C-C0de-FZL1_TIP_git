/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutySessionCache.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Caches all the session information received from the Authentication Agent
  */
#ifndef DUTYSESSIONCACHE_H_INCLUDED
#define DUTYSESSIONCACHE_H_INCLUDED

namespace TA_Base_Bus
{
	class DutySessionCacheImpl
	{
	public:

		friend class ACE_Singleton<DutySessionCacheImpl, ACE_Recursive_Thread_Mutex>;

		void sessionBegin        ( const TA_Base_Bus::SessionInfo& session );
		void sessionOverrideBegin( const TA_Base_Bus::SessionInfo& session );
		void sessionOverrideEnd  ( const TA_Base_Bus::SessionInfo& session );
		void sessionEnd          ( const TA_Base_Bus::SessionInfo& session );

		bool isOverrideBegin( const TA_Base_Bus::SessionInfo& session );

		DASType::DbKey getOperator( const std::string& session );
		DASType::DbKey getLocation( const std::string& session );
		void           getProfiles( const std::string& session, std::vector<DASType::DbKey>& profiles );

		bool isValidRegion( const DutyPrimitive& dp, const DASType::DbKey& region );

		TA_Base_Bus::SessionInfo getSessionInfo( const std::string& session );

		//cache the session for later DutyState Converters use
		void cacheSession(const TA_Base_Bus::SessionInfo& session);

	private:

		DutySessionCacheImpl(){}
		~DutySessionCacheImpl(){}

		DutySessionCacheImpl( const DutySessionCacheImpl& );
		DutySessionCacheImpl& operator=( const DutySessionCacheImpl& );

		
		typedef std::map<std::string,TA_Base_Bus::SessionInfo> SessionMap;
		
		SessionMap::iterator findSession( const std::string& session, bool retrieveFromAuth = false);
	

		SessionMap m_sessionMap;
		
		mutable ACE_RW_Mutex m_lock;

		AuthenticationLibrary m_authLibrary;
	};

	typedef ACE_Singleton<DutySessionCacheImpl, ACE_Recursive_Thread_Mutex> DutySessionCache;
}
#endif //  DUTYSESSIONCACHE_H_INCLUDED
