//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterMonitorEditDlg.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Dialog for selecting a monitor name for the Assign Camera To Monitor step.

#include "stdafx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/src/EntityTypeConstants.h"
#include "PlanManager.h"
#include "PlanStepParameterMonitorEditDlg.h"
#include "MonitorStepParameter.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "MessageBox.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const std::string ERROR_MSG = "Failed to get the monitors from the VideoSwitchAgent.";
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterMonitorEditDlg dialog

CPlanStepParameterMonitorEditDlg::CPlanStepParameterMonitorEditDlg(MonitorStepParameter& parameter, bool canEdit)
    : CPlanStepParameterEditDlg(parameter, CPlanStepParameterMonitorEditDlg::IDD, canEdit),
      m_monitorParameter( parameter )
{
    FUNCTION_ENTRY( "CPlanStepParameterMonitorEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterMonitorEditDlg)
    m_monitorName = _T("");
    m_parameterName = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterMonitorEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterMonitorEditDlg)
    DDX_Control(pDX, IDOK, m_okButton);
    DDX_Control(pDX, IDC_MONITOR_TREE, m_monitorTree);
    DDX_Text(pDX, IDC_MONITOR_NAME, m_monitorName);
    DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterMonitorEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterMonitorEditDlg)
    ON_NOTIFY(TVN_SELCHANGED, IDC_MONITOR_TREE, onSelectionChangedMonitorTree)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterMonitorEditDlg message handlers

BOOL CPlanStepParameterMonitorEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    CWaitCursor waitCursor;

    m_monitorName = m_monitorParameter.getMonitorName().c_str();
    m_parameterName = m_monitorParameter.getName().c_str();
    UpdateData( FALSE );
    m_monitorKey = m_monitorParameter.getMonitorKey();

    // Disable the OK button until a monitor has been selected.

    if ( m_monitorName.IsEmpty() )
    {
        m_okButton.EnableWindow( FALSE );
    }

       loadMonitorTree();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

//[[ GF-155 shiva
void CPlanStepParameterMonitorEditDlg::loadMonitorTree()
{
    FUNCTION_ENTRY( "loadMonitorTree" );

	TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorVector monitors;
	try
	{
		monitors = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoMonitors();
	}
	catch( const TA_Base_Core::ObjectResolutionException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
		CString actionName, agent;
		actionName.LoadString(IDS_RETRIEVE_MONITORS);
		agent.LoadString(IDS_SCHEDULING_AGENT);
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << actionName << agent;
		userMsg.showMsgBox(IDS_UE_030064);
	}
	catch( const TA_Base_Bus::VideoSwitchAgentException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
		LOGMORE( SourceInfo, ERROR_MSG.c_str() );
		CString actionName, agent;
		actionName.LoadString(IDS_RETRIEVE_MONITORS);
		agent.LoadString(IDS_SCHEDULING_AGENT);
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << actionName << agent;
		userMsg.showMsgBox(IDS_UE_030064);
	}
	catch( const CORBA::Exception& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
		MessageBox::error(IDS_RETRIEVE_MONITORS);
	}
	catch( const TA_Base_Core::TransactiveException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
		MessageBox::error(IDS_RETRIEVE_MONITORS, e);
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
		MessageBox::error(IDS_RETRIEVE_MONITORS);
	}

	HTREEITEM rootItem;
	HTREEITEM monitorItem;
	int groupFailures = 0;
	int monitorFailures = 0;

	// This will be set to the item for the monitor that is currently stored in the MonitorStepParameter.
	// It will be identified using m_initialMonitorKey and it will be automatically selected.

	HTREEITEM* initialMonitorItem = 0;

	TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorVector::iterator it;	
	for( it = monitors.begin(); it != monitors.end(); it++ )
	{

		std::string locationName = "";
		std::string monitorName = "";
		unsigned long monitorKey = 0;
		try
		{
			CORBA_CALL_RETURN( monitorName, ( *( *it ) ), getName, () );//limin
			CORBA_CALL_RETURN( monitorKey, ( *( *it ) ), getKey, () );//limin

		}
		catch( TA_Base_Bus::VideoSwitchAgentException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get monitor details.");
			monitorFailures++;
			continue;
		}
		if ( monitorName.empty() || 0 == monitorKey )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get monitor details.");
			monitorFailures++;
			continue;
		}

		try
		{
			// get the location of the monitor
			TA_Base_Core::IEntityData * pEntityData = NULL;
			TA_Base_Core::ILocation * loc = NULL;
			pEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(monitorKey);
			if(pEntityData != NULL)
			{
				loc = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(pEntityData->getLocationName());
				if(loc != NULL)
				{
					locationName = loc->getDisplayName();
				}			
			}		
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
			groupFailures++;
			continue;
		}

		if ( locationName.empty() )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Location name is empty." );
			groupFailures++;
			continue;
		}

		rootItem = NULL;
		HTREEITEM hCurrent = m_monitorTree.GetRootItem( );
		while (hCurrent != NULL)
		{
			CString nodeText = m_monitorTree.GetItemText(hCurrent);
			if(nodeText.CompareNoCase(locationName.c_str()) == 0)
			{
				rootItem = hCurrent;
				break;
			}
			hCurrent = m_monitorTree.GetNextItem(hCurrent, TVGN_NEXT);

		}
		

		if(rootItem == NULL)
		{
			rootItem = m_monitorTree.InsertItem( locationName.c_str(), 0, 0, TVI_ROOT, TVI_LAST );
			if ( NULL == rootItem )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to insert location name: %s.", locationName.c_str() );
				groupFailures++;
				break;

			}
		}


		// Insert the monitor's name into the tree and put the entity key in as the item data.

		monitorItem = m_monitorTree.InsertItem( monitorName.c_str(), 2 ,2, rootItem, TVI_LAST );
		if ( NULL == monitorItem )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to insert monitor: %s.", monitorName.c_str());
			monitorFailures++;
		}
		m_monitorTree.SetItemData( monitorItem, monitorKey );

		// Do this in here because it is faster than trying to find the item again.

		if ( monitorKey == m_monitorKey )
		{
			initialMonitorItem = &monitorItem;

		}

		// Remove the group name if none of the children could be inserted.

		if (  (NULL != rootItem) && (!m_monitorTree.ItemHasChildren( rootItem )) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Removing location name because it has no children: %s", locationName.c_str() );
			m_monitorTree.DeleteItem( rootItem );
		}


	}


	// Sort the whole tree.

	m_monitorTree.SortChildren( NULL );

	// Display an error message if there were errors loading the list.

	if ( 0 != groupFailures || 0 != monitorFailures )
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		unsigned int number = (0 != groupFailures) ? groupFailures : monitorFailures;
		CString itemName;
		itemName.LoadString( (0 != groupFailures) ? IDS_GROUPS : IDS_MONITORS);
		userMsg << number << itemName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_210083);
	}

	// When the bit below this works, we can take this bit out...

	if ( m_monitorName.IsEmpty() )
	{
		m_okButton.EnableWindow( FALSE );
	}
	else
	{
		m_okButton.EnableWindow( TRUE );
	}

	// Finally delete all the named obj refs, because we don't need them anymore.

	for ( it = monitors.begin(); it != monitors.end(); it++ )
	{
		delete *it;
		*it = 0;
	}


	FUNCTION_EXIT;


	/* KTTODO THIS DOESN'T WORK AT THE MOMENT.

	// Select the current monitor (if there is one and we found it).
	// It's faster to do it here while we already know the item.

	if ( 0 != m_monitorKey )
	{
	if ( ( 0 == initialMonitorItem ) || ( 0 == m_monitorTree.Select( *initialMonitorItem, TVGN_FIRSTVISIBLE ) ) )
	{
	std::stringstream error;
	if ( m_monitorName.IsEmpty() )
	{
	error << "Failed to select current Monitor (name unknown).";
	}
	else
	{
	error << "Failed to select current Monitor " << m_monitorName << ".";
	}
	MessageBox::error( error.str().c_str() );
	}
	else
	{
	// Now we've selected a monitor, so enable the OK button.

	m_okButton.EnableWindow( TRUE );
	}
	}
	*/

}
//]] GF-155 shiva

void CPlanStepParameterMonitorEditDlg::onSelectionChangedMonitorTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onSelectionChangedMonitorTree" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get currently selected item.

    HTREEITEM item = m_monitorTree.GetSelectedItem();
    if ( NULL == item )
    {
//        MessageBox::error( "Failed to identify selected monitor." );
        m_monitorName = "";
    }
    else
    {
        // If the key is not zero, then we have a monitor, otherwise it is a group name.

        unsigned long itemKey =  m_monitorTree.GetItemData( item );
        if ( 0 != itemKey )
        {
            m_monitorName = m_monitorTree.GetItemText( item );
            m_monitorKey = itemKey;

            // Now we've selected a monitor, so enable the OK button.

            m_okButton.EnableWindow( TRUE );
        }
    }
    UpdateData( FALSE );

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterMonitorEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    m_monitorParameter.setMonitorName( std::string( m_monitorName ) );
    m_monitorParameter.setMonitorKey( m_monitorKey );

    CPlanStepParameterEditDlg::OnOK();

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
