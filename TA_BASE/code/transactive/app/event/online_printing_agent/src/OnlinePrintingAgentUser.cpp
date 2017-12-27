/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
  * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentUser.cpp $
 * @author:   Bart Golab
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 *
 */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

#include "core/threads/src/Thread.h"
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
    public:
        
        typedef void ( T::*MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
        static ThreadedMemberFunction& alloc( T& o )
        {
            instance()->clear();
            instance() = new ThreadedMemberFunction( o );
            
            return *instance();
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member functions to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o )
            : m_object( o )
        {
        }
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        static ThreadedMemberFunction*& instance()
        {
            static ThreadedMemberFunction* m_instance = NULL;
            return m_instance;
        }
        
        static void clear()
        {
            delete instance();
            instance() = NULL;
        }
        
        T& m_object;
        MemberFunctionList m_members;
    };
    
    typedef ThreadedMemberFunction< OnlinePrintingAgentUser > OnlinePrintingAgentUserThreadedMemberFunction;
}
//++limin, failover
OnlinePrintingAgentUser::OnlinePrintingAgentUser(int argCount, char* args[]) : 
    m_genericAgent(NULL),
    m_printJobSpooler(NULL),
    m_printingService(NULL),
    m_printableEventGenerator(NULL),
	m_agentEnityName("")
{
	FUNCTION_ENTRY("OnlinePrintingAgentUser(int argCount, char* args[])");

    m_genericAgent = new TA_Base_Bus::GenericAgent(argCount, args, this);

    // All configuration updates for the agent entity will be handled by ConfigData instead of GenericAgent.
    TA_Base_Core::OnlineUpdateListener::getInstance().deregisterInterest(
        TA_Base_Core::ENTITY_OWNER, *m_genericAgent, m_genericAgent->getAgentEntityConfiguration()->getKey());

    m_printJobSpooler = new PrintJobSpooler;
    m_printingService = new PrintingService(*m_printJobSpooler);
    m_printableEventGenerator = new PrintableEventGenerator(*m_printingService, m_genericAgent->getOperationMode());

    FUNCTION_EXIT;
}


OnlinePrintingAgentUser::~OnlinePrintingAgentUser()
{
	FUNCTION_ENTRY("~OnlinePrintingAgentUser()");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    ConfigData::destroyInstance();

    delete m_printableEventGenerator;
    delete m_printingService;
    delete m_printJobSpooler;
   
    if (m_genericAgent)
    {
        m_genericAgent->deactivateAndDeleteServant();
        m_genericAgent = NULL;
    }

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::agentTerminate()
{
	FUNCTION_ENTRY("agentTerminate()");
    TA_ASSERT(m_printableEventGenerator != NULL, "Printable event generator pointer is NULL");
    TA_ASSERT(m_printingService != NULL, "Printing service pointer is NULL");

    m_printableEventGenerator->terminate();
    m_printingService->terminate();

	FUNCTION_EXIT;
}


TA_Base_Bus::IEntity* OnlinePrintingAgentUser::createEntity(TA_Base_Core::IEntityDataPtr entityData)
{
	FUNCTION_ENTRY("createEntity(TA_Base_Core::IEntityData& entityData)");

    // Do nothing

	FUNCTION_EXIT;
    
    return NULL;
}


void OnlinePrintingAgentUser::agentSetMonitor()
{
	FUNCTION_ENTRY("agentSetMonitor()");
    TA_ASSERT(m_printableEventGenerator != NULL, "Printable event generator pointer is NULL");

    //limin++--, failover
    OnlinePrintingAgentUserThreadedMemberFunction& thrd = OnlinePrintingAgentUserThreadedMemberFunction::alloc( *this );
    thrd.add( &OnlinePrintingAgentUser::unsubscribeToMessages );
    thrd.add( &OnlinePrintingAgentUser::registerForStateUpdates );
    thrd.start();
    // Unsubscribe from any previous messages. State updates are the only messages OnlinePrintingAgentUser subscribes to.
    //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    m_printableEventGenerator->setOperationMode(TA_Base_Core::Monitor);

    // Register for last processed event updates.
    //registerForStateUpdates();
    //limin++--, failover

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::agentSetControl()
{
	FUNCTION_ENTRY("agentSetControl()");
    TA_ASSERT(m_printableEventGenerator != NULL, "Printable event generator pointer is NULL");

    OnlinePrintingAgentUserThreadedMemberFunction& thrd = OnlinePrintingAgentUserThreadedMemberFunction::alloc( *this );
    thrd.add( &OnlinePrintingAgentUser::unsubscribeToMessages );
    thrd.add( &OnlinePrintingAgentUser::registerForStateUpdates );
    thrd.start();
    // Unsubscribe from any previous messages. State updates are the only messages OnlinePrintingAgentUser subscribes to.
    //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    m_printableEventGenerator->setOperationMode(TA_Base_Core::Control);

    // Register for state update requests.
    //registerForStateUpdates();
    //limin++--, failover

	FUNCTION_EXIT;
}


//limin, failover
void OnlinePrintingAgentUser::unsubscribeToMessages()
{
    FUNCTION_ENTRY( "unsubscribeToMessages" );

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::notifyGroupOffline(const std::string& group)
{
	FUNCTION_ENTRY("notifyGroupOffline(const std::string& group)");

    // Do nothing

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::notifyGroupOnline(const std::string& group)
{
	FUNCTION_ENTRY("notifyGroupOnline(const std::string& group)");

    // Do nothing

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::registerForStateUpdates()
{
	FUNCTION_ENTRY("registerForStateUpdates()");        

    TA_Base_Core::EOperationMode operationMode;

    // If registerForStateUpdates() is called in the process of instantiating m_genericAgent 
    // resort to looking up the runtime parameters for the control mode.
    if (m_genericAgent != NULL)
    {
        operationMode = m_genericAgent->getOperationMode();
    }
    else
    {
        std::string opModeParam(TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE));
        operationMode = (opModeParam == RPARAM_CONTROL? TA_Base_Core::Control : TA_Base_Core::Monitor);
    }

    if (operationMode == TA_Base_Core::Control)
    {
       // In control mode, subscribe to receive requests for state updates.
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::OnlinePrintingAgentStateUpdate::OnlinePrintingAgentStateUpdateRequest, this,
            getEntityName());    
    }
    else
    {
       // In monitor mode, subscribe to receive the actual state updates.
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::OnlinePrintingAgentStateUpdate::EventProcessedUpdate, this,
            getEntityName());    
    }

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
	FUNCTION_ENTRY("receiveSpecialisedMessage()");
    TA_ASSERT(m_genericAgent != NULL, "Generic agent pointer is NULL");
    TA_ASSERT(m_printableEventGenerator != NULL, "Printable event generator pointer is NULL");

	try
	{
        // Last processed event state update
		if (TA_Base_Core::OnlinePrintingAgentStateUpdate::EventProcessedUpdate.getTypeKey().compare(message.messageTypeKey.in()) == 0)
		{
			EventSequenceNumber lastProcessedEvent;

			if ((message.stateUpdateInfo >>= lastProcessedEvent))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received state update for last processed event");

				m_printableEventGenerator->setLastProcessedEvent(lastProcessedEvent);
			}
		}
        // State update request
		else if (TA_Base_Core::OnlinePrintingAgentStateUpdate::OnlinePrintingAgentStateUpdateRequest.getTypeKey().compare(message.messageTypeKey.in()) == 0)
		{
            m_printableEventGenerator->publishState();
		}        
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Ignoring invalid state update type %s", message.messageTypeKey.in());
        }
	}
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "State update failed");
    }

	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
	FUNCTION_ENTRY("processOnlineUpdate()");

    // Should not get called as we had deregisterd GenericAgent's interests in the agent entity.
    
	FUNCTION_EXIT;
}


void OnlinePrintingAgentUser::run()
{
	FUNCTION_ENTRY("run()");
    TA_ASSERT(m_genericAgent != NULL, "Generic agent pointer is NULL");
    TA_ASSERT(m_printableEventGenerator != NULL, "Printable event generator pointer is NULL");
    TA_ASSERT(m_printingService != NULL, "Printing service pointer is NULL");

    try
    {
        m_printingService->start();
        m_printableEventGenerator->start();
        m_genericAgent->run();
    }
    catch (const OnlinePrintingAgentException& ex)
    {
        std::ostringstream msg;
        msg << ex.what();

        if (::strlen(ex.getDetails()))
        {
            msg << " (" << ex.getDetails() << ")";
        }

        LOG_EXCEPTION_DETAILS(SourceInfo, "OnlinePrintingAgentException", msg.str().c_str());
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
    }

	FUNCTION_EXIT;
}

std::string OnlinePrintingAgentUser::getEntityName()
{
	if ( m_agentEnityName == "" )
	{
		m_agentEnityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	}
		
	return m_agentEnityName;
}
