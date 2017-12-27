/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ICellManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef ___ICellManager_h__
#define ___ICellManager_h__

#pragma warning(disable : 4786)

#include <vector>
#include <string>

class CEditableListCtrl;   

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//
#ifdef IMPORT_EXTENSIONS
struct __declspec(dllimport) ICellManager
#else
struct AFX_EXT_CLASS ICellManager
#endif
{
    typedef enum 
    {
        CT_NOTEDITABLE,
        CT_TEXT,
        CT_NUMBER,
        CT_SELECTION,
        CT_DECISION,
		CT_CHECKBOX,
		CT_CHECKBOX_DISABLED,
		CT_DATE,
		CT_TIME,
        CT_MASK,
        CT_BUTTON
    } CellType;

    typedef std::vector<std::string> CellData;


    // Return the type of the specified table cell
    virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem) = 0;

    // Return the type of the specified table cell. For unlimited number of characters this method
    // should return 0. This method is only relevant to CT_TEXT and CT_NUMBER field types.
    virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem) = 0;

    // Return the set of values allowed for the specified table cell. Used with CT_SELECTION.
    virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem) = 0;

    // Return the mask to be applied to values entered into the specified table cell. Used with CT_MASK.
    // The mask is formatted so that each changeable character is inside the brackets, and all static 
    // characters (characters not entered by the user, such as a dash or a slash) are outside the brackets. 
    // Dashes can be used inside the brackets to indicate a range of characters (based on the ASCII value 
    // of the start and end character).
    // e.g. "[AB][0-9][0-9]-[0-9][0-9][5-9][A-Z]"
    virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem) = 0;

    // Return the character to be used as a place holder for masked input. Used with CT_MASK.
    // e.g. Givem '_' as the prompt char, an empty masked edit cell using the above mask would contain "___-____".
    virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem) = 0;

    // Indicate whether the specified table cell can be updated to the specified value.
    virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value) = 0;
};

#endif
