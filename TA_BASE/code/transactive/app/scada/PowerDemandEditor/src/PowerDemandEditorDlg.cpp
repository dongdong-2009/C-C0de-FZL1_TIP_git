/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditorDlg.cpp $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the main dialog used to display the power demand data information.  
  * 
  */

#include "app/scada/PowerDemandEditor/src/stdafx.h"

#include <sstream>
#include <set>
#include "ace/OS.h"

#include "app/scada/PowerDemandEditor/src/PowerDemandEditor.h"
#include "app/scada/PowerDemandEditor/src/resource.h"
#include "app/scada/PowerDemandEditor/src/PowerDemandEditorDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/src/IPowerDemandData.h"
#include "core/data_access_interface/src/PowerDemandAccessFactory.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActiveDialog;

using namespace TA_Base_App;
using namespace TA_Base_Core;


namespace TA_Base_App
{

	/////////////////////////////////////////////////////////////////////////////
	// CPowerDemandEditorDlg dialog

	//
	// Constructor
	//
	CPowerDemandEditorDlg::CPowerDemandEditorDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
	: TransActiveDialog(genericGUICallback, CPowerDemandEditorDlg::IDD, pParent)
	{
		FUNCTION_ENTRY ("CPowerDemandEditorDlg()");

		FUNCTION_EXIT;
	}

	//
	// Destructor
	//
	CPowerDemandEditorDlg::~CPowerDemandEditorDlg()
	{
		FUNCTION_ENTRY ("~CPowerDemandEditorDlg()");

	
		FUNCTION_EXIT;
	}

	//
	// DoDataExchange
	//
	void CPowerDemandEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CPowerDemandEditorDlg)
		DDX_Control(pDX, IDC_COMBO_METER, m_meterPointCombo);
		DDX_Control(pDX, IDC_INTERVALTIMEPICKER, m_intervalTimePicker);
		DDX_Control(pDX, IDC_INTERVALDATEPICKER, m_intervalDatePicker);
		DDX_Control(pDX, IDC_EDIT_RECORDED, m_recordedValueEdit);
		DDX_Control(pDX, IDC_EDIT_CURRENT, m_updatedValueEdit);
		DDX_Control(pDX, IDC_EDIT_NEW, m_newValueEdit);
	//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}

	
	/////////////////////////////////////////////////////////////////////////////
	// CPowerDemandEditorDlg message handlers

	BEGIN_MESSAGE_MAP(CPowerDemandEditorDlg, TA_Base_Bus::TransActiveDialog)
		//{{AFX_MSG_MAP(CPowerDemandEditorDlg)
		ON_BN_CLICKED(IDC_BTN_APPLY, OnApply) 
		ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel) 
		//}}AFX_MSG_MAP
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_INTERVALDATEPICKER, OnIntervalDateSelectionChange)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_INTERVALTIMEPICKER, OnIntervalTimeSelectionChange)
		ON_CBN_SELCHANGE(IDC_COMBO_METER, OnMeterPointSelectionChange)
	END_MESSAGE_MAP()


	//
	// OnInitDialog
	//
	BOOL CPowerDemandEditorDlg::OnInitDialog()
	{
		FUNCTION_ENTRY ("OnInitDialog()");
	
		CRect windowSize;
		GetWindowRect(&windowSize);
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise = false;
		properties.maxHeight = -1;
		properties.maxWidth = -1;
		properties.minHeight = windowSize.bottom - windowSize.top;
		properties.minWidth = windowSize.right - windowSize.left;
		setResizingProperties(properties);
		
		TransActiveDialog::OnInitDialog();
	


		FUNCTION_EXIT;
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	void CPowerDemandEditorDlg::init()
	{
		// initialise data/time picker to the most recent 30-minute interval, based on current time

		// get the current time.
		CTime currentTime( CTime::GetCurrentTime() );
 
		// adjusted the time for 30-min interval display
		int minute = currentTime.GetMinute();
	   
		CTime convertedTime( currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetHour(), 
							 currentTime.GetHour(), (abs(30-currentTime.GetMinute())==1?0:30), 0 ); 

		// set the date and time picker
		m_intervalDatePicker.SetTime(&currentTime);
		m_intervalTimePicker.SetTime(&convertedTime);


		// get power demand meter data point names

		std::set<std::string> powerDemandDataPointName;
		bool isPowerDemandDataEmpty = false;

		try
		{
		
			PowerDemandAccessFactory::getInstance().getPowerDemandDataPointNames(powerDemandDataPointName);
			
		}
		catch ( const TA_Base_Core::DataException & ex )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.what() );

			isPowerDemandDataEmpty = true;
		}
		catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
              
			 isPowerDemandDataEmpty = true;                 
        }
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception");

			isPowerDemandDataEmpty = true;	
		}

		if (true == isPowerDemandDataEmpty)
		{
			// Construct the error message to display.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << std::string("Power Demand Editor");
			userMsg.showMsgBox(IDS_UE_803008);

			m_meterPointCombo.AddString("<None>");	
			m_meterPointCombo.SetCurSel(0);
			m_meterPointCombo.EnableWindow(false);
			m_intervalDatePicker.EnableWindow(false);
			m_intervalTimePicker.EnableWindow(false);
			m_recordedValueEdit.EnableWindow(false);
			m_updatedValueEdit.EnableWindow(false);
			m_newValueEdit.EnableWindow(false);
			GetDlgItem(IDC_BTN_APPLY)->EnableWindow(false);
			GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(false);

		}

		int length = powerDemandDataPointName.size();

		std::set<std::string>::iterator itr;

		for ( itr=powerDemandDataPointName.begin(); itr!=powerDemandDataPointName.end(); ++itr)
		{
			std::string name = *itr;
			m_meterPointCombo.AddString((*itr).c_str());
		}

	}

	void CPowerDemandEditorDlg::OnMeterPointSelectionChange()
	{
		FUNCTION_ENTRY ("OnMeterPointSelectionChange()");

		refreshPowerDemandValues();

		FUNCTION_EXIT;
	}


	void CPowerDemandEditorDlg::OnIntervalDateSelectionChange(NMHDR* pNMHDR, LRESULT* pResult)
	{
		FUNCTION_ENTRY ("OnIntervalDateSelectionChange()");

		refreshPowerDemandValues();

		FUNCTION_EXIT;
	}


	void CPowerDemandEditorDlg::OnIntervalTimeSelectionChange(NMHDR* pNMHDR, LRESULT* pResult)
	{
		FUNCTION_ENTRY ("OnIntervalDateSelectionChange()");

		m_intervalDatePicker.EnableWindow(false);

		CTime time;
		m_intervalTimePicker.GetTime(time);

		int minute = time.GetMinute();

		if (minute % 30 != 0 || time.GetSecond() != 0)
		{
			if (minute == 29 || minute==31)
			{
				minute = 0;
			}
			else
			{
				minute = 30;
			}

			CTime newTime(time.GetYear(), time.GetMonth(), time.GetDay(),
						  time.GetHour(), (abs(30-time.GetMinute())==1?0:30), 0);

			m_intervalTimePicker.SetTime(&newTime);
		}

		*pResult = 0;

		m_intervalDatePicker.EnableWindow(true);

		refreshPowerDemandValues();

		FUNCTION_EXIT;	
	}


	void CPowerDemandEditorDlg::refreshPowerDemandValues()
	{
		FUNCTION_ENTRY ("refreshPowerDemandValues()");

		// get source metering data point
		if ( CB_ERR == m_meterPointCombo.GetCurSel() )
		{
			FUNCTION_EXIT;
			return;
		}

		CString selectedMeterDpName;
		m_meterPointCombo.GetLBText(m_meterPointCombo.GetCurSel(), selectedMeterDpName);

		std::string meterDpName;
		meterDpName.assign(selectedMeterDpName);


		// get interval
		CTime date;
		CTime time;
		if (GDT_NONE == m_intervalDatePicker.GetTime(date))
		{
			FUNCTION_EXIT;
			return;
		}

		if (GDT_NONE == m_intervalTimePicker.GetTime(time))
		{
			FUNCTION_EXIT;
			return;	
		}

		CTime newTime(date.GetYear(), date.GetMonth(), date.GetDay(),
					  time.GetHour(), time.GetMinute(), 0);

		time_t interval = newTime.GetTime();

		// get power demand data
		PowerDemandData* powerDemandData = new PowerDemandData(meterDpName, interval, 0, 0);

		bool isPowerDemandDataEmpty = false;

		try
		{	
			
			PowerDemandAccessFactory::getInstance().getPowerDemandData(*powerDemandData);

		}
		catch ( const TA_Base_Core::DataException & ex )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.what() );

			isPowerDemandDataEmpty = true;
		}
		catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
              
			 isPowerDemandDataEmpty = true;                 
        }
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception");

			isPowerDemandDataEmpty = true;	
		}


		// update recorded and current value
		if (false == isPowerDemandDataEmpty)
		{
			unsigned long recordedValue = powerDemandData->getRecordedValue();
			unsigned long updatedValue = powerDemandData->getUpdatedValue();
		
			std::ostringstream ssRecordedValue;
			ssRecordedValue << recordedValue;
			m_recordedValueEdit.SetWindowText(ssRecordedValue.str().c_str());

			std::ostringstream ssUpdatedValue;
			ssUpdatedValue << updatedValue;
			m_updatedValueEdit.SetWindowText(ssUpdatedValue.str().c_str());

		}
		else
		{
			// Construct the error message to display.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << std::string("Power Demand Editor");
			userMsg.showMsgBox(IDS_UE_803009);

			m_recordedValueEdit.SetWindowText("<None>");
			m_updatedValueEdit.SetWindowText("<None>");
		}

		delete powerDemandData;
		powerDemandData = NULL;


		FUNCTION_EXIT;
	}


	void CPowerDemandEditorDlg::OnCancel()
	{
		FUNCTION_ENTRY ("OnCancel()");

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();

		FUNCTION_EXIT;
	}


	void CPowerDemandEditorDlg::OnApply()
	{
	
		FUNCTION_ENTRY ("OnApply()");

		// get new value
		CString newValueEdit;
		std::string newValueString;

		m_newValueEdit.GetWindowText(newValueEdit);
		newValueString.assign(newValueEdit);

		if ( 0 == newValueString.compare(""))
		{
			FUNCTION_EXIT;
			return;
		}

		unsigned long newValue;
		std::istringstream ss (newValueString);
		ss >> newValue;	
		
		// get source metering data point
		if ( CB_ERR == m_meterPointCombo.GetCurSel() )
		{
			FUNCTION_EXIT;
			return;
		}

		CString selectedMeterDpName;
		m_meterPointCombo.GetLBText(m_meterPointCombo.GetCurSel(), selectedMeterDpName);

		std::string meterDpName;
		meterDpName.assign(selectedMeterDpName);


		// get interval
		CTime date;
		CTime time;
		if (GDT_NONE == m_intervalDatePicker.GetTime(date))
		{
			FUNCTION_EXIT;
			return;
		}

		if (GDT_NONE == m_intervalTimePicker.GetTime(time))
		{
			FUNCTION_EXIT;	
			return;	
		}

		CTime newTime(date.GetYear(), date.GetMonth(), date.GetDay(),
					  time.GetHour(), time.GetMinute(), 0);

		time_t interval = newTime.GetTime();

	
		PowerDemandData* powerDemandData = new PowerDemandData(meterDpName, interval, 0, newValue);

		bool isPowerDemandSet = true;

		// set power demand data
		try
		{	
			
			PowerDemandAccessFactory::getInstance().setPowerDemandData(*powerDemandData);

		}
		catch ( const TA_Base_Core::DataException & ex )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.what() );

			isPowerDemandSet = false;
		}
		catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", ex.what());
              
			 isPowerDemandSet = false;                 
        }
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception");

			isPowerDemandSet = false;	
		}
	

		// reset current value
		if (false == isPowerDemandSet)
		{
			// Construct the error message to display.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << std::string("Power Demand Editor");
			userMsg.showMsgBox(IDS_UE_803010);
			
			FUNCTION_EXIT;
			return;
		}

		refreshPowerDemandValues();

		FUNCTION_EXIT;
		
	}
	

}
