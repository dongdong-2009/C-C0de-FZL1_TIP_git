/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/ConfigOpcTagAlias.cpp $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigOpcTagAlias is an implementation of IConfigOpcTagAlias. It holds the data specific to a OpcTagAlias entry
  * in the database, and allows read-write access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/opctag_alias/src/ConfigOpcTagAlias.h"
#include "core/data_access_interface/opctag_alias/src/OpcTagAliasHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include<ostream>

namespace TA_Base_Core
{
    long ConfigOpcTagAlias::s_nextAvailableIdentifier = 0;
    const std::string ConfigOpcTagAlias::OPC_ALIAS = "ALIAS";
    const std::string ConfigOpcTagAlias::ENTITY_KEY = "ENTITY_ID_DP";
    const std::string ConfigOpcTagAlias::ATTRIBUTE = "ATTRIBUTE_DP";

    ConfigOpcTagAlias::ConfigOpcTagAlias()
        : m_opcTagAliasHelper( new OpcTagAliasHelper() ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("default constructor");
        FUNCTION_EXIT;
    }

    ConfigOpcTagAlias::ConfigOpcTagAlias( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string& attribute)
        : m_opcTagAliasHelper( new OpcTagAliasHelper(key, entitykey, name, attribute) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("constructor with all the fields");
        FUNCTION_EXIT;
    }

    ConfigOpcTagAlias::ConfigOpcTagAlias( const unsigned long entitykey, const std::string& name )
		: m_opcTagAliasHelper( new OpcTagAliasHelper(entitykey,name) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("constructor with entitykey and alias name");
        FUNCTION_EXIT;
    }


	ConfigOpcTagAlias::ConfigOpcTagAlias( const ConfigOpcTagAlias& theOpcTagAlias)
        : m_opcTagAliasHelper( new OpcTagAliasHelper(*(theOpcTagAlias.m_opcTagAliasHelper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
        FUNCTION_ENTRY("copy constructor");
        FUNCTION_EXIT;
	}


    ConfigOpcTagAlias::~ConfigOpcTagAlias()
    {
        FUNCTION_ENTRY("destructor");
        if (m_opcTagAliasHelper != NULL)
        {
            delete m_opcTagAliasHelper;
            m_opcTagAliasHelper = NULL;
        }
        FUNCTION_EXIT;
    }


    bool ConfigOpcTagAlias::isNew()
    {
        FUNCTION_ENTRY("isNew");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->isNew();
    }


    ItemChanges ConfigOpcTagAlias::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");
        FUNCTION_EXIT;
        return m_itemChanges;
    }

        
    unsigned long ConfigOpcTagAlias::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getKey();
    }


    std::string ConfigOpcTagAlias::getName()
    {
        FUNCTION_ENTRY("getName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getName();
    }
    
    std::string ConfigOpcTagAlias::getOpcTagAlias()
    {
        FUNCTION_ENTRY("getOpcAlias");
        FUNCTION_EXIT;
        return (getName());
    }
    
    std::string ConfigOpcTagAlias::getAttributeName()
    {
        FUNCTION_ENTRY("getAttribute");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getAttributeName();
    }

    std::string ConfigOpcTagAlias::getEntityName()
    {
        FUNCTION_ENTRY("getDatapointName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        unsigned long toReturn = m_opcTagAliasHelper->getEntitykey();
        std::string entityname;
        IEntityData* data = NULL;
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(toReturn);
            entityname = data->getName();
        }
        catch (DataException) 
        {
            entityname = "";
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "Can not find the entity name for the entitykey = %d", toReturn);
        }
        catch (...) 
        {
            entityname = "";
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "Can not find the entity name for the entitykey = %d", toReturn);
        }
        if (data != NULL )
        {
            delete data;
        }
        FUNCTION_EXIT;
        return entityname;
    }

    unsigned long ConfigOpcTagAlias::getEntityKey()
    {
        FUNCTION_ENTRY("getDatapointKey");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getEntitykey();
    }
        
    time_t ConfigOpcTagAlias::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getDateCreated();
    }


    time_t ConfigOpcTagAlias::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getDateModified();
    }

    void ConfigOpcTagAlias::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        updateChanges(OPC_ALIAS,m_opcTagAliasHelper->getName(), name);
        m_opcTagAliasHelper->setName(name);
        FUNCTION_EXIT;
    }

    void ConfigOpcTagAlias::setOpcTagAlias(std::string& aliasname)
    {
        FUNCTION_ENTRY("setOpcTagAlias");
        //the name is the alias name
        setName(aliasname);
        FUNCTION_EXIT;
    }

    void ConfigOpcTagAlias::setAttributeName(std::string& attribute)
    {
        FUNCTION_ENTRY("setAttributeName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        updateChanges(ATTRIBUTE,m_opcTagAliasHelper->getAttributeName(), attribute);
        m_opcTagAliasHelper->setAttributeName(attribute);
        FUNCTION_EXIT;
    }

    void ConfigOpcTagAlias::setEntityName(std::string& entityname)
    {
        FUNCTION_ENTRY("setEntityName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        IEntityData* data = NULL;
        unsigned long entitykey = 0;
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(entityname);
            entitykey = data->getKey();
        }
        catch (DataException) 
        {
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "DataException caught. Can not find the entity name for the entitykey = %s", entityname.c_str());
            delete data;
            return;
        }
        catch (...) 
        {
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "Unknown exception caught. Can not find the entity name for the entitykey = %s", entityname.c_str());
            delete data;
            return;
        }
        delete data;

        setEntityKey(entitykey);
        FUNCTION_EXIT;
    }

    void ConfigOpcTagAlias::setEntityKey(unsigned long entitykey)
    {
        FUNCTION_ENTRY("setEntityKey");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        std::ostringstream oldstr;
        oldstr << m_opcTagAliasHelper->getEntitykey();

        std::ostringstream newstr;
        newstr << entitykey;

        updateChanges(ENTITY_KEY,oldstr.str(), newstr.str());
        m_opcTagAliasHelper->setEntitykey(entitykey);
        FUNCTION_EXIT;
    }

    void ConfigOpcTagAlias::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");

        m_opcTagAliasHelper->invalidate();
        m_itemChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigOpcTagAlias::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");

        m_opcTagAliasHelper->applyChanges();
        m_itemChanges.clear();

        FUNCTION_EXIT;
    }

    
    void ConfigOpcTagAlias::deleteThisOpcTagAlias()
    {
        FUNCTION_ENTRY("deleteThisOpcTagAlias");

        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");

        m_opcTagAliasHelper->deleteThisOpcTagAlias();

        FUNCTION_EXIT;
    }


    void ConfigOpcTagAlias::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_itemChanges.find(name);

        if (matching != m_itemChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_itemChanges.erase(matching);
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
            m_itemChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


} // closes TA_Base_Core
