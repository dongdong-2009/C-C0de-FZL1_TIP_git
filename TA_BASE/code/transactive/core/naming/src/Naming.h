#if !defined(NAMING_H)
#define NAMING_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/Naming.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is the implementation of the naming
  * handler
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include <string>
#include <list>
#include "omniORB4/CORBA.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/naming/IDL/src/NamedObjectRepositoryCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"


class ACE_Mutex;

namespace TA_Base_Core
{
	// Forward declare class
	class BindingRequestListener;
	class BindingHandler;
	class INamedObject;
	class NameObjectPairSeq;
	class ObjectRepositoryInfo;
    class NamingThreadPoolSingleton;

	/**
	  * NamingUser
	  * 
	  * This class is used by NamedObject to updated the 
	  * cache when an object reference is changed
	  */
	class NamingUser
	{
	public:
		/**
		  * NamingUser
		  */
		NamingUser();

		/**
		  * destructor
		  */
		virtual ~NamingUser();

		/** 
		  * handleNamingUpdate
		  *
		  * This is implemented by the derived class
		  * It is called when the agent Name, and object
		  * list is changed
		  *
		  * @param  std::string& agentName The name of the agent that sent the update
		  * @param NameObjectPairSeq& namedObjectSeq The list of objects changed
		  */
		virtual void handleNamingUpdate( const std::string& agentName, 
			                             const NameObjectPairSeq& namedObjectSeq ) = 0;

		/** 
		  * invalidateObject
		  *
		  * Called on the NamedObject when the agent has been invalidated.
		  * This will cause the cache to clear and the next request will result
		  * in a re-resolution
		  * 
		  * @param  std::string& agentName The name of the agent that has been invalidated
		  */
		virtual void invalidateObject( const std::string& agentName ) = 0;
	};


	/**
	  * Naming
	  */
	class Naming : public TA_Base_Core::AbstractMessageSubscriber
	{
		// Declare NamedObjectBase as a friend so it can use the 
		// resolveName function
		friend class AbstractNamedObject;
		friend class ServantBase;

	public:

		/**
		  * destructor
		  */
		virtual ~Naming();

		/**
		  * cleanUp
		  * 
		  * clean up the binding request listener
		  */
		static void cleanUp();
		
		/**
		  * getInstance
		  *
		  * static access to the one and only instance
		  */
		static Naming& getInstance();

		/** 
		  * registerNamingUser
		  *
		  * Register a Naming user to receive naming updates
		  *
		  * @param NamingUser* user the user to register
		  */
		void registerNamingUser( NamingUser* user );

		/** 
		  * deregisterNamingUser
		  *
		  * Remove a naming user from the list of users to be updated
		  *
		  * @param NamingUser* user the user to remove
		  */
		void deregisterNamingUser( NamingUser* user );

		/** 
		  * reBroadcastNamedObjects
		  *
		  * This function will cause this agent to rebroadcast
		  * it's list of NamedObjects.
		  */
		void rebroadcastNamedObjects( );

		std::string getServantKey(const std::string& agentName); //yanrong++: CL-21230

	    /**
		  * invalidateAgent
		  *
		  * This function will go through all the NamingUsers
		  * and cause them to invalidate their agents
		  */
		void invalidateAgent( const std::string& agentName );

	protected:

		/** 
		  * receiveMessage
		  *
		  * This function will receive the new object references for NamedObjects
		  *
		  * @param TA_Base_Core::StateUpdateMessageCorbaDef& message
		  */
		void receiveMessage(const CORBA::Any& message);

	private:

		/**
		  * resolveName
		  *
		  * Resolve a name from the Transactive Distributed Naming Service
		  *
		  * @param	agentName	The name of the agent that holds the object
		  * @param	objectName	the name of the object you want to resolve
		  * @return	Object	the resolved object
		  * @exception	NameNotFoundException if the name cannot be found
		  */
		CORBA::Object_var resolveName( const std::string& agentName, const std::string& objectName );
		
		/**
		  * registerName
		  *
		  * Register an Object in the Transactive Distributed Naming Service.
		  * Names are made up of the agent name and the entity name.
		  *
		  * The agent name will be taken from the --entity-name cmdline param
		  *
		  * @param	obj	the object to be stored
		  * @param	objectName	the name of the object
		  */
		static void registerName(CORBA::Object_ptr obj, const std::string& objectName);

		/**
		  * removeNameRegistration
		  *
		  * Remove a name registered in with naming
		  *
		  * @param	objectName	the name of the object you want to remove
		  */
		static void removeNameRegistration(const std::string& objectName);

		/**
		  * constructor
		  * 
		  * disable the copy constructor
		  */
		Naming(const Naming& theNaming);
		
		/**
		  * constructor
		  *
		  * Construct the naming object
		  */
		Naming();

		/**
		  * getObjectRepository
		  *
		  * Resolves the objectRepository and caches it. Returns the cached object
		  * if available
		  *
		  * If the object is not resolved it will be flagged as offline,
		  * This flag will be raised when a RebindName message comes in
		  * 
		  * @param	agentName	the agent that is hosting the object repository
		  * @return	the objectRepository	NULL If not found
		  */
		ObjectRepositoryInfo* getObjectRepository( const std::string& agentName );
        NamedObjectRepositoryCorbaDef_var getObjectRepositoryCorbaDef( const std::string& agentName );
        std::string getHostname(const std::string& agentName);

		/**
		  * setObjectRepository
		  *
		  * Replaces the objectRepository. This comes from NamedObjectBase
		  * 
		  * @param	agentName	the agent that is hosting the object repository
		  * @param	repository	the objectRepository
		  * @param	timeToLive	How long the name will remain valid
		  */
		void setObjectRepository( NamedObjectRepositoryCorbaDef_var repository,
			                      const std::string& agentName, CORBA::ULong timeToLive );

		/**
		  * getBindingListener
		  *
		  * Checks to see if the listener for this agent exists. Creates
		  * it if this is the first time a name has been bound
		  *
		  * @return	a valid BindingRequestListener
		  */
		BindingRequestListener* getBindingListener( );

	private:

		/**
		  * The listener for this agent
		  */
		BindingRequestListener* m_theListener;

		/**
		  * cache the repositories.
		  * 
		  * state:
		  *		Not found - need to resolve
		  *		Found but nil - offline
		  *		Found and healthy - available
		  */
		typedef std::map< std::string, ObjectRepositoryInfo* > RepositoryMap;
		RepositoryMap m_repositoryMap;

		/**
		  * store a map of NamedObjects for call-back purposes
		  **/
		typedef std::list< NamingUser* > NamingUserList;
		NamingUserList m_namingUsers;

		/**
		  * protect the listener
		  */
		TA_Base_Core::NonReEntrantThreadLockable m_listenerLock;

		/**
		  * protect creation of binding handler
		  */
		TA_Base_Core::NonReEntrantThreadLockable m_handlerLock;

		/**
		  * protect registration of naming users
		  */
		TA_Base_Core::NonReEntrantThreadLockable m_namingUserLock;

		/**
		  * the one and only classInstance
		  */
		static Naming* m_theClassInstance;
		
		/**
		  * protect singleton creation
		  */
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		static ACE_Mutex m_singletonLock;

        TA_Base_Core::NamingThreadPoolSingleton* m_namingThreadPoolSingleton;

        volatile bool m_terminating;
	};
} // TA_Base_Core
#endif // !defined(BindingHandler_6E7C4003_609A_422f_B72F_119D019379D6__INCLUDED_)
