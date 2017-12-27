/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.h $
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

#if !defined(AbstractDecoratorButton_2CCBB6DB_6ACB_44f3_AADB_96FFC2AE3407__INCLUDED_)
#define AbstractDecoratorButton_2CCBB6DB_6ACB_44f3_AADB_96FFC2AE3407__INCLUDED_

#include <afxwin.h>

class CDecoratorButton;

namespace TA_Base_Bus
{

    class AFX_EXT_CLASS AbstractDecoratorButton
    {

    public:
	    AbstractDecoratorButton();
	    virtual ~AbstractDecoratorButton();

	    /**
	      * Draw
	      * Performs drawing of the item. This function will be called by DrawItem method
          * of the concrete button class.
	      */
	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    /**
	     * Returns a reference to the undecorated concrete resource button.
	     */
	    CDecoratorButton* undecoratedResourceButton();

	    virtual void setFinalDecorator(AbstractDecoratorButton* pFinalDec) = 0;

	    /**
	     * Traverses the chain of decorators and delete each one in the sequence. The
	     * concrete component must not be deleted, thus default for the method will do
	     * nothing, overload in the decorator's parent class.
	     */
	    virtual void deleteTree();


    protected:
	    /**
	     * This points back to the concrete instance. It will be used to obtain a handle
	     * back to an "undecorated" instance. It can be used to obtain
	     * information/attributes of the CButton class.
	     */
	    CDecoratorButton* m_pConcreteDecoratorButton;

	    /**
	     * This needs to point to the final decorator in the queue to allow the concrete
	     * CButton variant to invoke the pattern from the windows callback.
	     */
	    AbstractDecoratorButton* m_pFinalDecorator;
    };
};

#endif // !defined(AbstractDecoratorButton_2CCBB6DB_6ACB_44f3_AADB_96FFC2AE3407__INCLUDED_)
