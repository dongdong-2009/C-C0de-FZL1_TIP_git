/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickFindTreeObject.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// QuickFindTreeObject.cpp: implementation of the QuickFindTreeObject class.
//
//////////////////////////////////////////////////////////////////////

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/ControlStation.h"
#include "app/system_control/control_station/src/QuickFindTreeObject.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QuickFindTreeObject::QuickFindTreeObject(TA_Base_Core::IItem& item,std::string& name,HICON icon,ObjectTypeInfo typeInfo) : 
m_itemData(&item), m_hasChildren(true), m_name(name), m_icon(icon), m_typeInfo(typeInfo)
{
    if (m_typeInfo.type == ENTITY)
    {
        // If this is an entity item, then there will be no further children, so set the flag now.
        m_hasChildren = false;
    }
    else if (m_typeInfo.type == REGION)
    {
        // If this in a region item, set the region key
        m_typeInfo.regionKey = m_itemData->getKey();
    }
}

QuickFindTreeObject::~QuickFindTreeObject()
{
    if (m_itemData != NULL)
    {
        delete m_itemData;
        m_itemData = NULL;
    }
}

bool QuickFindTreeObject::createChildItems()
{
    FUNCTION_ENTRY("createChildItems");

    switch (m_typeInfo.type)
    {
    case ENTITY:
        TA_ASSERT(false,"An entity item attempted to create child items. This should not occur.");
        break;
    case SUBSYSTEM:
        return createChildItemsOfSubsystem();
    case REGION:
        return createChildItemsOfRegion();
    default:
        TA_ASSERT(false,"An unknown tree object type ID was set. This should not be possible");
        break;
    }

    FUNCTION_EXIT;
    return m_hasChildren;
}

bool QuickFindTreeObject::createChildItemsOfRegion()
{
    // Child items of region objects are subsystems, so use the subsystem access factory.
    // NOTE: 4619 has not defined a "region" as yet, so using location until that is done
    std::vector<TA_Base_Core::ISubsystem*> subsystems;
    try
    {
        subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getSubsystemsAtRegion(m_itemData->getKey());
    }
    catch(...)
    {
            std::string message("An exception was caught while attempting to retrieve the subsystems in region "
                + m_name);
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",message.c_str());
            
            // If the children can't be retrieved, then this item has no children
            m_hasChildren = false;
            return false;
    }

    // If there was at least one subsystem for this region, then need to flag that there are children
    if (subsystems.size() > 0)
    {
        m_hasChildren = true;
    }
    else
    {
        m_hasChildren = false;
    }

    // Loop through the vector and create a child item for each subsystem
    for (int i = 0;i < subsystems.size();i++)
    {
        std::string subsystemName;
        try
        {
            subsystemName = subsystems[i]->getName();
        }
        catch(...)
        {
            std::string message("An exception was caught while attempting to retrieve the name of a subsystem in region "
                + m_name);
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",message.c_str());
            
            // If the name can't be retrieved, don't add the child
            continue;
        }

        // Create the child item and add it to the tree
        ObjectTypeInfo typeInfo = m_typeInfo;
        typeInfo.type = SUBSYSTEM;
        QuickFindTreeObject* childItem = new QuickFindTreeObject(*subsystems[i],subsystemName,NULL,typeInfo);
        AddChildItem(*childItem,false,true); // false = don't show it expanded already, true = have the tree delete the item
    }

    return m_hasChildren;
}

bool QuickFindTreeObject::createChildItemsOfSubsystem()
{
    std::vector<TA_Base_Core::IEntityData*> entities;
    try
    {
        entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfSubsystemAtLocation(m_itemData->getKey(),m_typeInfo.regionKey);
    }
    catch(...)
    {
            std::string message("An exception was caught while attempting to retrieve the entities of subsystem "
                + m_name);
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",message.c_str());
            
            // If the children can't be retrieved, then this item has no children
            m_hasChildren = false;
            return false;
    }

    // If there was at least one entity for this subsystem, then need to flag that there are children
    if (entities.size() > 0)
    {
        m_hasChildren = true;
    }
    else
    {
        m_hasChildren = false;
    }

    // Loop through the vector and create a child item for each subsystem
    for (int i = 0;i < entities.size();i++)
    {
        std::string entityName;
        try
        {
            entityName = entities[i]->getName();
        }
        catch(...)
        {
            std::string message("An exception was caught while attempting to retrieve the name of an entity in subsystem "
                + m_name);
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",message.c_str());
            
            // If the name can't be retrieved, don't add the child
            continue;
        }

        // Create the child item and add it to the tree
        ObjectTypeInfo typeInfo = m_typeInfo;
        typeInfo.type = ENTITY;
        QuickFindTreeObject* childItem = new QuickFindTreeObject(*entities[i],entityName,NULL,typeInfo);
        AddChildItem(*childItem,false,true); // false = don't show it expanded already, true = have the tree delete the item
    }
    return m_hasChildren;
}

const char* QuickFindTreeObject::getItemLabelText()
{
    return m_name.c_str();
}

bool QuickFindTreeObject::hasChildren()
{
    return m_hasChildren;
}

const std::string QuickFindTreeObject::getKey()
{
	return m_name;
}

HICON QuickFindTreeObject::getItemImage(bool itemExpanded)
{
    return m_icon;
}
