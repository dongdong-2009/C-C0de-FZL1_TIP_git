/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.cpp $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class extends the combo box control to allow a temporary item to be
 * inserted into it. When this item is unselected it is removed from
 * the combo box.
 */

#include "ComboTempItem.h"

namespace TA_Base_Bus
{
    ComboTempItem::ComboTempItem()
    {
    }


    ComboTempItem::~ComboTempItem()
    {
    }


    BEGIN_MESSAGE_MAP(ComboTempItem, CComboBox)
	    //{{AFX_MSG_MAP(ListCtrlOnOffSel)
        ON_CONTROL_REFLECT(CBN_SELENDOK,OnSelChange)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void ComboTempItem::OnSelChange()
    {
        // Get the current selection.
        int selIdx = GetCurSel();
        if ( CB_ERR != selIdx )
        {
            // Make sure the temp string isn't still selected.
            CString selText;
            GetLBText( selIdx, selText );
            if ( 0 == selText.Compare( m_tempString ) )
            {
                return;
            }
        }

        // Remove the temporary string.
        removeTempString();
    }

    int ComboTempItem::AddTempString( CString text )
    {
        // Make sure the temporary string isn't already set.
        removeTempString();

        // Now add the temp string.
        int tempIdx = AddString( text );
        if ( CB_ERR != tempIdx )
        {
            SetCurSel( tempIdx );
            m_tempString = text;
        }

        return tempIdx;
    }

    void ComboTempItem::removeTempString()
    {
        // If the temporary string exists in the combo box find it
        // and remove it.
        if ( !m_tempString.IsEmpty() )
        {
            int tempIdx = FindStringExact( 0, m_tempString );
            if ( CB_ERR != tempIdx )
            {
                DeleteString( tempIdx );
            }
            m_tempString.Empty();
        }
    }
} //TA_Base_Bus

