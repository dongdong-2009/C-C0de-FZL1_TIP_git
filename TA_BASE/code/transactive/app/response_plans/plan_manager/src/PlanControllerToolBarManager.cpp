//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "Resource.h"
#include "PlanControllerToolBarManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanControllerToolBarManager

PlanControllerToolBarManager::PlanControllerToolBarManager()
{
    FUNCTION_ENTRY( "PlanControllerToolBarManager" );
    FUNCTION_EXIT;
}


PlanControllerToolBarManager::~PlanControllerToolBarManager()
{
    FUNCTION_ENTRY( "~PlanControllerToolBarManager" );
    FUNCTION_EXIT;
}


void PlanControllerToolBarManager::selectToolBar(EToolBarType toolBarType)
{
    FUNCTION_ENTRY( "selectToolBar" );

    AbstractToolBarManager::selectToolBar( toolBarType );

    FUNCTION_EXIT;
}


void PlanControllerToolBarManager::initialiseToolBarList()
{
    FUNCTION_ENTRY( "initialiseToolBarList" );

    CToolBar *generalTabToolbar = addToolBarToList(IDR_PLAN_CONTROLLER_GENERAL_TOOLBAR);     // TBT_GENERAL
    CToolBar *stepsTabToolbar = addToolBarToList(IDR_PLAN_CONTROLLER_STEPS_TOOLBAR);         // TBT_STEPS
    //CToolBar *flowchartTabToolbar = addToolBarToList(IDR_PLAN_CONTROLLER_FLOWCHART_TOOLBAR); // TBT_STEPS

    extendToolBar(generalTabToolbar);
    extendToolBar(stepsTabToolbar);
    //extendToolBar(flowchartTabToolbar);

    FUNCTION_EXIT;
}


void PlanControllerToolBarManager::extendToolBar(CToolBar *toolBar)
{
    FUNCTION_ENTRY( "extendToolBar" );

    TA_ASSERT(toolBar, "Toolbar is null");

    int executeIdx = toolBar->CommandToIndex(ID_PLAN_EXECUTE);
    if (executeIdx != -1)
    {
        toolBar->GetToolBarCtrl().SetExtendedStyle(toolBar->GetToolBarCtrl().GetExtendedStyle() | TBSTYLE_EX_DRAWDDARROWS);
        toolBar->SetButtonStyle(executeIdx, toolBar->GetButtonStyle(executeIdx) | BTNS_DROPDOWN );
        toolBar->SetSizes(CSize(80, 55), CSize(24,24));
    }

    FUNCTION_EXIT;
}


