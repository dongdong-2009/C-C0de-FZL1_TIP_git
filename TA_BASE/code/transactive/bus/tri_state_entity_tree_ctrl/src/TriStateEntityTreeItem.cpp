/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/tri_state_entity_tree_ctrl/src/TriStateEntityTreeItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// TriStateEntityTreeItem.cpp : implementation file
//

#pragma warning(disable:4786)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls

#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"

#include "bus\tri_state_entity_tree_ctrl\src\TriStateEntityTreeCtrl.h"
#include "bus\tri_state_entity_tree_ctrl\src\TriStateEntityTreeItem.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CTriStateEntityTreeItem

CTriStateEntityTreeItem::CTriStateEntityTreeItem(TA_Base_Core::IEntityData* entityData) 
     : CObjectTreeItem(), m_entityData(entityData), m_iNumChildEntities(-1), m_hIcon(0)
{
	m_entityLabel = entityData->getName();
}

CTriStateEntityTreeItem::~CTriStateEntityTreeItem()
{
	delete m_entityData;

	// if the vector was not cleared for some reason, we must clean it up now
	// (I don't know if its possible not to have been)
	if( m_childEntities.size() > 0 )
	{
		EntityDataPtrVec::const_iterator   it;

		for( it=m_childEntities.begin(); it != m_childEntities.end(); it++ )
		{
			TA_Base_Core::IEntityData* pEntity = *it;
			delete pEntity;
		}
	}
}

bool CTriStateEntityTreeItem::hasChildren()
{
	// we should have a map of the originally selected 

	if( m_iNumChildEntities < 0 )
	{
		m_childEntities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(m_entityData->getKey(), false);
		m_iNumChildEntities = m_childEntities.size();

		// here is where we can determine the checkmark state of this parent item
		// if all children are checked or unchecked, or if some are checked

		if( m_iNumChildEntities > 0 )
		{
			HICON hIcon=NULL; // undefined value
			String2IntMap* pEntityMap = getTreeCtrl()->getEntityMap();

			for( EntityDataPtrVec::const_iterator it=m_childEntities.begin(); it != m_childEntities.end(); it++ )
			{
				TA_Base_Core::IEntityData* pEntity = *it;

				String2IntMap::iterator it2 = pEntityMap->find( pEntity->getName() );
				if( it2 == pEntityMap->end() )
				{
					if( hIcon == getTreeCtrl()->getCheckedIcon() )
					{
						// means this child is not in the map but another one is
						hIcon = getTreeCtrl()->getIndeterminantIcon();
						break;
					}
					hIcon = getTreeCtrl()->getUnCheckedIcon();
				}
				else
				{
					if( hIcon == getTreeCtrl()->getUnCheckedIcon() )
					{
						// means this child is in the map but another one wasn't
						hIcon = getTreeCtrl()->getIndeterminantIcon();
						break;
					}
					hIcon = getTreeCtrl()->getCheckedIcon();
				}
			}
			// set the icon
			setIcon( hIcon );
		}
	}
	return m_iNumChildEntities < 1? false:true;
}

bool CTriStateEntityTreeItem::createChildItems()
{
	// should already be filled in the hasChildren call

	if( m_childEntities.size() < 1 ) return false;

	EntityDataPtrVec::const_iterator   it;
	String2IntMap* pEntityMap = getTreeCtrl()->getEntityMap();

	for( it=m_childEntities.begin(); it != m_childEntities.end(); it++ )
	{
		TA_Base_Core::IEntityData* pEntity = *it;
		CTriStateEntityTreeItem& childItem = *(new CTriStateEntityTreeItem(pEntity));
		
		// see if this child is in the Zone map
		String2IntMap::iterator it2 = pEntityMap->find( pEntity->getName() );

		// set the icon
		childItem.setIcon( it2 == pEntityMap->end()? getTreeCtrl()->getUnCheckedIcon():getTreeCtrl()->getCheckedIcon() );

		AddChildItem(childItem, false, true);
	}

	// now clear the vector
	m_childEntities.clear();

	return true;
}

const char* CTriStateEntityTreeItem::getItemLabelText()
{
	return m_entityLabel.c_str();
}

const std::string CTriStateEntityTreeItem::getKey()
{
	return m_entityData->getName();
}

HICON CTriStateEntityTreeItem::getItemImage(bool itemExpanded)
{
	return m_hIcon;
}

