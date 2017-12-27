//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterCameraEditDlg.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Dialog for selecting a camera name for the Assign Camera To Monitor and Activate Camera Preset steps.

#include "stdafx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "PlanManager.h"
#include "PlanStepParameterCameraEditDlg.h"
#include "CameraStepParameter.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const std::string ERROR_MSG = "Failed to get the cameras from the VideoSwitchAgent.";
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterCameraEditDlg dialog

CPlanStepParameterCameraEditDlg::CPlanStepParameterCameraEditDlg(CameraStepParameter& parameter, bool canEdit)
    : CPlanStepParameterEditDlg(parameter, CPlanStepParameterCameraEditDlg::IDD, canEdit),
      m_cameraParameter( parameter )
{
    FUNCTION_ENTRY( "CPlanStepParameterCameraEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterCameraEditDlg)
    m_cameraName = _T("");
    m_parameterName = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterCameraEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterCameraEditDlg)
    DDX_Control(pDX, IDOK, m_okButton);
    DDX_Control(pDX, IDC_CAMERA_TREE, m_cameraTree);
    DDX_Text(pDX, IDC_CAMERA_NAME, m_cameraName);
    DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterCameraEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterCameraEditDlg)
    ON_NOTIFY(TVN_SELCHANGED, IDC_CAMERA_TREE, onSelectionChangedCameraTree)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterCameraEditDlg message handlers


BOOL CPlanStepParameterCameraEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    CWaitCursor waitCursor;

    m_parameterName = m_cameraParameter.getName().c_str();
    m_cameraName = m_cameraParameter.getCameraName().c_str();
    UpdateData( FALSE );
    m_cameraKey = m_cameraParameter.getCameraKey();

    // Disable the OK button until a camera has been selected.

    if ( m_cameraName.IsEmpty() )
    {
        m_okButton.EnableWindow( FALSE );
    }

       loadCameraTree();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterCameraEditDlg::loadCameraTree()
{
    FUNCTION_ENTRY( "loadCameraTree" );

    TA_Base_Bus::VideoSwitchAgentFactory::CameraVector cameras;
    try
    {
        cameras = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCameras();
    }
    catch( const TA_Base_Core::ObjectResolutionException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        CString actionName, agent;
        actionName.LoadString(IDS_LOAD_CAMERAS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const TA_Base_Bus::VideoSwitchAgentException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        // santTODO: change to appropriate message
        CString actionName, agent;
        actionName.LoadString(IDS_LOAD_CAMERAS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const CORBA::Exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }

    // Go through the vector and insert the cameras into the tree under their group names.

    std::string groupName = "";
    std::string cameraName = "";
    unsigned long cameraKey = 0;
    HTREEITEM* groupRootItem = 0;
    std::map< std::string, HTREEITEM* > rootItems;              // Map of group name to corresponding root item.
    std::map< std::string, HTREEITEM* >::iterator rootItemsIt;
    int failures = 0;

    // This will be set to the item for the camera that is currently stored in the CameraStepParameter.
    // It will be identified using m_initialCameraKey and it will be automatically selected.

    HTREEITEM* initialCameraItem = 0;

    TA_Base_Bus::VideoSwitchAgentFactory::CameraVector::iterator it;
    for( it = cameras.begin(); it != cameras.end(); ++it )
    {
        // Get the group for the current camera.
        // If it cannot be determined, assume this camera is part a root node.

        try
        {
            CORBA_CALL_RETURN( groupName, ( *( *it ) ), getVideoInputGroupName, () );//limin
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            groupName = "";
        }

        // Now get the camera's name and key.

        try
        {
            CORBA_CALL_RETURN( cameraName, ( *( *it ) ), getName, () );//limin
            CORBA_CALL_RETURN( cameraKey, ( *( *it ) ), getKey, () );//limin
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get camera details. Camera will not be added to tree." );
            failures++;
            continue;
        }

        if ( cameraName.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get camera details. Camera will not be added to tree." );
            failures++;
            continue;
        }
        else if ( 0 == cameraKey )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get camera details. Camera will not be added to tree." );
            failures++;
            continue;
        }

        // If groupName is not empty, find or insert the appropriate root item.
        // groupRootItem is set to TVI_ROOT in the event that we cannot determine the
        // camera's group name.  This means the camera will be inserted as a root item.
        // However they will not be added to the rootItems map, because we don't want
        // other camera's put under them in the tree.

        groupRootItem = 0;
        if ( !groupName.empty() )
        {
            rootItemsIt = rootItems.find( groupName );
            if ( rootItems.end() != rootItemsIt )
            {
                groupRootItem = rootItemsIt->second;    // We found it.
            }
            else
            {
                // The group isn't already in the tree (or the map), so insert it as a new root item.
                // Also add it to the map of root items.

                HTREEITEM rootItem = m_cameraTree.InsertItem( groupName.c_str(), 0, 0, TVI_ROOT, TVI_LAST );
                if ( NULL != rootItem )
                {
                    groupRootItem = &rootItem;
                    rootItems[ groupName ] = groupRootItem;
                }
            }
        }

        // Insert the camera's name into the tree and put the entity key in as the item data.
        // If the camera doesn't have a group or if we couldn't figure out the group, just
        // insert it as a root node.

        HTREEITEM cameraItem;
        if ( 0 == groupRootItem )
        {
            cameraItem = m_cameraTree.InsertItem( cameraName.c_str(), 2, 2, TVI_ROOT, TVI_LAST );
        }
        else
        {
            cameraItem = m_cameraTree.InsertItem( cameraName.c_str(), 2 ,2, *groupRootItem, TVI_LAST );
        }

        if ( NULL == cameraItem )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert camera details: %s", cameraName.c_str() );
            failures++;
        }
        else
        {
            m_cameraTree.SetItemData( cameraItem, cameraKey );

            // Do this in here because it is faster than trying to find the item again.

            if ( cameraKey == m_cameraKey )
            {
                initialCameraItem = &cameraItem;
            }
        }
    }

    // Sort the whole tree.

    m_cameraTree.SortChildren( NULL );

    if ( 0 != failures )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        CString itemName, agentName;
        itemName.LoadString(IDS_CAMERA);
        agentName.LoadString(IDS_VIDEO_SWITCH_AGENT);
        userMsg << itemName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
    }

    // When the bit below this works, we can take this bit out...

    if ( m_cameraName.IsEmpty() )
    {
        m_okButton.EnableWindow( FALSE );
    }
    else
    {
        m_okButton.EnableWindow( TRUE );
    }

/* KTTODO THIS DOESN'T WORK AT THE MOMENT.

    // Select the current camera (if there is one and we found it).
    // It's faster to do it here while we already know the item.

    if ( 0 != m_cameraKey )
    {
        if ( ( 0 == initialCameraItem ) || ( 0 == m_cameraTree.SelectSetFirstVisible( *initialCameraItem ) ) )
        {
            std::stringstream error;
            if ( m_cameraName.IsEmpty() )
            {
                error << "Failed to select current Camera (name unknown).";
            }
            else
            {
                error << "Failed to select current Camera " << m_cameraName << ".";
            }
            MessageBox::error( error.str().c_str() );
        }
        else
        {
            // Now we've selected a camera, so enable the OK button.

            m_cameraTree.EnsureVisible( *initialCameraItem );
            m_okButton.EnableWindow( TRUE );
        }
    }
*/

    // Finally delete all the named obj refs, because we don't need them anymore.

    for ( it = cameras.begin(); it != cameras.end(); it++ )
    {
        delete *it;
        *it = 0;
    }

    FUNCTION_EXIT;
}


void CPlanStepParameterCameraEditDlg::onSelectionChangedCameraTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onSelectionChangedCameraTree" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get currently selected item.

    HTREEITEM item = m_cameraTree.GetSelectedItem();
    if ( NULL == item )
    {
//        MessageBox::error( "Failed to identify selected camera." );
        m_cameraName = "";
    }
    else
    {
        // If the key is not zero, then we have a camera, otherwise it is a group name.

        unsigned long itemKey =  m_cameraTree.GetItemData( item );
        if ( 0 != itemKey )
        {
            m_cameraName = m_cameraTree.GetItemText( item );
            m_cameraKey = itemKey;

            // Now we've selected a camera, so enable the OK button.

            m_okButton.EnableWindow( TRUE );
        }
    }
    UpdateData( FALSE );

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterCameraEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    m_cameraParameter.setCameraName( std::string( m_cameraName ) );
    m_cameraParameter.setCameraKey( m_cameraKey );
    m_cameraTree.DeleteAllItems();

    CPlanStepParameterEditDlg::OnOK();

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)
