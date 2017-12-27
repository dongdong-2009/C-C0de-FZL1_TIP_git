/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAgentUser.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyAgentUser.h"
#include "app/security/duty_agent/src/DutyAgentImpl.h"
#include "app/security/duty_agent/src/MonitorExecutive.h"
#include "app/security/duty_agent/src/ControlExecutive.h"

#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/ThreadMemberFunction.h"

TA_Base_Bus::DASType::DbKey TA_Base_Bus::TransactiveAgentInfo::m_entity     = 0;
std::string            TA_Base_Bus::TransactiveAgentInfo::m_name       = "";
TA_Base_Bus::DASType::DbKey TA_Base_Bus::TransactiveAgentInfo::m_entityType = 0;
TA_Base_Bus::DASType::DbKey TA_Base_Bus::TransactiveAgentInfo::m_subsystem  = 0;
TA_Base_Bus::DASType::DbKey TA_Base_Bus::TransactiveAgentInfo::m_location   = 0;
TA_Base_Bus::DASType::DbKey TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout = 0;
TA_Base_Bus::IDutyAgentRespCorbaDef_var TA_Base_Bus::TransactiveAgentInfo::m_responseObjRef = 0;
TA_Base_Bus::DASConst::PolicyType TA_Base_Bus::TransactiveAgentInfo::m_policyType = TA_Base_Bus::DASConst::POLICY_TYPE_ROAD;
std::string			   TA_Base_Bus::TransactiveAgentInfo::m_assetName	= "";


//++limin, failover
namespace TA_Base_App
{
	
	typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::DutyAgentUser > AgentThreadedMemberFunction;

    DutyAgentUser::DutyAgentUser(int argc, char* argv[]) : 
        m_genericAgent(NULL),
		m_dutyAgentImpl(NULL),
		m_bulkUpdateComplete(0)
    {
		FUNCTION_ENTRY("DutyAgentUser");

		m_genericAgent = new TA_Base_Bus::GenericAgent(argc,argv,this,NULL,AGENT_USER_TYPE_NORMAL);

		TA_ASSERT( NULL != m_genericAgent, "m_genericAgent == NULL" );

		try
		{
			TA_Base_Core::DutyAgentEntityDataPtr entityData =
				boost::dynamic_pointer_cast<TA_Base_Core::DutyAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());

			TA_Base_Bus::TransactiveAgentInfo::m_entity     = entityData->getKey();
			TA_Base_Bus::TransactiveAgentInfo::m_name       = entityData->getName();
			TA_Base_Bus::TransactiveAgentInfo::m_entityType = entityData->getTypeKey();
			TA_Base_Bus::TransactiveAgentInfo::m_subsystem  = entityData->getSubsystem();
			TA_Base_Bus::TransactiveAgentInfo::m_location   = entityData->getLocation();
			TA_Base_Bus::TransactiveAgentInfo::m_assetName  = entityData->getAssetName();

			TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout = entityData->getUncontrolledTimeout();

			if( TA_Base_Core::DutyAgentEntityData::RAIL_POLICY_VALUE == entityData->getPolicyType() )
			{
				TA_Base_Bus::TransactiveAgentInfo::m_policyType = TA_Base_Bus::DASConst::POLICY_TYPE_RAIL;
			}
			else if( TA_Base_Core::DutyAgentEntityData::ROAD_POLICY_VALUE == entityData->getPolicyType() )
			{
				TA_Base_Bus::TransactiveAgentInfo::m_policyType = TA_Base_Bus::DASConst::POLICY_TYPE_ROAD;
			}
			else
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Invalid policy (%s) defaulting to Rail.", entityData->getPolicyType().c_str() );
			}


		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(),
				"Entity configuration retrieval failed." );
			TA_ASSERT( false, "Unable to continue without valid entity data" );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception",
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(),
				"Entity configuration retrieval failed." );
			TA_ASSERT( false, "Unable to continue without valid entity data" );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Entity configuration retrieval failed." );
			TA_ASSERT( false, "Unable to continue without valid entity data" );
		}

		FUNCTION_EXIT;
    }

    DutyAgentUser::~DutyAgentUser()
    {
		FUNCTION_ENTRY("~DutyAgentUser");

        if (NULL != m_genericAgent)
        {
            m_genericAgent->deactivateAndDeleteServant();
        }

        if (NULL != m_dutyAgentImpl)
        {
            m_dutyAgentImpl->deactivateAndDeleteServant();
        }

		FUNCTION_EXIT;
    }

	void DutyAgentUser::notifyGroupOffline( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOffline");

		try
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Group Offline: %s", group.c_str() );
			if( TA_Base_Bus::DutyDbCache::instance()->locationIsOcc(group) )
			{
				m_dutyAgentImpl->localDegrade();
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Notify Group Offline failed." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Notify Group Offline failed." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Notify Group Offline failed." );
		}

		FUNCTION_EXIT;
	}

	void DutyAgentUser::notifyGroupOnline ( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT;
	}

    void DutyAgentUser::agentTerminate()
    {
		FUNCTION_ENTRY("agentTerminate");

		try
		{
			TA_Base_Bus::AgentTerminateSubject::instance()->update();

			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

			if (NULL != m_dutyAgentImpl)
			{
				m_dutyAgentImpl->deactivateAndDeleteServant();
				m_dutyAgentImpl = NULL;
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
		}

		FUNCTION_EXIT;
    }

    TA_Base_Bus::IEntity* DutyAgentUser::createEntity( TA_Base_Core::IEntityDataPtr entityData )
    {
		FUNCTION_ENTRY("createEntity");
		FUNCTION_EXIT;
        return NULL;
    }

    void DutyAgentUser::agentSetMonitor()
    {
		FUNCTION_ENTRY("agentSetMonitor");

		m_mode = RPARAM_MONITOR;
		
		TA_Base_Bus::AgentModeSubject::instance()->update(RPARAM_MONITOR);

		try
		{
            //limin++--, failover
            AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
            thrd.add( &DutyAgentUser::unsubscribeMessages );
            thrd.add( &DutyAgentUser::registerForMonitorMessages );
            thrd.start();
			//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        
			IDutyAgentExecutive* newExec = new MonitorExecutive;
			m_dutyAgentImpl->setExecutive( newExec );
			m_dutyAgentImpl->localStopDutyMonitor();
		
			//registerForMonitorMessages();
            //limin++--, failover
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(),
				"Setting Agent to Monitor Mode failed." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception",
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(),
				"Setting Agent to Monitor Mode failed." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Setting Agent to Monitor Mode failed." );
		}

		FUNCTION_EXIT;
    }

    void DutyAgentUser::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");

		m_mode = RPARAM_CONTROL;

		TA_Base_Bus::AgentModeSubject::instance()->update(RPARAM_CONTROL);

		try
		{
            //limin++--, failover
            AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
            thrd.add( &DutyAgentUser::unsubscribeMessages );
            thrd.add( &DutyAgentUser::registerForControlMessages );
            thrd.start();
			//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

			IDutyAgentExecutive* newExec = new ControlExecutive;
			m_dutyAgentImpl->setExecutive( newExec );
			m_dutyAgentImpl->localStartDutyMonitor();

			//registerForControlMessages();
            //limin++--, failover
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(),
				"Setting Agent to Control Mode failed." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception",
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(),
				"Setting Agent to Control Mode failed." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Setting Agent to Control Mode failed." );
		}

		FUNCTION_EXIT;
    }

    //limin, failover
    void DutyAgentUser::unsubscribeMessages()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }
    void DutyAgentUser::registerForControlMessages()
    {
		FUNCTION_ENTRY("registerForControlMessages");

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::DutyStateUpdate::DutyAgentStateRequest, this,
			getAgentEntityName());

		FUNCTION_EXIT;
    }

	void DutyAgentUser::registerForMonitorMessages()
    {
		FUNCTION_ENTRY("registerForMonitorMessages");

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::DutyStateUpdate::DutyAgentStateUpdate, this,
            getAgentEntityName());

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::DutyStateUpdate::DutyAgentBulkUpdate, this,
            getAgentEntityName());

		FUNCTION_EXIT;
    }


    void DutyAgentUser::registerForStateUpdates()
    {
		FUNCTION_ENTRY("registerForStateUpdates");

		try
		{
			if( RPARAM_CONTROL == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE) )
			{
                //limin++--, failover
                AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
                thrd.add( &DutyAgentUser::registerForControlMessages );
                thrd.start();
				//registerForControlMessages();
                //limin++--, failover
			}
			else
			{
                //limin++--, failover
                AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
                thrd.add( &DutyAgentUser::registerForMonitorMessages );
                thrd.start();
				//registerForMonitorMessages();
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(),
				"Registration for state updates failed." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception",
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(),
				"Registration for state updates failed." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Registration for state updates failed." );
		}


		FUNCTION_EXIT;

    }

    void DutyAgentUser::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");

		try
		{
			// Update Request
			if(  0 == TA_Base_Core::DutyStateUpdate::DutyAgentStateRequest.getTypeKey().compare(message.messageTypeKey) )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received DutyAgentStateRequest" );
				
				while(true)
				{
					if( m_dutyAgentImpl  == NULL)
					{
						TA_Base_Core::Thread::sleep(100);
					}
					else
					{
						break;
					}
				}
				m_dutyAgentImpl->localSendBulkUpdate();
				return;
			}

			// Bulk OR Incremental Update
			if( (0 == TA_Base_Core::DutyStateUpdate::DutyAgentStateUpdate.getTypeKey().compare(message.messageTypeKey)) ||
				(0 == TA_Base_Core::DutyStateUpdate::DutyAgentBulkUpdate.getTypeKey().compare (message.messageTypeKey)) )
			{
				TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence* stateUpdate = NULL;

				if( (message.stateUpdateInfo >>= stateUpdate) != NULL )
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received state update" );
					
					TA_Base_Bus::DutyState dutyState;

					dutyState.set(*stateUpdate);

					TA_Base_Bus::StateUpdateSubject::instance()->update(dutyState);

					if( 0 == TA_Base_Core::DutyStateUpdate::DutyAgentBulkUpdate.getTypeKey().compare(message.messageTypeKey) )
					{
						m_bulkUpdateComplete.release();
					}
				}
				return;
			}

		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "State update failed." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception",
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "State update failed." );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "State update failed." );
		}
		
		FUNCTION_EXIT;
    }


    void DutyAgentUser::processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
		FUNCTION_ENTRY("processOnlineUpdate");

		if( m_mode != RPARAM_CONTROL )
		{
			FUNCTION_EXIT;
			return;
		}

		std::vector<std::string> changedParams;

		changedParams = updateEvent.getChangedParams();

		std::vector<std::string>::const_iterator it;
		
		it=changedParams.begin();
		bool foundTimeoutUpdate = false;

		while( !foundTimeoutUpdate && it!=changedParams.end() )
		{
			if( TA_Base_Core::DutyAgentEntityData::UNCONTROLLED_TIMEOUT_NAME == *it )
			{
				foundTimeoutUpdate = true;

				TA_Base_Core::DutyAgentEntityDataPtr entityData =
					boost::dynamic_pointer_cast<TA_Base_Core::DutyAgentEntityData>
					(m_genericAgent->getAgentEntityConfiguration());

				entityData->invalidate();

				if( entityData->getUncontrolledTimeout() != TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout )
				{
					m_dutyAgentImpl->localStopDutyMonitor();

					TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout = entityData->getUncontrolledTimeout();

					if( 0 != TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout )
					{
						m_dutyAgentImpl->localStartDutyMonitor();
					}
				}
			}
			it++;
		}
		
		FUNCTION_EXIT;
    }

    void DutyAgentUser::run()
    {
		FUNCTION_ENTRY("run");

		IDutyAgentExecutive* newExec = NULL;

		m_mode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);

		m_dutyAgentImpl = new DutyAgentImpl( getAgentEntityName());

		TA_Base_Bus::AgentModeSubject::instance()->update(m_mode);
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"operation mode : %s", m_mode.c_str());

		if( RPARAM_CONTROL == m_mode )
		{
			newExec = new ControlExecutive;
			m_dutyAgentImpl->localStartDutyMonitor();
		}
		else
		{
			newExec = new MonitorExecutive;
		}

		m_dutyAgentImpl->setExecutive( newExec );

		if( RPARAM_CONTROL != m_mode )
		{
			m_dutyAgentImpl->localSendBulkUpdateRequest();
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Waiting for bulk state update processing to complete" );

			m_bulkUpdateComplete.acquire();
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Bulk state update processing complete" );
		}

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before m_genericAgent run");
		m_genericAgent->run();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after m_genericAgent run");

		FUNCTION_EXIT;
    }
}
