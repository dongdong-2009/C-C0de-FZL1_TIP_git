/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/IBannerPage.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface must be implemented by each Banner page. The main Banner application
  * uses this interface to inform a Banner Page that its icon has been double clicked.
  * It is also used to provide the Banner Page with a reference to the parent dialog,
  * so that the flash state of its icon can be set.
  *
  */

#ifndef IBANNERPAGE_H
#define IBANNERPAGE_H

#include <string>

namespace TA_Base_Core
{
    class IDllHandler; // Forward declaration
}

namespace TA_Base_Bus
{
    class IBannerPage
    {
    public:

        /**
          * Destructor
          */
        virtual ~IBannerPage() {};

        /** 
          * setUpSingletons
          *
          * This will set up the singletons in the DLL to match those in the main application.
          *
          * @param dllHandler This is a handler from the main application that can be used to
          *                   retrieve the details of it's singletons. This can then be used to
          *                   set up the singletons in the DLL.
          * @param entityName The entity name of the DLL
          * @param dllName    The filename of the DLL
          */
        virtual void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& entityName,
                                     const std::string& dllName) = 0;

        /**
          * onButtonDoubleClick
          *
          * Informs the Banner Page that its icon has been double clicked, so that it can perform
          * a task specific to the page.
          */
        virtual void onButtonDoubleClick() = 0;

        /**
          * addBannerPage
          *
          * Tells the banner page to add itself to the banner.
          */
        virtual void addBannerPage() = 0;
    };
} // namespace TA_Base_Bus

#endif // IBANNERPAGE_H
