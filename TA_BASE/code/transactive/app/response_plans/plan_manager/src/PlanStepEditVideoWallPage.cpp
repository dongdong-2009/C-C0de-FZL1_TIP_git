//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditVideoWallPage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class handles the dialog interaction for aswallboarding a layout to a wallboard on the VideoWall.

#include "stdafx.h"

#if defined (STEPTYPE_VIDEOWALL)

#include <sstream>
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "app/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "app/cctv/video_wall_agent/IDL/src/IVideoWallAgentCorbaDef.h"
#include "PlanManager.h"
#include "PlanStepEditVideoWallPage.h"
#include "VideoWallStep.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const unsigned long DEFAULT_WALLBOARD_KEY = 0;
    const unsigned long DEFAULT_LAYOUT_KEY = 0;
    const std::string DEFAULT_NAME = "";
    const std::string DEFAULT_WALLBOARD_ITEM = "<No Wallboard Selected>";
    const std::string DEFAULT_LAYOUT_ITEM = "<No Layout Selected>";
    const std::string ERROR_MSG = "Failed to get the wallboards from the VideoSwitchAgent.";
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditVideoWallPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditVideoWallPage, CPlanStepEditParameterPage)

CPlanStepEditVideoWallPage::CPlanStepEditVideoWallPage(VideoWallStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditVideoWallPage::IDD, step),
      m_wallboardParameter(*step->getWallboardParameter()),
      m_layoutParameter(*step->getLayoutParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditVideoWallPage" );

    // {{AFX_DATA_INIT(CPlanStepEditVideoWallPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditVideoWallPage)
    DDX_Control(pDX, IDC_WALLBOARD_LIST, m_wallboards);
    DDX_Control(pDX, IDC_LAYOUT_LIST, m_layouts);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditVideoWallPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditVideoWallPage)
    ON_CBN_SELCHANGE(IDC_WALLBOARD_LIST, OnSelectionChangeWallboardList)
    ON_CBN_SELCHANGE(IDC_LAYOUT_LIST, OnSelectionChangeLayoutList)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditVideoWallPage message handlers

BOOL CPlanStepEditVideoWallPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    CWaitCursor waitCursor;

    PopulateWallboardCombo();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditVideoWallPage::OnSelectionChangeWallboardList()
{
    FUNCTION_ENTRY( "OnSelectionChangeWallboardList" );

    // Find the key of the selected wallboard.

    int wallboardIndex = m_wallboards.GetCurSel();
    if ( CB_ERR == wallboardIndex )
    {
        // There is no wallboard selected, so use the place holders.

        m_wallboardParameter.setWallboardKey( DEFAULT_WALLBOARD_KEY );
        m_wallboardParameter.setWallboardName( DEFAULT_NAME );
        m_wallboards.SelectString( 0, DEFAULT_WALLBOARD_ITEM.c_str() );
    }
    else
    {
        // Set the parameter info from the selected wallboard.

        unsigned long wallboardKey = m_wallboards.GetItemData( wallboardIndex );
        if ( DEFAULT_WALLBOARD_KEY == wallboardKey )
        {
            m_wallboardParameter.setWallboardKey( DEFAULT_WALLBOARD_KEY );
            m_wallboardParameter.setWallboardName( DEFAULT_NAME );
        }
        else if ( CB_ERR == wallboardKey )
        {
//            MessageBox::error( "Failed to identify of the selected wallboard." );
            m_wallboardParameter.setWallboardKey( DEFAULT_WALLBOARD_KEY );
            m_wallboardParameter.setWallboardName( DEFAULT_NAME );
            m_wallboards.SelectString( 0, DEFAULT_WALLBOARD_ITEM.c_str() );
        }
        else
        {
            m_wallboardParameter.setWallboardKey( wallboardKey );
            CString wallboardName = "";
            m_wallboards.GetLBText( wallboardIndex, wallboardName );
            m_wallboardParameter.setWallboardName( std::string( wallboardName ) );
        }
    }

    m_layoutParameter.setLayoutKey( DEFAULT_LAYOUT_KEY );
    m_layoutParameter.setLayoutName( DEFAULT_NAME );

    // Repopulate the layouts list.

    PopulateLayoutCombo( m_wallboardParameter.getWallboardKey() );

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::OnSelectionChangeLayoutList()
{
    FUNCTION_ENTRY( "OnSelectionChangeLayoutList" );

    // Find the key of the selected layout.

    int layoutIndex = m_layouts.GetCurSel();
    if ( CB_ERR == layoutIndex )
    {
        // There is no layout selected, so use the place holders.

        m_layoutParameter.setLayoutKey( DEFAULT_LAYOUT_KEY );
        m_layoutParameter.setLayoutName( DEFAULT_NAME );
        m_layouts.SelectString( 0, DEFAULT_LAYOUT_ITEM.c_str() );
    }
    else
    {
        // Set the parameter info from the selected layout.

        unsigned long layoutKey = m_layouts.GetItemData( layoutIndex );
        if ( DEFAULT_LAYOUT_KEY == layoutKey )
        {
            m_layoutParameter.setLayoutKey( DEFAULT_LAYOUT_KEY );
            m_layoutParameter.setLayoutName( DEFAULT_NAME );
        }
        else if ( CB_ERR == layoutKey )
        {
//            MessageBox::error( "Failed to identify the selected layout." );
            m_layoutParameter.setLayoutKey( DEFAULT_LAYOUT_KEY );
            m_layoutParameter.setLayoutName( DEFAULT_NAME );
            m_layouts.SelectString( 0, DEFAULT_WALLBOARD_ITEM.c_str() );
        }
        else
        {
            m_layoutParameter.setLayoutKey( layoutKey );
            CString layoutName = "";
            m_layouts.GetLBText( layoutIndex, layoutName );
            m_layoutParameter.setLayoutName( std::string( layoutName ) );
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_wallboards.EnableWindow(enable);
    m_layouts.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    // Get the key of the currently selected wallboard for this step.

    unsigned long currentWallboardKey = m_wallboardParameter.getWallboardKey();
    if ( DEFAULT_WALLBOARD_KEY == currentWallboardKey )
    {
        m_wallboards.SelectString( 0, DEFAULT_WALLBOARD_ITEM.c_str() );
    }
    else
    {
        // Select the current wallboard from the combo box.

        for ( int i = 0; i < m_wallboards.GetCount(); i++ )
        {
            if ( currentWallboardKey == m_wallboards.GetItemData(i) )
            {
                m_wallboards.SetCurSel(i);
                break;
            }
        }

        // If we can't find the wallboard in the list, display an error message.

        if ( i == m_wallboards.GetCount() )
        {
            // We won't be able to find the wallboard unless they have been loaded.
            // If the loading failed, there will already have been a message, so
            // only show a message if at least some wallboards loaded.
            // There will always be one entry for the default item.

            if ( i > 1 )
            {
                std::stringstream error;
                std::string wallboardName = m_wallboardParameter.getWallboardName();
                TA_Bus::TransActiveMessage userMsg;
                CString itemType;
                itemType.LoadString(IDS_WALLBOARD);
                userMsg << itemType;
                if ( wallboardName.empty() )
                {
                    CString itemName;
                    itemName.LoadString(IDS_UNKNOWN);
                    userMsg << itemName;
                }
                else
                {
                    userMsg << wallboardName;
                }
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_210075);
            }
            m_wallboards.SelectString( 0, DEFAULT_WALLBOARD_ITEM.c_str() );
            currentWallboardKey = DEFAULT_WALLBOARD_KEY;
        }
    }

    // Find the layout key.

    PopulateLayoutCombo( currentWallboardKey );

    unsigned long currentLayoutKey = m_layoutParameter.getLayoutKey();
    if ( DEFAULT_LAYOUT_KEY == currentLayoutKey )
    {
        m_layouts.SelectString( 0, DEFAULT_LAYOUT_ITEM.c_str() );

        FUNCTION_EXIT;
        return;     // Do nothing if there is no current wallboard to select.
    }

    // Select the current layout from the combo box.

    for ( int i = 0; i < m_layouts.GetCount(); i++ )
    {
        if ( currentLayoutKey == m_layouts.GetItemData(i) )
        {
            m_layouts.SetCurSel(i);

            FUNCTION_EXIT;
            return;
        }
    }

    // If we can't find the layout in the list, display an error message.

    if ( i == m_layouts.GetCount() )
    {
        // We won't be able to find the layout unless they have been loaded.
        // If the loading failed, there will already have been a message, so
        // only show a message if at least some layouts loaded.
        // There will always be one entry for the default item.

        if ( i > 1 )
        {
            std::string layoutName = m_layoutParameter.getLayoutName();
            TA_Bus::TransActiveMessage userMsg;
            CString itemType;
            itemType.LoadString(IDS_WALLBOARD_LAYOUT);
            userMsg << itemType;
            if ( layoutName.empty() )
            {
                CString itemName;
                itemName.LoadString(IDS_UNKNOWN);
                userMsg << itemName;
            }
            else
            {
                userMsg << layoutName;
            }
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_210075);
        }
        m_wallboards.SelectString( 0, DEFAULT_LAYOUT_ITEM.c_str() );
    }

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    VideoWallStep* wallboardSetLayoutStep = dynamic_cast< VideoWallStep* >( GetPlanStep() );
    TA_ASSERT( wallboardSetLayoutStep!=NULL, "Incorrect step type for the wallboard set layout page" );
    WallboardStepParameter* wallboardStepParameter = wallboardSetLayoutStep->getWallboardParameter();
    TA_ASSERT( wallboardStepParameter!=NULL, "Invalid wallboard step parameter for the wallboard set layout step" );
    wallboardStepParameter->setWallboardKey( m_wallboardParameter.getWallboardKey() );
      wallboardStepParameter->setWallboardName( m_wallboardParameter.getWallboardName() );

    WallboardLayoutStepParameter* layoutStepParameter = wallboardSetLayoutStep->getLayoutParameter();
    TA_ASSERT( layoutStepParameter!=NULL, "Invalid wallboard layout step parameter for the wallboard set layout step" );
    layoutStepParameter->setLayoutKey( m_layoutParameter.getLayoutKey() );
      layoutStepParameter->setLayoutName( m_layoutParameter.getLayoutName() );

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::PopulateWallboardCombo()
{
    FUNCTION_ENTRY( "PopulateWallboardCombo" );

    try
    {
        NameToKeyMap wallboards = GetWallboardDetails();
        if ( wallboards.empty() )
        {
            CString itemName, agentName;
            itemName.LoadString(IDS_WALLBOARD);
            agentName.LoadString(IDS_VIDEO_SWITCH_AGENT);
            TA_Bus::TransActiveMessage userMsg;
            userMsg << itemName << agentName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
            // Continue so that the default item is put in the combo box.
        }
        else
        {
            // Loop through the wallboards and add them to the combo box.

            int index = 0;
            NameToKeyMap::iterator it;
            for ( it = wallboards.begin(); it != wallboards.end(); ++it )
            {
                // Place the name in the combo box.

                index = m_wallboards.AddString( it->first.c_str() );

                if (index == CB_ERR)
                {
                    // If an error occurred on the insert, attempt to remove the new entry,
                    // and then move on to the next item.

                    m_wallboards.DeleteString(m_wallboards.FindStringExact(0,it->first.c_str()));
                    continue;
                }
                else
                {
                    // Place the entity key of the wallboard in the ItemData.
                    // NB Names for wallboards can change on the fly, entity keys cannot.

                    m_wallboards.SetItemData(index,it->second);
                }
            }
        }
    }
    catch( TA_Base_Core::TransactiveException&)
    {
        TA_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210080);
    }

    // Add a string stating "<No Wallboard Selected>" to the top of the combo box.

    int index = m_wallboards.InsertString(0,DEFAULT_WALLBOARD_ITEM.c_str());
    if (index == CB_ERR)
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_wallboards.DeleteString(m_wallboards.FindStringExact(0,DEFAULT_WALLBOARD_ITEM.c_str()));
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_WALLBOARD_KEY (easy to tell).

        m_wallboards.SetItemData(index,DEFAULT_WALLBOARD_KEY);
    }

    // Finally, select the "<All>" item.

    m_wallboards.SetCurSel(index);

    FUNCTION_EXIT;
}


void CPlanStepEditVideoWallPage::PopulateLayoutCombo( unsigned long wallboardEntityKey )
{
    FUNCTION_ENTRY( "PopulateLayoutCombo" );

    // If there is no wallboard selected, don't load any layouts and disable the combo box.

    if ( DEFAULT_WALLBOARD_KEY == wallboardEntityKey )
    {
        m_layouts.ResetContent();
        int index = m_layouts.AddString( DEFAULT_LAYOUT_ITEM.c_str() );
        m_layouts.SetCurSel( index );
        m_layouts.EnableWindow( FALSE );

        FUNCTION_EXIT;
        return;
    }

    try
    {
        // Get the layouts available for the selected wallboard.

        NameToKeyMap layouts = GetLayoutsForWallboard();

        // Loop through the layouts and add them to the combo box.

        m_layouts.ResetContent();

        if ( !layouts.empty() )
        {
            int index = 0;
            NameToKeyMap::iterator it;
            for ( it = layouts.begin(); it != layouts.end(); ++it )
            {
                // Place the name in the combo box.

                index = m_layouts.AddString( it->first.c_str() );

                if ( CB_ERR == index )
                {
                    // If an error occurred on the insert, attempt to remove the new entry,
                    // and then move on to the next item.

                    m_layouts.DeleteString( m_layouts.FindStringExact( 0, it->first.c_str() ) );
                    continue;
                }
                else
                {
                    // Place the unique key of the layout in the ItemData.
                    // NB Names for layouts can change on the fly, keys cannot.

                    m_layouts.SetItemData( index, it->second );
                }
            }
            m_layouts.EnableWindow( TRUE );
        }
        else
        {
            m_layouts.EnableWindow( FALSE );
            // Continue so that the default item is put in the combo box.
        }
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        TA_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210081);
        m_layouts.EnableWindow( FALSE );
        // Allow this to continue on.
    }

    // Add a string stating "<No Layout Selected>" to the top of the combo box.

    int index = m_layouts.InsertString( 0, DEFAULT_LAYOUT_ITEM.c_str() );
    if ( CB_ERR == index )
    {
        // If an error occurred on the insert, attempt to remove the new entry,
        // and then move on to the next item.

        m_layouts.DeleteString( m_layouts.FindStringExact( 0, DEFAULT_LAYOUT_ITEM.c_str() ) );
    }
    else
    {
        // As all "valid" item data will be unsigned longs (they're database keys), set this to DEFAULT_WALLBOARD_KEY (easy to tell).

        m_layouts.SetItemData( index, DEFAULT_LAYOUT_KEY );
    }

    // Finally, select the "<No Layout Selected>" item.

    m_layouts.SetCurSel( index );

    FUNCTION_EXIT;
}


CPlanStepEditVideoWallPage::NameToKeyMap CPlanStepEditVideoWallPage::GetWallboardDetails()
{
    FUNCTION_ENTRY( "GetWallboardDetails" );

    NameToKeyMap wallboards;
    VideoSwitchAgentFactory::VideoOutputGroupVector groups;
    VideoSwitchAgentFactory::VideoOutputGroupVector::iterator it;
    try
    {
        groups = TA_Base_App::VideoSwitchAgentFactory::getInstance().getVideoOutputGroups();

        // Insert all the wallboards into a map.  They can be identified from other groups because
        // they are capable of layouts.

        int failures = 0;
        std::string name = "(name unknown)";
        unsigned long entityKey = 0;
        for( it = groups.begin(); it != groups.end(); it++ )
        {
            try
            {
                CORBA_CALL_RETURN( name, ( *( *it ) ), getName, () );//limin
                CORBA_CALL_RETURN( entityKey, ( *( *it ) ), getKey, () );//limin
                CORBA::Boolean ret;
                CORBA_CALL_RETURN( ret, ( *( *it ) ), isCapableOfLayouts, () );//limin
                if( !ret )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Group is not capable of layouts, so it not a wallboard: %s", name.c_str() );
                    continue;
                }
                
                CORBA_CALL_RETURN( m_agents[ entityKey ], ( *( *it ) ), getVideoWallboardAgentName, () );
                wallboards[ name ] = entityKey;
                name = "(name unknown)";
            }
            catch( const TA_Base_App::VideoSwitchAgentException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
                LOGMORE( SourceInfo, "Failed to get group details for wallboard: %s.", name.c_str() );
                failures++;
            }
            catch( const CORBA::Exception& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
                LOGMORE( SourceInfo, "Failed to get group details for wallboard: %s.", name.c_str() );
                failures++;
            }
            catch( const TA_Base_Core::TransactiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
                LOGMORE( SourceInfo, "Failed to get group details for wallboard: %s.", name.c_str() );
                failures++;
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown reason" );
                LOGMORE( SourceInfo, "Failed to get group details for wallboard: %s.", name.c_str() );
                failures++;
            }
        }

        // Notify the user of any problems.

        if ( 0 != failures )
        {
            TA_Bus::TransActiveMessage userMsg;
            CString itemName;
            itemName.LoadString(IDS_WALLBOARDS);
            userMsg << failures << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_210083);
        }
    }
    catch( const TA_Base_Core::ObjectResolutionException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return wallboards;
    }
    catch( const TA_App::VideoSwitchAgentException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return wallboards;
    }
    catch( const CORBA::Exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        MessageBox::error(IDS_RETRIEVE_WALLBOARD_DETAILS);

        FUNCTION_EXIT;
        return wallboards;
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        MessageBox::error(IDS_RETRIEVE_WALLBOARD_DETAILS);

        FUNCTION_EXIT;
        return wallboards;
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
        MessageBox::error(IDS_RETRIEVE_WALLBOARD_DETAILS);

        FUNCTION_EXIT;
        return wallboards;
    }

    if ( 0 == wallboards.size() )
    {
        CString itemName, agentName;
        itemName.LoadString(IDS_WALLBOARDS);
        agentName.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << itemName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
        // Continue so that the default item is put in the combo box.
    }

    for( it = groups.begin(); it != groups.end(); it++ )
    {
        delete *it;
        *it = 0;
    }

    FUNCTION_EXIT;
    return wallboards;
}


CPlanStepEditVideoWallPage::NameToKeyMap CPlanStepEditVideoWallPage::GetLayoutsForWallboard()
{
    FUNCTION_ENTRY( "GetLayoutsForWallboard" );

    NameToKeyMap layouts;

    // Find the name of the agent monitoring this wallboard.

    unsigned long wallboardEntityKey = m_wallboardParameter.getWallboardKey();
    KeyToNameMap::iterator ait = m_agents.find( wallboardEntityKey );
    TA_ASSERT ( m_agents.end() != ait, "The agent name for this wallboard hasn't been stored." );
    std::string agentName = ait->second;

    // Get the Corba name of the agent.

    TA_Base_Core::NamedObject<TA_Base_App::IVideoWallAgentCorbaDef, TA_Base_App::IVideoWallAgentCorbaDef_ptr,
        TA_Base_App::IVideoWallAgentCorbaDef_var> agent;
    try
    {
         agent.setCorbaName( agentName, m_wallboardParameter.getWallboardName() );
    }
    catch( const TA_Base_Core::DatabaseException& e )
    {
        std::stringstream error;
        error << "The Corba name for agent " << agentName << " could not be determined, so the wallboard layouts could not be loaded.";
        LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_LAYOUT_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return layouts;
    }
    catch( const TA_Base_Core::DataException& e )
    {
        std::stringstream error;
        error << "The Corba name for agent " << agentName << " could not be determined, so the wallboard layouts could not be loaded.";
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_LAYOUT_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return layouts;
    }

    // Get the layout details for the wallboard.

    TA_Base_App::LayoutDetailsSequence_var layoutDetails;
    try
    {
        try
        {
            layoutDetails = agent->getLayoutsDetails();
        }
        // TD18706,jianghp, handle the operation mode
        catch (TA_Base_Core::OperationModeException &)
        {
            agent.clear();
            layoutDetails = agent->getLayoutsDetails();
        }
    }
    catch ( TA_Base_App::IVideoWallAgentCorbaDef::VideoWallAgentException& e )
    {
        std::stringstream error;
        error << "The layout details for wallboard " << m_wallboardParameter.getWallboardName() << " could not be loaded.";
        LOG_EXCEPTION_CATCH( SourceInfo, "IVideoWallAgentCorbaDef::VideoWallAgentException", static_cast< const char* >( e.reason ) );
        // TODO: use an appropriate message here
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_LAYOUT_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return layouts;
    }
    catch ( TA_Base_Core::ObjectResolutionException& e )
    {
        std::stringstream error;
        error << "The layout details for wallboard " << m_wallboardParameter.getWallboardName() << " could not be loaded.";
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_LAYOUT_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return layouts;
    }
    catch ( CORBA::Exception& e )
    {
        std::stringstream error;
        error << "The layout details for wallboard " << m_wallboardParameter.getWallboardName() << " could not be loaded.";
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_WALLBOARD_LAYOUT_DETAILS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);

        FUNCTION_EXIT;
        return layouts;
    }

    for ( unsigned int i = 0; i < layoutDetails->length(); i++ )
    {
        layouts[ std::string( layoutDetails[i].layoutName ) ] = layoutDetails[i].layoutId;
    }

    if ( layouts.empty() )
    {
        CString itemName, agentName;
        itemName.LoadString(IDS_WALLBOARD_LAYOUT);
        agentName.LoadString(IDS_VIDEO_WALL_LAYOUT_AGENT);
        TA_Bus::TransActiveMessage userMsg;
        userMsg << itemName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210079);
    }

    FUNCTION_EXIT;
    return layouts;
}


#endif // defined (STEPTYPE_VIDEOWALL)
