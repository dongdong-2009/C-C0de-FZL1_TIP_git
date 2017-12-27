/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentTreeCtrl.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which displays the Tree Ctrl in the parent window
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "EquipmentTreeCtrl.h"
#include "EquipmentStatusViewerDlg.h"

// NOTE : Location related
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// NOTE : Entity related
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

// NOTE : Debug related
#include "core/utilities/src/DebugUtil.h"

#include "bus\mfc_extensions\src\mfc_extensions.h"
// NOTE : Custom progress bar
#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Core;

namespace TA_Base_App
{

    CEquipmentTreeCtrl::CEquipmentTreeCtrl( TA_Base_Bus::IEntityFactory* entityRetrievalFactory /*= NULL*/,
                                            bool shouldDeleteFactory /*= false*/)
        : CScadaTreeCtrl( entityRetrievalFactory, shouldDeleteFactory)
    {
        m_bMulti = false; // TRUE;
        // Turn of the drag selection - it can be excessively slow (all the selection changed
        // notifications in turn require processing to determine state of subsystem checkbox, 
        // so the drag select operation can be quite chunky)
        m_bAllowDragSelection = FALSE;
    }


	CEquipmentTreeCtrl::~CEquipmentTreeCtrl() 
	{
	}


	BEGIN_MESSAGE_MAP(CEquipmentTreeCtrl, CScadaTreeCtrl)
		//{{AFX_MSG_MAP(CEquipmentTreeCtrl)
    	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	std::string CEquipmentTreeCtrl::GetItemDescription()
	{
        CObjectTreeItem* pItem = getSelectedItem();
        if (NULL == pItem)
            return " ";
        return std::string( pItem->getItemLabelText());
	}


	void CEquipmentTreeCtrl::ExpandNodeAllLevels(TA_Base_Bus::CEntityTreeItem& parentTreeItem,
                                                HTREEITEM hParent)
                                                // Node_Item aParentNodeType, 
												// HTREEITEM aParentNode, 
												// unsigned long aParentKey, 
												// CString aNodeLabel)

	{
        if ( parentTreeItem.getEntityData()->getType() == DataPointEntityData::getStaticType() )
        {
            // Reached the bottom level of the tree (and the recursion)
            return;
        }

		// expand this node one level
		//	ExpandNodeOneLevel(aParentNodeType, aParentNode, aParentKey, GetItemText(aParentNode), TVE_COLLAPSE);
        Expand( hParent, TVE_TOGGLE);
        Expand( hParent, TVE_TOGGLE);
        // parentTreeItem.createChildItems();

		// update window so we dont see just gray nothing
		UpdateWindow();

		// get all expanded children and expand them recursively
		HTREEITEM hNextChild = GetChildItem( hParent);
		while (hNextChild)
		{
				// Tree_Item_Type *param = (Tree_Item_Type *)GetItemData(aNextChild);
            TA_Base_Bus::CEntityTreeItem* treeItem =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(GetItemData(hNextChild));

		    if (treeItem != NULL)
            {
			    ExpandNodeAllLevels( *treeItem, hNextChild);
            }

//				ExpandNodeAllLevels(param->itemType, aNextChild, param->itemId, GetItemText(aNextChild));

				hNextChild = GetNextItem(hNextChild, TVGN_NEXT);
			}
	}

} // TA_Base_App
