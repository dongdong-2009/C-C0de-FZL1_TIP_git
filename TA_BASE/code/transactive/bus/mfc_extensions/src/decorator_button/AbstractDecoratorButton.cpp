/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.cpp $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * A slight modification on the decorator button needs to be implemented here due
 * to the CButton DrawItem windows callback that will allways target the concrete
 * class. To get around this, we need a handle to the last decorator in the queue.
 * This handle needs to be reset by every additional decorator that gets added to
 * the queue.
 *
 * NOTE: The BS_OWNERDRAW button style used by this class does not seem to be compatible with 
 * the radio button styles (BS_AUTORADIOBUTTON and BS_RADIOBUTTON) - these styles should not be used. Nor
 * should the CButton methods GetCheck() and SetCheck() be used. They do not function correctly.
 * Instead, this class provides an alternate "check mechanism" (see isChecked(), etc). 
 * Note that checked buttons are drawn as "pressed".
 * This also means that the client of this class is responsible for checking and un-checking "grouped" buttons. 
 *
 */


#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.h"

namespace TA_Base_Bus
{
    AbstractDecoratorButton::AbstractDecoratorButton() : m_pFinalDecorator(NULL)
    {

    }


    AbstractDecoratorButton::~AbstractDecoratorButton()
    {

    }

    void AbstractDecoratorButton::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {

    }

    CDecoratorButton* AbstractDecoratorButton::undecoratedResourceButton()
    {
        ASSERT(m_pFinalDecorator != NULL);
    
	    m_pFinalDecorator->deleteTree();
	    return m_pConcreteDecoratorButton;
    }

    void AbstractDecoratorButton::deleteTree()
    {
    }
}

