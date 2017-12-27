/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminObserver.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * AdminObserver is an abstract class. Each object that needs to register 
  * with the System Manager as an observer must inherit from this class and 
  * implement the OnNewData method.
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "SystemManager.h"
#include "AdminObserver.h"
#include "AdminManager.h"

namespace TA_Base_App
{

    AdminObserver::AdminObserver()
    {

    }

    AdminObserver::~AdminObserver()
    {

    }

    void AdminObserver::registerObserver()
    {
        AdminManager::registerObserver(this);
    }

    void AdminObserver::unregisterObserver()
    {
        AdminManager::unregisterObserver(this);
    }

} // namespace TA_Base_App
