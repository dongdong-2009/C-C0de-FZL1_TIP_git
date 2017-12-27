#ifndef SESSIONDETAILSCACHE_H_INCLUDED
#define SESSIONDETAILSCACHE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/SessionDetailsCache.h $
  *  @author:  <your name>
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Provides a cached list of all operators that were/are associated with
  *  a given session at a given time. 
  */

namespace TA_Base_Bus
{
	class SessionDetailsCache : public ISessionUpdateObserver, TA_Base_Core::IOnlineUpdatable
	{
	public:

		SessionDetailsCache( SessionUpdateSubject& sessionUpdate, const std::string& delim="/" );
		~SessionDetailsCache();

		std::string getOperatorDelimList( const std::string& session, time_t time );

		// Staisfy the IOnlineUpdatable interface
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		// Satisfy the ISessionUpdateObserver interface
		virtual void sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation );
		virtual void sessionEnd( const std::string& session );
		virtual void sessionOverrideBegin( const std::string& session, unsigned long profile );
		virtual void sessionOverrideEnd( const std::string& session, unsigned long profile );

	private:
		SessionDetailsCache( const SessionDetailsCache& );
		SessionDetailsCache& operator =( const SessionDetailsCache& );

		void getDatabaseInfo( const std::string& session );
		
		std::string getOperatorFromDb   ( const std::string& session   );
		std::string getOperatorFromEvent( const std::string& paramList );

		void updateUpdateRequired( const std::string& session, bool state );

		TimePairMapBuilder m_mapBuilder;

		typedef std::map<std::string, bool> RequiresUpdateMap;

		RequiresUpdateMap m_requiresUpdate;

		mutable TA_Base_Core::NonReEntrantThreadLockable m_accessLock;

		static const std::string SUPERVISOR_ID;


		typedef std::map<unsigned long, TA_Base_Core::IOperator*> OperatorMap;
		OperatorMap m_operatorMap;
		
		DbCacheLookup<unsigned long, TA_Base_Core::IOperator, TA_Base_Core::OperatorAccessFactory> m_operatorLookup;
		
		const std::string m_delim;
	};
}

#endif //SESSIONDETAILSCACHE_H_INCLUDED
