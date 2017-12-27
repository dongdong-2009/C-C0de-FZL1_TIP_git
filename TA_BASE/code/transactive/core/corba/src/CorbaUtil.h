#ifndef CORBAUTIL_H_INCLUDED
#define CORBAUTIL_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/corba/src/CorbaUtil.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/02 14:28:39 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * A singleton that is used to store much of the
  * CORBA related code.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <fstream>
#include "omniORB4/CORBA.h"
#include "COS/CosNotifyChannelAdmin.hh"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	// Forward declare class
	class ServantBase;
	class ServantActivator;

	/**
	* Note that CorbaUtil is a singleton, hence the constructor is
	* private and there is a static method to retrieve a reference
	* to the single instance
	*/
	class CorbaUtil
	{
	public:
		virtual ~CorbaUtil();

		/**
		* getInstance
		*
		* Get the one and only CorbaUtil object
		*
		* @return	CorbaUtil	The only corbautil object
		*/
		static CorbaUtil& getInstance();

        /**
        * setInstance
        *
        * Set the one and only CorbaUtil object
        *
        * KT 8 Jul 03: DLLs need to use the main application's CorbaUtil instance
        * because it can't create its own "mypoa" in activate since there is already
        * one active for this process and it can't resolve the main application's 
        * "mypoa" (it gets an InvalidName exception).  
        *
        * IMPORTANT:  Don't call CorbaUtil::cleanup() if you initialise the DLL
        * CorbaUtil instance using this function, because you will clean up the
        * main application's instance.
        *
        * @param    corbaUtil   The object to point this instance to.
        */

        static void setInstance(CorbaUtil& corbaUtil);

		/**
		* initialise
		*
		* General initialisation of the ORB etc.
		*
		* If the port is zeroed then the choice of the port to run on is left
		* up to the ORB, which is how it should be most of the time. If you
		* want to pin the port you can, but this should only be done for services
		* which really need to be well known and for which use of the naming
		* service is not appropriate (eg ProcessManagers)
		*
		* @param	port	the port number to pin
		* @return	bool	true if successful
		*/
		bool initialise(int port = 0);

		/**
		* activate
		*
		* Activate the POA. This will allow you to receive incoming
		* requests on a CORBA interface
		*/
		void activate();

		/**
		* run
		*
		*This method will block indefinitely as it processes incoming CORBA
		* requests. It will return when Shutdown() is called. It is not
		* necessary to call this unless you WANT a thread to block forever
		*/
		void run();

		/**
		* stop
		*
		* This method will unblock the run method
		*/
		void stop();

		/**
		* shutdown
		*
		* Tell the ORB to stop processing incoming requests
		*/
		void shutdown();
		
		/**
		* cleanUp()
		*
		* Cleanup the ORB on completion
		*/
		static void cleanup();
		
      /**
        * stringToObject
        *
        * This method takes an ior and returns a CORBA::object_ptr.
        * The reason I have left it up to the caller to narrow the object
        * to whatever they want is to prevent the introduction of more
        * dependencies within CorbaUtil
        */
        CORBA::Object_ptr stringToObject(const std::string& ior);
		
		/**
		* exceptionToString
		*
		* There is no CORBA-compliant method for accessing the error message
		* associated with a CORBA system exception. This method wraps the
		* proprietory method
		*
		* @param	ex	The exception to stringify
		* @return	std::string	The stringify'd exception
		*/

        // TODO LPP: CorbaUtil::exceptionToString() doesn't seem to be able to convert COS 
        // (OmniNotify) exceptions to strings, even though they are derived from CORBA::Exception.  
        // Maybe they are not Corba system exceptions.  Therefore COS exceptions have to be
        // individually caught (eg. FilterNotFound, ConstraintNotFound, etc).
		static std::string exceptionToString(const CORBA::Exception& ex);

		/**
		* setServantKey
		*
		* Associate the given CORBA servant with the given key
		* (eg "ProcessManager"). This allows other processes to access the object
		* using a URI-style IOR (eg "corbaloc::localhost:6000/ProcessManager")
		*
		* @param	theServant	The servant to set
		* @param	key	The key to bootstrap
		*/
		void setServantKey(ServantBase* theServant, const std::string& key);

		/**
		* getPOA
		*
		* Return a duplicate of the servantmanaged POA
		*
		* @return	the correct POA
		*/
		PortableServer::POA_ptr getPOA();

		/**
		* getOrb
		*
		* Access to the ORB (for string_to_object() and object_to_string() mostly)
		*
		* @return	ORB	The orb
		*/
		CORBA::ORB& getOrb();

		/**
		 * commFailureExceptionHandler
		 * 
		 * This will be used to retry COMM_FAILURE_waitingForReply exceptions
		 *
		 * @param	cookie	opaque pointer..
		 * @param	retries	the number of retries for this call
		 * @param	ex	The exception
		 * @return	true to retry the exception, false to propagate exception back to client
		 */
		static CORBA::Boolean commFailureExceptionHandler( void* cookie, CORBA::ULong n_retries, const CORBA::COMM_FAILURE& ex );
        static void writeOmniToDbgLog( const char* log );
        static void writeOmniToSeparateLog( const char* log );
        void setCorbaParam(const char * paramNameValuePair);
        std::string getObjectAddress( CORBA::Object_ptr );

	private:	

		/**
		* constructor
		*
		*Constructor is private because this is a singleton
		*/
		CorbaUtil();

		/**
		* createPolicyList
		*
		* Create the policy list for the POA
		*/
		void createPolicyList(PortableServer::POA_ptr poa, CORBA::PolicyList& list);
	
		/**
		* the one and only CorbaUtil
		*/
		static CorbaUtil* m_theCorbaUtil;

		/**
		* Keep a reference to the ORB
		*/
		CORBA::ORB_var m_orbVar;

		/**
		* Store a reference to the POA
		*/
		PortableServer::POA_var m_thePOA;

		/**
		* Protects singleton from multi-threaded access
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		
		/**
		* used to sleep the main thread
		*/
		bool m_running;

		/**
		 * Used to prevent the orb being re-activated
		 */
		bool m_activated;
		
		/**
		 * track whether or not the omniORB log file has been created
		 */
		static std::fstream m_omniLogFile;
		


	}; // CorbaUtil


}; // TA_Base_Core

#endif // CORBAUTIL_H_INCLUDED

