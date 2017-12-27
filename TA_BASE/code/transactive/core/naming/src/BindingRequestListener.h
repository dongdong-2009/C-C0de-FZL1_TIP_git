#if !defined(BindingRequestListener_06BF00B6_010F_46a0_A18C_AE7213E2C43E__INCLUDED_)
#define BindingRequestListener_06BF00B6_010F_46a0_A18C_AE7213E2C43E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/BindingRequestListener.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/09/14 23:40:11 $
  * Last modified by:  $Author: limin.zhu $
  *
  * The BindingRequestListener listens for BindingRequest messages
  * so that clients can resolve the NamedObjectRepository.
  *
  * It implements the named object repository interface.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/naming/IDL/src/NamedObjectRepositoryCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/BindingRequestMessageCorbaDef.h"
#include "core/utilities/src/RunParams.h"

#include "core/threads/src/IWorkItem.h"

/**
* This is used to specify the case where entity name is different from agent name
* This is to cater for KRTC RadioManagerAgent
*/
#define RPARAM_USEAGENTNAME "UseAgentName"


namespace TA_Base_Core
{
	class BindingRequestMessageSender;
    class NamingHeartbeatMonitor;
    class NamingThreadPoolSingleton;

	class BindingRequestListener 
		: public virtual POA_TA_Base_Core::NamedObjectRepositoryCorbaDef,
		  public virtual TA_Base_Core::ServantBase,
		  public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::BindingRequestMessageCorbaDef>,
		  public TA_Base_Core::RunParamUser
	{

	public:

		/** 
		  * BindingRequestListener
		  *
		  * Construct a new BindingRequestListener that will listen for binding requests for 
		  * this agent. It will also implement the NamedObjectFactory.
		  */
		BindingRequestListener( );

		/** 
		  * ~BindingRequestListener
		  *
		  * Destroy the binding request listener
		  */
		virtual ~BindingRequestListener( );

	public:

		/**
		* bind
		*
		* Bind a given name to the given object 
		*
		* @param	objRef	The object that may be resolved
		* @param	objectName	The name of the object
		*/
		void bind( CORBA::Object_ptr objRef, const std::string& objectName );

		/**
		* unbind
		*
		* Remove an object binding reference.
		*
		* @param	objectName	the named object binding to remove
		*/
		void unbind( const std::string& objectName );

		// --------------------------------------------------
		// Override for the Named object repository

	    /**
		* getObject
		*
    	* this method is called by the NamedObject library when trying to get the 
		* object pointer
		*
		* @param	objectName the name of the object required
		* @return	Object	the request object
		* @exception NameNotFound thrown when the agent does not hold the object
		* @exception OperationModeException	throw when the agent is not in control mode
	    */
		CORBA::Object_ptr getObject( const char* objectName );

		/**
		 * poll
		 *
		 * This function will be periodically called on the agent. If it fails, or throws
		 * an OperationModeException, the repository will be invalidated
		 */
		void poll();
		
		//---------------------------------------------------
		// Override for receiving the agent resolution messages

		/**
		* receiveSpecialisedMessage
		*
		* You need to override this pure virtual method in your derived class 
		*
		* @param    message	A pointer to a GenericMessage object
		*/
		virtual void receiveSpecialisedMessage(
				const TA_Base_Core::BindingRequestMessageCorbaDef& message );

		/**
		* processMessage
		*
		* You need to override this pure virtual method in your derived class 
		*
		* @param    message	A pointer to a GenericMessage object
		*/
		virtual void processMessage( TA_Base_Core::IWorkItem* item,
				const TA_Base_Core::BindingRequestMessageCorbaDef& message );

		/**
		* onRunParamChange
		*
		* Overide the runparamuser callback. This is used to notify the binding request listener
		* when the mode of the agent changes between monitor & control
		*
		* @param	name	the name of the parameter changing
		* @param	value	the value to which the parameter has change
		*/
		void onRunParamChange( const std::string& name, const std::string& value );

		/**
		* broadcastBoundObjects
		*
		* this is called when the agent enters operation mode. It will broadcast
		* all currently bound objects so that NamedObjRef can refresh
		* it's cached objects
		*
		* @param	contextOnly	True if the context has been create new and it
		*								  is in Control mode
		*/
		void broadcastBoundObjects(bool contextOnly = false);

	private:

		/**
		* doOperationModeChange
		*
		* this method will register/un-register the subscriber dependent on the 
		* agent operation mode.
		*
		* if the agent become Control, a message will be sent that contains all
		* the currently bound names
		*/
		void doOperationModeChange(bool toControl);
        void checkOperationMode(const std::string& func);

	private:

		/**
		* Stores the agentName. This is used to ensure incoming
		* binding request messages should be answered.
		*/
		std::string m_agentName;

		/**
		* Map of names to object references.
		*/
		std::map<std::string, CORBA::Object_ptr> m_bindingsMap;

		/**
		* Thread guard for adding new bindings
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_lock;

		/**
		* This is true if the agent is in control mode
		*/
		bool m_inControlMode;

		/**
		* This is the state update message sender used to send
		* rebind messages to all namedObjRefs
		*/
		TA_Base_Core::BindingRequestMessageSender* m_rebindMessageSender;

		/**
		* This is used to tell clients how long objects should be cached.
		*/
		unsigned long m_timeToLive;

		TA_Base_Core::IWorkItemPtr					m_workItem;
		std::vector<TA_Base_Core::IWorkItemPtr>		m_workItemVector;
		TA_Base_Core::NonReEntrantThreadLockable	m_workvecLock; // cl-21128 gongzong++

        // workaround for naming heartbeat, should be removed once naming issue fixed.
        NamingHeartbeatMonitor*                     m_HeartbeatMonitor;

        TA_Base_Core::NamingThreadPoolSingleton* m_namingThreadPoolSingleton;
        
        TA_Base_Core::NamedObjectRepositoryCorbaDef_var m_myObjRef;
	};

	class NamingWorkItem: public TA_Base_Core::IWorkItem
	{
	public:
		NamingWorkItem( TA_Base_Core::BindingRequestListener* callback, const TA_Base_Core::BindingRequestMessageCorbaDef& message );

		virtual void executeWorkItem();

	private:
		TA_Base_Core::BindingRequestListener* m_callback;
		const TA_Base_Core::BindingRequestMessageCorbaDef m_message;
			
	};


}; // TA_Base_Core

#endif // !defined(BindingRequestListener_06BF00B6_010F_46a0_A18C_AE7213E2C43E__INCLUDED_)
