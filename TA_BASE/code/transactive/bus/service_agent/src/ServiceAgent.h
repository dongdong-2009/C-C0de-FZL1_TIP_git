/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/service_agent/src/ServiceAgent.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last mofified by:  $Author: CM $
  *
  * Generic Agent provides a base implementation for functionality that is required
  * by all Agents within TransActive.
  */

#if !defined(SERVICEAGENT)
#define SERVICEAGENT

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <vector>

#include "core/process_management/IDL/src/IManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/src/IManagedProcessUser.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/ServantBase.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"

namespace TA_Base_Core
{
    class IEntityData; // Forward Declaration

    class ManagedProcess; // Foward Declaration

    class ConfigUpdateDetails; // Forward Declaration
}

namespace TA_Base_Bus
{
    class IEntity; // Forward Declaration

    class IGenericAgentUser; // Forward Declaration

    class ServiceAgent : //public virtual POA_TA_Base_Core::IManagedAgentCorbaDef,
                         //public virtual TA_Base_Core::IManagedProcessUser,
                         public virtual TA_Base_Core::ServantBase,
                         public virtual TA_Base_Core::IOnlineUpdatable,
                         public virtual TA_Base_Core::RunParamUser
    {

    public:

        /**
          * ensureControlMode
          *
          * This method checks the current operation mode and throws an exception if 
          * the current mode is Monitor
          *
          * @exception TA_Base_Core::OperationModeException thrown if the current operating mode is Monitor
          */
        static void ensureControlMode()
        {
            if( 0 != TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
                throw TA_Base_Core::OperationModeException();
            }
        }

    public:

        /**
          * Class constructor
          *
          * Construct the ServiceAgent, passing in any command line arguments
          * that were supplied to the agent.
          *
          * @param Number of parameters
          * @param Array of parameter values
          *
          * @exception GenericAgentException thrown if initialisation fails
          */
        ServiceAgent(int argc, char* argv[], IGenericAgentUser* registeredUser, int portNum);

        /**
          * Virtual destructor
          *
          * Delete should never be called directly. The deactivateAndDeleteServant
          * method inherited from ServantBase should be used instead.
          */
        virtual ~ServiceAgent();

        /**
          * onTerminate
          *
          * This method is called by ManagedProcess if the Process Manager
          * instructs us to terminate. The registered agents agentTerminate method
          * is called before cleaning up and terminating.
          */
        void onTerminate();

        /**
          * processUpdate
          *
          * This tells the application that one of the objects it was interested in has
          * been updated and should be processed.
          *
          * @param The update event
          */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
          * getOperationMode
          *
          * This method allows the Process Monitor to retrieve the
          * current operational state of the agent.
          *
          * @return Either Monitor or Control mode
          */
	    TA_Base_Core::EOperationMode getOperationMode();

        /**
          * setOperationMode
          *
          * This method allows the Process Monitor to set the current
          * operational mode to either control or monitor. The method sets the internal
          * state to control before calling agentSetControl or agentSetMonitor on the
          * registered agent.
          */
	    void setOperationMode(TA_Base_Core::EOperationMode mode);
		
		/**
		  * invalidateNotifyHosts
		  *
		  * This is called by the SystemController when a 
		  * peer system controller fails that is running one
		  * or more NotificationServiceAgents.
		  *
		  * @param NotifyHostSequence	the list of Notifyhosts to invalidate
		  */
		void invalidateNotifyHosts(const TA_Base_Core::IManagedAgentCorbaDef::NotifyHostSequence& notifyHosts);

		/** 
		  * notifyGroupOffline
		  *
		  * Used by the Process Monitor to inform an agent that
		  * a group is now offline.
		  */
        void notifyGroupOffline(const char* group);

		/** 
		  * notifyGroupOnline
		  *
		  * Used by the Process Monitor to inform an agent that
		  * a group is now online.
		  */
        void notifyGroupOnline(const char* group);

        /**
          * run
          * 
          * This method starts the Manged process monitoring thread 
          * before it calls agentRun. It then blocks while servicing
          * CORBA requests. The blocking call to run will return after
          * the ORB is shut down, which typically occurs as a result of
          * the Process Monitor calling terminate on the Generic Agent
          */
        void run();

        /**
          * onRunParamChange
          *
          * This method satisfies the RunParamUser interface. It is called 
          * whenever a run time parameter in which we have a registered
          * interest changes.
          *
          * @param The name of the changed run param
          * @param The value of the changed run param
          */
        void onRunParamChange(const std::string& name, const std::string& value);

        /**
         * getAgentStatus
         *
         * This returns the current state the agent is in
         *
         * @return The agent status
         */
        TA_Base_Core::EProcessStatus getAgentStatus()
        {
            return m_agentStatus;
        }

        /**
          * getAgentEntityConfiguration
          *
          * This method allows registered agents to retrieve
          * the entity data that pertains to the actual agent entity
          * (ie themselves).
          *
          * @return The agents entity configuration
          */
	    TA_Base_Core::IEntityData* getAgentEntityConfiguration();

        /**
          * getEntityMap
          *
          * This method returns a pointer to the map of entities (child entities) held
          * within ServiceAgent
          *
          * @return The Generic Agent entity map
          */
        const std::map<unsigned long, IEntity*>* getEntityMap();

        /**
          * addEntity
          *
          * This method allows safe additions to Generic Agents internal map of entities.
          */
        void addEntity( unsigned long p_entityKey, IEntity* p_entity );

        /**
          * deleteEntity
          *
          * This method allows safe deletions to Generic Agents internal map of entities.
          */
        void deleteEntity( unsigned long p_entityKey );

        /**
          * modifyEntity
          *
          * This method allows safe modifications to Generic Agents internal map of entities.
          * Internally, this method simply calls deleteEntity followed by addEntity
          */
        void modifyEntity( unsigned long p_entityKey, IEntity* p_entity );

    private:

        // Disable the copy constructor and assignment operator
        ServiceAgent( const ServiceAgent& theServiceAgent);
        ServiceAgent& operator=(const ServiceAgent &);

        /**
          * doEntityConfiguration
          *
          * This method retrieves and processes entity information for
          * the Agent Entity as well as any Child Entities.
          * 
          * This should be done during construction to ensure that registered
          * agents cant call getAgentEntityConfiguration before we have finished.
          *
          * @exception GenericAgentException raised if this fails
          */
        void doEntityConfiguration();

        /**
          * doAgentSetup
          *
          * This method is called by the ServiceAgent constructor. It's purpose
          * is to complete the ServiceAgent initialisation.
          */
        void doAgentSetup();

        /**
          * registerAsRunParamUser
          *
          * This method registers the GenericAgents interest in a number of 
          * run params. This is called from the constructor.
          */
        void registerAsRunParamUser();

        /**
          * cleanup
          *
          * This method handles the clean up that occurs after the orb is shut down
          */
        void cleanup();

        /**
          * handleUpdateForAgentEntity
          *
          * This method is called when an online update pertaining to the agent
          * entity is received.
          *
          * @param The update event
          */
        void handleUpdateForAgentEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
          * handleUpdateForChildEntity
          *
          * This method is called when an online update pertaining to one of the agents
          * child entities is received.
          *
          * @param The update event
          */
        void handleUpdateForChildEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * convertCSVStringToVectorOfStrings
         *
         * This takes a comma separated list of strings and converts it into a vector
         *
         * @param string - A comma separated list of values
         *
         * @return vector<string> - Each of the strings from the input.
         */
    	std::vector<std::string> convertCSVStringToVectorOfStrings(const std::string& inputString);

        /** 
          * outOfMemoryHandler
          *
          * Provides the standard way of handling out of memory problem
          * by throwing a bad_alloc exception. This will only be called if running on Windows.
          *
          * @param size_t size 
          *
          * @return Nothing as the bad_alloc exception will be thrown
          * 
          * @throw bad_alloc
          */
        static int outOfMemoryHandler( size_t size );

		void intitialiseUtilities(int argc, char* argv[], int portNum);
		void initialiseDebugUtil();
		void initialiseCorba(int portNum);
		void locateSystemController();
		void retrieveRunParams();

        // Map of entities that the agent is responsible for. Generic Agent takes
        // care of deleting the entity pointers.
        std::map<unsigned long,IEntity*> m_entities;

        // Current mode of operation (monitor or control)
	    TA_Base_Core::EOperationMode m_mode;

        // The current state of the agent
        TA_Base_Core::EProcessStatus m_agentStatus;

        // The agents entity data
        TA_Base_Core::IEntityData* m_agentEntityData;

        // The Managed Process
        //TA_Base_Core::ManagedProcess* m_managedProcess;

        // Child entity data (this is only kept so we can clean it up)
        std::vector<TA_Base_Core::IEntityData*> m_childEntityData;

        // The registered Generic Agent User
        IGenericAgentUser* m_registeredUser;

        // Thread lock to protect the m_entities vector
        TA_Base_Core::NonReEntrantThreadLockable m_entityMapLock;

 		// This is a pointer to the actual system controller.
		TA_Base_Core::IProcessManagerCorbaDef_var m_systemController;

        // The default value for number of retries to connect to system controller at start up
        static const unsigned int NUM_RETRIES;
    };
}

#endif // !defined(SERVICEAGENT)
