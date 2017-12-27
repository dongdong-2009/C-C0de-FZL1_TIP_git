/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasDatabaseAccessor.cpp $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ScadaAlias information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_alias/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasDatabaseAccessor.h"
#include "app/scada/OpcBridgeLib/attributes/DpAttributeNames.h"
#include "app/scada/OpcBridgeLib/attributes/DnAttributeNames.h"

#include "core/data_access_interface/opctag_alias/src/ConfigOpcTagAlias.h"
#include "core/data_access_interface/opctag_alias/src/OpcTagAliasAccessFactory.h"
#include "core/data_access_interface/opctag_alias/src/IOpcTagAlias.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ScadaAliasDatabaseAccessor::ScadaAliasDatabaseAccessor()
        : m_isLoaded(false)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    ScadaAliasDatabaseAccessor::~ScadaAliasDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAliases::iterator iter = m_scadaAliases.begin(); iter != m_scadaAliases.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void ScadaAliasDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the scada aliases and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAliases::iterator scadaAliasesToDelete = m_scadaAliases.begin(); scadaAliasesToDelete != m_scadaAliases.end(); ++scadaAliasesToDelete)
        {
            if( (scadaAliasesToDelete->second) != NULL )
            {
                delete scadaAliasesToDelete->second;
                scadaAliasesToDelete->second = NULL;
            }
        }
        m_scadaAliases.clear();


        // Now reload all the scada aliases
        std::vector<TA_Base_Core::IOpcTagAlias*> aliases = TA_Base_Core::OpcTagAliasAccessFactory::getInstance().getAllOpcTagAliases(true);

        // And insert the scada aliases into our map. At the same time cast them to IConfigOpcTagAlias*
        // instead of IOpcTagAlias*.
        for (std::vector<TA_Base_Core::IOpcTagAlias*>::iterator iter = aliases.begin();
             iter != aliases.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigOpcTagAlias* castAlias = dynamic_cast<TA_Base_Core::IConfigOpcTagAlias*>(*iter);
             if( castAlias != NULL )
             {
                m_scadaAliases.insert( LoadedAliases::value_type(castAlias->getUniqueIdentifier(), castAlias) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }

    ScadaAliasDatabaseAccessor::ScadaAliasRecord ScadaAliasDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        LoadedAliases::iterator matching = m_scadaAliases.find(identifier);
        if (matching == m_scadaAliases.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the scada alias with identifier %lu",identifier);
            FUNCTION_EXIT;
            ScadaAliasRecord record;
            return record;
        }

        ScadaAliasRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }

    std::map<unsigned long,ScadaAliasDatabaseAccessor::ScadaAliasRecord> ScadaAliasDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,ScadaAliasRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_scadaAliases.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (LoadedAliases::iterator it = m_scadaAliases.begin(); it != m_scadaAliases.end(); ++it)
        {
            if (it->second != NULL)
            {
                ScadaAliasRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, ScadaAliasRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }

    ScadaAliasDatabaseAccessor::ScadaAliasRecord ScadaAliasDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigOpcTagAlias& item)
    {
        ScadaAliasRecord record;
        record.opc_alias = CString(item.getOpcTagAlias().c_str());
        record.entity_name = CString(item.getEntityName().c_str());
        record.attribute = CString(item.getAttributeName().c_str());
        return record;
    }



    std::multimap<CString,unsigned long> ScadaAliasDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of scada aliases times by 2. This is because we want to step for each
        // ScadaAlias while it is loaded and then again while each ScadaAlias is added to the list.
        int currentPos = progress.GetPos();
        progress.SetRange( 0, (m_scadaAliases.size() * 2) + currentPos );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAliases::iterator iter = m_scadaAliases.begin();
             iter != m_scadaAliases.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                names.insert( std::multimap<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* ScadaAliasDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ScadaAlias matching the key passed in.
        LoadedAliases::iterator matching = m_scadaAliases.find(key);
        if (matching == m_scadaAliases.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ScadaAlias with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ScadaAliasDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ScadaAlias matching the key passed in.
        LoadedAliases::iterator matching = m_scadaAliases.find(key);
        if (matching == m_scadaAliases.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ScadaAlias with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ScadaAlias was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisOpcTagAlias();
        }
        delete matching->second;
        matching->second = NULL;
        m_scadaAliases.erase(matching);

        FUNCTION_EXIT;
    }

    std::map<unsigned long, std::string> ScadaAliasDatabaseAccessor::getAllDnAttributeNames()
    {
		FUNCTION_ENTRY("getAllDnAttributeNames()");

		m_attributeNameMap.clear();
        unsigned long cnt = 0;
        m_attributeNameMap.insert(std::pair<unsigned long, std::string>(cnt++, DN_ATTR_NAME_SHORT_LABEL) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_LABEL) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_QUALITY) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_ALARM_STATE ));
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_ACK_STATE ));
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_ASSET_NAME) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DN_ATTR_NAME_ONITP) );

		FUNCTION_EXIT;
		return m_attributeNameMap;
    }
        
    std::map<unsigned long, std::string> ScadaAliasDatabaseAccessor::getAllDpAttributeNames()
    {
		FUNCTION_ENTRY("getAllDpAttributeNames()");

		std::vector<std::string> attributeNames;
        unsigned long cnt = 0;

		m_attributeNameMap.clear();

		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DPT_ATTR_NAME_VALUE) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DPT_ATTR_NAME_ALARM_STATE ));
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DPT_ATTR_NAME_QUALITY) );
		m_attributeNameMap.insert( std::pair<unsigned long, std::string>(cnt++,DPT_ATTR_NAME_ACK_STATE) );

		FUNCTION_EXIT;
		return m_attributeNameMap;
    }
        
    TA_Base_Core::IConfigItem* ScadaAliasDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");
        TA_Base_Core::IConfigItem* newAlias = TA_Base_Core::OpcTagAliasAccessFactory::getInstance().createOpcTagAlias();
        m_scadaAliases.insert( LoadedAliases::value_type( newAlias->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IConfigOpcTagAlias*>(newAlias) ) );

        return newAlias;

        FUNCTION_EXIT;
    }

    TA_Base_Core::IConfigItem* ScadaAliasDatabaseAccessor::newItem(const std::string& aliasname, const std::string entityname)
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IEntityData* data;
        unsigned long entitykey;
        try
        {
            data = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityname);
            entitykey = data->getKey();
        }
        catch (TA_Base_Core::DataException) 
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "can not find the particular entity = %s",entityname);
            delete data;
            return NULL;
        }
        catch (...) 
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "can not find the particular entity = %s",entityname);
            delete data;
            return NULL;
        }
        delete data;

        TA_Base_Core::IConfigItem* newAlias = TA_Base_Core::OpcTagAliasAccessFactory::getInstance().createOpcTagAlias(entitykey, aliasname);
        m_scadaAliases.insert( LoadedAliases::value_type( newAlias->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IConfigOpcTagAlias*>(newAlias) ) );

        return newAlias;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ScadaAliasDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigOpcTagAlias* newAlias = NULL;

        // Attempt to find the entity matching the key passed in.
        LoadedAliases::iterator matching = m_scadaAliases.find(idOfItemToCopy);

        TA_ASSERT(matching != m_scadaAliases.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigOpcTagAlias stored in the map");

        newAlias = TA_Base_Core::OpcTagAliasAccessFactory::getInstance().copyOpcTagAlias(matching->second);

        if (newAlias != NULL)
        {
            m_scadaAliases.insert( LoadedAliases::value_type( newAlias->getUniqueIdentifier(), newAlias ) );
        }
        
        return newAlias;

        FUNCTION_EXIT;
    }


    bool ScadaAliasDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& aliasesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(aliasesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAliases::iterator iter = m_scadaAliases.begin(); iter != m_scadaAliases.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    aliasesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this ScadaAlias that has not had it's changes applied yet");
                    aliasesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this ScadaAlias that has not had it's changes applied yet");
                    aliasesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !aliasesNotApplied.empty();
    }
}
