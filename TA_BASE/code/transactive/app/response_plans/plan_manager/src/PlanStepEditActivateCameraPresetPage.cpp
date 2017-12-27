//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivateCameraPresetPage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Page for the ActivateCameraPreset step.

#include "stdafx.h"

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"
#include "planmanager.h"
#include "PlanStepEditActivateCameraPresetPage.h"
#include "ActivateCameraPresetStep.h"
#include "MessageBox.h"
#include "VideoEnquirer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const unsigned long DEFAULT_CAMERA_KEY = 0;
    const unsigned long DEFAULT_PRESET_KEY = 0;
    const std::string DEFAULT_PRESET_NAME = "";
    const std::string DEFAULT_PRESET_ITEM = "<No Preset Selected>";
}

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivateCameraPresetPage dialog


IMPLEMENT_DYNAMIC(CPlanStepEditActivateCameraPresetPage, CPlanStepEditParameterPage)

CPlanStepEditActivateCameraPresetPage::CPlanStepEditActivateCameraPresetPage(ActivateCameraPresetStep* step)
  : CPlanStepEditParameterPage(CPlanStepEditActivateCameraPresetPage::IDD, step),
    m_cameraParameter(*step->getCameraParameter()),
    m_presetParameter(*step->getPresetParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditActivateCameraPresetPage" );

    // {{AFX_DATA_INIT(CPlanStepEditActivateCameraPresetPage)
    m_cameraName = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditActivateCameraPresetPage::~CPlanStepEditActivateCameraPresetPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditActivateCameraPresetPage" );

    // Do nothing.

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditActivateCameraPresetPage)
    DDX_Control(pDX, IDC_PRESET_COMBO, m_presets);
    DDX_Text(pDX, IDC_CAMERA, m_cameraName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditActivateCameraPresetPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditActivateCameraPresetPage)
    ON_BN_CLICKED(IDC_PICK_CAMERA_BUTTON, OnPickCameraButton)
    ON_CBN_SELCHANGE(IDC_PRESET_COMBO, OnSelectionChangePresetCombo)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivateCameraPresetPage message handlers


void CPlanStepEditActivateCameraPresetPage::OnPickCameraButton()
{
    FUNCTION_ENTRY( "OnPickCameraButton" );

    // Select the required camera.

    m_cameraParameter.editParameter();
    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::OnSelectionChangePresetCombo()
{
    FUNCTION_ENTRY( "OnSelectionChangePresetCombo" );

    // Find the key of the selected preset.

    int presetIndex = m_presets.GetCurSel();
    if ( CB_ERR == presetIndex )
    {
        // There is no preset selected, so use the place holders.

        m_presetParameter.setPresetKey( DEFAULT_PRESET_KEY );
        m_presetParameter.setPresetName( DEFAULT_PRESET_NAME );
        m_presets.SelectString( 0, DEFAULT_PRESET_ITEM.c_str() );

        FUNCTION_EXIT;
        return;
    }

    // Set the parameter info from the selected preset.

    unsigned long presetKey = m_presets.GetItemData( presetIndex );
    if ( DEFAULT_PRESET_KEY == presetKey )
    {
        m_presetParameter.setPresetKey( DEFAULT_PRESET_KEY );
        m_presetParameter.setPresetName( DEFAULT_PRESET_NAME );
    }
    else if ( CB_ERR == presetKey )
    {
//        MessageBox::error( "Failed to identify the selected preset." );
        m_presetParameter.setPresetKey( DEFAULT_PRESET_KEY );
        m_presetParameter.setPresetName( DEFAULT_PRESET_NAME );
        m_presets.SelectString( 0, DEFAULT_PRESET_ITEM.c_str() );
    }
    else
    {
        m_presetParameter.setPresetKey( presetKey );
        CString presetName = DEFAULT_PRESET_NAME.c_str();
        m_presets.GetLBText( presetIndex, presetName );
        m_presetParameter.setPresetName( std::string( presetName ) );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* cameraButton = GetDlgItem(IDC_PICK_CAMERA_BUTTON);
    if (cameraButton)
    {
        cameraButton->EnableWindow(enable);
    }

    m_presets.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    m_cameraName = m_cameraParameter.getCameraName().c_str();
    UpdateData(false);

    PopulatePresetsCombo();

    // Select the current preset from the combo box.
    // Always select by the key not the name, because names
    // can change on the fly, but keys can't.

    unsigned long currentPresetKey = m_presetParameter.getPresetKey();
    for ( int i = 0; i < m_presets.GetCount(); i++ )
    {
        if ( currentPresetKey == m_presets.GetItemData(i) )
        {
            if ( CB_ERR == m_presets.SetCurSel(i) )
            {
                break;
            }

            FUNCTION_EXIT;
            return;
        }
    }

    // If we get to here, we haven't found the preset and we should have,
    // so display an error message and select the "<No Preset Selected>" item.

    std::stringstream error;
    // TODO: using narrow string at the moment, need to use wide string to display correctly
    std::string presetName = m_presetParameter.getPresetName();
    TA_Bus::TransActiveMessage userMsg;
    CString itemType;
    itemType.LoadString(IDS_PRESET);
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

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    ActivateCameraPresetStep* step = dynamic_cast<ActivateCameraPresetStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the activate camera preset step page");    // KTTODO This should be an exception, but at the moment it won't be handled.

    CameraStepParameter* cameraStepParameter = step->getCameraParameter();
    if ( 0 == cameraStepParameter )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update Activate Camera Preset parameters." );
        TA_Bus::TransActiveMessage userMsg;
        CString itemName;
        itemName.LoadString(IDS_CAMERA);
        userMsg << itemName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210076);

        FUNCTION_EXIT;
        return;     // KTTODO This should throw an exception... but would it be handled?
    }
    CameraPresetStepParameter* presetStepParameter = step->getPresetParameter();
    if ( 0 == presetStepParameter )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update Activate Camera Preset parameters." );
        TA_Bus::TransActiveMessage userMsg;
        CString itemName;
        itemName.LoadString(IDS_PRESET);
        userMsg << itemName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210076);

        FUNCTION_EXIT;
        return;     // KTTODO This should throw an exception... but would it be handled?
    }
    cameraStepParameter->setCameraKey(m_cameraParameter.getCameraKey());
    cameraStepParameter->setCameraName(m_cameraParameter.getCameraName());
    presetStepParameter->setPresetKey(m_presetParameter.getPresetKey());
    presetStepParameter->setPresetName(m_presetParameter.getPresetName());

    FUNCTION_EXIT;
}


void CPlanStepEditActivateCameraPresetPage::PopulatePresetsCombo()
{
    FUNCTION_ENTRY( "PopulatePresetsCombo" );

    unsigned long cameraEntityKey = m_cameraParameter.getCameraKey();

    // If there is no camera selected, don't load any presets and disable the combo box.

    if ( DEFAULT_CAMERA_KEY == cameraEntityKey )
    {
        m_presets.ResetContent();
        int index = m_presets.AddString( DEFAULT_PRESET_ITEM.c_str() );
        m_presets.SetCurSel( index );
        m_presets.EnableWindow( FALSE );

        FUNCTION_EXIT;
        return;
    }

    try
    {
        // Get the presets available for the selected Camera.

        VideoEnquirer enquirer;
        MovementControlCorbaDef::CameraPresetSequence* presets = enquirer.getPresets( cameraEntityKey );

        // Loop through the presets and add them to the combo box.

        m_presets.ResetContent();
        int index = 0;
        for ( unsigned int i = 0; i < presets->length(); i++ )
        {
            // Place the name in the combo box.

            index = m_presets.AddString( (*presets)[i].name );

            if ( CB_ERR == index )
            {
                // If an error occurred on the insert, attempt to remove the new entry,
                // and then move on to the next item.

                m_presets.DeleteString( m_presets.FindStringExact( 0, (*presets)[i].name ) );
                continue;
            }
            else
            {
                // Place the unique key of the preset in the ItemData.
                // NB Names for presets can change on the fly, keys cannot.

                m_presets.SetItemData( index, (*presets)[i].index );
            }
        }
        m_presets.EnableWindow( IsEditingEnabled() );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        TA_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210077);
        m_presets.EnableWindow( FALSE );
        // Allow this to continue on.
    }

    // Add a string stating "<No Preset Selected>" to the top of the combo box.

    int index = m_presets.InsertString( 0, DEFAULT_PRESET_ITEM.c_str() );
    if ( CB_ERR == index )
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_presets.DeleteString( m_presets.FindStringExact( 0, DEFAULT_PRESET_ITEM.c_str() ) );
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_VMS_KEY (easy to tell).

        m_presets.SetItemData( index, DEFAULT_PRESET_KEY );
    }

    // Finally, select the "<No Preset Selected>" item.

    m_presets.SetCurSel( index );

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)
