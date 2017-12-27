/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/selection_helpers/SelectionHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * SelectionHelper provides a couple of helper functions for tasks related
  * to CListCtrl selection
  *
  */

#pragma once

#include <vector>

namespace TA_Base_Bus
{
    namespace SelectionHelper
    {

        /**
         * deselectAll
         *
         * Clears selection in this list control
         *
         * @param listControl the list to retrieve selection from
         *
         */    
        // This guard is so that this MFC extension can be used in another
        // MFC extension DLL
        //
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) void deselectAll(CListCtrl& listControl);
        #else
        AFX_EXT_CLASS void deselectAll(CListCtrl& listControl);
        #endif

        /**
         * getSelectedItemIndices
         *
         * @return an array of the list indices of all those items selected in list
         *
         * @param listControl the list to retrieve selection from
         *
         */
        // This guard is so that this MFC extension can be used in another
        // MFC extension DLL
        //
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) std::vector<unsigned int> getSelectedItemIndices(const CListCtrl& listControl);
        #else
        AFX_EXT_CLASS std::vector<unsigned int> getSelectedItemIndices(const CListCtrl& listControl);
        #endif        

        /**
         * getSelectedItemDataValues
         *
         * @return an array of the item data values of all those items selected in list
         *
         * @param listControl the list to retrieve selection data from
         *
         */
        // This guard is so that this MFC extension can be used in another
        // MFC extension DLL
        //
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) std::vector<DWORD> getSelectedItemDataValues(const CListCtrl& listControl);
        #else
        AFX_EXT_CLASS std::vector<DWORD> getSelectedItemDataValues(const CListCtrl& listControl);
        #endif        


        /**
         * deleteSelectedItems
         *
         * Deletes the current selection from the input control
         *
         * @return the number of items remove from the input list control
         *
         * @param listControl the list to retrieve and then delete selection from
         *
         */
        // This guard is so that this MFC extension can be used in another
        // MFC extension DLL
        //
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) unsigned int deleteSelectedItems(CListCtrl& listControl);
        #else
        AFX_EXT_CLASS unsigned int deleteSelectedItems(CListCtrl& listControl);
        #endif        

        /**
         * deleteItems
         *
         * Deletes items defined by the input indices from the control
         *
         * @param listControl the list to delete selection from
         *
         * @param sel array defining the indices to be removed from control
         *
         * @return number of items removed (if index array contains out of range indices, this
         *          may be less than the number of input indices)
         *
         */
        // This guard is so that this MFC extension can be used in another
        // MFC extension DLL
        //
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) unsigned int deleteItems(CListCtrl& listControl, std::vector<unsigned int> sel);
        #else
        AFX_EXT_CLASS unsigned int deleteItems(CListCtrl& listControl, std::vector<unsigned int> sel);
        #endif                       

        /**
         * findStringInComboCaseSensitive
         *
         * Finds a specific string in the specified dialog, returning the index
         *
         * @param combo the combo box to be searched
         *
         * @param str the string to search for
         *
         * @return LB_ERR if string not found, otherwise the index in the combo box
         *
         */
        #ifdef IMPORT_EXTENSIONS
        __declspec(dllimport) int findStringInComboCaseSensitive(const CComboBox& combo, const std::string& str);
        #else
        AFX_EXT_CLASS int findStringInComboCaseSensitive(const CComboBox& combo, const std::string& str);
        #endif                       

    }   // namespace SelectionHelper

}   // namespace TA_Base_Bus
