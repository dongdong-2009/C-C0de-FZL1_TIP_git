/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SchedulingAgentEntityData.cpp $
  * @author Dirk McCormick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ISchedulingAgentEntityData provides read-only access to the
  * Scheduling Agent's entity data.
  */


#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include <string>
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PlanAgentData.h"
#include "core/data_access_interface/entity_access/src/SchedulingAgentEntityData.h"


namespace TA_Base_Core
{
    const std::string SchedulingAgentEntityData::SCHEDULING_AGENT_ENTITY_TYPE = "SchedulingAgent";

    const std::string SchedulingAgentEntityData::OPERATOR_NAME_PARAM = "OperatorName";
    const std::string SchedulingAgentEntityData::PLAN_AGENT_LOCATION_NAME_PARAM = "PlanAgentLocation";
	const std::string SchedulingAgentEntityData::ASSET_NAME = "AssetName";


	SchedulingAgentEntityData::SchedulingAgentEntityData()
	    :
        m_operatorID(0),
        m_password(""),
        m_planAgentLocationKey(0),
        m_planAgentEntityKey(0),
        m_planAgentSubsystemKey(0)
    {
    }

	
    SchedulingAgentEntityData::SchedulingAgentEntityData(unsigned long key)
	    :
        EntityData( key, getType() ),
        m_operatorID(0),
        m_password(""),
        m_planAgentLocationKey(0),
        m_planAgentEntityKey(0),
        m_planAgentSubsystemKey(0)
    {
    }

    SchedulingAgentEntityData::~SchedulingAgentEntityData()
    {
    }


    unsigned long SchedulingAgentEntityData::getOperatorID()
    {
        FUNCTION_ENTRY("getOperatorID");

		// parameter -- OPERATOR_NAME_PARAM
		std::string value = getHelper()->getParameter(OPERATOR_NAME_PARAM);
		if (!value.empty())
		{
			IOperator* schedulingOperator = OperatorAccessFactory::getInstance().getOperator(value, false);
			
			try
			{
				// Store the operatorID temporarily in case the
				// getPassword call throws an exception.
				m_operatorID = schedulingOperator->getKey();
			}
			catch(...)
			{
				delete schedulingOperator;
				schedulingOperator = NULL;
				throw;
			}
			delete schedulingOperator;
			schedulingOperator = NULL;
		}
		else
		{
			std::string message = OPERATOR_NAME_PARAM + " is blank";
			
			TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, OPERATOR_NAME_PARAM));
		}

        FUNCTION_EXIT;
        return m_operatorID;
	}


    unsigned long SchedulingAgentEntityData::getPlanAgentLocationKey()
    {
        FUNCTION_ENTRY("getPlanAgentLocationKey");

		// parameter -- PLAN_AGENT_LOCATION_NAME_PARAM
		std::string value = getHelper()->getParameter(PLAN_AGENT_LOCATION_NAME_PARAM);
		
		// Get the entity object from the location and type
		if (!value.empty())
		{
			std::vector<IEntityData*> entities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(PlanAgentData::getStaticType(), value);
			
			if(entities.size() != 1)
			{
				std::ostringstream message;
				message << "There is no Plan Agent at location '" << value << "'";
				
				const char* fld = "Plan Agent";
				const char* msg = message.str().c_str();
				
				TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
			}
			
			IEntityData* planAgentEntity = entities[0];
			
			// Get the location key the Plan Agent at that location.
			m_planAgentLocationKey  = planAgentEntity->getLocation();
			
			delete planAgentEntity;
			planAgentEntity = NULL;
		}
		else
		{
			std::string message = PLAN_AGENT_LOCATION_NAME_PARAM + " is blank";				
			TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, PLAN_AGENT_LOCATION_NAME_PARAM));
		}

        FUNCTION_EXIT;
        return m_planAgentLocationKey;
	}


    unsigned long SchedulingAgentEntityData::getPlanAgentSubsystemKey()
    {
        FUNCTION_ENTRY("getPlanAgentSubsystemKey");

		// parameter -- PLAN_AGENT_LOCATION_NAME_PARAM
		std::string value = getHelper()->getParameter(PLAN_AGENT_LOCATION_NAME_PARAM);
		
		// Get the entity object from the location and type
		if (!value.empty())
		{
			std::vector<IEntityData*> entities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(PlanAgentData::getStaticType(), value);
			
			if(entities.size() != 1)
			{
				std::ostringstream message;
				message << "There is no Plan Agent at location '" << value << "'";
				
				const char* fld = "Plan Agent";
				const char* msg = message.str().c_str();
				
				TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
			}
			
			IEntityData* planAgentEntity = entities[0];
			
			// Get the  subsystem key for the Plan Agent at that location.
			m_planAgentSubsystemKey = planAgentEntity->getSubsystem();
			
			delete planAgentEntity;
			planAgentEntity = NULL;
		}
		else
		{
			std::string message = PLAN_AGENT_LOCATION_NAME_PARAM + " is blank";				
			TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, PLAN_AGENT_LOCATION_NAME_PARAM));
        }

        FUNCTION_EXIT;
        return m_planAgentSubsystemKey;
	}


    unsigned long SchedulingAgentEntityData::getPlanAgentEntityKey()
    {
        FUNCTION_ENTRY("getPlanAgentEntityKey");

		// parameter -- PLAN_AGENT_LOCATION_NAME_PARAM
		std::string value = getHelper()->getParameter(PLAN_AGENT_LOCATION_NAME_PARAM);
		
		// Get the entity object from the location and type
		if (!value.empty())
		{
			std::vector<IEntityData*> entities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(PlanAgentData::getStaticType(), value);
			
			if(entities.size() != 1)
			{
				std::ostringstream message;
				message << "There is no Plan Agent at location '" << value << "'";
				
				const char* fld = "Plan Agent";
				const char* msg = message.str().c_str();
				
				TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
			}
			
			IEntityData* planAgentEntity = entities[0];
			
			// Get the entity key for the Plan Agent at that location.
			m_planAgentEntityKey    = planAgentEntity->getKey();
			
			delete planAgentEntity;
			planAgentEntity = NULL;
		}
		else
		{
			std::string message = PLAN_AGENT_LOCATION_NAME_PARAM + " is blank";				
			TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, PLAN_AGENT_LOCATION_NAME_PARAM));
        }

        FUNCTION_EXIT;
        return m_planAgentEntityKey;
	}


    std::string SchedulingAgentEntityData::getPassword()
    {
        FUNCTION_ENTRY("getPassword");

		// parameter -- OPERATOR_NAME_PARAM
		std::string value = getHelper()->getParameter(OPERATOR_NAME_PARAM);
		if (!value.empty())
		{
			IOperator* schedulingOperator = OperatorAccessFactory::getInstance().getOperator(value, false);
			m_password = schedulingOperator->getPassword();

			delete schedulingOperator;
			schedulingOperator = NULL;
		}
		else
		{
			std::string message = OPERATOR_NAME_PARAM + " is blank";
			TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, OPERATOR_NAME_PARAM));
		}

        FUNCTION_EXIT;
        return m_password;
	}


    std::string SchedulingAgentEntityData::getAssetName()
    {
        FUNCTION_ENTRY("getAssetName");
 
		// parameter -- ASSET_NAME
		std::string value = getHelper()->getParameter(ASSET_NAME);
		if (!value.empty())
		{
			return value;
		}

		std::string message = ASSET_NAME + " is blank";
		TA_THROW(DataException(message.c_str(), DataException::MISSING_MANDATORY, ASSET_NAME));
	}


	std::string SchedulingAgentEntityData::getType()
	{
        return getStaticType();
	}


	std::string SchedulingAgentEntityData::getStaticType()
	{
        return SCHEDULING_AGENT_ENTITY_TYPE;
    }

          
    IEntityData* SchedulingAgentEntityData::clone(unsigned long key)
    {
        return new SchedulingAgentEntityData(key);
    }

    void SchedulingAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

