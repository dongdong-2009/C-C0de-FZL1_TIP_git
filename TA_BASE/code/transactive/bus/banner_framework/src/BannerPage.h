/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/BannerPage.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * BannerPage is an abstract class that all pages in the banner should inherit
  * from. It inherits from the IBannerPage interface so that the main Banner dialog
  * can use this class to communicate with each page in its own DLL.
  *
  */

#ifndef BANNERPAGE_H
#define BANNERPAGE_H

#include "bus/banner_framework/src/StackedPage.h"
#include "bus/banner_framework/src/IBannerPage.h"


namespace TA_Base_Bus
{
    class BannerFrameworkDialog;

    class BannerPage: public TStackedPage, public IBannerPage
    {
    public:

       /**
         * Constructor
         * 
         * Constructs a page of the banner.
         *
         * @param buttonName - name of the associated tab button
         * @param dialogId - The dialog's resource ID
         * @param xPosition, yPosition, width, height - The dialog's position and dimensions
         * @param parentDialog - a pointer to this pages parent dialog
         *
         * Pre-conditions: parentDialog != NULL
         */
        BannerPage(const std::string& buttonName, UINT dialogId, int xPosition, int yPosition, 
                   int width, int height, BannerFrameworkDialog* parentDialog);

       /**
         * Destructor
         */
        virtual ~BannerPage();

        /** 
          * setUpSingletons
          *
          * This will set up the singletons in the DLL to match those in the main application.
          *
          * @param dllHandler This is a handler from the main application that can be used to
          *                       retrieve the details of it's singletons. This can then be used to
          *                       set up the singletons in the DLL.
          * @param entityName The entity name of the DLL
          * @param dllName    The filename of the DLL
          */
        virtual void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& entityName,
                                     const std::string& dllName);

        /**
          * enableButtonFlash
          *
          * Starts the button flashing.
          */
        void enableButtonFlash();

        /**
          * disableButtonFlash
          *
          * Ends the button flashing.
          */
        void disableButtonFlash();

        /**
          * getXPosition
          *
          * Returns the left most position of the page in pixels.
          */
        int getXPosition();

        /**
          * getYPosition
          *
          * Returns the top most position of the page in pixels.
          */
        int getYPosition();

        /**
          * getWidth
          *
          * Returns the width of the page in pixels.
          */
        int getWidth();

        /**
          * getHeight
          *
          * Returns the height of the page in pixels.
          */
        int getHeight();
    
        /**
          * OnInitDialog
          *
          * Move and size the dialog.
          */
        virtual BOOL OnInitDialog();

        /**
          * getParentDialog
          *
          * Returns the banner page's parent dialog.
          */
        BannerFrameworkDialog* getParentDialog()
        {
            return m_parentDialog;
        }

        /**
          * getButtonName
          *
          * Returns the banner page's button name.
          */
        std::string getButtonName()
        {
            return m_buttonName;
        }

    private:

        int m_xPosition;
        int m_yPosition;
        int m_width;
        int m_height;
        UINT m_buttonId;
        std::string m_buttonName;
        BannerFrameworkDialog* m_parentDialog;
    };

} // namespace TA_Base_Bus

#endif // BANNERPAGE_H
