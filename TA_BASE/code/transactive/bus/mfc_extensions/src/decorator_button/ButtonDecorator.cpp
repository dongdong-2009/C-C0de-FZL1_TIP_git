/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/ButtonDecorator.cpp $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"

namespace TA_Base_Bus
{
    ButtonDecorator::~ButtonDecorator()
    {

    }

    ButtonDecorator::ButtonDecorator(AbstractDecoratorButton* pcomponent) :
            m_pcomponent(pcomponent)
    {
	    setFinalDecorator(this);
    }

    void ButtonDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {

    }

    void ButtonDecorator::deleteTree()
    {
	    if ( m_pcomponent != NULL )
	    {
		    m_pcomponent->deleteTree();
            m_pcomponent = NULL;
	    }
	    delete this;
    }

    void ButtonDecorator::setFinalDecorator(AbstractDecoratorButton* pFinalDec)
    {
        ASSERT(m_pcomponent != NULL);
    
	    m_pcomponent->setFinalDecorator(pFinalDec);
	    m_pFinalDecorator = this;
    }
}
