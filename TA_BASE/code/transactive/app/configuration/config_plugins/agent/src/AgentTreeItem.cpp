
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This is an item to be added to the agent tree
  */

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeItem.h"
#include "app/configuration/config_plugins/agent/src/AgentComponent.h"
#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"

#include "bus/resources/resource.h"

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/ILocation.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
    AgentTreeItem::AgentTreeItem(AgentDatabaseAccessor& db,
                                 TA_Base_Core::IItem* item,
                                 bool shouldDelete) 
         : m_databaseAccessor(db),
           m_data(item),
           m_shouldDeleteItem(shouldDelete),
           m_icon(NULL),
           CObjectTreeItem()
    {
    }


    AgentTreeItem::~AgentTreeItem()
    {
        if (m_shouldDeleteItem && m_data != NULL)
        {
            delete m_data;
            m_data = NULL;
        }
    }


    bool AgentTreeItem::hasChildren()
    {
        if( dynamic_cast<TA_Base_Core::ILocation*>(m_data) != NULL ) 
        {
            // This is a location so check if it has children
            return m_databaseAccessor.locationHasChildren( m_data->getKey() );
        }

        ConfigAgent* agent = dynamic_cast<ConfigAgent*>(m_data);
        if( agent != NULL )
        {
            // This is an agent so check if it has children
            return m_databaseAccessor.agentHasChildren( agent->getUniqueIdentifier() );
        }

        // Anything else doesn't have children
        return false;
    }


    bool AgentTreeItem::createChildItems()
    {
        std::vector<TA_Base_Core::IConfigItem*> childItems;
        try
        {
            if( dynamic_cast<TA_Base_Core::ILocation*>(m_data) != NULL ) 
            {
                childItems = m_databaseAccessor.getItemsAtLocation( m_data->getKey() );
            }
            else
            {
                ConfigAgent* agent = dynamic_cast<ConfigAgent*>(m_data);
                if (agent != NULL)
                {
                    childItems = m_databaseAccessor.getChildItemsOfAgent( agent->getUniqueIdentifier() );
                }
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            //TODO: Error message?
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve child items for the tree");
            return false;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve child items for the tree");
            return false;
        }
            
        std::vector<TA_Base_Core::IConfigItem*>::iterator iter;
	    for (iter = childItems.begin(); iter != childItems.end(); iter++)
	    {
            AgentTreeItem* newItem = new AgentTreeItem(m_databaseAccessor, dynamic_cast<TA_Base_Core::IItem*>(*iter), false);
		    AddChildItem(*newItem, true, true);
	    }

        SortChildren();

	    return true;
    }


    const char* AgentTreeItem::getItemLabelText()
    {
        m_itemLabelText = m_data->getName();
        return m_itemLabelText.c_str();
    }


    const std::string AgentTreeItem::getKey()
    {
	    return m_data->getName().c_str();
    }


    HICON AgentTreeItem::getItemImage(bool itemExpanded)
    {
        if (m_icon == NULL)
        {
            if( dynamic_cast<TA_Base_Core::ILocation*>(m_data) != NULL ) 
            {
                // A Location
                m_icon = LoadIcon(GetModuleHandle(AgentComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_LOCATION) );
            }
            else if ( dynamic_cast<ConfigAgent*>(m_data) != NULL )
            {
                // An agent
                m_icon = LoadIcon(GetModuleHandle(AgentComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_AGENT) );
            }
            else
            {
                // Some other entity
                m_icon = LoadIcon(GetModuleHandle(AgentComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ENTITY) );
            }
        }

        return m_icon;
    }
};
