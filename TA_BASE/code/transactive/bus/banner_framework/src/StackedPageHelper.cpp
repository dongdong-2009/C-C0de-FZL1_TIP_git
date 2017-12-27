/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StackedPageHelper.cpp $
  * @author:  Zoran M.Todorovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TStackedPageHelper class defintion
  */

#include "stdafx.h"

#pragma warning(disable:4786)   // Decorated name too long

#include "bus/banner_framework/src/StackedPageHelper.h"
#include "bus/banner_framework/src/StackDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    TStackedPageHelper::TStackedPageHelper()
    {
        m_pDialog = NULL;
        m_bCreated = FALSE;
        m_bActive = FALSE;
    }

    TStackedPageHelper::TStackedPageHelper(const TStackedPageHelper& obj)
    { 
        *this = obj;
    }

    TStackedPageHelper::~TStackedPageHelper()
    {
    }

    TStackedPageHelper& TStackedPageHelper::operator=(const TStackedPageHelper& obj)
    {
        m_nDialogID = obj.m_nDialogID;
        m_pDialog = obj.m_pDialog;
        m_dwHelpID = obj.m_dwHelpID;
        m_bCreated = obj.m_bCreated;
        m_bActive = obj.m_bActive;
        m_dwData1 = obj.m_dwData1;
        m_dwData2 = obj.m_dwData2;
        return *this;
    }

    // Returns a pointer to child dialog associated with this page
    TStackedPage *TStackedPageHelper::GetPage(void)
    {
        return m_pDialog;
    }

    // Returns TRUE if the child dialog associated with this page is currently
    // selected, visible and focused
    BOOL TStackedPageHelper::IsActive(void)
    {
        return m_bActive;
    }

    // Activate this page and associated child dialog
    void TStackedPageHelper::Activate(void)
    {
        m_pParent->SetPage(m_nDialogID);
    }

    // Deactivate this page and associated child dialog
    void TStackedPageHelper::Deactivate(void)
    {
        m_pParent->SetPage();
    }
}
