/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/BorderDecorator.h $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Manipuate the "border" color, i.e. the outer frame's color and thickness can be
 * specified. It is also possible to display the "default" border. This can also
 * be used to simulate "flashing" behaviour by alternating between the default and
 * specified background colour on a timed interval.
 */

#if !defined(BorderDecorator_DCB2A0B5_49E6_4a7c_8CF4_22E3121D0ADB__INCLUDED_)
#define BorderDecorator_DCB2A0B5_49E6_4a7c_8CF4_22E3121D0ADB__INCLUDED_

#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"

class AbstractDecoratorButton;

namespace TA_Base_Bus
{
    class AFX_EXT_CLASS BorderDecorator : public ButtonDecorator 
    {

    public:
	    BorderDecorator(AbstractDecoratorButton* pComponent);
	    virtual ~BorderDecorator();

    public:

	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    void setBorderProperties(COLORREF borderColor, int width);

    private:
        // disabled methods
        BorderDecorator();

    private:

	    COLORREF m_borderColor;

	    int m_width;
    };
};

#endif // !defined(BorderDecorator_DCB2A0B5_49E6_4a7c_8CF4_22E3121D0ADB__INCLUDED_)