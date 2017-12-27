/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/AbstractNamedObject.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class wrappers a CORBA object reference and associates it with
  * an agent and a name. The agent will be used to resolve the agent object
  * reference via the TDNS. Once the agent is resolved this will allow us
  * to make direct calls on the agent to resolve the necessary objects
  * 
  * The NamedObject also listens for NameRebindMessages that are sent by
  * agents when they enter control mode. This will allow the NamedObject
  * to attain a new object reference without actually needing to re-resolve
  * anything
  *
  * The NamedObject template specialises this class to deal with a single
  * interface
  */

#ifndef ABSTRACT_NAMED_OBJECT_H
#define ABSTRACT_NAMED_OBJECT_H

#ifdef WIN32
// disable "warning C4290: C++ Exception Specification ignored"
#pragma warning(push)
#pragma warning(disable : 4290)
#endif // WIN32

#include <string>
#include "INamedObject.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/naming/src/Naming.h"

namespace TA_Base_Bus
{
	class CorbaRightsLibrary;
};

namespace TA_Base_Core
{
	class ObjectResolutionException;

	class AbstractNamedObject : public INamedObject, public NamingUser
	{
		// NOTE: This is not a permanent addition. It is for tracking down rights issues
		// 4669_FAT:140
		friend class TA_Base_Bus::CorbaRightsLibrary;

	public:

		/**
		  * Destructor
		  */
		virtual ~AbstractNamedObject();

		/** 
		  * getObjectName
		  *
		  * Returns the object name of this NamedObject. Useful when trying to find the
		  * object in a list
		  *
		  * @return std::string 
		  *
		  * @param void
		  */
		std::string getObjectName( void ) const;

		/** 
		  * getAgentName
		  *
		  * Returns the Agent name of this NamedObject. Useful when trying to find the
		  * object in a list
		  *
		  * @return std::string the Agent name
		  *
		  * @param void
		  */
		virtual std::string getAgentName( void ) const;

		/** 
		  * getCorbaName
		  *
		  * Returns the full corbaName of this NamedObject
		  *
		  * @return CorbaName the name of the agent and the object
		  *
		  * @param void
		  */
		virtual CorbaName getCorbaName( void ) const;

		/** 
		  * setCorbaName
		  *
		  * Set a CorbaName, or update a NamedObject to have a new CorbaName
		  *
		  * @param  std::string& agentName - the agentName
		  * @param std::string& objectName - the objectName
		  *
		  */
		virtual void setCorbaName( const std::string& agentName, const std::string& objectName );

		/** 
		  * setCorbaName
		  *
		  * Set a CorbaName, or update a NamedObject to have a new CorbaName
		  *
		  * @param CorbaName& corbaName - the agentName and objectName
		  */
		virtual void setCorbaName( const CorbaName& corbaName );

		/**
		  * assignObject
		  *
		  * Assign a new object var as received from the NameRebind message
		  * 
		  * @param	newObject	the new Object to use
		  */
		virtual void assignObject(CORBA::Object_var newObject) = 0;

        virtual std::string getDescription() const;
        virtual const std::string& getHostname() const { return m_hostname; }
        virtual void setHostname(const std::string& hostname) { m_hostname = hostname; }

	protected:

		/** 
		  * Constructor
		  */
		AbstractNamedObject();

		/**
		  * clear
		  *
		  * Clear the cache, so the reference returns to the uninitialised state
		  */
		virtual void clear() = 0;

		/**
		  * resolveObject
		  *
		  * Force immediate resolution of the object reference from the name
		  *
		  * @exception ObjectResolutionException
		  *
		  * @return the resolved object
		  */
		CORBA::Object_var resolveObject();

		/** 
		  * handleNamingUpdate
		  *
		  * This is called by Naming when a name is modified/changed. It is 
		  * used to clear the cache (when the agent name is applicable), or
		  * clear the cache and assign the new object when the named object is valid
		  *
		  * @param  std::string& agentName The name of the agent that sent the update
		  * @param NameObjectPairSeq& namedObjectSeq The list of objects changed
		  */
		virtual void handleNamingUpdate( const std::string& agentName, 
			const NameObjectPairSeq& namedObjectSeq );

		/** 
		  * invalidateObject
		  *
		  * Called on the NamedObject when the agent has been invalidated.
		  * This will cause the cache to clear and the next request will result
		  * in a re-resolution
		  * 
		  * @param  std::string& agentName The name of the agent that has been invalidated
		  */
		virtual void invalidateObject( const std::string& agentName );

	private:

		/**
		* The agent name associated with the object. ( eg "PMSAgent_Station1" )
		*/
		std::string m_agentName;

		/**
		* The object name (eg: "Datapoint_0001" );
		*/
		std::string m_objectName;

        std::string m_hostname;
	};
}; // namespace TA_Base_Core

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // NAMED_OBJ_REF_BASE_H

