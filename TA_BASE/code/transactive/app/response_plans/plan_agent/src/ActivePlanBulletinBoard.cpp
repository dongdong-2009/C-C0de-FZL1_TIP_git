/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  */

#include <algorithm>
#include "ace/OS.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"
#include "app/response_plans/plan_agent/src/SortablePlanId.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/common/src/StringConstants.h"

const time_t NETWORK_DELAY = 3u;  //seconds allowance for a network delay -feel free to adjust if proven unsuitable
const char* const DEFAULT_ASSOC_ALARM_ID = "defaultId"; //default value for associated alarm ID - only those plans which raise alarms will 
											//have this value set to the one supplied by alarm agent (unique for unique alarms)
const char* const PLAN_EDIT_LOCK_SEPARATOR = ":";

// This needs to exist so we can put ActivePlandId in a std::map

namespace TA_Base_App
{
	const int ActivePlanBulletinBoardImpl::INDICATION_SESSION_AND_PMID = 36;

	ActivePlanBulletinBoardImpl::ActivePlanBulletinBoardImpl()
	{
		m_updateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );
		m_pasUpdateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );

		m_hangupMessageReceived = false;
		m_isControlMode = false;
		m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::PlanAgentBroadcastComms::Context );
        m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender ( TA_Base_Core::PlanAgentStateUpdate::Context );

		TA_Base_Bus::RightsLibraryFactory rlf;

		m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL);
        m_authenticationLib = new TA_Base_Bus::AuthenticationLibrary();

        TA_Base_Core::IResource* pRes = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( PlanAgentUser::getInstance()->getAgentEntityKey() );
        m_resourceId = pRes->getKey();
        delete pRes;
	}

	ActivePlanBulletinBoardImpl::~ActivePlanBulletinBoardImpl()
	{
		deleteMessageSenders ();
		deleteRightsLib ();
	}

	void ActivePlanBulletinBoardImpl::deleteMessageSenders()
	{
		if ( NULL != m_commsSender )
		{
		    delete m_commsSender;
		    m_commsSender = NULL;
		}

		if ( NULL != m_stateSender )
		{
            delete m_stateSender;
		    m_stateSender = NULL;
		}
	}

	void ActivePlanBulletinBoardImpl::deleteRightsLib ()
	{
		if ( NULL != m_rightsLibrary )
		{
			delete m_rightsLibrary;
			m_rightsLibrary = NULL;
		}

        if ( NULL != m_authenticationLib )
        {
            delete m_authenticationLib;
            m_authenticationLib = NULL;
        }
	}

	bool ActivePlanBulletinBoardImpl::wasHangupMessageReceived(void)
	{
		return m_hangupMessageReceived;
	}

	void ActivePlanBulletinBoardImpl::setHangupMessageReceived(void)
	{
		m_hangupMessageReceived = true;
	}

	bool ActivePlanBulletinBoardImpl::isActionPermitted(const std::string& session, unsigned long actionKey, std::string& reason)
	{
        return isActionPermitted( session, m_resourceId, actionKey, reason );
	}

	bool ActivePlanBulletinBoardImpl::isActionPermitted(const std::string& session, unsigned long resourceKey, unsigned long actionKey, std::string& reason)
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        bool bActionPermitted = false;

		try
        {
            if ( NULL != m_rightsLibrary )
            {
                bActionPermitted = m_rightsLibrary->isActionPermittedOnResource( session.c_str(), resourceKey, actionKey, reason, decisionModule);
            }
		}
        catch (const TA_Base_Core::RightsException& ex)
        {
			reason = "Error: Rights Exception. ";
            reason += ex.what();
        }
		catch ( ... )
		{
			reason = "Error: The Rights Library could not determine the access rights.";
        }

        return bActionPermitted;
	}

    bool ActivePlanBulletinBoardImpl::isControlMode(void)
    {
        return m_isControlMode;
    }

	void ActivePlanBulletinBoardImpl::setControlMode( bool controlMode )
	{
		std::string message;
		if ( controlMode )
		{
			if ( NULL == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate);
			}
			m_isControlMode = true;

			message = "Plan Agent has been put into CONTROL MODE.";
		}
		else
		{
			if ( NULL == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest);
			}
			m_isControlMode = false;
			message = "Plan Agent has been put into MONITOR MODE.";
            // Move state update request to PlanAgentUser::agentSetMonitor
// 			sendStateUpdateRequestToControlAgent();
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str());
	}

    bool ActivePlanBulletinBoardImpl::isRegisteredManager(const std::string& session, const std::string& pmId)
    {
		std::string compositePmId = session + pmId;
        bool bRegistered = false;

        {
		    ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
            bRegistered = ( m_pmObjRef.find( compositePmId ) != m_pmObjRef.end() );
        }
		
        return bRegistered;
	}

	void ActivePlanBulletinBoardImpl::registerPlanManager(const std::string& session, const std::string& pmId, TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerRef)
	{
		TA_Base_Bus::IPlanManagerCorbaDef_ptr myRef = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate(planManagerRef);
        std::string compositePmId = session + pmId;

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
			PMOBJREFMapIterator it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				CORBA::release(it->second);
				m_pmObjRef.erase(compositePmId);
			}

			m_pmObjRef.insert(PMOBJREFMap::value_type(compositePmId,myRef));
		}

        //  TD 10849 Reassign all plans for this user to the new Plan Manager's session key
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);

        {
            ACE_Write_Guard<ACE_RW_Mutex> planMapGuard(m_planMapLock);
            for (PlanMapIterator planIt =  m_planMap.begin(); planIt!= m_planMap.end(); planIt++)
            {
                if ( userKey == planIt->second.m_userKey )
                {
                    if ( NULL == m_authenticationLib )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
                        break;
                    }

					try
					{
						if( m_authenticationLib->getSessionInfo(session, session).WorkstationId != 
							m_authenticationLib->getSessionInfo(planIt->second.m_session, planIt->second.m_session).WorkstationId )
						{
							continue;
						}
					}
					catch(...)
					{
						 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Exception catch when try find workstatinid of session %s and session %s", session.c_str(), planIt->second.m_session.c_str());
						 continue;
					}

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan [%lu, %lu] current [SessionId:%s, PmId:%s].", planIt->second.m_detail.plan.plan, planIt->second.m_detail.plan.instance, planIt->second.m_session.c_str(), planIt->second.m_pmId.c_str());
				    planIt->second.m_session = session;
				    planIt->second.m_pmId = pmId;
		            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan [%lu, %lu] reassigned to [SessionId:%s, PmId:%s].", planIt->second.m_detail.plan.plan, planIt->second.m_detail.plan.instance, session.c_str(), pmId.c_str() );
                }
            }
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PlanManager[SessionId:%s, PmId:%s] got registered.", session.c_str(), pmId.c_str() );
		if ( !m_isControlMode )
        {
			return;
        }

		//Send State update to the PlanAgent in monitor mode
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::RegisterPlanManagerUpdate_var registerPMUpdate = new TA_Base_Core::RegisterPlanManagerUpdate;
		registerPMUpdate->session = session.c_str();
		registerPMUpdate->pmId = pmId.c_str();
		registerPMUpdate->planManager = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate(planManagerRef);
		pasUpdateData->update.planManager(registerPMUpdate);

		try
		{
			sendStateUpdateMessage(pasUpdateData);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate for PlanManager[SessionId:%s, PmId:%s] Registration has been sent.", session.c_str(), pmId.c_str());
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: StateUpdate for PlanManager[SessionId:%s, PmId:%s] Registration could not be sent.", session.c_str(), pmId.c_str());
		}
	}

	void ActivePlanBulletinBoardImpl::deregisterPlanManager(const std::string& session, const std::string& pmId)
	{
        std::string compositePmId = session + pmId;

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
			PMOBJREFMapIterator it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				CORBA::release(it->second);
				m_pmObjRef.erase(it);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Manager[SessionId:%s] got deregistered.", session.c_str() );
			}
			else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Plan Manager[SessionId:%s] is not registered hence cannot be deregistered.", session.c_str() );
            }
		}

		if ( !m_isControlMode )
        {
			return;
        }

        TA_Base_Core::DeregisterPlanManagerUpdate dpmu;
        dpmu.session = session.c_str();
        dpmu.pmId = pmId.c_str();

		//Send State update to the PlanAgent in monitor mode
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.deregisterPlanManager(dpmu);

		try
		{
			sendStateUpdateMessage(pasUpdateData);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate for Plan Manager[SessionId:%s] Deregistration has been sent.", session.c_str());
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: StateUpdate for Plan Manager[SessionId:%s] Deregistration could not be sent.", session.c_str());
		}
	}

	void ActivePlanBulletinBoardImpl::getPlanManagerObjRef( const TA_Base_Core::ActivePlanId& plan, TA_Base_Bus::IPlanManagerCorbaDef_ptr& pmCorbaRef)
	{
		pmCorbaRef = NULL;
		std::string session = getSession( plan );
        std::string pmId = getPlanManagerId(plan);

        std::string compositePmId = session + pmId;

        ACE_Read_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);

		PMOBJREFMapIterator it = m_pmObjRef.find( compositePmId );

		if( it != m_pmObjRef.end() )
		{
			pmCorbaRef = it->second;
			return;
		}

        //Could not find PMObjRef for given sessionId and pmId, so search for another PM which has the same sessionId
        std::string compositeId = "";
        for ( it = m_pmObjRef.begin(); it != m_pmObjRef.end(); it++ )
        {
            compositeId = it->first;

            int pos = compositeId.find(session);

            if ( pos == 0 )
            {                
                pmCorbaRef = it->second;

                int sessionLen = session.length();
                std::string pmId = compositeId.substr(sessionLen, compositeId.length()-sessionLen); 
                setPlanManagerId(plan, pmId);
            }
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Manager reference not found for [SessionId:%s] and Active Plan[P:%u,I:%u]", session.c_str(), plan.plan, plan.instance );
	}

    std::string ActivePlanBulletinBoardImpl::getOperatorName( const TA_Base_Core::ActivePlanId& plan )
    {
        return getOwner(getSession( plan ) );
    }


	std::string ActivePlanBulletinBoardImpl::getOwner( const std::string& idString )
	{
        std::string session = idString;

        int pos = idString.find(PLAN_EDIT_LOCK_SEPARATOR);

        if ( pos != std::string::npos )
        {
            session = idString.substr(0, pos);
        }

        std::string owner = "session: " + session;

        if ( NULL == m_authenticationLib )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return owner;
        }

        try 
        {
            // The format is "<operator name> (<base profile>[ / <override profile>])"
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(session, session);
		    std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));

            std::string profile("");

            for (std::vector<unsigned long>::const_iterator pIt = sessionInfo.ProfileId.begin(); pIt != sessionInfo.ProfileId.end(); pIt++)
            {
		        std::auto_ptr<TA_Base_Core::IProfile> iProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*pIt, false));

                if (!profile.empty())
                {
                    profile += " / ";
                }

                profile += iProfile->getName();
            }

            if ( profile.empty() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Session has no valid profile associated with it" );
            }

            std::ostringstream opNameAndProfile;
            opNameAndProfile << iOperator->getName() << " (" << profile << ")";

		    owner = opNameAndProfile.str().c_str();
        }
        catch(const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
        }
        catch ( ... )
        {
            std::string message = "Operator name and profile could not be retrieved from session id[";
            message += session;
            message += "]. Constructing default owner from session id.";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, message.c_str() );
        }

		return owner;
	}

	unsigned long ActivePlanBulletinBoardImpl::getOwnerId( const std::string& idString )
	{
        std::string session = idString;

        int pos = idString.find(PLAN_EDIT_LOCK_SEPARATOR);

        if ( pos != std::string::npos )
        {
            session = idString.substr(0, pos); 
        }

        unsigned long ownerId;
        
        if (getUserKeyFromSession(session, ownerId))
        {
            return ownerId;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
             "Failed to convert session to operator key, returning 0");

		return 0;
	}

    bool ActivePlanBulletinBoardImpl::getUserKeyFromSession( const std::string& session, unsigned long& userKey )
    {
        if (( NULL == m_authenticationLib ) || session.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return false;
        }

        try
        {
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(session, session);
            userKey = sessionInfo.UserId;
            return (!sessionInfo.OperatorId.empty());
        }
        catch(const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception" );
        }

        return false;
    }

	bool ActivePlanBulletinBoardImpl::isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan)
	{
		std::string sessionId = getSession( plan );

		if ( sessionId.compare(session) == 0 )
        {
			return true;
        }

        if ( NULL == m_authenticationLib )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return false;
        }

		try
		{
			if(m_authenticationLib->getSessionInfo(session, session).WorkstationId != 
				m_authenticationLib->getSessionInfo(sessionId, sessionId).WorkstationId)
			{
				return false;
			}
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Exception catch when try find workstatinid of session %s and session %s",
				session.c_str(), sessionId.c_str());
			return false;
		}
        unsigned long userKey = 0xFFFFFFFF;
        if ( getUserKeyFromSession( session, userKey) )
        {
            if ( userKey == getUserKey(plan) )
            {
                setSession( plan, session );
                return true;
            }
        }

		return false;
	}

	unsigned long ActivePlanBulletinBoardImpl::getUserKey( const TA_Base_Core::ActivePlanId& plan )
	{
		unsigned long rval = 0;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			ConstPlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = it->second.m_userKey;
			}
			else
			{
				std::ostringstream what;
				what << "Plan:[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getUserKey [P:%lu,I:%lu]: userKey = %lu.", plan.plan, plan.instance, rval );

		return rval;
	}

    void ActivePlanBulletinBoardImpl::setPlanManagerId( const TA_Base_Core::ActivePlanId& plan, const std::string& pmId )
    {
        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				it->second.m_pmId = pmId;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::setPlanManagerId():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setPlanManagerId [P:%lu,I:%lu]: Plan Manager ID = %s.", plan.plan, plan.instance, pmId.c_str() );
    }

    void ActivePlanBulletinBoardImpl::setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			PlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				it->second.m_session = session;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::setSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, session.c_str() );
	}

	std::string ActivePlanBulletinBoardImpl::getSession( const TA_Base_Core::ActivePlanId& plan ) const
	{
		std::string rval = "";

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
			ConstPlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = it->second.m_session;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, rval.c_str() );

		return rval;
	}

    std::string ActivePlanBulletinBoardImpl::getPlanManagerId( const TA_Base_Core::ActivePlanId& plan ) const
    {
		std::string rval = "";

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			ConstPlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = it->second.m_pmId;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getPlanManagerId():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getPlanManagerId [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, rval.c_str() );

		return( rval );
    }

	bool ActivePlanBulletinBoardImpl::isActive( const unsigned long plan, const TA_Base_Core::ActivePlanId& ignoreInstance ) const
	{
		bool active = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            ConstPlanMapIterator itLower;
            ConstPlanMapIterator itUpper;
            ConstPlanMapIterator it;
            getInstances( plan, itLower, itUpper );
			it = itLower;

			while( (!active) && (itUpper != it) )
			{
				// check that it is not the instance of the plan that we
				// should be ignoring

				if( it->second.m_detail.plan.plan != ignoreInstance.plan )
				{
					active = it->second.isActive();
				}
				else
				{
					if( it->second.m_detail.plan.instance != ignoreInstance.instance )
					{
						active = it->second.isActive();
					}
				}

				++it;
	        }
		}

		// No PlanNotFoundException as asking if a non-existant plan is active is a valid question.
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: %s active.", plan, (active?"is":"is NOT") );

		return active;
	}

    bool ActivePlanBulletinBoardImpl::isActive( const TA_Base_Core::ActivePlanId& plan ) const
    {
        bool active = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            ConstPlanMapIterator itFound = m_planMap.find( plan );

            if ( itFound != m_planMap.end() )
            {
                active = itFound->second.isActive();
            }
        }

        return active;
    }
    
    bool ActivePlanBulletinBoardImpl::isAllPlanStopped() const
    {
        bool bActive = true;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            for ( ConstPlanMapIterator itLoop = m_planMap.begin(); m_planMap.end() != itLoop; ++itLoop )
            {
                if ( itLoop->second.isActive() )
                {
                    bActive = false;
                    break;
                }
            }
        }

        return bActive;
    }

	TA_Base_Core::ExecutionHistory* ActivePlanBulletinBoardImpl::getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate)
	{
        TA_Base_Core::ExecutionHistory_var history = new TA_Base_Core::ExecutionHistory();
        unsigned long updateId = getNextUpdateId();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;

				what << "ActivePlanBulletinBoardImpl::getPlanExecutionHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}

		    int totalESteps = it->second.m_exeHistoryMap.size();

		    history->activeUpdateId = updateId;
		    history->details.length(totalESteps);
            ExeHistoryMapIterator hIt = it->second.m_exeHistoryMap.begin();

		    for ( int i = 0; i < totalESteps; i++ )
		    {
			    history->details[i] = (*hIt).second;
			    hIt++;
            }
        }

        if ( bSendIdUpdate )
        {
            //Send StateUpdate for change in m_updateId
            sendStateUpdateMessage(updateId);
        }

		return history._retn();
	}


	TA_Base_Core::CustomisationHistory* ActivePlanBulletinBoardImpl::getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate)
    {
        TA_Base_Core::CustomisationHistory_var history = new TA_Base_Core::CustomisationHistory();
        unsigned long updateId = getNextUpdateId();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getPlanCustomisationHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}

		    int totalCSteps = it->second.m_custHistoryMap.size();

		    history->customUpdateId = updateId;
		    history->details.length(totalCSteps);
		    CustHistoryMapIterator cIt = it->second.m_custHistoryMap.begin();

		    for ( int i = 0; i < totalCSteps; i++ )
		    {
			    history->details[i] = (*cIt).second;
			    cIt++;
            }
        }

        if ( bSendIdUpdate )
        {
            //Send StateUpdate for change in m_updateId
            sendStateUpdateMessage( updateId );
        }

		return history._retn();
	}

	void ActivePlanBulletinBoardImpl::getActiveInstances( const unsigned long plan, std::list<TA_Base_Core::ActivePlanId>& lstActiveInstances ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        ConstPlanMapIterator itLower;
        ConstPlanMapIterator itUpper;
		getInstances( plan, itLower, itUpper );

        lstActiveInstances.clear();
		for( ConstPlanMapIterator it = itLower; it != itUpper; ++it )
		{
			if( it->second.isActive() )
			{
				lstActiveInstances.push_back( it->second.m_detail.plan );
			}
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: Found %lu instance(s).", plan, lstActiveInstances.size() );
	}

    bool ActivePlanBulletinBoardImpl::existsPlan(const TA_Base_Core::ActivePlanId& plan)
    {
//Note: This is already being called from SendUpdate which has acquired the lock

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        if ( m_planMap.find(plan) != m_planMap.end() )
        {
            return true;
        }

        return false;
    }


	 bool ActivePlanBulletinBoardImpl::isGRCPlan (const TA_Base_Core::ActivePlanId& plan)
	 {
		 std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan);
		 //for GRC plans assumption is they all come from GRC category
		 //this is enforced (to an extent) by displaying only GRC category for 
		 //automatic plan execution (from scheduler and GRC command menu) 
		 std::string rootCategory = planPath.substr (0,5);

		 if (rootCategory.compare ("/GRC/") == 0)
		 {
			 //we want an additional check done here to see if the 
			 //plan contains any BRC steps (to the best of my knowledge 
			 //the only step which is considered BRC at the moment is 
			 //DATA_POINT_SET_STEP simply add more 
			 //cases if more step types turn out to be BRC)
			 TA_Base_Core::StepDetails_var stepDetails = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( plan.plan);
			 for (unsigned int i = 0; i < stepDetails->length (); i++)
			 {
				 TA_Base_Core::EStep	stepType = stepDetails [i].type;
				 switch (stepType)
				 {
				 case TA_Base_Core::DATA_POINT_SET_STEP:
					 return true;
				 default:
					 continue;//just keep cheking steps
				 }
			 }

			 //if no BRCs found then this is not a GRC Plan
			 return false;
		 }
		 else
		 {
			 return false;
		 }
	 }


	 bool ActivePlanBulletinBoardImpl::isManualPlan(const TA_Base_Core::ActivePlanId& plan)
	 {
         bool bManual = false;

         {
             ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
             ConstPlanMapIterator it = m_planMap.find( plan );

             if( it != m_planMap.end() )
             {
                 bManual = it->second.m_detail.manuallyExecuted;
             }
             else
             {
                 std::ostringstream what;
                 what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                 TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
             }
         }

		 return bManual;
	 }

    const TA_Base_Core::ActivePlanDetail&  ActivePlanBulletinBoardImpl::getActivePlanDetail( const TA_Base_Core::ActivePlanId& plan ) const
    {
		// get the list instances
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        ConstPlanMapIterator itFound = m_planMap.find(plan);

        if ( itFound == m_planMap.end() )
        {
            std::ostringstream what;
            what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
        }

        return itFound->second.m_detail;
    }


	TA_Base_Core::ActivePlanDetails* ActivePlanBulletinBoardImpl::getActivePlanDetail( unsigned long plan ) const
	{
		TA_Base_Core::ActivePlanDetails_var apd = new TA_Base_Core::ActivePlanDetails;

		// get the list instances
		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan, itLower, itUpper );

			PlanMap::difference_type instances = std::distance( itLower, itUpper);

			if( 0 >= instances )
			{
				apd->length(0);
			}
			else
			{
				apd->length(instances);

				int i = 0;

				for( it = itLower; it != itUpper; ++it )
				{
					apd[i] = it->second.m_detail;
					++i;
				}
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: Found details for %lu instances.", plan, apd->length() );

		// This method does not throw PlanNotFoundException as it is called
		// by the Plan Manager for each plan a user selects (active or not).
		// It is therefore valid to have a zero length list of plan details
		// returned.

		return apd._retn();
	}

	void ActivePlanBulletinBoardImpl::setAssociatedAlarmId (const TA_Base_Core::ActivePlanId& plan, const std::string& alarmId)
	{
        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            PlanMapIterator it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
	          it->second.m_detail.assocAlarmId = alarmId.c_str();
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getState() : [P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}

	const TA_Base_Core::ActivePlanDetail& ActivePlanBulletinBoardImpl::getAssocAlarmActivePlanDetail(const std::string& alarmId) const
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        ConstPlanMapIterator it = m_planMap.begin();
	    for( ; it != m_planMap.end(); ++it )
	    {
		    if (alarmId.compare (it->second.m_detail.assocAlarmId.in()) == 0)
            {
                break;
		    }
        }

        if ( it == m_planMap.end() )
        {
            std::ostringstream what;
            what << "No plan instance associated with alarm [" << alarmId.c_str() << "]";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, 0, 0) );
        }

        return it->second.m_detail; 
	}

	TA_Base_Core::ActivePlanDetails* ActivePlanBulletinBoardImpl::getActivePlanDetail( bool stateUpdate ) const
	{
		TA_Base_Core::ActivePlanDetails_var apd = new TA_Base_Core::ActivePlanDetails;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			if( 0 >= m_planMap.size() )
			{
				apd->length(0);
			}
			else
			{
				ConstPlanMapIterator it;
				apd->length(m_planMap.size());

				int i = 0;

				for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
				{
					apd[i] = it->second.m_detail;

                    if ( stateUpdate )
                    {
                        //The owner is replaced by sessionId only when this method is used for stateUpdate
                        apd[i].owner = it->second.m_session.c_str();
                    }

					++i;
				}
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Found details for %lu plans/instances.", apd->length() );

		// No PlanNotFoundException as it is valid for there to be no
		// plans running.
		return apd._retn();
	}

	int ActivePlanBulletinBoardImpl::getState( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			ConstPlanMapIterator it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
                TA_Base_Core::EActivePlanState state = it->second.m_detail.activePlanState;
				return( it->second.m_detail.activePlanState );
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getState() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}


	ACE_Future<int> ActivePlanBulletinBoardImpl::getFuture( const TA_Base_Core::ActivePlanId& plan ) const
	{
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		ConstPlanMapIterator it = m_planMap.find(plan);

		if ( m_planMap.end() != it )
		{
			return( it->second.m_result );
		}
		else
		{
			std::ostringstream what;
			what << "ActivePlanBulletinBoardImpl::getFuture() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
	}


	int ActivePlanBulletinBoardImpl::waitForOperatorResponse( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const
	{
		ACE_Future<int> future = getFuture( plan );

		int result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;
		int rval = UNDEFINED_WR;

		if( -1 == future.get( result, &tv ) )
		{
			if( ETIME != ACE_OS::last_error() )
			{
                std::stringstream what;
				what << "ActivePlanBulletinBoardImpl::waitForOperatorResponse() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " ACE_Future::get failed with errno: " << ACE_OS::last_error() << ".";

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}
			return( TIMEOUT_WR );
		}

		switch( result )
		{
			case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
			{
				rval = SUCCESS_WR;
			}
			break;

			default:
			{
				rval = FAILURE_WR;

			}
			break;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: wait for completion %s.", plan.plan, plan.instance, ((SUCCESS_WR==rval)?"successful":"failed") );

		return( rval );
	}

	int ActivePlanBulletinBoardImpl::waitForCompletion( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const
	{
		ACE_Future<int> future = getFuture( plan );

		int result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;
		int rval = UNDEFINED_WR;

		if( -1 == future.get( result, &tv ) )
		{
			if( ETIME != ACE_OS::last_error() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::waitForCompletion() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " ACE_Future::get failed with errno: " << ACE_OS::last_error()
					 << ".";

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}

			return( TIMEOUT_WR );
		}

		switch( result )
		{
			case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
			{
				rval = SUCCESS_WR;
			}
			break;

			default:
			{
				rval = FAILURE_WR;
			}
			break;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: wait for completion %s.", plan.plan, plan.instance, ((SUCCESS_WR==rval)?"successful":"failed") );

		return( rval );
	}


	int ActivePlanBulletinBoardImpl::waitForTermination( unsigned long plan, ACE_Time_Value& tv ) const
	{
		int rval = UNDEFINED_WR;

        // Would have preferred to make a list of all the ACE_Future objects
        // but they are not stl::list friendly.

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            ConstPlanMapIterator itLower;
            ConstPlanMapIterator itUpper;
			getInstances( plan, itLower, itUpper );
			ConstPlanMapIterator it = itLower;

            int result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;

			while(( rval != TIMEOUT_WR ) && ( it != itUpper ))
			{
				if( -1 == it->second.m_result.get( result, &tv ))
				{
                    if( ETIME != ACE_OS::last_error() )
					{
						std::ostringstream what;
						what << "ActivePlanBulletinBoardImpl::waitForTermination() : ACE_Future::get failed with errno: " << ACE_OS::last_error() << ".";

						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
						TA_THROW( ThirdPartyLibException( what.str() ) );
					}
					else
					{
						rval = TIMEOUT_WR;
					}
				}
				else
				{
					rval = SUCCESS_WR;
				}

				++it;
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: wait for termination %s.", plan, ((SUCCESS_WR==rval)?"successful":"timed out") );
		return rval;
	}

	void ActivePlanBulletinBoardImpl::getInstances( const unsigned long plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const
	{
		TA_Base_Core::ActivePlanId idLower;
		TA_Base_Core::ActivePlanId idUpper;

		idLower.plan = plan;
		idUpper.plan = plan;
		idLower.instance = 1;
		idUpper.instance = std::numeric_limits<CORBA::ULong>::max();

		// get the bounds for all instances of the given plan
		itLower = m_planMap.lower_bound( idLower );
		itUpper = m_planMap.upper_bound( idUpper );
	}

    unsigned long ActivePlanBulletinBoardImpl::getFirstAvailableInstanceNumber(const unsigned long plan)
    {
        // TD10997: There can only be so many plan instances executing at any given time.
        //          Do not allow the specified plan to be instantiated if the active plan
        //          limit has been reached. 
        //
        //    Note: We should be able to remove this restriction once we are able to properly
        //          publish and control instances that are queued and waiting to start 
        //          execution (see defect comments in TD).

        unsigned long activeInstanceCount = 0u;

        for (ConstPlanMapIterator it(m_planMap.begin()); it != m_planMap.end(); it++)
        {
            if (it->second.isActive())
            {
                activeInstanceCount++;
            }
        }

        if (activeInstanceCount >= PlanAgentUser::getInstance()->getActivePlanThreadPoolSize())
        {
            std::ostringstream what;
			what << "The number of currently active instances has reached the servicable limit of "
                 << PlanAgentUser::getInstance()->getActivePlanThreadPoolSize() << ". No new instances can be created.";

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.str().c_str());
            TA_THROW( TA_Base_Core::PlanAgentException( what.str(), TA_Base_Core::PAE_NO_RESOURCE) );
        }

        // Find the first free instance number for the given plan. Start with 1 and work upwards.
        TA_Base_Core::ActivePlanId activePlanId;
        activePlanId.plan = plan;
        activePlanId.instance = 1u;
        bool bFound = false;

        while (activePlanId.instance < std::numeric_limits<CORBA::ULong>::max())
        {
            if (m_planMap.find(activePlanId) == m_planMap.end())
            {
                bFound = true;
                break;
            }

            activePlanId.instance++;
        }

        if ( !bFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No space left to store plan information");
            TA_THROW( TA_Base_Core::PlanAgentException( "No space left to store plan information", TA_Base_Core::PAE_NO_RESOURCE) );
        }

        return activePlanId.instance;
    }

	//marvin++
	//TD13527
    bool ActivePlanBulletinBoardImpl::insertEntityPlanMapRestrictionIfNotExst( const TA_Base_Core::ActivePlanId& plan )
    {
        ACE_Write_Guard<ACE_RW_Mutex> entityPlanRestrictionMapWriteGuard(m_entityPlanRestrictionMultiMapGuard);
        EntityPlanRestrictionMultiMapIterator value = std::find(m_entityPlanRestrictionMultiMap.begin(), m_entityPlanRestrictionMultiMap.end(), EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));

        if (value != m_entityPlanRestrictionMultiMap.end())
        {
            return false;
        }

        m_entityPlanRestrictionMultiMap.insert(EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));
        return true;
    }
	//++marvin
	//TD13527

    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
        const int initialState, const ACE_Future<int>& planResult, bool manExec)
	{
		std::string owner = getOwner(session);
		std::string remark = (initialState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE? RemarkCustomizationInProgress : RemarkPlanStarted);

		//marvin++
		//TD13527
		if(plan.entitykey != -1)
		{
			//enforce one running plan per entity when entitykey is not empty
			if(!insertEntityPlanMapRestrictionIfNotExst(plan))
			{
				//throw plan agent exception
				std::ostringstream planException;
				planException << "Entity " << plan.entitykey << " is restricted to running only a single instance of a plan. " << "Currently running plan id " << plan.plan;

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, planException.str().c_str());
				TA_THROW( TA_Base_Core::PlanAgentException( planException.str(), TA_Base_Core::PAE_EXCEEDED_RUNNING_PLANS_FOR_ENTITY) );
			}
		}
		//marvin++
		//TD13527

        plan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);

        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        if (m_isControlMode)
        {
		    plan.instance = getFirstAvailableInstanceNumber(plan.plan);
        }

        //this is to be filled in latter with an appropriate value
        //if an alarm is raised by this plan
        std::string assocAlarmId = DEFAULT_ASSOC_ALARM_ID;//default value

		// Keep a copy of the instance in the detail, otherwise we would have to set it 
        // each time we retrieved it from the map.
        PlanDetail newDetail(session, pmId, userKey, getNextUpdateId(), owner, plan, initialState, remark, planResult, manExec, assocAlarmId);

        // Try adding the new plan instance to the plan map.
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(plan, newDetail) );

		if( !insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << plan.plan << ",I:" << plan.instance << "]: " << "STL Map insertion failed";
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: instance reserved.", plan.plan, plan.instance );

        return true;
	}


    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const int initialState, const ACE_Future<int>& planResult )
	{
		std::string owner = getOwner(session);
		std::ostringstream remark;
        remark << "Plan spawned by '" 
               << TA_Base_Bus::PlanDataReader::instance()->getNodePath(parentPlan.plan) 
               << "' instance " << parentPlan.instance;

        childPlan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);

        // TD10997: Find the first free instance for the child plan. This also ensures that 
        //          the maximum number of active instances for all plans does not get exceeded.
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        if (m_isControlMode)
        {
		    childPlan.instance = getFirstAvailableInstanceNumber(childPlan.plan);
        }

        //this is to be filled in latter with an appropriate value
        //if an alarm is raised by this plan
        std::string assocAlarmId = DEFAULT_ASSOC_ALARM_ID;//default value

		// Keep a copy of the instance in the detail, otherwise we would have to set it 
        // each time we retrieved it from the map.
        PlanDetail newDetail(session, pmId, userKey, getNextUpdateId(), owner, childPlan, initialState, remark.str(), planResult, false, assocAlarmId);//not sure about second last param

        // Try adding the new plan instance to the plan map.
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(childPlan, newDetail) );

		if( !insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << childPlan.plan << ",I:" << childPlan.instance << "]: " << "STL Map insertion failed";
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: instance reserved.", childPlan.plan, childPlan.instance );

        return true;
	}


    void ActivePlanBulletinBoardImpl::undoReserveInstance( TA_Base_Core::ActivePlanId plan )
    {
        PlanMapIterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

            if ( it != m_planMap.end() )
            {
                m_planMap.erase(it);
            }
		}
    }

    void ActivePlanBulletinBoardImpl::sendUpdateMessage(const TA_Base_Core::ActivePlanId& plan)
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

		PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
            sendUpdateMessage( it->second );
        }
    }


	void ActivePlanBulletinBoardImpl::releaseInstance( const TA_Base_Core::ActivePlanId& plan )
	{
		PlanMap::size_type numRemoved = 0;

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
			 numRemoved = m_planMap.erase( plan );
		}

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);

            for ( StepResponseStatusMapIt itLoop = m_mapStepReponseStatus.begin(); itLoop != m_mapStepReponseStatus.end(); )
            {
                if (( itLoop->first.getNodeId() == plan.plan ) && ( itLoop->first.getPlanInstance() == plan.instance ))
                {
                    m_mapStepReponseStatus.erase( itLoop++ );
                }
                else
                {
                    ++itLoop;
                }
            }
        }

		if( 1 != numRemoved )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::releaseInstance() : [P:" << plan.plan << ",I:" << plan.instance << "]: Not in Bulletin Board or not released";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: %d instance released.", plan.plan, plan.instance, numRemoved );
	}

    void ActivePlanBulletinBoardImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;

                for (unsigned long i=0; i < historyUpdate.history.details.length(); i++)
                {
                    const TA_Base_Core::StepCustomisableDetail& custDetail = historyUpdate.history.details[i];

                    CustHistoryMapIterator chIt = it->second.m_custHistoryMap.find(custDetail.position);

					if ( chIt != it->second.m_custHistoryMap.end() )
					{
                        chIt->second = custDetail;
					}
					else
					{
						it->second.m_custHistoryMap.insert(CustHistoryMap::value_type(custDetail.position, custDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.customUpdateId);

		if( !foundInstance )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Plan [%lu, %lu] not found when processCustomisationHistoryUpdate", historyUpdate.plan.plan, historyUpdate.plan.instance );
		}
    }

    void ActivePlanBulletinBoardImpl::processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate )
    {
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;

                for (unsigned long i=0; i < historyUpdate.history.details.length(); i++)
                {
                    const TA_Base_Core::ActiveStepDetail& stepDetail = historyUpdate.history.details[i];

                    ExeHistoryMapIterator ehIt = it->second.m_exeHistoryMap.find(stepDetail.position);

					if ( ehIt != it->second.m_exeHistoryMap.end() )
					{
                        ehIt->second = stepDetail;
					}
					else
					{
						it->second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(stepDetail.position, stepDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.activeUpdateId);

		if( !foundInstance )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Plan [%lu, %lu] not found when processExecutionHistoryUpdate", historyUpdate.plan.plan, historyUpdate.plan.instance );
		}
    }

	void ActivePlanBulletinBoardImpl::initStep( const TA_Base_Core::ActivePlanId& plan, 
        TA_Base_Core::StepNumber step, const std::string& stepName, const std::string& remark )
    {
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
            if(( step != it->second.m_detail.currentStepDetail.position ) ||
                ( 0 != stepName.compare( it->second.m_detail.currentStepDetail.name.in() )) ||
                ( 0 != remark.compare( it->second.m_detail.currentStepDetail.remark.in() )) ||
				( TA_Base_Core::LOADED_ACTIVE_STEP_STATE != it->second.m_detail.currentStepDetail.state ))
			{
                it->second.m_detail.currentStepDetail.position = step;
                it->second.m_detail.currentStepDetail.name = stepName.c_str();
                it->second.m_detail.currentStepDetail.remark = remark.c_str();
                it->second.m_detail.currentStepDetail.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
                it->second.m_detail.remark = remark.c_str();
                it->second.m_detail.activeUpdateId = getNextUpdateId();
                sendUpdateMessage( it->second.m_detail );

				ExeHistoryMapIterator ehIt = it->second.m_exeHistoryMap.find(step);

				if ( ehIt != it->second.m_exeHistoryMap.end() )
				{
					ehIt->second.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
				}
				else
				{
                    TA_Base_Core::ActiveStepDetail cpInsertStepDetail = it->second.m_detail.currentStepDetail;
					it->second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(step, cpInsertStepDetail));
				}
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: loaded.", plan.plan, plan.instance, step );
		}
        else
        {
            std::ostringstream what;
            what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
        }
	}

	//marvin++
	//TD13527
	void ActivePlanBulletinBoardImpl::deleteEntityPlanMapRestrictionIfApplicable(const TA_Base_Core::ActivePlanId& plan, const int state)
	{
		if ( state == TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::FAILED_ACTIVE_PLAN_STATE)
		{
			//clear map related to entity restrictions once plan has finished
			if( plan.entitykey != -1 )
			{
				ACE_Write_Guard<ACE_RW_Mutex> entityPlanRestrictionMapWriteGuard(m_entityPlanRestrictionMultiMapGuard);
				EntityPlanRestrictionMultiMapIterator value = std::find(m_entityPlanRestrictionMultiMap.begin(), m_entityPlanRestrictionMultiMap.end(), EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));

				if(value != m_entityPlanRestrictionMultiMap.end())
				{
					m_entityPlanRestrictionMultiMap.erase(value);
				}
				else
				{
					std::ostringstream planWarn;
					planWarn << "Entity and plan id pair not found on m_entityPlanRestrictionMultiMap deletion failed for entity " << plan.entitykey << " restricted to running a single instance of plan " << plan.plan;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, planWarn.str().c_str());
				}
			}
		}
	}
	//++marvin
	//TD13527

	void ActivePlanBulletinBoardImpl::updateStepState( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, const int state, const std::string& remark )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator it = m_planMap.find( plan );

        if( it  != m_planMap.end() )
        {
            // Update only if the state and remark changed
            if(( step == it->second.m_detail.currentStepDetail.position ) && 
                (( state != it->second.m_detail.currentStepDetail.state ) || 
                ( 0 != remark.compare( it->second.m_detail.currentStepDetail.remark.in() ))))
            {
                it->second.m_detail.currentStepDetail.state = static_cast<TA_Base_Core::EActiveStepState>(state);
                it->second.m_detail.currentStepDetail.remark = remark.c_str();
                it->second.m_detail.remark = remark.c_str();

                if ( TA_Base_Core::PAUSED_ACTIVE_STEP_STATE == state )
                {
                    it->second.set( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE );
                }

                it->second.m_detail.activeUpdateId = getNextUpdateId();
                sendUpdateMessage( it->second.m_detail );

                ExeHistoryMapIterator ehIt = it->second.m_exeHistoryMap.find(step);

                if ( ehIt != it->second.m_exeHistoryMap.end() )
                {
                    // Update the state and remark only
                    ehIt->second.state = it->second.m_detail.currentStepDetail.state;
                    ehIt->second.remark = it->second.m_detail.currentStepDetail.remark;
                }
                else
                {
                    TA_Base_Core::ActiveStepDetail cpInsertStepDetail = it->second.m_detail.currentStepDetail;
                    it->second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(step, cpInsertStepDetail));
                }
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: state is %lu.", plan.plan, plan.instance, step, state );
        }
        else
        {
            std::ostringstream what;

            what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
        }
	}

	void ActivePlanBulletinBoardImpl::updatePlanState( const TA_Base_Core::ActivePlanId& plan, const int state, const std::string& remark )
	{
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false;

		//marvin++
		//TD13527
		deleteEntityPlanMapRestrictionIfApplicable(plan,state);
		//++marvin
		//TD13527

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			PlanMapIterator it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;
                TA_Base_Core::EActivePlanState oldState = it->second.m_detail.activePlanState;

				if( it->second.set( state ) )
				{
					it->second.m_detail.activeUpdateId = getNextUpdateId();
					it->second.m_detail.remark = remark.c_str();

                    if ( state == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE )
                    {
                        // Do not change step state here
//                         it->second.m_detail.currentStepDetail.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
                    }
                    else if ( (state == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE) &&
                              (oldState == TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE) )
                    {
                        it->second.m_detail.currentStepDetail.state = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;
                    }
					sendUpdateMessage( it->second.m_detail );

                    //Plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE
                    if ( it->second.isNonActive() )
                    {
                        ExeHistoryMapIterator itHisLoop = it->second.m_exeHistoryMap.begin();
                        while ( it->second.m_exeHistoryMap.end() != itHisLoop )
                        {
                            TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;

                            if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                                ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                            {
                                it->second.m_exeHistoryMap.erase( itHisLoop++ );
                            }
                            else
                            {
                                ++itHisLoop;
                            }
                        }
                    }
				}
			}
		}

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: state is %lu.", plan.plan, plan.instance, state );
		}
	}

    void ActivePlanBulletinBoardImpl::updateOwner( const TA_Base_Core::ActivePlanId& plan, const std::string& session, const std::string& pmId )
	{
		//used only for change of owner 

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
			PlanMapIterator it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;

				if( session != it->second.m_session )
				{
					TA_Base_Core::EActivePlanState currentState = it->second.m_detail.activePlanState;
                    it->second.m_detail.activeUpdateId = getNextUpdateId();
                    it->second.m_session = session;
                    it->second.m_pmId = pmId;
                    unsigned long userKey = 0xFFFFFFFF;
                    getUserKeyFromSession( session, userKey );
                    it->second.m_userKey = userKey;
                    std::string owner = getOwner(session);
                    it->second.m_detail.owner = owner.c_str();
                    std::string remark = owner;
                    remark += RemarkOperatorTookControl;
					it->second.m_detail.remark = remark.c_str();
					it->second.m_detail.activePlanState = TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE;

                    // Actually, the session id and plan manager id could not be sync to monitor agent by this update
					sendUpdateMessage( it->second.m_detail );

					it->second.m_detail.activePlanState = currentState;
				}				
			}
		}

		if( !foundInstance )
		{
			std::ostringstream what;
			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: SessionId is %s.", plan.plan, plan.instance, session.c_str() );
		}
	}

	void ActivePlanBulletinBoardImpl::updateCustom( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

		PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
			CustHistoryMapIterator chIt = it->second.m_custHistoryMap.find(detail.position);

			if ( chIt != it->second.m_custHistoryMap.end() )
			{
				chIt->second.position = detail.position;
				chIt->second.skip = detail.skip;
				chIt->second.delay = detail.delay;
			}
			else
			{
				it->second.m_custHistoryMap.insert(CustHistoryMap::value_type(detail.position, detail));
			}

			TA_Base_Core::PlanCustomisationUpdate_var updateData = new TA_Base_Core::PlanCustomisationUpdate();
			updateData->plan = plan;
            unsigned long updateId = getNextUpdateId();
			updateData->customUpdateId = updateId;
			updateData->detail = detail;

			sendCustomUpdateMessage( updateData );

            //TODO: Send StateUpdate for change in m_updateId
            sendStateUpdateMessage( updateId );
		}
	}

    void ActivePlanBulletinBoardImpl::cullStoppedPlan(TA_Base_Core::ActivePlanId plan)
    {
        ACE_Guard<ACE_Mutex> guard(m_cullLock);
	    updatePlanState( plan, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
	    releaseInstance( plan );
    }

    void ActivePlanBulletinBoardImpl::cullAllPlans()
    {
		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.
        ACE_Guard<ACE_Mutex> guard(m_cullLock);
		std::list<TA_Base_Core::ActivePlanId> plansToCull;

		// make sure the read lock goes out of scope before
		// we try to take the write lock when erasing the old
		// plans
		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			for( PlanMapIterator it = m_planMap.begin(); it != m_planMap.end(); ++it )
			{
			    plansToCull.push_back( it->second.m_detail.plan );
			}
		}

		if( plansToCull.size() > 0 )
		{
			std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

			for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
			{
				updatePlanState( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
				releaseInstance( *cullIt );
			}
		}
    }

	void ActivePlanBulletinBoardImpl::cullStoppedPlans()
	{
        if ( !m_isControlMode )
        {
            return;
        }

		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.

        ACE_Guard<ACE_Mutex> guard(m_cullLock);
		std::list<TA_Base_Core::ActivePlanId> plansToCull;

		// make sure the read lock goes out of scope before
		// we try to take the write lock when erasing the old
		// plans
		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			for( PlanMapIterator it = m_planMap.begin(); it != m_planMap.end(); ++it )
			{
				if( it->second.isNonActive() )
				{
					time_t diff = ACE_OS::time() - it->second.m_lastModified;

                    if( diff > PlanAgentUser::getInstance()->getActivePlanExpiryTime() )
					{
						plansToCull.push_back( it->second.m_detail.plan );
					}
				}
			}
		}

		if( plansToCull.size() > 0 )
		{
			std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

			for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
            {
                updatePlanState( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
				releaseInstance( *cullIt );
			}
		}
	}

	void ActivePlanBulletinBoardImpl::sendUpdateMessage( const PlanDetail& detail )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "**PlanDetail UpdateId=%u.", detail.m_detail.activeUpdateId);

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();
        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

		CORBA::Any commsData;
		commsData <<= detail.m_detail;
		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, commsData, &filterData );

		//Send StateUpdate to the monitor PlanAgent
        try
        {
            sendStateUpdateMessage(detail.m_detail);
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: StateUpdate message with ActivePlanDetail could not be sent.");
        }
	}

	void ActivePlanBulletinBoardImpl::sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;

        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

		CORBA::Any commsData;
		commsData <<= configData;

		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::PlanConfigUpdate,
										 commsData,
                                         &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);


		CORBA::Any commsData;
		commsData <<= updateData;

		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::PlanCustomisationUpdate,
										 commsData,
                                         &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData )
	{
		if ( !m_isControlMode || ( NULL == m_stateSender ))
        {
			return;
        }

        stateUpdateData.pasUpdateId = getNextPasUpdateId();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: PAS Update message sent ( update id: %u ), ( type: %u )", stateUpdateData.pasUpdateId, stateUpdateData.update._d() );

		CORBA::Any stateData;
		stateData <<= stateUpdateData;

		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
                                                PlanAgentUser::getInstance()->getAgentEntityName(),
												stateData,
												NULL
											 );
    }

    // Pack all states update message to only one message to avoid message lost when Monitor Agent startup
    void ActivePlanBulletinBoardImpl::sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq )
    {
        if ( !m_isControlMode || ( NULL == m_stateSender ))
        {
            return;
        }

        for ( unsigned int nSeqLoop = 0; nSeqLoop < refStateUpdateDataSeq.length(); ++nSeqLoop )
        {
            refStateUpdateDataSeq[nSeqLoop].pasUpdateId = getNextPasUpdateId();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "-Msg-: Generate update id for all update message ( update id: %u ), ( type: %u )", 
                refStateUpdateDataSeq[nSeqLoop].pasUpdateId, refStateUpdateDataSeq[nSeqLoop].update._d() );
        }

        CORBA::Any stateData;
        stateData <<= refStateUpdateDataSeq;

        m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
                                                PlanAgentUser::getInstance()->getAgentEntityName(),
                                                stateData,
                                                NULL );
    }

    void ActivePlanBulletinBoardImpl::sendOperatorPausedStateUpdate( const TA_Base_Core::ActivePlanId& plan )
    {
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.operatorPausedPlanDetail( getActivePlanDetail( plan ));
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate sent for PausedForOperator state");
    }

	void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( const TA_Base_Core::ActivePlanDetail& planDetail )
	{
		if ( !m_isControlMode )
        {
			return;
        }

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.planDetail(planDetail);
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate sent for activePlanDetail: Plan[%u:%u]", planDetail.plan.plan, planDetail.plan.instance);
	}

    void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( const unsigned long updateId )
    {
		if ( !m_isControlMode )
        {
			return;
        }

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.activeUpdateId(updateId);
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate sent for activeUpdateId.");
    }

	void ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent(void)
	{
        //Send update for registered PlanManagers
        std::vector<TA_Base_Core::PASUpdateData*> vecPasUpdateDatas;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
			std::string strSessionID = "";
			std::string strPmID = "";
            for ( PMOBJREFMapIterator it = m_pmObjRef.begin(); it != m_pmObjRef.end(); it++)
            {
				if (getSessionIDAndPmID(it->first, strSessionID, strPmID))
				{
					//Send State update to the PlanAgent in monitor mode
					TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData();
                    vecPasUpdateDatas.push_back( pasUpdateData );

					pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
					TA_Base_Core::RegisterPlanManagerUpdate_var registerPMUpdate = new TA_Base_Core::RegisterPlanManagerUpdate;
					registerPMUpdate->session = strSessionID.c_str();
					registerPMUpdate->pmId = strPmID.c_str();					
					registerPMUpdate->planManager = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate(it->second);
					pasUpdateData->update.planManager(registerPMUpdate);

//                     sendStateUpdateMessage(pasUpdateData);
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Composite session string [%s]", it->first.c_str());
				}
			}
		}

        // Plan Step completion status update
        TA_Base_Core::ActiveStepCompletionStatusSeq_var varStepStatus = getStepCompletionStatusSeq();
        TA_Base_Core::PASUpdateData* pasStepStatusUpdateData = new TA_Base_Core::PASUpdateData();
        vecPasUpdateDatas.push_back( pasStepStatusUpdateData );
        pasStepStatusUpdateData->update.stepCompletionStatusSeq( varStepStatus );

        //ActivePlanDetail update
        TA_Base_Core::ActivePlanDetails_var pAPD = getActivePlanDetail(true);

        int nPlans = pAPD->length();

        for (int p=0; p<nPlans; p++)
        {
            TA_Base_Core::ActivePlanDetail& refPlanDetail = pAPD[p];
            TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasUpdateData );

            TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
            psu->session = refPlanDetail.owner;
			psu->pmId =  getPlanManagerId(refPlanDetail.plan).c_str();
            psu->plan = refPlanDetail.plan;
            psu->manuExec = refPlanDetail.manuallyExecuted;
            psu->remark = refPlanDetail.remark;

            if ( refPlanDetail.activePlanState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE )
            {
                pasUpdateData->update.customisePlanUpdate(psu);
            }
            else
            {
                pasUpdateData->update.runPlanUpdate(psu);
            }

//             sendStateUpdateMessage(pasUpdateData);
        }

        for (int i=0; i<nPlans; i++)
        {
            TA_Base_Core::ActivePlanDetail& refPlanDetail = pAPD[i];
		    TA_Base_Core::PASUpdateData* pasDetailUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasDetailUpdateData );

            pasDetailUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ActivePlanDetail_var apd = new TA_Base_Core::ActivePlanDetail();

            apd->plan = refPlanDetail.plan;
            apd->path = refPlanDetail.path;
            apd->activePlanState = refPlanDetail.activePlanState;
            apd->currentStepDetail = refPlanDetail.currentStepDetail;
            apd->currentStepDetail.name = refPlanDetail.currentStepDetail.name;
            apd->activeUpdateId = refPlanDetail.activeUpdateId;
            apd->remark = refPlanDetail.remark;
            apd->owner = refPlanDetail.owner;
            apd->lastStateTransitionTime = refPlanDetail.lastStateTransitionTime;
            apd->instantiationTime = refPlanDetail.instantiationTime;
            apd->manuallyExecuted = refPlanDetail.manuallyExecuted;
            apd->timer = refPlanDetail.timer;

//ActivePlanUpdate
            pasDetailUpdateData->update.planDetail(apd);
//             sendStateUpdateMessage(pasDetailUpdateData);

//Customization History Update
		    TA_Base_Core::PASUpdateData* pasCustomiseUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasCustomiseUpdateData );

            pasCustomiseUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::CustomisationHistory_var customisationHistory = getPlanCustomisationHistory(apd->plan, false);

            if ( customisationHistory->details.length() != 0 )
            {
                TA_Base_Core::CustomisationHistoryUpdate_var chu = new TA_Base_Core::CustomisationHistoryUpdate();
                chu->plan = apd->plan;
                chu->history = customisationHistory;
                pasCustomiseUpdateData->update.customisationHistory(chu);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
            }
            else
            {
                pasCustomiseUpdateData->update.activeUpdateId(customisationHistory->customUpdateId);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
           }

//Execution History Update
		    TA_Base_Core::PASUpdateData* pasHistoryUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasHistoryUpdateData );

            pasHistoryUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ExecutionHistory_var executionHistory = getPlanExecutionHistory(apd->plan, false);

            if ( executionHistory->details.length() != 0 )
            {
                TA_Base_Core::ExecutionHistoryUpdate_var ehu = new TA_Base_Core::ExecutionHistoryUpdate();
                ehu->plan = apd->plan;
                ehu->history = executionHistory;
                pasHistoryUpdateData->update.executionHistory(ehu);
//                 sendStateUpdateMessage(pasHistoryUpdateData);
           }
           else
           {
                pasHistoryUpdateData->update.activeUpdateId(executionHistory->activeUpdateId);
//                 sendStateUpdateMessage(pasHistoryUpdateData);
           }

//Active State update
           if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE != apd->activePlanState )
           {
               TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData();
               vecPasUpdateDatas.push_back( pasUpdateData );

               pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
               TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
               psu->session = pAPD[i].owner;
               psu->pmId = getPlanManagerId(pAPD[i].plan).c_str();
               psu->plan = apd->plan;
               psu->remark = pAPD[i].remark;
               psu->manuExec = pAPD[i].manuallyExecuted;
               if ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.stopPlan(psu);
               }
               else if ( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.pausePlan(psu);
               }
               else if ( TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.operatorPausedPlanDetail(apd);
               }
               else
               {
                    vecPasUpdateDatas.pop_back();
                    delete pasUpdateData;
                    pasUpdateData = NULL;
               }
           }
        }

        // Send all state update message anyway because Monitor Agent need to confirm state update request is received by Control Agent
        TA_Base_Core::PASUpdateDataSeq seqPasUpdateData;
        seqPasUpdateData.length( vecPasUpdateDatas.size() );
        for ( unsigned int nSeqLoop = 0; nSeqLoop < seqPasUpdateData.length(); ++nSeqLoop )
        {
            if ( NULL != vecPasUpdateDatas[nSeqLoop] )
            {
                seqPasUpdateData[nSeqLoop] = *(vecPasUpdateDatas[nSeqLoop]);
                delete vecPasUpdateDatas[nSeqLoop];
                vecPasUpdateDatas[nSeqLoop] = NULL;
            }
        }

        vecPasUpdateDatas.clear();
        sendAllStatesUpdateMessage(seqPasUpdateData);
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent");
	}


	void ActivePlanBulletinBoardImpl::sendStateUpdateRequestToControlAgent( void )
    {
        if ( m_isControlMode || ( NULL == m_stateSender ))
        {
            return;
        }

		TA_Base_Core::PASUpdateRequest_var pasRequest = new TA_Base_Core::PASUpdateRequest();
        pasRequest->pasRequestId = ACE_OS::time();
		pasRequest->entityName = PlanAgentUser::getInstance()->getAgentEntityName().c_str();

		CORBA::Any requestData;
		requestData <<= pasRequest;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Sent state update request to Control Agent");
		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
												PlanAgentUser::getInstance()->getAgentEntityName(),
												requestData,
												NULL
											 );
	}

	unsigned long ActivePlanBulletinBoardImpl::getNextUpdateId(void)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);

	    if ( m_updateId >= TA_Base_Core::MAX_UPDATE_ID )
        {
            m_updateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : (TA_Base_Core::MIN_UPDATE_ID - 1u);
        }

	    return ++m_updateId;
	}

	void ActivePlanBulletinBoardImpl::setUpdateId(unsigned long updateId)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);

        if (( m_updateId != TA_Base_Core::MAX_UPDATE_ID ) && ( updateId <= m_updateId ))
        {
            return;
        }

	    m_updateId = updateId;
	}

    unsigned long ActivePlanBulletinBoardImpl::getNextPasUpdateId(void)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);

	    if ( m_pasUpdateId >= TA_Base_Core::MAX_UPDATE_ID  )
        {
			m_pasUpdateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );
        }

        //Note: m_pasUpdateId MUST be incremented and then sent and NOT otherway(i.e. send and increment).
        //      This way the after update is received by the Monitoring PlanAgent, the state of the m_pasUpdateId
        //      is the same as that in the Controlling PlanAgent.

	    return ++m_pasUpdateId; //Not m_pasUpdateId++
    }

    void ActivePlanBulletinBoardImpl::setPasUpdateId(unsigned long updateId)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);

        if (( m_pasUpdateId != TA_Base_Core::MAX_UPDATE_ID ) && ( updateId < m_pasUpdateId ))
        {
            return;
        }

        m_pasUpdateId = updateId;
    }

	bool ActivePlanBulletinBoardImpl::getSessionIDAndPmID(const std::string& strCompositePmId, std::string& strSessionID, std::string& strPmID)
	{
		FUNCTION_ENTRY("getSessionIDAndPmID");
		bool bSuccess = false;
		
		if(strCompositePmId.size() >= 2 * INDICATION_SESSION_AND_PMID)
		{
			strSessionID = strCompositePmId.substr(0, INDICATION_SESSION_AND_PMID);
			strPmID = strCompositePmId.substr(INDICATION_SESSION_AND_PMID, INDICATION_SESSION_AND_PMID);
			bSuccess = true;
		}

		FUNCTION_EXIT;
		return bSuccess;
	}

    void ActivePlanBulletinBoardImpl::clearAllPlanStates()
    {
        {
		    ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            m_planMap.clear();
        }

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);
            m_mapStepReponseStatus.clear();
        }
    }
    
    void ActivePlanBulletinBoardImpl::updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refNewDetail )
    {
		bool foundInstance = false;
        {
            // Life cycle start for plan map lock
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            PlanMapIterator itDst = m_planMap.find( refNewDetail.plan );
            if( itDst  != m_planMap.end() )
            {
                foundInstance = true;

                if (( itDst->second.isNonActive() ) && ( itDst->second.m_detail.activePlanState != refNewDetail.activePlanState ))
                {
                    // If old plan state is non active, the plan state should not be updated to other state but could update the step state.
                    return;
                }

                // if current step detail is changed, update the execution history
                std::string strNameCmp = itDst->second.m_detail.currentStepDetail.name.in();
                std::string strRemarkCmp = itDst->second.m_detail.currentStepDetail.remark.in();
                if (( itDst->second.m_detail.currentStepDetail.position != refNewDetail.currentStepDetail.position ) ||
                    ( itDst->second.m_detail.currentStepDetail.state != refNewDetail.currentStepDetail.state ) || 
                    ( 0 != strRemarkCmp.compare( refNewDetail.currentStepDetail.remark.in() )) || 
                    ( 0 != strNameCmp.compare( refNewDetail.currentStepDetail.name.in() )))
                {
                    itDst->second.m_detail.currentStepDetail = refNewDetail.currentStepDetail;

                    if ( TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE != itDst->second.m_detail.currentStepDetail.state )
                    {
                        ExeHistoryMapIterator itHisItem = itDst->second.m_exeHistoryMap.find( refNewDetail.currentStepDetail.position );

                        if ( itDst->second.m_exeHistoryMap.end() != itHisItem )
                        {
                            itHisItem->second = refNewDetail.currentStepDetail;
                        }
                        else
                        {
                            TA_Base_Core::ActiveStepDetail newDetail = refNewDetail.currentStepDetail;
                            itDst->second.m_exeHistoryMap.insert( ExeHistoryMap::value_type( refNewDetail.currentStepDetail.position, newDetail ));
                        }
                    }
                }

                if ( itDst->second.m_detail.activePlanState != refNewDetail.activePlanState )
                {
                    deleteEntityPlanMapRestrictionIfApplicable( refNewDetail.plan, refNewDetail.activePlanState );

                    if ( TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE != refNewDetail.activePlanState )
                    {
                        itDst->second.set( refNewDetail.activePlanState );
                    }

                    // If plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE in execution history
                    if ( itDst->second.isNonActive() )
                    {
                        ExeHistoryMapIterator itHisLoop = itDst->second.m_exeHistoryMap.begin();
                        while ( itDst->second.m_exeHistoryMap.end() != itHisLoop )
                        {
                            TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;
                            
                            if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                                ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                            {
                                itDst->second.m_exeHistoryMap.erase( itHisLoop++ );
                            }
                            else
                            {
                                ++itHisLoop;
                            }
                        }
                    }
                }

                // Update active plan detail
                itDst->second.m_detail.manuallyExecuted = refNewDetail.manuallyExecuted;
                itDst->second.m_detail.timer = refNewDetail.timer;
                itDst->second.m_detail.remark = refNewDetail.remark.in();
                itDst->second.m_detail.activeUpdateId = getNextUpdateId();
            }
            // Life cycle end for plan map lock
        }
        
        if( !foundInstance )
        {
            std::ostringstream what;
            
            what << "ActivePlanBulletinBoardImpl::updateActivePlanDetail() : [P:" << refNewDetail.plan.plan << ",I:" << refNewDetail.plan.instance << "]:"
                << " Not found in bulletin board.";
            
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, refNewDetail.plan.plan, refNewDetail.plan.instance) );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: state is %lu.", refNewDetail.plan.plan, refNewDetail.plan.instance, refNewDetail.currentStepDetail.position, refNewDetail.activePlanState );
		}
    }

    void ActivePlanBulletinBoardImpl::updateActivePlanTimer( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulTimer )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator itDst = m_planMap.find( plan );
        if( itDst  != m_planMap.end() )
        {
            itDst->second.m_detail.timer = ulTimer;
        }
    }

    void ActivePlanBulletinBoardImpl::setStepCompletionStatus( const TA_Base_Bus::SortableActiveStepId& step, const int status )
    {
        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);
            StepResponseStatusMapIt itFound = m_mapStepReponseStatus.find( step );
            if ( m_mapStepReponseStatus.end() != itFound )
            {
                if ( itFound->second == status )
                {
                    return;
                }
                else
                {
                    itFound->second = status;
                }
            }
            else
            {
                m_mapStepReponseStatus.insert( StepResponseStatusMap::value_type( step, status ));
            }
        }

        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::ActiveStepCompletionStatusSeq_var varStatus = getStepCompletionStatusSeq();
        pasUpdateData->update.stepCompletionStatusSeq( varStatus );
        sendStateUpdateMessage( pasUpdateData );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "StateUpdate sent for active step completion status [%d:%d:%d:%d]", 
            step.getNodeId(), step.getPlanInstance(), step.getStepNumber(), status);
    }

    void ActivePlanBulletinBoardImpl::removeStepCompletionStatus( const TA_Base_Bus::SortableActiveStepId& step )
    {
        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);
            StepResponseStatusMapIt itFound = m_mapStepReponseStatus.find( step );
            if ( m_mapStepReponseStatus.end() != itFound )
            {
                m_mapStepReponseStatus.erase( itFound );
            }
        }

        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::ActiveStepCompletionStatusSeq_var varStatus = getStepCompletionStatusSeq();
        pasUpdateData->update.stepCompletionStatusSeq( varStatus );
        sendStateUpdateMessage( pasUpdateData );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "StateUpdate sent for active step completion status removal [%d:%d:%d]", 
            step.getNodeId(), step.getPlanInstance(), step.getStepNumber());
    }

    int ActivePlanBulletinBoardImpl::getStepCompletionStatus( const TA_Base_Bus::SortableActiveStepId& step )
    {
        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);

            StepResponseStatusMapIt itFound = m_mapStepReponseStatus.find(step);
            if ( m_mapStepReponseStatus.end() != itFound )
            {
                return itFound->second;
            }
        }

        return TA_Base_Core::PSCS_UNDEFINED;
    }

    TA_Base_Core::ActiveStepCompletionStatusSeq* ActivePlanBulletinBoardImpl::getStepCompletionStatusSeq()
    {
        TA_Base_Core::ActiveStepCompletionStatusSeq_var ascss = new TA_Base_Core::ActiveStepCompletionStatusSeq();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);

            ascss->length( m_mapStepReponseStatus.size() );
            int nIndex = 0;

            for ( StepResponseStatusMapIt itLoop = m_mapStepReponseStatus.begin(); 
                itLoop != m_mapStepReponseStatus.end(); ++itLoop )
            {
                ascss[nIndex].plan = itLoop->first.getNodeId();
                ascss[nIndex].instance = itLoop->first.getPlanInstance();
                ascss[nIndex].stepPosition = itLoop->first.getStepNumber();
                ascss[nIndex].completionStatus = static_cast<TA_Base_Core::EPlanStepCompletionStatus>(itLoop->second);
                ++nIndex;
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found %d details for active step completion status.", ascss->length() );
        return ascss._retn();
    }

    void ActivePlanBulletinBoardImpl::updateStepResponseStatusMap( const TA_Base_Core::ActiveStepCompletionStatusSeq& statusSeq )
    {
        if ( m_isControlMode )
        {
            return;
        }

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxStepResponseStatusLock);

            m_mapStepReponseStatus.clear();

            for ( unsigned int nLoop = 0u; nLoop < statusSeq.length(); ++nLoop )
            {
                const TA_Base_Core::ActiveStepCompletionStatus& refStatus = statusSeq[nLoop];
                m_mapStepReponseStatus[refStatus] = refStatus.completionStatus;
            }
        }
    }
} // TA_Base_App

