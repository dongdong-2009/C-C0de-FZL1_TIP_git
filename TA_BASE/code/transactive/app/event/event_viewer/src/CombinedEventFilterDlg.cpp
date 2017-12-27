/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventFilterDlg.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2013/04/25 16:33:00 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * CombinedEventFilterDlg is an MFC class deriving from the FilterDlg header (which is in turn a CDialog derived class).
 * This dialog provides filtering capabilities on the data items displayed in the combined event view.
 */

#pragma warning(disable:4786)       // identifier truncated to 255 characters
#pragma warning(disable:4800)       // forcing value to bool 'true' or 'false' (performance warning)

#include <afxtempl.h> // For the CArray
#include <algorithm>

#include "app/event/event_viewer/src/ConstNames.h"
#include "ATSComWrapper.h"
#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/EventViewerDlg.h"
#include "app/event/event_viewer/src/CombinedEventFilterDlg.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/SaveFilterDlg.h"


#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"


using namespace constnames;
using TA_Base_Core::DataException;
using TA_Base_Core::NamedFilterAccessFactory;
using TA_Base_Core::NamedFilter;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	const CTime EARLY_TIME_LIMIT(1);
    //const unsigned int TWENTY_FOUR_HOURS( 24*60*60 );
    //const std::string ERROR_LOAD( "load" );
    //const std::string ERROR_LOADED( "loaded" );
    //const std::string ERROR_SAVE( "save" );
    //const std::string ERROR_SAVED( "saved" );
    //const std::string ERROR_DELETE( "delete" );
    //const std::string ERROR_DELETED( "deleted" );
}

/////////////////////////////////////////////////////////////////////////////
// CombinedEventFilterDlg dialog

const int CombinedEventFilterDlg::UNCHECKED = 0;
const int CombinedEventFilterDlg::CHECKED = 1;
//const std::string CombinedEventFilterDlg::ALARM_FILTER_MESSAGE("No Alarm Filter");
const int CombinedEventFilterDlg::COMBO_LIMIT = 0;

// change from -1. As this value will be converted into "unsigned long", 
// and when it saved and loaded from DB, it may cause some problem in some oracle client.
// Using 0 can avoid these problems. 
const int CombinedEventFilterDlg::EVENT_ONLY_KEY = 0;  

//const char* CombinedEventFilterDlg::EVENT_ONLY_STRING = "Event";

CombinedEventFilterDlg::CombinedEventFilterDlg(CombinedEventFilter* filter, CWnd* pParent /*=NULL*/)
: FilterDlg(CombinedEventFilterDlg::IDD, pParent), m_filter(filter), m_disableEvents(true), m_dbAccess(DatabaseAccess::getInstance()),
      m_originalFilterName(m_filter->getNamedFilter()->getName())
{
	//{{AFX_DATA_INIT(CombinedEventFilterDlg)
	//}}AFX_DATA_INIT
	CoInitialize(0);
	m_someFilterChanged = false;
	m_originalFilterChanged = false;
}

CombinedEventFilterDlg::~CombinedEventFilterDlg()
{
	CoUninitialize();
}


void CombinedEventFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	FilterDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CombinedEventFilterDlg)
	DDX_Control(pDX, IDC_ALARM_TYPE, m_alarmtypeEdit);
	DDX_Control(pDX, IDC_EVENT_TYPE, m_eventtypeEdit);
	DDX_Control(pDX, IDC_SYSTEM, m_systemEdit);
	DDX_Control(pDX, IDC_ALARM_STATE, m_alarmStateEdit);
	DDX_Control(pDX, IDC_OPERATOR, m_operatorEdit);
	DDX_Control(pDX, IDC_FROM, m_fromCheck);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, ID_DEFAULT_FILTER, m_defaultButton);
	DDX_Control(pDX, ID_APPLY, m_applyButton);
	DDX_Control(pDX, IDC_ALARM_ID, m_alarmIdEdit);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionEdit);
	DDX_Control(pDX, IDC_ASSET, m_assetEdit);
	DDX_Control(pDX, IDC_LOCATION, m_locationEdit);
	DDX_Control(pDX, IDC_SUBSYSTEM, m_subsystemEdit);
	DDX_Control(pDX, IDC_TO_TIME, m_toTime);
	DDX_Control(pDX, IDC_TO_DATE, m_toDate);
	DDX_Control(pDX, IDC_CURRENT, m_toCheck);
	DDX_Control(pDX, IDC_FROM_TIME, m_fromTime);
	DDX_Control(pDX, IDC_FROM_DATE, m_fromDate);
	DDX_Control(pDX, IDC_COMBO_NAMEDFILTER, m_namedFilterCombo);
	DDX_Control(pDX, IDC_BUTTON_SAVEFILTER, m_saveFilterButton);
	DDX_Control(pDX, IDC_BUTTON_DELETEFILTER, m_deleteFilterButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO1, m_radio1);
	DDX_Control(pDX, IDC_RADIO2, m_radio2);
	DDX_Control(pDX, IDC_RADIO3, m_radio3);
	DDX_Control(pDX, IDC_RADIO4, m_radio4);
}


BEGIN_MESSAGE_MAP(CombinedEventFilterDlg, FilterDlg)
	//{{AFX_MSG_MAP(CombinedEventFilterDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CURRENT, OnTo)
	ON_BN_CLICKED(IDC_SUBSYSTEM_EXT, OnSubsystemExt)
	ON_BN_CLICKED(IDC_LOCATION_EX, OnLocationExt)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_BN_CLICKED(ID_DEFAULT_FILTER, OnDefaultFilter)
	ON_EN_CHANGE(IDC_ASSET, OnChangeAsset)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnChangeDescription)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TO_DATE, OnDatetimechangeToDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TO_TIME, OnDatetimechangeToTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_FROM_DATE, OnDatetimechangeFromDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_FROM_TIME, OnDatetimechangeFromTime)
	ON_LBN_KILLFOCUS(IDC_SEVERITY_LIST, OnKillfocusSeverityList)
	ON_CONTROL(CLBN_CHKCHANGE, IDC_SEVERITY_LIST, OnChkChange)
	ON_BN_CLICKED(IDC_FROM, OnFrom)
	ON_BN_CLICKED(IDC_EVENTTYPE_EXT, OnEventtypeExt)
	ON_BN_CLICKED(IDC_ALARMTYPE_EXT, OnAlarmtypeExt)
	ON_BN_CLICKED(IDC_OPERATOR_EXT, OnOperatorExt)
    ON_BN_CLICKED(IDC_BUTTON_SAVEFILTER, OnSaveFilter)
    ON_BN_CLICKED(IDC_BUTTON_DELETEFILTER, OnDeleteFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_NAMEDFILTER, OnSelchangeComboNamedFilter)
	ON_CBN_EDITCHANGE(IDC_COMBO_NAMEDFILTER,OnTextchangeNamedFilter)

	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_RADIO1, &CombinedEventFilterDlg::OnBnClickedRadio1)
//	ON_BN_CLICKED(IDC_RADIO2, &CombinedEventFilterDlg::OnBnClickedRadio2)
//ON_NOTIFY(BCN_HOTITEMCHANGE, IDC_RADIO1, &CombinedEventFilterDlg::OnBnHotItemChangeRadio1)
ON_BN_CLICKED(IDC_RADIO1, &CombinedEventFilterDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &CombinedEventFilterDlg::OnBnClickedRadio2)
ON_BN_CLICKED(IDC_RADIO3, &CombinedEventFilterDlg::OnBnClickedRadio3)
ON_BN_CLICKED(IDC_RADIO4, &CombinedEventFilterDlg::OnBnClickedRadio4)
ON_BN_CLICKED(IDC_ALARMTYPE_EXT2, &CombinedEventFilterDlg::OnAlarmStateExt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CombinedEventFilterDlg message handlers

void CombinedEventFilterDlg::OnClose() 
{
    FUNCTION_ENTRY("OnClose");

    m_filter->resumeCurrentListDataLoad();

    if ( !m_someFilterChanged )
    {
        AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_CANCEL);
    }
     else if ( m_originalFilterChanged )
    {
        OnOK(); // will re-apply the current filter because it's been updated.
        
    }
	 else
	 {
		AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_CLOSED);
	 }

    // And close this dialog
    FUNCTION_EXIT;
    FilterDlg::OnClose();
}

void CombinedEventFilterDlg::resetStartEndTimeDefined(FilterCriteria & filterCriteria)
{

	filterCriteria.StartTimePeriodDefined = false ;
	filterCriteria.EndTimePeriodDefined = false ;
// 	if( CHECKED == m_fromCheck.GetCheck() )
// 	{
// 		filterCriteria.StartTimePeriodDefined = true ;
// 	}
// 	if( CHECKED == m_toCheck.GetCheck())
// 	{
// 		filterCriteria.EndTimePeriodDefined = true ;
// 	}
	if( CHECKED == m_fromCheck.GetCheck() || CHECKED == m_toCheck.GetCheck() )
	{
		filterCriteria.StartTimePeriodDefined = true ;
		filterCriteria.EndTimePeriodDefined = true ;
	}

}

void CombinedEventFilterDlg::populateFilterCriteria(TA_Base_App::FilterCriteria & filterCriteria)
{
	m_filter->populateFilterCriteria(filterCriteria);  // this will populate many criteria of filter

	resetStartEndTimeDefined(filterCriteria);


	// set Alarm states to filterCriteria:
	if(filterCriteria.EventTypeEqualOr.size()>0)
	{
		filterCriteria.EventTypeEqualOr.clear();
	}
	setStringlistFromNumericNamedFilter( filterCriteria.EventTypeEqualOr, CombinedEventFilter::FILTER_ALARMSTATE, m_alarmState);



// 	setStringlistFromNumericNamedFilter( filterCriteria.SubsystemNameEqualOr, CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM, m_subsystems);
// 	setStringlistFromNumericNamedFilter( filterCriteria.SubsystemNameEqualOr, CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM, m_subsystems);
// 	//for ATS subsystem names ,just add them directley into filterCriteria.SubsystemNameEqualOr
// 	NamedFilter::StringList subsystemNames = m_namedFilter.getStringFilter(CombinedEventFilter::FILTER_SUBSYSTEM_STR);
// 	for(int i=0; i<subsystemNames.size(); i++)
// 	{
// 		filterCriteria.SubsystemNameEqualOr.push_back(subsystemNames[i]);
// 	}
// 	

}

void CombinedEventFilterDlg::OnApply()
{
    FUNCTION_ENTRY("OnApply");

    // Apply has been clicked. Need to apply the filters.
    m_filter->setNamedFilter( std::auto_ptr<NamedFilter>( m_namedFilter.clone() ) );

    // Enable and disable buttons as appropriate
    m_applyButton.EnableWindow(false);

    // Force the named filters to refresh.
    populateNamedFilters();

	/// add for 955: 
	FilterCriteria filterCriteria;
	populateFilterCriteria(filterCriteria);  

	if( !filterCriteria.StartTimePeriodDefined && !filterCriteria.EndTimePeriodDefined  )
	{
		filterCriteria.setPrevious1dayForStartTime();
		m_filter->startNewTask(filterCriteria, RealTimeMode);
	}
	else
	{
		filterCriteria.searchDirection = TA_Base_App::Backward;  // this is just the default value. can be deleted
		m_filter->startNewTask(filterCriteria,HistoricalMode);
	}
	//m_filter->setFilterCriteria(filterCriteria); // to save the current FilterCriteria 

    // Then notify the main dialog the filters have changed.
   AfxGetMainWnd()->PostMessage(WM_USER_FILTER_DLG_APPLY);

    FUNCTION_EXIT;
}

//set list<string>: System Names, subsystem names, operator names, location names to Filter Criteria
void CombinedEventFilterDlg::setStringlistFromNumericNamedFilter( list<std::string> & listToBeSet, const std::string columnName, ListItems & items)
{
	// set system names to filterCriteria:
	SelectedItems selection = m_namedFilter.getNumericFilter( columnName );
	if ( 0 != selection.size() )
	{
		SelectedItems::const_iterator selectionIt;
		for ( selectionIt=selection.begin(); selectionIt!=selection.end(); selectionIt++ )
		{
			// Get the string associated with the selected item.
			ListItem item( *selectionIt, "" );
			ListItems::iterator it = std::find( items.begin(), items.end(), item );

			if ( items.end() != it )
			{
				// Add it to the string.
				listToBeSet.push_back(it->str);
			}
			else
			{
				// What should we do in this unlikely event? Just ignore it for now.
			}
		}
	}
}

void CombinedEventFilterDlg::OnOK() 
{
    FUNCTION_ENTRY("OnOK");
	//updateTimes();

	m_filter->setNamedFilter( std::auto_ptr<NamedFilter>( m_namedFilter.clone() ) );

	EventViewerDlg * dlg = dynamic_cast<EventViewerDlg *>(GetMainWindow());

	// This will only delete the sorting icon in event viewer dialog
	dlg->clearSortMark();  

	// This will atually reset the sort info in DisplayItem which will used in sorting the eventstore list
	DisplayItem::setSortInfo(CombinedEventFilter::COLUMN_DATETIME, false); 

	/// start a new task: 
	FilterCriteria filterCriteria;
	populateFilterCriteria(filterCriteria);
	if( !filterCriteria.StartTimePeriodDefined && !filterCriteria.EndTimePeriodDefined  )
	{
		filterCriteria.setPrevious1dayForStartTime();
		m_filter->startNewTask(filterCriteria, RealTimeMode);
	}
	else
	{
		filterCriteria.searchDirection = TA_Base_App::Backward;
		m_filter->startNewTask(filterCriteria,HistoricalMode);
	}
	
	//m_filter->setFilterCriteria(filterCriteria); // to save the current FilterCriteria
	// Then notify the main dialog the filters have changed.
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

void CombinedEventFilterDlg::loadListDataFromCache()
{
	// to do:

	// get ISCS systems pkey/name from DataAccess cache and 
	// get ATS systems pkey/name from EventViewerDlg cache
	// and combined the 2 into (m_systems)
	loadSystemsListData();

	// get ISCS subsystems pkey/name from DataAccess cache and 
	// get ATS subsystems pkey/name from EventViewerDlg cache
	// and combined the 2 into (m_subsystems)
	SelectedItems emptySelected;
	retrieveSubsystemsBySeletedSystem(emptySelected);
	
	// Retrieve the locations and then populate the locations list.
	loadLocationKeyDisplayNameListData();

	// Retrieve the operators and then populate the operators list.
	loadOperatorListData();

	// load alarmStates to m_alarmState list
	loadAlarmStateListData();

	loadEventTypeListData();

	// Retrieve the alarm types and then populate the alarmtypes list.
	// Currently AlarmType is hidden in 3001 (it's only used in 4669), but for convenience, we still keep the code and prepare data for it.
	// If no requirement in the future, we might delete this part of code(including the code in DatabaseAccess->load4FilterDlg->loadAlarmTypes).
	loadAlarmTypeListData();

	// load alarmSeverities to m_alarmSeverities.
	m_alarmSeverities = m_dbAccess.getSeverities();

}

void CombinedEventFilterDlg::loadSystemsListData ()
{
	// clear m_system first:
	m_atsSystems.clear();
	m_systems.clear();

	//load ISCS system list Data
	std::vector< std::pair<unsigned long,std::string> > systems = m_dbAccess.getSystems();
	for ( int i=0; i<(int)systems.size(); i++ )
	{
		ListItem item( systems[i].first, systems[i].second );
		m_systems.push_back( item );
	}
	m_systems.sort();

	//load ATS system list Data from cache EventViewer::m_ATSSystemsVector:
    EventViewerDlg* evDlg = dynamic_cast<EventViewerDlg *>(GetMainWindow());
	std::vector<ATSSystemItem>::iterator itr;
	for(itr =evDlg->m_ATSSystemsVector.begin(); itr !=evDlg->m_ATSSystemsVector.end(); itr++)
	{
		ListItem item( itr->pkey, itr->systemName);   // to do: itr->pkey is a negative number, but ListItem is <unstigned long, string>
		m_atsSystems.push_back( item );	
	}

	//combine two list together:
	for (std::list<ListItem>::iterator itr = m_atsSystems.begin(); itr!=m_atsSystems.end(); itr++)
	{
		m_systems.push_back(*itr);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getSystems");

}


void CombinedEventFilterDlg::loadAlarmTypeListData()
{
	std::vector< std::pair<unsigned long,std::string> > alarmtypes = m_dbAccess.getAlarmTypes();
	for (int i=0; i<(int)alarmtypes.size(); i++ )
	{
		ListItem item( alarmtypes[i].first, alarmtypes[i].second );
		m_alarmtypes.push_back( item );
	}
	m_alarmtypes.sort();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getAlarmTypes");
}
void CombinedEventFilterDlg::loadAlarmStateListData()
{
	std::vector< std::pair<unsigned long,std::string> > alarmStates = m_dbAccess.getAlarmStates(); 
	for (int i=0; i<(int)alarmStates.size(); i++ )
	{
		ListItem item( alarmStates[i].first, alarmStates[i].second );
		m_alarmState.push_back( item );
	}
	m_alarmState.sort();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getAlarmStates");
}

void CombinedEventFilterDlg::loadLocationKeyDisplayNameListData ()
{
	std::vector< std::pair<unsigned long,std::string> > locations = m_dbAccess.getLocationKeyDispNamePairs();
	for (int i=0; i<(int)locations.size(); i++ )
	{
		ListItem item( locations[i].first, locations[i].second );
		m_locationKeyDisplayNameList.push_back( item );
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getLocations");
}


void CombinedEventFilterDlg::loadOperatorListData()
{
	std::vector< std::pair<unsigned long,std::string> > operators = m_dbAccess.getOperators();
	for (int i=0; i<(int)operators.size(); i++ )
	{
		ListItem item( operators[i].first, operators[i].second );
		m_operators.push_back( item );
	}
	m_operators.sort();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getOperators");
}

BOOL CombinedEventFilterDlg::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog");

    FilterDlg::OnInitDialog();

    // Set max length to same as database field
    m_assetEdit.SetLimitText(80);
    m_descriptionEdit.SetLimitText(300);

    // Load data required for multiple selections.
	m_namedFilter = *(m_filter->getNamedFilter());
    
	loadListDataFromCache();  // for subsystem: should be filtered by system selections.

	// Populate the dialog fields with correct data
	setUpSeverities( m_alarmSeverities );

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


void CombinedEventFilterDlg::retrieveSubsystemsBySeletedSystem (SelectedItems selectedSystemKeys)
{
	// all system are selected:
	if( selectedSystemKeys.size() == 0 ||
		selectedSystemKeys.size() == m_systems.size())
	{
		loadSubsystemsListDataAll();
		return;
	}

	m_ISCSsubsystems.clear();
	m_ATSsubsystems.clear();
	m_subsystems.clear();

	if(selectedSystemKeys.size()>0)
	{
		EventViewerDlg * dlg = dynamic_cast<EventViewerDlg *> (GetMainWindow());
		std::vector<ISCSSubsystemItem>::iterator itr;
		
		// load ISCS subsystem by selected system key, from EventViewerDlg::m_ISCSsubsystemsVector
		// for each ISCS subsystem ( not filtered)
		for( itr=dlg->m_ISCSsubsystemsVector.begin() ; itr!=dlg->m_ISCSsubsystemsVector.end(); itr++)  
		{
			unsigned long currentSysKey = itr->system_key;

			for ( int i=0 ;i <(int)selectedSystemKeys.size(); i++) 
			{
				if (selectedSystemKeys[i]== currentSysKey)
				{
					//means this subsytem is under one of the selected system, so should be loaded
					ListItem tempItem(itr->pkey, itr->subsystemName);
					m_ISCSsubsystems.push_back(tempItem);
					break;
				}
			}
		}

		// load ATS subsystem by selected system key, from EventViewerDlg::m_ATSSystemsVector
		vector<ATSSystemItem>::iterator ATS_itr;
		for( ATS_itr=dlg->m_ATSSystemsVector.begin() ; ATS_itr!=dlg->m_ATSSystemsVector.end(); ATS_itr++)  
		{
			unsigned long currentSysKey = ATS_itr->pkey;

			for ( int i=0 ;i <(int)selectedSystemKeys.size(); i++) 
			{
				if (selectedSystemKeys[i]== currentSysKey)
				{
					//means this subsytem is under one of the selected system, so should be loaded
					std::vector<pair<int , std::string >> tempVector =  ATS_itr->subsystemVector;
					for( int j=0; j<(int)tempVector.size(); j++)
					{
						ListItem tempItem(tempVector[j].first,tempVector[j].second );
						m_ATSsubsystems.push_back(tempItem);
					}
					break;
				}
			}
		}
	}

	m_ISCSsubsystems.sort();

	//combined into m_subsystems
	ListItems::iterator ListItems_itr;
	for ( ListItems_itr=m_ISCSsubsystems.begin(); ListItems_itr!= m_ISCSsubsystems.end(); ListItems_itr++)
	{
		m_subsystems.push_back(*ListItems_itr);
	}

	for ( ListItems_itr=m_ATSsubsystems.begin(); ListItems_itr!= m_ATSsubsystems.end(); ListItems_itr++)
	{
		m_subsystems.push_back(*ListItems_itr);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadSubsystemsListDataBySeleted");
}


// should be called only when selectedSystems has no content.
void CombinedEventFilterDlg::loadSubsystemsListDataAll ()
{
	m_ISCSsubsystems.clear();
	m_ATSsubsystems.clear();
	m_subsystems.clear();

	EventViewerDlg * dlg = dynamic_cast<EventViewerDlg *>(GetMainWindow());
	std::vector<ISCSSubsystemItem>::iterator itr;

	// load ISCS subsystem by selected system key, from EventViewerDlg::m_ISCSsubsystemsVector
	// for each ISCS subsystem ( not filtered)
	for( itr=dlg->m_ISCSsubsystemsVector.begin() ; itr!=dlg->m_ISCSsubsystemsVector.end(); itr++)  
	{
		ListItem tempItem(itr->pkey, itr->subsystemName);
		m_ISCSsubsystems.push_back(tempItem);
	}

	// load ATS subsystem by selected system key, from EventViewerDlg::m_ATSSystemsVector
	vector<ATSSystemItem>::iterator ATS_itr;
	for( ATS_itr=dlg->m_ATSSystemsVector.begin() ; ATS_itr!=dlg->m_ATSSystemsVector.end(); ATS_itr++)  
	{
		std::vector<pair<int , std::string >> tempVector =  ATS_itr->subsystemVector;
		for( int j=0; j<(int)tempVector.size(); j++)
		{
			ListItem tempItem(tempVector[j].first,tempVector[j].second );
			m_ATSsubsystems.push_back(tempItem);
		}
	}

	m_ISCSsubsystems.sort();

	//combined into m_subsystems
	ListItems::iterator ListItems_itr;
	for ( ListItems_itr=m_ISCSsubsystems.begin(); ListItems_itr!= m_ISCSsubsystems.end(); ListItems_itr++)
	{
		m_subsystems.push_back(*ListItems_itr);
	}

	for ( ListItems_itr=m_ATSsubsystems.begin(); ListItems_itr!= m_ATSsubsystems.end(); ListItems_itr++)
	{
		m_subsystems.push_back(*ListItems_itr);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadSubsystemsListDataAll");
}

void CombinedEventFilterDlg::populateTMTypeFromFilter ()
{
	//clear the check first:
	m_radio1.SetCheck(0);
	m_radio2.SetCheck(0);
	m_radio3.SetCheck(0);
	m_radio4.SetCheck(0);

	if( m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_TMTYPE ).size()> 0){
		string tmType = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_TMTYPE )[0];
		if(tmType == "O"){
			m_radio1.SetCheck(1);
		}else if(tmType == "M"){
			m_radio2.SetCheck(1);
		}else if(tmType == "B"){
			m_radio3.SetCheck(1);
		}else {
			m_radio4.SetCheck(1);
		}
	}else{
		m_radio4.SetCheck(1);
	}
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
		 listStr = ANGULAR_BRACKETS_ALL;

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

void CombinedEventFilterDlg::setUpSeverities(std::vector<std::pair<unsigned long,std::string> > dataItems)
{
    FUNCTION_ENTRY("setUpSeverities");

    CRect rect;
    GetDlgItem(IDC_LIST_PLACEHOLDER)->GetWindowRect(rect);
    ScreenToClient(rect);
    if(!m_severityList.CreateEx(WS_EX_CLIENTEDGE, _T("LISTBOX"), "Severity List", 
        LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_DISABLENOSCROLL,
        rect, this, IDC_SEVERITY_LIST))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not create Severity List Box");
        GetDlgItem(IDC_LIST_PLACEHOLDER)->ShowWindow(SW_HIDE);
        return;
    }

    GetDlgItem(IDC_LIST_PLACEHOLDER)->ShowWindow(SW_HIDE);
	
	// TD #12567
	// Hide the 'severity' list and the border frame around it
	// if NO_SEVERITY_FILTER has been defined
	// Now using RUN_PARAM instead
	if (TA_Base_Core::RunParams::getInstance().isSet( EventConstants::RPARAM_NOSEVERITY.c_str()))
	{
		    m_severityList.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_SEVERITY)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_severityList.ShowWindow(SW_SHOW);
	}
	
    for (unsigned int i = 0; i < dataItems.size();i++)
    {
        m_severityList.InsertString(i, dataItems[i].second.c_str());
        m_severityList.SetItemData(i, dataItems[i].first);
    }

    // Insert "Event" into the list
    m_severityList.InsertString(dataItems.size(), EVENT_ONLY_STRING);
    m_severityList.SetItemData(dataItems.size(), EVENT_ONLY_KEY);

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

void CombinedEventFilterDlg::OnSubsystemExt() 
{
    FUNCTION_ENTRY("OnSubsystemExt");
    // Called when the "..." next to the "Subsystem" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_SUBSYSTEM_TITLE);

    // Populate the list with the loaded subsystems.
    loadDlg.setItems( m_subsystems, true );

    // The selection is a combination of filters from application and physical subsystems.
    SelectedItems subsystems = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM );
    SelectedItems physicalSubsystems = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM );
	NamedFilter::StringList subsystemsStrListOld = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_SUBSYSTEM_STR );
	SelectedItems atsSubsystemPkeys = getATSSubsystemPkeysByNames(subsystemsStrListOld);
    subsystems.insert( subsystems.end(), physicalSubsystems.begin(), physicalSubsystems.end() );
	subsystems.insert( subsystems.end(), atsSubsystemPkeys.begin(), atsSubsystemPkeys.end() );

    // Set the current selections.
/*	if(subsystems.size()==m_subsystems.size())*/
	if(subsystemsStrListOld.size()==0)
	{
		SelectedItems noItems;
		loadDlg.setSelection( noItems );
	}else
	{
		loadDlg.setSelection( subsystems );
	}
    

    // Display the dialog.
    if( loadDlg.DoModal() == IDOK )
    {
        // Get the current selection.
        SelectedItems selection = loadDlg.getSelection();

        // Clear the filters.
        m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM );
        m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM );
		m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_SUBSYSTEM_STR);

		// if selection.size==0
		SelectedItems selectedSystems = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_ISCS_SYSTEM ); 
		if( selection.size() == 0 && selectedSystems.size()>0)
		{
			SelectedItems allSubsystems,appSubsystems,physicalSubsystems, atsSubsystems;
			getAllSubsystemKeys(allSubsystems);
			m_dbAccess.separateSubsystems( allSubsystems, appSubsystems, physicalSubsystems, atsSubsystems );

			// Apply the filters.
			if ( !appSubsystems.empty() )
			{
				m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM, appSubsystems );
			}
			if ( !physicalSubsystems.empty() )
			{
				m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM, physicalSubsystems );
			}
// 			if( !atsSubsystems.empty() )
// 			{
// 				NamedFilter::StringList atsSubsystemStrList = getATSSubsystemNamesByPKeys(atsSubsystems);
// 				m_namedFilter.setStringFilter( CombinedEventFilter::FILTER_ATS_SUBSYSTEM_STR, atsSubsystemStrList );
// 			}
		}
		else
		{
			// Need to distinguish between application and physical subsystems.
			subsystems.clear();
			physicalSubsystems.clear();
			atsSubsystemPkeys.clear();
			m_dbAccess.separateSubsystems( selection, subsystems, physicalSubsystems, atsSubsystemPkeys );

			// Apply the filters.
			if ( !subsystems.empty() )
			{
				m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM, subsystems );
			}
			if ( !physicalSubsystems.empty() )
			{
				m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM, physicalSubsystems );
			}
// 			if( !atsSubsystemPkeys.empty() )
// 			{
// 				NamedFilter::StringList atsSubsystemNames = getATSSubsystemNamesByPKeys(atsSubsystemPkeys);
// 				m_namedFilter.setStringFilter( CombinedEventFilter::FILTER_ATS_SUBSYSTEM_STR, atsSubsystemNames );
// 			}
			NamedFilter::StringList subsystemStrListNew;
			SelectedItems::const_iterator selectionIt;
			for ( selectionIt=selection.begin(); selectionIt!=selection.end(); selectionIt++ )
			{
				// Get the string associated with the selected item.
				ListItem item( *selectionIt, "" );
				ListItems::iterator it = std::find( m_subsystems.begin(), m_subsystems.end(), item );

				if ( m_subsystems.end() != it )
				{
					// Add it to the string.
					subsystemStrListNew.push_back(it->str);
				}

			}

			if(!subsystemStrListNew.empty())
			{
				m_namedFilter.setStringFilter(CombinedEventFilter::FILTER_SUBSYSTEM_STR,subsystemStrListNew);
			}

		}


        // Populate the edit control with the new subsystems.
        populateEditFromSelection( m_subsystemEdit, selection, m_subsystems );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}

NamedFilter::StringList CombinedEventFilterDlg::getStrVectorFromStrList(list<std::string> strList)
{
	NamedFilter::StringList strVector;
	for(list<std::string>::iterator itr=strList.begin();itr!=strList.end(); itr++)
	{
		strVector.push_back( *itr);
	}
	return strVector;
}

ListItems CombinedEventFilterDlg::getListItmesFromVector(std::vector<std::pair<unsigned long,std::string> > keyNamePairs)
{
	ListItems listToRet;
	for (int i=0; i<(int)keyNamePairs.size(); i++ )
	{
		ListItem item( keyNamePairs[i].first, keyNamePairs[i].second );
		listToRet.push_back( item );
	}
	return listToRet;
}

void CombinedEventFilterDlg::OnLocationExt() 
{
    FUNCTION_ENTRY("OnLocationExt");
    // Called when the "..." next to the "Location" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Location...");
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_LOCATION_TITLE);

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_locationKeyDisplayNameList, true );

    // Set the current selections.
    loadDlg.setSelection( m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_LOCATION ) );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

        // If selection changed, update selection and repopulate edit control.
		m_namedFilter.removeNumericFilter(CombinedEventFilter::FILTER_LOCATION);
		m_namedFilter.removeStringFilter(CombinedEventFilter::FILTER_LOCATION_STR);
		m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_LOCATION, selection );
		list<std::string> locationNames;
		ListItems locationKeyNameList = getListItmesFromVector(m_dbAccess.getLocationsKeyNamePairs());
		setStringlistFromNumericNamedFilter( locationNames, CombinedEventFilter::FILTER_LOCATION, locationKeyNameList );
		NamedFilter::StringList locationNamesStrList = getStrVectorFromStrList(locationNames);
		m_namedFilter.setStringFilter(CombinedEventFilter::FILTER_LOCATION_STR, locationNamesStrList);

        populateEditFromSelection( m_locationEdit, selection, m_locationKeyDisplayNameList );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnOperatorExt() 
{
    FUNCTION_ENTRY("OnOperatorExt");
    // Called when the "..." next to the "Operator" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Operator...");
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_OPERATOR_TITLE);

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_operators, true );

    // Set the current selections.
    loadDlg.setSelection( m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_OPERATOR ) );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

        // If selection changed, update selection and repopulate edit control.
		m_namedFilter.removeNumericFilter(CombinedEventFilter::FILTER_OPERATOR);
		m_namedFilter.removeStringFilter(CombinedEventFilter::FILTER_OPERATOR_STR);
        m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_OPERATOR, selection );
		list<std::string> operatorNames;
		setStringlistFromNumericNamedFilter( operatorNames, CombinedEventFilter::FILTER_OPERATOR,m_operators );
		NamedFilter::StringList operatorStrList = getStrVectorFromStrList(operatorNames);
		m_namedFilter.setStringFilter(CombinedEventFilter::FILTER_OPERATOR_STR, operatorStrList);

        populateEditFromSelection( m_operatorEdit, selection, m_operators );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}

SelectedItems CombinedEventFilterDlg::getATSSystemPkeysByNames(NamedFilter::StringList atsSystemsStrList)
{
	SelectedItems atsPkeyList;
	for(int i=0; i<(int)atsSystemsStrList.size(); i++)
	{
		for(std::list<ListItem>::iterator itr = m_atsSystems.begin(); itr!=m_atsSystems.end(); itr++)
		{
			if ( itr->str.compare(atsSystemsStrList[i]) ==0)
			{
				atsPkeyList.push_back(itr->pkey);
				break;
			}
		}
	}

	return atsPkeyList;
}

SelectedItems CombinedEventFilterDlg::getATSSubsystemPkeysByNames(NamedFilter::StringList atsSubsystemsStrList)
{
	SelectedItems atsPkeyList;
	for(int i=0; i<(int)atsSubsystemsStrList.size(); i++)
	{
		for(std::list<ListItem>::iterator itr = m_ATSsubsystems.begin(); itr!=m_ATSsubsystems.end(); itr++)
		{
			if ( itr->str.compare(atsSubsystemsStrList[i]) ==0)
			{
				atsPkeyList.push_back(itr->pkey);
				break;
			}
		}
	}

	return atsPkeyList;
}
NamedFilter::StringList CombinedEventFilterDlg::getATSSystemNamesByPKeys(SelectedItems atsSystemPkeys)
{
	NamedFilter::StringList atsNameList;
	for(int i=0; i<(int)atsSystemPkeys.size(); i++)
	{
		for(std::list<ListItem>::iterator itr = m_atsSystems.begin(); itr!=m_atsSystems.end(); itr++)
		{
			if ( itr->pkey == atsSystemPkeys[i] )
			{
				atsNameList.push_back(itr->str);
				break;
			}
		}
	}

	return atsNameList;
}

NamedFilter::StringList CombinedEventFilterDlg::getATSSubsystemNamesByPKeys(SelectedItems atsSubsystemPkeys)
{
	NamedFilter::StringList atsNameList;
	for(int i=0; i<(int)atsSubsystemPkeys.size(); i++)
	{
		for(std::list<ListItem>::iterator itr = m_ATSsubsystems.begin(); itr!=m_ATSsubsystems.end(); itr++)
		{
			if ( itr->pkey == atsSubsystemPkeys[i] )
			{
				atsNameList.push_back(itr->str);
				break;
			}
		}
	}

	return atsNameList;
}

void CombinedEventFilterDlg::OnEventtypeExt() 
{
    FUNCTION_ENTRY("OnEventtypeExt");
    // Called when the "..." next to the "Event Type" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Event Type...");
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_EVTYPE_TITLE);

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_eventtypes, true );
    
    // Set the current selections.
	loadDlg.setSelection( m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_EVENTTYPE ) );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

        // If selection changed, update selection and repopulate edit control.
        m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_EVENTTYPE, selection );
        populateEditFromSelection( m_eventtypeEdit, selection, m_eventtypes );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnSystemExt() 
{
    FUNCTION_ENTRY("OnSystemExt");
    // Called when the "..." next to the "Event Type" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Event Type...");
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_EVTYPE_TITLE);

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_systems, true );
    
	// The selection is a combination of filters from application and physical subsystems.
	SelectedItems systems = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_ISCS_SYSTEM );
	NamedFilter::StringList systemsStrListOld = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_SYSTEM_STR );
	SelectedItems atsSystemsPkey = getATSSystemPkeysByNames(systemsStrListOld);

	systems.insert(systems.end(), atsSystemsPkey.begin(), atsSystemsPkey.end());
    // Set the current selections.
    loadDlg.setSelection(systems );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

		// Clear the filters.
		m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_ISCS_SYSTEM );
		m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_SYSTEM_STR );

		// set filter: FILTER_ISCS_SYSTEM
		SelectedItems ISCS_selection;
		for (int i=0;i<(int)selection.size(); i++)
		{
			// if the selection is not an ATS's pkey, then keep it to filter:FILTER_ISCS_SYSTEM
			ListItem item( selection[i], "");
			ListItems::iterator it = std::find( m_atsSystems.begin(), m_atsSystems.end(), item );
			if(m_atsSystems.end() == it)
			{
				ISCS_selection.push_back(selection[i]);
			}
		}
		if (ISCS_selection.size()>0)
		{
			m_namedFilter.setNumericFilter(CombinedEventFilter::FILTER_ISCS_SYSTEM, ISCS_selection);
		}

		// set filter: FILTER_SYSTEM_STR
	    NamedFilter::StringList systemStrListNew;
		SelectedItems::const_iterator selectionIt;
		for ( selectionIt=selection.begin(); selectionIt!=selection.end(); selectionIt++ )
		{
			// Get the string associated with the selected item.
			ListItem item( *selectionIt, "" );
			ListItems::iterator it = std::find( m_systems.begin(), m_systems.end(), item );

			if ( m_systems.end() != it )
			{
				// Add it to the string.
				systemStrListNew.push_back(it->str);
			}

		}
		if(!systemStrListNew.empty())
		{
			m_namedFilter.setStringFilter(CombinedEventFilter::FILTER_SYSTEM_STR,systemStrListNew);
		}


		// Populate the edit control with the new subsystems.
		populateEditFromSelection( m_systemEdit, selection, m_systems );

		//dynamic change subsystems after user selected systems:
		setSubsystemsBySystemkeys ( selection );

        // Now enable the buttons.
        enableButtons();

        // Now that the filter is changed it is user defined.
        selectUserFilter();
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::getAllSubsystemKeys(SelectedItems &selection)
{
	selection.clear();
	for(std::list<ListItem>::iterator itr=m_subsystems.begin(); itr!=m_subsystems.end(); itr++)
	{
		selection.push_back(itr->pkey);
	}
}

// reset m_ISCSsubsystems/ m_ATSsubsystems/m_subsystems, reset m_substemEdit to "ALL", reset 3 NumericFilters
void CombinedEventFilterDlg::setSubsystemsBySystemkeys ( SelectedItems selectedSystemKeys)
{
	//reload m_ISCSsubsytems, m_atsSubsystems, m_subsystem 
	retrieveSubsystemsBySeletedSystem(selectedSystemKeys);

	// Populate the edit control : show 'all' in subsystemEdit
	SelectedItems selection;
	populateEditFromSelection( m_subsystemEdit, selection, m_subsystems);

	// Clear the filters:
	m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM );
	m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM );
	m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_SUBSYSTEM_STR );

	// reset the namedFilter:
	if(selectedSystemKeys.size()>0 )  //some systems has been selected:
	{
		SelectedItems subsystems,appSubsystems,physicalSubsystems, atsSubsystems;
		getAllSubsystemKeys(subsystems);
		m_dbAccess.separateSubsystems( subsystems, appSubsystems, physicalSubsystems, atsSubsystems );

		// Apply the filters.
		if ( !appSubsystems.empty() )
		{
			m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM, appSubsystems );
		}
		if ( !physicalSubsystems.empty() )
		{
			m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM, physicalSubsystems );
		}
// 		if( !atsSubsystems.empty() )
// 		{
// 			NamedFilter::StringList atsSubsystemsStrList = getATSSubsystemNamesByPKeys (atsSubsystems);
// 			m_namedFilter.setStringFilter( CombinedEventFilter::FILTER_ATS_SUBSYSTEM_STR, atsSubsystemsStrList );
// 		}

	}
}

void CombinedEventFilterDlg::OnAlarmtypeExt() 
{
    FUNCTION_ENTRY("OnAlarmtypeExt");
    // Called when the "..." next to the "Alarm Type" edit box is clicked.

    // Create a MultipleLoadDlg - this will hold the list providing the selections.
    //MultipleLoadDlg loadDlg("Select Alarm Type...");
	MultipleLoadDlg loadDlg(MULTIPLE_DLG_ALARMTYPE_TITLE);

    // Populate the list with the loaded locations.
    loadDlg.setItems( m_alarmtypes, true );

    // Set the current selections.
    loadDlg.setSelection( m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_ALARMTYPE ) );

    // Display the dialog.
    if ( loadDlg.DoModal() == IDOK )
    {
        // Get the list of selected items.
        SelectedItems selection = loadDlg.getSelection();

        // If selection changed, update selection and repopulate edit control.
        m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_ALARMTYPE, selection );
        populateEditFromSelection( m_alarmtypeEdit, selection, m_alarmtypes );

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
    m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_ASSET );
    if ( !text.IsEmpty() )
    {
        m_namedFilter.addStringFilter( CombinedEventFilter::FILTER_ASSET, std::string(text) );
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
    m_descriptionEdit.GetWindowText( text );
    m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_DESCRIPTION );
    if ( !text.IsEmpty() )
    {
        m_namedFilter.addStringFilter( CombinedEventFilter::FILTER_DESCRIPTION, std::string(text) );
    }

    FUNCTION_EXIT;
}

void CombinedEventFilterDlg::OnKillfocusSeverityList() 
{
    // Remove the selection highlighting
    m_severityList.SetCurSel(-1);
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
    m_namedFilter.removeNumericFilter( CombinedEventFilter::FILTER_SEVERITY );
    for( unsigned long i = 0; i < static_cast<unsigned long>(m_severityList.GetCount()); ++i )
    {
        if( m_severityList.GetCheck(i) == 1 )
        {
            m_namedFilter.addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, m_severityList.GetItemData(i) );
        }
    }

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
		m_toDate.SetRange(&EARLY_TIME_LIMIT,&curTime);
		m_toTime.SetRange(&EARLY_TIME_LIMIT,&curTime);

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

	std::string old_filterName = "";
	ENamedFilterState namedFilterState = getNamedFilterState( filterName);
	if (NF_READWRITE == namedFilterState)
	{
		old_filterName = getOldFilterName(filterName);
	}

	filterNameCStr.MakeUpper();
	bool isUserORDefindORDefault = ( filterNameCStr.Find("USER") >= 0 
		|| filterNameCStr.Find("DEFAULT") >= 0 ||filterNameCStr.Find("DEFINED") >= 0 ) ? true : false;

	//bool isInComboList =  m_namedFilterCombo.FindStringExact(-1, filterNameCStr) >= 0 ? true : false;

	if ( NF_READONLY == namedFilterState 
		|| (NF_UNSAVED == namedFilterState && isUserORDefindORDefault) 
		||NF_USERDEFINED == namedFilterState ) // Can't save filters with these kind of names
	{
		// Display a message box to disallow these keywords as name
		TA_Base_Bus::TransActiveMessage userMsg;
		//const std::string saveEr("save an invalid name for");
		//const std::string fileEr("saved as " + filterName);
		const std::string saveEr(SAVE_ERR);
		const std::string fileEr(SAVE_AS + filterName);
		userMsg << saveEr;
		userMsg << fileEr;
		userMsg.showMsgBox(IDS_UE_040031);
		m_namedFilter.setName( NamedFilter::UNNAMED );
		return;
	}
	
	// This part is to delete old NamedFilter from database and create a new one
	if (  NF_READWRITE == namedFilterState ) {
		// TD12034: Should be prompted when overwriting an existing name.
		TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg << "overwrite '" + oldFilterName + "' with the current filter settings"; 
			userMsg << OVERWRITE_FILTER+ "'" + old_filterName + "'"; 
		if ( IDNO == userMsg.showMsgBox(IDS_UW_010017) )
		{
			return;
		}

		try
		{
			// Load the filter.
			std::auto_ptr<NamedFilter> loadedFilter( TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( old_filterName ) );
			loadedFilter->deleteNamedFilter();
		}
		catch(...)
		{
		}
	}
	

	//End-- TD14346
    try
    {
        // Set the name of the current filter to the name specified.
        m_namedFilter.setName(filterName );

        // Save the filter.
        m_namedFilter.saveNamedFilter();

		m_someFilterChanged = true;
		if(old_filterName.compare(m_originalFilterName) == 0)
		{
			m_originalFilterChanged = true;
		}
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
    populateNamedFilters(true);
	
	GetFilterData();// TD14346
    m_okButton.EnableWindow(true);

	// TD #12583
	// On a save, make no change to the Apply button, and make
	// sure the Save button is disabled
    //m_applyButton.EnableWindow(true);
	m_saveFilterButton.EnableWindow(false);

    FUNCTION_EXIT;
}

std::string CombinedEventFilterDlg::getOldFilterName (const std::string& new_filterName )
{
	std::string oldName="";
	int index = m_namedFilterCombo.FindStringExact( -1, new_filterName.c_str() );
	if(index>=0)
	{
		CString szname;
		m_namedFilterCombo.GetLBText(index,szname);
		oldName = (LPCTSTR)szname;
	}
	return oldName.c_str();
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
		m_originalFilterChanged = true;
    }

	populateNamedFilters(true);

	m_someFilterChanged= true;

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

// update time picker in GUI(m_fromDate ,m_fromTIme , m_toDate, m_toTime) and reset m_namedfilter (Totime, FromTime)
void CombinedEventFilterDlg::updateTimes() 
{
    FUNCTION_ENTRY("updateTimes");

    CTime date;
    CTime time;

    // Get the current time.
    CTime currentTime = CTime::GetCurrentTime();

    // TD11082: 'to' time cannot be more than current time.
    m_toDate.SetRange( &EARLY_TIME_LIMIT, &currentTime );
    m_toTime.SetRange( &EARLY_TIME_LIMIT, &currentTime );

    // Get the to time - it should now be less than current time.
    m_toDate.GetTime(date);
    m_toTime.GetTime(time);
    CTime toTime = getCombinedTime( date, time );

	// Get the from time - it should now be less than 'to' time.
	m_fromDate.GetTime(date);
	m_fromTime.GetTime(time);
	CTime fromTime = getCombinedTime( date, time );

    // reset the 'to'/'from' time to the named filter.
    m_namedFilter.removeNumericFilter( Filter::FILTER_TOTIME );
	m_namedFilter.removeNumericFilter( Filter::FILTER_FROMTIME );
	// TD 14914 not to get the current time but the 24hrs time set when the "from" check box is checked
	// ie. set the default current time, when both are unchecked
    if (UNCHECKED == m_toCheck.GetCheck())
    {
		if ( UNCHECKED == m_fromCheck.GetCheck())
		{
			// If its not checked make sure the 'to' time is the current time.
			toTime = currentTime;
			fromTime = CTime( toTime.GetTime() - TWENTY_FOUR_HOURS );

			m_fromDate.SetTime( &fromTime );
			m_fromTime.SetTime( &fromTime );
			m_toDate.SetTime( &toTime );
			m_toTime.SetTime( &toTime );

			m_namedFilter.addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long>(Filter::CURRENT_TIME) );
			m_namedFilter.addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(Filter::PREVIOUS_DAY) );
		}
		else
		{
// 			toTime = CTime( fromTime.GetTime() + TWENTY_FOUR_HOURS);
// 			if( toTime.GetTime() > currentTime.GetTime())
// 			{
// 				toTime = currentTime;
// 			}
			toTime = currentTime;  // according to SWDS, when "from" selected but "To" not selected, should retrieve events from startTime to current time.

			m_namedFilter.addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long>(toTime.GetTime()) );
			m_namedFilter.addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(fromTime.GetTime()) );
			
			m_toDate.SetTime( &toTime );
			m_toTime.SetTime( &toTime );
			
		}

    }
    else   // CHECKED == m_toCheck.GetCheck()
    {   
		if(UNCHECKED == m_fromCheck.GetCheck())
		{
			
			fromTime = CTime( toTime.GetTime() - TWENTY_FOUR_HOURS );
			m_fromDate.SetTime( &fromTime );
			m_fromTime.SetTime( &fromTime );
		}

		m_namedFilter.addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(fromTime.GetTime()) );		
		m_namedFilter.addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long>(toTime.GetTime()) );
    }

    // 'from' time cannot be more than 'to' time.
	m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toTime);
	m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toTime);


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
        m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toTime);
        m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toTime);
    }
    else
    {
        // The to time filter is enabled - so uncheck the "current" box, and set the date and time selection boxes accordingly.
        m_toCheck.SetCheck(CHECKED);
        m_toDate.SetTime(&toTime);
        m_toTime.SetTime(&toTime);

        // And set the valid time range
        m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toTime);
        m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toTime);

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

		m_fromDate.SetRange( &EARLY_TIME_LIMIT, &toTime );
		m_fromTime.SetRange( &EARLY_TIME_LIMIT, &toTime );
		fromTime = CTime( toTime.GetTime() - TWENTY_FOUR_HOURS );
        m_fromDate.SetTime( &fromTime );
        m_fromTime.SetTime( &fromTime );
    }

	// ++Wenguang

	// populate systemEdit...
	NamedFilter::NumericList systemIDs = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_ISCS_SYSTEM );
	NamedFilter::StringList atsSystemsStrList = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_SYSTEM_STR );
	SelectedItems atsSystemsPkey = getATSSystemPkeysByNames(atsSystemsStrList);
	systemIDs.insert(systemIDs.end(), atsSystemsPkey.begin(), atsSystemsPkey.end());

	populateEditFromSelection( m_systemEdit, systemIDs, m_systems );
	retrieveSubsystemsBySeletedSystem(systemIDs); 

	/*populateSubsystemEdit();*/
	// Get the subsystems ids
	NamedFilter::NumericList subsystemIDs = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM );
	NamedFilter::NumericList physicalSubsystems = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM );
	NamedFilter::StringList atsSubsystemsStrList = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_SUBSYSTEM_STR );
	SelectedItems atsSubsystemsPkey = getATSSubsystemPkeysByNames(atsSubsystemsStrList);
	subsystemIDs.insert( subsystemIDs.end(), physicalSubsystems.begin(), physicalSubsystems.end() );
	subsystemIDs.insert( subsystemIDs.end(), atsSubsystemsPkey.begin(), atsSubsystemsPkey.end() );
	/*if(subsystemIDs.size()<m_subsystems.size())*/
	if(atsSubsystemsStrList.size()>0)
	{
		populateEditFromSelection( m_subsystemEdit, subsystemIDs,m_subsystems );
	}
	else
	{
		NamedFilter::NumericList emptyList;
		populateEditFromSelection( m_subsystemEdit, emptyList,m_subsystems );
	}
	

    // And now do it all again for location...
    populateEditFromFilter( m_locationEdit, CombinedEventFilter::FILTER_LOCATION, m_locationKeyDisplayNameList );

    // Same for operator...
    populateEditFromFilter( m_operatorEdit, CombinedEventFilter::FILTER_OPERATOR, m_operators );

	// Same for eventtype...
	populateEditFromFilter( m_eventtypeEdit, CombinedEventFilter::FILTER_EVENTTYPE, m_eventtypes );

	// TMTYPE : set radio box status by m_namedFilter
	populateTMTypeFromFilter ();

    // And now the asset and description
    std::vector<std::string> names;
    names = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_ASSET );
    if ( 0 == names.size() )
    {
        names.push_back( "" );
    }
    m_assetEdit.SetWindowText( names[0].c_str() );

    names = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_DESCRIPTION );
    if ( 0 == names.size() )
    {
        names.push_back( "" );
    }
    m_descriptionEdit.SetWindowText( names[0].c_str() );

    // Severity
    std::vector<unsigned long> ids = m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_SEVERITY );
	// AnitaL : PW2970 : New filter requirements have severities as checkboxes
	// rather than combo box.
    for( int i = 0; i < m_severityList.GetCount(); ++i )
    {
        std::vector<unsigned long>::iterator it = std::find( ids.begin(), ids.end(), m_severityList.GetItemData(i) );
        if ( it != ids.end() )
        {
            m_severityList.SetCheck( i, 1 );
        }
        else
        {
            m_severityList.SetCheck( i, 0 );
        }
    }
    
    // Alarm Type
    populateEditFromFilter( m_alarmtypeEdit, CombinedEventFilter::FILTER_ALARMTYPE, m_alarmtypes );

    // Alarm Id
    names = m_namedFilter.getStringFilter( CombinedEventFilter::FILTER_ALARMID );
    if (0 < names.size())
    {
        m_alarmIdEdit.SetWindowText( names[0].c_str() );
    }
    else
    {
        m_alarmIdEdit.SetWindowText( ALARM_FILTER_MESSAGE.c_str() );
    }

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

void CombinedEventFilterDlg::populateNamedFilters(bool reload)
{
    CString str;

    // Make sure the combo box is empty.
    m_namedFilterCombo.ResetContent();

    try
    {
		NamedFilterAccessFactory::FilterNameList filters;

		EventViewerDlg * eventViewerDlg = dynamic_cast<EventViewerDlg*> (GetMainWindow());
		if(reload == true)
		{
			eventViewerDlg->m_FilterNameList = NamedFilterAccessFactory::getInstance().getAllFilterNames();
			
		}
		filters = eventViewerDlg->m_FilterNameList;
			
		


        // Now add each to the combo box.
        NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
        for ( namedFilterIt= filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
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
	m_OldFilterData.m_strSubsystem.Empty();
	m_OldFilterData.m_strLocation.Empty();
	m_OldFilterData.m_strOperator.Empty();
	m_OldFilterData.m_strSystem.Empty();
	m_OldFilterData.m_strAsset.Empty();
	m_OldFilterData.m_strDescription.Empty();
	m_OldFilterData.m_strAlarmType.Empty();
	m_OldFilterData.m_strAlarmID.Empty();
	m_OldFilterData.m_bFromTimeChecked = false;
	m_OldFilterData.m_bToTimeChecked = false;
	m_OldFilterData.m_TMType = "";

	GetDlgItem(IDC_SUBSYSTEM)->GetWindowText(m_OldFilterData.m_strSubsystem);
	GetDlgItem(IDC_LOCATION)->GetWindowText(m_OldFilterData.m_strLocation);
	GetDlgItem(IDC_OPERATOR)->GetWindowText(m_OldFilterData.m_strOperator);
	GetDlgItem(IDC_SYSTEM)->GetWindowText(m_OldFilterData.m_strSystem);
	GetDlgItem(IDC_ASSET)->GetWindowText(m_OldFilterData.m_strAsset);
	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(m_OldFilterData.m_strDescription);
	GetDlgItem(IDC_ALARM_TYPE)->GetWindowText(m_OldFilterData.m_strAlarmType);
	GetDlgItem(IDC_ALARM_ID)->GetWindowText(m_OldFilterData.m_strAlarmID);
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

	// set TM type to m_oldFilter:
	if( m_radio1.GetCheck() == TRUE )
	{
		m_OldFilterData.m_TMType = "O";
	}else if( m_radio2.GetCheck() == TRUE )
	{
		m_OldFilterData.m_TMType = "M";
	}else if( m_radio3.GetCheck() == TRUE )
	{
		m_OldFilterData.m_TMType = "B";
	}else if( m_radio4.GetCheck() == TRUE )
	{
		m_OldFilterData.m_TMType = "";
	}

}
//End-- TD14346

//Begin--   TD14346
bool CombinedEventFilterDlg::IsFilterDataChanged()
{
	CString strTemp;
	GetDlgItem(IDC_SUBSYSTEM)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strSubsystem != strTemp)
		return true;
	GetDlgItem(IDC_LOCATION)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strLocation != strTemp)
		return true;
	GetDlgItem(IDC_OPERATOR)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strOperator != strTemp)
		return true;
	GetDlgItem(IDC_SYSTEM)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strSystem != strTemp)
		return true;
	GetDlgItem(IDC_ASSET)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strAsset != strTemp)
		return true;
	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strDescription != strTemp)
		return true;
	GetDlgItem(IDC_ALARM_TYPE)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strAlarmType != strTemp)
		return true;
	GetDlgItem(IDC_ALARM_ID)->GetWindowText(strTemp);
	if(m_OldFilterData.m_strAlarmID != strTemp)
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
		if (m_OldFilterData.m_ctFromDate != tmDate || m_OldFilterData.m_ctFromTime != tmTime) {
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
		if (m_OldFilterData.m_ctToDate != tmDate || m_OldFilterData.m_ctToTime != tmTime) {
			return true;
		}
	}

	// add for TM type comparing:
	if( m_radio1.GetCheck() == TRUE &&  "O"!= m_OldFilterData.m_TMType )
	{
		return true;
	}else if( m_radio2.GetCheck() == TRUE &&  "M" !=  m_OldFilterData.m_TMType)
	{
		return true;
	}else if( m_radio3.GetCheck() == TRUE && "B" !=  m_OldFilterData.m_TMType )
	{
		return true;
	}else if( m_radio4.GetCheck() == TRUE &&  "" !=  m_OldFilterData.m_TMType)
	{
		return true;
	}

	return false;
}
//End-- TD14346



void CombinedEventFilterDlg::OnBnClickedRadio1()
{
	OnTMTypeRadioBoxChanged("O");
}

void CombinedEventFilterDlg::OnBnClickedRadio2()
{
	OnTMTypeRadioBoxChanged("M");
}

void CombinedEventFilterDlg::OnBnClickedRadio3()
{
	OnTMTypeRadioBoxChanged("B");

}

void CombinedEventFilterDlg::OnBnClickedRadio4()
{
	OnTMTypeRadioBoxChanged("");	
}
void CombinedEventFilterDlg::OnTMTypeRadioBoxChanged(std::string tmtype)
{
	if ( m_disableEvents )
	{
		return;
	}

	// Enable the buttons.
	enableButtons();

	// Now that the filter is changed it is user defined.
	selectUserFilter();

	m_namedFilter.removeStringFilter( CombinedEventFilter::FILTER_TMTYPE );
	if(""!= tmtype)
	{
		m_namedFilter.addStringFilter( CombinedEventFilter::FILTER_TMTYPE, tmtype);
	}
	

}
void CombinedEventFilterDlg::OnAlarmStateExt()
{
	FUNCTION_ENTRY("OnAlarmStateExt");
	// Called when the "..." next to the "Alarm Type" edit box is clicked.

	// Create a MultipleLoadDlg - this will hold the list providing the selections.
	MultipleLoadDlg loadDlg("Select Alarm State...");

	// Populate the list with the loaded locations.
	loadDlg.setItems( m_alarmState, true );

	// Set the current selections.
	loadDlg.setSelection( m_namedFilter.getNumericFilter( CombinedEventFilter::FILTER_ALARMSTATE ) );

	// Display the dialog.
	if ( loadDlg.DoModal() == IDOK )
	{
		// Get the list of selected items.
		SelectedItems selection = loadDlg.getSelection();

		// If selection changed, update selection and repopulate edit control.
		m_namedFilter.setNumericFilter( CombinedEventFilter::FILTER_ALARMSTATE, selection );
		populateEditFromSelection( m_alarmStateEdit, selection,m_alarmState );

		// Now enable the buttons.
		enableButtons();

		// Now that the filter is changed it is user defined.
		selectUserFilter();
	}

	FUNCTION_EXIT;
}

void CombinedEventFilterDlg::loadEventTypeListData()
{
	unsigned int i;
	// Retrieve the event types and then populate the eventtypes list.
	std::vector< std::pair<unsigned long,std::string> > eventtypes = m_dbAccess.getEventTypes(m_filter->getEventLevel());
	for ( i=0; i<eventtypes.size(); i++ )
	{
		ListItem item( eventtypes[i].first, eventtypes[i].second );
		m_eventtypes.push_back( item );
	}
	m_eventtypes.sort();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getEventTypes");
}

CWnd* CombinedEventFilterDlg::GetMainWindow()
{
	return AfxGetApp()->GetMainWnd();
}
