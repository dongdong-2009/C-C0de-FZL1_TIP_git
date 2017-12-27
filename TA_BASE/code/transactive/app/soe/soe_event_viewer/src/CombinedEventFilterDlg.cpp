/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventFilterDlg.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2009/12/03 17:15:17 $
 * Last modified by:  $Author: miao.yu $
 *
 * CombinedEventFilterDlg is an MFC class deriving from the FilterDlg header (which is in turn a CDialog derived class).
 * This dialog provides filtering capabilities on the data items displayed in the combined event view.
 */

#pragma warning(disable:4786)       // identifier truncated to 255 characters
#pragma warning(disable:4800)       // forcing value to bool 'true' or 'false' (performance warning)

#include <afxtempl.h> // For the CArray
#include <algorithm>

#include "app/soe/soe_event_viewer/src/stdafx.h"
#include "app/soe/soe_event_viewer/src/EventViewer.h"
#include "app/soe/soe_event_viewer/src/EventViewerDlg.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilterDlg.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilter.h"
#include "app/soe/soe_event_viewer/src/SaveFilterDlg.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/mfc_extensions/src/coding/Encoder.h"

#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/soe/src/SOEEvent.h"
#include "core/data_access_interface/soe/src/SOEFilter.h"


using namespace TA_Base_Core;

using TA_Base_Core::DataException;
using TA_Base_Core::NamedFilterAccessFactory;
using TA_Base_Core::NamedFilter;
using TA_Base_Core::SOEEvent;

using TA_Base_Bus::Encoder;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const unsigned int TWENTY_FOUR_HOURS( 24*60*60 );
    //const std::string ERROR_LOAD( "load" );
    //const std::string ERROR_LOADED( "loaded" );
    //const std::string ERROR_SAVE( "save" );
    //const std::string ERROR_SAVED( "saved" );
    //const std::string ERROR_DELETE( "delete" );
    //const std::string ERROR_DELETED( "deleted" );
    const std::string ERROR_LOAD( "载入" );
    const std::string ERROR_LOADED( "已载入" );
    const std::string ERROR_SAVE( "保存" );
    const std::string ERROR_SAVED( "已保存" );
    const std::string ERROR_DELETE( "删除" );
    const std::string ERROR_DELETED( "已删除" );
}

/////////////////////////////////////////////////////////////////////////////
// CombinedEventFilterDlg dialog

const int CombinedEventFilterDlg::UNCHECKED = 0;
const int CombinedEventFilterDlg::CHECKED = 1;
 
 
const int CombinedEventFilterDlg::COMBO_LIMIT = 0;
const int CombinedEventFilterDlg::EVENT_ONLY_KEY = -1;
//const char* CombinedEventFilterDlg::EVENT_ONLY_STRING = "Event";
const char* CombinedEventFilterDlg::EVENT_ONLY_STRING = "事件";

CombinedEventFilterDlg::CombinedEventFilterDlg(Filter* filter, CWnd* pParent /*=NULL*/)
	: FilterDlg(CombinedEventFilterDlg::IDD, pParent), m_filter(filter), m_disableEvents(true),
      m_originalFilterName(m_filter->getNamedFilter()->getName())
{
	//{{AFX_DATA_INIT(CombinedEventFilterDlg)
	//}}AFX_DATA_INIT
}


void CombinedEventFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	FilterDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CombinedEventFilterDlg)
	DDX_Control(pDX, IDC_FROM, m_fromCheck);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, ID_DEFAULT_FILTER, m_defaultButton);
	DDX_Control(pDX, ID_APPLY, m_applyButton);
	DDX_Control(pDX, IDC_DISPLAY_VALUE, m_txtDisplayValue);
	DDX_Control(pDX, IDC_DESC, m_txtDescription);
	DDX_Control(pDX, IDC_ASSET, m_assetEdit);
	DDX_Control(pDX, IDC_LOCATION, m_locationEdit);
	DDX_Control(pDX, IDC_TO_TIME, m_toTime);
	DDX_Control(pDX, IDC_TO_DATE, m_toDate);
	DDX_Control(pDX, IDC_CURRENT, m_toCheck);
	DDX_Control(pDX, IDC_FROM_TIME, m_fromTime);
	DDX_Control(pDX, IDC_FROM_DATE, m_fromDate);
    DDX_Control(pDX, IDC_COMBO_NAMEDFILTER, m_namedFilterCombo);
    DDX_Control(pDX, IDC_BUTTON_SAVEFILTER, m_saveFilterButton);
    DDX_Control(pDX, IDC_BUTTON_DELETEFILTER, m_deleteFilterButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CombinedEventFilterDlg, FilterDlg)
	//{{AFX_MSG_MAP(CombinedEventFilterDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CURRENT, OnTo)
	ON_BN_CLICKED(IDC_LOCATION_EX, OnLocationExt)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_BN_CLICKED(ID_DEFAULT_FILTER, OnDefaultFilter)
	ON_EN_CHANGE(IDC_ASSET, OnChangeAsset)
	ON_EN_CHANGE(IDC_DESC, OnChangeDescription)
	ON_EN_CHANGE(IDC_DISPLAY_VALUE, OnChangeDisplayValue)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TO_DATE, OnDatetimechangeToDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TO_TIME, OnDatetimechangeToTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_FROM_DATE, OnDatetimechangeFromDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_FROM_TIME, OnDatetimechangeFromTime)
	ON_BN_CLICKED(IDC_FROM, OnFrom)
    ON_BN_CLICKED(IDC_BUTTON_SAVEFILTER, OnSaveFilter)
    ON_BN_CLICKED(IDC_BUTTON_DELETEFILTER, OnDeleteFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_NAMEDFILTER, OnSelchangeComboNamedFilter)
	ON_CBN_EDITCHANGE(IDC_COMBO_NAMEDFILTER,OnTextchangeNamedFilter)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CombinedEventFilterDlg message handlers

void CombinedEventFilterDlg::OnClose()
{
    FUNCTION_ENTRY("OnClose");

    m_filter->resumeCurrentListDataLoad();

    if ( m_originalFilterName == m_filter->getNamedFilter()->getName() )
    {
        // OnClose is called when the filter box is closed (by clicking the "X" in the right-hand corner).
        // As OK was not explicitly clicked, that is the same as cancelling - so send the "CANCEL" message to the main dialog.
        AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_CANCEL);
    }
    else
    {
        // TD12035: If the names don't match the filter has changed and cancelling must still apply the change (E.g.
        // when a named filter that is currenly being used is deleted).
        AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_CLOSED);
    }

    // And close this dialog
    FUNCTION_EXIT;
    FilterDlg::OnClose();
}

void CombinedEventFilterDlg::OnApply()
{
    FUNCTION_ENTRY("OnApply");

    m_filter->stopCurrentListDataLoad();

    while(dynamic_cast <EventViewerDlg*> (AfxGetMainWnd())->m_eventList.isActive())
    {
        TA_Base_Core::Thread::sleep(200);
    }

    m_filter->resetListLoadFlag();

	m_filter->resumeRefresh();//changxing
    AfxGetMainWnd()->PostMessage(WM_USER_LOAD_COUNT,0,0);

    // Apply has been clicked. Need to apply the filters.
    m_filter->setNamedFilter( std::auto_ptr<NamedFilter>( m_namedFilter.clone() ) );

    // Enable and disable buttons as appropriate
    m_applyButton.EnableWindow(false);

    // Force the named filters to refresh.
    populateNamedFilters();

    // Then notify the main dialog the filters have changed.
    AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_APPLY);

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnOK()
{
    FUNCTION_ENTRY("OnOK");

    m_filter->stopCurrentListDataLoad();

    while(dynamic_cast <EventViewerDlg*> (AfxGetMainWnd())->m_eventList.isActive())
    {
        TA_Base_Core::Thread::sleep(200);
    }

    m_filter->resetListLoadFlag();
	m_filter->resumeRefresh();//changxing

    AfxGetMainWnd()->PostMessage(WM_USER_LOAD_COUNT,0,0);

    // Need to apply the filters and then send the close message
    m_filter->setNamedFilter( std::auto_ptr<NamedFilter>( m_namedFilter.clone() ) );

    AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_CLOSED);

    // And then actually close the dialog
    FUNCTION_EXIT;
    FilterDlg::OnOK();
}

void CombinedEventFilterDlg::OnCancel()
{
    FUNCTION_ENTRY("OnCancel");

    m_filter->resumeCurrentListDataLoad();

    // Dialog was canceled, so don't want to apply the filters. Do want to send the cancel message however
    OnClose();

    // Make sure the dialog closes (OnClose() should actually close it)
    FUNCTION_EXIT;
    FilterDlg::OnCancel();
}

BOOL CombinedEventFilterDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog");

    FilterDlg::OnInitDialog();

    // Set max length to same as database field
    m_assetEdit.SetLimitText(80);
    m_txtDisplayValue.SetLimitText(300);
	m_txtDescription.SetLimitText(300);

    // Load data required for multiple selections.
    loadListData();

     // Load the filters
    loadFilter( true );

    // Populate the named filter combo box.
    populateNamedFilters();

    // Enable and disable buttons as appropriate
    m_applyButton.EnableWindow(false);
    m_okButton.EnableWindow(true);

	// TD #12583
	// The Save button should be disabled upon startup
	m_saveFilterButton.EnableWindow(false);

	m_namedFilterCombo.LimitText(32); //TD15027 azenitha
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15027: Set limit of text");

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CombinedEventFilterDlg::loadListData()
{
    FUNCTION_ENTRY("loadListData");

    // Construct a DataAccess object to retrieve all required data from the database
    unsigned int i;

    // Retrieve the locations and then populate the locations list.
    std::vector< std::pair<unsigned long,std::string> > locations = m_dbAccess.getLocations();
    for ( i=0; i<locations.size(); i++ )
    {
        ListItem item( locations[i].first, locations[i].second );
        m_locations.push_back( item );
    }
    //m_locations.sort();

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::populateEditFromFilter( CEdit& edit, const std::string& columnId, ListItems& items )
{
    FUNCTION_ENTRY("populateEditFromFilter");

    SelectedItems selection = m_namedFilter.getNumericFilter( columnId );
    populateEditFromSelection( edit, selection, items );

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::populateEditFromSelection( CEdit& edit, const SelectedItems& selection, ListItems& items )
{
    FUNCTION_ENTRY("populateEditFromSelection");

    std::string listStr("");

    if ( 0 == selection.size() )
    {
        // If nothing is selected ALL are allowed.
        listStr = "<全部>";
    }
    else
    {
        // Otherwise allow only what is currently selected.
        SelectedItems::const_iterator selectionIt;
        for ( selectionIt=selection.begin(); selectionIt!=selection.end(); selectionIt++ )
        {
            // Get the string associated with the selected item.
            ListItem item( *selectionIt, "" );
            ListItems::iterator it = std::find( items.begin(), items.end(), item );

            if ( items.end() != it )
            {
                // Add it to the string.
                listStr += it->str;
            }
            else
            {
                // What should we do in this unlikely event? Just ignore it for now.
            }

            // Add a comma if there are still more remaining.
            if ( selection.end() != ( selectionIt+1 ) )
            {
                listStr += ", ";
            }
        }
    }

    // Finally, set the text.
    edit.SetWindowText(listStr.c_str());

    FUNCTION_EXIT;
}

 

void CombinedEventFilterDlg::OnFrom()
{
    FUNCTION_ENTRY("OnFrom");

    // Called when the "From: checkbox state changes.
    if ( UNCHECKED == m_fromCheck.GetCheck() )
    {
        // The box is now unchecked, so need to disable the "from" filters
        m_fromTime.EnableWindow(false);
        m_fromDate.EnableWindow(false);

        // Get the current "to" time.
		offsetFromTimeTakeRefToTime(TWENTY_FOUR_HOURS);

        // If the value of "from" depends on "to" don't apply any restrictions
        // on the "to" range.
        // Need to cast to avoid ambiguous use of SetRange
        m_toTime.SetRange( static_cast<CTime*>(NULL), static_cast<CTime*>(NULL) );
        m_toDate.SetRange( static_cast<CTime*>(NULL), static_cast<CTime*>(NULL) );
    }
    else
    {
		// The box is now checked, so need to enable the "from" filters
		m_fromTime.EnableWindow(true);
		m_fromDate.EnableWindow(true);

		// TD 14914 Update the To-time only if it is unchecked
        // If the previous button is not checked we need to update its value.
        if ( UNCHECKED == m_toCheck.GetCheck() )
        {
			offsetToTimeTakeRefFromTime(TWENTY_FOUR_HOURS);
		}
    }

    // Enable Buttons as necessary
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Update the named filter times.
    updateTimes();

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnTo()
{
    FUNCTION_ENTRY("OnTo");

    // Called when the "To:" checkbox state changes
    if (UNCHECKED == m_toCheck.GetCheck())
    {
        // The box is now unchecked, so need to disable the "to" filters
        m_toTime.EnableWindow(false);
        m_toDate.EnableWindow(false);

        // Get the current "to" time.
        CTime toTime( CTime::GetCurrentTime() );

        // Set the value of the "to" fields.
        m_toDate.SetTime(&toTime);
        m_toTime.SetTime(&toTime);

        // Set valid time range for "from" field.
        m_fromTime.SetRange(NULL,&toTime);
        m_fromDate.SetRange(NULL,&toTime);

        // If the previous button is not checked we need to update its value.
        if ( UNCHECKED == m_fromCheck.GetCheck() )
        {
			offsetFromTimeTakeRefToTime(TWENTY_FOUR_HOURS);
        }
    }
    else
    {
        // The box is now checked, so need to enable the "to" filters
        m_toTime.EnableWindow(true);
        m_toDate.EnableWindow(true);

		// Get the time from the From box and set to 24 hrs but not later than current time
        // If the previous button is not checked we need to update its value.
        if ( UNCHECKED == m_fromCheck.GetCheck() )
        {
			offsetFromTimeTakeRefToTime(TWENTY_FOUR_HOURS);
		}
  }

    // Enable Buttons as necessary
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Update the filter buffers times.
    updateTimes();

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnLocationExt()
{
    FUNCTION_ENTRY("OnLocationExt");
    // Called when the "..." next to the "Location" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Location...");
	MultipleLoadDlg loadDlg("选择位置...");

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_locations, true );

    // Set the current selections.
    loadDlg.setSelection( m_namedFilter.getNumericFilter( SOEEventFilter::FILTER_LOCATION_KEY ) );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

        // If selection changed, update selection and repopulate edit control.
		m_namedFilter.setNumericFilter( SOEEventFilter::FILTER_LOCATION_KEY, selection );
        populateEditFromSelection( m_locationEdit, selection, m_locations );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnDefaultFilter()
{
    FUNCTION_ENTRY("OnDefaultFilter");

    std::auto_ptr<NamedFilter> loadedFilter;

    try
    {
        // Trigger the filter load.
        loadedFilter = NamedFilterAccessFactory::getInstance().getNamedFilter( CombinedEventFilter::NAMEDFILTER_DEFAULT );
    }
    catch ( DataException& ex )
    {
        // Log the exception.
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

        // Display the load error message.
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << ERROR_LOAD;
        userMsg << ERROR_LOADED;
        userMsg.showMsgBox(IDS_UE_040031);
        return;
    }

    // Now enable the buttons.
    enableButtons();

	 //as per TD 14762
	 //we don't want to enable Apply button
	 //if Default filter was already selected
	 //so redo part of what enableButtons() does
	 //here before the combo box is
	 //populated with new values
	 CString defaultFilterName, oldFilterName;
	 m_namedFilterCombo.GetWindowText( oldFilterName );
	 defaultFilterName = loadedFilter->getName ().c_str ();
	 if (oldFilterName.Compare (defaultFilterName) == 0)
	 {
		 m_applyButton.EnableWindow (false);
	 }

    // Copy the new filter to our internal filter.
    m_namedFilter = *(loadedFilter.get());

    // Force the filter dialog to show the new filter.
    loadFilter( false );

    // Now force the combo box to reflect the new state.
    populateNamedFilters();
    m_saveFilterButton.EnableWindow( FALSE );

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnChangeAsset()
{
    FUNCTION_ENTRY("OnChangeAsset");

    if ( m_disableEvents )
    {
        return;
    }

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    CString text;
    m_assetEdit.GetWindowText( text );
	m_namedFilter.removeStringFilter( SOEEventFilter::FILTER_ASSET_NAME );
    if ( !text.IsEmpty() )
    {
		m_namedFilter.addStringFilter( SOEEventFilter::FILTER_ASSET_NAME, std::string(text) );
    }

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnChangeDescription()
{
	FUNCTION_ENTRY("OnChangeDescription");

	if ( m_disableEvents )
	{
		return;
	}

	// Enable the buttons.
	enableButtons();

	// Now that the filter is changed it is user defined.
	selectUserFilter();

	CString text;
	m_txtDescription.GetWindowText( text );
	m_namedFilter.removeStringFilter( SOEEventFilter::FILTER_DESCRIPTION );
	if ( !text.IsEmpty() )
	{
		m_namedFilter.addStringFilter( SOEEventFilter::FILTER_DESCRIPTION, std::string(text) );
	}

	FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnTextchangeNamedFilter()
{
    // Get the new name.
    CString filterName;
    m_namedFilterCombo.GetWindowText( filterName );

    // Change filter button states.
    enableNamedFilterButtons( std::string(filterName) );

	// TD #12583
	// The Save button should always be enabled if the name has changed
	m_saveFilterButton.EnableWindow(true);
}

void CombinedEventFilterDlg::OnChangeDisplayValue()
{
    FUNCTION_ENTRY("OnChangeDisplayValue");

    if ( m_disableEvents )
    {
        return;
    }

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    CString text;
    m_txtDisplayValue.GetWindowText( text );
	m_namedFilter.removeStringFilter( SOEEventFilter::FILTER_DISPLAY_VALUE );
    if ( !text.IsEmpty() )
    {
		// GF-23: convert Ascii to utf8
		std::auto_ptr<char> pstr = Encoder::AnsiToUTF8((LPCTSTR)text);
        m_namedFilter.addStringFilter( SOEEventFilter::FILTER_DISPLAY_VALUE, std::string(pstr.get()) );
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnKillfocusSeverityList()
{
    // Remove the selection highlighting
     
}

void CombinedEventFilterDlg::OnChkChange()
{
    FUNCTION_ENTRY("OnChkChange");

    if ( m_disableEvents )
    {
        return;
    }

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Update the filter buffer.
    //m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_SEVERITY );
    //for( unsigned long i = 0; i < static_cast<unsigned long>(m_severityList.GetCount()); ++i )
    //{
    //    if( m_severityList.GetCheck(i) == 1 )
    //    {
    //        m_namedFilter.addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, m_severityList.GetItemData(i) );
    //    }
    //}

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnDatetimechangeToDate(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY("OnDatetimechangeToDate");

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Synchronise times between controls.
    CTime toDate;
    m_toDate.GetTime( toDate );
    m_toTime.SetTime( &toDate );

	// offset FROM time if it is unchecked
    if ( UNCHECKED == m_fromCheck.GetCheck() )
    {
		offsetFromTimeTakeRefToTime(TWENTY_FOUR_HOURS);
	}

    // Update the filter buffers times.
    updateTimes();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnDatetimechangeToTime(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY("OnDatetimechangeToTime");

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Synchronise times between controls.
    CTime toTime;
    m_toTime.GetTime( toTime );
    m_toDate.SetTime( &toTime );

	// offset FROM time if it is unchecked
    if ( UNCHECKED == m_fromCheck.GetCheck() )
    {
		offsetFromTimeTakeRefToTime(TWENTY_FOUR_HOURS);
	}

    // Update the filter buffers times.
    updateTimes();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::OnDatetimechangeFromDate(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY("OnDatetimechangeFromDate");

    // Enable the buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Synchronise times between controls.
    CTime fromDate;
    m_fromDate.GetTime( fromDate );
    m_fromTime.SetTime( &fromDate );

	// offset to time if it is unchecked
    if ( UNCHECKED == m_toCheck.GetCheck() )
    {
		offsetToTimeTakeRefFromTime(TWENTY_FOUR_HOURS);
	}

    // Update the filter buffers times.
    updateTimes();

    *pResult = 0;

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnDatetimechangeFromTime(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY("OnDatetimechangeFromTime");

    // Enable the ok, apply, and cancel buttons.
    enableButtons();

    // Now that the filter is changed it is user defined.
    selectUserFilter();

    // Synchronise times between controls.
    CTime fromTime;
    m_fromTime.GetTime( fromTime );
    m_fromDate.SetTime( &fromTime );

	// offset to time if it is unchecked
    if ( UNCHECKED == m_toCheck.GetCheck() )
    {
		offsetToTimeTakeRefFromTime(TWENTY_FOUR_HOURS);
	}

    // Update the filter buffers times.
    updateTimes();

    *pResult = 0;

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::offsetToTimeTakeRefFromTime(unsigned long hours)
{
	// Get the time from time and set it "Hours" hrs earlier but not later than current time
	// Get the current "to" time.
	CTime time;
	m_fromTime.GetTime( time );
	// Now adjust "from" to be "Hours" hours after "from".
	CTime toTime(time.GetTime() + hours);

	// Check current time and set to this if exceed the current time
	CTime curTime( CTime::GetCurrentTime() );
	if (toTime > curTime)
	{
		// Set the value of the "to" fields.
		m_toDate.SetRange(NULL,&curTime);
		m_toTime.SetRange(NULL,&curTime);

		// And set the box to have adjusted from+24hr time
		m_toDate.SetTime( &curTime );
		m_toTime.SetTime( &curTime );
	}
	else
	{
		// And set the box to have adjusted from+24hr time
		m_toDate.SetTime( &toTime );
		m_toTime.SetTime( &toTime );
	}
}

void CombinedEventFilterDlg::offsetFromTimeTakeRefToTime(unsigned long hours)
{
	// Get the current "to" time.
	CTime time;
	m_toTime.GetTime( time );

	// Now adjust "from" to be 24 hours before "to".
	CTime fromTime(time.GetTime() - hours);

	// And set the box to have the "current" time
	m_fromDate.SetTime( &fromTime );
	m_fromTime.SetTime( &fromTime );
}

void CombinedEventFilterDlg::OnSelchangeComboNamedFilter()
{
    FUNCTION_ENTRY("OnSelchangeComboNamedFilter");

    // Get the currently selected item.
    int selIdx = m_namedFilterCombo.GetCurSel();
    if ( 0 <= selIdx )
    {
        // Get the name of the filter.
        CString filterName_c;
        m_namedFilterCombo.GetLBText( selIdx, filterName_c );
        std::string filterName( filterName_c );

        // Make sure the same filter wasn't selected.
        if ( filterName != m_namedFilter.getName() )
        {
            std::auto_ptr<NamedFilter> loadedFilter;

            try
            {
                // Load the filter.
                loadedFilter = NamedFilterAccessFactory::getInstance().getNamedFilter( filterName );
            }
            catch ( DataException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

                // Display the load error message.
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << ERROR_LOAD;
                userMsg << ERROR_LOADED;
                userMsg.showMsgBox(IDS_UE_040031);

                // Reselect the current named filter.
                populateNamedFilters();
                return;
            }

            // Copy the new filter to our internal filter.
            m_namedFilter = *(loadedFilter.get());

            // Force the filter dialog to show the new filter.
            loadFilter( false );

            // Enable the ok, apply, and cancel buttons.
            enableButtons();
        }

        // Enable the named filter buttons as required.
        enableNamedFilterButtons( filterName );

		// TD #12583
		// The Save button should always be disabled after a saved named filter,
		// has been selected (including "Default Filter", etc)
		m_saveFilterButton.EnableWindow(false);
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnSaveFilter()
{
    FUNCTION_ENTRY("OnSaveFilter");

    // Get the name that user wants to save the filter as.
    CString filterNameCStr;
    m_namedFilterCombo.GetWindowText( filterNameCStr );
    std::string filterName( filterNameCStr );

	//Begin--  TD14346
	filterNameCStr.MakeUpper();
	//bool isUserDefined = ( filterNameCStr.Find("USER DEFINED") >= 0 )? true : false;
	//bool isDefault = ( filterNameCStr.Find("DEFAULT") >= 0 ) ? true : false;
	bool isUserDefined = ( filterNameCStr.Find("自定义过滤器") >= 0 )? true : false;
	bool isDefault = ( filterNameCStr.Find("默认过滤器") >= 0 ) ? true : false;
	bool isInComboList = m_namedFilterCombo.FindString(-1, filterNameCStr) >= 0 ? true : false;
	if ( isUserDefined || isDefault ) // Can't save a Default or User Defined file
	{
		// Display a message box to disallow these keywords as name
		TA_Base_Bus::TransActiveMessage userMsg;
		//const std::string saveEr("save an invalid name for");
		//const std::string fileEr("saved as " + filterName);
		const std::string saveEr("保存无效的名字 ");
		const std::string fileEr("保存为 " + filterName);
		userMsg << saveEr;
		userMsg << fileEr;
		userMsg.showMsgBox(IDS_UE_040031);
		m_namedFilter.setName( NamedFilter::UNNAMED );
		return;
	}

	// This part is to delete old NamedFilter from database and create a new one
	// get the old filter name, in the event that this is a rename operation..
	// the old filter is the one we need to delete
	// if the filter name we want to save to is in the list, then that
	// is the one that needs to be deleted
	std::string oldFilterName(m_namedFilter.getName());
	if (IsFilterDataChanged() || isInComboList) {
		if(isInComboList)
		{
			oldFilterName = filterName;
		}

		if(oldFilterName != TA_Base_Core::NamedFilter::UNNAMED)
		{
			// make the user confirm the overwrite
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << "用当前的过滤器配置覆盖 '" + oldFilterName + "'";
			if( IDNO == userMsg.showMsgBox(IDS_UW_010003) )
			{
				return;
			}

			//Delete the old filter
			std::auto_ptr<NamedFilter> loadedFilter;
			try
			{
				std::auto_ptr<NamedFilter> loadedFilter( TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( oldFilterName ) );
				loadedFilter->deleteNamedFilter();
			}
			catch(...)
			{
			}
		}
	}

	 
    try
    {
        // Set the name of the current filter to the name specified.
        m_namedFilter.setName(filterName );

        // Save the filter.
        m_namedFilter.saveNamedFilter();
    }
    catch ( TA_Base_Core::DataException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

        // Display the save filter error message.
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << ERROR_SAVE << ERROR_SAVED;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);

        // Only an unnamed filter can be saved, so rename it back.
        m_namedFilter.setName( NamedFilter::UNNAMED );
        return;
    }
    catch (...)
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Failed to create and save filter");

        // Display the save filter error message.
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << ERROR_SAVE << ERROR_SAVED;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);

        // Only an unnamed filter can be saved, so rename it back.
        m_namedFilter.setName( NamedFilter::UNNAMED );
        return;
    }

    // Refresh the state of the window controls.
    populateNamedFilters();

	GetFilterData();// TD14346
    m_okButton.EnableWindow(true);

	// TD #12583
	// On a save, make no change to the Apply button, and make
	// sure the Save button is disabled
    //m_applyButton.EnableWindow(true);
	m_saveFilterButton.EnableWindow(false);

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnDeleteFilter()
{
    FUNCTION_ENTRY("OnDeleteFilter");

    // Confirm the deletion.
    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << ERROR_DELETE+" '"+m_namedFilter.getName()+"'";
    if ( IDNO == userMsg.showMsgBox(IDS_UW_010003) )
    {
        return;
    }

    // Store the old filter name so it can be used for comparison later.
    std::string oldFilterName = m_namedFilter.getName();

    try
    {
        // Delete the current named filter.
        m_namedFilter.deleteNamedFilter();
    }
    catch ( DataException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

        // Display the load error message.
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << ERROR_DELETE;
        userMsg << ERROR_DELETED;
        userMsg.showMsgBox(IDS_UE_040031);
        return;
    }

    // TD12035: If filter is deleted it should revert to default.
    OnDefaultFilter();

    // Now update filter buttons.
    enableNamedFilterButtons( m_namedFilter.getName() );

    // If the filter was the active one it also needs to be renamed.
    if ( oldFilterName == m_filter->getNamedFilter()->getName() )
    {
        // TD12035: Cannot just rename, we must now revert to default filter.
        m_filter->setNamedFilter( std::auto_ptr<NamedFilter>( m_namedFilter.clone() ) );
    }

    // The filter list has changed, so tell the main dialog to repopulate its control.
    //AfxGetMainWnd()->PostMessage( WM_USER_FILTER_DLG_NAMED_FILTER_CHANGE, 0, 0 ); //TD15032 azenitha
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15032: No need to refresh main dialog");
    FUNCTION_EXIT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Non-generated methods
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


void CombinedEventFilterDlg::updateTimes()
{
    FUNCTION_ENTRY("updateTimes");

    CTime date;
    CTime time;

    // Get the current time.
    CTime currentTime = CTime::GetCurrentTime();

    // TD11082: 'to' time cannot be more than current time.
    m_toDate.SetRange( NULL, &currentTime );
    m_toTime.SetRange( NULL, &currentTime );

    // Get the to time - it should now be less than current time.
    m_toDate.GetTime(date);
    m_toTime.GetTime(time);
    CTime toTime = getCombinedTime( date, time );

    // Apply the 'to' time to the named filter.
    m_namedFilter.removeNumericFilter( Filter::FILTER_TOTIME );
	// TD 14914 not to get the current time but the 24hrs time set when the "from" check box is checked
	// ie. set the default current time, when both are unchecked
    if (UNCHECKED == m_toCheck.GetCheck() && UNCHECKED == m_fromCheck.GetCheck() )
    {
        // If its not checked make sure the 'to' time is the current time.
        toTime = currentTime;
        m_toDate.SetTime( &toTime );
        m_toTime.SetTime( &toTime );

        //m_namedFilter.addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long>(Filter::CURRENT_TIME) );
    }
    else
    {
        m_namedFilter.addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long>(toTime.GetTime()) );
    }

    // 'from' time cannot be more than 'to' time.
    m_fromDate.SetRange( NULL, &toTime );
    m_fromTime.SetRange( NULL, &toTime );

    // Get the from time - it should now be less than 'to' time.
    m_fromDate.GetTime(date);
    m_fromTime.GetTime(time);
    CTime fromTime = getCombinedTime( date, time );

    // Apply the 'from' time to the named filter.
    m_namedFilter.removeNumericFilter( Filter::FILTER_FROMTIME );
    if ( UNCHECKED == m_fromCheck.GetCheck() )
    {
        // If its not checked make sure the 'from' time is 24 hours before the 'to' time.
        fromTime = CTime( toTime.GetTime() - TWENTY_FOUR_HOURS );
        m_fromDate.SetTime( &fromTime );
        m_fromTime.SetTime( &fromTime );

        //m_namedFilter.addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(Filter::PREVIOUS_DAY) );
    }
    else
    {
        m_namedFilter.addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(fromTime.GetTime()) );
    }

    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::loadFilter( bool importFromFilter )
{
    FUNCTION_ENTRY("loadFilter");

    // If we need to load by importing the named filter from the filter object.
    if ( importFromFilter )
    {
        // Copy the current filter to our internal filter.
        m_namedFilter = *(m_filter->getNamedFilter());
    }

    // Disable events during load.
    m_disableEvents = true;

    // Now Get the to time.
    CTime toTime( getTimeFromNamedFilter( Filter::FILTER_TOTIME ) );
    if ( Filter::CURRENT_TIME == toTime.GetTime() )
    {
        // The "to" time filter is disabled - that is, the filter is in "current" mode. Need to disable the to date and
        // time selection boxes, and check the "current" box
        m_toCheck.SetCheck(UNCHECKED);
        m_toDate.EnableWindow(false);
        m_toTime.EnableWindow(false);

        // And set the box to have the "current" time
        toTime = CTime::GetCurrentTime();
        m_toDate.SetTime(&toTime);
        m_toTime.SetTime(&toTime);

        // And set the valid time range
        m_fromTime.SetRange(NULL,&toTime);
        m_fromDate.SetRange(NULL,&toTime);
    }
    else
    {
        // The to time filter is enabled - so uncheck the "current" box, and set the date and time selection boxes accordingly.
        m_toCheck.SetCheck(CHECKED);
        m_toDate.SetTime(&toTime);
        m_toTime.SetTime(&toTime);

        // And set the valid time range
        m_fromTime.SetRange(NULL,&toTime);
        m_fromDate.SetRange(NULL,&toTime);

        // Also need to enable the date and time slection boxes in case they were previously disabled
        m_toDate.EnableWindow(true);
        m_toTime.EnableWindow(true);
    }

    // Get the from time from the filter object and set the date and time selection boxes accordingly
    CTime fromTime( getTimeFromNamedFilter( Filter::FILTER_FROMTIME ) );
    if ( Filter::PREVIOUS_DAY == fromTime.GetTime() )
    {
        // The "from" time filter is disabled - that is, the filter is only retrieving
        // events from the last 24 hours.
        m_fromCheck.SetCheck(UNCHECKED);
        m_fromDate.EnableWindow(false);
        m_fromTime.EnableWindow(false);

        // Set the boxes to have values 24 hours before "to" time.
        fromTime = toTime.GetTime() - TWENTY_FOUR_HOURS;
        m_fromTime.SetTime(&fromTime);
        m_fromDate.SetTime(&fromTime);

        // Use the from time to set the lower bound for the to time
        m_toTime.SetRange(&fromTime,NULL);
        m_toDate.SetRange(&fromTime,NULL);
    }
    else
    {
        // The to time filter is enabled - so uncheck the "current" box, and set the date and time selection boxes accordingly.
        m_fromCheck.SetCheck(CHECKED);
        m_fromDate.SetTime(&fromTime);
        m_fromTime.SetTime(&fromTime);

        // And set the valid time range
        m_toTime.SetRange(&fromTime,NULL);
        m_toDate.SetRange(&fromTime,NULL);

        // Also need to enable the date and time slection boxes in case they were previously disabled
        m_fromDate.EnableWindow(true);
        m_fromTime.EnableWindow(true);
    }



	// Wenguang++
	// Fix for TD14987
	// It might because the SetRange() cause the SetTime() not working properly for m_toTime
	// Add this part to make sure the time display correctly when load default filter
    if (UNCHECKED == m_toCheck.GetCheck() && UNCHECKED == m_fromCheck.GetCheck() )
    {
        // If its not checked make sure the 'to' time is the current time.
        toTime = CTime::GetCurrentTime();
        m_toDate.SetTime( &toTime );
        m_toTime.SetTime( &toTime );

		m_fromDate.SetRange( NULL, &toTime );
		m_fromTime.SetRange( NULL, &toTime );
		fromTime = CTime( toTime.GetTime() - TWENTY_FOUR_HOURS );
        m_fromDate.SetTime( &fromTime );
        m_fromTime.SetTime( &fromTime );
    }

    // And now do it all again for location...
	populateEditFromFilter( m_locationEdit, SOEEventFilter::FILTER_LOCATION_KEY, m_locations );


    // And now the asset and description
    std::vector<std::string> names;
	names = m_namedFilter.getStringFilter( SOEEventFilter::FILTER_ASSET_NAME );
    if ( 0 == names.size() )
    {
        names.push_back( "" );
    }
    m_assetEdit.SetWindowText( names[0].c_str() );

	names = m_namedFilter.getStringFilter( SOEEventFilter::FILTER_DISPLAY_VALUE );
    if ( 0 == names.size() )
    {
        names.push_back( "" );
    }
    m_txtDisplayValue.SetWindowText( names[0].c_str() );

	names = m_namedFilter.getStringFilter( SOEEventFilter::FILTER_DESCRIPTION );
	if ( 0 == names.size() )
	{
		names.push_back( "" );
	}
	m_txtDescription.SetWindowText( names[0].c_str() );
	

    // Now that the data is loaded events can be enabled.
    m_disableEvents = false;

	GetFilterData();   //TD14346
    FUNCTION_EXIT;
}


void CombinedEventFilterDlg::updateFilter()
{
    FUNCTION_ENTRY("updateFilter");

    // Load the new filter.
    loadFilter( true );

    // Make sure the combo box matches.
    populateNamedFilters();

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::enableButtons()
{
    // Enable buttons as required.
    m_okButton.EnableWindow(true);
    m_applyButton.EnableWindow(true);
    m_cancelButton.EnableWindow(true);
	m_saveFilterButton.EnableWindow(true);
}

CTime CombinedEventFilterDlg::getCombinedTime(const CTime& date,const CTime& time)
{
    // Convert the date and time into a single time entity.
    return CTime( date.GetYear(), date.GetMonth(), date.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond() );
}

void CombinedEventFilterDlg::populateNamedFilters()
{
    CString str;

    // Make sure the combo box is empty.
    m_namedFilterCombo.ResetContent();

    try
    {
        // Get a list of the filters.
        NamedFilterAccessFactory::FilterNameList filters = NamedFilterAccessFactory::getInstance().getAllFilterNames();

        // Now add each to the combo box.
        NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
        for ( namedFilterIt=filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
        {
            int index = m_namedFilterCombo.AddString( namedFilterIt->first.c_str() );
            if ( 0 <= index )
            {
                // If the data was inserted correctly store whether it is read only.
                m_namedFilterCombo.SetItemData( index, static_cast<int>(namedFilterIt->second) );

                // If it is the current filter then select it.
                if ( namedFilterIt->first == m_namedFilter.getName() )
                {
                    m_namedFilterCombo.SetCurSel( index );
                }
            }
        }
    }
    catch ( DataException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not get a list of named filters!");
    }

    // If the filter is custom it won't appear in the combo box, so add its text
    // manually.
    if ( NamedFilter::UNNAMED == m_namedFilter.getName() )
    {
        m_namedFilterCombo.AddTempString( NamedFilter::UNNAMED.c_str() );
    }

    // Now set the filter button states.
    enableNamedFilterButtons( m_namedFilter.getName() );
}

void CombinedEventFilterDlg::selectUserFilter()
{
    // Get the currently selected name.
    CString filterName;
    m_namedFilterCombo.GetWindowText( filterName );

    // Only select user defined filter if the current filter is read only or unnamed.
    ENamedFilterState state = getNamedFilterState( std::string(filterName) );
    if ( ( NF_USERDEFINED == state ) || ( NF_READONLY == state ) )
    {
        // Whenever the operator modifies the filter criteria the filter changes to 'User Defined'.
        m_namedFilterCombo.AddTempString( NamedFilter::UNNAMED.c_str() );

        // 'User Defined' cannot be saved or deleted.
        m_saveFilterButton.EnableWindow( FALSE );
        m_deleteFilterButton.EnableWindow( FALSE );
    }
}

time_t CombinedEventFilterDlg::getTimeFromNamedFilter( const std::string& timeIdentifier )
{
    time_t time = 0;

    // Get the time.
    NamedFilter::NumericList timeList = m_namedFilter.getNumericFilter( timeIdentifier );
    if ( 0 < timeList.size() )
    {
        time = static_cast<time_t>(timeList[0]);
    }

    return time;
}

CombinedEventFilterDlg::ENamedFilterState CombinedEventFilterDlg::getNamedFilterState( const std::string& namedFilter )
{
    ENamedFilterState state = NF_UNSAVED;

    if ( namedFilter == NamedFilter::UNNAMED )
    {
        state = NF_USERDEFINED;
    }
    else
    {
        // Search the combo box for the specified string.
        int index = m_namedFilterCombo.FindStringExact( 0, namedFilter.c_str() );
        if ( CB_ERR != index )
        {
            if ( static_cast<bool>(m_namedFilterCombo.GetItemData( index )) )
            {
                state = NF_READWRITE;
            }
            else
            {
                state = NF_READONLY;
            }
        }
    }

    return state;
}

void CombinedEventFilterDlg::enableNamedFilterButtons( const std::string& filterName )
{
    switch ( getNamedFilterState( filterName ) )
    {
    case NF_UNSAVED:
        m_saveFilterButton.EnableWindow( TRUE );
        m_deleteFilterButton.EnableWindow( FALSE );
        break;
    case NF_READWRITE:
		// TD #12583
		// Don't enable the Save button until a change has been made..
        m_saveFilterButton.EnableWindow( FALSE );
        m_deleteFilterButton.EnableWindow( TRUE );
        break;
    default:
        m_saveFilterButton.EnableWindow( FALSE );
        m_deleteFilterButton.EnableWindow( FALSE );
        break;
    };
}
//Begin-- TD14346
void CombinedEventFilterDlg::GetFilterData()
{
	 
	m_OldFilterData.m_strLocation.Empty();
	m_OldFilterData.m_strOperator.Empty();
	 
	m_OldFilterData.m_strAsset.Empty();
	m_OldFilterData.m_strDescription.Empty();
	 
	 
	m_OldFilterData.m_bFromTimeChecked = false;
	m_OldFilterData.m_bToTimeChecked = false;

	GetDlgItem(IDC_LOCATION)->GetWindowText(m_OldFilterData.m_strLocation);
	GetDlgItem(IDC_ASSET)->GetWindowText(m_OldFilterData.m_strAsset);
	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(m_OldFilterData.m_strDescription);
	m_OldFilterData.m_bFromTimeChecked =  UNCHECKED == m_fromCheck.GetCheck()?false:true;
	m_OldFilterData.m_bToTimeChecked = UNCHECKED == m_toCheck.GetCheck()?false:true;
	if (true == m_OldFilterData.m_bFromTimeChecked) {
		m_fromDate.GetTime(m_OldFilterData.m_ctFromDate);
		m_fromTime.GetTime(m_OldFilterData.m_ctFromTime);
	}
	if (true == m_OldFilterData.m_bToTimeChecked) {
		m_toDate.GetTime(m_OldFilterData.m_ctToDate);
		m_toTime.GetTime(m_OldFilterData.m_ctToTime);
	}
}
//End-- TD14346

//Begin--   TD14346
bool CombinedEventFilterDlg::IsFilterDataChanged()
{
	CString strTemp;
	GetDlgItem(IDC_LOCATION)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strLocation != strTemp)
		return true;
	GetDlgItem(IDC_ASSET)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strAsset != strTemp)
		return true;
	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strDescription != strTemp)
		return true;
	bool bTemp;

	bTemp =  UNCHECKED == m_fromCheck.GetCheck()?false:true;
	if (m_OldFilterData.m_bFromTimeChecked != bTemp) {
		return true;
	}
	else
	{
		CTime tmDate,tmTime;
		m_fromDate.GetTime(tmDate);
		m_fromTime.GetTime(tmTime);
		if (m_OldFilterData.m_ctToDate != tmDate || m_OldFilterData.m_ctFromTime != tmTime) {
			return true;
		}
	}
	bTemp = UNCHECKED == m_toCheck.GetCheck()?false:true;
	if (m_OldFilterData.m_bToTimeChecked != bTemp) {
		return true;
	}
	else
	{
		CTime tmDate,tmTime;
		m_toDate.GetTime(tmDate);
		m_toTime.GetTime(tmTime);
		if (m_OldFilterData.m_ctFromDate != tmDate || m_OldFilterData.m_ctToTime != tmTime) {
			return true;
		}
	}
	return false;
}
//End-- TD14346
