//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditVmsSetScenePage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#include "stdafx.h"

#if defined (STEPTYPE_VMS_SET_SCENE)

#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/signs/vms_agent/idl/src/SignControlCorbaDef.h"
#include "PlanManager.h"
#include "PlanStepEditVmsSetScenePage.h"
#include "VmsSetSceneStep.h"
#include "VmsEnquirer.h"
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
    const unsigned long DEFAULT_SCENE_KEY = 0;
    const std::string DEFAULT_NAME = "";
    const std::string DEFAULT_ITEM = "<No VMS Selected>";
    const std::string DEFAULT_SCENE_ITEM = "<No Scene Selected>";
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditVmsSetScenePage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditVmsSetScenePage, CPlanStepEditParameterPage)

CPlanStepEditVmsSetScenePage::CPlanStepEditVmsSetScenePage(VmsSetSceneStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditVmsSetScenePage::IDD, step),
      m_vmsParameter(*step->getVmsParameter()),
      m_vmsSceneParameter(*step->getVmsSceneParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditVmsSetScenePage" );

    // {{AFX_DATA_INIT(CPlanStepEditVmsSetScenePage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditVmsSetScenePage)
    DDX_Control(pDX, IDC_SIGN_LIST, m_signs);
    DDX_Control(pDX, IDC_SCENE_LIST, m_scenes);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditVmsSetScenePage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditVmsSetScenePage)
    ON_CBN_SELCHANGE(IDC_SIGN_LIST, OnSelectionChangeSignList)
    ON_CBN_SELCHANGE(IDC_SCENE_LIST, OnSelectionChangeSceneList)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditVmsSetScenePage message handlers

BOOL CPlanStepEditVmsSetScenePage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    PopulateVmsCombo();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditVmsSetScenePage::OnSelectionChangeSignList()
{
    FUNCTION_ENTRY( "OnSelectionChangeSignList" );

    // Find the key of the selected sign.

    int signIndex = m_signs.GetCurSel();
    if ( CB_ERR == signIndex )
    {
        // There is no sign selected, so use the place holders.

        m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
        m_vmsParameter.setVmsName( DEFAULT_NAME );
        m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );
    }
    else
    {
        // Set the parameter info from the selected sign.

        unsigned long signKey = m_signs.GetItemData( signIndex );
        if ( DEFAULT_VMS_KEY == signKey )
        {
            m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
            m_vmsParameter.setVmsName( DEFAULT_NAME );
        }
        else if ( CB_ERR == signKey )
        {
            TA_Bus::TransActiveMessage userMsg;
            CString itemType;
            itemType.LoadString(IDS_VMS_SIGN);
            userMsg << itemType;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_210085);
            m_vmsParameter.setVmsKey( DEFAULT_VMS_KEY );
            m_vmsParameter.setVmsName( DEFAULT_NAME );
            m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );
        }
        else
        {
            m_vmsParameter.setVmsKey( signKey );
            CString vmsName = "";
            m_signs.GetLBText( signIndex, vmsName );
            m_vmsParameter.setVmsName( std::string( vmsName ) );
        }
    }

    m_vmsSceneParameter.setVmsSceneKey( DEFAULT_SCENE_KEY );
    m_vmsSceneParameter.setVmsSceneName( DEFAULT_NAME );

    // Repopulate the scenes list.

    PopulateVmsSceneCombo( m_vmsParameter.getVmsKey() );

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::OnSelectionChangeSceneList()
{
    FUNCTION_ENTRY( "OnSelectionChangeSceneList" );

    // Find the key of the selected scene.

    int sceneIndex = m_scenes.GetCurSel();
    if ( CB_ERR == sceneIndex )
    {
        // There is no scene selected, so use the place holders.

        m_vmsSceneParameter.setVmsSceneKey( DEFAULT_SCENE_KEY );
        m_vmsSceneParameter.setVmsSceneName( DEFAULT_NAME );
        m_scenes.SelectString( 0, DEFAULT_SCENE_ITEM.c_str() );
    }
    else
    {
        // Set the parameter info from the selected scene.

        unsigned long sceneKey = m_scenes.GetItemData( sceneIndex );
        if ( DEFAULT_SCENE_KEY == sceneKey )
        {
            m_vmsSceneParameter.setVmsSceneKey( DEFAULT_SCENE_KEY );
            m_vmsSceneParameter.setVmsSceneName( DEFAULT_NAME );
        }
        else if ( CB_ERR == sceneKey )
        {
            TA_Bus::TransActiveMessage userMsg;
            CString itemType;
            itemType.LoadString(IDS_VMS_SCENE);
            userMsg << itemType;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_210085);
            m_vmsSceneParameter.setVmsSceneKey( DEFAULT_SCENE_KEY );
            m_vmsSceneParameter.setVmsSceneName( DEFAULT_NAME );
            m_scenes.SelectString( 0, DEFAULT_ITEM.c_str() );
        }
        else
        {
            m_vmsSceneParameter.setVmsSceneKey( sceneKey );
            CString vmsSceneName = "";
            m_scenes.GetLBText( sceneIndex, vmsSceneName );
            m_vmsSceneParameter.setVmsSceneName( std::string( vmsSceneName ) );
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_signs.EnableWindow(enable);
    m_scenes.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    // Get the key of the currently selected VMS for this step.

    unsigned long currentVmsKey = m_vmsParameter.getVmsKey();
    if ( DEFAULT_VMS_KEY == currentVmsKey )
    {
        m_signs.SelectString( 0, DEFAULT_ITEM.c_str() );
    }
    else
    {
        // Select the current VMS from the combo box.

        for ( int i = 0; i < m_signs.GetCount(); i++ )
        {
            if ( currentVmsKey == m_signs.GetItemData(i) )
            {
                m_signs.SetCurSel(i);
                break;
            }
        }

        // If we can't find the sign in the list, display an error message.

        if ( i == m_signs.GetCount() )
        {
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
            currentVmsKey = DEFAULT_VMS_KEY;
        }
    }

    // Find the scene key.

    PopulateVmsSceneCombo( currentVmsKey );

    unsigned long currentVmsSceneKey = m_vmsSceneParameter.getVmsSceneKey();
    if ( DEFAULT_SCENE_KEY == currentVmsSceneKey )
    {
        m_scenes.SelectString( 0, DEFAULT_SCENE_ITEM.c_str() );

        FUNCTION_EXIT;
        return;     // Do nothing if there is no current VMS to select.
    }

    // Select the current scene from the combo box.

    for ( int i = 0; i < m_scenes.GetCount(); i++ )
    {
        if ( currentVmsSceneKey == m_scenes.GetItemData(i) )
        {
            m_scenes.SetCurSel(i);

            FUNCTION_EXIT;
            return;
        }
    }

    // If we can't find the scene in the list, display an error message.

    if ( i == m_scenes.GetCount() )
    {
        TA_Bus::TransActiveMessage userMsg;
        CString itemType;
        itemType.LoadString(IDS_VMS_SCENE);
        userMsg << itemType;
        if ( presetName.empty() )
        {
            CString preset;
            preset.LoadString(IDS_UNKNOWN);
            userMsg << preset;
        }
        else
        {
            userMsg << presetName;
        }

        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210075);
        m_signs.SelectString( 0, DEFAULT_SCENE_ITEM.c_str() );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    VmsSetSceneStep* vmsSetSceneStep = dynamic_cast< VmsSetSceneStep* >( GetPlanStep() );
    TA_ASSERT( vmsSetSceneStep!=NULL, "Incorrect step type for the VMS set scene page" );
    VmsStepParameter* vmsStepParameter = vmsSetSceneStep->getVmsParameter();
    TA_ASSERT( vmsStepParameter!=NULL, "Invalid VMS step parameter for the VMS set scene step" );
    vmsStepParameter->setVmsKey( m_vmsParameter.getVmsKey() );
      vmsStepParameter->setVmsName( m_vmsParameter.getVmsName() );

    VmsSceneStepParameter* vmsSceneStepParameter = vmsSetSceneStep->getVmsSceneParameter();
    TA_ASSERT( vmsSceneStepParameter!=NULL, "Invalid VMS scene step parameter for the VMS set scene step" );
    vmsSceneStepParameter->setVmsSceneKey( m_vmsSceneParameter.getVmsSceneKey() );
      vmsSceneStepParameter->setVmsSceneName( m_vmsSceneParameter.getVmsSceneName() );

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::PopulateVmsCombo()
{
    FUNCTION_ENTRY( "PopulateVmsCombo" );

    try
    {
        TA_Signs::SignObjectSeq signs = TA_Signs::VmsEnquirer::getInstance().getSigns();

        // Loop through the signs and add them to the combo box.

        int index = 0;
        for (unsigned int i = 0;i < signs.length(); i++)
        {
            // Place the name in the combo box.

            index = m_signs.AddString(signs[i].name);

            if (index == CB_ERR)
            {
                // If an error occurred on the insert, attempt to remove the new entry,
                // and then move on to the next item.

                m_signs.DeleteString(m_signs.FindStringExact(0,signs[i].name));
                continue;
            }
            else
            {
                // Place the entity key of the VMS in the ItemData.
                // NB Names for signs can change on the fly, entity keys cannot.

                m_signs.SetItemData(index,signs[i].entityKey);
            }
        }
    }
    catch( TA_Core::TransactiveException& e )
    {
        TA_Bus::TransActiveMessage userMsg;
        CString itemName, agentName;
        itemName.LoadString(IDS_VMS);
        agentName.LoadString(IDS_VMS_AGENT);
        userMsg << itemName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
    }

    // Add a string stating "<No VMS Selected>" to the top of the combo box.

    int index = m_signs.InsertString(0,DEFAULT_ITEM.c_str());
    if (index == CB_ERR)
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_signs.DeleteString(m_signs.FindStringExact(0,DEFAULT_ITEM.c_str()));
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_VMS_KEY (easy to tell).

        m_signs.SetItemData(index,DEFAULT_VMS_KEY);
    }

    // Finally, select the "<All>" item.

    m_signs.SetCurSel(index);

    FUNCTION_EXIT;
}


void CPlanStepEditVmsSetScenePage::PopulateVmsSceneCombo( unsigned long vmsEntityKey )
{
    FUNCTION_ENTRY( "PopulateVmsSceneCombo" );

    // If there is no sign selected, don't load any scenes and disable the combo box.

    if ( DEFAULT_VMS_KEY == vmsEntityKey )
    {
        m_scenes.ResetContent();
        int index = m_scenes.AddString( DEFAULT_SCENE_ITEM.c_str() );
        m_scenes.SetCurSel( index );
        m_scenes.EnableWindow( FALSE );

        FUNCTION_EXIT;
        return;
    }

    try
    {
        // Get the scenes available for the selected VMS.

        TA_Signs::SignSpecificObjectSeq scenes = TA_Signs::VmsEnquirer::getInstance().getScenesForSign( vmsEntityKey );

        // Loop through the scenes and add them to the combo box.

        m_scenes.ResetContent();
        int index = 0;
        for ( unsigned int i = 0; i < scenes.length(); i++ )
        {
            // Place the name in the combo box.

            index = m_scenes.AddString( scenes[i].name );

            if ( CB_ERR == index )
            {
                // If an error occurred on the insert, attempt to remove the new entry,
                // and then move on to the next item.

                m_scenes.DeleteString( m_scenes.FindStringExact( 0, scenes[i].name ) );
                continue;
            }
            else
            {
                // Place the unique key of the scene in the ItemData.
                // NB Names for scenes can change on the fly, keys cannot.

                m_scenes.SetItemData( index, scenes[i].id );
            }
        }
        m_scenes.EnableWindow( TRUE );
    }
    catch( TA_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        TA_Bus::TransActiveMessage userMsg;
        CString itemName, agentName;
        itemName.LoadString(IDS_VMS_SCENES);
        agentName.LoadString(IDS_VMS_AGENT);
        userMsg << itemName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
        m_scenes.EnableWindow( FALSE );
        // Allow this to continue on.
    }

    // Add a string stating "<No Scene Selected>" to the top of the combo box.

    int index = m_scenes.InsertString( 0, DEFAULT_SCENE_ITEM.c_str() );
    if ( CB_ERR == index )
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_scenes.DeleteString( m_scenes.FindStringExact( 0, DEFAULT_SCENE_ITEM.c_str() ) );
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_VMS_KEY (easy to tell).

        m_scenes.SetItemData( index, DEFAULT_SCENE_KEY );
    }

    // Finally, select the "<No Scene Selected>" item.

    m_scenes.SetCurSel( index );

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_VMS_SET_SCENE)
