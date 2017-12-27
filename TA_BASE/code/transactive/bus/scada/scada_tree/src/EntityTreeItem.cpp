/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/scada_tree/src/EntityTreeItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// EntityTreeItem.cpp : implementation file
//

#include "stdafx.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "bus/scada/scada_tree/src/IEntityFactory.h"

#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\StationEntityData.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\IConfigEntity.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"
#include "core\data_access_interface\src\IConfigItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CEntityTreeItem

CEntityTreeItem::CEntityTreeItem(TA_Base_Core::IEntityData* entityData, IEntityFactory* entityRetrievalFactory,
								 bool bExpandAllEntityTreeItems /*= true*/) 
     : CObjectTreeItem(), m_entityData(entityData), m_entityRetrievalFactory(entityRetrievalFactory),
		m_bExpandAllEntityTreeItems(bExpandAllEntityTreeItems)
{
    m_entityLabel = getDisplayName(entityData->getName().c_str());

        
    if (m_entityData->getType()==TA_Base_Core::DataPointEntityData::getStaticType())
    {
        // This is a datapoint so it definitely does not have any children
        m_hasChildren = false;
    }
    else
    {
        bool isVectorOfChildrenEmpty = true;
        TA_Base_Core::IConfigItem* configItem = dynamic_cast<TA_Base_Core::IConfigItem*>(m_entityData);
        if( (configItem == NULL) || (!configItem->isNew()))
        {
            isVectorOfChildrenEmpty = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfChildren(m_entityData->getKey()).empty();
			//TD15806 - added a new feature for the CScadaTreeCtrl where the user can set the
			//object to display the datapoints or not (using the m_bExpandAllEntityTreeItems flag) 
			if (!m_bExpandAllEntityTreeItems && !isVectorOfChildrenEmpty)
			{
				TA_Base_Core::CorbaNameList dbDatapointList;
				dbDatapointList = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfChildrenOfEntityOfType(entityData->getName(),TA_Base_Core::DataPointEntityData::getStaticType());
				isVectorOfChildrenEmpty = (dbDatapointList.size() > 0) ? true : false;
				dbDatapointList.clear();				
			}
        }
        m_hasChildren = !isVectorOfChildrenEmpty;
    }
}

CEntityTreeItem::~CEntityTreeItem()
{
    if (m_entityRetrievalFactory == NULL)
    {
        // This means we created the data so we must delete it
	    delete m_entityData;
    }
}

CString CEntityTreeItem::getDisplayName(const CString& fullName)
{
	CString newName(fullName);

    int position = fullName.ReverseFind('.');
	if (position!=-1)
	{
		newName = fullName.Right(fullName.GetLength() - position - 1);
	}

    return newName;
}


bool CEntityTreeItem::hasChildren()
{
    if (m_entityRetrievalFactory != NULL)
    {
        if (m_entityData->getType()==TA_Base_Core::DataPointEntityData::getStaticType())
        {
            // This is a datapoint so it definitely does not have any children
            return false;
        }
        
        TA_Base_Core::IConfigItem* configItem = dynamic_cast<TA_Base_Core::IConfigItem*>(m_entityData);
        if( (configItem == NULL) || (!configItem->isNew()))
        {
            return m_entityRetrievalFactory->hasChildren(m_entityData->getKey(), m_hasChildren);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return m_hasChildren;
    }
}

bool CEntityTreeItem::createChildItems()
{
	// Get systems associated with this station
    std::vector<TA_Base_Core::IEntityData*>     entities;
	std::vector<TA_Base_Core::IEntityData*>::const_iterator   it;

    if (m_entityRetrievalFactory != NULL)
    {
        TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(m_entityData);

        // We only want to retrieve child entities if this entity is not a configuration entity OR
        // if it is not a new entity. So for any new entities we do not want to retrieve the children
        if( (entity == NULL) || (!entity->isNew()))
        {
            entities = m_entityRetrievalFactory->getChildEntitiesOf(m_entityData->getKey());
        }
    }
    else
    {
        entities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(m_entityData->getKey(), false);
    }
	if (entities.empty() )
    {
        m_hasChildren = false;
        return false;
    }

    m_hasChildren = true;

	for (it=entities.begin();it!=entities.end();it++)
	{
		AddChildItem(*(new CEntityTreeItem(*it,m_entityRetrievalFactory,m_bExpandAllEntityTreeItems)),false,true);
	}

    SortChildren();

	return true;
}

const char* CEntityTreeItem::getItemLabelText()
{
	return m_entityLabel.c_str();
}


const std::string CEntityTreeItem::getKey()
{
	return m_entityData->getName();
}

HICON CEntityTreeItem::getItemImage(bool itemExpanded)
{
	// No icons by default
	static HICON stationIcon=0;

	return stationIcon;
}

