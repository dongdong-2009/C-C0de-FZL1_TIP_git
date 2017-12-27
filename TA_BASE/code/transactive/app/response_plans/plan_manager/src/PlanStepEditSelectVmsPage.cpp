//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditSelectVmsPage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#include "stdafx.h"

#if defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)

#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/signs/vms_agent/idl/src/SignControlCorbaDef.h"
#include "PlanManager.h"
#include "PlanStepEditSelectVmsPage.h"
#include "VmsBlankoutStep.h"
#include "VmsEnquirer.h"
#include "VmsRestoreScheduleStep.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const unsigned long DEFAULT_VMS_KEY = 0;
    const std::string DEFAULT_VMS_NAME = "";
    const std::string DEFAULT_ITEM = "<No VMS Selected>";
}

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditSelectVmsPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditSelectVmsPage, CPlanStepEditParameterPage)

CPlanStepEditSelectVmsPage::CPlanStepEditSelectVmsPage(VmsBlankoutStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditSelectVmsPage::IDD, step),
      m_vmsParameter(*step->getVmsParameter()),
      m_stepType(step->getStepType())
{
    FUNCTION_ENTRY( "CPlanStepEditSelectVmsPage" );
    FUNCTION_EXIT;
}


CPlanStepEditSelectVmsPage::CPlanStepEditSelectVmsPage(VmsRestoreScheduleStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditSelectVmsPage::IDD, step),
      m_vmsParameter(*step->getVmsParameter()),
      m_stepType(step->getStepType())
{
    FUNCTION_ENTRY( "CPlanStepEditSelectVmsPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditSelectVmsPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditSelectVmsPage)
    DDX_Control(pDX, IDC_SIGN_LIST, m_signs);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditSelectVmsPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditSelectVmsPage)
    ON_CBN_SELCHANGE(IDC_SIGN_LIST, OnSelectionChangeSignList)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditSelectVmsPage message handlers


BOOL CPlanStepEditSelectVmsPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    PopulateVmsCombo();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditSelectVmsPage::OnSelectionChangeSignList()
{
    FUNCTION_ENTRY( "OnSelectionChangeSignList" );

    // Find the key of the selected sign.

    int signIndex = m_signs.GetCurSel();
    if ( CB_ERR == signIndex )
    {
        // There is no sign selected, so use the place holders.

        m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
        m_vmsParameter.setVmsName( DEFAULT_VMS_NAME );
        m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );

        FUNCTION_EXIT;
        return;
    }

    // Set the parameter info from the selected sign.

    unsigned long signKey = m_signs.GetItemData( signIndex );
    if ( DEFAULT_VMS_KEY == signKey )
    {
        m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
        m_vmsParameter.setVmsName( DEFAULT_VMS_NAME );
    }
    else if ( CB_ERR == signKey )
    {
//        MessageBox::error( "Failed to identify the selected sign." );
        m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
        m_vmsParameter.setVmsName( DEFAULT_VMS_NAME );
        m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );
    }
    else
    {
        m_vmsParameter.setVmsKey( signKey );
        CString vmsName = DEFAULT_VMS_NAME.c_str();
        m_signs.GetLBText( signIndex, vmsName );
        m_vmsParameter.setVmsName( std::string( vmsName ) );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditSelectVmsPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_signs.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditSelectVmsPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    // Select the current VMS from the combo box.
    // Always select by the key not the name, because names
    // can change on the fly, but keys can't.

    unsigned long currentVmsKey = m_vmsParameter.getVmsKey();
    for ( int i = 0; i < m_signs.GetCount(); i++ )
    {
        if ( currentVmsKey == m_signs.GetItemData(i) )
        {
            if ( CB_ERR == m_signs.SetCurSel(i) )
            {
                break;
            }

            FUNCTION_EXIT;
            return;
        }
    }

    // If we get to here, we haven't found the VMS and we should have,
    // so display an error message and select the "<No VMS Selected>" item.

    std::wstring vmsName = m_vmsParameter.getVmsName();
    TA_Bus::TransActiveMessage userMsg;
    CString itemType;
    itemType.LoadString(IDS_VMS);
    userMsg << itemType;
    if ( vmsName.empty() )
    {
        CString itemName;
        itemName.LoadString(IDS_UNKNOWN);
        userMsg << itemName;
    }
    else
    {
        userMsg << vmsName;
    }
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_210075);
    m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );

    FUNCTION_EXIT;
}


void CPlanStepEditSelectVmsPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Have to put these out here o/w you get error C2360: "initialization of 'blankoutStep' is skipped by 'case' label".

    VmsBlankoutStep* blankoutStep = 0;
    VmsRestoreScheduleStep* restoreStep = 0;

    VmsStepParameter* vmsStepParameter = 0;

    switch ( m_stepType )
    {
        case TA_Base_Core::VMS_BLANKOUT_STEP:

            blankoutStep = dynamic_cast< VmsBlankoutStep* >( GetPlanStep() );
            TA_ASSERT( blankoutStep!=NULL, "Incorrect step type for the select VMS page" );  // KTTODO This should be an exception situation not an assert, but currently it wouldn't be handled.
            vmsStepParameter = blankoutStep->getVmsParameter();
            break;

        case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:

            restoreStep = dynamic_cast< VmsRestoreScheduleStep* >( GetPlanStep() );
            TA_ASSERT( restoreStep!=NULL, "Incorrect step type for the select VMS page" );  // KTTODO This should be an exception situation not an assert, but currently it wouldn't be handled.
            vmsStepParameter = restoreStep->getVmsParameter();
            break;

        default:

            TA_ASSERT( true, "Incorrect step type for the select VMS page" );
    }

    if ( 0 == vmsStepParameter )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update VMS parameters." );
//        MessageBox::error( "Failed to update VMS step parameters." );
        m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );

        FUNCTION_EXIT;
        return;     // KTTODO This should throw an exception... but would it be handled?
    }
    vmsStepParameter->setVmsKey( m_vmsParameter.getVmsKey() );
      vmsStepParameter->setVmsName( m_vmsParameter.getVmsName() );

    FUNCTION_EXIT;
}


void CPlanStepEditSelectVmsPage::PopulateVmsCombo()
{
    FUNCTION_ENTRY( "PopulateVmsCombo" );

    int index = 0;
    try
    {
        TA_Signs::SignObjectSeq signs = TA_Signs::VmsEnquirer::getInstance().getSigns();

        // Loop through the signs and add them to the combo box.

        for ( unsigned int i = 0; i < signs.length(); i++ )
        {
            // Place the name in the combo box.

            index = m_signs.AddString( signs[i].name );

            if ( CB_ERR == index )
            {
                // If an error occurred on the insert, attempt to remove the new entry,
                // and then move on to the next item.

                m_signs.DeleteString( m_signs.FindStringExact( 0, signs[i].name ) );
                continue;
            }
            else
            {
                // Place the entity key of the VMS in the ItemData.
                // NB Names for signs can change on the fly, entity keys cannot.

                m_signs.SetItemData( index, signs[i].entityKey );
            }
        }
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        MessageBox::error( IDS_LOAD_VMS, e );
    }

    // Add a string stating "<No VMS Selected>" to the top of the combo box.

    index = m_signs.InsertString( 0, DEFAULT_ITEM.c_str() );
    if ( CB_ERR == index )
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_signs.DeleteString( m_signs.FindStringExact( 0, DEFAULT_ITEM.c_str() ) );
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_VMS_KEY (easy to tell).

        m_signs.SetItemData( index, DEFAULT_VMS_KEY );
    }

    // Finally, select the "<No Scene Selected>" item.

    m_signs.SetCurSel( index );

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)
