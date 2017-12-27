/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Allows the Parameters tab to be disabled for steps that do not have any parameters (e.g. Abort).
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepEditorTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorTabCtrl

CPlanStepEditorTabCtrl::CPlanStepEditorTabCtrl()
{
    FUNCTION_ENTRY( "CPlanStepEditorTabCtrl" );
    FUNCTION_EXIT;
}


CPlanStepEditorTabCtrl::~CPlanStepEditorTabCtrl()
{
    FUNCTION_ENTRY( "~CPlanStepEditorTabCtrl" );

    m_disabledTabs.clear();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditorTabCtrl, CTabCtrlWithDisable)
    // {{AFX_MSG_MAP(CPlanStepEditorTabCtrl)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorTabCtrl message handlers

void CPlanStepEditorTabCtrl::DisableTab(int tabIdx)
{
    FUNCTION_ENTRY( "DisableTab" );

    if (IsTabEnabled(tabIdx))
    {
        m_disabledTabs.push_back(tabIdx);
    }

    FUNCTION_EXIT;
}


BOOL CPlanStepEditorTabCtrl::IsTabEnabled(int tabIdx)
{
    FUNCTION_ENTRY( "IsTabEnabled" );

    for (std::vector<int>::const_iterator iter = m_disabledTabs.begin(); iter != m_disabledTabs.end(); iter++)
    {
        if (*iter == tabIdx)
        {
            FUNCTION_EXIT;
            return FALSE;
        }
    }

    FUNCTION_EXIT;
    return TRUE;
}


