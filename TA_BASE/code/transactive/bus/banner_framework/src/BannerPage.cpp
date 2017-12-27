/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/BannerPage.cpp $
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

#pragma warning(disable:4786)   // Decorated name too long

#include "stdafx.h"

#include "bus/banner_framework/src/BannerPage.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"
#include "bus/banner_framework/src/SingletonHelper.h"

#include "core/utilities/src/IDllHandler.h"

namespace TA_Base_Bus
{
    BannerPage::BannerPage(const std::string& buttonName, UINT dialogId, int xPosition, int yPosition, int width, int height, BannerFrameworkDialog* parentDialog)
    :
    TStackedPage(dialogId, parentDialog),
    m_xPosition(xPosition),
    m_yPosition(yPosition),
    m_width(width),
    m_height(height),
    m_buttonId(dialogId),
    m_buttonName(buttonName),
    m_parentDialog(parentDialog)
    {
        ASSERT( m_parentDialog != NULL );
        m_parentDialog->AssertValid();
    }


    BannerPage::~BannerPage()
    {
    }


    void BannerPage::setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& entityName, 
                                     const std::string& name)
    {
        SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, entityName, name);
    }


    BOOL BannerPage::OnInitDialog()
    {
        TStackedPage::OnInitDialog();
        MoveWindow(m_xPosition, m_yPosition, m_width, m_height);
        return TRUE;
    }


    void BannerPage::enableButtonFlash()
    {
        if ( NULL != m_parentDialog )
        {
            m_parentDialog->enableButtonFlash(m_buttonId);
        }
    }


    void BannerPage::disableButtonFlash()
    {
        if ( NULL != m_parentDialog )
        {
            m_parentDialog->disableButtonFlash(m_buttonId);
        }
    }


    int BannerPage::getXPosition()
    {
        return m_xPosition;
    }


    int BannerPage::getYPosition()
    {
        return m_yPosition;
    }


    int BannerPage::getWidth()
    {
        return m_width;
    }


    int BannerPage::getHeight()
    {
        return m_height;
    }

} // namespace TA_Base_Bus
