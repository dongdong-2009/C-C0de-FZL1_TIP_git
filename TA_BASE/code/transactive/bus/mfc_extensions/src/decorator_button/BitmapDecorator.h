/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/BitmapDecorator.h $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Display an Icon/Bitmap at a specified position on the button
 */

#if !defined(BitmapDecorator_C4257EF9_D335_4152_87FD_63BE488233E7__INCLUDED_)
#define BitmapDecorator_C4257EF9_D335_4152_87FD_63BE488233E7__INCLUDED_

#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"

class AbstractDecoratorButton;

namespace TA_Base_Bus
{
    class AFX_EXT_CLASS BitmapDecorator : public ButtonDecorator 
    {

    public:

	    /**
	      * Constructor
	      * 
	      */
	    BitmapDecorator(AbstractDecoratorButton* pComponent);

	    /**
	      * Destructor
	      * 
	      */
	    virtual ~BitmapDecorator();

	    /**
	      * Draw
	      * 
	      * Perform drawing, this function will be called by the concrete decorator class
	      */
	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    /**
	      * setBitmap
	      * 
	      * set the bitmap object of the decorator. This class will own the bitmap
          * and delete it when it is not needed.
	      */
	    void setBitmap(CBitmap* bitmap, int x, int y);

    private:
        // disabled methods
        BitmapDecorator();
        BitmapDecorator(const BitmapDecorator&);
        void operator=(const BitmapDecorator&);
    

    private:

        /**
	      * The bitmap to be drawn
          */
	    CBitmap* m_bmpImage;

        /**
	      * Holds bitmap information
          */
	    BITMAP   m_bmpInfo;			

        /**
	      * Position of the bitmap
          */
	    int m_xPos;

        /**
	      * Position of the bitmap
          */
	    int m_yPos;
	    


    };
};

#endif // !defined(BitmapDecorator_C4257EF9_D335_4152_87FD_63BE488233E7__INCLUDED_)
