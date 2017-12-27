/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/FlexibleTimeoutNamedObject.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class provides an interface-specific wrapper for the
  * NamedObjectBase class, which just deals with CORBA::Objects
  *
  * It is templatised with the interface, ptr and var classes which are
  * generated from a CORBA interface
  *
  * Methods are provided to access the templatised type
  */

#ifndef FLEXIBLE_TIMEOUT_NAMED_OBJECT_H
#define FLEXIBLE_TIMEOUT_NAMED_OBJECT_H

#ifdef WIN32
// disable "warning C4290: C++ Exception Specification ignored"
#pragma warning(push)
#pragma warning(disable : 4290)
#endif // WIN32

#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"


//namespace TA_IRS_Core
namespace TA_Base_Core
{
	template <class CorbaDef, class CorbaDef_ptr, class CorbaDef_var> class FlexibleTimeoutNamedObject
		: public TA_Base_Core::NamedObject<CorbaDef, CorbaDef_ptr, CorbaDef_var>
	{

	public:

#		ifndef WIN32
#			define NAMEDOBJECTDEF TA_Base_Core::NamedObject<CorbaDef, CorbaDef_ptr, CorbaDef_var>::
#       else
#			define NAMEDOBJECTDEF
#		endif // WIN32

		/** 
		  * FlexibleTimeoutNamedObject
		  *
		  * Create an empty FlexibleTimeoutNamedObject so that setCorbaName can be called
		  * when necessary
		  */
		FlexibleTimeoutNamedObject() 
		{
		}

		/** 
		  * FlexibleTimeoutNamedObject
		  *
		  * Create a new FlexibleTimeoutNamedObject with the provided agentName and objectName
		  *
		  * @param  std::string& agentName
		  * @param std::string& objectName
		  */
		FlexibleTimeoutNamedObject( const std::string& agentName, const std::string& objectName )
			: TA_Base_Core::NamedObject<CorbaDef, CorbaDef_ptr, CorbaDef_var>(agentName, objectName)
		{
		}

		/** 
		  * FlexibleTimeoutNamedObject
		  *
		  * Create a new FlexibleTimeoutNamedObject with the provided corbaName
		  *
		  * @param  CorbaName& corbaName
		  */
		FlexibleTimeoutNamedObject( const TA_Base_Core::CorbaName& corbaName )
			: TA_Base_Core::NamedObject<CorbaDef, CorbaDef_ptr, CorbaDef_var>(corbaName)
		{
		}
		/** 
		  * NamedObject
		  *
		  * Create a new NamedObject with the following entity.
		  *
		  * Note - this method will lookup the agentName for the given entity using the 
		  * EntityAccessFactory. If you are creating LOTS (>100's) of NamedObject's,
		  * please do not use this method as it is not as fast as providing the necessary
		  * details/CorbaName.
		  *
		  * @param std::string& entityName
		  *	  
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
		  */
		FlexibleTimeoutNamedObject( const std::string& entityName, const bool isAgentName )
			: TA_Base_Core::NamedObject<CorbaDef, CorbaDef_ptr, CorbaDef_var>(entityName, isAgentName)
		{
		}

		/** 
		  * Destructor
		  */
		virtual ~FlexibleTimeoutNamedObject()
		{
		}

		/** 
		  * setObjectTimeout
		  *
		  * This function will allow programmers to specify specific timeouts for
		  * NamedObjects. If not called, it will default to the default timeout
		  * period (20 secs).
		  * 
		  * valid range: 0 - 300 ( 0 disables the timeout, the next call will default to the global timeout )
		  *
		  * @param unsigned short seconds, the timeout period in seconds
		  */
		virtual void setObjectTimeout( unsigned short seconds )
		{
			unsigned short timeout = seconds;

			if (seconds == 0)
			{
				try
				{
					timeout = NAMEDOBJECTDEF m_corbaDefVar->getFlexibleTimeOutValue();
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo,
						TA_Base_Core::DebugUtil::DebugDebug,
						"NamedObject::setObjectTimeout() using default timeout for [ %s, %s ]",
						NAMEDOBJECTDEF getAgentName().c_str(),
						NAMEDOBJECTDEF getObjectName().c_str() );
				}
			}
			
			// Wanted to call :
			// TA_Base_Core::NamedObject::setObjectTimeout(seconds);
			// But the compliler want allow it so I've copied the code from the method :)
			// To Do: Remove the code below and call the method with new version of compiler????

            //---------------------------------------------------------------          
			//TA_ASSERT( ( seconds >= 0 ) && ( seconds <= 300 ),
			TA_ASSERT( timeout <= 300 , "Object timeout must be in the range of 0 - 300 seconds" );

			// apply the timeout to the object reference, the timeout is in milliseconds!
			// Note that if the value is 0, this will disable the timeout ( and default to the global timeout value )
			NAMEDOBJECTDEF m_timeoutPeriod = timeout;
			// Make sure the obj reference is valid before calling. If not valid
			// cache the timeoutPeriod and apply once the object is successfully resolved
			if ( ! CORBA::is_nil( NAMEDOBJECTDEF m_corbaDefVar ) )
			{
				omniORB::setClientCallTimeout( NAMEDOBJECTDEF m_corbaDefVar, ( NAMEDOBJECTDEF m_timeoutPeriod * 1000 ) );

				// the object is now valid
				NAMEDOBJECTDEF m_varInvalid = false;
			}
			//---------------------------------------------------------------
		}

	private:
		/**
		* constructor
		*
		* Copy constructor hidden
		*/
		FlexibleTimeoutNamedObject( const FlexibleTimeoutNamedObject& );

		/**
		 * hide the assignment operator
		 */
		FlexibleTimeoutNamedObject& operator=(const FlexibleTimeoutNamedObject& );
	};

}; // namespace TA_IRS_Core

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // FLEXIBLE_TIMEOUT_NAMED_OBJECT_H
