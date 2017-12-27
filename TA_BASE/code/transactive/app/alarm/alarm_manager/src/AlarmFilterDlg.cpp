/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// AlarmFilterDlg.cpp : implementation file
//
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "stdafx.h"
#include <sstream>
#include "Afxdtctl.h"
#include "app/alarm/alarm_manager/src/AlarmFilterDlg.h"
#include "app/alarm/alarm_manager/src/Resource.h"
#include "app/alarm/alarm_manager/src/Globals.h"

#include "bus\alarm\alarm_list_control\src\Filter.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus\mfc_extensions\src\coding\Encoder.h"		// chinese encode/decode

#include "core\exceptions\src\AlarmListCtrlException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"

#include "core\data_access_interface\src\ISubsystem.h"
#include "core\data_access_interface\src\SubsystemAccessFactory.h"
#include "core\data_access_interface\src\ILocation.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\data_access_interface\src\OperatorAccessFactory.h"
#include "core\data_access_interface\src\IAlarmSeverityData.h"
#include "core\data_access_interface\src\AlarmSeverityData.h"
#include "core\data_access_interface\src\AlarmSeverityAccessFactory.h"
#include "core\data_access_interface\src\IAlarmTypeData.h"
#include "core\data_access_interface\src\AlarmTypeAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"

#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/exceptions/src/DataException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using TA_Base_Core::NamedFilter;
using TA_Base_Bus::Encoder;

namespace // unnamed
{
    const CTime EARLY_TIME_LIMIT(1);
    const CTime LATEST_TIME_LIMIT(LONG_MAX);
    const CString TIME_PICKER_FORMAT("HH:mm:ss");

    const std::string sSave("save");
    const std::string sSaved("saved");

    const std::string sLoad("load");
    const std::string sLoaded("loaded");

    const std::string sDelete("delete");
    const std::string sDeleted("deleted");

	/////////////////////////////////////////////////////////////////////////////
	// This function constructs a new CTime from a date and a time CTime.
	CTime getCombinedTime(const CTime& date,const CTime& time)
	{    
		return CTime(date.GetYear(),date.GetMonth(),date.GetDay(),
						   time.GetHour(),time.GetMinute(),time.GetSecond() );
	}

	/////////////////////////////////////////////////////////////////////////////
	// This is to get the selected item from a list of items, 
	// e.g. the operator list dlg, the location list dlg, alarm list dlg, etc.
	CString getSelectedItemsString(const TA_Base_App::ListItems& selectedItems)
	{
		CString text;
		if(selectedItems.empty())
		{
			text = "<全部>";
		}
		else
		{
			for(TA_Base_App::ListItems::const_iterator itr = selectedItems.begin(); itr != selectedItems.end(); ++itr)
			{
				if (itr != selectedItems.begin())
				{
					text += ", ";
				}
				text += std::string(itr->str).c_str();
			}
		}
		return text;
	}

} // unnamed namespace

/////////////////////////////////////////////////////////////////////////////
// AlarmFilterDlg dialog
/////////////////////////////////////////////////////////////////////////////

namespace TA_Base_App
{
	////////////////////////////////////////////////////////////////////////
	// Constant string for use in the Named Filter Database
	//
	const std::string AlarmFilterDlg::FILTER_DEFAULT("默认过滤器");

	const std::string AlarmFilterDlg::FILTER_TOTIME("终止时间");
	const std::string AlarmFilterDlg::FILTER_FROMTIME("起始时间");
	const std::string AlarmFilterDlg::FILTER_SUBSYSTEM("子系统");
	const std::string AlarmFilterDlg::FILTER_LOCATION("位置");
	const std::string AlarmFilterDlg::FILTER_OPERATOR("操作员");
	const std::string AlarmFilterDlg::FILTER_ASSET("设备");
	const std::string AlarmFilterDlg::FILTER_DESCRIPTION("描述");
	const std::string AlarmFilterDlg::FILTER_SEVERITY("严重级别");
	const std::string AlarmFilterDlg::FILTER_ALARMTYPE("警报级别");

	const std::string AlarmFilterDlg::FILTER_ACKSTATE("确认状态");
	const std::string AlarmFilterDlg::FILTER_STATE("状态");
	const std::string AlarmFilterDlg::FILTER_OPEN("打开");
	const std::string AlarmFilterDlg::FILTER_CLOSE("关闭");
	const std::string AlarmFilterDlg::FILTER_ACKED("已确认");
	const std::string AlarmFilterDlg::FILTER_UNACKED("未确认");
	const std::string AlarmFilterDlg::FILTER_ALL("全部");
	
	//const int AlarmFilterDlg::ALARM_FILTER_DATA_INITIALIZE_TIMER = 1; //TD17081

	/////////////////////////////////////////////////////////////////////////////
	// Ctor
	AlarmFilterDlg::AlarmFilterDlg(TA_Base_Bus::AlarmListCtrl& alarmList, CWnd* pParent /*=NULL*/)
		: CDialog(AlarmFilterDlg::IDD, pParent),
		m_updateNeeded(false),
		m_subsystemDlg(NULL), m_locationDlg(NULL),
		m_operatorDlg(NULL), m_alarmTypeDlg(NULL),
		m_alarmList(alarmList),
		m_nCurFilterState(0),
		m_bNMFilterUpdated(false), m_strCurDlgFilter("")
	{
		//{{AFX_DATA_INIT(AlarmFilterDlg)
		//}}AFX_DATA_INIT
	}

	/////////////////////////////////////////////////////////////////////////////
	// Dtor
	AlarmFilterDlg::~AlarmFilterDlg()
	{
		if (m_subsystemDlg != NULL)
		{
			delete m_subsystemDlg;
			m_subsystemDlg = NULL;
		}
		if (m_locationDlg != NULL)
		{
			delete m_locationDlg;
			m_locationDlg = NULL;
		}
		if (m_operatorDlg != NULL)
		{
			delete m_operatorDlg;
			m_operatorDlg = NULL;
		}
		if (m_alarmTypeDlg != NULL)
		{
			delete m_alarmTypeDlg;
			m_alarmTypeDlg = NULL;
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(AlarmFilterDlg)
	DDX_Control(pDX, IDC_OK, m_okButton);
	DDX_Control(pDX, IDC_APPLY, m_applyButton);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TOTIME, m_toTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TODATE, m_toDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_FROMTIME, m_fromTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_FROMDATE, m_fromDate);
	DDX_Control(pDX, IDC_TO_TIME, m_toCheck);
	DDX_Control(pDX, IDC_FROM_TIME, m_fromCheck);
	DDX_Control(pDX, IDC_EDIT_ASSET, m_assetEdit);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_descEdit);
	DDX_Control(pDX, IDC_LOCATION_EDIT, m_locationEdit);
	DDX_Control(pDX, IDC_SUBSYSTEM_EDIT, m_subsystemEdit);
	DDX_Control(pDX, IDC_OPERATOR_EDIT, m_operatorEdit);
	DDX_Control(pDX, IDC_ALARM_TYPE_EDIT, m_alarmTypeEdit);
    DDX_Control(pDX, IDC_NAMED_FILTER, m_namedFilter);
    DDX_Control(pDX, IDC_SAVE, m_saveButton);
	DDX_Control(pDX, IDC_DELETE, m_deleteButton);
    DDX_Control(pDX, IDC_CANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_EDIT_FROM, m_editFrom);
    DDX_Control(pDX, IDC_EDIT_TO, m_editTo);
    DDX_Control(pDX, IDC_COMBO_FROM, m_comboFrom);
    DDX_Control(pDX, IDC_COMBO_TO, m_comboTo);
    DDX_Control(pDX, IDC_SPIN_FROM, m_spinFrom);
    DDX_Control(pDX, IDC_SPIN_TO, m_spinTo);
    //}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(AlarmFilterDlg, CDialog)
		//{{AFX_MSG_MAP(AlarmFilterDlg)
	ON_BN_CLICKED(IDC_CLEAR_FILTERS_BUTTON, OnClearFiltersButton)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_OK, OnOK)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//TD 11033 Ensure the same functions are called accordingly regardless of single or double click
	ON_BN_DOUBLECLICKED(IDC_CLEAR_FILTERS_BUTTON, OnClearFiltersButton)
	ON_BN_DOUBLECLICKED(IDC_DELETE, OnDelete)
	ON_BN_DOUBLECLICKED(IDC_APPLY, OnApply)
	ON_BN_DOUBLECLICKED(IDC_OK, OnOK)
	ON_BN_DOUBLECLICKED(IDC_CANCEL, OnCancel)

	ON_EN_CHANGE(IDC_EDIT_ASSET, OnChangeEditAsset)
	ON_EN_CHANGE(IDC_EDIT_DESCRIPTION, OnChangeEditDescription)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_FROMDATE, OnDatetimechangeDatetimepickerFromdate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_FROMTIME, OnDatetimechangeDatetimepickerFromtime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TODATE, OnDatetimechangeDatetimepickerTodate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TOTIME, OnDatetimechangeDatetimepickerTotime)
	ON_BN_CLICKED(IDC_SUBSYSTEM_BUTTON, OnSubsystemButton)
	ON_BN_CLICKED(IDC_LOCATION_BUTTON, OnLocationButton)
	ON_BN_CLICKED(IDC_OPERATOR_BUTTON, OnOperatorButton)
	ON_BN_CLICKED(IDC_ALARM_TYPE_BUTTON, OnAlarmTypeButton)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_FROM_TIME, OnFromCheckBoxClicked)
	ON_BN_CLICKED(IDC_TO_TIME, OnToCheckBoxClicked)
	ON_WM_SHOWWINDOW()
    ON_LBN_KILLFOCUS(IDC_SEVERITY_LIST, OnKillfocusSeverityList)
	ON_CONTROL(CLBN_CHKCHANGE, IDC_SEVERITY_LIST, OnChkChange)
	ON_CBN_SELCHANGE(IDC_NAMED_FILTER, OnSelchangeNamedFilter)
	ON_CBN_EDITCHANGE(IDC_NAMED_FILTER,OnTextchangeNamedFilter)
//	ON_WM_TIMER() //TD17081
    //}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_STATE_OPEN, IDC_STATE_ALL, OnStateRadioButton)
	ON_COMMAND_RANGE(IDC_ACK_ACKED, IDC_ACK_ALL, OnAckRadioButton)

	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AlarmFilterDlg message handlers
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	BOOL AlarmFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		// TD17081++ - moved the initialization of subsystems, locations, 
		// operators, alarmtypes and severities to the timer thread	
		retrieveSubsystems();
		retrieveLocations();
		retrieveOperators();
		retrieveAlarmTypes();
		retrieveSeverities();
		//SetTimer(ALARM_FILTER_DATA_INITIALIZE_TIMER, 0, NULL);
		// ++TD17081

		// setup font
		m_font.CreatePointFont(100,"宋体");
		m_namedFilter.SetFont(&m_font,FALSE);

        // Set the time format on the date/time pickers
        m_toTime.SetFormat(TIME_PICKER_FORMAT);
        m_fromTime.SetFormat(TIME_PICKER_FORMAT);

		//TD15028 ++ wangyn
		OnFromCheckBoxClicked();
		OnToCheckBoxClicked();
		//++ TD15028
		//m_noApply = false;
		
		//changxing  always make ok button enable.
		m_okButton.EnableWindow(true);

		m_namedFilter.LimitText(32); //TD15027 azenitha
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15027: Set limit of text");

        return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

// ++Remove the Timer initialization because it will create a risk of crashing
// the alarm manager because we cannot guarantee that these initialization will
// be done by the time we show the dialog
    /////////////////////////////////////////////////////////////////////////////
	//TD17081
// 	void AlarmFilterDlg::OnTimer(UINT nIDEvent)
// 	{
// 		if (nIDEvent == ALARM_FILTER_DATA_INITIALIZE_TIMER)
// 		{
// 			// Get the list of items for multiple selection dialogs
// 			retrieveSubsystems();
// 			retrieveLocations();
// 			retrieveOperators();
// 			retrieveAlarmTypes();
// 			retrieveSeverities();
//  		KillTimer(ALARM_FILTER_DATA_INITIALIZE_TIMER);
// 		}
// 	}

    /////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnOK() 
	{
		// Apply the filter.
		if (m_updateNeeded) OnApply();
			CDialog::OnOK();
	}

	/////////////////////////////////////////////////////////////////////////////
	int AlarmFilterDlg::DoModal()
	{
		return CDialog::DoModal();
	}

	/////////////////////////////////////////////////////////////////////////////
	// On clicking Default button
	//
	void AlarmFilterDlg::OnClearFiltersButton()
	{
		// Clear display
		clearAllDisplays();
		std::string filterName(m_filter.getFilterName());

		// Check whether it is already in default filter
		if ( filterName != AlarmFilterDlg::FILTER_DEFAULT && filterName != "" )
		{
			// Clear the filter
			m_filter.removeAll();
			m_filter.setFilterName(AlarmFilterDlg::FILTER_DEFAULT);

			// Reset the filter combo box
			populateNamedFilters(AlarmFilterDlg::FILTER_DEFAULT);

			m_updateNeeded = true;
		}
		// TD 11033: A case of no filter is set when new  or on initialisation, set it to default
		else if ( filterName == "" ) 
		{
			// Clear the filter
			m_filter.setFilterName(AlarmFilterDlg::FILTER_DEFAULT);

			// Reset the filter combo box
			populateNamedFilters(AlarmFilterDlg::FILTER_DEFAULT);

			m_updateNeeded = false;
		} 
		// TD 11033: A case of no filter is DEFAULT, set it to the filter to default
		// This step may not be necessary, but do it anyway. Don't change the m_updateNeeded, leave it alone!!
		// to whatever is set previous
		else if ( filterName == AlarmFilterDlg::FILTER_DEFAULT )
		{
			// Clear the filter
			m_filter.setFilterName(AlarmFilterDlg::FILTER_DEFAULT);

			// Reset the filter combo box
			populateNamedFilters(AlarmFilterDlg::FILTER_DEFAULT);
		}

		m_savedFilter = true; // Default filter need not be saved
		if (m_updateNeeded == true)
		{
			enableButtons();
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Clear or reset all controls and display
	//
	void AlarmFilterDlg::clearAllDisplays()
	{

		// Clears all filter selections.
		m_selectedSubsystems.clear();
		m_selectedLocations.clear();
		m_selectedOperators.clear();
		m_selectedAlarmTypes.clear();

		populateSubsystemsEditCtrl();
		populateLocationsEditCtrl();
		populateOperatorsEditCtrl();
		populateAlarmTypesEditCtrl();

		m_assetEdit.SetSel(0, -1);
		m_assetEdit.Clear();

		m_descEdit.SetSel(0, -1);
		m_descEdit.Clear();

		// Set the "All" radio buttons.
		setAckRadioButton(IDC_ACK_ALL);
		setStateRadioButton(IDC_STATE_ALL);

		// make sure the Ack/Closed radio buttons don't stay disabled if they previously were
		static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->EnableWindow(true);
		static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->EnableWindow(true);

		// Unset the from/to time check boxes.
		static_cast<CButton*>(GetDlgItem(IDC_FROM_TIME))->SetCheck(FALSE);
		static_cast<CButton*>(GetDlgItem(IDC_TO_TIME))->SetCheck(FALSE);

		// Disable the date/time controls.
		fromCheckBoxUpdated(false);
        toCheckBoxUpdated(false);

		// Check the severity checkboxes.
		SetAllSeverityFilterControls(TRUE);
	}
	
	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnApply() 
	{
		bool isDefault = false;
		if(m_updateNeeded) //check if anything is updated
		{
			// Clear the filter object of all old filters
			m_filter.removeAll();
			m_filterUserDef.removeAll();

			// No need to create filter if there is no filter ie. Default is set
			// Create the filter object, then tell the list control to filter on it.
			CString curName;
			m_namedFilter.GetWindowText(curName);
			
			std::string fName(curName);
			int index = 0;
			bool inFilterDB = checkInNamedFilterDatabase(fName);
			bool savedFilter = false;

			if ( curName != AlarmFilterDlg::FILTER_DEFAULT.c_str() )
			{	
				if (inFilterDB == true && m_savedFilter == true)
				{
					//This is a case that the current filter changes is saved
					savedFilter = true;
					index = m_namedFilter.GetCurSel();
					if(index>=0)
					m_namedFilter.GetLBText(index, curName);
				}
				else if (inFilterDB == true && m_savedFilter == false)
				{
					//This is a case that the current filter is changed and not saved
					//Display a message to remind user to save the filter
					savedFilter = false;

					// TD #11238
					// Revert the name back to "User Defined"
					curName = TA_Base_Core::NamedFilter::UNNAMED.c_str();
					m_namedFilter.SetWindowText(curName);
					m_saveButton.EnableWindow(false);
					m_deleteButton.EnableWindow(false);
				}
			}

			// This is for saved filter which is not Default and User Defined
			if ( curName != AlarmFilterDlg::FILTER_DEFAULT.c_str()  && curName != TA_Base_Core::NamedFilter::UNNAMED.c_str() && savedFilter == true )
			{
				m_filter = createFilter();
				m_filter.setFilterName(fName);
				index = index;
				filterIndex = INDEX_LIST;
			}
			// Set the name if it is a user defined one which is not saved
			else if ( curName == TA_Base_Core::NamedFilter::UNNAMED.c_str() ) 
			{
				m_filter = createFilter();
				m_filter.setFilterName(NamedFilter::UNNAMED);
				filterIndex = INDEX_USER_DEFINED;
				m_filterUserDef = m_filter;
			}
			// it must be a default filter name but check anyway
			else if ( curName == AlarmFilterDlg::FILTER_DEFAULT.c_str() )
			{
				// No need to create filter as no filter is need for default
				filterIndex = INDEX_DEFAULT;
				isDefault = true;
			}
			// This is a new filter that is not saved
			else if ( curName != AlarmFilterDlg::FILTER_DEFAULT.c_str()  && curName != TA_Base_Core::NamedFilter::UNNAMED.c_str() && savedFilter == false )
			{
				m_filter = createFilter();
				m_filter.setFilterName(fName);
				filterIndex = INDEX_USER_DEFINED; // User defined name will be refected in the parent dialog
			}

			try
			{
				//clear all the fitlers in the alarm list
				m_alarmList.setFilter(m_filter,isDefault);
				setUpdateNeeded(false);
				m_saveButton.EnableWindow( !m_savedFilter );
				m_nCurFilterState = 0;
				m_strCurDlgFilter = m_filter.getFilterName();
				// Then notify the main dialog the filters have changed depending on the filter name type
				AfxGetMainWnd()->PostMessage(WM_ON_FILTER_DLG_APPLY, filterIndex, index);	
			}
			catch(TA_Base_Core::AlarmListCtrlException)
			{
				TA_Base_Bus::TransActiveMessage userMsg;
				UINT selectedButton = userMsg.showMsgBox(IDS_UI_040022);
			}
		}
		m_applyButton.EnableWindow(false);
		// Nothing to be done if not updated
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnChangeEditAsset() 
	{
		setUpdateNeeded(true);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnChangeEditDescription() 
	{
		setUpdateNeeded(true);
	}

	void AlarmFilterDlg::OnTextchangeNamedFilter()
	{
		// if the text entered is the name of an existing filter (in the combo)
		CString str;
		m_namedFilter.GetWindowText(str);

		bool isCurFilter = str.Compare(m_filter.getFilterName().c_str()) ? false : true;
		
		str.MakeUpper();
		if(!str.Compare("") || str.Find("DEFAULT")>=0
			|| str.Find("USER")>=0)
		{
			m_saveButton.EnableWindow( false );
		}
		else if (isCurFilter && !m_updateNeeded) // there is no change in name and settings
		{
			m_saveButton.EnableWindow( false );
		}
		else
		{
			// TD #13187
			// don't call setUpdateNeeded here, otherwise the named filter combo
			// box will reset to 'User Defined' on every keystroke
			//setUpdateNeeded(true);
			m_updateNeeded = true;
			m_saveButton.EnableWindow( true );
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnDatetimechangeDatetimepickerFromdate(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        // See OnDatetimechangeDatetimepickerTodate for comments
        CTime toTime;
        m_toTime.GetTime(toTime);
        CTime fromDate;
        m_fromDate.GetTime(fromDate);
        CTime fromTime;
        m_fromTime.GetTime(fromTime);
        CTime currentTime = CTime::GetCurrentTime();

        m_toDate.SetRange(&fromDate,&currentTime);

        CTime toDate;
        m_toDate.GetTime(toDate);

        CTime fromDateTime = getCombinedTime(fromDate,fromTime);
        CTime toDateTime = getCombinedTime(toDate,toTime);

        m_fromTime.SetTime(&fromDateTime);
        m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);

        if (toDateTime < fromDateTime)
        {
            m_fromTime.SetTime(&toDateTime);
            m_toTime.SetRange(&toDateTime,&currentTime);
        }
        else
        {
            m_toTime.SetRange(&fromDateTime,&currentTime);
        }

		setUpdateNeeded(true);

		*pResult = 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnDatetimechangeDatetimepickerFromtime(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        // See OnDatetimechangeDatetimepickerTotime for comments
        CTime fromTime;
        m_fromTime.GetTime(fromTime);
        CTime toDate;
        m_toDate.GetTime(toDate);
        CTime fromDate;
        m_fromDate.GetTime(fromDate);        
        CTime currentTime = CTime::GetCurrentTime();

        CTime fromDateTime = getCombinedTime(fromDate,fromTime);

        m_toTime.SetRange(&fromDateTime,&currentTime);

        CTime toTime;
        m_toTime.GetTime(toTime);

        CTime toDateTime = getCombinedTime(toDate,toTime);

        if (toDateTime < fromDateTime)
        {
            m_toTime.SetTime(&fromDateTime);
        }
		setUpdateNeeded(true);
		
		*pResult = 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnDatetimechangeDatetimepickerTodate(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        CTime toDate;
        CTime toTime;
        CTime fromTime;
        m_toDate.GetTime(toDate);
        m_toTime.GetTime(toTime);
        m_fromTime.GetTime(fromTime);
        CTime currentTime = CTime::GetCurrentTime();

        // The simple part - the "from" date cannot be later than the "to" date.
        m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toDate);

        // Now get the from date from the date picker - if the above set range forced a change, it will be picked up here
        CTime fromDate;
        m_fromDate.GetTime(fromDate);

        // Now work out the "actual" date/time values for each pair (as each picker only defines half the actual time)
        CTime fromDateTime = getCombinedTime(fromDate,fromTime);
        CTime toDateTime = getCombinedTime(toDate,toTime);
        
        // If the toDateTime is bigger than the max range, then set the time to the max range.
        // eg. this could happen if the time portion is greater than the current time, but the date
        // isn't and then the date portion is changed to the current day.
        CTime minRange, maxRange;
        m_toTime.GetRange(&minRange, &maxRange);
        if(toDateTime > maxRange)
        {
            m_toTime.SetTime(&maxRange);
        }
        else
        {
            // Set the to time (we have changed the to date, so the current date stored in the to time picker is now wrong, and 
            // in order to obey its range, that has to be correct)
            m_toTime.SetTime(&toDateTime);
        }

        // Once the time is set, specify the range. This makes sure the to time cannot go any earlier than the from time (remember
        // that the from time might have changed when we called SetRange on it above)
        m_toTime.SetRange(&fromDateTime,&currentTime);
        

        // The rest of this code acts as "protection" making sure the times cannot move out of the range protected zones. This
        // comes into effect when a date is change that then causes the time to become invalid.
        if (toDateTime < fromDateTime)
        {
            // If the to date/time is eariler than the from date/time, need to fix that. Do so by setting the to time to be 
            // equal to the from time.
            m_toTime.SetTime(&fromDateTime);

            // Finally, need to set the from time max. As we have just set the to time to be the same as the from time, we
            // now set tell the from time that its current value IS the max
            m_fromTime.SetRange(&EARLY_TIME_LIMIT,&fromDateTime);
        }
        else
        {
            // If the to time is still valid, the new from time max is the current to time.
            m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
        }

		setUpdateNeeded(true);
		
		*pResult = 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnDatetimechangeDatetimepickerTotime(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        CTime minRange;
        CTime maxRange; 
        //m_toTime.GetRange(&minRange, &maxRange);
        CTime currentTime = CTime::GetCurrentTime();

        //m_toTime.SetRange(&minRange,&currentTime);
        
        // Get the date and time picker values that we can here.
        CTime toTime;
        m_toTime.GetTime(toTime);
        CTime toDate;
        m_toDate.GetTime(toDate);
        CTime fromDate;
        m_fromDate.GetTime(fromDate);
        CTime fromTime;
        m_fromTime.GetTime(fromTime);
        
        CTime fromDateTime = getCombinedTime(fromDate, fromTime);
        m_toTime.SetRange(&fromDateTime,&currentTime);
        
        // Calculate the combined to date/time
        CTime toDateTime = getCombinedTime(toDate,toTime);

        // And set the from time max as this value
        m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);

        // Then get the from time. Unlike calling SetRange() on a date picker, this doesn't seem to actually change the value, but
        // call it after the SetRange anyway to be on the safe side (in case MFC does occasionaly change the value)
        //CTime fromTime;
        //m_fromTime.GetTime(fromTime);

        // Calculate the combined from date/time
        //CTime fromDateTime = getCombinedTime(fromDate,fromTime);

        // If the to date/time is earlier than the from date/time, then again need to reset one of the times. As we're playing
        // with the to time here, change the from time to at least match (so sanity is preserved).
        if (toDateTime < fromDateTime)
        {
            m_fromTime.SetTime(&toDateTime);
        }

		setUpdateNeeded(true);
		
		*pResult = 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnSubsystemButton() 
	{
		if (m_subsystemDlg == NULL)
		{
			m_subsystemDlg = new MultipleSelectionDlg("选择子系统...");
			m_subsystemDlg->setItems(m_subsystems,true);
		}
		m_subsystemDlg->setSelection(m_selectedSubsystems);
		if(m_subsystemDlg->DoModal() == IDOK)
		{
			m_selectedSubsystems = m_subsystemDlg->getSelection();
			populateSubsystemsEditCtrl(); // update edit control
			setUpdateNeeded(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnLocationButton() 
	{
		if(m_locationDlg == NULL)
		{
			m_locationDlg = new MultipleSelectionDlg("选择位置...");
			m_locationDlg->setItems(m_locations,true);
		}
		m_locationDlg->setSelection(m_selectedLocations);
        m_locationDlg->setSortItems( false );
		if(m_locationDlg->DoModal() == IDOK)
		{
			m_selectedLocations = m_locationDlg->getSelection();
			populateLocationsEditCtrl(); // update edit control
			setUpdateNeeded(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnOperatorButton() 
	{
		if(m_operatorDlg == NULL)
		{
			m_operatorDlg = new MultipleSelectionDlg("选择操作员...");
			m_operatorDlg->setItems(m_operators,true);
		}
		m_operatorDlg->setSelection(m_selectedOperators);
		if(m_operatorDlg->DoModal() == IDOK)
		{
			m_selectedOperators = m_operatorDlg->getSelection();
			populateOperatorsEditCtrl(); // update edit control
			setUpdateNeeded(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnAlarmTypeButton() 
	{
		if(m_alarmTypeDlg == NULL)
		{
			m_alarmTypeDlg = new MultipleSelectionDlg("选择警报类型...");
			m_alarmTypeDlg->setItems(m_alarmTypes,true);
		}
		m_alarmTypeDlg->setSelection(m_selectedAlarmTypes);
		if(m_alarmTypeDlg->DoModal() == IDOK)
		{
			m_selectedAlarmTypes = m_alarmTypeDlg->getSelection();
			populateAlarmTypesEditCtrl(); // update edit control
			setUpdateNeeded(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnCancel() 
	{		
		if (m_bNMFilterUpdated) 
		{
			if (m_nCurFilterState == -1) //current deleted
			{
				populateNamedFilters(AlarmFilterDlg::FILTER_DEFAULT);
				OnApply();
			}
			else if (m_nCurFilterState == 1) //current modified
			{
				populateNamedFilters(m_strCurDlgFilter);
				OnApply();
			}
			else
			{
				AfxGetMainWnd()->PostMessage(WM_ON_FILTER_DLG_UPDATE, 0, 0);
			}
			
		}
		
		CDialog::OnCancel();
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnSave() 
	{
		FUNCTION_ENTRY("OnSaveFilter");

		// Get the name to use.
		CString fName;
		m_namedFilter.GetWindowText(fName);
		std::string filterName(fName);

		// Convert to upper case for comparison
		fName.MakeUpper();

		//bool isUserDefined = ( fName.Find("USER DEFINED") >= 0 )? true : false;
		//bool isDefault = ( fName.Find("DEFAULT") >= 0 ) ? true : false;
		bool isUserDefined = ( fName.Find("自定义过滤器") >= 0 )? true : false;
		bool isDefault = ( fName.Find("默认过滤器") >= 0 ) ? true : false;
		bool isInComboList = m_namedFilter.FindStringExact(-1, fName) >= 0 ? true : false;
		
		if ( isUserDefined || isDefault ) // Can't save a Default or User Defined file
		{
			// Display a message box to disallow these keywords as name
			TA_Base_Bus::TransActiveMessage userMsg;
			//const std::string saveEr("save an invalid name for");
			//const std::string fileEr("saved as " + filterName);
			const std::string saveEr("保存的名字无效 ");
			const std::string fileEr("保存为 " + filterName);
			userMsg << saveEr;
			userMsg << fileEr;
			userMsg.showMsgBox(IDS_UE_040031);
			m_nmFilter.setName( NamedFilter::UNNAMED );
			return;
		}


		// This part is to delete old NamedFilter from database and create a new one
		if (/* (m_savedFilter) &&*/ (isInComboList == true) )
		{
			// get the old filter name, in the event that this is a rename operation..
			// the old filter is the one we need to delete
			std::string oldFilterName(m_filter.getFilterName());

			// if the filter name we want to save to is in the list, then that
			// is the one that needs to be deleted
			if(isInComboList)
			{
				//oldFilterName = filterName;
				CString strOldFilterName;
				int nIndex = m_namedFilter.FindStringExact(-1, fName);
				m_namedFilter.GetLBText(nIndex, strOldFilterName);
				oldFilterName = strOldFilterName;
			}

			if(oldFilterName != TA_Base_Core::NamedFilter::UNNAMED)
			{
				// TD #11235
				// make the user confirm the overwrite
				TA_Base_Bus::TransActiveMessage userMsg;
				//userMsg << "overwrite '" + oldFilterName + "' with the current filter settings";
				userMsg << "用当前滤器设置覆盖 '" + oldFilterName + "'";
				if( IDNO == userMsg.showMsgBox(IDS_UW_010003) )
				{
					return;
				}

				std::auto_ptr<NamedFilter> loadedFilter;

				try
				{
					// Load the filter.
					loadedFilter = TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( oldFilterName );
					deleteNamedFilter (*loadedFilter.get());
				}
			   catch(...)
				{
				}
			}
		}

		// Clear current filter and create a new one
		m_filter.removeAll();
		m_filter = createFilter();
		m_filter.setFilterName(filterName);

		m_nmFilter.clear();
		m_nmFilter = convertFilterToNamedFilter (m_filter);

		try
		{
			// Save the filter.
			m_nmFilter.saveNamedFilter();

			if (stricmp(m_strCurDlgFilter.c_str(), m_filter.getFilterName().c_str()) ==0)
				m_nCurFilterState = 1;
		}
		catch ( TA_Base_Core::DataException& ex )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

			// Display the save filter error message.
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << sSave << sSaved;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);
			
			// Only an unnamed filter can be saved, so rename it back.
			m_nmFilter.setName( NamedFilter::UNNAMED );
			return;
		}
		catch (...)
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Failed to create and save filter");
			// Display the save filter error message.
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << sSave << sSaved;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);
			
			// Only an unnamed filter can be saved, so rename it back.
			m_nmFilter.setName( NamedFilter::UNNAMED );
            return;
		}

		// Now update the combo box.
		populateNamedFilters(m_nmFilter.getName());
		setUpdateNeeded(true);
		m_savedFilter = true;
		m_bNMFilterUpdated = true;
		// TD12755 Reset the save button
		m_saveButton.EnableWindow(false);

		FUNCTION_EXIT;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnFromCheckBoxClicked() 
	{
		// Enable or disable the From date/time controls.
		CButton* checkBox = (CButton*)GetDlgItem(IDC_FROM_TIME);
		bool enableWindow = checkBox->GetCheck() == 1;

        CTime currentTime = CTime::GetCurrentTime();
        if(enableWindow)
        {
            // TD 8703: When checkboxes are enabled, set times to earliest and latest in displayed list.
            time_t earliestTime = m_alarmList.getEarliestAlarmTime();
            
			// get the currently set date/time of the 'To' fields
            CTime toTime;
            CTime toDate;
            m_toTime.GetTime(toTime);
            m_toDate.GetTime(toDate);

            CTime toDateTime(getCombinedTime(toDate, toTime));

            if(earliestTime == 0)
            {
                // If there are no alarms displayed, then set to current time
                earliestTime = currentTime.GetTime();
            }

            CTime fromDateTime(earliestTime);
			CButton* toCheckBox = (CButton*)GetDlgItem(IDC_TO_TIME);

			// if the 'To' checkbox is ticked, then the maximum range needs to be limited
			if(toCheckBox->GetCheck() == 1)
			{
				// set the maximum range bound to be the time that 'To' is set to,
				// so the user cannot overlap to/from times
				m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
				m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);

				if(fromDateTime.GetTime() > toDateTime.GetTime())
				{
					fromDateTime = toDateTime;
				}
			}
			// ..otherwise, the maximum range can extend to the current time
			else
			{
				m_fromDate.SetRange(&EARLY_TIME_LIMIT,&currentTime);
				m_fromTime.SetRange(&EARLY_TIME_LIMIT,&currentTime);
			}

			// set the initial date/time of the 'From' fields to be that of the
			// earliest alarm. if there are no alarms, this will be the current date/time
            m_fromDate.SetTime(&fromDateTime);
		    m_fromTime.SetTime(&fromDateTime);

			// set the minimum range bound of 'To' to be the time that 'From' has been set to,
			// again, so the user cannot overlap to/from times		
			m_toTime.SetRange(&fromDateTime,&currentTime);
			m_toDate.SetRange(&fromDateTime,&currentTime);
			
        }
		else
		{
            //m_toTime.SetRange(&EARLY_TIME_LIMIT,&currentTime);
            //m_toDate.SetRange(&EARLY_TIME_LIMIT,&currentTime);

			//marvin++
			//TD14801
			//when to pressed must check for earliest time
			time_t earliestTime = m_alarmList.getEarliestAlarmTime();

			//TD15208++
			if(earliestTime == 0)
			{
				earliestTime = currentTime.GetTime();
			}
			//++TD15208
			CTime fromEarliest(earliestTime);

			//TD15028++ wangyn
			m_fromDate.SetTime(&fromEarliest);
		    m_fromTime.SetTime(&fromEarliest);
			//++TD15028
			m_toDate.SetRange(&fromEarliest,&currentTime);
			m_toTime.SetRange(&fromEarliest,&currentTime);
			//marvin++
			//TD14801
		}
        
		m_fromDate.EnableWindow(enableWindow);
		m_fromTime.EnableWindow(enableWindow);
		fromCheckBoxUpdated(enableWindow);	//TD15208
		setUpdateNeeded(true);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnToCheckBoxClicked() 
	{
		// Enable or disable the To date/time controls.
		CButton* checkBox = (CButton*)GetDlgItem(IDC_TO_TIME);
		bool enableWindow = checkBox->GetCheck() == 1;

        CTime currentTime = CTime::GetCurrentTime();
        if(enableWindow)
        {
            // TD 8703: When checkboxes are enabled, set times to earliest and latest in displayed list.
            time_t latestTime = m_alarmList.getLatestAlarmTime();

            CTime fromTime;
            CTime fromDate;
            m_fromTime.GetTime(fromTime);
            m_fromDate.GetTime(fromDate);
            
            CTime fromDateTime = getCombinedTime(fromDate, fromTime);

            if(latestTime == 0)
            {
                // If there are no alarms displayed, then set to current time
                latestTime = currentTime.GetTime();
            }
            
            CTime toDateTime(latestTime);
			CButton* fromCheckBox = (CButton*)GetDlgItem(IDC_FROM_TIME);

			// if the 'From' checkbox is ticked, then the minimum range needs to be limited

			

			if(fromCheckBox->GetCheck() == 1)
			{            
				// set the minimum range bound to be the time that 'From' is set to,
				// so the user cannot overlap to/from times
				m_toDate.SetRange(&fromDateTime,&currentTime);
				m_toTime.SetRange(&fromDateTime,&currentTime);

				if(toDateTime.GetTime() < fromDateTime.GetTime())
				{
					toDateTime = fromDateTime;
				}
			}
			// ..otherwise, the minimum range can extend back to the earliest time
			else
			{

				//marvin++
				//TD14801
				//when to pressed must check for earliest time
				time_t earliestTime = m_alarmList.getEarliestAlarmTime();

				CTime fromDateTime(earliestTime);

				m_toDate.SetRange(&fromDateTime,&currentTime);
				m_toTime.SetRange(&fromDateTime,&currentTime);
				//marvin++
				//TD14801
			}

			// set the initial date/time of the 'To' fields to be that of the
			// latest alarm. if there are no alarms, this will be the current date/time
            m_toDate.SetTime(&toDateTime);
		    m_toTime.SetTime(&toDateTime);

			// set the maximum range bound of 'From' to be the time that 'To' has been set to,
			// again, so the user cannot overlap to/from times
            m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
            m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
        }
		else
		{
			//TD15028++ wangyn
			time_t latestTime = m_alarmList.getLatestAlarmTime();

			if(latestTime==0)
			{
				latestTime = currentTime.GetTime();
			}

			CTime toLatestTime(latestTime);
			
			m_toDate.SetTime(&toLatestTime);
		    m_toTime.SetTime(&toLatestTime);
			//++TD15208
            m_fromTime.SetRange(&EARLY_TIME_LIMIT,&currentTime);
            m_fromDate.SetRange(&EARLY_TIME_LIMIT,&currentTime);
		}

		m_toDate.EnableWindow(enableWindow);
		m_toTime.EnableWindow(enableWindow);
		toCheckBoxUpdated(enableWindow);	//TD15208
		setUpdateNeeded(true);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		CDialog::OnShowWindow(bShow, nStatus);
		
		if (bShow)
		{
			// Window is being shown.
			m_updateNeeded = false; //No update when opened
			disableButtons(); //Disable all buttons except "Cancel"
		}
	}


	/////////////////////////////////////////////////////////////////////////////
	// This is normally called by the parent to inform filter dialog to display
	// the named filter passed
	//
	void AlarmFilterDlg::showFilter (std::string filterName) 
	{
		FUNCTION_ENTRY("showFilter");
		// Record the current dialog filter
		m_strCurDlgFilter = filterName;
		m_bNMFilterUpdated = false;

		bool isDefault = (filterName == AlarmFilterDlg::FILTER_DEFAULT) ? true : false;
		bool isUserDefined = (filterName == TA_Base_Core::NamedFilter::UNNAMED) ? true : false;
		
		if ( filterName != "" && isDefault == false && isUserDefined == false && checkInNamedFilterDatabase(filterName) == true ) // This filter can be found in database
		{
			// Load the namedfilter
			std::auto_ptr<NamedFilter> loadedFilter;

			try
			{
				// Load the filter.
				loadedFilter = TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( filterName );
			}
			catch ( TA_Base_Core::DataException& ex )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

				// Display the load filter error message.
				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << sLoad << sLoaded;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);

				return;
			}

			// Copy the new filter to our internal filter.
			m_nmFilter.clear();
			m_nmFilter = *(loadedFilter.get());

			// Convert the nmFilter to Filter class
			std::vector<std::string> stringList = m_nmFilter.getStringFilterList();
			std::vector<std::string> numericList = m_nmFilter.getNumericFilterList();
			if (stringList.size() > 0 || numericList.size() > 0) {
				m_filter.removeAll();
				m_filter = convertNamedFilterToFilter( m_nmFilter );
				populateFilterSelections(m_filter);
				populateNamedFilters(filterName);
			}
			else
			{
				//Warn user of null named filter
				TA_Base_Bus::TransActiveMessage userMsg;
				//const std::string retEr("retrieve");
				//const std::string fileEr("load using " + filterName);
				const std::string retEr("获取");
				const std::string fileEr("用 " + filterName + " 载入");
				userMsg << retEr;
				userMsg << fileEr;
				userMsg.showMsgBox(IDS_UE_040031);

				return;
			}

			// Set the state of the save and delete buttons.
			m_saveButton.EnableWindow( true );
			m_deleteButton.EnableWindow( true );
			setUpdateNeeded(false);
		}
		else if ( isDefault == true )
		{
			OnClearFiltersButton(); // set to default
		}
		else if ( isUserDefined == true )
		{
			m_filter.removeAll();
			m_filter = m_filterUserDef;
			populateFilterSelections(m_filter);
			m_namedFilter.AddTempString(TA_Base_Core::NamedFilter::UNNAMED.c_str());

			m_updateNeeded = true;
			enableButtons();
		}
		else // The passed in filter name is not valid or found in database
		{
			// Display a message to inform user
			TA_Base_Bus::TransActiveMessage userMsg;
			//const std::string retEr("show / retrieve ");
			//const std::string fileEr("load as an invalid or unfound filter is used.");
			const std::string retEr("显示 / 获取 ");
			const std::string fileEr("载入 由于使用了无效或者没被找到的滤器");
			userMsg << retEr;
			userMsg << fileEr;
			userMsg.showMsgBox(IDS_UE_040031);
		}

		//as per TD 14910
		//Apply button is always to be disabled upon start up
		m_applyButton.EnableWindow(false);

		FUNCTION_EXIT;
	}
	
  /**
	* getCurrentFilterString
	* 
	* This is to return string of the named filter parameters for display on caption, etc
	*
	* @return: A param string of the named filter for display
	*/
	std::string AlarmFilterDlg::getCurrentFilterString ()
	{
		FUNCTION_ENTRY("getCurrentFilterString");
		
		// Clear the named filter and set to the current one which can be default or user defined
		m_nmFilter.clear();
		m_nmFilter = convertFilterToNamedFilter(m_filter);

		// Filter data is contained in two maps - a string-string and a string-long map, so create iterators for both.
		TA_Base_Core::NamedFilter::StringList stringFilterList = m_nmFilter.getStringFilterList();
		TA_Base_Core::NamedFilter::StringList numericFilterList = m_nmFilter.getNumericFilterList();

		TA_Base_Core::NamedFilter::StringList::iterator stringIter;

		// Iterate through each map, adding any filters to the string..
		//std::string filterString(" - Filtered on: ");
		std::string filterString(" - 过滤在: ");
		for ( stringIter=stringFilterList.begin(); stringIter!=stringFilterList.end(); stringIter++ )
		{
			filterString += "[" + *stringIter + "]";
		}
		for ( stringIter=numericFilterList.begin(); stringIter!=numericFilterList.end(); stringIter++ )
		{
			// There are a couple of numeric filters that should not appear in the filter summary.
			// if ( ( AlarmFilterDlg::FILTER_TOTIME != *stringIter ) && ( AlarmFilterDlg::FILTER_FROMTIME != *stringIter ) )
			//{
				filterString += " [" + *stringIter + "]";
			//}
		}

		FUNCTION_EXIT;

		return filterString;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnStateRadioButton(UINT nId)
	{
		// The state of the "Acked" radio button should be the opposite
		// of the "Closed" radio button.
		bool isClosedSelected = ( static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->GetCheck() == 1 ) ? true : false;
		static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->EnableWindow(!isClosedSelected);
		setUpdateNeeded(true);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::OnAckRadioButton(UINT nId)
	{
		// The state of the "Closed" radio button should be the opposite
		// of the "Acked" radio button.
		bool isAckedSelected = ( static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->GetCheck() == 1);
		static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->EnableWindow(!isAckedSelected);
		setUpdateNeeded(true);
	}

	void AlarmFilterDlg::OnKillfocusSeverityList() 
    {
	    // Remove the selection highlighting
	    m_severityList.SetCurSel(-1);
    }


    void AlarmFilterDlg::OnChkChange() 
    {
        setUpdateNeeded(true);
    } 
	

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AlarmFilterDlg helper functions
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Call this when the controls have changed
	void AlarmFilterDlg::setUpdateNeeded(bool setUpdateNeeded)
	{
		m_updateNeeded = setUpdateNeeded;
		enableButtons(); //enable the buttons to apply change
		
		// Get the name of selected filter only if the m_namedFilter is not empty
		std::string filterName(m_filter.getFilterName());
		bool isDefaultFilter = (m_filter.getFilterName() == AlarmFilterDlg::FILTER_DEFAULT) ? true : false;

		// Check whether there is need to change the combox name for filter
		// Remain unchanged if the display is reflective of current filter
		// Change default to user defined
		if (setUpdateNeeded && isDefaultFilter) 
		{
			selectUserFilter();
			m_saveButton.EnableWindow( false );
		}
		else
		{
			m_saveButton.EnableWindow( true );
		}

		m_savedFilter = !setUpdateNeeded; //changes made but filter is not saved
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateFilterSelections(TA_Base_Bus::Filter& filter)
	{
		// Clear the dialog.
		clearAllDisplays();

		// Need to clear the severity filters checkboxes as they will be individually
		// set by the filters as they are loaded
		SetAllSeverityFilterControls(FALSE);
		
		std::vector<TA_Base_Bus::Filter::FilterDetails> filterSettings = filter.getFilters();

		std::string stringValue;
		unsigned long ulValue;
        bool isFilterTimeSet = false;
		// Need to track how many severity filters have been set as there is a special case
		// on dialog init where there can be zero severity filters. This means load ALL
		// alarm severities. We need to put the code to handle this case in here because the
		// cleaner option (code-wise) would be to force the filter to explicitly specify all
		// severities, but this would cause an alarm store reload whenever the dialog is
		// loaded so it is not practical.
		unsigned int numSeverityFilters = 0;

		for (unsigned int i = 0; i < filterSettings.size(); ++i)
		{
			TA_Base_Bus::Filter::FilterDetails setting = filterSettings[i];
			switch (setting.type)
			{
				case TA_Base_Bus::Filter::FILTER_ALARM_TIME:
					ulValue = setting.value.ulValue;
					if (setting.comparison == TA_Base_Bus::Filter::COMPARE_GREATER_THAN_EQUAL)
					{
						// Filter was on from time. Tick the check button.
						static_cast<CButton*>(GetDlgItem(IDC_FROM_TIME))->SetCheck(1);
						unsigned long ulStartTime = setting.value.ulValue;
						CTime fromDateTime = ulStartTime;

						// Enable the from time controls
						m_fromDate.EnableWindow(true);
						m_fromTime.EnableWindow(true);
						fromCheckBoxUpdated(true);
						
						// Set the correct range first so SetTime will not fail
						CTime time = CTime::GetCurrentTime();
						m_fromTime.SetRange(&EARLY_TIME_LIMIT,&time);
						m_fromDate.SetRange(&EARLY_TIME_LIMIT,&time);
						m_toTime.SetRange(&fromDateTime,&LATEST_TIME_LIMIT);
						m_toDate.SetRange(&fromDateTime,&LATEST_TIME_LIMIT);

						// Populate the date and time
						m_fromDate.SetTime(&fromDateTime);
						m_fromTime.SetTime(&fromDateTime);
						m_toDate.SetTime(&time);
						m_toTime.SetTime(&time);
						
						isFilterTimeSet = true;
					}
					else if (setting.comparison == TA_Base_Bus::Filter::COMPARE_LESS_THAN_EQUAL)
					{
						// Filter was on to time. Tick the check button.
						static_cast<CButton*>(GetDlgItem(IDC_TO_TIME))->SetCheck(1);
						// Populate the date and time
						unsigned long ulEndTime = setting.value.ulValue;
						CTime toDateTime = ulEndTime;

						// Enable the to time controls
						toCheckBoxUpdated(true);
						m_toDate.EnableWindow(true);
						m_toTime.EnableWindow(true);
						
						// Set the correct range first so SetTime will not fail
						CTime time = CTime::GetCurrentTime();
						m_toDate.SetRange(&EARLY_TIME_LIMIT,&LATEST_TIME_LIMIT);
						m_toTime.SetRange(&EARLY_TIME_LIMIT,&LATEST_TIME_LIMIT);
						m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
						m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toDateTime);

						m_toDate.SetTime(&toDateTime);
						m_toTime.SetTime(&toDateTime);
						m_fromTime.SetTime(&toDateTime);
						m_fromDate.SetTime(&toDateTime);

						isFilterTimeSet = true;
					}

					break;

				case TA_Base_Bus::Filter::FILTER_ALARM_TIME_RANGE:
					{	
						// Set the check buttons as checked.
						static_cast<CButton*>(GetDlgItem(IDC_FROM_TIME))->SetCheck(1);
						static_cast<CButton*>(GetDlgItem(IDC_TO_TIME))->SetCheck(1);

						// Enable the date/time controls
						m_fromDate.EnableWindow(true);
						m_fromTime.EnableWindow(true);
						m_toDate.EnableWindow(true);
						m_toTime.EnableWindow(true);

						TA_Base_Bus::Filter::SDateRange dateRange = setting.value.dateRange;
						CTime fromDateTime = dateRange.startTime;					
						CTime toDateTime = dateRange.endTime;

						// Set the correct range first so SetTime will not fail
						m_fromDate.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
						m_fromTime.SetRange(&EARLY_TIME_LIMIT,&toDateTime);
						m_toDate.SetRange(&toDateTime,&LATEST_TIME_LIMIT);
						m_toTime.SetRange(&toDateTime,&LATEST_TIME_LIMIT);

						m_fromDate.SetTime(&fromDateTime);
						m_fromTime.SetTime(&fromDateTime);
						m_toDate.SetTime(&toDateTime);
						m_toTime.SetTime(&toDateTime);

						// flag that the time range has been set
						isFilterTimeSet = true;

						// enable the controls
						fromCheckBoxUpdated(true);
						toCheckBoxUpdated(true);
					}
					break;

				case TA_Base_Bus::Filter::FILTER_ASSET_NAME:
					stringValue = *(setting.value.stringValue);
					m_assetEdit.SetWindowText(stringValue.c_str());
					break;

				case TA_Base_Bus::Filter::FILTER_DESCRIPTION:
					stringValue = *(setting.value.stringValue);
					m_descEdit.SetWindowText(stringValue.c_str());
					break;

				case TA_Base_Bus::Filter::FILTER_SUBSYSTEM:
					{
						ListItems::iterator it;
						for(it = m_subsystems.begin(); it != m_subsystems.end(); ++it)
						{
							if(it->pkey == setting.value.ulValue)
							{
								m_selectedSubsystems.push_back(*it);
								break;
							}
						}
					}
					break;

				case TA_Base_Bus::Filter::FILTER_LOCATION:
					{
						ListItems::iterator it;
						for(it = m_locations.begin(); it != m_locations.end(); ++it)
						{
							if(it->pkey == setting.value.ulValue)
							{
								m_selectedLocations.push_back(*it);
								break;
							}
						}
					}
					break;

				case TA_Base_Bus::Filter::FILTER_OPERATOR:
					{
						stringValue = *(setting.value.stringValue);
						// Find stringValue in m_operators and then add the item to m_selectedOperators
						for(ListItems::iterator it = m_operators.begin(); it != m_operators.end(); ++it)
						{
							if(stringValue.compare((*it).str) == 0)
							{
								m_selectedOperators.push_back(*it);
								break; // break out of the for loop
							}
						}
					}
					break;

				case TA_Base_Bus::Filter::FILTER_ALARM_TYPE:
					{
						ListItems::iterator it;
						for(it = m_alarmTypes.begin(); it != m_alarmTypes.end(); ++it)
						{
							if(it->pkey == setting.value.ulValue)
							{
								m_selectedAlarmTypes.push_back(*it);
								break;
							}
						}
					}
					break;

				case TA_Base_Bus::Filter::FILTER_SEVERITY:
					// BJR KLUDGE: This is pretty ugly, but I can't think of anything a lot
					// cleaner that isn't a lot of work (this previously used the resource
					// directly (which was still a kludge, but was fewer lines of code) but
					// this won't work with the CColorCheck class.
					++numSeverityFilters;
					ulValue = setting.value.ulValue; // Severity pkey
					if(setting.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
					{
                        for(int i = 0; i < m_severityList.GetCount(); ++i)
                        {
                            if(m_severityList.GetItemData(i) == ulValue)
                            {
                                m_severityList.SetCheck(i, 1);
                                break;
                            }
                        }
                    }
					else if(setting.comparison == TA_Base_Bus::Filter::COMPARE_LESS_THAN)
					{
						// The less than filter is used to indicate that ALL severities
						// are currently filtered out. Don't need to do anything since we
						// have already cleared all the filters
						TA_ASSERT(ulValue==0,"Invalid filter parameter");
					}
					else
					{
						// This is not possible unless a code maintainer adds a severity filter
						// type but does not fix this code
						TA_ASSERT(false, "Invalid filter type.");
					}
					break;

				case TA_Base_Bus::Filter::FILTER_STATE:
					populateState(setting);
					break;

				default:
					TA_ASSERT(0,"AlarmFilterDlg::populateFilterSelections(): Invalid filter type received.");
					break;
			} 
		}
        
		// Handle the special case (on init dialog) where there are no severity
		// filters. In this case we actually DO want to set all severity checkbox controls
		if(numSeverityFilters == 0)
		{
			SetAllSeverityFilterControls(TRUE);
		}

		if (!isFilterTimeSet)
        {
			// Set the "To Time" pickers to have the "current" time as necessary.
			CTime time = CTime::GetCurrentTime();
			m_toDate.SetTime(&time);
			m_toTime.SetTime(&time);
			
            // And set the valid time range
            m_toDate.SetRange(&EARLY_TIME_LIMIT,&time);
            m_toTime.SetRange(&EARLY_TIME_LIMIT,&time);
            m_fromDate.SetRange(&EARLY_TIME_LIMIT,&time);
            m_fromTime.SetRange(&EARLY_TIME_LIMIT,&time);
        }

		populateLocationsEditCtrl();
		populateSubsystemsEditCtrl();
		populateOperatorsEditCtrl();
		populateAlarmTypesEditCtrl();
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::retrieveSubsystems()
	{
		std::vector<TA_Base_Core::ISubsystem*> allSubsystemData;
		try
		{
			allSubsystemData = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(false);
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to retrieve subsystems matching the one passed in");
			for (unsigned int i = 0;i < allSubsystemData.size(); ++i)
			{
				delete allSubsystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the severity information from the database.");
			for (unsigned int i = 0;i < allSubsystemData.size(); ++i)
			{
				delete allSubsystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
			for (unsigned int i = 0;i < allSubsystemData.size(); ++i)
			{
				delete allSubsystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving subsystem information."));
		}

		// store the subsystems into the map and delete the temporary structure
		m_subsystems.clear();
        for (unsigned int i = 0;i < allSubsystemData.size(); ++i)
        {
			ListItem subsystem(allSubsystemData[i]->getKey(),allSubsystemData[i]->getDisplayName());
			m_subsystems.push_back(subsystem);
			delete allSubsystemData[i];
		}
		// sort the subsystems
		m_subsystems.sort();
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::retrieveLocations()
	{
		std::vector<TA_Base_Core::ILocation*> allLocationData;
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retrieving operator locations.");
			allLocationData = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving operator locations. There were problems communicating with the database");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving operator locations. The data could not be converted to the required format");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving Operator locations.");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving location information."));
		}

		// store the locations into the map and delete the temporary structure
		m_locations.clear();
		for (unsigned int i = 0;i < allLocationData.size(); ++i)
        {
			if(allLocationData[i]->getKey() != 0)
			{
				ListItem location(allLocationData[i]->getKey(), allLocationData[i]->getDisplayName()); //allLocationData[i]->getName());
				m_locations.push_back(location);
				delete allLocationData[i];
			}
		}
		// sort the locations
		//m_locations.sort();
	}

	/////////////////////////////////////////////////////////////////////////////
	// Note that the operators are sorted first by their type (non-system operators
	// take precedence over system operators) and then by their name.
	void AlarmFilterDlg::retrieveOperators()
	{
		std::vector<TA_Base_Core::IOperator*> allOperatorData;
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retrieving operator locations.");
			allOperatorData = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperatorsInfo();
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving operator information. There were problems communicating with the database");
			for (unsigned int i = 0;i < allOperatorData.size(); ++i)
			{
				delete allOperatorData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve operator information from database. Aborting."));
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving operator information. The data could not be converted to the required format");
			for (unsigned int i = 0;i < allOperatorData.size(); ++i)
			{
				delete allOperatorData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve operator information from database. Aborting."));
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving Operator information.");
			for (unsigned int i = 0;i < allOperatorData.size(); ++i)
			{
				delete allOperatorData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving operator information."));
		}

		// store the non-system (real) operators into the structure
		m_operators.clear();
		unsigned int i = 0;
		for ( i = 0;i < allOperatorData.size(); ++i)
		{
			if(!allOperatorData[i]->isSystemOperator())
			{
				ListItem op(allOperatorData[i]->getKey(),allOperatorData[i]->getName());
				m_operators.push_back(op);
			}
		}
		// Sort the operator list
		m_operators.sort();

		// store the system operators into a temporary structure
		ListItems systemOperators;
		for (i = 0;i < allOperatorData.size(); ++i)
		{
			if(allOperatorData[i]->isSystemOperator())
			{
				ListItem op(allOperatorData[i]->getKey(),allOperatorData[i]->getName());
				systemOperators.push_back(op);
			}
		}
		// Sort the system operators
		systemOperators.sort();

		// Add the system operators to the non-system operators (at the end)
		for(ListItems::iterator it = systemOperators.begin(); it != systemOperators.end(); ++it)
		{
			m_operators.push_back(*it);
		}

		// Cleanup
		for (i = 0;i < allOperatorData.size(); ++i)
		{
	        delete allOperatorData[i];
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::retrieveAlarmTypes()
	{
		// Retrieve the alarmType information from the database.
		std::vector<TA_Base_Core::IAlarmTypeData*> allAlarmTypeData;
		try
		{
			allAlarmTypeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to retrieve alarm type matching the one passed in");
            // Delete the contents of the allAlarmTypeData vector
            for (unsigned int i = 0;i < allAlarmTypeData.size(); ++i)
            {
                delete allAlarmTypeData[i];
            }
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve alarm type information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the alarm type information from the database.");
            // Delete the contents of the allAlarmTypeData vector
            for (unsigned int i = 0;i < allAlarmTypeData.size(); ++i)
            {
                delete allAlarmTypeData[i];
            }
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve alarm type information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
            // Delete the contents of the allAlarmTypeData vector
            for (unsigned int i = 0;i < allAlarmTypeData.size(); ++i)
            {
                delete allAlarmTypeData[i];
            }
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving alarm type information."));
		}

		// store the alarm types into the map and delete the temporary structure
		m_alarmTypes.clear();
        for (unsigned int i = 0;i < allAlarmTypeData.size(); ++i)
        {
			ListItem alarmType(allAlarmTypeData[i]->getKey(),allAlarmTypeData[i]->getDisplayName());//allAlarmTypeData[i]->getName());
			m_alarmTypes.push_back(alarmType);
            delete allAlarmTypeData[i];
        }
		// sort the alarm types
		m_alarmTypes.sort();
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::retrieveSeverities()
	{
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

        m_severityList.ShowWindow(SW_SHOW);
        GetDlgItem(IDC_LIST_PLACEHOLDER)->ShowWindow(SW_HIDE);

		std::vector<TA_Base_Core::IAlarmSeverityData*> allSeverityData;
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retrieving alarm severity information from database.");
			allSeverityData = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving severity information. There were problems communicating with the database");
			// Delete the contents of the allSeverityData vector
			for (unsigned int i = 0;i < allSeverityData.size(); ++i)
			{
				delete allSeverityData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve severity information from database. Aborting."));
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving severity information. The data could not be converted to the required format");
			// Delete the contents of the allSeverityData vector
			for (unsigned int i = 0;i < allSeverityData.size(); ++i)
			{
				delete allSeverityData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve severity information from database. Aborting."));
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving severity information.");
			// Delete the contents of the allSeverityData vector
			for (unsigned int i = 0;i < allSeverityData.size(); ++i)
			{
				delete allSeverityData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving alarm severity information."));
		}

		// store the severities into the map and cleanup the temporary structure
		
        std::map <int, COLORREF> itemColours;

        for (unsigned int i = 0; i < allSeverityData.size(); ++i)
        {
			CString namedes = allSeverityData[i]->getName().c_str();
//			namedes += " - ";
//			namedes += (dynamic_cast<TA_Base_Core::AlarmSeverityData*>(allSeverityData[i]))->getDescription().c_str();
            m_severityList.InsertString(i, namedes);
            m_severityList.SetItemData(i, allSeverityData[i]->getKey());
            m_severityList.SetCheck(i, 1);

			//TD17081++ pass in the severityData itself to limit database access
            // Get the severity colour
            //COLORREF severityColour = getSeverityColour(allSeverityData[i]->getKey());
			COLORREF severityColour = getSeverityColour(allSeverityData[i]);			
			//++TD17081
	 
            itemColours[i] = severityColour;
            
            delete allSeverityData[i];
        }

        m_severityList.addItemColours(itemColours);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateLocationsEditCtrl()
	{
		m_locationEdit.SetWindowText(getSelectedItemsString(m_selectedLocations));
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateSubsystemsEditCtrl()
	{
		m_subsystemEdit.SetWindowText(getSelectedItemsString(m_selectedSubsystems));
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateOperatorsEditCtrl()
	{
		m_operatorEdit.SetWindowText(getSelectedItemsString(m_selectedOperators));
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateAlarmTypesEditCtrl()
	{
		m_alarmTypeEdit.SetWindowText(getSelectedItemsString(m_selectedAlarmTypes));
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::populateState(const TA_Base_Bus::Filter::FilterDetails& setting)
	{
		switch (setting.value.alarmStateValue)
		{
			case TA_Base_Bus::Filter::STATE_ALL_OPEN:
				setAckRadioButton(IDC_ACK_ALL);
				setStateRadioButton(IDC_STATE_OPEN);
				break;

			case TA_Base_Bus::Filter::STATE_OPEN_UNACKED:
				setAckRadioButton(IDC_ACK_UNACKED);
				setStateRadioButton(IDC_STATE_OPEN);
				break;

			case TA_Base_Bus::Filter::STATE_OPEN_ACKED:
				setAckRadioButton(IDC_ACK_ACKED);
				setStateRadioButton(IDC_STATE_ALL);
				static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->EnableWindow(false);
				break;
 
			case TA_Base_Bus::Filter::STATE_CLOSED:
				setAckRadioButton(IDC_ACK_ALL);
				setStateRadioButton(IDC_STATE_CLOSED);
				static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->EnableWindow(false);
				break;

			case TA_Base_Bus::Filter::STATE_ALL_UNACKED:
				setAckRadioButton(IDC_ACK_UNACKED);
				setStateRadioButton(IDC_STATE_ALL);
				break;

			case TA_Base_Bus::Filter::STATE_ANY:
				setAckRadioButton(IDC_ACK_ALL);
				setStateRadioButton(IDC_STATE_ALL);
				break;
		}  
	}
 
	/////////////////////////////////////////////////////////////////////////////
	TA_Base_Bus::Filter AlarmFilterDlg::createFilter()
	{	
		// Create a filter object
		TA_Base_Bus::Filter filter;
		ListItems::iterator iter;
 
		// Filter on subsystem
		for(iter = m_selectedSubsystems.begin(); iter != m_selectedSubsystems.end(); iter++)
		{
			// Add a filter for this KEY
			filter.addFilter(TA_Base_Bus::Filter::FILTER_SUBSYSTEM, TA_Base_Bus::Filter::COMPARE_EQUAL, iter->pkey);
		}

		// Filter on location
		for(iter = m_selectedLocations.begin(); iter != m_selectedLocations.end(); iter++)
		{
			// Add a filter for this KEY
			filter.addFilter(TA_Base_Bus::Filter::FILTER_LOCATION, TA_Base_Bus::Filter::COMPARE_EQUAL, iter->pkey);
		}

		// Filter on operator
		for(iter = m_selectedOperators.begin(); iter != m_selectedOperators.end(); iter++)
		{
			// Add a filter for this STRING
			filter.addFilter(TA_Base_Bus::Filter::FILTER_OPERATOR, TA_Base_Bus::Filter::COMPARE_EQUAL, iter->str);
		}

		// Filter on alarm type
		for(iter = m_selectedAlarmTypes.begin(); iter != m_selectedAlarmTypes.end(); iter++)
		{
			// Add a filter for this STRING
			filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TYPE, TA_Base_Bus::Filter::COMPARE_EQUAL, iter->pkey);
		}

		// Filter on asset
		CString assetText;
		m_assetEdit.GetWindowText(assetText);
		std::string assetTextStr(assetText);
		if(!assetTextStr.empty())
		{
			filter.addFilter(TA_Base_Bus::Filter::FILTER_ASSET_NAME, TA_Base_Bus::Filter::COMPARE_CONTAINS, assetTextStr);
		}

		// Filter on description
		CString descriptionText;
		m_descEdit.GetWindowText(descriptionText);
		std::auto_ptr<char> strUTF8 = Encoder::AnsiToUTF8((LPCTSTR)descriptionText);
		descriptionText = strUTF8.get();
		
		std::string descTextStr(descriptionText);
		if(!descTextStr.empty())
		{
			filter.addFilter(TA_Base_Bus::Filter::FILTER_DESCRIPTION, TA_Base_Bus::Filter::COMPARE_CONTAINS, descTextStr);
		}
		
		// Filter on severity
        unsigned int numSeverityFilters = 0;
        for(int i = 0; i < m_severityList.GetCount(); ++i)
        {
            if(m_severityList.GetCheck(i) == 1)
            {
                filter.addFilter(TA_Base_Bus::Filter::FILTER_SEVERITY, TA_Base_Bus::Filter::COMPARE_EQUAL, m_severityList.GetItemData(i));
                ++numSeverityFilters;
            }
        }

        if(numSeverityFilters == 0)
		{
			// Create a filter that filters out all severities
			// Severities cannot be negative, so the following filter will do
			// TD #11265
			// Use severity=-1 filter rather than severity<0
			filter.addFilter(TA_Base_Bus::Filter::FILTER_SEVERITY, TA_Base_Bus::Filter::COMPARE_EQUAL, -1);
		}

		// Filter on state
		addStateFilters(filter);

		// Filter on time
		addTimeFilters(filter);

		return filter;
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::addTimeFilters(TA_Base_Bus::Filter& filter)
	{
		bool filterOnFromTime = (static_cast<CButton*>(GetDlgItem(IDC_FROM_TIME))->GetCheck() == 1)? true : false;
		bool filterOnToTime = (static_cast<CButton*>(GetDlgItem(IDC_TO_TIME))->GetCheck() == 1)? true : false;

		CTime lowerboundTime, lowerboundDate, upperboundTime, upperboundDate;
		CTime fromResult, toResult;

		if (filterOnFromTime) //Only get From time if there is request
		{
			m_fromDate.GetTime(lowerboundDate);
			m_fromTime.GetTime(lowerboundTime);
			fromResult = getCombinedTime(lowerboundDate, lowerboundTime);
		}

		if (filterOnToTime) //Only get To time if there is request
		{
			m_toDate.GetTime(upperboundDate);
			m_toTime.GetTime(upperboundTime);
			toResult = getCombinedTime(upperboundDate, upperboundTime);
		}

		if (filterOnFromTime && !filterOnToTime)
		{
			// Filter on from time
			filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME,
				TA_Base_Bus::Filter::COMPARE_GREATER_THAN_EQUAL, fromResult.GetTime());
		}
		else if (!filterOnFromTime && filterOnToTime)
		{
			// Filter on to time
			filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME,
				TA_Base_Bus::Filter::COMPARE_LESS_THAN_EQUAL, toResult.GetTime());
		}
		else if (filterOnFromTime && filterOnToTime)
		{
			// Filter on both from time and to time
			TA_Base_Bus::Filter::SDateRange range;
			range.startTime = fromResult.GetTime();
			range.endTime = toResult.GetTime();

			filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME_RANGE,
				TA_Base_Bus::Filter::COMPARE_RANGE_INCLUSIVE, range);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::addStateFilters(TA_Base_Bus::Filter& filter)
	{
		bool filterOnAcked   = ( static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->GetCheck() == 1)? true : false;
		bool filterOnUnacked = ( static_cast<CButton*>(GetDlgItem(IDC_ACK_UNACKED))->GetCheck() == 1 )? true : false;
		bool filterOnOpen    = ( static_cast<CButton*>(GetDlgItem(IDC_STATE_OPEN))->GetCheck() == 1)? true : false;
		bool filterOnClosed  = ( static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->GetCheck() == 1)? true : false;

		if (filterOnAcked)
		{
			if (filterOnOpen)
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_ACKED);
			}
			else if (filterOnClosed)
			{
				// Should never get here.
				TA_ASSERT(false, "Cannot filter on alarms that are acked AND closed");
			}
			else
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_ACKED);
			}
		}
		else if (filterOnUnacked)
		{
			if (filterOnOpen)
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_UNACKED);
			}
			else if (filterOnClosed)
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_CLOSED);
			}
			else
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_ALL_UNACKED);
			}
		}
		else
		{
			if (filterOnOpen)
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_ALL_OPEN);
			}
			else if (filterOnClosed)
			{
				filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
					TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_CLOSED);
			}
			else
			{
				// Do nothing
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Set or Clear all the severity checkbox controls
	void AlarmFilterDlg::SetAllSeverityFilterControls(BOOL doSet)
	{
        int doCheck = 0;
        if(doSet)
        {
            doCheck = 1;
        }

        for(int i = 0; i < m_severityList.GetCount(); ++i)
        {
            m_severityList.SetCheck(i, doCheck);
        }

	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::setAckRadioButton(int resourceID)
	{
		TA_ASSERT(resourceID == IDC_ACK_ACKED || resourceID == IDC_ACK_UNACKED
					|| resourceID == IDC_ACK_ALL, 
					"Invalid resource ID for setAckRadioButton");

		static_cast<CButton*>(GetDlgItem(IDC_ACK_ACKED))->SetCheck(IDC_ACK_ACKED == resourceID);
		static_cast<CButton*>(GetDlgItem(IDC_ACK_UNACKED))->SetCheck(IDC_ACK_UNACKED == resourceID);
		static_cast<CButton*>(GetDlgItem(IDC_ACK_ALL))->SetCheck(IDC_ACK_ALL == resourceID);
	}

	/////////////////////////////////////////////////////////////////////////////
	void AlarmFilterDlg::setStateRadioButton(int resourceID)
	{
		TA_ASSERT(resourceID == IDC_STATE_OPEN || resourceID == IDC_STATE_CLOSED
					|| resourceID == IDC_STATE_ALL, 
					"Invalid resource ID for setStateRadioButton");

		static_cast<CButton*>(GetDlgItem(IDC_STATE_OPEN))->SetCheck(IDC_STATE_OPEN == resourceID);
		static_cast<CButton*>(GetDlgItem(IDC_STATE_CLOSED))->SetCheck(IDC_STATE_CLOSED == resourceID);
		static_cast<CButton*>(GetDlgItem(IDC_STATE_ALL))->SetCheck(IDC_STATE_ALL == resourceID);
	}

	// TD17081- modified the parameter to the severitydata instead of the item key to
	// limit database access
 	COLORREF AlarmFilterDlg::getSeverityColour(TA_Base_Core::IAlarmSeverityData* severityData)
    {
        COLORREF severityColour(RGB(0, 0, 0));

		TA_ASSERT(severityData != NULL, "Invalid severity data.");
        try
        {
			// TD17081++
            // iSeverityData = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getSeverityByKey(severityKey);
            // TA_Base_Core::AlarmSeverityData* severityData = dynamic_cast <TA_Base_Core::AlarmSeverityData*> (iSeverityData);			
            // severityColour = severityData->getSeverityColourKey(TA_Base_Core::IAlarmSeverityData::OPEN_ACKED_BG1); // TD15314 wangyn
			// ++TD17081
			// TD15314 wangyn
			severityColour = severityData->getSeverityColourKey(TA_Base_Core::IAlarmSeverityData::OPEN_UNACKED_BG1);
            
        }
        //TD17081 - modified exception handling
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not retrieve severity colour. Setting to default.");            
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not retrieve severity colour. Setting to default.");            
        }   
		//++TD17081
        return severityColour;		
    }


	void AlarmFilterDlg::selectUserFilter()
	{
		m_namedFilter.AddTempString( NamedFilter::UNNAMED.c_str() );
		m_saveButton.EnableWindow( true );
		m_deleteButton.EnableWindow( false );
	}

	void AlarmFilterDlg::enableButtons()
	{
		// Enable buttons as required.
		m_okButton.EnableWindow(true);
		m_applyButton.EnableWindow(true);
		m_cancelButton.EnableWindow(true);
	}

	void AlarmFilterDlg::disableButtons()
	{
		// Disable buttons as required.
//		m_okButton.EnableWindow(false);  //changxing:need not do this.
		m_applyButton.EnableWindow(false);
		m_cancelButton.EnableWindow(true);
	}


	void AlarmFilterDlg::OnSelchangeNamedFilter() 
	{
		FUNCTION_ENTRY("OnSelchangeNamedFilter");

		// Get the currently selected item.
		int selIdx = m_namedFilter.GetCurSel();
		if ( 0 <= selIdx )
		{
			// Get the name of the filter.
			CString filterName_c;
			m_namedFilter.GetLBText( selIdx, filterName_c );
			std::string filterName( filterName_c );

			bool isCurFilter = filterName.compare(m_filter.getFilterName()) ? false : true;
			
			//++ Noel
			bool isDefault = filterName.compare(AlarmFilterDlg::FILTER_DEFAULT) ? false : true;
			if (isDefault)
			{
				OnClearFiltersButton();
				return;
			}

			// Make sure the same filter wasn't selected.
			if ( isCurFilter == false )
			{
				std::auto_ptr<NamedFilter> loadedFilter;

				try
				{
					// Load the filter.
					loadedFilter = TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( filterName );
				}
				catch ( TA_Base_Core::DataException& ex )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

					// Display the load filter error message.
					TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << sLoad << sLoaded;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);

					// Reselect the current named filter.
					populateFilterSelections(m_filter);
					return;
				}
                catch(...)
                {
                    // Display the load filter error message.
					TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << sLoad << sLoaded;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);
                    // Reselect the current named filter.
					populateFilterSelections(m_filter);
					return;
                }

				// Copy the new filter to our internal filter.
				m_nmFilter.clear();
				m_nmFilter = *(loadedFilter.get());

				// Convert the nmFilter to Filter class
				std::vector<std::string> stringList = m_nmFilter.getStringFilterList();
				std::vector<std::string> numericList = m_nmFilter.getNumericFilterList();
				if (stringList.size() > 0 || numericList.size() > 0) {
					m_filter.removeAll();
					m_filter = convertNamedFilterToFilter( m_nmFilter );
					populateFilterSelections(m_filter);
				}
				else
				{
					//Warn user of null named filter
					TA_Base_Bus::TransActiveMessage userMsg;
					//const std::string retEr("retrieve ");
					//const std::string fileEr("load using " + filterName);
					const std::string retEr("获取 ");
					const std::string fileEr("用 " + filterName + " 载入");
					userMsg << retEr;
					userMsg << fileEr;
					userMsg.showMsgBox(IDS_UE_040031);

					m_namedFilter.SetWindowText(filterName_c); // set it back to the previous name

					return;
				}

				// Set the state of the save and delete buttons.
				m_namedFilter.SetWindowText(filterName_c);
				m_saveButton.EnableWindow( true );
				m_deleteButton.EnableWindow( true );

				setUpdateNeeded(true); // A new filter selected => need updating
				m_saveButton.EnableWindow( false );
				m_savedFilter = true; // A newly loaded filter need not be saved again unless change is made
			}
			else
			{
				setUpdateNeeded(false); // Filter is current => no need to update
				m_saveButton.EnableWindow(false);
				m_savedFilter = true;
			}
		}

		FUNCTION_EXIT;
		
	}

	void AlarmFilterDlg::OnDelete() 
	{
		FUNCTION_ENTRY("OnDeleteFilter");

		// Get the name to use.
		CString fName;
		m_namedFilter.GetWindowText(fName);
		std::string filterName(fName);

		bool isUserDefined = filterName.compare(TA_Base_Core::NamedFilter::UNNAMED) ? false : true;
		bool isDefault = filterName.compare(AlarmFilterDlg::FILTER_DEFAULT) ? false : true;

		// User can't delete a not existing Default or User Defined filter
		if ( isUserDefined || isDefault )
		{
			// Display a message box to disallow these keywords as name
			TA_Base_Bus::TransActiveMessage userMsg;
			//const std::string delEr("delete an invalid name for ");
			//const std::string fileEr("deleted as " + filterName);
			const std::string delEr("删除无效的名字为这个");
			const std::string fileEr("删除为 " + filterName);
			userMsg << delEr;
			userMsg << fileEr;
			userMsg.showMsgBox(IDS_UE_040031);

			return;
		}

		if (checkInNamedFilterDatabase(filterName)) // Check that it is a saved filter
		{
			int index = m_namedFilter.GetCurSel();
			m_namedFilter.GetLBText(index, fName);
		}
		else // The name is not in the database and cannot be deleted
		{
			// Display a message box to disallow the delete function
			TA_Base_Bus::TransActiveMessage userMsg;
			//const std::string delEr("delete ");
			//const std::string fileEr("deleted as " + filterName + " not in the database");
			const std::string delEr("删除 ");
			const std::string fileEr("删除 因为 " + filterName + " 不在数据库中");
			userMsg << delEr;
			userMsg << fileEr;
			userMsg.showMsgBox(IDS_UE_040031);

			return;
		}

		bool isRightFile = m_nmFilter.getName() == filterName ? true : false; // Ensure that the filter is correctly selected
		// Delete an exiting NamedFilter in database
		if (isRightFile == true)
		{
			// Confirm the deletion.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << "删除 '" + m_nmFilter.getName() + "'";
			if ( IDNO == userMsg.showMsgBox(IDS_UW_010003) )
			{
				return;
			}

			bool isDeleteOk = deleteNamedFilter (m_nmFilter);

			if (isDeleteOk == false)
			{
				// Display not successful message
				TA_Base_Bus::TransActiveMessage userMsg;
				//const std::string delEr("delete ");
				//const std::string fileEr("delete " + m_filter.getFilterName() + " due to database error");
				const std::string delEr("删除 ");
				const std::string fileEr("删除 " + m_filter.getFilterName() + " 由于数据库错误");
				userMsg << delEr;
				userMsg << fileEr;
				userMsg.showMsgBox(IDS_UE_040031);

				populateNamedFilters(filterName);
				
				return;
			}
			else if (stricmp(m_strCurDlgFilter.c_str(), filterName.c_str()) ==0)
				m_nCurFilterState = -1;

			m_bNMFilterUpdated = true;
			// Go back to default setting when old filter is deleted
			// And recreate all settings
			OnClearFiltersButton();
			//marvin++
			//TD14756
			setUpdateNeeded(true);
			//++marvin
			//TD14756

			//TD15032
			populateNamedFilters(AlarmFilterDlg::FILTER_DEFAULT);
			//TD15032
			// Upon successful delete, notify the parent dialog to update the named filter list
			filterIndex = INDEX_LIST;
			//AfxGetMainWnd()->PostMessage(WM_ON_FILTER_DLG_DELETE, filterIndex, 0); //TD15032 azenitha
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15032: No need to refresh main dialog");
		}

		FUNCTION_EXIT;
	}

	bool AlarmFilterDlg::deleteNamedFilter (TA_Base_Core::NamedFilter& nmFilter)
	{
		FUNCTION_ENTRY("deleteNamedFilter");
		
		try
		{
			// Delete the current named filter.
			nmFilter.deleteNamedFilter();

			return true;
		}
		catch ( TA_Base_Core::DataException& )
		{
			return false;
		}

		FUNCTION_EXIT;
	}


	TA_Base_Bus::Filter AlarmFilterDlg::convertNamedFilterToFilter (TA_Base_Core::NamedFilter& nmFilter)
	{
		FUNCTION_ENTRY("convertNamedFilterToFilter");

		// Create a filter object
		TA_Base_Bus::Filter filter;
		std::vector<std::string> sVector;
		std::vector<std::string>::iterator its;
		std::vector<unsigned long> ulVector;
		std::vector<unsigned long>::iterator itl;

		// Filter on subsystem
		try
		{
			// add filter for subsystem
			ulVector = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_SUBSYSTEM);
			if (ulVector.size() > 0)
			{
				for ( itl = ulVector.begin(); itl != ulVector.end(); ++itl )
				{
					filter.addFilter( TA_Base_Bus::Filter::FILTER_SUBSYSTEM, TA_Base_Bus::Filter::COMPARE_EQUAL, *itl );
				}
			}
		
			// add filter on location
			ulVector.clear();
			ulVector = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_LOCATION);
			if (ulVector.size() > 0) 
			{
				for ( itl = ulVector.begin(); itl != ulVector.end(); ++itl )
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_LOCATION, TA_Base_Bus::Filter::COMPARE_EQUAL, *itl);
				}
			}

			// add filter on operator
			sVector = nmFilter.getStringFilter(AlarmFilterDlg::FILTER_OPERATOR);
			if (sVector.size() > 0) 
			{
				for ( its = sVector.begin(); its != sVector.end(); ++its )
				{
					// Add a filter for this STRING
					filter.addFilter(TA_Base_Bus::Filter::FILTER_OPERATOR, TA_Base_Bus::Filter::COMPARE_EQUAL, *its );
				}
			}

			// add filter on alarm type
			ulVector.clear();
			ulVector = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_ALARMTYPE);
			if (ulVector.size() > 0)
			{
				for ( itl = ulVector.begin(); itl != ulVector.end(); ++itl )
				{
					// Add a filter for this STRING
					filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TYPE, TA_Base_Bus::Filter::COMPARE_EQUAL, *itl );
				}
			}

			// add filter on asset
			sVector.clear();
			sVector = nmFilter.getStringFilter(AlarmFilterDlg::FILTER_ASSET);
			if (sVector.size() > 0) 
			{
				for ( its = sVector.begin(); its != sVector.end(); ++its )
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_ASSET_NAME, TA_Base_Bus::Filter::COMPARE_CONTAINS, *its );
				}
			}

			// add filter on description
			sVector.clear();
			sVector = nmFilter.getStringFilter(AlarmFilterDlg::FILTER_DESCRIPTION);
			if (sVector.size() > 0) 
			{
				for ( its = sVector.begin(); its != sVector.end(); ++its )
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_DESCRIPTION, TA_Base_Bus::Filter::COMPARE_CONTAINS, *its );
				}
			}

			// add filter on severity
			ulVector.clear();
			ulVector = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_SEVERITY);
			if (ulVector.size() > 0) 
			{
				for( itl = ulVector.begin(); itl != ulVector.end(); ++itl )
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_SEVERITY, TA_Base_Bus::Filter::COMPARE_EQUAL, *itl );
				}
			}
			else
			{
				// Create a filter that filters out all severities
				// Severities cannot be negative, so the following filter will do
				filter.addFilter(TA_Base_Bus::Filter::FILTER_SEVERITY, TA_Base_Bus::Filter::COMPARE_LESS_THAN, 0);
			}

			//add filter on state
			std::string ack;
			std::string state;
			sVector.clear();
			sVector = nmFilter.getStringFilter(AlarmFilterDlg::FILTER_ACKSTATE);
			if (sVector.size() >0) ack = sVector[0];
			
			sVector.clear();
			sVector = nmFilter.getStringFilter(AlarmFilterDlg::FILTER_STATE);
			if (sVector.size() >0) state = sVector[0];
			
			if (ack == AlarmFilterDlg::FILTER_ACKED)
			{
				if (state == AlarmFilterDlg::FILTER_OPEN)
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_ACKED);
				}
				else if (state == AlarmFilterDlg::FILTER_CLOSE)
				{
					// Should never get here.
					TA_ASSERT(false, "Cannot filter on alarms that are acked AND closed");
				}
				else // Default state is ALL
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_ACKED);
				}
			}
			else if (ack == AlarmFilterDlg::FILTER_UNACKED)
			{
				if (state == AlarmFilterDlg::FILTER_OPEN)
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_OPEN_UNACKED);
				}
				else if (state == AlarmFilterDlg::FILTER_CLOSE)
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_CLOSED);
				}
				else // Default the state is ALL
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_ALL_UNACKED);
				}
			}
			else // Default the acknowledge is ALL
			{
				if (state == AlarmFilterDlg::FILTER_OPEN)
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_ALL_OPEN);
				}
				else if (state == AlarmFilterDlg::FILTER_CLOSE)
				{
					filter.addFilter(TA_Base_Bus::Filter::FILTER_STATE,
						TA_Base_Bus::Filter::COMPARE_EQUAL, TA_Base_Bus::Filter::STATE_CLOSED);
				}
				else // Default the state is ALL
				{
					// Do nothing
				}
			}

			// add filter on time
			// Get the FROM time.
		    time_t fromTime = 0;
			NamedFilter::NumericList ftimeList = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_FROMTIME);
			if ( 0 < ftimeList.size() ) fromTime = static_cast<time_t>(ftimeList[0]);

			// Get the TO time.
			time_t toTime = 0;
			NamedFilter::NumericList ttimeList = nmFilter.getNumericFilter(AlarmFilterDlg::FILTER_TOTIME);
			if ( 0 < ttimeList.size() ) toTime = static_cast<time_t>(ttimeList[0]);

			if (fromTime && !toTime)
			{
				// Filter on from time
				CTime ftime(fromTime);
				filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME,
					TA_Base_Bus::Filter::COMPARE_GREATER_THAN_EQUAL, ftime.GetTime());
			}
			else if (!fromTime && toTime)
			{
				// Filter on to time
				CTime ttime(toTime);
				filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME,
					TA_Base_Bus::Filter::COMPARE_LESS_THAN_EQUAL, ttime.GetTime());
			}
			else if (fromTime && toTime)
			{
				// Filter on both from time and to time
				CTime ftime(fromTime);
				CTime ttime(toTime);
				TA_Base_Bus::Filter::SDateRange range;
				range.startTime = ftime.GetTime();
				range.endTime = ttime.GetTime();

				filter.addFilter(TA_Base_Bus::Filter::FILTER_ALARM_TIME_RANGE,
					TA_Base_Bus::Filter::COMPARE_RANGE_INCLUSIVE, range);
			}

			filter.setFilterName( nmFilter.getName() );
		}
		catch (...)
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to convert Named Filter to Filter object");
			TA_ASSERT (false, "Failed to convert Named Filter to Filter object");

		}

		return filter;

		FUNCTION_EXIT;
	}


	TA_Base_Core::NamedFilter AlarmFilterDlg::convertFilterToNamedFilter (TA_Base_Bus::Filter& filter)
	{
		FUNCTION_ENTRY("convertFilterToNamedFilter");

		TA_Base_Core::NamedFilter nmFilter;
		try
		{
			std::vector<TA_Base_Bus::Filter::FilterDetails> filterDetails = filter.getFilters();

			for (std::vector<TA_Base_Bus::Filter::FilterDetails>::iterator iter = filterDetails.begin() ; iter != filterDetails.end() ; ++iter )
			{
				TA_Base_Bus::Filter::FilterDetails fDetails = *iter;
				switch (fDetails.type)
				{
					case TA_Base_Bus::Filter::FILTER_SUBSYSTEM:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_SUBSYSTEM, fDetails.value.ulValue);
						break;

					case TA_Base_Bus::Filter::FILTER_LOCATION:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_LOCATION, fDetails.value.ulValue);
						break;

					case TA_Base_Bus::Filter::FILTER_OPERATOR:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_OPERATOR, *fDetails.value.stringValue);
						break;

					case TA_Base_Bus::Filter::FILTER_ALARM_TYPE:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_ALARMTYPE, fDetails.value.ulValue);
						break;

					case TA_Base_Bus::Filter::FILTER_ASSET_NAME:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_CONTAINS)
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ASSET, *fDetails.value.stringValue);
						break;

					case TA_Base_Bus::Filter::FILTER_DESCRIPTION:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_CONTAINS)
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_DESCRIPTION, *fDetails.value.stringValue);
						break;

					case TA_Base_Bus::Filter::FILTER_SEVERITY:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_EQUAL)
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_SEVERITY, fDetails.value.ulValue);
						break;

					case TA_Base_Bus::Filter::FILTER_STATE:
						if (fDetails.value.alarmStateValue == TA_Base_Bus::Filter::STATE_OPEN_ACKED)
						{
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ACKSTATE, AlarmFilterDlg::FILTER_ACKED);
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_STATE, AlarmFilterDlg::FILTER_OPEN);
						}
						else if (fDetails.value.alarmStateValue == TA_Base_Bus::Filter::STATE_OPEN_UNACKED)
						{
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ACKSTATE, AlarmFilterDlg::FILTER_UNACKED);
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_STATE, AlarmFilterDlg::FILTER_OPEN);
						}
						else if (fDetails.value.alarmStateValue == TA_Base_Bus::Filter::STATE_CLOSED)
						{
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ACKSTATE, AlarmFilterDlg::FILTER_UNACKED);
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_STATE, AlarmFilterDlg::FILTER_CLOSE);
						}
						else if (fDetails.value.alarmStateValue == TA_Base_Bus::Filter::STATE_ALL_UNACKED)
						{
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ACKSTATE, AlarmFilterDlg::FILTER_UNACKED);
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_STATE, AlarmFilterDlg::FILTER_ALL);
						}
						else if (fDetails.value.alarmStateValue == TA_Base_Bus::Filter::STATE_ALL_OPEN)
						{
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_ACKSTATE, AlarmFilterDlg::FILTER_ALL);
							nmFilter.addStringFilter(AlarmFilterDlg::FILTER_STATE, AlarmFilterDlg::FILTER_OPEN);
						}
						break;

					case TA_Base_Bus::Filter::FILTER_ALARM_TIME:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_GREATER_THAN_EQUAL)
						{
							time_t ftime = fDetails.value.ulValue;
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_FROMTIME, ftime);
						}
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_LESS_THAN_EQUAL)
						{
							time_t ttime = fDetails.value.ulValue;
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_TOTIME, ttime);
						}
						break;

					case TA_Base_Bus::Filter::FILTER_ALARM_TIME_RANGE:
						if (fDetails.comparison == TA_Base_Bus::Filter::COMPARE_RANGE_INCLUSIVE)
						{
							time_t ftime = fDetails.value.dateRange.startTime;
							time_t ttime = fDetails.value.dateRange.endTime;
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_FROMTIME, ftime);
							nmFilter.addNumericFilter(AlarmFilterDlg::FILTER_TOTIME, ttime);
						}
						break;

					default: // Do nothing
						 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No filter settings from named filter for filter");

				}
			}

			nmFilter.setName( filter.getFilterName() );

		}

		catch (...)
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to convert Filter to Named Filter object");
			TA_ASSERT (false, "Failed to convert Filter to Named Filter object");

		}
		return nmFilter;

		FUNCTION_EXIT;
	}

	bool AlarmFilterDlg::checkInNamedFilterDatabase(std::string curName)
	{
		FUNCTION_ENTRY("checkInNamedFilterDatabase");

		try
		{			
			// Get a list of the filters.
			TA_Base_Core::NamedFilterAccessFactory::FilterNameList filters = TA_Base_Core::NamedFilterAccessFactory::getInstance().getAllFilterNames();

			// Now add each to the combo box.
			TA_Base_Core::NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
			for ( namedFilterIt=filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
			{
				if ( curName == namedFilterIt->first ) return true;
			}

		}
		catch ( TA_Base_Core::DataException& )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not get a list of named filters!");
		}

		return false;

		FUNCTION_EXIT;
	}
	
	void AlarmFilterDlg::populateNamedFilters(std::string curName)
	{

		// Make sure the combo box is empty.
		m_namedFilter.ResetContent();
		bool isUserDefined = curName.compare(TA_Base_Core::NamedFilter::UNNAMED) ? false : true;
		bool isDefault = curName.compare(AlarmFilterDlg::FILTER_DEFAULT) ? false : true;
		int nDefaultIdx = 0;
		try
		{			
			// Get a list of the filters.
			TA_Base_Core::NamedFilterAccessFactory::FilterNameList filters = TA_Base_Core::NamedFilterAccessFactory::getInstance().getAllFilterNames();

			// Now add each to the combo box.
			TA_Base_Core::NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
			for ( namedFilterIt=filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
			{
				int index = m_namedFilter.AddString( namedFilterIt->first.c_str() );
			}

			// Add the default filter to the ComboBox for default selection
			nDefaultIdx = m_namedFilter.AddString(AlarmFilterDlg::FILTER_DEFAULT.c_str());

		}
		catch ( TA_Base_Core::DataException& )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not get a list of named filters!");
		}

		// Check validity of curName and set the combo box to the name
		if (isDefault == false && isUserDefined == false)
		{
			// If it is the current filter then select it.
			int index = m_namedFilter.SelectString(-1, curName.c_str());
			m_namedFilter.SetCurSel(index);
			m_saveButton.EnableWindow( true );
			m_deleteButton.EnableWindow( true );
		}
		
		// If the filter is custom it won't appear in the combo box, so add its text
		// manually.
		else if ( isUserDefined == true )
		{
			selectUserFilter();
		}
		else if ( isDefault == true ) // Default filter name
		{

			//m_namedFilter.AddTempString(AlarmFilterDlg::FILTER_DEFAULT.c_str());
			m_namedFilter.SetCurSel(nDefaultIdx);
			m_saveButton.EnableWindow( false );
			m_deleteButton.EnableWindow( false );
		}
	}

	void AlarmFilterDlg::setNamedFilter(std::string filterName)
	{
		bool isDefault = (filterName == AlarmFilterDlg::FILTER_DEFAULT) ? true : false;
		//bool isCurFilter = (filterName == m_filter.getFilterName()) ? true : false;
		//if (isCurFilter) return; // No need to reapply current filter
		
		if ( filterName != "" && isDefault == false && checkInNamedFilterDatabase(filterName) == true ) // This filter can be found in database
		{
			// Load the named filter from database	
			std::auto_ptr<NamedFilter> loadedFilter;

			try
			{
				// Load the filter.
				loadedFilter = TA_Base_Core::NamedFilterAccessFactory::getInstance().getNamedFilter( filterName );
			}
			catch ( TA_Base_Core::DataException& ex )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

				// Display the load filter error message.
				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << sLoad << sLoaded;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_040031);

				return;
			}

			// Copy the new filter to our internal filter.
			m_nmFilter.clear();
			m_nmFilter = *(loadedFilter.get());

			// Convert the nmFilter to Filter class
			std::vector<std::string> stringList = m_nmFilter.getStringFilterList();
			std::vector<std::string> numericList = m_nmFilter.getNumericFilterList();
			
			if (stringList.size() > 0 || numericList.size() > 0) {
				// Clear the current filter and set it the to named filter
				m_filter.removeAll();
				m_filter = convertNamedFilterToFilter( m_nmFilter );
			}
			else
			{
				//Warn user of null named filter
				TA_Base_Bus::TransActiveMessage userMsg;
				const std::string retEr("retrieve ");
				const std::string fileEr("load using " + filterName);
				userMsg << retEr;
				userMsg << fileEr;
				userMsg.showMsgBox(IDS_UE_040031);

				return;
			}

		}
		else if ( isDefault == true ) // This is for default setting
		{
			m_filter.removeAll();
			m_filter.setFilterName(AlarmFilterDlg::FILTER_DEFAULT);
		}
		else // The filter name is undefined or not present
		{
			// Display a error message to notify an invalid filter name
			TA_Base_Bus::TransActiveMessage userMsg;
			const std::string retEr("load ");

			if ( filterName == "" )
			{
				//const std::string fileEr("load using empty string. Please provide a valid name");
				const std::string fileEr("用空字符串载入. 请提供有效名字");
				userMsg << retEr;
				userMsg << fileEr;
				userMsg.showMsgBox(IDS_UE_040031);
			}
			else
			{
				//const std::string fileEr("load using " + filterName + " as it is not found in database");
				const std::string fileEr("用 " + filterName + " 载入, 因为没有在数据库中找到");
				userMsg << retEr;
				userMsg << fileEr;
				userMsg.showMsgBox(IDS_UE_040031);
			}

			return;
		}

		// set the alarm control list to this filter
		try
		{
			//clear all the fitlers in the alarm list
			m_alarmList.setFilter(m_filter,isDefault);
		}
		catch(TA_Base_Core::AlarmListCtrlException)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UI_040022);
		}
	}


    void AlarmFilterDlg::fromCheckBoxUpdated(bool isTicked)
    {
        // TD 8703: Requirement is that the date and time fields are blanked out if the checkboxes
        // are unchecked. As date time controls don't allow empty strings, replacing these fields 
        // with a combo box and edit control gives the desired affect.

        int windowState = SW_SHOW;
        if(isTicked)
        {
            windowState = SW_HIDE;
        }

		//TD15028++ wangyn
		CString fromDateStr;
		CString fromTimeStr;

		GetDlgItemText(IDC_DATETIMEPICKER_FROMDATE, fromDateStr);
		GetDlgItemText(IDC_DATETIMEPICKER_FROMTIME, fromTimeStr);

		m_comboFrom.SetWindowText(fromDateStr);
		m_editFrom.SetWindowText(fromTimeStr);
		//++TD15208
        m_comboFrom.ShowWindow(windowState);
        m_comboFrom.EnableWindow(false);
        
        m_editFrom.ShowWindow(windowState);
        m_editFrom.EnableWindow(false);
        
        m_spinFrom.ShowWindow(windowState);
        m_spinFrom.EnableWindow(false);
        
        if(windowState == SW_SHOW)
        {
            windowState = SW_HIDE;
        }
        else
        {
            windowState = SW_SHOW;
        }

        m_fromDate.ShowWindow(windowState);
        m_fromTime.ShowWindow(windowState);
    }


    void AlarmFilterDlg::toCheckBoxUpdated(bool isTicked)
    {
        // TD 8703: Requirement is that the date and time fields are blanked out if the checkboxes
        // are unchecked. As date time controls don't allow empty strings, replacing these fields 
        // with a combo box and edit control gives the desired affect.

        int windowState = SW_SHOW;
        if(isTicked)
        {
            windowState = SW_HIDE;
        }
		//TD15028 ++ wangyn
		CString toDateStr;
		CString toTimeStr;

		GetDlgItemText(IDC_DATETIMEPICKER_TODATE, toDateStr);
		GetDlgItemText(IDC_DATETIMEPICKER_TOTIME, toTimeStr);

		m_comboTo.SetWindowText(toDateStr);
		m_editTo.SetWindowText(toTimeStr);
        //++ TD15028

        m_comboTo.ShowWindow(windowState);
        m_comboTo.EnableWindow(false);

        m_editTo.ShowWindow(windowState);
        m_editTo.EnableWindow(false);

        m_spinTo.ShowWindow(windowState);
        m_spinTo.EnableWindow(false);
        
        if(windowState == SW_SHOW)
        {
            windowState = SW_HIDE;
        }
        else
        {
            windowState = SW_SHOW;
        }

        m_toDate.ShowWindow(windowState);
        m_toTime.ShowWindow(windowState);
    }

	//TD15028++ wangyn
	void AlarmFilterDlg::setFromDateTime(TA_Base_Bus::AlarmListCtrl& listCtrl)
	{
		CTime currentTime = CTime::GetCurrentTime();
		time_t earliestTime = listCtrl.getEarliestAlarmTime();

		if(earliestTime == 0)
		{
			earliestTime = currentTime.GetTime();
		}

		CTime fromEarliest(earliestTime);

		m_fromDate.SetTime(&fromEarliest);
		m_fromTime.SetTime(&fromEarliest);

		m_toDate.SetRange(&fromEarliest,&currentTime);
		m_toTime.SetRange(&fromEarliest,&currentTime);
	}
	//++TD15208

	
	//TD15208++
	void AlarmFilterDlg::setToDateTime(TA_Base_Bus::AlarmListCtrl& listCtrl)
	{
		CTime currentTime = CTime::GetCurrentTime();
		time_t latestTime = m_alarmList.getLatestAlarmTime();

		if(latestTime==0)
		{
			latestTime = currentTime.GetTime();
		}

		CTime toLatestTime(latestTime);
			
		m_toDate.SetTime(&toLatestTime);
		m_toTime.SetTime(&toLatestTime);

        m_fromTime.SetRange(&EARLY_TIME_LIMIT,&currentTime);
        m_fromDate.SetRange(&EARLY_TIME_LIMIT,&currentTime);
	}
	//++TD15208

} // TA_Base_App
//Note: When auto generating windows message handlers, you will have to move it above the 
//      TA_Base_App scope.


