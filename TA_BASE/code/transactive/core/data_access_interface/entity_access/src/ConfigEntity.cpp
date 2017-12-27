/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConfigEntity.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * ConfigEntity is a concrete implementation of IConfigEntity, which is in turn an implementation
  * of IEntityData. It does not provide member variables for parameters, and as such is 
  * generic to all entities. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for entities.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // Disable the "identifier truncated" warning
#endif

#include <sstream>

#include "core/data_access_interface/entity_access/src/ConfigEntity.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/Region.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigEntity::s_nextAvailableIdentifier = 0;

    const std::string ConfigEntity::NO_PARENT = "<None>";
    const std::string ConfigEntity::NO_AGENT = "<None>";
    
    const std::string ConfigEntity::m_alarmDisplayParameter = "AlarmDisplay";

    const std::string ConfigEntity::BOOLEAN_TRUE = "Yes";
    const std::string ConfigEntity::BOOLEAN_FALSE = "No";
    
    const std::string ConfigEntity::INTEGER_DATATYPE = "INTEGER";
    const std::string ConfigEntity::BOOLEAN_DATATYPE = "BOOLEAN";
    const std::string ConfigEntity::STRING_DATATYPE = "STRING";
    const std::string ConfigEntity::ENUMERATION_DATATYPE = "ENUMERATION";
    const std::string ConfigEntity::REAL_DATATYPE = "REAL";

    const std::string ConfigEntity::NAME = "Name";
    const std::string ConfigEntity::ADDRESS = "Address";
    const std::string ConfigEntity::DESCRIPTION = "Description";
    const std::string ConfigEntity::PARENT = "Parent";
    const std::string ConfigEntity::AGENT = "Agent";
    const std::string ConfigEntity::LOCATION = "Location";
	const std::string ConfigEntity::SE_REGION = "Se_Region";
    const std::string ConfigEntity::SUBSYSTEM = "Subsystem";
    const std::string ConfigEntity::PHYSICAL_SUBSYSTEM = "Physical Subsystem";

	const std::string ConfigEntity::MMS_INHIBIT_STATUS = "MMSInhibitStatus";

    ConfigEntity::ConfigEntity(const std::string& type)
        : m_entityHelper(new EntityHelper(type)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_isNewEntity(true)
    {            
    }


    ConfigEntity::ConfigEntity(const unsigned long key, const std::string& type)
        : m_isNewEntity(false),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity(key,type) (constructor)");
        m_entityHelper = new EntityHelper(key,type);
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntityt(key,type) (constructor)");
    }


    ConfigEntity::ConfigEntity( const ConfigEntity& theConfigEntityToCopy)
        : m_parameters(theConfigEntityToCopy.m_parameters),
          m_entityHelper(new EntityHelper(*(theConfigEntityToCopy.m_entityHelper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_isNewEntity(true)
    {
    }


    ConfigEntity::~ConfigEntity()
    {
        if (m_entityHelper != NULL)
        {
            delete m_entityHelper;
            m_entityHelper = NULL;
        }

    }


    void ConfigEntity::updateEntityChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateEntityChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_entityChanges.find(name);

        if (matching != m_entityChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_entityChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_entityChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
    std::vector<std::string> ConfigEntity::getParameterNames()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::getParameterNames");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }
        // set up iterators to step through the map.
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.begin());
        std::map<std::string,ParameterValueType>::iterator iterEnd (m_parameters.end());

        // set up the return vector
        std::vector<std::string> parameterNames;
        while (iter != iterEnd)
        {
            if (iter->second.isVisible)
            {
	            parameterNames.push_back(iter->first);
            }
	        ++iter;
        }
        // if there are no parameters, this vector will be empty (size() == 0)
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::getParameterNames");
        return parameterNames;
    }

    
    std::string ConfigEntity::getParameterValue(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::getParameterValue");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else
        {
            LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::getParameterValue");
            return iter->second.value;
        }
    }


    IConfigEntity::DataType ConfigEntity::getParameterDataType(const std::string& name)
    {
        FUNCTION_ENTRY("getParameterDataType");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else
        {
            FUNCTION_EXIT;
            return iter->second.dataType;
        }
    }


    std::string ConfigEntity::getParameterValidation(const std::string& name)
    {
        FUNCTION_ENTRY("getParameterValidation");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else
        {
            FUNCTION_EXIT;
            return iter->second.validation;
        }
    }
        

    std::map<std::string,std::string> ConfigEntity::getPossibleEnumerationValues(const std::string& name)
    {
        FUNCTION_ENTRY("getParameterValidation");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        if (iter->second.possibleEnumerationValues.empty())
        {
            iter->second.possibleEnumerationValues = loadEnumerationValues(iter->second.validation, iter->second.isMandatory);
        }

        return iter->second.possibleEnumerationValues;
    }


    bool ConfigEntity::isParameterMandatory(const std::string& name)
    {
        FUNCTION_ENTRY("isParameterMandatory");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        FUNCTION_EXIT;
        return iter->second.isMandatory;
    }
        
    bool ConfigEntity::isParameterOnlineUpdatable(const std::string& name)
    {
        FUNCTION_ENTRY("isParameterOnlineUpdatable");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

        // find the entry for the name
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // name not found
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        FUNCTION_EXIT;
        return iter->second.isOnlineUpdatable;
    }

    
    void ConfigEntity::setParameterValue(const std::string& value,const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setParameterValue");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        if (m_parameters.begin() == m_parameters.end()) // Either there *are* no parameters for this entity, or they haven't been loaded
        {
            // load the parameters
            loadParameters(); // if there are no parameters, this will simply do nothing
        }

		// Need to call a get() or set() method on m_entityHelper to force
		// EntityHelper::reload to be called. Otherwise the user will not be able to
		// call applyChanges() after only setting a parameter.
		m_entityHelper->getAddress();

        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

        if (iter == m_parameters.end()) // Name not currently in map - need to add it
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Parameter " << name.c_str() << " not valid for type " << m_entityHelper->getEntityType().c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        updateEntityChanges(name, iter->second.value, value);

        iter->second.value = value;
            
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setParameterValue");
    }


    void ConfigEntity::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::applyChanges");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // First check that all data is valid and all mandatory fields have been entered.
        checkDataIsValid();
        
        // If we get to here we have valid data

        // Write the standard entity data
        m_entityHelper->writeEntityData();

        // Now the parameters, create a map that can be passed to EntityHelper
        std::map<std::string,std::string> parameters;

        // set up iterators to step through the member parameter map.
        std::map<std::string,ParameterValueType>::iterator iter (m_parameters.begin());
        std::map<std::string,ParameterValueType>::iterator iterEnd (m_parameters.end());

        // step through member map
        while (iter != iterEnd)
        {
            // put the parameter name and value in a map that the entity helper can use
    		parameters.insert(std::map<std::string,std::string>::value_type(iter->first,convertToWriteableValue(iter->second) ));
	        ++iter;
        }

        m_entityHelper->writeParameters(parameters);
		// apply changes to any of the entity status data
		applyEntityStatusChange();
        m_isNewEntity = false;
        m_entityChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::applyChanges");
    }


    void ConfigEntity::deleteThisEntity()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::deleteThisEntity");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        m_entityHelper->deleteEntity();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::deleteThisEntity");
    }

    void ConfigEntity::deleteAndResetEntity()
    {
        FUNCTION_ENTRY("deleteAndResetEntity");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        m_entityHelper->deleteEntity();
        m_entityHelper->resetEntityHelper();

        m_isNewEntity = true;
        FUNCTION_EXIT;
    }

    void ConfigEntity::resetEntity()
    {
        FUNCTION_ENTRY("resetEntity");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        m_entityHelper->resetEntityHelper();

        m_isNewEntity = true;
        FUNCTION_EXIT;
    }


    void ConfigEntity::setName(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setName");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        updateEntityChanges(NAME, m_entityHelper->getName(), name);

        m_entityHelper->setName(name);
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setName");
    }


    void ConfigEntity::setAddress(const std::string& address)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setAddress");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        updateEntityChanges(ADDRESS, m_entityHelper->getAddress(), address);

        m_entityHelper->setAddress(address);
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setAddress");
    }


    void ConfigEntity::setDescription(const std::string& description)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setDescription");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        updateEntityChanges(DESCRIPTION, m_entityHelper->getDescription(), description);

        m_entityHelper->setDescription(description);
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setDescription");
    }


    void ConfigEntity::setSubsystem(const unsigned long subsystem)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setSubsystem");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // Retrieve the name of the old subsystem now while we have the key set
        std::string oldSubsystem = getPhysicalSubsystemName();

        m_entityHelper->setSubsystem(subsystem);

        // Now retrieve the name of the new subsystem now that the key has been changed
        std::string newSubsystem = getPhysicalSubsystemName();

        updateEntityChanges(SUBSYSTEM,oldSubsystem, newSubsystem);
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setSubsystem");
    }


    void ConfigEntity::setLocation(const unsigned long location)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setLocation");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // Retrieve the name of the old location now while we have the key set
        std::string oldLocation = getLocationName();

        m_entityHelper->setLocation(location);

        // Now retrieve the name of the new location now that the key has been changed
        std::string newLocation = getLocationName();
        updateEntityChanges(LOCATION, oldLocation, newLocation);

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setLocation");
    }


	void ConfigEntity::setRegion(const unsigned long region)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setRegion");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // Retrieve the name of the old region now while we have the key set
        std::string oldRegion = getRegionName();

        m_entityHelper->setRegion(region);

        // Now retrieve the name of the new region now that the key has been changed
        std::string newRegion = getRegionName();
        updateEntityChanges(SE_REGION, oldRegion, newRegion);

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setRegion");
    }


	void ConfigEntity::setParent(const unsigned long parent)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::setParent");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // Retrieve the name of the old parent now while we have the key set
        std::string oldParent = getParentName();

        m_entityHelper->setParent(parent);

        // Now retrieve the name of the new parent now that the key has been changed
        std::string newParent = getParentName();
        updateEntityChanges(PARENT, oldParent, newParent);

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::setParent");
	}


    void ConfigEntity::setAgent(const unsigned long agent)
    {
		FUNCTION_ENTRY("setAgent");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        // Retrieve the name of the old parent now while we have the key set
        std::string oldAgent = getAgentName();

        m_entityHelper->setAgent(agent);

        // Now retrieve the name of the new parent now that the key has been changed
        std::string newAgent = getAgentName();
        updateEntityChanges(AGENT, oldAgent, newAgent);

        FUNCTION_EXIT;
    }

    void ConfigEntity::loadParameters()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::loadParameters");            
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        // Get the parameters from the entity helper
        IData& parameters = m_entityHelper->getConfigParameters();
        for (unsigned long i = 0;i < parameters.getNumRows();i++) 
        {
            // get the name of the parameter
            std::string name = parameters.getStringData(i,m_entityHelper->PARAMETERNAME);

            // initialise an instance of the ParameterValueType struct
            ParameterValueType pvt;
            pvt.type = parameters.getStringData(i,m_entityHelper->PARAMETERTYPE);
            pvt.validation = parameters.getStringData(i, m_entityHelper->PARAMETERVALIDATION);
            pvt.isMandatory = false;
            if( 0 == parameters.getStringData(i,m_entityHelper->PARAMETERMANDATORY).compare("Y") )
            {
                pvt.isMandatory = true;
            }
            
            pvt.isOnlineUpdatable = false;
            if( 0 == parameters.getStringData(i,m_entityHelper->PARAMETERONLINEUPDATE).compare("Y") )
            {
                pvt.isOnlineUpdatable = true;
            }

            pvt.isVisible = false;
            if ( 0 == parameters.getStringData(i, m_entityHelper->PARAMETERVISIBLE).compare("Y") )
            {
                pvt.isVisible = true;
            }

            std::string type = parameters.getStringData(i,m_entityHelper->PARAMETERDATATYPE);
            if (type == INTEGER_DATATYPE)
            {
                pvt.dataType = INTEGER;
            }
            else if (type == BOOLEAN_DATATYPE)
            {
                pvt.dataType = BOOLEAN;
            }
            else if (type == ENUMERATION_DATATYPE)
            {
                pvt.dataType = ENUMERATION;
            }
            else if (type == REAL_DATATYPE)
            {
                pvt.dataType = REAL;
            }
            else
            {
                pvt.dataType = STRING;
            }

            std::string value = parameters.getStringData(i,m_entityHelper->PARAMETERVALUE);
			std::string value1 = parameters.getStringData(i,m_entityHelper->PARAMETERMANDATORY);
			std::string value2 = parameters.getStringData(i,m_entityHelper->PARAMETERDEFAULTVALUE);
			
			if (!m_isNewEntity)
            {
                // If this is not a new entity then just store the value

                // TD 12235: If a value has not been set, then display the default value
                if(value.empty())
                {
                    value = value2;
                }

                pvt.value = value;
                convertToDisplayableValue(pvt);
            }
            else
            {
                // If this is a new entity then first check to see if we've copied this value
                if (0 != value.compare(""))
                {
                    // We've copied a valid value so just set it
                    pvt.value = value;
                    convertToDisplayableValue(pvt);
                }
                else
                {
                    // If this entity is new then set the default value to be the one stored in the database
                    pvt.value = parameters.getStringData(i, m_entityHelper->PARAMETERDEFAULTVALUE);
                    convertToDisplayableValue(pvt);

                    if (!pvt.value.empty())
                    {
                        // Add this as an entity change
                        updateEntityChanges(name,"",pvt.value);
                    }
                }
            }
            


            // check to see if the parameter name exists already
            std::map<std::string,ParameterValueType>::iterator iter (m_parameters.find(name));

            if (iter == m_parameters.end()) // Not found in map, so we can insert
            {
                m_parameters.insert(std::map<std::string,ParameterValueType>::value_type(name,pvt) );
            }
            else // Oh oh...the parametername was already in the map
            {
                // Throw a not-unique excepiton
                std::ostringstream reasonMessage;
			    reasonMessage << "More than one entry found of parameter " << name.c_str();
                reasonMessage <<" for type " << m_entityHelper->getEntityType().c_str();
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
            }
        } // do it all again

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::loadParameters");
    }


    IEntityData* ConfigEntity::clone(unsigned long key)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigEntity::clone");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigEntity::clone");     
        return new ConfigEntity(key, m_entityHelper->getEntityType());   
    }


    unsigned long ConfigEntity::getKey()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getKey();
    }


    unsigned long ConfigEntity::getTypeKey()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getTypeKey();
    }


    std::string ConfigEntity::getName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getName();
    }


    std::string ConfigEntity::getAddress()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getAddress();
    }


    std::string ConfigEntity::getDescription()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDescription();
    }


    unsigned long ConfigEntity::getSubsystem()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystem(true);
    }


    std::string ConfigEntity::getSubsystemName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        ISubsystem* subsystem = NULL;
        try
        {
		    // Get the entity object
  		    subsystem = SubsystemAccessFactory::getInstance().getSubsystemByKey( m_entityHelper->getSubsystem(true), false );
		    std::string name( subsystem->getName() );

		    // Clean up
		    delete subsystem;

    		// Return the name
            return name;
        }
        catch ( ... )
        {
            if (subsystem != NULL)
            {
                // Clean up if an exception is thrown
                delete subsystem;
            }
            throw;
        }

		return "";
    }


    unsigned long ConfigEntity::getPhysicalSubsystem()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystem(false);
    }


    std::string ConfigEntity::getPhysicalSubsystemName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        ISubsystem* subsystem = NULL;
        try
        {
		    // Get the entity object
  		    subsystem = SubsystemAccessFactory::getInstance().getSubsystemByKey( m_entityHelper->getSubsystem(false), false );
		    std::string name( subsystem->getName() );

		    // Clean up
		    delete subsystem;

    		// Return the name
            return name;
        }
        catch ( ... )
        {
            if (subsystem != NULL)
            {
                // Clean up if an exception is thrown
                delete subsystem;
            }
            throw;
        }

		return "";
    }


    unsigned long ConfigEntity::getLocation()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getLocation();
    }


	std::string ConfigEntity::getLocationName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        
        ILocation* location = NULL;

        try
        {
		    // Get the location object
		    location = LocationAccessFactory::getInstance().getLocationByKey( m_entityHelper->getLocation(), false );
		    std::string name( location->getName() );

		    // Clean up
		    delete location;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            if (location != NULL)
            {
                // Clean up if an exception is thrown
                delete location;
            }
            throw;
        }

		return "";
	}


	unsigned long ConfigEntity::getRegion()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getRegion();
    }


	std::string ConfigEntity::getRegionName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        
        IRegion* region = NULL;

        try
        {
		    // Get the region object
		    region = RegionAccessFactory::getInstance().getRegionByKey( m_entityHelper->getRegion(), false );
		    std::string name( region->getName() );

		    // Clean up
		    delete region;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            if (region != NULL)
            {
                // Clean up if an exception is thrown
                delete region;
            }
            throw;
        }

		return "";
	}


    unsigned long ConfigEntity::getParent()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getParent();
    }


    std::string ConfigEntity::getParentName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        if (getParent() == 0)
        {
            return NO_PARENT;
        }
        if (getParent() == ULONG_MAX)
        {
            // This is a new entity and the parent has not been set yet
            return "";
        }

        IEntityData* entityParent = NULL;
        try
        {

		    // Get the entity object
		    entityParent = EntityAccessFactory::getInstance().getEntity( m_entityHelper->getParent(), false );
		    std::string name( entityParent->getName() );

		    // Clean up
		    delete entityParent;

    		// Return the name
            return name;
        }
        catch ( ... )
        {
            if (entityParent != NULL)
            {
                // Clean up if an exception is thrown
                delete entityParent;
            }
            throw;
        }

		return "";    }


    unsigned long ConfigEntity::getAgent()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getAgent();
    }


	std::string ConfigEntity::getAgentName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        if (getAgent() == 0)
        {
            return NO_AGENT;
        }

        IEntityData* agent = NULL;
        try
        {
		    // Get the entity object
		    agent = EntityAccessFactory::getInstance().getEntity( m_entityHelper->getAgent(), false );
		    std::string name( agent->getName() );

		    // Clean up
		    delete agent;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            if (agent != NULL)
            {
                // Clean up if an exception is thrown
                delete agent;
            }
            throw;
        }
    }


    time_t ConfigEntity::getDateCreated()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDateCreated();
    }


    time_t ConfigEntity::getDateModified()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDateModified();
    }


    std::string ConfigEntity::getAlarmDisplay()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        // Get the parameters
        IData& data = m_entityHelper->getConfigParameters();

        // Set the value to empty string here. If it is not defined for the entity,
        // then this will not be changed.
        std::string alarmDisplay = "";
        // Loop through the defined parameters to determine if AlarmDisplay exists.
        for (unsigned long i = 0;i < data.getNumRows();i++)
        {
            // get the parameter name that was retrieved
            std::string parameterName = data.getStringData(i,m_entityHelper->PARAMETERNAME);

            if (m_alarmDisplayParameter == parameterName)
            {
                alarmDisplay = data.getStringData(i,m_entityHelper->PARAMETERVALUE);
                break;
            }
        }

        return alarmDisplay;
    }


    std::map<unsigned long, std::string> ConfigEntity::getPossibleParentEntities()
    {
        FUNCTION_ENTRY("getPossibleParentEntities");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        
        // Possible parent entities can be determined using the ENTITYTYPEPARENT table
        /*std::ostringstream sql;
        sql << "select pkey, name from entity_v where typekey in ";
        sql << "(select t.pkey from entitytype t, entitytypeparent p ";
        sql << "where t.pkey = p.et_parentkey and p.et_key = ";
        sql << "   (select pkey from entitytype where name = '" << databaseConnection->escapeQueryString(m_entityHelper->getEntityType()) << "')";
		sql << ") and deleted = 0";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2518, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2518, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2518,
			databaseConnection->escapeQueryString(m_entityHelper->getEntityType()) );



        std::string keyColumn = "PKEY";
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        // get a connection to the database
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        std::map<unsigned long, std::string> possibleParents;

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    unsigned long key = data->getUnsignedLongData( i, keyColumn );
                    
                    std::string name(EntityHelper::NO_PARENT);
                    if (key != 0)
                    {
                        name = data->getStringData( i, nameColumn );
                    }

                    possibleParents.insert( std::map<unsigned long, std::string>::value_type(key,name) );
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

        // If we get to here and the map is empty then this means this entity type does not have an
        // entry in the ENTITYTYPEPARENT table. This means the only parent type that can be set is
        // none so we add that in.

        if (possibleParents.empty())
        {
            possibleParents.insert( std::map<unsigned long, std::string>::value_type(0,EntityHelper::NO_PARENT) );
        }

        FUNCTION_EXIT;
		return possibleParents;
    }


    std::map<std::string, std::string> ConfigEntity::loadEnumerationValues(const std::string& validation, const bool isMandatory)
    {
        FUNCTION_ENTRY("loadEnumerationValues");

        std::map<std::string, std::string> values;

        // Conver the statement to all lower case
        std::string lowerCaseStatement = validation;
        std::string charToInsert("?");
        for (unsigned int i = 0; i < lowerCaseStatement.length(); ++i)
        {
            charToInsert[0] = tolower( lowerCaseStatement[i] );
            lowerCaseStatement.replace(i,1,charToInsert);
        }

        // Now determine if we have a select statement or a set of ranges
		if (std::string::npos == validation.find("select"))
        {
            // We don't have a select statement, instead we have a separated list so we must parse it
            values = parseEnumerationValues(validation);
        }
        else
        {
            std::string valueColumn = "VALUE";
            std::string displayColumn = "DISPLAYNAME";
            std::vector<std::string> columnNames;
            columnNames.push_back(valueColumn);
            columnNames.push_back(displayColumn);

            // Get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

            // We are responsible for deleting the returned IData object when we're done with it
            //waitforcheck delete by lin
            //waitforcheck next time
            SQLStatement strSql;
            strSql.strCommon = validation;
            
            IData* data = databaseConnection->executeQuery(strSql,columnNames);

            do
            {
                for (unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    values.insert( std::map<std::string,std::string>::value_type( data->getStringData(i,displayColumn), data->getStringData(i,valueColumn) ) );
                }

                delete data;
                data = NULL;
            } while(databaseConnection->moreData(data));
        }

        
        // If this is not a mandatory parameter then it is possible to select nothing. So add an entry
        // for this
        if (!isMandatory)
        {
            values.insert( std::map<std::string,std::string>::value_type("","") );
        }

        FUNCTION_EXIT;
        return values;
    }


    std::map<std::string, std::string> ConfigEntity::parseEnumerationValues(const std::string& validation)
    {
        FUNCTION_ENTRY("parseEnumerationValue");

        std::string::size_type beginOfFirstPos = validation.find_first_of('[') + 1;
        std::string::size_type endOfFirstPos = validation.find_first_of(']');
        std::string::size_type beginOfSecondPos = validation.find_first_of('[',endOfFirstPos) + 1;
        std::string::size_type endOfSecondPos = validation.find_first_of(']',beginOfSecondPos);

        std::string storedValues = validation.substr(beginOfFirstPos,endOfFirstPos-beginOfFirstPos);
        std::string displayValues = validation.substr(beginOfSecondPos,endOfSecondPos-beginOfSecondPos);

        //
        // Break up the values
        //
        std::vector<std::string> valuesToStore;

        std::string::size_type startingPos = 0;                       // This is the start of each string
        std::string::size_type separatorPos = storedValues.find(';'); // This will be the end of each string

		while ( separatorPos != std::string::npos) // While we haven't processed every ;
        {
            // Pull out the string
            valuesToStore.push_back( storedValues.substr(startingPos, separatorPos - startingPos));

            // Search for the next one
            startingPos = separatorPos + 1;
            separatorPos = storedValues.find(';',startingPos);
        }
        // Get the last string after the last ;
        valuesToStore.push_back( storedValues.substr(startingPos, storedValues.size() - startingPos));


        //
        // Break up the display values
        //
        std::vector<std::string> valuesToDisplay;

        startingPos = 0;                       // This is the start of each string
        separatorPos = displayValues.find(';'); // This will be the end of each string

        while ( separatorPos != -1) // While we haven't processed every ;
        {
            // Pull out the string
            valuesToDisplay.push_back( displayValues.substr(startingPos, separatorPos - startingPos));

            // Search for the next one
            startingPos = separatorPos + 1;
            separatorPos = displayValues.find(';',startingPos);
        }
        // Get the last string after the last ;
        valuesToDisplay.push_back( displayValues.substr(startingPos, displayValues.size() - startingPos));


        std::map<std::string, std::string> processedData;

        if (valuesToStore.size() != valuesToDisplay.size())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "The validation string is not correct. The number of values and display values does not match: %s", validation.c_str());
            return processedData;
        }

        for ( unsigned int i = 0; i < valuesToStore.size(); ++i)
        {
            processedData.insert( std::map<std::string,std::string>::value_type(valuesToDisplay[i],valuesToStore[i]) );
        }

        FUNCTION_EXIT;
        return processedData;
    }



    void ConfigEntity::checkDataIsValid()
    {
        FUNCTION_ENTRY("checkDataIsValid");

        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

        std::string invalidValues("");
        std::string missingValues("");

        if (m_entityHelper->getName() == "")
        {
            missingValues += NAME;
            missingValues += "\n";
        }
        if (m_entityHelper->getDescription() == "")
        {
            missingValues += DESCRIPTION;
            missingValues += "\n";
        }
        if (m_entityHelper->getAddress() == "")
        {
            missingValues += ADDRESS;
            missingValues += "\n";
        }
        if (getParentName() == "")
        {
            missingValues += PARENT;
            missingValues += "\n";
        }

        // We mainly want to check that each parameter is valid and also that all mandatory parameters
        // are set.
		IData& parameters = m_entityHelper->getConfigParameters();

        for(std::map<std::string,ParameterValueType>::iterator iter = m_parameters.begin();
            iter != m_parameters.end(); ++iter
            )
        {
            // First check if the item is set.
            if ( (iter->second.isMandatory) && (iter->second.value.empty()) )
            {
                //To get the default value of the mandatory parameter						
				for (unsigned long i = 0;i < parameters.getNumRows();i++)
				{
					if( 0 == parameters.getStringData(i,m_entityHelper->PARAMETERNAME).compare(iter->first) )
					{
						 iter->second.value = parameters.getStringData(i, m_entityHelper->PARAMETERDEFAULTVALUE);
						 break;
					}
				}				
	
				if(iter->second.value.empty())
				{				
					missingValues += iter->first;
					missingValues += "\n";
					continue;
				}
			}
            else if (iter->second.value.empty())
            {
                // Then it is ok for this item to be empty
                continue;
            }

            // Now check the data is set to be correct
            switch(iter->second.dataType)
            {
                case(INTEGER):
                {
                    std::istringstream integerTester;
                    integerTester.str(iter->second.value);

                    // Attempt to convert the value into an integer
                    int testValue = 0;
                    integerTester >> testValue;

                    // Now convert it back and we should end up with the strings the same
                    std::ostringstream integerTester2;
                    integerTester2 << testValue;

                    // If the strings are different then we have a problem
                    if( 0 != integerTester2.str().compare(integerTester.str()) )
                    {
                        invalidValues += iter->first;
                        invalidValues += "\n";
//TD17763 - marc implementation for parameter range checking
						break; //Do not proceed to the code below if value is invalid in the first place
//TD17763 - marc implementation for parameter range checking

                    }
//TD17763 - marc implementation for parameter range checking
					std::string szValidationString;
						/* Check if there is range checking */
					for (unsigned long i = 0;i < parameters.getNumRows();i++)
					{
						if( 0 == parameters.getStringData(i,m_entityHelper->PARAMETERNAME).compare(iter->first) )
						{

	    	     			szValidationString = parameters.getStringData(i, m_entityHelper->PARAMETERVALIDATION);
							if (szValidationString.length() <= 0)
							{
								//No range specified, continue with normal operation
								break;
							}
							if (checkWithinValidRange(iter->second.value, szValidationString) == false)
							{
							   //If value is not within valid range, consider as an invalid value
	                           invalidValues += iter->first;
	                           invalidValues += "\n";
							}
					
	              		   break;
						}
					}	
//TD17763 - marc implementation for parameter range checking
					break;
                }

                case(BOOLEAN):
                {
                    if( (0 != iter->second.value.compare(BOOLEAN_TRUE)) && (0 != iter->second.value.compare(BOOLEAN_FALSE)) )
                    {
                        invalidValues += iter->first;
                        invalidValues += "\n";
                    }
                    break;
                }

                case(REAL):
                {
                    std::istringstream realTester;
                    realTester.str(iter->second.value);

                    // Attempt to convert the value into a float
                    float testValue = 0;
                    realTester >> testValue;

                    if( (testValue == 0) && (iter->second.value.compare("0") != 0))
                    {
                        invalidValues += iter->first;
                        invalidValues += "\n";
                    }

                    break;
                }

                case(ENUMERATION):
                {
                    // If this parameter is an enumeration we want to check we have the value in our list
                    if (iter->second.possibleEnumerationValues.empty())
                    {
                        iter->second.possibleEnumerationValues = loadEnumerationValues(iter->second.validation,iter->second.isMandatory);
                    }

                    std::map<std::string,std::string>::iterator matching = iter->second.possibleEnumerationValues.find( iter->second.value );
                    if (matching == iter->second.possibleEnumerationValues.end() )
                    {
                        invalidValues += iter->first;
                        invalidValues += "\n";
                    }
                
                    break;
                }
            }
        }

        if(!missingValues.empty())
        {
            TA_THROW( DataException("Mandatory parameters were missing",DataException::MISSING_MANDATORY,missingValues) );
        }

        if (!invalidValues.empty())
        {
            TA_THROW( DataException("Some values were not entered correctly",DataException::WRONG_TYPE,invalidValues) );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "All data was valid");
        FUNCTION_EXIT;
    }

//TD17763 - marc implementation for parameter range checking
	bool ConfigEntity::checkWithinValidRange(const std::string &szValueString, const std::string &szRangeString)
	{
         std::istringstream		integerTester;
		 std::string			firstInteger;
         int					testValue = 0;
		 short	int				i = 0;
		 short   int			curr_state;
		 short int				iFirstOperation;
		 bool					bCondition = true;
		 bool					bIntegerNegative = false;
		 bool					bIntegerFound = false;
		 int					integervalue;
		 const int				STATE_FIRST_INTEGER = 0;
		 const int				STATE_FIRST_OPERATOR = 1;
		 const int				STATE_SECOND_OPERATOR = 2;
		 const int				STATE_SECOND_INTEGER = 3;
		 const int				STATE_VARIABLE = 4;
		 const int			    OPERATOR_LESS = 5;
		 const int				OPERATOR_LESS_EQUAL = 6;
		 const int				OPERATOR_GREAT = 7;
		 const int				OPERATOR_GREAT_EQUAL = 8;


 
         integerTester.str(szValueString);
         // Attempt to convert the value into an integer

         integerTester >> testValue;		
		 // Now parse the range string
		 // The following is the correct format
		 // {integer1}( <,>,<=,>= ) {variable} ( <,>,<=,>= ) {integer 2}
		 // for example, 3<x<5, x > 0
		 // If the range string does not conform to the format, this method would return true
		 curr_state = STATE_FIRST_INTEGER;
		 while (i < szRangeString.length())
		 {
			 if (isspace(szRangeString[i]))
			 {
				i++; 
				continue;
			 }

			 if (curr_state == STATE_FIRST_INTEGER )
			 {
				if (isalpha(szRangeString[i]) && bIntegerNegative == false && bIntegerFound == false)
				{
					curr_state = STATE_VARIABLE;
					continue;
					
				}
				if (szRangeString[i] == '-' )
				{
					if (bIntegerNegative == true)
						break;//Integer sign already encountered
					bIntegerNegative = true;
				}
				else if (isdigit(szRangeString[i]))
				{
					bIntegerFound = true;
					firstInteger += szRangeString[i];
				}
				else if (szRangeString[i] == '<' || szRangeString[i] == '>')
				{
					if (bIntegerFound ==false) 
					{
						break;
					}
					//Get the first integer value
					std::istringstream	integerTester;		

		            integerTester.str(firstInteger);
 
					
                    integerTester >> integervalue;
					if (bIntegerNegative) 
					{
						integervalue *= -1;
					}
					curr_state = STATE_FIRST_OPERATOR;
					continue;
				
				}
				else
				{
					// an invalid character was encountered
					break;
				}
				i++;
			 }
			 else if (curr_state == STATE_FIRST_OPERATOR)
			 {

				if (szRangeString[i] != '<' && szRangeString[i] != '>')
				{
					//We are expecting an comparison operator;
					break;
				}

				// Determine comparison operation to be done
				if (szRangeString[i] == '<')
				{
					iFirstOperation = OPERATOR_GREAT;	
				}
				else
				{
					iFirstOperation =  OPERATOR_LESS;
				}
				curr_state = STATE_FIRST_OPERATOR;  //set the current state to variable lookup
				i++;
				if (szRangeString[i] == '=')
				{
					if (iFirstOperation == OPERATOR_GREAT)
					{
						iFirstOperation =  OPERATOR_GREAT_EQUAL;	
					}
					else
					{
						iFirstOperation = OPERATOR_LESS_EQUAL;
					}
					i++;
				}

				// Compare the value to the boundary 
				switch (iFirstOperation)
				{
				case OPERATOR_GREAT:
					if (testValue <= integervalue)
					{
						bCondition = false;
					}
					break;
				case OPERATOR_GREAT_EQUAL:
					if (testValue < integervalue)
					{
						bCondition = false;
					}
					break;
				case OPERATOR_LESS:
					if (testValue >= integervalue)
					{
						bCondition = false;
					}
					break;
				case OPERATOR_LESS_EQUAL:
					if (testValue > integervalue)
					{
						bCondition = false;
					}
					break;
				default:
					break;
				}

				if (bCondition == false) break;  //There was a false value, then no need to proceed with parsing
				curr_state = STATE_VARIABLE;
			 }
			 else if (curr_state == STATE_VARIABLE)
			 {
				if (isalpha(szRangeString[i])) 
				{
					i++;
				}
				else
				{
					curr_state = STATE_SECOND_OPERATOR;
				    bIntegerNegative = false;
				    bIntegerFound = false;
					firstInteger = "";
				}
			 }
			 else if (curr_state == STATE_SECOND_OPERATOR)
			 {
				if (szRangeString[i] != '<' && szRangeString[i] != '>')
				{
					//We are expecting an comparison operator;
					break;
				}
				 
				 // Determine comparison operation to be done
				if (szRangeString[i] == '<')
				{
					iFirstOperation = OPERATOR_LESS;	
				}
				else
				{
					iFirstOperation =  OPERATOR_GREAT;
				}
				curr_state = STATE_FIRST_OPERATOR;  //set the current state to variable lookup
				i++;
				if (szRangeString[i] == '=')
				{
					if (iFirstOperation == OPERATOR_GREAT)
					{
						iFirstOperation =  OPERATOR_GREAT_EQUAL;	
					}
					else
					{
						iFirstOperation = OPERATOR_LESS_EQUAL;
					}
					i++;
				}
				curr_state = STATE_SECOND_INTEGER;
			 }
			 else if (curr_state == STATE_SECOND_INTEGER)
			 {
				if (szRangeString[i] == '-' )
				{
					if (bIntegerNegative == true)
						break;//Integer sign already encountered
					bIntegerNegative = true;
				}
				else if (isdigit(szRangeString[i]))
				{
					bIntegerFound = true;
					firstInteger += szRangeString[i];
				}
				else 
				{
					break;
				}
				i++;
			 }
		 }

		 if (curr_state == STATE_SECOND_INTEGER && bIntegerFound == true)
		 {
			//Get the first integer value
			std::istringstream	integerTester;		

            integerTester.str(firstInteger);
 

            integerTester >> integervalue;
			if (bIntegerNegative) 
			{
				integervalue *= -1;
			}
			// Compare the value to the boundary 
			switch (iFirstOperation)
			{
			case OPERATOR_GREAT:
				if (testValue <= integervalue)
				{
					bCondition = false;
				}
				break;
			case OPERATOR_GREAT_EQUAL:
				if (testValue < integervalue)
				{
					bCondition = false;
				}
				break;
			case OPERATOR_LESS:
				if (testValue >= integervalue)
				{
					bCondition = false;
				}
				break;
			case OPERATOR_LESS_EQUAL:
				if (testValue > integervalue)
				{
					bCondition = false;
				}
				break;
			default:
				break;
			}
		 }

		 return bCondition;
	}
//TD17763 - marc implementation for parameter range checking

    void ConfigEntity::convertToDisplayableValue(ParameterValueType& parameterDetails)
    {
        FUNCTION_ENTRY("convertToDisplayableValue");
		bool check=false;
		bool check1=false;

        if (0 == parameterDetails.value.compare(""))
        {
            FUNCTION_EXIT;
            return;
        }

        switch ( parameterDetails.dataType)
        {
            case(STRING):
            case(INTEGER):
            case(REAL):
                FUNCTION_EXIT;
                return;

            case(BOOLEAN):
                if (0 == parameterDetails.value.compare("0"))
                {
                    parameterDetails.value = BOOLEAN_FALSE;
                    FUNCTION_EXIT;
                    return;
                }
                parameterDetails.value = BOOLEAN_TRUE; 
                FUNCTION_EXIT;
                return;

            case(ENUMERATION):
               // If this parameter is an enumeration we want to check we have the value in our list
                if (parameterDetails.possibleEnumerationValues.empty())
                {
                    parameterDetails.possibleEnumerationValues = loadEnumerationValues(parameterDetails.validation,parameterDetails.isMandatory);
                }

                // We have to search through the second value in the map in this instance so unfortunately we
                // have to step through ourselves
                for (std::map<std::string,std::string>::iterator iter = parameterDetails.possibleEnumerationValues.begin();
                     iter != parameterDetails.possibleEnumerationValues.end(); ++iter)
                {
                     if (0 == iter->second.compare(parameterDetails.value))
                     {
                         // we have a match
                         parameterDetails.value = iter->first;
						 //////// set a flag stating that selected alarm store's console id is in the iter list 
						 check1=true;
						 break;
                     }
					 else
					 {
						 //////// set a flag stating that selected alarm store's console id is not in the iter list 
						 check = true;
					 }
                }
					 //// Making the value of parameterDetails to null and not returning the ID
					 if((check==true)&&(check1==false))
				parameterDetails.value = "";
        }

        FUNCTION_EXIT;
    }


    std::string ConfigEntity::convertToWriteableValue(ParameterValueType& parameterDetails)
    {
        FUNCTION_ENTRY("convertToWriteableValue");

        switch ( parameterDetails.dataType)
        {
            case(STRING):
            case(INTEGER):
            case(REAL):
                FUNCTION_EXIT;
                return parameterDetails.value;

            case(BOOLEAN):
                if (0 == parameterDetails.value.compare(BOOLEAN_FALSE))
                {
                    FUNCTION_EXIT;
                    return "0";
                }
                else if (0 == parameterDetails.value.compare(BOOLEAN_TRUE))
                {
                    FUNCTION_EXIT;
                    return "1";
                }

            case(ENUMERATION):

                // If this parameter is an enumeration we want to check we have the value in our list
                if (parameterDetails.possibleEnumerationValues.empty())
                {
                    parameterDetails.possibleEnumerationValues = loadEnumerationValues(parameterDetails.validation, parameterDetails.isMandatory);
                }

                std::map<std::string,std::string>::iterator iter = parameterDetails.possibleEnumerationValues.find( parameterDetails.value );

                if (iter != parameterDetails.possibleEnumerationValues.end())
                {
                    FUNCTION_EXIT;
                    return iter->second;
                }
        }

        FUNCTION_EXIT;
        return "";
    }


	void ConfigEntity::assignDefaultData( IData* data, unsigned long row )
	{
		m_entityHelper->assignDefaultData( data, row );
	}


    void ConfigEntity::invalidate()
    {
        m_entityHelper->invalidate();
        m_parameters.clear();
        m_entityChanges.clear();
    }


    std::string ConfigEntity::getType()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getEntityType();
    }
	
    EntityStatusData ConfigEntity::getEntityStatusData(EEntityStatusType statustype)
    {
        FUNCTION_ENTRY("getEntityStatusData");
		TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        FUNCTION_EXIT;
		return m_entityHelper->getEntityStatusData(statustype);
    }

    EntityStatusData ConfigEntity::getEntityStatusData(unsigned int statustype)
    {
        FUNCTION_ENTRY("getEntityStatusData");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        FUNCTION_EXIT;
		return m_entityHelper->getEntityStatusData((EEntityStatusType)statustype);
    }

    bool ConfigEntity::getBoolEntityStatusValue(EEntityStatusType statustype)
    {
        FUNCTION_ENTRY("getBoolEntityStatusValue");
		TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        FUNCTION_EXIT;
		return m_entityHelper->getBoolEntityStatusValue(statustype);
    }

    bool ConfigEntity::getBoolEntityStatusValue(unsigned int statustype)
    {
        FUNCTION_ENTRY("getBoolEntityStatusValue");
       	TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        FUNCTION_EXIT;
		return m_entityHelper->getBoolEntityStatusValue((EEntityStatusType)statustype);
    }

    void ConfigEntity::setEntityStatusData(const EntityStatusData& entityStatus)
    {
        FUNCTION_ENTRY("setEntityStatusData");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

		EntityStatusData oldEntityStatusData = getEntityStatusData(entityStatus.entityStatusType);
		
		std::string oldEntityStatusValue = oldEntityStatusData.entityStatusDataValue;
		std::string newEntityStatusValue = entityStatus.entityStatusDataValue;

		
		if (entityStatus.entityStatusType == MMSInhibitStatus)
		{
			updateEntityChanges(MMS_INHIBIT_STATUS, oldEntityStatusValue, newEntityStatusValue);
        }

		m_entityHelper->setEntityStatusData(entityStatus);        
        FUNCTION_EXIT;
    }

    void ConfigEntity::setEntityStatusData(unsigned int statustype, EStatusDataType datatype, const std::string& statusvalue, time_t dateModified )
    {
        FUNCTION_ENTRY("setEntityStatusData");
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");

		EntityStatusData oldEntityStatusData = getEntityStatusData(statustype);
		std::string oldEntityStatusValue = oldEntityStatusData.entityStatusDataValue;
		if (statustype == MMSInhibitStatus)
		{
			updateEntityChanges(MMS_INHIBIT_STATUS, oldEntityStatusValue, statusvalue);
        }

		m_entityHelper->setEntityStatusData(statustype, datatype, statusvalue, dateModified);
        FUNCTION_EXIT;
    }

	void ConfigEntity::applyEntityStatusChange()
	{
		FUNCTION_ENTRY("applyEntityStatusChange");

		ItemChanges::iterator entityStatusChange = m_entityChanges.find(MMS_INHIBIT_STATUS);
        if (entityStatusChange != m_entityChanges.end())
		{
			// write any changes to the entity status data
			
			m_entityHelper->writeEntityStatusData(MMSInhibitStatus, entityStatusChange->second.newValue);

		}

		FUNCTION_EXIT;
	}

	void ConfigEntity::justApplyEntityStatusChange()
	{
		FUNCTION_ENTRY("justApplyEntityStatusChange");

		applyEntityStatusChange();

        m_isNewEntity = false;
        m_entityChanges.clear();

		FUNCTION_EXIT;
	}

} // closes TA_Base_Core
