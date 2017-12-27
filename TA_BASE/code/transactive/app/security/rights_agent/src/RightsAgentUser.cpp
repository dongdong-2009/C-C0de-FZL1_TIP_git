/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/src/RightsAgentUser.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/07/03 17:57:59 $
 * Last modified by:  $Author: liwei.gong $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 */

#include "RightsAgent.h"

#if defined(_MSC_VER)
//#pragma warning(disable:4680)
#endif // defined(_MSC_VER)

using TA_Base_Bus::GenericAgent;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

unsigned long TA_Base_Bus::RightsAgentInfo::m_entity     = 0;
std::string            TA_Base_Bus::RightsAgentInfo::m_name       = "";
unsigned long TA_Base_Bus::RightsAgentInfo::m_entityType = 0;
unsigned long TA_Base_Bus::RightsAgentInfo::m_subsystem  = 0;
unsigned long TA_Base_Bus::RightsAgentInfo::m_location   = 0;
std::string TA_Base_Bus::RightsAgentInfo::m_assetName = "";

namespace TA_Base_App
{

    RightsAgentUser::RightsAgentUser(int argc, char* argv[]) : 
        m_genericAgent(NULL), 
		m_rightsAgentImpl(NULL)
    {
		FUNCTION_ENTRY("RightsAgentUser");

        m_genericAgent = new GenericAgent(argc,argv,this,NULL,AGENT_USER_TYPE_NORMAL);
		try
		{
			TA_Base_Core::RightsAgentEntityDataPtr entityData =
				boost::dynamic_pointer_cast<TA_Base_Core::RightsAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());
			
			TA_Base_Bus::RightsAgentInfo::m_entity     = entityData->getKey();
			TA_Base_Bus::RightsAgentInfo::m_name       = entityData->getName();
			TA_Base_Bus::RightsAgentInfo::m_entityType = entityData->getTypeKey();
			TA_Base_Bus::RightsAgentInfo::m_subsystem  = entityData->getSubsystem();
			TA_Base_Bus::RightsAgentInfo::m_location   = entityData->getLocation();
			TA_Base_Bus::RightsAgentInfo::m_assetName  = entityData->getAssetName();			
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

    RightsAgentUser::~RightsAgentUser()
    {
		FUNCTION_ENTRY("~RightsAgentUser");

        if (NULL != m_genericAgent)
        {
            m_genericAgent->deactivateAndDeleteServant();
        }

        if (NULL != m_rightsAgentImpl)
        {
            m_genericAgent->deactivateAndDeleteServant();
        }
		FUNCTION_EXIT;
    }

    void RightsAgentUser::agentTerminate()
    {
		FUNCTION_ENTRY("agentTerminate");

		if (NULL != m_rightsAgentImpl)
		{
			m_rightsAgentImpl->deactivateAndDeleteServant();
			m_rightsAgentImpl = NULL;
		}
		FUNCTION_EXIT;
    }

    TA_Base_Bus::IEntity* RightsAgentUser::createEntity( TA_Base_Core::IEntityDataPtr entityData )
    {
		FUNCTION_ENTRY("createEntity");

		FUNCTION_EXIT;
        return NULL;
    }

    void RightsAgentUser::agentSetMonitor()
    {
		FUNCTION_ENTRY("agentSetMonitor");
		FUNCTION_EXIT;
    }

    void RightsAgentUser::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");
		m_rightsAgentImpl->agentSetControl();
		FUNCTION_EXIT;
    }

    void RightsAgentUser::registerForStateUpdates()
    {
		FUNCTION_ENTRY("registerForStateUpdates");
		FUNCTION_EXIT;
    }

    void RightsAgentUser::receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		FUNCTION_EXIT;
    }

    void RightsAgentUser::processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
		FUNCTION_ENTRY("processOnlineUpdate");
		FUNCTION_EXIT;
    }

    void RightsAgentUser::run()
    {
		FUNCTION_ENTRY("run");

        m_rightsAgentImpl = new RightsAgentImpl(getAgentEntityName(), this);
        m_genericAgent->run();
		FUNCTION_EXIT;
    }

	void RightsAgentUser::checkOperationMode()
	{
		std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);			
		if( RPARAM_CONTROL != opMode )
		{
			throw( TA_Base_Core::OperationModeException(RPARAM_MONITOR) );
		}
	}

}
