/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityHelper.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * EntityHelper is an object that is held by all type-specific entity objects, as well as by
  * the ConfigEntity. It contains all data common across entity types, and manipulation
  * methods for the data. It helps avoid re-writing code for each entity type object.
  *
  * NOTE: The EntityHelper class works on the assumption that the key passed in the constructor
  * is correct.
  */

#ifdef _MSC_VER
  #pragma warning(disable:4786) // disable the "trucated to 255 character" warning
  #pragma warning(disable:4503)
#endif // _MSC_VER

#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SimpleDbData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/utilities/src/RunParams.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

#include <vector>
#include <ctime>

using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string EntityHelper::NO_PARENT = "<None>";
    const std::string EntityHelper::NO_AGENT = "<None>";
    
    const std::string EntityHelper::PARAMETERNAME("PARAMETERNAME");
    const std::string EntityHelper::PARAMETERTYPE("PARAMETERTYPE");
    const std::string EntityHelper::PARAMETERVALUE("VALUE");
    const std::string EntityHelper::PARAMETERDEFAULTVALUE("PARAMETERDEFAULTVALUE");
    const std::string EntityHelper::PARAMETERVISIBLE("PARAMETERVISIBLE");
    const std::string EntityHelper::PARAMETERMANDATORY("PARAMETERMANDATORY");
    const std::string EntityHelper::PARAMETERDATATYPE("PARAMETERDATATYPE");
    const std::string EntityHelper::PARAMETERVALIDATION("PARAMETERVALIDATION");
    const std::string EntityHelper::PARAMETERONLINEUPDATE("PARAMETERONLINEUPDATE");

    const std::string EntityHelper::DELETED_FLAG("#");
    const unsigned long EntityHelper::DEFAULT_NAME_MAX_LENGTH = 85;
    volatile unsigned long EntityHelper::m_nameMaxLength = 0;
    TA_Base_Core::NonReEntrantThreadLockable EntityHelper::m_nameMaxLengthLock;

    EntityHelper::EntityHelper(const unsigned long key, const std::string& typeName)
            : m_isValidData(false),
			m_isValidStatusData(false),
            m_key(key), 
            m_typeKey(0),
            m_name (""),
            m_type(typeName),
            m_address(""), 
            m_description(""), 
            m_subsystem(0), 
            m_fixedSubsystem(0),
            m_location(ULONG_MAX),
			m_region(ULONG_MAX),
            m_parent(ULONG_MAX),
            m_agent(0),
            m_subsystemName(""), 
            m_fixedSubsystemName(""),
            m_locationName(""),
			m_regionName(""),
            m_parentName(""),
            m_agentName(""),
            m_originalParent(ULONG_MAX),
            m_isManuallySetAgent(false),
			m_dateCreated(0),
            m_dateModified(0),
            m_configParameterData(NULL)
    {
    }

        
    EntityHelper::EntityHelper( const EntityHelper& theEntityHelperToCopy)
    {
        m_isValidData = false;
		m_isValidStatusData = false;
        m_key = 0;
        m_typeKey = theEntityHelperToCopy.m_typeKey;
        m_name = theEntityHelperToCopy.m_name;
        m_type = theEntityHelperToCopy.m_type;
        m_address = theEntityHelperToCopy.m_address;
        m_description = theEntityHelperToCopy.m_description;
        m_subsystem = theEntityHelperToCopy.m_subsystem;
        m_fixedSubsystem = theEntityHelperToCopy.m_fixedSubsystem;
        m_location = theEntityHelperToCopy.m_location;
		m_region = theEntityHelperToCopy.m_region;
        m_parent = theEntityHelperToCopy.m_parent;
        m_agent = theEntityHelperToCopy.m_agent;
        m_subsystemName = theEntityHelperToCopy.m_subsystemName;
        m_fixedSubsystemName = theEntityHelperToCopy.m_fixedSubsystemName;
        m_locationName = theEntityHelperToCopy.m_locationName;
		m_regionName = theEntityHelperToCopy.m_regionName;
        m_parentName = theEntityHelperToCopy.m_parentName;
        m_agentName = theEntityHelperToCopy.m_agentName;
        m_originalParent = m_parent;
        m_isManuallySetAgent = theEntityHelperToCopy.m_isManuallySetAgent;
        m_dateCreated = theEntityHelperToCopy.m_dateCreated;
        m_dateModified = theEntityHelperToCopy.m_dateModified;
        m_nameMaxLength = theEntityHelperToCopy.m_nameMaxLength;
		m_parameterData = theEntityHelperToCopy.m_parameterData;

        // Now we need to copy the entity parameters. We need to do this by creating a new IData object.
        m_configParameterData = NULL;        
    }


    EntityHelper::EntityHelper(const std::string& typeName)
      : m_isValidData(false),
	    m_isValidStatusData(false),
        m_key(0), 
        m_typeKey(ULONG_MAX),
        m_name (""), 
        m_type(typeName),
        m_address(""),  
        m_description(""),
        m_subsystem(0),
		m_fixedSubsystem(0),
        m_location(0),
		m_region(0),
        m_parent(ULONG_MAX),
        m_agent(0),
        m_subsystemName(""), 
        m_fixedSubsystemName(""),
        m_locationName(""),
		m_regionName(""),
        m_parentName(""),
        m_agentName(""),
        m_originalParent(ULONG_MAX),
        m_isManuallySetAgent(false),
		m_dateCreated(0),
        m_dateModified(0), 
        m_configParameterData(NULL)
    {
        try
        {
            getSubsystemForNewEntity();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Do not want this to escape out of the constructor. We will just set the subsystem to be 0");
            m_fixedSubsystem = 0;
        }
    };


    unsigned long EntityHelper::getKey()
    {
        // This method cannot be called until the key has been set
        //TA_ASSERT(0 != m_key,"The data must be written to the database before the key can be retrieved");
        
        return m_key;
    }

    
    std::string EntityHelper::getEntityType()
    {
        return m_type;
    }

    
    unsigned long EntityHelper::getTypeKey()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_typeKey;
    }


    std::string EntityHelper::getName()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {            
            reload();
        }
        return m_name;
    }


    
    void EntityHelper::setName(const std::string& name)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_name = name;
    }


    std::string EntityHelper::getDescription()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_description;
    }

    void EntityHelper::setDescription(const std::string& description)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_description = description;
    }


    std::string EntityHelper::getAddress()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_address;
    }

    
    void EntityHelper::setAddress(const std::string& address)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setAddress" );
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_address = address;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setAddress" );
    }


    unsigned long EntityHelper::getSubsystem(bool isFixed)
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }

        if (isFixed)
        {
            return m_fixedSubsystem;
        }
        else
        {
            return m_subsystem;
        }
    }


    std::string EntityHelper::getSubsystemName(bool isFixed)
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }

        if (isFixed)
        {
            return m_fixedSubsystemName;
        }
        else
        {
            return m_subsystemName;
        }
    }


    void EntityHelper::setSubsystem(const unsigned long subsystem)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setSubsystem" );
        if (!m_isValidData && (0 != m_key))
        {
            reload();
        }
        m_subsystem = subsystem;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setSubsystem" );
    }


    unsigned long EntityHelper::getLocation()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_location;
    }


	std::string EntityHelper::getLocationName()
	{	
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_locationName;
	}

    
    void EntityHelper::setLocation(const unsigned long location)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setLocation" );
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_location = location;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setLocation" );
    }


	unsigned long EntityHelper::getRegion()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_region;
    }


	std::string EntityHelper::getRegionName()
	{
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_regionName;
	}

    
    void EntityHelper::setRegion(const unsigned long region)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setRegion" );
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_region = region;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setRegion" );
    }


    unsigned long EntityHelper::getParent()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_parent;
    }


    std::string EntityHelper::getParentName()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }
        return m_parentName;
    }
    
    
    void EntityHelper::setParent(const unsigned long parent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setParent" );
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_parent = parent;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setParent" );
    }


    void EntityHelper::setAgent(const unsigned long agent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::setAgent" );
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_agent = agent;
        m_isManuallySetAgent = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::setAgent" );
    }

    
    std::string EntityHelper::getAgentName()
	{
        unsigned int key = getAgent();
        if (key == 0)
        {
            return NO_AGENT;
        }

        // if the agent key is unchanged, just return the current agent name
        if (m_agent == key)
        {
            return m_agentName;
        }

        IEntityData* agent = NULL;
        try
        {
		    // Get the entity object
		    agent = EntityAccessFactory::getInstance().getEntity( getAgent(), false );
		    std::string name( agent->getName() );

		    // Clean up
		    delete agent;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            delete agent;
            throw;
        }
	}
    
    
    unsigned long EntityHelper::getAgent()
    {
        // We don't want to call reload() if this is a new entity and it has not been written to the database
        if (!m_isValidData && 0 != m_key)  // This is an existing entity and the data needs to be loaded
        {
            reload();
        }

        if (!m_isManuallySetAgent && m_parent != m_originalParent)
        {
            // This means the user has not set the agentkey manually but they have changed the parent
            // at some point. The agentkey stored is not correct. So we will retrieve the correct one.
            // We must not store it because the apply method relies on the agent key being the old agent key
            return getNewAgentKey();
        }
        return m_agent;
    }


    time_t EntityHelper::getDateCreated()
    {
        TA_ASSERT(0 != m_key,"The data must be written to the database before the date created can be retrieved");
        // This method cannot be called until the date created has been set
        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t EntityHelper::getDateModified()
    {
        TA_ASSERT(0 != m_key,"The data must be written to the database before the date modified can be retrieved");
        // This method cannot be called until the date modified has been set
        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }


    void EntityHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::invalidate" );
        // This method cannot be called until the key has been set
        TA_ASSERT(0 != m_key,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;
        if (NULL != m_configParameterData) // the way to "invalidate" the parameter data is to null it
        {
            delete m_configParameterData; // need to delete the current data first though.
            m_configParameterData = NULL;
        }
		//TD15302
		m_isValidStatusData = false;
		m_entityStatusList.clear();
		m_parameterData.clear();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::invalidate" );
    }

    void EntityHelper::resetEntityHelper()
    {
        FUNCTION_ENTRY("resetEntityHelper");
        
        // Need to reset the member variables that "flag" this as already having been written to the database
        m_key = 0;
        m_isValidData = false;
		m_isValidStatusData = false; // TD15302
        m_parent = ULONG_MAX;
		m_parameterData.clear();

        FUNCTION_EXIT;
    }

	//zhou yuan++
	std::string EntityHelper::getParameter(const std::string& parameterName)
	{
		FUNCTION_ENTRY("EntityHelper::getParameter");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "load parameter %s", parameterName.c_str());

		if(m_parameterData.size() == 0)
		{
			// Need to retrieve the parameter data from the database
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_SELECT_20551, m_key, m_type);
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20551, m_key, m_type);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20551, m_key, m_type);
//			TA_ASSERT(strSql.size() > 0, " EntityHelper::getParameter(): Prepare SQL Statement error");
			
            // create the SQL string to retrieve the parameters from the database - the manner
            // in which the parameters will be retrieved depends on whether or not we have an 
            // entity key for this entity as yet.
            /*std::ostringstream sql;
			sql << "SELECT ep.name, nvl( ev.value, ep.ep_default ) as VALUE  FROM entityparameter ep, entityparametervalue ev WHERE ";
			sql << "ep.typekey = ( select pkey from entitytype where name = '";
			sql << m_type << "' ) and ep.pkey = ev.parameterkey(+) and ev.entitykey(+) = " << m_key;*/
			
            // Set up the columnNames vector to be passed to executeQuery()
            std::vector<std::string> columnNames;
			columnNames.push_back(PARAMETERNAME);
            columnNames.push_back(PARAMETERVALUE);
			
            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.                
            IData* data= databaseConnection->executeQuery( strSql, columnNames );
			int recordNum = data->getNumRows();
			for(int i = 0 ; i < recordNum ; ++i)
			{
				assignDefaultParameterValueData(data, i);
			}

			delete data;
		}

		std::string ret("");

		TA_Base_Core::ThreadGuard guard(m_parameterDataLock);
		EntityParameterNameValueMapIter it = m_parameterData.find(parameterName);

		if(it!=m_parameterData.end())
		{
			ret = it->second;
		}
		else // try to find the default value of the parameter
		{
			ret = EntityAccessFactory::getInstance().getParameterDefaultValue(parameterName, m_typeKey);
			m_parameterData.insert(std::make_pair(parameterName, ret));
		}

		FUNCTION_EXIT;
		return ret;
	}

	EntityHelper::EntityParameterNameValueMap& EntityHelper::getAllParameters()
	{
		if ( m_parameterData.size() == 0 )
		{
			// Need to retrieve the parameter data from the database
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_SELECT_20551, m_key, m_type);
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20551, m_key, m_type);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20551, m_key, m_type);
//			TA_ASSERT(strSql.size() > 0, "EntityHelper::getAllParameters(): Prepare SQL Statement error");
			
            // create the SQL string to retrieve the parameters from the database - the manner
            // in which the parameters will be retrieved depends on whether or not we have an 
            // entity key for this entity as yet.
            /*std::ostringstream sql;
			sql << "SELECT ep.name, nvl( ev.value, ep.ep_default ) FROM entityparameter ep, entityparametervalue ev WHERE ";
			sql << "ep.typekey = ( select pkey from entitytype where name = '";
			sql << m_type << "' ) and ep.pkey = ev.parameterkey(+) and ev.entitykey(+) = " << m_key;*/
			
            // Set up the columnNames vector to be passed to executeQuery()
            std::vector<std::string> columnNames;
			columnNames.push_back(PARAMETERNAME);
            columnNames.push_back(PARAMETERVALUE);
			
            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.                
            IData* data= databaseConnection->executeQuery( strSql, columnNames );
			int recordNum = data->getNumRows();
			for(int i = 0 ; i < recordNum ; ++i)
			{
				assignDefaultParameterValueData(data, i);
			}

			delete data;
		}

		return m_parameterData;
	}

	IData& EntityHelper::getConfigParameters()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::getConfigParameters" );

        if (NULL == m_configParameterData)
        { // Need to retrieve the parameter data from the database
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

            // create the SQL string to retrieve the parameters from the database - the manner
            // in which the parameters will be retrieved depends on whether or not we have an 
            // entity key for this entity as yet.
            std::ostringstream entityString;
            entityString << m_key;

		SQLStatement sql;

		    // The nvl function is needed in the following queries for the fields ep_default and
		    // ep_default_datatype, as the query would otherwise fail if these fields are null.

            if (0 == m_key) // No database write as yet - need to retrieve the default parameters
                            // and populate the value column with null
            {
                /*sql = "select ep.name, nvl(ep.ep_default_datatype, 0) as PARAMETERTYPE, '' as value, ep.ep_default, ";
                sql += " ep.visible, ep.ep_mandatory, ep.onlineupdate, epdt.datatype, ep_validation from";
                sql += " entityparameter ep, entityparameterdatatype epdt where ep.typekey = (select pkey from entitytype";
                sql += " where name = '" + (databaseConnection->escapeInsertString(m_type)) + "') and epdt.pkey = ep.ep_default_datatype";*/
//				sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_SELECT_20552, (databaseConnection->escapeInsertString(m_type)) );
//				sql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20552, (databaseConnection->escapeInsertString(m_type)) );

		databaseConnection->prepareSQLStatement(sql, ENTITYPARAMETER_SELECT_20552, (databaseConnection->escapeInsertString(m_type)) );
//				TA_ASSERT(sql.size() > 0, "EntityHelper::getConfigParameters(): Prepare SQL Statement error");
            }
            else // Get the parameters specifically for this entity
            {
                // The union used in this select statement allows for the retrieval of ALL parameter
                // names, even those not specifically set for this entity.

				/*sql = "select ep.name, nvl(ep.ep_default_datatype, 0) as PARAMETERTYPE, ev.value, ep.ep_default, ";
				sql += "ep.visible, ep.ep_mandatory, ep.onlineupdate, epdt.datatype,  ep_validation from";
				sql += " entityparameter ep, entityparametervalue ev, entityparameterdatatype epdt where ep.typekey = ";
				sql += "(select pkey from entitytype where name = '";
				sql += (databaseConnection->escapeInsertString(m_type)) + "')";
				sql += " and ep.pkey = ev.parameterkey and ev.entitykey = ";
				sql += entityString.str();
				sql += " and epdt.pkey = ep.ep_default_datatype ";
				sql += " union ";
				sql += "select ep.name, nvl(ep.ep_default_datatype, 0), '' as value, ep.ep_default, ";
				sql += "ep.visible, ep.ep_mandatory, ep.onlineupdate, epdt.datatype, ep_validation from";
				sql += " entityparameter ep, entity e, entityparameterdatatype epdt where e.deleted = 0 and ep.typekey = e.typekey and e.pkey = ";
				sql += entityString.str() + " and ";
				sql += "ep.name not in ( select ep.name from entityparameter ep, ";
				sql += "entityparametervalue ev where ep.typekey = (select pkey from entitytype ";
                sql += "where name = '" + (databaseConnection->escapeInsertString(m_type));
                sql += "') and ep.pkey = ev.parameterkey and ev.entitykey = ";
                sql += entityString.str() + ")";
                sql += " and epdt.pkey = ep.ep_default_datatype ";*/
//				sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_SELECT_20553, (databaseConnection->escapeInsertString(m_type)),
//				sql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20553, (databaseConnection->escapeInsertString(m_type)),

		databaseConnection->prepareSQLStatement(sql, ENTITYPARAMETER_SELECT_20553, (databaseConnection->escapeInsertString(m_type)),
					entityString.str(), entityString.str(), (databaseConnection->escapeInsertString(m_type)), entityString.str());
//				TA_ASSERT(sql.size() > 0, "EntityHelper::getConfigParameters(): Prepare SQL Statement error");
			    
            }

            // Set up the columnNames vector to be passed to executeQuery()
            std::vector<std::string> columnNames;
            columnNames.push_back(PARAMETERNAME);
            columnNames.push_back(PARAMETERTYPE);
            columnNames.push_back(PARAMETERVALUE);
            columnNames.push_back(PARAMETERDEFAULTVALUE);
            columnNames.push_back(PARAMETERVISIBLE);
            columnNames.push_back(PARAMETERMANDATORY);
            columnNames.push_back(PARAMETERONLINEUPDATE);
            columnNames.push_back(PARAMETERDATATYPE);
            columnNames.push_back(PARAMETERVALIDATION);

            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.                
            m_configParameterData = databaseConnection->executeQuery(sql,columnNames);
        }
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::getConfigParameters" );
        return *(m_configParameterData);
    }

	void EntityHelper::assignDefaultParameterValueData( IData* data, unsigned long row )
	{
		FUNCTION_ENTRY("EntityHelper::assignDefaultParameterValueData");

		std::string parameterName = data->getStringData(row, PARAMETERNAME);
		std::string parameterNameValue = data->getStringData(row, PARAMETERVALUE);

		TA_Base_Core::ThreadGuard guard(m_parameterDataLock);
		m_parameterData[parameterName] = parameterNameValue;

		FUNCTION_EXIT;
	}

	//++zhou yuan

    void EntityHelper::assignDefaultParameterValueData(const std::string &parameterName,const std::string& parameterValue)
    {
        FUNCTION_ENTRY("EntityHelper::assignDefaultParameterValueData");

        TA_Base_Core::ThreadGuard guard(m_parameterDataLock);
        m_parameterData[parameterName] = parameterValue;

        FUNCTION_EXIT;
    }
    
    void EntityHelper::writeParameters(std::map<std::string, std::string>& parameters)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::writeParameters" );
        TA_ASSERT(0 != m_key, "writeParameters called before the entity was written to the database");
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // The parameters map is in the form of Name->Value, so need to iterate through
        // and extract each name/value pair and write it to the database.
        std::map<std::string, std::string>::iterator iter (parameters.begin());
        std::map<std::string, std::string>::iterator iterEnd (parameters.end());

        while (iter != iterEnd)
        {
            // extract the name and value from the map
            std::string parameterName = iter->first;
            std::string parameterValue = iter->second;

            // Check that the parameter name exists
           /* std::string sql = "select pkey from entityparameter where name = '" + 
				databaseConnection->escapeQueryString(parameterName) + 
				"' and typekey = (select pkey from entitytype where name = '" + 
				databaseConnection->escapeQueryString(m_type) + "')";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20501, 
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20501, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20501,
				databaseConnection->escapeQueryString(parameterName), databaseConnection->escapeQueryString(m_type));
//			TA_ASSERT(strSql.size() > 0, "EntityHelper::writeParameters(): Prepare SQL Statement error");
          
            // Can just create the vector with any value - we're not going to try getting 
            // anything out of it. This select is just to check the parameter name actually
            // exists.
            std::vector<std::string> columnNames;
            columnNames.push_back("PKEY");

            // Run the query
            IData* data = databaseConnection->executeQuery(strSql,columnNames);

            // check the number of rows returned - 1 is good, antything else is bad.
            if (0 == data->getNumRows()) // No entry found with the specified name
            {
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "No parameter named " << parameterName.c_str();
                reasonMessage << " exists for type " << m_type.c_str();
                TA_THROW (DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
            }
            else if (1 < data->getNumRows()) // More than one entry found for the name
            {
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "More than one entry found of parameter " << parameterName.c_str();
                reasonMessage << " for type " << m_type.c_str();
                TA_THROW (DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
            }
            //If it gets to here, then the select returned exactly one row.

            // okay, now we know the parameter name exists and is unique, so we can 
            // go ahead and check if the parameter value exists.
            delete data;
            data = NULL;
            
            std::ostringstream entityString;
            entityString << m_key;

            // This query will return the pkey of the parametervalue by name. As above, the
            // actual result of the query is not important, but the number of results is.
            /*sql = "select pkey from entityparametervalue where parameterkey = ";
            sql += "(select pkey from entityparameter where name = '";
            sql += databaseConnection->escapeQueryString(parameterName) + "'";
			sql += " and typekey = (select pkey from entitytype where name = '";
			sql += databaseConnection->escapeQueryString(m_type);
            sql += "')) and entitykey = ";
            sql += databaseConnection->escapeQueryString(entityString.str());*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20502, databaseConnection->escapeQueryString(parameterName), 
//			strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20502, databaseConnection->escapeQueryString(parameterName), 

		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20502, databaseConnection->escapeQueryString(parameterName),
				databaseConnection->escapeQueryString(m_type), databaseConnection->escapeQueryString(entityString.str()) );
//			TA_ASSERT(strSql.size() > 0, "EntityHelper::writeParameters(): Prepare SQL Statement error");

            // Re-use the columnNames vector as is, because it already contains one column
            // named pkey. The lines included in comments below are to enhance readability.
            // columnNames.clear();             - not needed here, so commented out
            // columnNames.push_back("PKEY");   - not needed here, so commented out

            //execute the query
            data = databaseConnection->executeQuery(strSql,columnNames);

            // check the number of rows returned - 1 means it exists, and we have to UPDATE  
            // the value, 0 means it needs to be INSERTed. More than 1 is bad.
            if (0 == data->getNumRows()) // A new parametervalue needs to be added to the database
            {
                // Only insert a new value if the value is not empty
                if( !parameterValue.empty() )
                {
                    LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                        "Parameter %s does not exist and now has a valid value. A new parameter will be added.",
                        parameterName.c_str());
					/*sql = "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
					sql += getLocalDatabaseName();
					sql += "','PUBLIC', '";
                    sql += "insert into entityparametervalue (pkey,parameterkey,entitykey,value)";
                    sql += " values (entityparametervalue_seq.nextval,";
                    sql += "(select pkey from entityparameter where name = ''";
                    sql += databaseConnection->escapeAQSQLString(parameterName);
				    sql += "'' and typekey = (select pkey from entitytype where name = ''";
				    sql += databaseConnection->escapeAQSQLString(m_type) + "''))";
                    sql += ", " + databaseConnection->escapeAQSQLString(entityString.str());
                    sql += ", ''" + databaseConnection->escapeAQSQLString(parameterValue) + "'')'); END;";*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_INSERT_20851, getLocalDatabaseName(), 
//					strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_INSERT_20851, getLocalDatabaseName(), 

		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_INSERT_20851, getLocalDatabaseName(),databaseConnection->getNextPkey("ENTITYPARAMETERVALUE_SEQ"),
						databaseConnection->escapeAQSQLString(parameterName), databaseConnection->escapeAQSQLString(m_type),  
						databaseConnection->escapeAQSQLString(entityString.str()), databaseConnection->escapeAQSQLString(parameterValue));
//					TA_ASSERT(strSql.size() > 0, "EntityHelper::writeParameters(): Prepare SQL Statement error");

                    // execute the insert
                    databaseConnection->executeModification(strSql);
                }

            }
            else if (1 == data->getNumRows()) // Update existing parametervalue
            {
                if (!parameterValue.empty())
                {
                    LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                        "Parameter %s already exists. The current value will be updated.",
                        parameterName.c_str());
					/*sql = "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
					sql += getLocalDatabaseName();
					sql += "','PUBLIC', '";
                    sql += "update entityparametervalue set value = ''";
                    sql += databaseConnection->escapeAQSQLString(parameterValue);
                    sql += "'' where parameterkey = ";
				    sql += "(select pkey from entityparameter where name = ''";
				    sql += databaseConnection->escapeAQSQLString(parameterName) + "''";
				    sql += " and typekey = (select pkey from entitytype where name = ''";
				    sql += databaseConnection->escapeAQSQLString(m_type);
				    sql += "'')) and entitykey = ";			
                    sql += databaseConnection->escapeAQSQLString(entityString.str());
					sql += "'); END;";*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_UPDATE_20701, getLocalDatabaseName(),
//					strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_UPDATE_20701, getLocalDatabaseName(),

		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_UPDATE_20701, getLocalDatabaseName(),
						databaseConnection->escapeAQSQLString(parameterValue), databaseConnection->escapeAQSQLString(parameterName), 
						databaseConnection->escapeAQSQLString(m_type), databaseConnection->escapeAQSQLString(entityString.str()));
//					TA_ASSERT(strSql.size() > 0, "EntityHelper::writeParameters(): Prepare SQL Statement error");
                }
                else
                {
                    // The user has set this parameter to be an empty value so we want to 
                    // delete the parametervalue from the database
					/*sql = "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
                    sql += getLocalDatabaseName();
                    sql += "','PUBLIC', '";
                    sql += "delete from entityparametervalue where parameterkey = ";
				    sql += "(select pkey from entityparameter where name = ''";
				    sql += databaseConnection->escapeAQSQLString(parameterName) + "''";
				    sql += " and typekey = (select pkey from entitytype where name = ''";
				    sql += databaseConnection->escapeAQSQLString(m_type);
				    sql += "'')) and entitykey = ";			
                    sql += databaseConnection->escapeAQSQLString(entityString.str());
					sql += "'); END;";*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_DELETE_20951, getLocalDatabaseName(),
//					strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_DELETE_20951, getLocalDatabaseName(),

		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_DELETE_20951, getLocalDatabaseName(),
						databaseConnection->escapeAQSQLString(parameterName), databaseConnection->escapeAQSQLString(m_type),databaseConnection->escapeAQSQLString(entityString.str()) );
//					TA_ASSERT(strSql.size() > 0, "EntityHelper::writeParameters(): Prepare SQL Statement error");
                }


                // execute the update
                databaseConnection->executeModification(strSql);
            }
            else // More than one row returned...that's bad. Throw an exception.
            {
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "More than one value found for parametertype " << parameterName.c_str();
                reasonMessage << " of entitytype " << m_type.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"parameter name") );
            }
            //clean up the pointer
            delete data;
            data = NULL;
			
			//Bohong++ sync the write data into memory cached map
			TA_Base_Core::ThreadGuard guard(m_parameterDataLock);
			m_parameterData[parameterName] = parameterValue;

            // increment the iterator, and do it all again
	        ++iter;
        }
        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::writeParameters" );

    }


    void EntityHelper::writeEntityData()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::writeEntityData" );
        // Set up the boolean variables that specify if something is wrong.
        bool addressValid = "" != m_address;
        bool descriptionValid = "" != m_description;
        bool nameValid = "" != m_name;
        bool locationValid = ULONG_MAX != m_location;
		bool regionValid = ULONG_MAX != m_region;
        //bool subsystemValid = ULONG_MAX != m_subsystem; NJ - removed 19/03/04. subsystem can now be NULL, so there is no need to force a caller to set a subsystem
        bool parentValid = ULONG_MAX != m_parent;

        // don't need to check type or key - the key will never be "invalid", while type is
        // hardcoded, and not read from the database.
        
        // Now need to determine which of the above tests failed, and add each failure
        // to a vector of failed field names. Unfortunately, this leads to a lot of if 
        // statements, but there's no way around it.
        std::vector<std::string> fieldNames;
        if (!addressValid)
        {
            fieldNames.push_back("Address");
        }
        if (!descriptionValid)
        {
            fieldNames.push_back("Description");
        }
        if (!nameValid)
        {
            fieldNames.push_back("Name");
        }
        if (!locationValid)
        {
            fieldNames.push_back("Location");
        }
		if (!regionValid)
        {
            fieldNames.push_back("Region");
        }
        if (!parentValid)
        {
            fieldNames.push_back("Parent");
        }
        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
                TA_THROW( TA_Base_Core::DataConfigurationException("Entity data not fully specified. Entity cannot be written to database", fieldNames) );
        }
        // Write the Entity to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will be updated
        if (0 == m_key) // This is a new entity
        {
            addNewEntity();
        }
        else // This is an existing entity
        {
            modifyExistingEntity();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_originalParent = m_parent;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::writeEntityData" );
    }


	void EntityHelper::assignDefaultData( IData* data, unsigned long row )
	{
		FUNCTION_ENTRY( "assignDefaultData" );

		// process the data
		processData( data, row );

		m_isValidData = true;

		FUNCTION_EXIT;
	}


    void EntityHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::reload" );
        TA_ASSERT(0 != m_key, "This entity has not yet been written to the database, therefore it cannot be reloaded");
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the data of the entity
        // based upon the key
        std::ostringstream entityString;
        entityString << m_key;

        /*std::string sql = "select TYPEKEY,NAME,ADDRESS,DESCRIPTION,SUBSYSTEMKEY,SUBSYSTEMNAME,nvl(PHYSICAL_SUBSYSTEM_KEY,0) as PHYSICAL_SUBSYSTEM_KEY,";
        sql += "PHYSICAL_SUBSYSTEM_NAME,LOCATIONKEY,LOCATIONNAME,SEREGI_ID,SEREGINAME,PARENTKEY,PARENTNAME,AGENTKEY,AGENTNAME,TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),";
        sql += "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')";
        sql += "from entity_v where deleted = 0 and pkey = " + databaseConnection->escapeQueryString(entityString.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2574,databaseConnection->escapeQueryString(entityString.str()));
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2574,databaseConnection->escapeQueryString(entityString.str()));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2574,databaseConnection->escapeQueryString(entityString.str()));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::reload(): Prepare SQL Statement error");


        // Set up the columnNames vector to be passed to executeQuery()
        std::string typeColumn = "TYPEKEY";
        std::string nameColumn = "NAME";
        std::string addressColumn = "ADDRESS";
        std::string descriptionColumn = "DESCRIPTION";
        std::string subsystemColumn = "SUBSYSTEMKEY";
        std::string subsystemNameColumn = "SUBSYSTEMNAME";
        std::string physicalSubsystemColumn = "PHYSICAL_SUBSYSTEM_KEY";
        std::string physicalSubsystemNameColumn = "PHYSICAL_SUBSYSTEM_NAME";
        std::string locationColumn = "LOCATIONKEY";
        std::string locationNameColumn = "LOCATIONNAME";
        std::string regionColumn = "SEREGI_ID";
        std::string regionNameColumn = "SEREGINAME";
        std::string parentColumn = "PARENTKEY";
        std::string parentNameColumn = "PARENTNAME";
		std::string agentColumn = "AGENTKEY";
		std::string agentNameColumn = "AGENTNAME";
        std::string dateCreatedColumn = "DATECREATED";
        std::string dateModifiedColumn = "DATEMODIFIED";
        std::vector<std::string> columnNames;
        columnNames.push_back(typeColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(addressColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(subsystemColumn);
        columnNames.push_back(subsystemNameColumn);
        columnNames.push_back(physicalSubsystemColumn);
        columnNames.push_back(physicalSubsystemNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(locationNameColumn);
		columnNames.push_back(regionColumn);
        columnNames.push_back(regionNameColumn);
        columnNames.push_back(parentColumn);
        columnNames.push_back(parentNameColumn);
		columnNames.push_back(agentColumn);
		columnNames.push_back(agentNameColumn);
        columnNames.push_back(dateCreatedColumn);
        columnNames.push_back(dateModifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

		// Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "No entity found for pkey = " << m_key;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one entity found for pkey = " << m_key;			
            TA_THROW (DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"") );
        }

		// process the data
		processData( data, 0 );

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        initialiseMaxFieldLengths();
        
        // Need to record that we now have valid data
        m_isValidData = true;

		//zhou yuan++
		m_parameterData.clear();
		//++zhou yuan

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::reload" );
    }

	void EntityHelper::processData( IData* data, unsigned long row )
	{  
        std::string typeColumn = "TYPEKEY";
        std::string nameColumn = "NAME";
        std::string addressColumn = "ADDRESS";
        std::string descriptionColumn = "DESCRIPTION";
        std::string subsystemColumn = "SUBSYSTEMKEY";
        std::string subsystemNameColumn = "SUBSYSTEMNAME";
        std::string physicalSubsystemColumn = "PHYSICAL_SUBSYSTEM_KEY";
        std::string physicalSubsystemNameColumn = "PHYSICAL_SUBSYSTEM_NAME";
        std::string locationColumn = "LOCATIONKEY";
        std::string locationNameColumn = "LOCATIONNAME";
		std::string regionColumn = "SEREGI_ID";
        std::string regionNameColumn = "SEREGINAME";
        std::string parentColumn = "PARENTKEY";
        std::string parentNameColumn = "PARENTNAME";
		std::string agentColumn = "AGENTKEY";
		std::string agentNameColumn = "AGENTNAME";
        std::string dateCreatedColumn = "DATECREATED";
        std::string dateModifiedColumn = "DATEMODIFIED";

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_typeKey = data->getUnsignedLongData(row,typeColumn);
        m_address = data->getStringData(row,addressColumn);
        m_description = data->getStringData(row,descriptionColumn);
        m_name = data->getStringData(row,nameColumn);
        m_fixedSubsystem = data->getUnsignedLongData(row,subsystemColumn);
        m_fixedSubsystemName = data->getStringData(row,subsystemNameColumn);
        // Subsystem can be null, so need to check it before calling getUnsignedLongData
        m_subsystem = data->getUnsignedLongData(row,physicalSubsystemColumn);
        m_subsystemName = data->getStringData(row,physicalSubsystemNameColumn);
        m_location = data->getUnsignedLongData(row,locationColumn);
        m_locationName = data->getStringData(row,locationNameColumn);
		m_region = data->getUnsignedLongData(row,regionColumn);
        m_regionName = data->getStringData(row,regionNameColumn);
        m_parent = data->getUnsignedLongData(row,parentColumn);
        m_parentName = data->getStringData(row,parentNameColumn);
        m_originalParent = m_parent;
		m_agent = data->getUnsignedLongData(row,agentColumn);
		m_agentName = data->getStringData(row,agentNameColumn);
        m_dateCreated = data->getDateData(row,dateCreatedColumn,0);
        m_dateModified = data->getDateData(row,dateModifiedColumn,0);
	}


    void EntityHelper::deleteEntity()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "EntityHelper::deleteEntity" );
        TA_ASSERT(0 != m_key, "This entity does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        checkForChildEntities(); // This will throw an exception if child entities are found

        // Ok there are no child entities so we are right to delete
        // We must retrieve the new name for the entity

		// first make sure the max length fields have been set..
		if(m_nameMaxLength <= 0)
		{
			initialiseMaxFieldLengths();
		}

        std::string newEntityName = databaseConnection->escapeInsertString(retrieveDeletedNameForEntity(m_nameMaxLength));

        /*std::ostringstream sql;
        sql << "update ENTITY set DELETED = 1, name = '" << newEntityName << "' where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_UPDATE_4651, newEntityName, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_UPDATE_4651, newEntityName, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_UPDATE_4651, newEntityName, m_key);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::deleteEntity(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "EntityHelper::deleteEntity" );
    }


    void EntityHelper::addNewEntity()
    {
        FUNCTION_ENTRY("addNewEntity");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Entity %s is a new entity. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // First check there is not alreay an entity with this name (it is a uniqueness
        // constraint).
        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from ENTITY_V where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2516, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2516, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2516, databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::addNewEntity(): Prepare SQL Statement error");

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An entity already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        unsigned long newAgent = getNewAgentKey();
        const unsigned long PKEY_RANGE_PER_LOCATION = 10000000;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        // TD4708:  New pkey must be in the same group as other entities for that location...
        // TD10928: ...but careful not to choose a pkey that's already taken (should an entity's 
        //          location be changed).
		std::ostringstream osSubsystem;
        /*formatSQL.str("");
        formatSQL << "insert into ENTITY (PKEY,NAME,ADDRESS,DESCRIPTION,";
        formatSQL << "SUBSYSTEMKEY,PHYSICAL_SUBSYSTEM_KEY,LOCATIONKEY,SEREGI_ID,TYPEKEY,PARENTKEY,AGENTKEY) ";
        formatSQL << "values ( (SELECT MAX(pkey)+1 FROM entity WHERE pkey > (" << m_location << "+1) * ";
        formatSQL << PKEY_RANGE_PER_LOCATION << " ";
        formatSQL << "and pkey < (" << m_location <<  "+2) * ";
        formatSQL << PKEY_RANGE_PER_LOCATION << "),'";
        formatSQL << databaseConnection->escapeInsertString(m_name) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_address) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_description) << "',";
        formatSQL << m_fixedSubsystem << ",";*/
        // If the subsystem is 0, then we want to add null...
        if (m_subsystem == 0)
        {
            osSubsystem << "NULL";
        }
        else
        {
            osSubsystem << m_subsystem;
        }
		/*formatSQL << m_location << ",";
        formatSQL << m_region << ",";
        formatSQL << "(select PKEY from ENTITYTYPE where NAME = '";
		formatSQL << databaseConnection->escapeInsertString(m_type) + "'),";
		formatSQL << m_parent << ",";
        formatSQL << newAgent << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_INSERT_4801, m_location, PKEY_RANGE_PER_LOCATION, m_location,
//		strSql  = databaseConnection->prepareSQLStatement(ENTITY_INSERT_4801, m_location, PKEY_RANGE_PER_LOCATION, m_location,

		databaseConnection->prepareSQLStatement(strSql, ENTITY_INSERT_4801, getNextPkey(m_location), 
			databaseConnection->escapeInsertString(m_name),databaseConnection->escapeInsertString(m_address),
			databaseConnection->escapeInsertString(m_description), m_fixedSubsystem, osSubsystem.str(), m_location, m_region, 
			databaseConnection->escapeInsertString(m_type), m_parent, newAgent);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::addNewEntity(): Prepare SQL Statement error");
 
        databaseConnection->executeModification(strSql);

        m_agent = newAgent;

        //now need to get the PKEY back out for this entity and update the date fields
        /*std::ostringstream sql;
		sql << "select PKEY, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from ENTITY_V where NAME = '";
        sql << databaseConnection->escapeQueryString(m_name).c_str() << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2575, databaseConnection->escapeQueryString(m_name));
//		strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2575, databaseConnection->escapeQueryString(m_name));

		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2575, databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::addNewEntity(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void EntityHelper::modifyExistingEntity()
    {
        FUNCTION_ENTRY("modifyExistingEntity");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Entity %s already exists. It's deatils will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select PKEY from entity where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4503, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4503, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4503, databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::modifyExistingEntity(): Prepare SQL Statement error");

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An entity already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entity already exists name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

        unsigned long newAgentKey = getNewAgentKey();

        std::ostringstream sqlSubsystem;
        /*sql << "update ENTITY set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', ADDRESS = '" << (databaseConnection->escapeQueryString(m_address)).c_str();
        sql << "', DESCRIPTION = '" << (databaseConnection->escapeQueryString(m_description)).c_str();
        sql << "', PHYSICAL_SUBSYSTEM_KEY = ";*/
        // If subsystem key is 0, need to set the database entry to NULL.
        if (m_subsystem == 0)
        {
            sqlSubsystem << "NULL";
        }
        else
        {
            sqlSubsystem << m_subsystem;
        }
        /*sql << ", LOCATIONKEY = " << m_location;
        sql << ", SEREGI_ID = " << m_region;
        sql << ", TYPEKEY = (select PKEY from ENTITYTYPE where NAME = '" << (databaseConnection->escapeQueryString(m_type)).c_str();
        sql << "'), PARENTKEY = " << m_parent;
        sql << ", AGENTKEY = " << newAgentKey;
        sql << " where pkey = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_UPDATE_4652, (databaseConnection->escapeQueryString(m_name)),
//		strSql  = databaseConnection->prepareSQLStatement(ENTITY_UPDATE_4652, (databaseConnection->escapeQueryString(m_name)),

		databaseConnection->prepareSQLStatement(strSql, ENTITY_UPDATE_4652, (databaseConnection->escapeQueryString(m_name)),
			(databaseConnection->escapeQueryString(m_address)), (databaseConnection->escapeQueryString(m_description)), sqlSubsystem.str(), m_location,
			m_region, (databaseConnection->escapeQueryString(m_type)), m_parent, newAgentKey, m_key );
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::modifyExistingEntity(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        // If the agent key is new we must change all the child entities as well
        if (newAgentKey != m_agent)
        {
            modifyAgentKeys(newAgentKey);
            m_agent = newAgentKey;
        }

        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') from ENTITY_V where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str() << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2576, (databaseConnection->escapeQueryString(m_name)));
//		strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2576, (databaseConnection->escapeQueryString(m_name)));

		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2576, (databaseConnection->escapeQueryString(m_name)));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::modifyExistingEntity(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }

    
    unsigned long EntityHelper::getNewAgentKey()
    {
        FUNCTION_ENTRY("getNewAgentKey");

        if (m_isManuallySetAgent)
        {
            // The agent key was set by the user manually so we don't want to automatically generate one
            FUNCTION_EXIT;
            return m_agent;
        }

        if (m_originalParent == m_parent)
        {
            // Parent hasn't changed so the agent doesn't need to change
            FUNCTION_EXIT;
            return m_agent;
        }

        if ( (m_agent == m_key) && (m_key != 0) )
        {
            // If the agent and key are the same and this is not a new item then
            // this is an agent so the agent key should not change
            FUNCTION_EXIT;
            return m_agent;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        /*std::ostringstream sql;
        sql << "select agentkey from entity where deleted = 0 and pkey = " << m_parent;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4504, m_parent);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4504, m_parent);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4504, m_parent);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::getNewAgentKey(): Prepare SQL Statement error");

        std::string parentAgentKeyColumn = "PARENT_AGENT_KEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(parentAgentKeyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for pkey = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for pkey = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"key") );
        }

        unsigned long parentAgentKey;
        try
        {
            parentAgentKey = data->getUnsignedLongData(0, parentAgentKeyColumn);

            delete data;
            data = NULL;
        }
        catch( ... )
        {
            delete data;
            throw;
        }

        if (parentAgentKey == 0)
        {
            // Our parent's agentkey is 0 so we do not want to update ours
            FUNCTION_EXIT;
            return m_agent;
        }

        // If we get to here we need to update the agent key to this value
        FUNCTION_EXIT;
        return parentAgentKey;
    }


    void EntityHelper::modifyAgentKeys(unsigned long newAgentKey)
    {
        FUNCTION_ENTRY("modifyAgentKeys");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_Oracle_UPDATE_4701,m_key, newAgentKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_UPDATE_4701,m_key, newAgentKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_UPDATE_4701,m_key, newAgentKey);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::modifyAgentKeys(): Prepare SQL Statement error");

       /* std::ostringstream sqlFunctionCall;
        sqlFunctionCall << "call agent_config.change_child_agent(" << m_key;
        sqlFunctionCall << ", null, " << newAgentKey << ")";*/

        databaseConnection->executeProcedure( strSql );

        FUNCTION_EXIT;
    }


    void EntityHelper::checkForChildEntities()
    {
        FUNCTION_ENTRY("checkForChildEntities");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2517, m_key, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2517, m_key, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2517, m_key, m_key);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::checkForChildEntities(): Prepare SQL Statement error");

        // If there are any entities with this entity as the parent key or agent then we cannot delete
        // until they have been moved.

        /*std::ostringstream sql;
        sql << "select name from ENTITY_V where deleted = 0 and (PARENTKEY = " << m_key;
        sql << " or AGENTKEY = " << m_key << ") and AGENTKEY <> PKEY";*/

        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(strSql,columnNames,11);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        if (data->getNumRows() > 0)
        {
            std::string childEntities("");
            // Only display the first ten items that we receive. If there are more then show
            // this using "...<more>".
            for( unsigned int i = 0; (i < data->getNumRows() && i < 10); ++i)
            {
                childEntities += data->getStringData(i,nameColumn);
                childEntities += "\n";
            }

            if (data->getNumRows() > 10)
            {
                childEntities += "...<more>\n";
            }

            delete data;

            TA_THROW( DataException("This entity cannot be deleted as it has child entities.", DataException::CANNOT_DELETE, childEntities ) );
        }

        FUNCTION_EXIT;
    }


    std::string EntityHelper::retrieveDeletedNameForEntity(unsigned long maxFieldSize)
    {
        FUNCTION_ENTRY("retrieveDeletedNameForEntity");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        std::string nameToSearchFor = m_name + DELETED_FLAG;

        if(nameToSearchFor.size() > maxFieldSize)
        {
            // Remove the character before the delete flag
            nameToSearchFor.erase(nameToSearchFor.size() - 3, 2);
        }

        /*std::ostringstream sql;
        sql << "select name from ENTITY where name like '" << databaseConnection->escapeQueryString(nameToSearchFor) << "%'";
        sql << " and DELETED <> 0";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4505,databaseConnection->escapeQueryString(nameToSearchFor) );
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4505,databaseConnection->escapeQueryString(nameToSearchFor) );
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4505,databaseConnection->escapeQueryString(nameToSearchFor) );
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::retrieveDeletedNameForEntity(): Prepare SQL Statement error");

        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        // If no rows are returned then we haven't got any deleted items by teh same name. This means
        // we can just append the first number.
        if (data->getNumRows() == 0)
        {
            std::string newName(m_name);

            // If the field has already reached its max length, remove the last character so it can still
            // be inserted in to the database.
            if(newName.size() + 1 > maxFieldSize)
            {
                newName.erase(newName.size() - 2, 2);
            }

            newName += DELETED_FLAG;
            newName += "1";

            delete data;
            data= NULL;

            FUNCTION_EXIT;
            return newName;
        }

        // If we get to here then we have other entities by the same name that have already been
        // deleted. We must determine the next number to append to the end. We must step through
        // each returned item and find the highest number.
        unsigned long highestAppendedNumber = 0;

        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                std::string name = data->getStringData(i,nameColumn);
                std::istringstream appendedNumber;

                // Retrieve all the text on the string after the text matching the nameToSearchFor
                appendedNumber.str( name.substr( nameToSearchFor.size(), name.length() - nameToSearchFor.length() ) );
                
                // Convert the string into an unsigned long
                unsigned long number;
                appendedNumber >> number;

                if (number > highestAppendedNumber)
                {
                    highestAppendedNumber = number;
                }
            }

            delete data;
            data = NULL;
        } while ( databaseConnection->moreData(data) );

        // If the field has already reached its max length, remove the last few characters so it can still
        // be inserted in to the database.
        std::ostringstream numberString;
        numberString << highestAppendedNumber+1;
        if(nameToSearchFor.size() + numberString.str().size() > maxFieldSize)
        {
            unsigned long trimLength = maxFieldSize - (nameToSearchFor.size() + numberString.str().size());
            std::string::size_type deletedFlagPos = nameToSearchFor.find(DELETED_FLAG);
            if(deletedFlagPos != std::string::npos)
            {
                nameToSearchFor.erase(deletedFlagPos - trimLength, trimLength);
            }
        }

        std::ostringstream newName;
        newName << nameToSearchFor << highestAppendedNumber+1;

        FUNCTION_EXIT;
        return newName.str();
    }


    void EntityHelper::getSubsystemForNewEntity()
    {
        FUNCTION_ENTRY("getSubsystemForNewEntity");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4017, databaseConnection->escapeQueryString(getEntityType()));
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4017, databaseConnection->escapeQueryString(getEntityType()));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4017, databaseConnection->escapeQueryString(getEntityType()));
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::getSubsystemForNewEntity(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select TA_SUBSYSTEMKEY from entitytype where name = '" << databaseConnection->escapeQueryString(getEntityType()) << "'";*/

        std::string subsystemColumn = "subsystem";
        std::vector<std::string> columnNames;
        columnNames.push_back(subsystemColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

		// Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "No subsystem found for entitytype = " << getTypeKey();		
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "Multiple subsystems found for entitytype = " << getTypeKey();			
            TA_THROW (DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"") );
        }

        m_fixedSubsystem = data->getUnsignedLongData(0, subsystemColumn);

        FUNCTION_EXIT;
    }

    void EntityHelper::initialiseMaxFieldLengths()
    {
        FUNCTION_ENTRY("initialiseMaxFieldLengths");

        TA_Base_Core::ThreadGuard guard(m_nameMaxLengthLock);
        if (m_nameMaxLength > 0)
        {
            // we've already gotten the max length from last time, so don't need to fetch it again
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_Oracle_SELECT_4551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4551);
//		TA_ASSERT(strSql.size() > 0 , "EntityHelper::initialiseMaxFieldLengths(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select COLUMN_NAME, DATA_LENGTH from USER_TAB_COLS where TABLE_NAME = 'ENTITY' ";
        sql << "and COLUMN_NAME = 'NAME'";*/

        std::string columnName = "COLUMN_NAME";
        std::string columnLength = "COLUMN_LENGTH";
        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);
        columnNames.push_back(columnLength);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve column lengths. Setting to default values");

            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

            FUNCTION_EXIT;
            return;
        }
        else if (1 < data->getNumRows()) 
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving column lengths. Setting to default values");

            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

            FUNCTION_EXIT;
            return;
        }
        
        try
        {
            m_nameMaxLength = data->getUnsignedLongData(0,columnLength);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            
            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }

	std::string EntityHelper::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, OnlineUpdatable_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(OnlineUpdatable_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

	 //TD15302 Mintao++
    EntityStatusData EntityHelper::getEntityStatusData(EEntityStatusType statustype)
    {
        EntityStatusData toReturn;	
		// create a default-initialized entity status data
		// in case database query gets an error, there is  some data
		// being returned
 		toReturn.entityKey = m_key;
		toReturn.entityStatusType = statustype;
		toReturn.lastSessionId = "";
		toReturn.lastModifyDate = 0;		
		toReturn.entityStatusDataType = getEntityStatusDataType(statustype);
		toReturn.entityStatusDataValue = getDefaultValueForStatutsType(toReturn.entityStatusDataType);
		
		if (0 != m_key )
		{
			if (!m_isValidStatusData ) 
			{   // No database write as yet - need to retrieve the default parameters			 
				//reload from database	
				reloadEntitystatusdata(toReturn.entityStatusType);		 
			}	 
			
			std::map<EEntityStatusType, EntityStatusData>::iterator itr;
			itr = m_entityStatusList.find(statustype);
			if (itr != m_entityStatusList.end())
			{	//compare if the value in the m_entityStatusList is 
				// still the same from that of the value stored in the DB
				// update the list 		
				toReturn = itr->second;			
			}			
		}
		else
		{
			std::ostringstream reasonMessage;
			reasonMessage << "No entity status data found for entitykey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
		}

        return toReturn;
    }

     //caller must catch the DataException and unknown exception when call this function
    bool EntityHelper::getBoolEntityStatusValue(EEntityStatusType statustype)
    {
        bool retStatusValue = false;

		if (0 != m_key)
		{

			if ( !m_isValidStatusData ) 
			{   // No database write as yet - need to retrieve the default parameters			 
				//reload from database	
				reloadEntitystatusdata(statustype);		 
			}	 

			EntityStatusData statusdata;
			std::map<EEntityStatusType, EntityStatusData>::iterator itr;
			itr = m_entityStatusList.find(statustype);
			if (itr == m_entityStatusList.end())
			{   // there is no entity status type registered for this entity, just return false
				return retStatusValue;
			}

			// retrieve status data for this status ty
			statusdata = m_entityStatusList[statustype];
			
			if (BOOLEAN_DATA == getEntityStatusDataType(statustype))
			{
				std::string BOOLEAN_STRING_TRUE = "1";
				if (statusdata.entityStatusDataValue.compare(BOOLEAN_STRING_TRUE.c_str()) == 0)
				{
					retStatusValue = true;
				}			
			}
			else
			{
				std::ostringstream reasonMessage;
				reasonMessage << "No entity status data found for entitykey = " << m_key;
				TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
			}
			
		}
        return retStatusValue;
    }

    void EntityHelper::setEntityStatusData(const EntityStatusData& entityStatus)
    {	   		
		if (!m_isValidStatusData && 0 != m_key) 
		{   // No database write as yet - need to retrieve the default parameters			 
			 //reload from database	
			reloadEntitystatusdata(entityStatus.entityStatusType);		 
		}		 
	        
		m_entityStatusList[entityStatus.entityStatusType] = entityStatus;
    }

    void EntityHelper::setEntityStatusData(unsigned int statustype, EStatusDataType datatype, const std::string& statusvalue, time_t dateModified )
    {
        // check if the entity status data is already existing,
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "setEntityStatusData");
		
		EntityStatusData statusdata;
	 	statusdata.entityKey = m_key;
		statusdata.entityStatusType = (EEntityStatusType)statustype;
		statusdata.lastSessionId = "";
		statusdata.lastModifyDate = dateModified;		
		statusdata.entityStatusDataType = datatype;
		statusdata.entityStatusDataValue = statusvalue;
		
		if (!m_isValidStatusData && 0 != m_key)
		{   // No database write as yet - need to retrieve the default parameters			 
			 //reload from database	
			reloadEntitystatusdata(statusdata.entityStatusType);		 
		}
		
		m_entityStatusList[statusdata.entityStatusType ] = statusdata;
		
    }
	void EntityHelper::reloadEntitystatusdata(EEntityStatusType statustype)
	{		
		//can not find in the current list, search in the database
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "reloadEntitystatusdata");
		EntityStatusData statusdata;
		statusdata.entityKey = m_key;
		statusdata.entityStatusType = statustype;

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);	
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ENTITYSTATUSDATA_Oracle_SELECT_21051, statusdata.entityKey, statusdata.entityStatusType);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYSTATUSDATA_SELECT_21051, statusdata.entityKey, statusdata.entityStatusType);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYSTATUSDATA_SELECT_21051, statusdata.entityKey, statusdata.entityStatusType);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::reloadEntitystatusdata(): Prepare SQL Statement error");

		/*std::ostringstream sql;
		sql.str("");		
		
		sql << "select " << LAST_MODIFIED_SESSION_COL << ", " << ENTITY_STATUS_VALUE_COL  << ", " << STATUS_DATATYPE_COL << " , ";
		sql << "TO_CHAR(" << LAST_MODIFIED_DATE_COL << ",'YYYYMMDDHH24MISS') ";
		sql << "from ENTITYSTATUSDATA WHERE ENTITYKEY = " << statusdata.entityKey << " and ENTITYSTATUSTYPE = " << statusdata.entityStatusType;*/
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::string LAST_MODIFIED_SESSION_COL = "LAST_MODIFIED_SESSION";
		std::string ENTITY_STATUS_VALUE_COL = "ENTITYSTATUSVALUE";
		std::string STATUS_DATATYPE_COL = "STATUSDATATYPE";
		std::string LAST_MODIFIED_DATE_COL = "LAST_MODIFIED_DATE";
		std::vector<std::string> columnNames;
		columnNames.push_back(LAST_MODIFIED_SESSION_COL);
		columnNames.push_back(ENTITY_STATUS_VALUE_COL);
		columnNames.push_back(STATUS_DATATYPE_COL);
		columnNames.push_back(LAST_MODIFIED_DATE_COL);
		
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery( strSql, columnNames );			
	   	
		if (1 == data->getNumRows())
		{
			// Assign the data as appropriate to the member variables.
			// These calls can throw DataExceptions, as documented in the comment of this method.
			statusdata.lastSessionId = data->getStringData ( 0, LAST_MODIFIED_SESSION_COL );
			statusdata.lastModifyDate = data->getDateData ( 0, LAST_MODIFIED_DATE_COL );
			statusdata.entityStatusDataValue = data->getStringData ( 0, ENTITY_STATUS_VALUE_COL );
			statusdata.entityStatusDataType = (EStatusDataType) (data->getUnsignedLongData( 0, STATUS_DATATYPE_COL ));
			
			// update the saved status data for the given entity
			m_entityStatusList[statusdata.entityStatusType] = statusdata;
			
		}
		else if (0 == data->getNumRows())
		{
			// don't know what to do here, but should not return an exception
			// when an entity status data does not exist for an entity
			// that status data might be required to be inserted into the db
			// however, it is still best to inform the caller of this method
			// that no entity status data was retrieved
			delete data;
            data = NULL;
            //std::ostringstream reasonMessage;
            //reasonMessage << "No entity status data found for pkey = " << m_key;			
            //TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") )
		}				
		else if (1 < data->getNumRows()) // More than one entry found for the pkey
		{
			delete data;
			data = NULL;
			//std::ostringstream reasonMessage;
			//reasonMessage << "More than one entity status data found for entitykey = " << m_key << " and entitystatustype = " << statusdata.entityStatusType;
			//TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
		}
		 
		 		
			// Now that we're done with the IData object, it's our responsibility to delete it
		delete data;
		data = NULL;	 
		
		m_isValidStatusData = true;
	}

	void EntityHelper::writeEntityStatusData(EEntityStatusType statustype, const std::string& newValue)
	 {		 
		 // an entity must be added before it can have an entity status data
		 // therefore must check that m_key != 0
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "writeEntityStatusData");
		if ( 0 != m_key)
		{				
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Write);
			
			if (databaseConnection == NULL)
			{
				//cannot connect to database, throw IncidentException
				std::ostringstream reasonMessage;
				reasonMessage << "Cannot connect to database";                        
				TA_THROW( DataException( reasonMessage.str().c_str(),DataException::MISSING_MANDATORY,"" ) );
			}

			std::string ENTITY_KEY_COL = "ENTITYKEY";	 			
 			std::string ENTITY_STATUS_TYPE_COL = "ENTITYSTATUSTYPE";	 
			
			//std::ostringstream sql;  
			//sql.str("");		 
			//// Create a select statement that will ensure that the status data to be  updated
			//// already exists in the database
			//
			//sql << "select ENTITYKEY, ENTITYSTATUSTYPE";
			//sql << " from ENTITYSTATUSDATA WHERE ENTITYKEY = " << m_key;
			//sql << " and ENTITYSTATUSTYPE = " << statustype;
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ENTITYSTATUSDATA_STD_SELECT_21001, m_key, statustype);
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYSTATUSDATA_SELECT_21001, m_key, statustype);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYSTATUSDATA_SELECT_21001, m_key, statustype);
//			TA_ASSERT(strSql.size() > 0, "EntityHelper::writeEntityStatusData(): Prepare SQL Statement error");
			
			// Set up the vector to pass to exectueQuery
			std::vector<std::string> columnNames;
			columnNames.push_back(ENTITY_KEY_COL);
			columnNames.push_back(ENTITY_STATUS_TYPE_COL);
			
			// execute the query
			IData* existingData = databaseConnection->executeQuery(strSql,columnNames);
			if (existingData == NULL)
			{
				//cannot connect to database, throw IncidentException
				std::ostringstream reasonMessage;
				reasonMessage << "No status data for the given status type";                        
				TA_THROW( DataException( reasonMessage.str().c_str(),DataException::MISSING_MANDATORY,"" ) );		
			}
			else if (0 == existingData->getNumRows())
			{	// do an add
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                        "Status data does not exist and now has a valid value. A new status data will be added.");

				EntityStatusData entityStatusData;
				std::map<EEntityStatusType, EntityStatusData>::iterator itr;
				itr = m_entityStatusList.find(statustype);
				if (itr!=m_entityStatusList.end())
				{
					statustype = itr->first;
					entityStatusData = itr->second;
					std::string currentSession = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);	
					entityStatusData.lastSessionId = currentSession;
					entityStatusData.entityStatusDataValue = newValue;
					addNewEntityEntityStatusData(databaseConnection, entityStatusData);
				}
			}
			else if (1 == existingData->getNumRows())
			{  // do an update	
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Status data already exists. The current value will be updated." );

				EntityStatusData entityStatusData;
				std::map<EEntityStatusType, EntityStatusData>::iterator itr;
				itr = m_entityStatusList.find(statustype);
				if (itr!=m_entityStatusList.end())
				{
					statustype = itr->first;
					entityStatusData = itr->second;
					std::string currentSession = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);						
					entityStatusData.lastSessionId = currentSession;
					entityStatusData.entityStatusDataValue = newValue;
					modifyExistingEntityStatusData(databaseConnection, entityStatusData);
				}
			}
			else
			{
				std::ostringstream reasonMessage;
				reasonMessage << "status data for the given status type is not unique";                        
				TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"" ) );		

			}

			delete existingData;
			existingData = NULL;
		 }		 
	 }

	 void EntityHelper::modifyExistingEntityStatusData(TA_Base_Core::IDatabase* databaseConnection, const EntityStatusData& statusdata)
	 {
		 TA_ASSERT(NULL != databaseConnection, "database connection is null");
	 	// apply changes to database		
		struct tm *t;
		char strDate[22] = {0};
		t = localtime(&statusdata.lastModifyDate);
		sprintf(strDate, "%04d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		
		/*std::ostringstream sql;  
		sql.str("");
		
		sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";	
		sql << "update ENTITYSTATUSDATA set ";
		sql << "LAST_MODIFIED_SESSION = ''" << databaseConnection->escapeQueryString(statusdata.lastSessionId) <<"'', ";
		sql << "LAST_MODIFIED_DATE = TO_DATE(''" << strDate << "'',''YYYYMMDDHH24MISS''), ";				 
		sql << "STATUSDATATYPE = " << statusdata.entityStatusDataType  << ", ";
		sql << "ENTITYSTATUSVALUE = ''"<< databaseConnection->escapeQueryString(statusdata.entityStatusDataValue) << "'' "; 
		sql << "where ENTITYKEY = " << statusdata.entityKey;
		sql << " and ENTITYSTATUSTYPE = " << statusdata.entityStatusType << "'); END;";	*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ENTITYSTATUSDATA_Oracle_UPDATE_21201,getLocalDatabaseName(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYSTATUSDATA_UPDATE_21201,getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYSTATUSDATA_UPDATE_21201,getLocalDatabaseName(),
			databaseConnection->escapeQueryString(statusdata.lastSessionId),strDate, statusdata.entityStatusDataType,
			databaseConnection->escapeQueryString(statusdata.entityStatusDataValue), statusdata.entityKey, statusdata.entityStatusType);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::modifyExistingEntityStatusData(): Prepare SQL Statement error");
		
		
		//LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "the SQL: %s", strSql.c_str());
		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "the SQL: %s", strExSql.c_str());

		
		databaseConnection->executeModification(strSql);  	
		
		// we now have a valid data
		m_isValidStatusData = true;
		
		m_entityStatusList[statusdata.entityStatusType] = statusdata;
		
		
	 }

	 void EntityHelper::addNewEntityEntityStatusData(TA_Base_Core::IDatabase* databaseConnection, const EntityStatusData& statusData)
	 {
		 	// apply changes to database		
		/*std::ostringstream sql;  
		sql.str("");*/		 
			
		struct tm *t;
		char strDate[22] = {0};
		t = localtime(& statusData.lastModifyDate);
		sprintf(strDate, "%04d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			
		/*sql << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";				
		sql << "insert into ENTITYSTATUSDATA (ENTITYSTATUSTYPE, ENTITYKEY, STATUSDATATYPE, ";
		sql << "ENTITYSTATUSVALUE, LAST_MODIFIED_SESSION, LAST_MODIFIED_DATE)";
		sql << "values ("
			<< statusData.entityStatusType << ", "
			<< statusData.entityKey << ", "
			<< statusData.entityStatusDataType << ", "
			<< "''"<< databaseConnection->escapeAQSQLString(statusData.entityStatusDataValue) << "'', " 
			<< "''"<< databaseConnection->escapeAQSQLString(statusData.lastSessionId)  << "'',"
			<< "TO_DATE(''" << strDate << "'',''YYYYMMDDHH24MISS''))'); END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ENTITYSTATUSDATA_Oracle_INSERT_21351, getLocalDatabaseName(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYSTATUSDATA_INSERT_21351, getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYSTATUSDATA_INSERT_21351, getLocalDatabaseName(),
			statusData.entityStatusType, statusData.entityKey, statusData.entityStatusDataType, databaseConnection->escapeAQSQLString(statusData.entityStatusDataValue),
			databaseConnection->escapeAQSQLString(statusData.lastSessionId), strDate);
//		TA_ASSERT(strSql.size() > 0, "EntityHelper::addNewEntityEntityStatusData(): Prepare SQL Statement error");

		//LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "the SQL: %s", strSql.c_str());
		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "the SQL: %s", strExSql.c_str());

		databaseConnection->executeModification(strSql);       
 
		// we now have a valid data
		 m_isValidStatusData = true;
		 
		m_entityStatusList[statusData.entityStatusType ] = statusData;

	 }

	 EStatusDataType EntityHelper::getEntityStatusDataType(EEntityStatusType statustype)
	 {
		 // currently, we only have the MMSInhibitStatus status type
		 // so simply return the datatype for MMSInhibitStatus.
		 // change when additional status types are added
		 return BOOLEAN_DATA;
	 }
	 
	 std::string EntityHelper::getDefaultValueForStatutsType(EStatusDataType datatype)
	 {
		 std::string defaultVal = "";
		 
		 switch(datatype)
		 {
		 case INTEGER_DATA :
		 case BOOLEAN_DATA :
		 case REAL_DATA :
				defaultVal = "0";
				break;
		 }
		 return defaultVal;
	 }


	bool EntityHelper::getBooleanData  ( const std::string & parameterValue )
	{
		if ( ( true == parameterValue.empty() ) || ( "0" == parameterValue ) )
		{
			return false;
		}
		else
		{
			return true;
		}
	}


	int EntityHelper::getIntegerData( const std::string & parameterValue )
	{
		//
		// code borrow from SimpleDbData
		//

		if (parameterValue.size() == 0)
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Empty string to can not be converted to INT");
			throw DataException ( "Empty string to can not be converted to INT", DataException::NO_VALUE, parameterValue );
		}

		std::string::size_type validCharacters = parameterValue.find_first_not_of("0123456789-");
		std::string::size_type dashLocation = parameterValue.find_last_of("-");

		if ((std::string::npos != validCharacters) || !(dashLocation == 0 || dashLocation == std::string::npos))
		{
			TA_THROW(DataException ( "Could not convert from a STRING to an INT: ", DataException::WRONG_TYPE, parameterValue ) );
		}

		static char tempBuffer[256] = {0};

		sprintf(tempBuffer,"%ld",INT_MAX);
		static const std::string maxInt(tempBuffer);
		memset(tempBuffer, 0, 256);
		sprintf(tempBuffer,"%ld",INT_MIN);
		static const std::string minInt(tempBuffer);

		if ((dashLocation == 0 && parameterValue.size() >= minInt.size()) ||
			(dashLocation == -1 && parameterValue.size() >= maxInt.size())) // this could be a problem
		{
			int numberComparison = 0;
			if (parameterValue.size() == maxInt.size())
			{
				numberComparison = parameterValue.compare(maxInt);	// will return a negative number if
															// target < maxInt
			}
			else if (parameterValue.size() == minInt.size() && dashLocation == 0)
			{
				numberComparison = parameterValue.compare(minInt);	// will return a negative number if
															// target > minInt
			}
			else
			{
				numberComparison = 1;
			}

			// final comparison
			if (numberComparison > 0)
			{
				TA_THROW ( DataException ( "Number is too large to be an INT: ", DataException::WRONG_TYPE, parameterValue ) );
			}
		}

		int value = atoi ( parameterValue.c_str() );
		return value;
	}


    int EntityHelper::getIntegerData( const std::string & parameterValue, int defaultValue )
    {
        try
        {
            if (parameterValue.size())
            {
                return getIntegerData(parameterValue);
            }
        }
        catch (...)
        {
        }

        return defaultValue;
    }


	unsigned long EntityHelper::getUnsignedLongData  ( const std::string & parameterValue )
	{
		//
		// code borrow from SimpleDbData
		//

		if (parameterValue.size() == 0)
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Empty string to can not be converted to UNSIGNED LONG");
			throw DataException ( "Empty string to can not be converted to UNSIGNED LONG", DataException::NO_VALUE, parameterValue );
		}

		std::string::size_type validCharacters = parameterValue.find_first_not_of("0123456789");

		if (std::string::npos != validCharacters)
		{
 			TA_THROW ( DataException ( "Could not convert from a STRING to an UNSIGNED LONG - no valid digits: ", DataException::WRONG_TYPE, parameterValue ) );
		}

		static char tempBuffer[256] = {0};
		sprintf(tempBuffer,"%lu",ULONG_MAX);
		static const std::string maxul(tempBuffer);

		if (parameterValue.size() >= maxul.size()) // this could be a problem
		{
			int numberComparison = 0;
			if (parameterValue.size() == maxul.size())
			{
				numberComparison = parameterValue.compare(maxul);	// will return a negative number if
															// target < maxInt
			}
			else
			{
				numberComparison = 1;	// set this to 1, as the number is too big, and doesn't need
										// to be compared
			}

			// final comparison
			if (numberComparison > 0)
			{
				TA_THROW ( DataException ( "Number is too large to be an UNSIGNED LONG: ", DataException::WRONG_TYPE, parameterValue ) );
			}
		}

		unsigned long value = atol ( parameterValue.c_str() );
		return value;
	}


	double EntityHelper::getRealData ( const std::string & parameterValue )
	{
		if (parameterValue.size() == 0) // the data string is empty
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Empty string to can not be converted to REAL");
			throw DataException ( "Empty string to can not be converted to REAL: ", DataException::NO_VALUE, parameterValue );
		}

		std::string::size_type validCharacters = parameterValue.find_first_not_of("0123456789-.");				// locates any invalid characters in this string
		
		if (std::string::npos != validCharacters) // there is an invalid character
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - no valid digits: ", DataException::WRONG_TYPE, parameterValue ) );
		}

		std::string::size_type firstDash = parameterValue.find_first_of("-");									// finds the location of the first "-"
		std::string::size_type otherDash = parameterValue.find_first_of("-",firstDash+1);						// determines if there are any subsequent "-"s
		
		if (firstDash != otherDash && std::string::npos != otherDash) // there is more than one dash
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - more than one dash: ", DataException::WRONG_TYPE, parameterValue ) );
		}
		
		if (std::string::npos != firstDash && 0 != firstDash) // the only dash is not in the first location
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - dash in wrong location: ", DataException::WRONG_TYPE, parameterValue ) );
		}
		
		std::string::size_type trailingDash = parameterValue.find_first_of("0123456789",firstDash+1);			// finds the location of the first character that is a number following the dash - this should be firstDash+1
		
		if (std::string::npos != firstDash && (firstDash+1) != trailingDash) // there is not a numerical character immeidately trailing the dash
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - no digit following dash: ", DataException::WRONG_TYPE, parameterValue ) );
		}

		std::string::size_type firstPoint = parameterValue.find_first_of(".");									// finds the location of the first "."
		std::string::size_type otherPoint = parameterValue.find_first_of(".",firstPoint+1);					// determines if there are any subsequent "."s
		
		if (firstPoint != otherPoint && std::string::npos != otherPoint) // there is more than one point
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - more than one point ", DataException::WRONG_TYPE, parameterValue ) );
		}
		
		std::string::size_type trailingPoint = parameterValue.find_first_of("0123456789",firstPoint+1);	// finds the locatino of the first -numerical character after the first point - this should return firstPoint+1

		if (std::string::npos != firstPoint && (firstPoint+1) != trailingPoint) // there is not a numerical character immeidately trailing the point
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - no digit following point: ", DataException::WRONG_TYPE, parameterValue ) );
		}
		
		if (std::string::npos != firstPoint && ((0 == firstPoint) || (std::string::npos != firstDash && 1 == firstPoint))) // the decimal point is in an ivalid location (either the first position, or the second if there is a dash)
		{
			TA_THROW ( DataException ( "Could not convert from a STRING to a REAL - point in invalid location: ", DataException::WRONG_TYPE, parameterValue ) );
		}

		double value = atof ( parameterValue.c_str() );
		return value;
	}

	unsigned long EntityHelper::getNextPkey( unsigned long locationKey )
	{
        FUNCTION_ENTRY("getNextPkey");

        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		SQLStatement keySql;
		databaseConnection->prepareSQLStatement(keySql, ENTITY_SEQ_PKG_SELECT_4515, locationKey);
        std::vector<std::string> keyColumns;
        keyColumns.push_back("next_entitykey");
		std::auto_ptr<IData> data(databaseConnection->executeQuery(keySql, keyColumns));
        if (data->getNumRows() == 0)
        {
            TA_THROW( DataException("Could not determine the next key", DataException::NO_VALUE ,"") );
        }
        FUNCTION_EXIT;
		return data->getUnsignedLongData(0, "next_entitykey");
	}

} // closes TA_Base_Core
