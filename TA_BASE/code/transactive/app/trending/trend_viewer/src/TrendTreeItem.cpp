/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendTreeItem.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TrendTreeItem.cpp : implementation file
//

#include "app/trending/trend_viewer/src/StdAfx.h"

#include "app/trending/trend_viewer/src/TrendTreeItem.h" 
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/GenericItem.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TrendViewerException.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

 

using namespace TA_Base_Core;
/////////////////////////////////////////////////////////////////////////////
// CTrendTreeItem

namespace TA_Base_App
{	
 
	CTrendTreeItem::CTrendTreeItem(TA_Base_Core::IEntityData* entityData, IItemStore* itemStore, 
		   bool bItemTrendEnabled /*= false*/ ) //TD17812
		: CObjectTreeItem(), m_entityData(entityData), m_itemStore(itemStore), m_hasChildren(true),
		  m_bItemTrendEnabled(bItemTrendEnabled)
	{		
		//Note: entityData and itemstore should not be NULL
		if ((m_entityData == NULL) || (m_itemStore == NULL))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "EntityData or Item Store should not be NULL.");
			TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, "An unexpected error occurred."));
		}		

		m_entityLabel = getDisplayName(entityData->getName().c_str());				
        
		if (m_entityData->getType()==TA_Base_Core::DataPointEntityData::getStaticType())
		{
			// This is a datapoint so it definitely does not have any children
			m_hasChildren = false;
		} 
		else
		{
			bool isChildListEmpty = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfChildren(m_entityData->getKey()).empty();
			m_hasChildren = !isChildListEmpty;			
		}		
	}
	
	CString CTrendTreeItem::getDisplayName(const CString& fullName)
	{
		CString newName(fullName);

		int position = fullName.ReverseFind('.');
		if (position!=-1)
		{
			newName = fullName.Right(fullName.GetLength() - position - 1);
		}

		return newName;
	}

	bool CTrendTreeItem::hasChildren()
	{
		return m_hasChildren;
	}
	
	bool CTrendTreeItem::createChildItems()
	{
 		if (true == m_hasChildren)
		{
			std::vector<TA_Base_Core::IEntityData*> entities;
			entities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(m_entityData->getKey(), false);
			if (entities.empty() )
			{
				m_hasChildren = false;				
			}			
			else
			{	
				bool hasChildEnabledTrend = false;

				std::vector<TA_Base_Core::IEntityData*>::const_iterator   it;
				for (it=entities.begin();it!=entities.end();it++)
				{
					
					//check first if the entity to be added to the tree is a datapoint
					if ((*it)->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
					{						
						std::string entityName =  (*it)->getName();
						try
						{
							// if item is a datapoint, then lookup 
							// if the datapoint is stored in the the item store
							// if the datapoint is not found in the item store, 
							// just skip this entity, no need to display in the scada tree							
							GenericItem* gItem = m_itemStore->getItem(entityName);
							if (NULL != gItem)
							{
								AddChildItem(*(new CTrendTreeItem(*it, m_itemStore,true)),false,true);								
							}	
						}
						catch(...)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Item not found in IItemStore : %s", entityName);
						}
					}
					else
					{
						// must ignore synthetic datanodes
						CString entityName =  getDisplayName((*it)->getName().c_str());
						if (entityName.Left(3).Compare("syn") != 0) 
						{
							AddChildItem(*(new CTrendTreeItem(*it, m_itemStore)),false,true);
							hasChildEnabledTrend = true;
						}
					}				
				}
				//TD17767 - if no valid child was created, reset m_hasChildren to false
				if (hasChildEnabledTrend)
					SortChildren();
				
				m_hasChildren = hasChildEnabledTrend;
				//++TD17767
			}
		}
		return m_hasChildren;
	}
	
	const char* CTrendTreeItem::getItemLabelText()
	{
		return m_entityLabel.c_str();
	} 
}