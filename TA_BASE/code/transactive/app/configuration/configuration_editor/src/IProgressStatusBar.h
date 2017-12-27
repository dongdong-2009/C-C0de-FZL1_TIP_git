/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/IProgressStatusBar.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This interface is for the Status Bar progress bar control. This interface is used to pass it around
  * so the classes receiving it only see limited methods.
  */

#if !defined(IProgressStatusBar_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
#define IProgressStatusBar_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_

#include "app/configuration/configuration_editor/src/StdAfx.h"

namespace TA_Base_App
{
    class IProgressStatusBar
    {
    public:

        /**
          * Destructor
          */
        virtual ~IProgressStatusBar() { };


        /**
          * displayProgressBar
          *
          * This displays a progress bar over the top of the status bar and returns the control
          * so that the progress can be incremented and controlled
          *
          * @return CProgressCtrl& - The progress control showing on the status bar
          */
        virtual CProgressCtrl& displayProgressBar() =0;
        

        /**
          * hideProgressBar
          *
          * This hides the progress bar so that it is no longer showing over the status bar
          */
        virtual void hideProgressBar() =0;
	};
}

#endif //IProgressStatusBar_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_
