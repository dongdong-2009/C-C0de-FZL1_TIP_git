//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanControllerToolBarManager.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANCONTROLLERTOOLBARMANAGER_H__AAEC7BD5_9E47_4BC8_94F2_F8319065C9F5__INCLUDED_)
#define AFX_PLANCONTROLLERTOOLBARMANAGER_H__AAEC7BD5_9E47_4BC8_94F2_F8319065C9F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AbstractToolBarManager.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanControllerToolBarManager

    class PlanControllerToolBarManager : public AbstractToolBarManager
    {
    // Types
    public:
        typedef enum
                {
                    TBT_GENERAL = 0,
                    TBT_STEPS,
                    TBT_FLOWCHART,
                    TBT_NONE
                }
                EToolBarType;

    // Operations
    public:
        PlanControllerToolBarManager();
        virtual ~PlanControllerToolBarManager();

        void selectToolBar(EToolBarType toolBarType);

    protected:
        virtual void initialiseToolBarList();
        virtual void extendToolBar(CToolBar *toolBar);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANCONTROLLERTOOLBARMANAGER_H__AAEC7BD5_9E47_4BC8_94F2_F8319065C9F5__INCLUDED_)
