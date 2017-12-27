/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentTreeCtrl.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which displays the Tree Ctrl in the parent window
 *
 */

#if !defined(AFX_EQUIPMENTTREECTRL_H__BEE5C350_BFEE_4198_AC8A_FB0BB9E8BBFE__INCLUDED_)
#define AFX_EQUIPMENTTREECTRL_H__BEE5C350_BFEE_4198_AC8A_FB0BB9E8BBFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "CustomDefines.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "app/scada/EquipmentStatusViewer/src/ProgressBar.h"

namespace TA_Base_App
{

	class CEquipmentStatusViewerDlg;
    class IEntityFactory;

	class CEquipmentTreeCtrl : public TA_Base_Bus::CScadaTreeCtrl // CTreeCtrl
	{

	public:

        /**
         * Constructor
         *
         * @param IEntityFactory* - This is the factory that should be used to retrieve the entity data.
         *                          If this is NULL then the EntityAccessFactory will be used instead.
         * @param bool - This indicates whether this class should delete the IEntityFactory or whether
         *               the caller will do it. true = this class will delete. false = caller must delete
         *               Default is for the caller to delete.
         */
	    CEquipmentTreeCtrl(TA_Base_Bus::IEntityFactory* entityRetrievalFactory = NULL, bool shouldDeleteFactory = false);

		/**
		 *	~CEquipmentTreeCtrl()
		 *
		 *	Default destructor
		 *
		 */
		virtual ~CEquipmentTreeCtrl();

		/**
		 *	GetItemDescription()
		 *
		 *	Gets the description of the item from its Data
		 *
		 *	@return		Description as string
		 *
		 */
		std::string GetItemDescription();

        /** 
          * ExpandNodeAllLevels
          *
          * Recursively add all the child datanodes and datapoints for this item
          *
          * @param parentTreeItem   The item to expand (stored in the item.Param in the tree control)
          * @param hParent          The tree control handle for this item
          *
          */
	    void ExpandNodeAllLevels( TA_Base_Bus::CEntityTreeItem& parentTreeItem, HTREEITEM hParent);

        // ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEquipmentTreeCtrl)
		protected:
		//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(CEquipmentTreeCtrl)
	    //}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};

} // TA_Base_App


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPMENTTREECTRL_H__BEE5C350_BFEE_4198_AC8A_FB0BB9E8BBFE__INCLUDED_)
