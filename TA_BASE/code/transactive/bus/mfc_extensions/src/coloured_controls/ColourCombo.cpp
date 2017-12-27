/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/ColourCombo.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Extension of a CComboBox which provides functions to save & restore selection.
  * Also allows text colour of individual combo box items to be defined
  *
  */

#include "../stdafx.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"

#pragma warning(disable : 4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    ColourCombo::ColourCombo()
    :
    m_savedIndex(-1),
    m_savedData(0)
    {
    }
    
    ColourCombo::~ColourCombo()
    {
    }
    
    
    BEGIN_MESSAGE_MAP(ColourCombo, CComboBox)
    	//{{AFX_MSG_MAP(ColourCombo)
    	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()
    
    
    /////////////////////////////////////////////////////////////////////////////
    // ColourCombo message handlers
    void ColourCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
    {
        // This code taken from MSDN sample (CComboBox::DrawItem)
        ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
    
        CString str;
        if (lpDrawItemStruct->itemID != (-1))
        {
            GetLBText(lpDrawItemStruct->itemID, str);
        }
    
        CDC dc;
        dc.Attach(lpDrawItemStruct->hDC);
    
        // Save these value to restore them when done drawing.
        COLORREF crOldTextColor = dc.GetTextColor();
        COLORREF crOldBkColor = dc.GetBkColor();
    
        // If this item is selected, set the background color 
        // and the text color to appropriate values. Erase
        // the rect by filling it with the background color.
        if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
           (lpDrawItemStruct->itemState  & ODS_SELECTED))
        {
           dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
           dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
           dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
        }
        else
        {
           dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
        }
    
        
        DWORD colour = getColourForItem(lpDrawItemStruct->itemData);
    
        if (0 != colour)
        {
            // Colour shouldn't be default black - apply changed colour
            dc.SetTextColor(colour);
        }
    
        // Draw the text if present
        if (str.GetLength() > 0)
        {
            dc.DrawText(
                str.GetBuffer(0),
                str.GetLength(),
                &lpDrawItemStruct->rcItem,
                DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }
        
        // Reset the background color and the text color back to their
        // original values.
        dc.SetTextColor(crOldTextColor);
        dc.SetBkColor(crOldBkColor);
    
        dc.Detach();	
    }
    
    
    void ColourCombo::saveSelection()
    {
        int index = GetCurSel();
    
        m_savedIndex = -1;
    
        if (index >= 0)
        {
            m_savedIndex = index;
            m_savedData = GetItemData(index);
            GetLBText(index, m_savedString);
        }
    }
    
    
    bool ColourCombo::restoreSelection(int defaultIndex)
    {
        // Check if exists a savepoint
        if (m_savedIndex >= 0)
        {    
            for (int i = 0; i < GetCount(); i ++)
            {
                if (GetItemData(i) == m_savedData)
                {
                    CString text;
                    GetLBText(i, text);
                    if (text == m_savedString)
                    {
                        // Found a match, update selection to this
                        SetCurSel(i);
                        return true;
                    }
                }
            }
        }
    
        SetCurSel(defaultIndex);
        return false;
    }
    
    
    void ColourCombo::mapItemDataToColour(DWORD itemData, DWORD textColour)
    {
        m_colourMap[itemData] = textColour;
    }
    
    
    DWORD ColourCombo::getColourForItem(DWORD itemData)
    {
        std::map<DWORD, DWORD>::iterator itr = m_colourMap.find(itemData);
    
        if (itr == m_colourMap.end())
        {
            // return black by default
            return 0;
        }
        else
        {
            return itr->second;
        }
    }

}   // namespace TA_Base_Bus
