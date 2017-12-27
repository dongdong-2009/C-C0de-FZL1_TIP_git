/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to retrieve entities for the Scada Tree. This ensures that the SCADA tree only shows
  * boolean data nodes and does not show other select data points.
  */

#pragma warning(disable:4786)

#include <algorithm>

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
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
    SelectAssociationEntityFactory::SelectAssociationEntityFactory(std::vector<unsigned long> dataPointsToIgnore,
                                                                   bool shouldOnlyShowBoolean,
                                                                   bool shouldOnlyShowOutput)
       : m_dataPointsToIgnore(dataPointsToIgnore),
         m_shouldOnlyShowBoolean(shouldOnlyShowBoolean),
         m_shouldOnlyShowOutput(shouldOnlyShowOutput)
    {
    }

    
    SelectAssociationEntityFactory::~SelectAssociationEntityFactory()
    {
    }


    std::vector<TA_Base_Core::IEntityData*> SelectAssociationEntityFactory::getChildEntitiesOf(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getChildEntitiesOf");

        std::vector<TA_Base_Core::IEntityData*> entitiesToReturn;

        try
        {
            std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey,true);

            for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entitiesRetrieved.begin();
                                                              iter != entitiesRetrieved.end();
                                                              ++iter)
            {
                 // First check if the data point is in our list of those to ignore
                 if( std::find(m_dataPointsToIgnore.begin(), m_dataPointsToIgnore.end(), (*iter)->getKey()) != m_dataPointsToIgnore.end() )
                 {
                    // We are ignoring this data point so delete the object and ignore it

                    delete *iter;
                    *iter = NULL;
                    continue;
                 }

                 // We only need to check the type of this node if it is a datapoint
                 if ((*iter)->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
                 {
                     TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(*iter);

                     // If we are only showing boolean data points and this is not a boolean datapoint then ignore it
                     if( m_shouldOnlyShowBoolean )
                     {
                         if( (entity != NULL) &&
                             (entity->getParameterValue( ScadaDetailView::DATAPOINT_TYPE ) != ScadaDetailView::BOOLEAN_TYPE )
                           )
                         {
                             delete *iter;
                             *iter = NULL;
                             continue;
                         }
                     }
                     if( m_shouldOnlyShowOutput )
                     {
                         // Add the output datapoints
                         if( (entity != NULL) &&
                             (entity->getParameterValue( ScadaDetailView::ALLOW_WRITE) != ScadaDetailView::TA_TRUE )
                           )
                         {
                             delete *iter;
                             *iter = NULL;
                             continue;
                         }
                     }
                     else
                     {
                         // Add the input datapoints
                         if( (entity != NULL) &&
                             (entity->getParameterValue( ScadaDetailView::ALLOW_WRITE) == ScadaDetailView::TA_TRUE )
                           )
                         {
                             delete *iter;
                             *iter = NULL;
                             continue;
                         }
                     }

                 }

                 // Ok we want this item so add it to the list of items to return
                 entitiesToReturn.push_back(*iter);
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database error", "Could not load a list of child entities");
        }
       
        FUNCTION_EXIT;
        return entitiesToReturn;
    }


    std::vector<TA_Base_Core::IEntityData*> SelectAssociationEntityFactory::getEntitiesOfType(const std::string& entityTypeName)
    {
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(entityTypeName);
    }
    

    TA_Base_Core::IEntityData* SelectAssociationEntityFactory::getEntity(unsigned long entityKey)
    {
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
    }


    TA_Base_Core::IEntityData* SelectAssociationEntityFactory::getEntity(const std::string& entityName)
    {
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
    }


    bool SelectAssociationEntityFactory::hasChildren(unsigned long entityKey, bool hasChildren)
    {
        return hasChildren;

        // This section below determines the correct value for hasChildren but takes too long to
        // calculate. It has been left here in case necessary.
        /*
        // If we already have a record of this entity key stored we can return the cached value
        std::map<unsigned long, bool>::iterator matching = m_childCache.find(entityKey);
        if (matching != m_childCache.end())
        {
            return matching->second;
        }

        // Retrieve all child entities
        std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey,true);

        bool nowHasChildren = false;
        for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entitiesRetrieved.begin();
                                                          iter != entitiesRetrieved.end();
                                                          ++iter)
        {
             if (nowHasChildren)
             {
                 // Already found a match so don't waste time
                 delete *iter;
                 *iter = NULL;
                 continue;
             }

             if( (*iter)->getType() != TA_Base_Core::DataPointEntityData::getStaticType())
             {
                 // If we find a child that is not a datapoint then return true.
                 nowHasChildren = true;
                 delete *iter;
                 *iter = NULL;
                 continue;
             }

             // Check if the data point is in our list of those to ignore
             if( std::find(m_dataPointsToIgnore.begin(), m_dataPointsToIgnore.end(), (*iter)->getKey()) != m_dataPointsToIgnore.end() )
             {
                 // Ignoring it so carry on to the next item
                 delete *iter;
                 *iter = NULL;
                 continue;
             }

            // We are not ignoring this data point so check if it is shown
             TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(*iter);

             // If we are only showing boolean data points and this is not a boolean datapoint then ignore it
             if( m_shouldOnlyShowBoolean )
             {
                 if( (entity != NULL) &&
                     (entity->getParameterValue( ScadaDetailView::DATAPOINT_TYPE ) == ScadaDetailView::BOOLEAN_TYPE )
                   )
                 {
                     nowHasChildren = true;
                     delete *iter;
                     *iter = NULL;
                     continue;
                 }
             }
             if( m_shouldOnlyShowOutput )
             {
                 // If this is an output datapoints then add it
                 if( (entity != NULL) &&
                     (entity->getParameterValue( ScadaDetailView::ALLOW_WRITE) == ScadaDetailView::TA_TRUE )
                   )
                 {
                     nowHasChildren = true;
                     delete *iter;
                     *iter = NULL;
                     continue;
                 }
             }
             else
             {
                 // Add the input datapoints
                 if( (entity != NULL) &&
                     (entity->getParameterValue( ScadaDetailView::ALLOW_WRITE) == ScadaDetailView::TA_FALSE)
                   )
                 {
                     nowHasChildren = true;
                     delete *iter;
                     *iter = NULL;
                     continue;
                 }
             }

             if (*iter != NULL)
             {
                 delete *iter;
                 *iter = NULL;
             }
        }

        m_childCache.insert( std::map<unsigned long, bool>::value_type(entityKey, nowHasChildren) );

        return nowHasChildren;
        */
    }

}
