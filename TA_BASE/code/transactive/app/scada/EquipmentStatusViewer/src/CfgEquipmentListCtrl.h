/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/CfgEquipmentListCtrl.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is responsible for displaying the ListCtrl in the Configuration dialog.
 * It supports column sorting based on text in the cell
 *
 */

#if !defined(AFX_CFGEQUIPMENTLISTCTRL_H__20952D24_C2E8_4D2C_ADED_928E19E696FC__INCLUDED_)
#define AFX_CFGEQUIPMENTLISTCTRL_H__20952D24_C2E8_4D2C_ADED_928E19E696FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "CustomDefines.h"

namespace TA_Base_App
{
	class CCfgEquipmentListCtrl : public CListCtrl
	{

	public:

		/**
  		 * CCfgEquipmentListCtrl()
  		 *
         * Constructor
         *
          * @param reportItems  The collection of EquipmentStatusListItems used to sort the list.
          *                     This ListCtrl stores the entity keys for each item, and the keys
          *                     access the EquipmentStatusListItems map.
  		 */
    	CCfgEquipmentListCtrl( TA_Base_App::EquipmentStatusListItemMap const& reportItems);

		/**
  		 * SortColumn()
		 *
		 * Sorts a particular column
  		 *
  		 * @param	aCol	the 0-based index of the column to sort
		 *
  		 */
		void SortColumn(int aCol);

		/**
  		 * GetSortOrder()
		 *
		 * Gets the sort order of a particular column
  		 *
  		 * @param	aCol	the 0-based index of the column sort order to retrieve
		 * @return			bool specifying if column sorting set to Ascending (true)
		 *					or Descending (false)
		 *
  		 */
		bool GetSortOrder(int aCol);

		/**
  		 * SetSortOrder()
  		 *
		 * Sets sort order of a particular column
		 *
  		 * @param	aCol		the 0-based index of the column sort order to set
		 * @param	aAscending	bool specifying if the column sort order to set is
		 *						to be Ascending (true) or Descending (false)
		 *
  		 */
		void SetSortOrder(int aCol, bool aAscending);

		/**
  		 * ~CCfgEquipmentListCtrl()
  		 *
		 *	Default virtual destructor
		 *
  		 */
		virtual ~CCfgEquipmentListCtrl();

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCfgEquipmentListCtrl)
		protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL

	protected:

		//{{AFX_MSG(CCfgEquipmentListCtrl)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	private:

		/**
		 * An array of bool identifying whether a column is to be sorted in 
		 * Ascending (true) order or Descending (false) order
		 *
  		 */
		bool m_bAscending[8];

        /**
          * CompareFunc
          *
          * Called when the CListCtrl is sorting to determine the relative order
          * of two items.
          *
          * @param lParam1      First item's data (entitykey)
          * @param lParam2      Second item's data
          * @param lParamSort   struct Sort_Parameters containing the column number to sort
          *                     by, the sort direction and the pointer to the parent dialog
          *                     where the proxies are kept
          *
          * @return             -1 if first item should come before second item
          *                     0  if items are the same sort position (ie. they match on this column)
          *                     +1 if first item should come after second item
          */
        static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

		/** 
		 * Parameters to pass to the list ctrl SortItems() operation.
		 * When using, set the required column and sort direction.
         * The collection of EquipmentStatusListItems that provides the data to sort
		 * is set in the Sort_Parameters constructor.
  		 */
        Sort_Parameters m_sortParams;

    };

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFGEQUIPMENTLISTCTRL_H__20952D24_C2E8_4D2C_ADED_928E19E696FC__INCLUDED_)
