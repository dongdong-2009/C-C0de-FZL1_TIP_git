/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentTreeCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a tree control that displays agents (primarily) and some other entities.
  */

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeCtrl.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeItem.h"
#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"

#include "core/data_access_interface/src/ILocation.h"

#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    AgentTreeCtrl::AgentTreeCtrl(AgentDatabaseAccessor& db)
        : m_databaseAccessor(db)
    {
    }


    AgentTreeCtrl::~AgentTreeCtrl()
    {
    }


BEGIN_MESSAGE_MAP(AgentTreeCtrl, CObjectTreeCtrl)
	//{{AFX_MSG_MAP(AgentTreeCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AgentTreeCtrl::populateTree()
    {
        FUNCTION_ENTRY("populateTree");

        std::vector<TA_Base_Core::ILocation*> locations = m_databaseAccessor.getAllLocationObjects();

        for (std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin();
                                                              iter != locations.end();
                                                              ++iter )
        {
            AgentTreeItem* item = new AgentTreeItem(m_databaseAccessor, *iter, true);
            AddRootItem(*item, true);
        }

        FUNCTION_EXIT;
    }


    void AgentTreeCtrl::setInitialSelection(unsigned long identifierOfItemToSelect)
    {
        FUNCTION_ENTRY("setInitialSelection");

        TA_Base_Core::IConfigItem* itemToSelect = m_databaseAccessor.getItem(identifierOfItemToSelect);

        TA_Base_Core::IConfigEntity* entity = NULL;
        ConfigAgent* agent = dynamic_cast<ConfigAgent*>(itemToSelect);

        try
        {
            if (agent == NULL)
            {
                // If we don't have an agent that we are selecting then attempt to find the agent from the entity
                entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(itemToSelect);
                if (entity != NULL)
                {
                    agent = m_databaseAccessor.getAgentFromKey( entity->getParent() );
                }
            }

            if (agent == NULL)
            {
                // We don't have an item we can successfully select
                FUNCTION_EXIT;
                return;
            }

            // First find the location of this item and expand that node
            std::string locationName = agent->getLocationName();
            CObjectTreeItem* rootItem = findSelectedRootItem( locationName );
            if (rootItem == NULL)
            {
                FUNCTION_EXIT;
                return;
            }
            rootItem->expandItem();

            // Now find the item within the root item
            CObjectTreeItem* agentItem = rootItem->findSelectedItem( agent->getName() );

            if( (entity == NULL) && (agentItem != NULL) )
            {
                // It is the agent we want to select so select it
                agentItem->selectItem();
                FUNCTION_EXIT;
                return;
            }

            agentItem->expandItem();

            // Else we want to select the item under the agent so we must find it
            CObjectTreeItem* childItem = agentItem->findSelectedItem( entity->getName() );
            if (childItem != NULL)
            {
                childItem->selectItem();
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not select the item in the tree as an error occurred while trying to retrieve certain details from the database");
        }

        FUNCTION_EXIT;
    }


    int AgentTreeCtrl::countNodes(HTREEITEM item)
    {
        // First expand the node to ensure that all children are loaded and then set it back. This ensures
        // the view does not change but the items are loaded
        Expand(item, TVE_TOGGLE);
        Expand(item, TVE_TOGGLE);

        HTREEITEM child = GetChildItem(item);
        if ( child == NULL )
        {
            // This child has no children so return 0
            return 0;
        }

        // else we must determine how many children we have
        int numberOfNodes = 0;
        while(child != NULL)
        {
            // Add this node to the number of nodes plus the count of all the child nodes
            numberOfNodes = numberOfNodes + 1 + countNodes(child);

            child = GetNextItem(child, TVGN_NEXT);
        }

        return numberOfNodes;
    }
}
