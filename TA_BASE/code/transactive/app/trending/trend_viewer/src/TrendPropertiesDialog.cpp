/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TrendProperties.cpp : implementation file
//

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/TrendPropertiesDialog.h"
#include "app/trending/trend_viewer/src/ItemSelectDialog.h"
#include "app/trending/trend_viewer/src/TrendConfigDatabaseAccess.h"
#include "app/trending/trend_viewer/src/Utilities.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/ColourDefs.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "app/trending/trend_viewer/src/DataPointSelectDlg.h" //TD17631++

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TrendViewerException.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using TA_Base_Core::DebugUtil;
using TA_Base_Core::TrendViewerException;


namespace TA_Base_App
{

    const int MAX_BUFFER_SIZE = 100;
    const COleDateTime CTrendPropertiesDialog::EARLIEST_START_TIME = COleDateTime(1970, 1, 1, 0, 0, 0);

    /////////////////////////////////////////////////////////////////////////////
    // CTrendPropertiesDialog dialog

    CTrendPropertiesDialog::CTrendPropertiesDialog(IItemStore& itemStore, IArchiveDataStore& archiveDataStore, CWnd* pParent /*=NULL*/)
        : CDialog(CTrendPropertiesDialog::IDD, pParent),
          m_itemStore(itemStore),
          m_archiveDataStore(archiveDataStore),
          m_changeSamplePeriod(false),
          m_defaultOffsetValueStr(""),
          m_disableNameField(false),
          m_initialising(true),
          m_rangeTooLarge(false),
		  m_rangeInSeconds(LIVE_FIELD_UPDATES), //TD18018
		  m_dateTimeChanged(false), //TD18018
		  m_maxTrendsDisplayed(MAX_ITEMS_DEFAULT) //TD17529
    {
        // Initialise Trend Properties member variable.
        m_trendProperties.name = "";
        m_trendProperties.description = "";
        m_trendProperties.graphType = LINE;
        m_trendProperties.endDateTime = CTime::GetCurrentTime().GetTime();
        m_trendProperties.startDateTime = m_trendProperties.endDateTime - 3600;
        m_trendProperties.samplePeriod = DEFAULT_SAMPLE_PERIOD; //TD17529
        m_trendProperties.live = 1;
        m_trendProperties.items.clear();
        m_trendProperties.extraInfo = "";
        m_trendProperties.lastModified = 0;
        m_trendProperties.modificationsUnsaved = false;
        m_trendProperties.isNew = true;

        std::ostringstream str;
        str << DEFAULT_OFFSET_VALUE;
        m_defaultOffsetValueStr = str.str().c_str();

        //{{AFX_DATA_INIT(CTrendPropertiesDialog)
    m_name = _T("");
    m_description = _T("");
    m_isNowChecked = TRUE;
    m_offsetEdit = _T(m_defaultOffsetValueStr);
    //}}AFX_DATA_INIT

        
    }


    void CTrendPropertiesDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CTrendPropertiesDialog)
    DDX_Control(pDX, IDC_OFFSET_COMBO, m_offsetCombo);
    DDX_Control(pDX, IDC_SAMPLE_COMBO, m_samplePeriodCtrl);
    DDX_Control(pDX, IDC_TYPE_COMBO, m_graphTypeCtrl);
    DDX_Control(pDX, IDC_STARTDATE, m_startDateCtrl);
	DDX_Control(pDX, IDC_ENDDATE, m_endDateCtrl);
    DDX_Text(pDX, IDC_NAME_EDIT, m_name);
	DDV_MaxChars(pDX, m_name, 30);
    DDX_Text(pDX, IDC_DESC_EDIT, m_description);
	DDV_MaxChars(pDX, m_description, 100);
    DDX_Check(pDX, IDC_NOW_CHECK, m_isNowChecked);
    DDX_Text(pDX, IDC_OFFSET_EDIT, m_offsetEdit);
    DDV_MaxChars(pDX, m_offsetEdit, 3);
	//}}AFX_DATA_MAP

    }


    BEGIN_MESSAGE_MAP(CTrendPropertiesDialog, CDialog)
        //{{AFX_MSG_MAP(CTrendPropertiesDialog)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_NOW_CHECK, OnNowCheck)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON1, OnColourButton1)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON2, OnColourButton2)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON3, OnColourButton3)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON4, OnColourButton4)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON5, OnColourButton5)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON6, OnColourButton6)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON7, OnColourButton7)
    ON_BN_CLICKED(IDC_COLOUR_BUTTON8, OnColourButton8)
    ON_BN_CLICKED(IDC_ITEM_BUTTON1, OnItemButton1)
    ON_BN_CLICKED(IDC_ITEM_BUTTON2, OnItemButton2)
    ON_BN_CLICKED(IDC_ITEM_BUTTON3, OnItemButton3)
    ON_BN_CLICKED(IDC_ITEM_BUTTON4, OnItemButton4)
    ON_BN_CLICKED(IDC_ITEM_BUTTON5, OnItemButton5)
    ON_BN_CLICKED(IDC_ITEM_BUTTON6, OnItemButton6)
    ON_BN_CLICKED(IDC_ITEM_BUTTON7, OnItemButton7)
    ON_BN_CLICKED(IDC_ITEM_BUTTON8, OnItemButton8)
    ON_WM_CTLCOLOR()
    ON_CBN_SELCHANGE(IDC_SAMPLE_COMBO, OnSelchangeSampleCombo)
    ON_NOTIFY(DTN_DATETIMECHANGE, ID_START_TIME, OnStartTimeChange)
    ON_NOTIFY(DTN_DATETIMECHANGE, ID_END_TIME, OnEndTimeChange)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTDATE, OnStartDateChange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_ENDDATE, OnEndDateChange)
	ON_NOTIFY(DTN_CLOSEUP, IDC_STARTDATE, OnDateChange)     //TD18018
	ON_NOTIFY(DTN_CLOSEUP, IDC_ENDDATE, OnDateChange)  //TD18018
    ON_CBN_SELCHANGE(IDC_TYPE_COMBO, OnSelchangeTypeCombo)
    ON_CBN_SELCHANGE(IDC_OFFSET_COMBO, OnSelchangeOffsetCombo)
    ON_EN_CHANGE(IDC_OFFSET_EDIT, OnChangeOffsetEdit)
    ON_EN_UPDATE(IDC_DISPLAY1, OnDisplayNameChange1)
    ON_EN_UPDATE(IDC_DISPLAY2, OnDisplayNameChange2)
    ON_EN_UPDATE(IDC_DISPLAY3, OnDisplayNameChange3)
    ON_EN_UPDATE(IDC_DISPLAY4, OnDisplayNameChange4)
    ON_EN_UPDATE(IDC_DISPLAY5, OnDisplayNameChange5)
    ON_EN_UPDATE(IDC_DISPLAY6, OnDisplayNameChange6)
    ON_EN_UPDATE(IDC_DISPLAY7, OnDisplayNameChange7)
    ON_EN_UPDATE(IDC_DISPLAY8, OnDisplayNameChange8)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_EN_CHANGE(IDC_DESC_EDIT, OnChangeDescEdit)
    ON_MESSAGE(WM_LARGE_RANGE, OnLargeRange)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTrendPropertiesDialog message handlers

 
    //
    // OnInitDialog
    //
    BOOL CTrendPropertiesDialog::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");

        CDialog::OnInitDialog();

        initialiseColourList();
        initialiseItemList();
        setDateTimePickers();
        initialiseComboBoxes();
        loadTrendProperties();

        m_initialising = false;

        m_startTimeCtrl.SetFont(GetFont());
        m_endTimeCtrl.SetFont(GetFont());

        if(m_disableNameField)
        {
            TA_ASSERT(GetDlgItem(IDC_NAME_EDIT) != NULL, "Name edit box does not exist.");

            bool enableNameField = false;
            if(m_trendProperties.modificationsUnsaved && m_trendProperties.isNew)
            {
                enableNameField = true;
            }

            GetDlgItem(IDC_NAME_EDIT)->EnableWindow(enableNameField);
        }
       
        bool enableSave = m_trendProperties.modificationsUnsaved;
        GetDlgItem(IDC_SAVE)->EnableWindow(enableSave);

        // Display a warning if the existing trend has been deleted
        if(!m_trendProperties.isNew)
        {
            try
            {
                TrendPropertiesStore trendStore;
                trendStore.getTrendProperties(m_trendProperties.name);
            }
            catch(const TrendViewerException& ex)
            {
                if(ex.getType() == TrendViewerException::TEMPLATE_NOT_EXIST)
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << m_trendProperties.name;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_120038);

                    // Since the trend has been deleted, this is a new trend. Enable the name field.
                    GetDlgItem(IDC_NAME_EDIT)->EnableWindow(true);
                    m_trendProperties.isNew = true;
                    m_trendProperties.modificationsUnsaved = true;
                }
            }
            catch(...)
            {
                // Doesn't matter. We're just checking to see if the trend has
                // been deleted here.
            }
        }

        FUNCTION_EXIT;
        return TRUE;
    }

	//TD17529++
	void CTrendPropertiesDialog::setMaxTrendsToDisplay(int newMaxTrendsDisplayed) 
	{
		m_maxTrendsDisplayed = newMaxTrendsDisplayed;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Max Trends To Display updated to: %d", m_maxTrendsDisplayed);
	}
	//++TD17529

    //
    // OnOK
    //
    void CTrendPropertiesDialog::OnOK()
    {
        FUNCTION_ENTRY("OnOK");

        UpdateData(true);
		
        updateCurrentTrendTemplate();

        if(hasCorrectItemCountIfXY() == false)
        {
            FUNCTION_EXIT;
            return;
        }

        if(m_rangeTooLarge)
        {
            // Range too large error
            CString ActionName("display/save trend"); //TD17777
            CString Reason("range is too large. Please select again."); //TD18018
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ActionName << Reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

			// setLiveMode(true); //TD17777
			revertSavedTimeProperties(); //TD18018

            FUNCTION_EXIT;
            return;
        }

        // Launch a warning dialog if archive data needs to be restored to display all points
        long rangeSeconds = m_trendProperties.endDateTime - m_trendProperties.startDateTime;
        checkArchivedDataNeeded(m_trendProperties.startDateTime, rangeSeconds);	
 
		CDialog::OnOK();
        FUNCTION_EXIT;
    }
 
 
    //
    // OnSave
    //
    void CTrendPropertiesDialog::OnSave() 
    {
	    FUNCTION_ENTRY("OnSave");

        updateCurrentTrendTemplate();

        if(m_trendProperties.name.empty())
        {
            std::string attribute("Name");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << attribute;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120002);
            return;
        }

        // Don't allow save if there aren't enough items for XY graph
        if(hasCorrectItemCountIfXY() == false)
        {
            FUNCTION_EXIT;
            return;
        }

        if(m_rangeTooLarge)
        {
            // Range too large error
            CString ActionName("display/save trend"); //TD17777
            CString Reason("range is too large. Please select again."); //TD18018
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ActionName << Reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

			// setLiveMode(true); //TD17777
			revertSavedTimeProperties(); //TD18018
			
            FUNCTION_EXIT;
            return;
        }

        time_t lastModified = m_trendProperties.lastModified;

        TrendPropertiesStore trendStore;
        TrendProperties trendProperties;
        try
        {
            trendProperties = trendStore.getTrendProperties(m_trendProperties.name);

            if(!m_disableNameField)
            {
                // If the name field is not disabled, then this is a new trend. This means that
                // the name of this new trend matches the name of an existing trend. This
                // is not allowed.

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_trendProperties.name;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120003);

                FUNCTION_EXIT;
                return;
            }
            
            // Is an existing one
            if(modifyExistingTemplate(lastModified))
            {
                // Retrieve the latest modification time in case the same template is
                // modified again.
                retrieveLastModifiedTime();
                modificationsSaved();
            }

            FUNCTION_EXIT;
            return;
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_NOT_EXIST)
            {
                // This is a new one
                if(addNewTemplate())
                {
                    // Retrieve the latest modification time in case the same template is
                    // modified again.
                    retrieveLastModifiedTime();
                    modificationsSaved();
                }

                // Because this is an existing trend template, we want to disable the name field so
                // that it cannot be modified. The name is what we use to reference the trend in the
                // TREND_TEMPLATE table, so this needs to remain static.
                m_disableNameField = true;
                TA_ASSERT(GetDlgItem(IDC_NAME_EDIT) != NULL, "Name edit box does not exist.");
                GetDlgItem(IDC_NAME_EDIT)->EnableWindow(false);

                FUNCTION_EXIT;
                return;
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
        }

        std::string action("save");
        std::string reason("an unexpected error occurred.");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << action << reason;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

	    FUNCTION_EXIT;
    }


    //
    // OnDestroy
    //
    void CTrendPropertiesDialog::OnDestroy() 
    {
        CDialog::OnDestroy();
    }


    //
    // OnNowCheck
    //
    void CTrendPropertiesDialog::OnNowCheck() 
    {
        FUNCTION_ENTRY("OnNowCheck");

        UpdateData(true);   
        
        if(m_isNowChecked)
        {
            m_startTimeCtrl.EnableWindow(false);
            m_startDateCtrl.EnableWindow(false);
            m_endTimeCtrl.EnableWindow(false);
            m_endDateCtrl.EnableWindow(false);			
		}
        else
        {
            m_startTimeCtrl.EnableWindow(true);
            m_startDateCtrl.EnableWindow(true);
            m_endTimeCtrl.EnableWindow(true);
            m_endDateCtrl.EnableWindow(true);
			//TD17773++
			if (m_samplePeriodCtrl.GetCurSel() == 0)
			{
				m_samplePeriodCtrl.SetCurSel(1); 
			}
			//++TD17773			
		}

        modificationsMade();

        FUNCTION_EXIT;
    }


    //
    // OnSelchangeSampleCombo
    //
    void CTrendPropertiesDialog::OnSelchangeSampleCombo() 
    {
        // If '0' is selected, automatically set Live mode
        int index = m_samplePeriodCtrl.GetCurSel();
        if(index == 0)
        {
            if(m_isNowChecked == false)
            {
                setLiveMode(true);
            }

            if(m_graphTypeCtrl.GetCurSel() == BAR)
            {
                m_changeSamplePeriod = true;
                m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(index);
            }
        }
        else
        {
            if(m_graphTypeCtrl.GetCurSel() == BAR)
            {
                m_trendProperties.extraInfo = "";
                m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(index);
                m_changeSamplePeriod = false;
            }
        }
            
        // The sample period has changed so make sure that the minimum difference allowable
        // between the start and end times is equivalent to the sample period time
        long samplePeriod = m_samplePeriodCtrl.GetItemData(index);

        COleDateTime endTime;
        m_endTimeCtrl.GetTime(endTime);
        COleDateTime endDate;
        m_endDateCtrl.GetTime(endDate);

        COleDateTime endDateTime = getCombinedTime(endDate, endTime);

        COleDateTimeSpan timeDiff(0, 0, 0, samplePeriod);
        COleDateTime startRangeMax(endDateTime - timeDiff);
        
        m_startTimeCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);
        m_startDateCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);
        

        COleDateTime startTime;
        m_startTimeCtrl.GetTime(startTime);
        COleDateTime startDate;
        m_startTimeCtrl.GetTime(startDate);

        COleDateTime startDateTime = getCombinedTime(startDate, startTime);

        COleDateTime endRangeMin(startDateTime + timeDiff);
        
        COleDateTime currentTime = COleDateTime::GetCurrentTime();
        m_endTimeCtrl.SetRange(&endRangeMin, &currentTime);
        m_endDateCtrl.SetRange(&endRangeMin, &currentTime);

        modificationsMade();
    }


    //
    // OnStartTimeChange
    //
    void CTrendPropertiesDialog::OnStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        COleDateTime fromTime;
        m_startTimeCtrl.GetTime(fromTime);
        
        m_previousStartTime = fromTime;
            
        // Synch the date component
        m_startDateCtrl.SetTime(fromTime);

        // Want a minimum of the sample period in between the start and end times
        long samplePeriod = getCurrentSamplePeriod();
        COleDateTimeSpan timeDiff(0, 0, 0, samplePeriod);
        COleDateTime endRangeMin(fromTime + timeDiff);

        // Don't let the end time be later than the current time
        COleDateTime currentTime = COleDateTime::GetCurrentTime();
        m_endTimeCtrl.SetRange(&endRangeMin, &currentTime);
        m_endDateCtrl.SetRange(&endRangeMin, &currentTime);

        try
        {
            // Update the list of sample periods, if necessary
            populateSamplePeriods(TimeFunction::getInstance().ConvertToTimeT(fromTime),
                    TimeFunction::getInstance().ConvertToTimeT(m_previousEndTime), samplePeriod);
            m_rangeTooLarge = false;
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE)
            {
                // There seems to be a bug with the date/time control where if a message box is
                // displayed while it's message is being handled, it seems to trigger a continuous
                // stream of update events. So post a message instead so we can finish handling
                // the update event first and then display the error message.
                PostMessage(WM_LARGE_RANGE, 0, 0);
            }
            else
            {
                // rethrow any unexpected exception
                throw;
            }
        }

        modificationsMade();
    }
       //TD18018
	void CTrendPropertiesDialog::OnDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		//TD18018
		if (m_dateTimeChanged)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			CString msg = "Time range has changed. Sample periods will be updated.";
			userMsg << msg;
			userMsg.showMsgBox(IDS_UW_050018);		
			m_dateTimeChanged = false;
		}		 
	}

    void CTrendPropertiesDialog::OnStartDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("OnStartDateChange");

        COleDateTime fromDate;
        m_startDateCtrl.GetTime(fromDate);

        if(m_previousStartTime == fromDate)
        {
            // When selecting from the Month Calandar control, two events are generated.
            // We only need to process one of these as the time change is exactly the
            // same for both events.
            FUNCTION_EXIT;
            return;
        }

        m_previousStartTime = fromDate;
        
        // Synch the time component
        m_startTimeCtrl.SetTime(fromDate);

        // Want a minimum of the sample period in between the start and end times
        long samplePeriod = getCurrentSamplePeriod();
        COleDateTimeSpan timeDiff(0, 0, 0, samplePeriod);
        COleDateTime endRangeMin(fromDate + timeDiff);

        // Reset the ranges for the start date/time
        COleDateTime currentTime = COleDateTime::GetCurrentTime();
        m_endDateCtrl.SetRange(&endRangeMin, &currentTime);
        m_endTimeCtrl.SetRange(&endRangeMin, &currentTime);

        try
        {
            // Update the list of sample periods, if necessary
            populateSamplePeriods(TimeFunction::getInstance().ConvertToTimeT(fromDate),
                    TimeFunction::getInstance().ConvertToTimeT(m_previousEndTime), samplePeriod);
            m_rangeTooLarge = false;
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE)
            {
                // Hide the calandar control if it's displayed
                CMonthCalCtrl* pMoCalCtrl = m_startDateCtrl.GetMonthCalCtrl();
                if(pMoCalCtrl != NULL)
                {
                    pMoCalCtrl->ShowWindow(SW_HIDE);
                }

                // There seems to be a bug with the date/time control where if a message box is
                // displayed while it's message is being handled, it seems to trigger a continuous
                // stream of update events. So post a message instead so we can finish handling
                // the update event first and then display the error message.
                PostMessage(WM_LARGE_RANGE, 0, 0);
            }
            else
            {
                // rethrow any unexpected exception
                throw;
            }
        }

        modificationsMade();

	    *pResult = 0;

        FUNCTION_EXIT;
    }

    //
    // OnEndTimeChange
    //
    void CTrendPropertiesDialog::OnEndTimeChange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        COleDateTime toTime;
        m_endTimeCtrl.GetTime(toTime);

        m_previousEndTime = toTime;
        
        // Synch the date component
        m_endDateCtrl.SetTime(toTime);

        // Want a minimum of the sample period in between the start and end times
        long samplePeriod = getCurrentSamplePeriod();
        COleDateTimeSpan timeDiff(0, 0, 0, samplePeriod);
        COleDateTime startRangeMax(toTime - timeDiff);

        // Set the ranges for the start date/time 
        m_startTimeCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);
        m_startDateCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);

        try
        {
            // Update the list of sample periods, if necessary
            populateSamplePeriods(TimeFunction::getInstance().ConvertToTimeT(m_previousStartTime),
                    TimeFunction::getInstance().ConvertToTimeT(toTime), samplePeriod);
            m_rangeTooLarge = false;
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE)
            {
                // There seems to be a bug with the date/time control where if a message box is
                // displayed while it's message is being handled, it seems to trigger a continuous
                // stream of update events. So post a message instead so we can finish handling
                // the update event first and then display the error message.
                PostMessage(WM_LARGE_RANGE, 0, 0);
            }
            else
            {
                // rethrow any unexpected exception
                throw;
            }
        }

        modificationsMade();
    }


    void CTrendPropertiesDialog::OnEndDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("OnEndDateChange");

        COleDateTime toDate;
        m_endDateCtrl.GetTime(toDate);

        if(m_previousEndTime == toDate)
        {
            // When selecting from the Month Calandar control, two events are generated.
            // We only need to process one of these as the time change is exactly the
            // same for both events.
            FUNCTION_EXIT;
            return;
        }

        m_previousEndTime = toDate;
            
        // Synch the time component
        m_endTimeCtrl.SetTime(toDate);

        // Want a minimum of the sample period in between the start and end times
        long samplePeriod = getCurrentSamplePeriod();
        COleDateTimeSpan timeDiff(0, 0, 0, samplePeriod);
        COleDateTime startRangeMax(toDate - timeDiff);

        // Set the ranges for the date/time
        m_startDateCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);
        m_startTimeCtrl.SetRange(&EARLIEST_START_TIME, &startRangeMax);

        try
        {
            // Update the list of sample periods, if necessary
            populateSamplePeriods(TimeFunction::getInstance().ConvertToTimeT(m_previousStartTime),
                    TimeFunction::getInstance().ConvertToTimeT(toDate), samplePeriod);
            m_rangeTooLarge = false;
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE)
            {
                // Hide the calandar control if it's displayed
                CMonthCalCtrl* pMoCalCtrl = m_endDateCtrl.GetMonthCalCtrl();
                if(pMoCalCtrl != NULL)
                {
                    pMoCalCtrl->ShowWindow(SW_HIDE);
                }

                // There seems to be a bug with the date/time control where if a message box is
                // displayed while it's message is being handled, it seems to trigger a continuous
                // stream of update events. So post a message instead so we can finish handling
                // the update event first and then display the error message.
                PostMessage(WM_LARGE_RANGE, 0, 0);
            }
            else
            {
                // rethrow any unexpected exception
                throw;
            }
        }
        
        modificationsMade();

	    *pResult = 0;

        FUNCTION_EXIT;
    }


    //
    // OnSelchangeTypeCombo
    //
    LRESULT CTrendPropertiesDialog::OnLargeRange(WPARAM wParam, LPARAM lParam)
    {
		//TD18018 - Instead of immediately changing to Live Mode,
		// the time range must return back to the last previously saved time range		
        // m_samplePeriodCtrl.SetCurSel(0);
        // setLiveMode(true);
        m_rangeTooLarge = true;        
        //Should display an error message here that the range is too large
		// must reselect a new time range		
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120042);
		// then revert back to the saved time range and sample period properties
		revertSavedTimeProperties();
		//++TD18018
        return 0;
    }

	void CTrendPropertiesDialog::revertSavedTimeProperties()
	{
		//TD18018 - Instead of immediately changing to Live Mode,
		// the time range must revert back to the saved time range and sample period properties
		long rangeSeconds = m_trendProperties.endDateTime - m_trendProperties.startDateTime;
        populateSampleComboBox(rangeSeconds);     

		int index = 0;
		m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(0);
		if (m_trendProperties.samplePeriod == LIVE_FIELD_UPDATES)
		{
			index = 1;
			m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(1);
		}
		m_samplePeriodCtrl.SetCurSel(index);

		// Set the date/time ranges - This is done after the sample period cursor has been set
        // Want the minimum time between the start and end dates as the sample period      
		//
        // revert the End Date/Time
        //
		CTime endTime(m_trendProperties.endDateTime);
        m_endTimeCtrl.SetTime(&endTime);
        m_endDateCtrl.SetTime(&endTime);
        m_previousEndTime = COleDateTime(m_trendProperties.endDateTime);

		//
        // revert the Start Date/Time, but revert the range first 
        //		
        time_t maxTime = endTime.GetTime() - m_trendProperties.samplePeriod;
        COleDateTime endCTime(maxTime);

        m_startTimeCtrl.SetRange(&EARLIEST_START_TIME, &endCTime);
        m_startDateCtrl.SetRange(&EARLIEST_START_TIME, &endCTime);

		CTime startTime(m_trendProperties.startDateTime);
        m_startTimeCtrl.SetTime(&startTime);
        m_startDateCtrl.SetTime(&startTime);
        m_previousStartTime = COleDateTime(m_trendProperties.startDateTime);
		m_dateTimeChanged = false;
		//TD18018
	}

    //
    // OnSelchangeTypeCombo
    //
    void CTrendPropertiesDialog::OnSelchangeTypeCombo() 
    {
        int graphIndex = m_graphTypeCtrl.GetCurSel();
        if(graphIndex == COMPARISON)
        {
             // Enable Offset controls
            GetDlgItem(IDC_OFFSET_EDIT)->EnableWindow(true);
            GetDlgItem(IDC_OFFSET_COMBO)->EnableWindow(true);
        }
        else
        {
            // Disable Offset controls
            GetDlgItem(IDC_OFFSET_EDIT)->EnableWindow(false);
            GetDlgItem(IDC_OFFSET_COMBO)->EnableWindow(false);
        }

		//TD17529++ - reimplemented setControlsForXY() to this method
		setControlsForTrends( static_cast<EGraphType>(graphIndex));
		//++TD17529 - lizettejl

        // Get the current sample period selection
        long currentSamplePeriod = getCurrentSamplePeriod();
        
        CTime startTime;
        m_startTimeCtrl.GetTime(startTime);

        CTime endTime;
        m_endTimeCtrl.GetTime(endTime);

		//TD17529++  catch an exception thrown by this method
        try
        {
            // Update the list of sample periods
			populateSamplePeriods(startTime.GetTime(), endTime.GetTime(), currentSamplePeriod);
            m_rangeTooLarge = false;
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE)
            {
                // There seems to be a bug with the date/time control where if a message box is
                // displayed while it's message is being handled, it seems to trigger a continuous
                // stream of update events. So post a message instead so we can finish handling
                // the update event first and then display the error message.
                PostMessage(WM_LARGE_RANGE, 0, 0);
            }
            else
            {
                // rethrow any unexpected exception
                throw;
            }
        }
		//++TD17529 - lizettejl

        
        modificationsMade();
    }


    //
    // OnSelchangeOffsetCombo
    //
    void CTrendPropertiesDialog::OnSelchangeOffsetCombo() 
    {
        int selIndex = m_offsetCombo.GetCurSel();
        
        if(selIndex == YEARS)
        {
            char text[MAX_BUFFER_SIZE];
            ((CEdit*) (GetDlgItem(IDC_OFFSET_EDIT)))->GetLine(0, text);
            
            m_offsetEdit = text;

            std::ostringstream str;
            str << MAX_OFFSET_YEARS;

            int number = atoi(text);
            if(number > MAX_OFFSET_YEARS)
            {
                GetDlgItem(IDC_OFFSET_EDIT)->SetWindowText(str.str().c_str());
            }

            m_offsetEdit = str.str().c_str();
        }

        modificationsMade();
    }


    //
    // OnChangeOffsetEdit
    //
    void CTrendPropertiesDialog::OnChangeOffsetEdit() 
    {
        static bool valueUndo = false;
        static std::string lastValue(m_defaultOffsetValueStr);

        if(valueUndo == true)
        {
            valueUndo = false;
            return;
        }

        char text[MAX_BUFFER_SIZE];
        ((CEdit*) (GetDlgItem(IDC_OFFSET_EDIT)))->GetLine(0, text);
        m_offsetEdit = text;

        if(std::string(text).find_first_not_of("0123456789") != std::string::npos)
        {
            valueUndo = true;
            GetDlgItem(IDC_OFFSET_EDIT)->SetWindowText(lastValue.c_str());
            m_offsetEdit = lastValue.c_str();
            return;
        }
        
        int selIndex = m_offsetCombo.GetCurSel();
        if(selIndex == YEARS)
        {
            int number = atoi(text);
            if(number > MAX_OFFSET_YEARS)
            {
                valueUndo = true;
                GetDlgItem(IDC_OFFSET_EDIT)->SetWindowText(lastValue.c_str());
                m_offsetEdit = lastValue.c_str();
                return;
            }
        }

        lastValue = text;

        modificationsMade();
    }


    //
    // OnColourButton
    //
    void CTrendPropertiesDialog::OnColourButton1() 
    {
        setNewColour(1);  
		//TD18011
		if (m_graphTypeCtrl.GetCurSel() == XY)
		{
			LOGBRUSH logBrush;
            m_itemColours[1]->GetLogBrush(&logBrush);
			setColourEditBackground(2,logBrush.lbColor); 
		}
		//++TD18011
    }


    void CTrendPropertiesDialog::OnColourButton2() 
    {
        setNewColour(2);  
    }


    void CTrendPropertiesDialog::OnColourButton3() 
    {
        setNewColour(3);  
    }


    void CTrendPropertiesDialog::OnColourButton4() 
    {
        setNewColour(4);  
    }


    void CTrendPropertiesDialog::OnColourButton5() 
    {
        setNewColour(5);  
    }


    void CTrendPropertiesDialog::OnColourButton6() 
    {
        setNewColour(6);  
    }


    void CTrendPropertiesDialog::OnColourButton7() 
    {
        setNewColour(7);  
    }


    void CTrendPropertiesDialog::OnColourButton8() 
    {
        setNewColour(8);  
    }


    //
    // OnItemButton
    //
    void CTrendPropertiesDialog::OnItemButton1() 
    {
        setItem(1);  
    }

    void CTrendPropertiesDialog::OnItemButton2() 
    {
        setItem(2);  
    }

    void CTrendPropertiesDialog::OnItemButton3() 
    {
        setItem(3);  
    }

    void CTrendPropertiesDialog::OnItemButton4() 
    {
        setItem(4);  
    }

    void CTrendPropertiesDialog::OnItemButton5() 
    {
        setItem(5);  
    }

    void CTrendPropertiesDialog::OnItemButton6() 
    {
        setItem(6);  
    }

    void CTrendPropertiesDialog::OnItemButton7() 
    {
        setItem(7);  
    }

    void CTrendPropertiesDialog::OnItemButton8() 
    {
        setItem(8);  
    }

    //
    // OnDisplayNameChange
    //
    void CTrendPropertiesDialog::OnDisplayNameChange1()
    {
        onDisplayNameChange(1);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange2()
    {
        onDisplayNameChange(2);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange3()
    {
        onDisplayNameChange(3);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange4()
    {
        onDisplayNameChange(4);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange5()
    {
        onDisplayNameChange(5);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange6()
    {
        onDisplayNameChange(6);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange7()
    {
        onDisplayNameChange(7);
    }

    void CTrendPropertiesDialog::OnDisplayNameChange8()
    {
        onDisplayNameChange(8);
    }

    void CTrendPropertiesDialog::OnChangeDescEdit() 
    {
	    modificationsMade();
	}

    //
    // OnCtlColor
    //
    HBRUSH CTrendPropertiesDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
    {
        ResourceToBrushMap::iterator iter;

        // This method is called when window controls are being painted.
        switch (nCtlColor)
        {
            // When the Edit fields are being painted
            case CTLCOLOR_EDIT:
                {
                
                    int ctrlId = pWnd->GetDlgCtrlID();

                    // Use a brush from m_itemColours for the Colour edit boxes.
                    std::map<int, int>::iterator it = m_colBoxId.begin();
                    for(; it != m_colBoxId.end(); ++it)
                    {
                        int id = it->second;
                        if(id == ctrlId)
                        {
                            int index = it->first;
                            iter = m_itemColours.find(index);
                            if(iter != m_itemColours.end())
                            {
                                return (HBRUSH)(iter->second->GetSafeHandle());
                            }
                        }
                    }
                }

            // Use the default for everything else.
            default:
                return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
        }

        return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    }


    // setTrendProperties
    //
    void CTrendPropertiesDialog::setTrendProperties(const TrendProperties& trendProperties)
    {
        FUNCTION_ENTRY("setTrendProperties");

        m_trendProperties.name = trendProperties.name;
        m_trendProperties.description = trendProperties.description;
        m_trendProperties.graphType = trendProperties.graphType;
        m_trendProperties.endDateTime = trendProperties.endDateTime;
        m_trendProperties.startDateTime = trendProperties.startDateTime;
        m_trendProperties.samplePeriod = trendProperties.samplePeriod;
        m_trendProperties.live = trendProperties.live;
        m_trendProperties.extraInfo = trendProperties.extraInfo;
        m_trendProperties.lastModified = trendProperties.lastModified;
        m_trendProperties.modificationsUnsaved = trendProperties.modificationsUnsaved;
        m_trendProperties.isNew = trendProperties.isNew;

        m_trendProperties.items.clear();
        m_trendProperties.items = trendProperties.items;

        // Because this is an existing trend template, we want to disable the name field so
        // that it cannot be modified. The name is what we use to reference the trend in the
        // TREND_TEMPLATE table, so this needs to remain static.

        if(!m_trendProperties.isNew)
        {
            m_disableNameField = true;
        }
        
        FUNCTION_EXIT;
    }


    //
    // setDateTimePickers
    //
    void CTrendPropertiesDialog::setDateTimePickers()
    {
        FUNCTION_ENTRY("setDateTimePickers");

        // Retrieve the range time placeholder coordinates
        CRect rangeTimeRect;
        GetDlgItem(IDC_RANGE_PLACEHOLDER)->GetWindowRect(rangeTimeRect);
        ScreenToClient(rangeTimeRect);

        // Retrieve the start date/time placeholder coordinates
        CRect startDateTimeRect;
        GetDlgItem(IDC_DATETIME_PLACEHOLDER)->GetWindowRect(startDateTimeRect);
        ScreenToClient(startDateTimeRect);

        if(!m_startTimeCtrl.Create(WS_VISIBLE | WS_CHILD | DTS_TIMEFORMAT, 
            startDateTimeRect, this, ID_START_TIME))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to create range time control.");
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DIALOG_INIT_ERROR,
                "Error occured creating time controls."));
        }
        m_startTimeCtrl.ShowWindow(SW_SHOW);
        m_startTimeCtrl.SetFormat( "HH:mm" );


        // Retrieve the end date/time placeholder coordinates
        CRect endDateTimeRect;
        GetDlgItem(IDC_END_DATETIME_PLACEHOLDER)->GetWindowRect(endDateTimeRect);
        ScreenToClient(endDateTimeRect);

        if(!m_endTimeCtrl.Create(WS_VISIBLE | WS_CHILD | DTS_TIMEFORMAT, 
            endDateTimeRect, this, ID_END_TIME))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to create range time control.");
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DIALOG_INIT_ERROR,
                "Error occured creating time controls."));
        }
        m_endTimeCtrl.ShowWindow(SW_SHOW);
        m_endTimeCtrl.SetFormat( "HH:mm" );

        FUNCTION_EXIT;
    }


    //
    // loadTrendProperties
    //
    void CTrendPropertiesDialog::loadTrendProperties()
    {
        FUNCTION_ENTRY("loadTrendProperties");

        //
        // Name
        //
        m_name = m_trendProperties.name.c_str();
        
        //
        // Description
        //
        m_description = m_trendProperties.description.c_str();
        
        //
        // Graph Type
        //
        m_graphTypeCtrl.SetCurSel(m_trendProperties.graphType);
        
        // If the graph is a BAR graph, this will adjust the sample period to
        // 0 if it is live. This is because the sample period could have been 
        // changed to make the graph less cluttered, but when the property dialog
        // is displayed, we want the sample period to be displayed as 0.
        adjustSamplePeriod();
        

        // Set the Date/Times - the ranges for these controls will be set after the
        // sample periods have been set

        //
        // Start Date/Time
        //
        CTime startTime(m_trendProperties.startDateTime);
        m_startTimeCtrl.SetTime(&startTime);
        m_startDateCtrl.SetTime(&startTime);
        m_previousStartTime = COleDateTime(m_trendProperties.startDateTime);
        
        //
        // End Date/Time
        //
        CTime endTime(m_trendProperties.endDateTime);
        m_endTimeCtrl.SetTime(&endTime);
        m_endDateCtrl.SetTime(&endTime);
        m_previousEndTime = COleDateTime(m_trendProperties.endDateTime);
        
        
        //
        // Live
        //
        m_isNowChecked = m_trendProperties.live;
        
        //
        // Items
        //

        // Clear the item names
        for(int j = 0; j < MAX_ITEMS_DEFAULT; ++j)
        {
			GetDlgItem(m_itemNamesId[j+1])->SetWindowText(EMPTY_STRING);
        }
		
        // Insert the new item names
        std::map<std::string, TA_Base_Core::ItemInfo>::iterator it = m_trendProperties.items.begin(); 
        for(; it != m_trendProperties.items.end(); ++it)		
        {
            int index = it->second.index; 
            COLORREF colour = Utilities::convertHexStringToColorRef(it->second.colour);
            std::string name = it->first;
            std::string displayName = it->second.displayName;

            GetDlgItem(m_itemNamesId[index])->SetWindowText(name.c_str());
            GetDlgItem(m_displayNamesId[index])->SetWindowText(displayName.c_str());
            setColourEditBackground(index, colour);
        }

        //
        // Extra Info - eg. Offset 
        //
        setExtraInfo(m_trendProperties.graphType);
        
        UpdateData(false);

        // Reset the sample period combo box if necessary. Sample periods allowed can be limited by
        // the range, number of items selected and graph type.
        populateSamplePeriods(m_trendProperties.startDateTime, m_trendProperties.endDateTime, m_trendProperties.samplePeriod, true);

        // Set the date/time ranges - This is done after the sample period cursor has been set
        // Want the minimum time between the start and end dates as the sample period
        int index = m_samplePeriodCtrl.GetCurSel();
        long samplePeriod = m_samplePeriodCtrl.GetItemData(index);

        time_t minTime = startTime.GetTime() + samplePeriod;
        COleDateTime startCTime(minTime);

        COleDateTime currentTime = COleDateTime::GetCurrentTime();
        m_endTimeCtrl.SetRange(&startCTime, &currentTime);
        m_endDateCtrl.SetRange(&startCTime, &currentTime);

        time_t maxTime = endTime.GetTime() - samplePeriod;
        COleDateTime endCTime(maxTime);

        m_startTimeCtrl.SetRange(&EARLIEST_START_TIME, &endCTime);
        m_startDateCtrl.SetRange(&EARLIEST_START_TIME, &endCTime);
        
        // Enable/Disable date/time control depending on the live state
        OnNowCheck();
        OnSelchangeSampleCombo();

        //TD17529++ - reimplemented setControlsForXY() to this method
		setControlsForTrends(m_trendProperties.graphType);
		//++TD17529 - lizettejl

        FUNCTION_EXIT;
    }


    //
    // initialiseItemPointers
    //
    void CTrendPropertiesDialog::initialiseItemList()
    {
        FUNCTION_ENTRY("initialiseItemList");

        // Item Names

        m_itemNamesId.clear();

        m_itemNamesId[1] = static_cast <int> (IDC_ITEM1);
        m_itemNamesId[2] = static_cast <int> (IDC_ITEM2);
        m_itemNamesId[3] = static_cast <int> (IDC_ITEM3);
        m_itemNamesId[4] = static_cast <int> (IDC_ITEM4);
        m_itemNamesId[5] = static_cast <int> (IDC_ITEM5);
        m_itemNamesId[6] = static_cast <int> (IDC_ITEM6);
        m_itemNamesId[7] = static_cast <int> (IDC_ITEM7);
        m_itemNamesId[8] = static_cast <int> (IDC_ITEM8);

        // Display Names

        m_displayNamesId.clear();

        m_displayNamesId[1] = static_cast <int> (IDC_DISPLAY1);
        m_displayNamesId[2] = static_cast <int> (IDC_DISPLAY2);
        m_displayNamesId[3] = static_cast <int> (IDC_DISPLAY3);
        m_displayNamesId[4] = static_cast <int> (IDC_DISPLAY4);
        m_displayNamesId[5] = static_cast <int> (IDC_DISPLAY5);
        m_displayNamesId[6] = static_cast <int> (IDC_DISPLAY6);
        m_displayNamesId[7] = static_cast <int> (IDC_DISPLAY7);
        m_displayNamesId[8] = static_cast <int> (IDC_DISPLAY8);

        for(std::map<int, int>::iterator iter = m_displayNamesId.begin();
            iter != m_displayNamesId.end();
            ++iter)
        {
            ((CEdit*) GetDlgItem(iter->second))->SetLimitText(MAX_DISPLAY_NAME_LENGTH);
        }

		//TD17529++ 
		// static identifiers
		m_staticItemsId.clear();
        m_staticItemsId[1] = static_cast <int> (IDC_ITEM_STATIC1);
        m_staticItemsId[2] = static_cast <int> (IDC_ITEM_STATIC2);
        m_staticItemsId[3] = static_cast <int> (IDC_ITEM_STATIC3);
        m_staticItemsId[4] = static_cast <int> (IDC_ITEM_STATIC4);
        m_staticItemsId[5] = static_cast <int> (IDC_ITEM_STATIC5);
        m_staticItemsId[6] = static_cast <int> (IDC_ITEM_STATIC6);
        m_staticItemsId[7] = static_cast <int> (IDC_ITEM_STATIC7);
        m_staticItemsId[8] = static_cast <int> (IDC_ITEM_STATIC8);

		// item selector buttons
		m_itemButtonsId.clear();
        m_itemButtonsId[1] = static_cast <int> (IDC_ITEM_BUTTON1);
        m_itemButtonsId[2] = static_cast <int> (IDC_ITEM_BUTTON2);
        m_itemButtonsId[3] = static_cast <int> (IDC_ITEM_BUTTON3);
        m_itemButtonsId[4] = static_cast <int> (IDC_ITEM_BUTTON4);
        m_itemButtonsId[5] = static_cast <int> (IDC_ITEM_BUTTON5);
        m_itemButtonsId[6] = static_cast <int> (IDC_ITEM_BUTTON6);
        m_itemButtonsId[7] = static_cast <int> (IDC_ITEM_BUTTON7);
        m_itemButtonsId[8] = static_cast <int> (IDC_ITEM_BUTTON8);
		//++TD17529


        FUNCTION_EXIT;
    }

    
    //
    // initialiseComboBoxes
    //
    void CTrendPropertiesDialog::initialiseComboBoxes()
    {
        FUNCTION_ENTRY("initialiseComboBoxes");

        // Initialise graph type combo box
        m_graphTypeCtrl.InsertString(LINE, "LINE");
        m_graphTypeCtrl.InsertString(BAR, "BAR");
        m_graphTypeCtrl.InsertString(COMPARISON, "COMPARISON");
        m_graphTypeCtrl.InsertString(XY, "XY");
        m_graphTypeCtrl.SetCurSel(LINE);

        // Initialise sample period combo box
        populateSampleComboBox(m_trendProperties.endDateTime - m_trendProperties.startDateTime);
        
        // Initialise offset combo box
        for(int i = 0; i < NUM_OFFSET_UNITS; ++i)
        {
            m_offsetCombo.InsertString(i, OFFSET_UNITS[i].c_str());
        }
        m_offsetCombo.SetCurSel(HOURS);
        GetDlgItem(IDC_OFFSET_EDIT)->SetWindowText(m_defaultOffsetValueStr);
        m_offsetEdit = m_defaultOffsetValueStr;

        if(m_trendProperties.graphType == COMPARISON)
        {
            // Enable Offset controls
            GetDlgItem(IDC_OFFSET_EDIT)->EnableWindow(true);
            GetDlgItem(IDC_OFFSET_COMBO)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_OFFSET_EDIT)->EnableWindow(false);
            GetDlgItem(IDC_OFFSET_COMBO)->EnableWindow(false);
        }

        FUNCTION_EXIT;
    }

    
    //
    // populateSampleComboBox
    //
    void CTrendPropertiesDialog::populateSampleComboBox(long rangeSeconds)
    {
        FUNCTION_ENTRY("populateSampleComboBox");

        m_changeSamplePeriod = false;

        //
        // Get the graph type selected - For a BAR graph, the sample periods available
        // will be dependent on the number of items that have been selected and the 
        // range specified. For a BAR graph, we don't want to plot more than 120 points
        // else the graph looks too cluttered.
        // For all other graphs, the sample periods avaiable are dependent only on the
        // range - max points is set at 1000.
        //
        // TD17718 - use the configured max points in DB 
		// int maxPoints = MAX_POINTS;
		int maxPoints = 0;
        if(m_graphTypeCtrl.GetCurSel() == BAR)
        {
            // Determine how many items have been selected
            std::map<std::string, TA_Base_Core::ItemInfo> selectedItems;
            populateTrendPropertyItem(selectedItems, BAR);
            
            int totalSelectedItems = 1;
            std::map<std::string, TA_Base_Core::ItemInfo>::iterator it = selectedItems.find(EMPTY_STRING);
            if(it != selectedItems.end() && selectedItems.size() != 1)
            {
                totalSelectedItems = selectedItems.size() - 1;
            }
            else
            {
                totalSelectedItems = selectedItems.size();
            }
            
            TA_ASSERT(totalSelectedItems != 0, "Divide by 0");

			// TD17718 - use the configured max points in DB 
            //maxPoints = MAX_POINTS_BAR / totalSelectedItems;
			maxPoints = Utilities::getMaxPointsToDisplay(true) / totalSelectedItems;
        }
		else
		{
			maxPoints = Utilities::getMaxPointsToDisplay();
		}
		//++TD17718
        
        m_samplePeriodCtrl.ResetContent();

        // Initialise sample period combo box
        m_samplePeriodCtrl.InsertString(0, "REAL");
        m_samplePeriodCtrl.SetItemData(0, 0);

        std::map<unsigned long, std::string> samplePeriods = TrendConfigDatabaseAccess::getInstance().getSamplePeriods();
        std::map<unsigned long, std::string>::iterator it;
        int index = 1;
        for(int i = 0; i < NUM_SAMPLE_RATES; ++i)
        {
            it = samplePeriods.find(SAMPLE_RATES[i]);

            if(it != samplePeriods.end())
            {
                // Only allow the sample rate to be available if there will be
                // 1000 records or less
                if(rangeSeconds / it->first <= static_cast<unsigned int>(maxPoints))
                {
                    m_samplePeriodCtrl.InsertString(index, it->second.c_str());
                    m_samplePeriodCtrl.SetItemData(index, it->first);
                    ++index;
                }
            }
        }

        int j = 0;
        for(; j < m_samplePeriodCtrl.GetCount(); ++j)
        {
            if(m_samplePeriodCtrl.GetItemData(j) == m_trendProperties.samplePeriod)
            {
                //m_samplePeriodCtrl.SetCurSel(j);

                if(m_trendProperties.samplePeriod == 0)
                {
                    m_changeSamplePeriod = true;
                }

                break;
            }
        }

        FUNCTION_EXIT;
    }


    //
    // initialiseColourList
    //
    void CTrendPropertiesDialog::initialiseColourList()
    {
        FUNCTION_ENTRY("initialiseColourList");

        // Colour edit boxes
        m_colBoxId.clear();

        m_colBoxId[1] = static_cast <int> (IDC_COLOUR1);
        m_colBoxId[2] = static_cast <int> (IDC_COLOUR2);
        m_colBoxId[3] = static_cast <int> (IDC_COLOUR3);
        m_colBoxId[4] = static_cast <int> (IDC_COLOUR4);
        m_colBoxId[5] = static_cast <int> (IDC_COLOUR5);
        m_colBoxId[6] = static_cast <int> (IDC_COLOUR6);
        m_colBoxId[7] = static_cast <int> (IDC_COLOUR7);
        m_colBoxId[8] = static_cast <int> (IDC_COLOUR8);

        m_itemColours.clear();
        for(int i = 0; i < NUM_COLOURS; ++i)
        {
            setColourEditBackground(i+1, DEFAULT_COLOURS[i]); 
        }
       
		//TD17529 - colour selector buttons
		m_colButtonId.clear();
        m_colButtonId[1] = static_cast <int> (IDC_COLOUR_BUTTON1);
        m_colButtonId[2] = static_cast <int> (IDC_COLOUR_BUTTON2);
        m_colButtonId[3] = static_cast <int> (IDC_COLOUR_BUTTON3);
        m_colButtonId[4] = static_cast <int> (IDC_COLOUR_BUTTON4);
        m_colButtonId[5] = static_cast <int> (IDC_COLOUR_BUTTON5);
        m_colButtonId[6] = static_cast <int> (IDC_COLOUR_BUTTON6);
        m_colButtonId[7] = static_cast <int> (IDC_COLOUR_BUTTON7);
        m_colButtonId[8] = static_cast <int> (IDC_COLOUR_BUTTON8);
		//++TD17529
		
        FUNCTION_EXIT;
    }


    //
    // setColourEditBackground
    //
    void CTrendPropertiesDialog::setColourEditBackground(int index, COLORREF colour)
    {
        FUNCTION_ENTRY("setColourEditBackground");

        std::map<int, int>::iterator idIt = m_colBoxId.find(index);
        if(idIt != m_colBoxId.end())
        {
            // Hide the edit, set its background color, then show it again.
            GetDlgItem(m_colBoxId[index])->ShowWindow(false);

            m_itemColours[index] = boost::shared_ptr<CBrush> (new CBrush(colour));

            GetDlgItem(m_colBoxId[index])->ShowWindow(true);
        }
        else
        {
            // A resource Id has not been set for this index. This is a programatic error.
            std::ostringstream msg;
            msg << "ResourceId not set for index: " << index;
            TA_ASSERT(false, msg.str().c_str());
        }

        FUNCTION_EXIT;
    }


    //
    // setNewColour
    //
    void CTrendPropertiesDialog::setNewColour(int index)
    {
        FUNCTION_ENTRY("setNewColour");

        CColorDialog dialog;
        if(dialog.DoModal() == IDOK)
        {
            COLORREF colour = dialog.GetColor();
            setColourEditBackground(index, colour);
            modificationsMade();
        }

        FUNCTION_EXIT;
    }


    //
    // setItem
    //
    void CTrendPropertiesDialog::setItem(int index)
    {
        FUNCTION_ENTRY("setItem");

        char buffer[MAX_BUFFER_SIZE];
        ((CEdit*) (GetDlgItem(m_itemNamesId[index])))->GetLine(0, buffer);

		std::map<std::string, TA_Base_Core::ItemInfo> selectedItems;
        populateTrendPropertyItem(selectedItems, static_cast<EGraphType> (m_graphTypeCtrl.GetCurSel()));

		//TD17631++
		//CItemSelectDialog addDialog(buffer, selectedItems, m_itemStore);
		CDataPointSelectDlg addDialog(buffer, selectedItems, m_itemStore);
		//++TD17631
        if(addDialog.DoModal() == IDOK)
        {
            std::string itemName = addDialog.getItemName();
            GetDlgItem(m_itemNamesId[index])->SetWindowText(itemName.c_str());

            // If an item has been selected, set the display name
            if(itemName != EMPTY_STRING)
            {
                // If the display name hasn't been set, set it as the first part of the item name
                char buffer[MAX_BUFFER_SIZE];
                ((CEdit*) (GetDlgItem(m_displayNamesId[index])))->GetLine(0, buffer);
                std::string displayName(buffer);
            
                if(displayName.empty())
                {
                    if(itemName.size() > MAX_DISPLAY_NAME_LENGTH)
                    {
                        itemName = itemName.substr(0, MAX_DISPLAY_NAME_LENGTH);
                    }
 
                    GetDlgItem(m_displayNamesId[index])->SetWindowText(itemName.c_str());
                }
                else        // TD: 8498 (220205) GT - Display Name now updates when new item selected.
                {
                    GetDlgItem(m_displayNamesId[index])->SetWindowText("");

                    if(itemName.size() > MAX_DISPLAY_NAME_LENGTH)
                    {
                        itemName = itemName.substr(0, MAX_DISPLAY_NAME_LENGTH);
                    }
 
                    GetDlgItem(m_displayNamesId[index])->SetWindowText(itemName.c_str());
                }           
            }
            else
            {
                // Clear the display name
                GetDlgItem(m_displayNamesId[index])->SetWindowText("");
            }
        }

        // If it's a BAR graph, reset the sample periods as these are dependent
        // on the number of items that have been selected
        if(m_graphTypeCtrl.GetCurSel() == BAR)
        {
            long currentSamplePeriod = getCurrentSamplePeriod();
            populateSamplePeriods(m_trendProperties.startDateTime, m_trendProperties.endDateTime, currentSamplePeriod);
        }

        modificationsMade();

        FUNCTION_EXIT;
    }


    //
    // populateTrendPropertyItem
    //
    void CTrendPropertiesDialog::populateTrendPropertyItem(std::map<std::string, TA_Base_Core::ItemInfo>& items, const EGraphType& graphType)
    {
        FUNCTION_ENTRY("populateTrendPropertyItem");

        int numItems = MAX_ITEMS_DEFAULT;
        if(graphType == XY)
        {
            numItems = XY_MAX_ITEMS; //TD17529++ - changed to use a defined constant
        }

        // Load the new items
        items.clear();
        for(int i = 0; i < numItems; ++i)
        {
            // Item Name
            char buffer[MAX_BUFFER_SIZE];			            			
			((CEdit*) (GetDlgItem(m_itemNamesId[i+1])))->GetLine(0, buffer);			
            std::string itemName(buffer);

            TA_Base_Core::ItemInfo itemInfo;

            // Item Display Name
            ((CEdit*) (GetDlgItem(m_displayNamesId[i+1])))->GetLine(0, buffer);
            itemInfo.displayName = buffer;

            // Item Index			
            itemInfo.index = i + 1;

            // Item Colour
            int colourIndex = i + 1;
            if(graphType == XY)
            {
                colourIndex = 1;
            }

            LOGBRUSH logBrush;
            m_itemColours[colourIndex]->GetLogBrush(&logBrush);
            itemInfo.colour = Utilities::createHexStringFromColourRef(logBrush.lbColor);
            
            items[itemName] = itemInfo;
        }

        FUNCTION_EXIT;
    }

    
    //
    // populateExtraInfo
    //
    void CTrendPropertiesDialog::populateExtraInfo(const EGraphType& graphType)
    {
        FUNCTION_ENTRY("populateExtraInfo");

        if(graphType == COMPARISON)
        {
            char text[MAX_BUFFER_SIZE];
            ((CEdit*) (GetDlgItem(IDC_OFFSET_EDIT)))->GetLine(0, text);
            m_offsetEdit = text;

            int selected = m_offsetCombo.GetCurSel();
            std::string units = OFFSET_UNITS[selected];

            m_trendProperties.extraInfo = std::string(text) + " " + units;
        }
        else if(graphType == BAR)
        {
            // Don't do anything as the extraInfo attribute for this graph
            // type is set elsewhere
        }
        else
        {
            m_trendProperties.extraInfo = "";
        }

        FUNCTION_EXIT;
    }


    //
    // setExtraInfo
    //
    void CTrendPropertiesDialog::setExtraInfo(const EGraphType& graphType)
    {
        FUNCTION_ENTRY("setExtraInfo");

        if(graphType == COMPARISON)
        {
            std::string offsetStr = m_trendProperties.extraInfo;

            int pos = offsetStr.find(" ");
            std::string numberStr = offsetStr.substr(0, pos);
            std::string units = offsetStr.substr(pos + 1, offsetStr.size() - pos);

            //((CEdit*) (GetDlgItem(IDC_OFFSET_EDIT)))->SetWindowText(numberStr.c_str());
            m_offsetEdit = numberStr.c_str();

            for(int i = 0; i < NUM_OFFSET_UNITS; ++i)
            {
                if(units == OFFSET_UNITS[i])
                {
                    m_offsetCombo.SetCurSel(i);
                    break;
                }
            }
        }

        UpdateData(false);

        FUNCTION_EXIT;
    }


    //
    // onRangeChange
    //
    void CTrendPropertiesDialog::onRangeChange(int days)
    {
        /*UpdateData(true);   
        
        CTime startTime;
        m_startTimeCtrl.GetTime(startTime);

        CTime endTime;
        m_endTimeCtrl.GetTime(endTime);
                   
        // Repopulate the sample periods based on the new range
        populateSamplePeriods(endTime.GetTime(), startTime.GetTime());
        */
    }


    //
    // onDisplayNameChange
    //
    void CTrendPropertiesDialog::onDisplayNameChange(int index)
    {
        std::map<int, int>::iterator iter = m_displayNamesId.find(index);

        TA_ASSERT(iter != m_displayNamesId.end(), "Invalid index");

        int resourceId = iter->second;

        char text[MAX_BUFFER_SIZE];
        ((CEdit*) (GetDlgItem(resourceId)))->GetLine(0, text);
        std::string nameStr(text);

        //if(nameStr.size() > MAX_DISPLAY_NAME_LENGTH)
        //{
        //    nameStr = nameStr.substr(0, MAX_DISPLAY_NAME_LENGTH);
        //    GetDlgItem(resourceId)->SetWindowText(nameStr.c_str());
        //}

        modificationsMade();
    }


    //
    // checkArchivedDataNeeded
    //
    //void CTrendPropertiesDialog::checkArchivedDataNeeded(time_t startTime, int rangeDays, long rangeTime)
    void CTrendPropertiesDialog::checkArchivedDataNeeded(time_t startTime, long rangeSeconds)
    {
        FUNCTION_ENTRY("checkArchivedDataNeeded");

        CTime nowTime = CTime::GetCurrentTime();
        time_t earliestDate = nowTime.GetTime() - ((long) m_archiveDataStore.getDatabaseStorageDays() + 1) * SECONDS_IN_DAY;
        
        if(startTime < earliestDate)
        {
            //time_t endTime = startTime + Utilities::getRangeSeconds(rangeDays, rangeTime);
            time_t endTime = startTime + rangeSeconds;
            if(!m_archiveDataStore.isAllDataRestored(startTime, endTime))
            {
                // Need to restore archived data before all points can be displayed
                std::string unrestoredDates = m_archiveDataStore.getUnrestoredDates(startTime, endTime);

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << unrestoredDates;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120034);
            }
        }

        FUNCTION_EXIT;
    }


    // TD17529++ 
    // setControlsForTrends - this determines the controls to be displayed
	//               depending on the number of trends configurable
	//               and the graph type 
    //
    void CTrendPropertiesDialog::setControlsForTrends(const EGraphType& graphType)
    {
		bool isXY = (graphType == XY );
    
		// there must be at least 1 datapoint to  be configured for trending
		// for the second 
        if(isXY == true)
        {
            GetDlgItem(IDC_ITEM_STATIC1)->SetWindowText("X:");
            GetDlgItem(IDC_ITEM_STATIC2)->SetWindowText("Y:");
			//TD18011 - colour pixels for XY graph should be the same
			// retrieve the colour set for the first color box
			LOGBRUSH logBrush;
            m_itemColours[1]->GetLogBrush(&logBrush);
            // set the colour to the second color box
			setColourEditBackground(2,logBrush.lbColor); 
			//disable the second color button to avoid the colour to be
			//modified
			GetDlgItem(m_colButtonId[2])->EnableWindow(FALSE);
			//++TD18011
        }
        else
        {
            GetDlgItem(IDC_ITEM_STATIC1)->SetWindowText("1.");
            GetDlgItem(IDC_ITEM_STATIC2)->SetWindowText("2.");	
			//TD18011 - for other graph types, must revert the color box
			//to its default colour, and enable it back
			setColourEditBackground(2,DEFAULT_COLOURS[1]); 
			GetDlgItem(m_colButtonId[2])->EnableWindow(TRUE);
			//++TD18011
        }	 
		
		int nCmdShow = SW_SHOW;		
		int nItemsToShow = (isXY == true) ? XY_MAX_ITEMS : m_maxTrendsDisplayed;

		for(int i = 1; i <= MAX_ITEMS_DEFAULT; ++i)
		{
			int nCmdShow = (i <= nItemsToShow) ? SW_SHOW : SW_HIDE;
			GetDlgItem(m_itemNamesId[i])->ShowWindow(nCmdShow);
			GetDlgItem(m_colBoxId[i])->ShowWindow(nCmdShow);
			GetDlgItem(m_staticItemsId[i])->ShowWindow(nCmdShow);
			GetDlgItem(m_itemButtonsId[i])->ShowWindow(nCmdShow);
			GetDlgItem(m_colButtonId[i])->ShowWindow(nCmdShow);
			GetDlgItem(m_displayNamesId[i])->ShowWindow(nCmdShow);
		}			
    }


	/****************
	// TD17529++ - The implementation of this function is incorporated in the newly
	// added method setControlsForTrends()
	 void CTrendPropertiesDialog::setControlsForXY(bool isXY)
    {
        // Item and colour edit boxes
        for(int i = 3; i < 9; ++i)
        {
            GetDlgItem(m_itemNamesId[i])->ShowWindow(!isXY);
            GetDlgItem(m_colBoxId[i])->ShowWindow(!isXY);
        }

        GetDlgItem(m_colBoxId[2])->ShowWindow(!isXY);

        // Item labels
        GetDlgItem(IDC_ITEM_STATIC3)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_STATIC4)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_STATIC5)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_STATIC6)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_STATIC7)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_STATIC8)->ShowWindow(!isXY);

        if(isXY == true)
        {
            GetDlgItem(IDC_ITEM_STATIC1)->SetWindowText("X:");
            GetDlgItem(IDC_ITEM_STATIC2)->SetWindowText("Y:");
        }
        else
        {
            GetDlgItem(IDC_ITEM_STATIC1)->SetWindowText("1.");
            GetDlgItem(IDC_ITEM_STATIC2)->SetWindowText("2.");
        }

        // Item select buttons
        GetDlgItem(IDC_ITEM_BUTTON3)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_BUTTON4)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_BUTTON5)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_BUTTON6)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_BUTTON7)->ShowWindow(!isXY);
        GetDlgItem(IDC_ITEM_BUTTON8)->ShowWindow(!isXY);

         // Display Names
        GetDlgItem(IDC_DISPLAY3)->ShowWindow(!isXY);
        GetDlgItem(IDC_DISPLAY4)->ShowWindow(!isXY);
        GetDlgItem(IDC_DISPLAY5)->ShowWindow(!isXY);
        GetDlgItem(IDC_DISPLAY6)->ShowWindow(!isXY);
        GetDlgItem(IDC_DISPLAY7)->ShowWindow(!isXY);
        GetDlgItem(IDC_DISPLAY8)->ShowWindow(!isXY);

        // Colour select buttons
        GetDlgItem(IDC_COLOUR_BUTTON2)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON3)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON4)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON5)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON6)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON7)->ShowWindow(!isXY);
        GetDlgItem(IDC_COLOUR_BUTTON8)->ShowWindow(!isXY);
    }
	*/

    //
    // checkItemCountIfXY
    //
    bool CTrendPropertiesDialog::hasCorrectItemCountIfXY()
    {
        if(m_graphTypeCtrl.GetCurSel() == XY)
        {
            int count = 0;
            std::map<std::string, TA_Base_Core::ItemInfo>::iterator iter = m_trendProperties.items.begin();
            for(0; iter != m_trendProperties.items.end(); ++iter)
            {
                if(iter->first != EMPTY_STRING)
                {
                    ++count;
                }
            }

			//TD17529 - changed 
            if(count < XY_MAX_ITEMS) 
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120035);
                return false;
            }
        }

        return true;
    }


    //
    // setSamplePeriod
    //
    void CTrendPropertiesDialog::setSamplePeriod()
    {
        if(m_graphTypeCtrl.GetCurSel() == BAR)
        {
            if(m_changeSamplePeriod == true)
            {
                // If there are no sample available (ie. the only choice is '0') then 
                // display a warning
                if(m_samplePeriodCtrl.GetCount() == 1)
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_120036);
                    return;
                }

                // Set the lowest sample period that will still display the graph nicely and set
                // information in the extraInfo attribute so when the dialog is loaded again, the
                // sample period combo box will be set correctly
                m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(1);
                m_trendProperties.extraInfo = "Live";

                m_changeSamplePeriod = false;

                return;
            }
            else
            {
                m_trendProperties.extraInfo = "";
            }
        }
        
        int index = m_samplePeriodCtrl.GetCurSel();
        m_trendProperties.samplePeriod = m_samplePeriodCtrl.GetItemData(index);
    }


    //
    // getSamplePeriod
    //
    void CTrendPropertiesDialog::adjustSamplePeriod()
    {
        if(m_trendProperties.graphType == BAR)
        {
            std::string info(m_trendProperties.extraInfo);
            if(m_trendProperties.extraInfo == "Live")
            {
                m_trendProperties.samplePeriod = LIVE_FIELD_UPDATES;
                return;
            }
        }
    }


    //
    // loadSamplePeriod
    //
    void CTrendPropertiesDialog::loadSamplePeriod(long currentSamplePeriod, bool initialLoad)
    {
        if(m_trendProperties.graphType == BAR)
        {
            std::string info(m_trendProperties.extraInfo);
            if(m_trendProperties.extraInfo == "Live")
            {
                m_samplePeriodCtrl.SetCurSel(0);
                return;
            }
        }

        //
        // Sample Period
        //
        
        // Only set the selection to the specified sample period if this is
        // the initial load, ie. the Trend Properties dialog has just been
        // launched.
        if(initialLoad)
        {
            int i = 0;
            for(; i < m_samplePeriodCtrl.GetCount(); ++i)
            {
                if(m_samplePeriodCtrl.GetItemData(i) == m_trendProperties.samplePeriod)
                {
                    m_samplePeriodCtrl.SetCurSel(i);
                    break;
                }
            }
            // liborm - 28/01/2005 - fix for TD8913
            if( i == m_samplePeriodCtrl.GetCount() )
            {
                // the sample period configured is not available in the list box
                // set sample period list box to the first item available
                m_samplePeriodCtrl.SetCurSel(1);
            }
        }
        else
        {
            if(m_samplePeriodCtrl.GetCount() > 1)
            {
                // If live isn't set but the sample period is set to 0, it means the sample
                // period was automatically set to 0 because the range was too high but the 
                // live checkbox has been manually unchecked. If there are other sample to 
                // choose from apart from '0', it means that the range has been reduced, so 
                // now set it to the lowest sample period.
                if(currentSamplePeriod == 0 && !m_isNowChecked)
                {
                    m_samplePeriodCtrl.SetCurSel(1);
                }
                else
                {
                    // Set the sample period to what it was previously set to before the
                    // reload.
                    int i = 0;
                    for(; i < m_samplePeriodCtrl.GetCount(); ++i)
                    {
                        if(m_samplePeriodCtrl.GetItemData(i) == currentSamplePeriod)
                        {
                            m_samplePeriodCtrl.SetCurSel(i);
                            break;
                        }
                    }

                    // If the previous value isn't available, set it to the lowest sample 
                    // period
                    if(i == m_samplePeriodCtrl.GetCount())
                    {
                        m_samplePeriodCtrl.SetCurSel(1);
                    }
                }

                OnSelchangeSampleCombo();
            }
            else
            {
                // If there is only one option, this means that the range is too large for the possible
                // available sample periods
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LARGE_RANGE, "Range too large"));
            }
        }
    }

    //
    // addNewTemplate
    //
    bool CTrendPropertiesDialog::addNewTemplate()
    {
        FUNCTION_ENTRY("addNewTemplate");
        
        try
        {
            TrendPropertiesStore trendStore;
            trendStore.addTrendProperties(m_trendProperties.name, m_trendProperties);

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << m_trendProperties.name;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120040);

            //modificationsSaved();

            FUNCTION_EXIT;
            return true;
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_ALREADY_EXISTS)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_trendProperties.name;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120003);
            }
            else if(ex.getType() == TrendViewerException::DATABASE_ERROR)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << ex.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
            }
            else
            {
                std::string reason("Unknown");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            std::string action("save");
            std::string reason("an unexpected error occurred.");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << action << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);
        }

        // Retrieve the latest modification time in case the same template is
        // modified again.
        //retrieveLastModifiedTime();

        FUNCTION_EXIT;
        return false;
    }
    
    
    //
    // modifyExistingTemplate
    //
    bool CTrendPropertiesDialog::modifyExistingTemplate(time_t lastModified)
    {
        FUNCTION_ENTRY("modifyExistingTemplate");

        TrendPropertiesStore trendStore;
            
        try
        {
            trendStore.modifyTrendProperties(m_trendProperties.name, m_trendProperties, lastModified);

            // Retrieve the latest modification time in case the same template is
            // modified again.
            //retrieveLastModifiedTime();

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << m_trendProperties.name;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120041);

            //modificationsSaved();

            FUNCTION_EXIT;
            return true;
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_MODIFIED)
            {
                // Template has been modified since user has viewed it. Give warning and 
                // allow them to cancel if desired.
                std::string name(m_trendProperties.name);
                std::string action("save");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << name << action;
            
                if(userMsg.showMsgBox(IDS_UE_120039) == IDOK)
                {
                    try
                    {
                        trendStore.forceModifyTrendProperties(m_trendProperties.name, m_trendProperties);

                        // Retrieve the latest modification time in case the same template is
                        // modified again.
                        //retrieveLastModifiedTime();

                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << m_trendProperties.name;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120041);

                        //modificationsSaved();
                        FUNCTION_EXIT;
                        return true;
                    }
                    catch(const TrendViewerException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

                        if(ex.getType() == TrendViewerException::DATABASE_ERROR)
                        {
                            TA_Base_Bus::TransActiveMessage userMsg;
                            userMsg << ex.what();
                            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
                        }
                        else
                        {
                            std::string reason("Unknown.");
                            TA_Base_Bus::TransActiveMessage userMsg;
                            userMsg << reason;
                            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
                        }

                        FUNCTION_EXIT;
                        return false;
                    }
                    catch(...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                        std::string action("save");
                        std::string reason("an unexpected error occurred.");
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << action << reason;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

                        FUNCTION_EXIT;
                        return false;
                    }
                }
            }
        }
        catch(...)
        {
        }

        std::string action("save");
        std::string reason("an unexpected error occurred.");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << action << reason;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

        FUNCTION_EXIT;
        return false;
    }


    void CTrendPropertiesDialog::retrieveLastModifiedTime()
    {
        FUNCTION_ENTRY("retrieveLastModifiedTime");

        try
        {
            // Retrieve the new lastModified time in case this template is modified again
            TrendPropertiesStore trendStore;
            TrendProperties trendProperties = trendStore.getTrendProperties(m_trendProperties.name);
            m_trendProperties.lastModified = trendProperties.lastModified;
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_NOT_EXIST)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_trendProperties.name;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120038);
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error retrieving modification time. Disabling save.");

                TA_ASSERT(GetDlgItem(IDC_SAVE) != NULL, "Save button doesn't exist");
                GetDlgItem(IDC_SAVE)->EnableWindow(false);
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error retrieving modification time. Disabling save.");
            
            TA_ASSERT(GetDlgItem(IDC_SAVE) != NULL, "Save button doesn't exist");
            GetDlgItem(IDC_SAVE)->EnableWindow(false);
        }

        FUNCTION_EXIT;
    }


    void CTrendPropertiesDialog::updateCurrentTrendTemplate()
    {
        FUNCTION_ENTRY("updateCurrentTrendTemplate");

        UpdateData(true);

        m_trendProperties.name = m_name;

        m_trendProperties.description = m_description;

        try
        {
            m_trendProperties.graphType = TrendPropertiesStore::getGraphType(m_graphTypeCtrl.GetCurSel());
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120033);
            return;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120033);
            return;
        }

        CTime startTime;
        m_startTimeCtrl.GetTime(startTime);
        m_trendProperties.startDateTime = startTime.GetTime();

        CTime endTime;
        m_endTimeCtrl.GetTime(endTime);
        m_trendProperties.endDateTime = endTime.GetTime();

        setSamplePeriod();

        // If the sample period is set to 0 - set live to true, even if it wasn't
        // manually set...
        bool isLive;
        if(m_samplePeriodCtrl.GetCurSel() == 0)
        {
            isLive = true;

            // If live mode has been set because the range is too large, then adjust the
            // start and end times so the range is the default range
            if(m_isNowChecked)
            {
                m_trendProperties.endDateTime = CTime::GetCurrentTime().GetTime();
                m_trendProperties.startDateTime = m_trendProperties.endDateTime - 3600;
	            m_rangeTooLarge = false;
            }		 
        }
        else
        {
            isLive = (m_isNowChecked != 0);
        }
        m_trendProperties.live = isLive;
        
        populateTrendPropertyItem(m_trendProperties.items, m_trendProperties.graphType);

        populateExtraInfo(m_trendProperties.graphType);

        FUNCTION_EXIT;
    }


    void CTrendPropertiesDialog::modificationsMade()
    {
        if(!m_initialising)
        {
            GetDlgItem(IDC_SAVE)->EnableWindow(true);
            m_trendProperties.modificationsUnsaved = true;
        }
    }


    void CTrendPropertiesDialog::modificationsSaved()
    {
        GetDlgItem(IDC_SAVE)->EnableWindow(false);
        m_trendProperties.modificationsUnsaved = false;
        m_trendProperties.isNew = false;
    }


    long CTrendPropertiesDialog::getCurrentSamplePeriod()
    {
        int sampleIndex = m_samplePeriodCtrl.GetCurSel();
        return m_samplePeriodCtrl.GetItemData(sampleIndex);
    }

    
    void CTrendPropertiesDialog::populateSamplePeriods(time_t startTime, time_t endTime, long samplePeriod, bool initialLoad /* =false */)
    {
        // Repopulate the sample periods based on the new range
        long rangeSeconds = endTime - startTime;
                //TD18018++
		if (!initialLoad && (m_rangeInSeconds != rangeSeconds))
		{
			m_dateTimeChanged = true;			
		}
		m_rangeInSeconds = rangeSeconds;
               //++TD18018
        populateSampleComboBox(rangeSeconds);

		loadSamplePeriod(samplePeriod, initialLoad);
    }


    void CTrendPropertiesDialog::setLiveMode(bool isLive)
    {
        int buttonState = BST_CHECKED;
        if(!isLive)
        {
            buttonState = BST_UNCHECKED;
        }

        ((CButton*) (GetDlgItem(IDC_NOW_CHECK)))->SetCheck(buttonState);
        m_isNowChecked = isLive;

        OnNowCheck();
    }


    COleDateTime CTrendPropertiesDialog::getCombinedTime(const COleDateTime& date,const COleDateTime& time)
    {
        // Convert the date and time into a single time entity.
        return COleDateTime( date.GetYear(), date.GetMonth(), date.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond() );
    }
}
