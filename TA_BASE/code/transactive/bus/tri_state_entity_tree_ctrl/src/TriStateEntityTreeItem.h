/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/tri_state_entity_tree_ctrl/src/TriStateEntityTreeItem.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// TriStateEntityTreeItem.h : header file
//

#if !defined(ENTITYTREEITEM_H_INCLUDED)
#define ENTITYTREEITEM_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.h"

namespace TA_Base_Core
{
	class IEntityData;
}

typedef std::vector<TA_Base_Core::IEntityData*> EntityDataPtrVec;

#define TREE_ICON_UNCHECKED		0
#define TREE_ICON_CHECKED		1
#define TREE_ICON_INDETERMINATE	2

class CTriStateEntityTreeCtrl;

/////////////////////////////////////////////////////////////////////////////
// CTriStateEntityTreeItem

#if 0
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CTriStateEntityTreeItem : public CObjectTreeItem
#else
class AFX_EXT_CLASS CTriStateEntityTreeItem : public CObjectTreeItem
#endif
#endif
class CTriStateEntityTreeItem : public CObjectTreeItem
{
	public:

		CTriStateEntityTreeItem(TA_Base_Core::IEntityData* entityData);
		~CTriStateEntityTreeItem();

		TA_Base_Core::IEntityData* getEntityData() { return m_entityData;};
		CTriStateEntityTreeCtrl* getTreeCtrl() { return (CTriStateEntityTreeCtrl*)CObjectTreeItem::getTreeCtrl(); }
		void setIcon( HICON hIcon ) { m_hIcon = hIcon; }

		virtual bool createChildItems();
		virtual const char * getItemLabelText();
		virtual bool hasChildren();
		virtual const std::string getKey();
		virtual HICON getItemImage(bool itemExpanded);

	private:

		int						m_iNumChildEntities;
		EntityDataPtrVec		m_childEntities;
		std::string				m_entityLabel;
		TA_Base_Core::IEntityData*	m_entityData;
		HICON					m_hIcon;
};

#endif // !defined(ENTITYTREEITEM_H_INCLUDED)
