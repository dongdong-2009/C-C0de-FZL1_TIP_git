/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/INamedObject.h $
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

#ifndef INAMED_OBJECT_H
#define INAMED_OBJECT_H

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>

namespace TA_Base_Core
{
	/**
	 * A simple struct representing the location of an object in the TDNS
	 */
	struct CorbaName
	{
	public:
		CorbaName( const std::string& agentName, const std::string& objectName ) :
			m_entityKey(0),
			m_agentName(agentName),
			m_objectName(objectName),
			m_locationKey(0){}
		
		CorbaName( unsigned long entityKey, const std::string& agentName, const std::string& objectName, unsigned long locationKey ) :
			m_entityKey(entityKey),
			m_agentName(agentName), 
			m_objectName(objectName),
			m_locationKey(locationKey) {}

		unsigned long getEntityKey() const
		{
			return m_entityKey;
		}

		std::string getAgentName() const
		{
			return m_agentName;
		}

		std::string getObjectName() const
		{
			return m_objectName;
		}

		unsigned long getLocationKey() const
		{
			return m_locationKey;
		}

	private:
		/**
		 * CorbaName
		 *
		 * Hide the default constructor
		 */
		CorbaName();

		/**
		 * The entity key, Agent Name and object name, and location
	     */
		unsigned long m_entityKey;
		std::string m_agentName;
		std::string m_objectName;
		unsigned long m_locationKey;
	};

	typedef std::vector<CorbaName> CorbaNameList;

	class INamedObject
	{
	public:

		/**
		  * Destructor
		  */
		virtual ~INamedObject() {};

		/** 
		  * getObjectName
		  *
		  * Returns the object name of this NamedObject. Useful when trying to find the
		  * object in a list
		  *
		  * @return std::string the object name
		  *
		  * @param void
		  */
		virtual std::string getObjectName( void ) const = 0;

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
		virtual std::string getAgentName( void ) const = 0;

		/** 
		  * getCorbaName
		  *
		  * Returns the full corbaName of this NamedObject
		  *
		  * @return CorbaName the name of the agent and the object
		  *
		  * @param void
		  */
		virtual CorbaName getCorbaName( void ) const = 0;

		/** 
		  * setCorbaName
		  *
		  * Set a CorbaName, or update a NamedObject to have a new CorbaName. You can 
		  * change the CorbaName of a NamedObject at any time. Be aware that your new object
		  * 
		  *
		  * @param  std::string& agentName - the agentName
		  * @param std::string& objectName - the objectName
		  *
		  */
		virtual void setCorbaName( const std::string& agentName, const std::string& objectName ) = 0;

		/** 
		  * setCorbaName
		  *
		  * Set a CorbaName, or update a NamedObject to have a new CorbaName
		  *
		  * @param CorbaName& corbaName - the agentName and objectName
		  */
		virtual void setCorbaName( const CorbaName& corbaName ) = 0;

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
		virtual void setObjectTimeout( unsigned short seconds ) = 0;
	};
}; // namespace TA_Base_Core

#endif // NAMED_OBJ_REF_BASE_H

