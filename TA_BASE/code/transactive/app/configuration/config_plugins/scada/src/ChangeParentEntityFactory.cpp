/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ChangeParentEntityFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to retrieve entities for the Scada Tree. This ensures that the SCADA tree only shows
  * data nodes and does not show any data points.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ChangeParentEntityFactory.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ChangeParentEntityFactory::ChangeParentEntityFactory()
    {
    }

    
    ChangeParentEntityFactory::~ChangeParentEntityFactory()
    {
    }


    std::vector<TA_Base_Core::IEntityData*> ChangeParentEntityFactory::getChildEntitiesOf(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getChildEntitiesOf");

        std::vector<TA_Base_Core::IEntityData*> entitiesToReturn;

        try
        {
            std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey);

            for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entitiesRetrieved.begin();
                                                              iter != entitiesRetrieved.end();
                                                              ++iter)
            {
                 // Do not add any types that are data points
                 if ((*iter)->getType() != TA_Base_Core::DataPointEntityData::getStaticType())
                 {
                     entitiesToReturn.push_back(*iter);
                 }
                 else
                 {
                     delete *iter;
                     *iter = NULL;
                 }
            }
        }
        catch ( ... )
        {
            // Nothing to do. We just return the empty vector
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve a list of child entities.");
        }
       
        FUNCTION_EXIT;
        return entitiesToReturn;
    }


    std::vector<TA_Base_Core::IEntityData*> ChangeParentEntityFactory::getEntitiesOfType(const std::string& entityTypeName)
    {
        FUNCTION_ENTRY("getEntitiesOfType");

        std::vector<TA_Base_Core::IEntityData*> entitiesToReturn;

        if (entityTypeName == TA_Base_Core::DataPointEntityData::getStaticType())
        {
            // Return nothing
            FUNCTION_EXIT;
            return entitiesToReturn;
        }

        try
        {
            entitiesToReturn = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(entityTypeName);
        }
        catch ( ... )
        {
            //Nothing to do. We just return the empty vector
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve a list of entities.");
        }

        FUNCTION_EXIT;
        return entitiesToReturn;
    }
    

    TA_Base_Core::IEntityData* ChangeParentEntityFactory::getEntity(unsigned long entityKey)
    {
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
    }


    TA_Base_Core::IEntityData* ChangeParentEntityFactory::getEntity(const std::string& entityName)
    {
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
    }


    bool ChangeParentEntityFactory::hasChildren(unsigned long entityKey, bool hasChildren)
    {
        // If we already have a record of this entity key stored we can return the cached value
        std::map<unsigned long, bool>::iterator matching = m_childCache.find(entityKey);
        if (matching != m_childCache.end())
        {
            return matching->second;
        }

        // Retrieve all child entities
        std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey);

        bool nowHasChildren = false;
        for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entitiesRetrieved.begin();
                                                          iter != entitiesRetrieved.end();
                                                          ++iter)
        {
             if( !nowHasChildren && (*iter)->getType() != TA_Base_Core::DataPointEntityData::getStaticType())
             {
                 // If we find a child that is not a datapoint then return true.
                 nowHasChildren = true;
             }

             delete *iter;
             *iter = NULL;
        }

        m_childCache.insert( std::map<unsigned long, bool>::value_type(entityKey, nowHasChildren) );

        return nowHasChildren;
    }

}
