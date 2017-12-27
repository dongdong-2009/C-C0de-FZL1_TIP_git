/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusListCtrl.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is responsible for displaying the ListCtrl in the main window of the application.
 * It handles the column preparation and supports list sorting, column hiding/unhiding and
 * the image list (icons) to display.
 *
 */

#if !defined(AFX_EQUIPMENTSTATUSLISTCTRL_H__0060C665_8803_4C5B_942D_D84E8B454E33__INCLUDED_)
#define AFX_EQUIPMENTSTATUSLISTCTRL_H__0060C665_8803_4C5B_942D_D84E8B454E33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <set>

#include "CustomDefines.h"

namespace TA_Base_App
{
	class CEquipmentStatusListCtrl : public CListCtrl
	{

	public:

        /** 
          * CEquipmentStatusListCtrl
          *
          * Constructor
          *
          * @param reportItems  The collection of EquipmentStatusListItems used to sort the list.
          *                     This ListCtrl stores the entity keys for each item, and the keys
          *                     access the EquipmentStatusListItems map.
          */
	    CEquipmentStatusListCtrl( TA_Base_App::EquipmentStatusListItemMap const& reportItems);

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
		 * Default virtual destructor
		 *
		 */
		virtual ~CEquipmentStatusListCtrl();

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
          * prepareColumns
          *
          * Sets the column widths and header text depending on the type of report to load.
          *
          * @param reportKey    The key of the report that is being loaded, or one of
          *                     ACTIVE_PROFILE_ALL_DATA or ACTIVE_PROFILE_MMS_DATA
          */
        void prepareColumns( const std::string& reportKey);


        //TD16889 Mintao 
	    /**
	      * getCurrentSortCol
	      * 
	      * get current sort col
	      * 
	      * @return int 
	      * 
	      * @exception <exceptions> Optional
	      */
        int getCurrentSortCol() { return m_currentSortCol;}
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEquipmentStatusListCtrl)
		protected:
		virtual void PreSubclassWindow();
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
        //}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(CEquipmentStatusListCtrl)
        afx_msg void OnDestroy();
	    //}}AFX_MSG

		DECLARE_MESSAGE_MAP()

    private:

        /** 
          * hideColumn
          *
          * Removes column from display by setting its width to zero.
          *
          * @param colIndex     The index number of the column to hide.
          */
        void hideColumn(int colIndex);

        /** 
          * showColumn
          *
          * Restores the column to the display.
          *
          * @param colIndex     The index number of the column to restore.
          * @param width        How wide the column should be now.
          *
          */
        void showColumn(int colIndex, int width);

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
         * @return icon handle for specified icon ID, or NULL if not found
         */
        HICON getIconHandle(const EIcon& icon);

		/** 
		 * An array of bool identifying whether a column is to be sorted in 
		 * Ascending (true) order or Descending (false) order
		 *
  		 */
		bool m_bAscending[ESL_COLUMN_MAX];

        typedef std::map<EIcon, HICON> IconMap;
        IconMap     m_icons;
        CImageList	m_imageList;

        // Defines all the hidden columns in this control
        std::set<int>   m_hiddenCols;

		/** 
		 * Parameters to pass to the list ctrl SortItems() operation.
		 * When using, set the required column and sort direction.
         * The collection of EquipmentStatusListItems that provides the data to sort
		 * is set in the Sort_Parameters constructor.
  		 */
        Sort_Parameters m_sortParams;
        
        //TD16889 Mintao 
        //the currrent sort column
        int m_currentSortCol;
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPMENTSTATUSLISTCTRL_H__0060C665_8803_4C5B_942D_D84E8B454E33__INCLUDED_)
