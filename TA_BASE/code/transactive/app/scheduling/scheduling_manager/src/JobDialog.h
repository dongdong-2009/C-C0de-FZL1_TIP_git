/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/JobDialog.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Dialog that will be used to display details of a job.
  */

#if !defined(AFX_JOBDIALOG_H__C511CFB6_9D19_442C_8F5D_68408FB89010__INCLUDED_)
#define AFX_JOBDIALOG_H__C511CFB6_9D19_442C_8F5D_68408FB89010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JobDialog.h : header file
//

#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "app/scheduling/scheduling_manager/src/ComponentLibraryAccessor.h"
#include "app/scheduling/scheduling_manager/src/Job.h"
#include <vector>
#include <string>
#include <map>
#include "bus/mfc_extensions/src/multi_dialog/MultiDialog.h"
#include "bus/mfc_extensions/src/bitmap_and_text_push_button/FooButton.h"

/////////////////////////////////////////////////////////////////////////////
// JobDialog dialog

namespace TA_Base_Core
{
    class ILocation;
    class IProfile;
}

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_Base_App
{
    class CSchedulingManagerDoc;

    class JobDialog : public CMultiDialog
    {
    // Construction
    public:
        /** 
          * JobDialog
          *
          * description
          *
          * @param title The resource id that holds the title string of the dialog box
          * @param jobTypes The job types to be supported by the dialog
          * @param doc The document object
          * @param readOnly True if read only
          * @param pParent The parent of the dialog.  Default to NULL.
          *
          */
	    JobDialog(  const unsigned long titleId,
                    std::vector<std::string>& jobTypes, 
                    CSchedulingManagerDoc& doc, 
                    const bool readOnly,
                    const bool hasAssociatedPlugin,
                    CWnd* pParent = NULL);


        /** 
          * ~JobDialog
          *
          * Destructor.
          *
          */
        virtual ~JobDialog();


        /** 
          * setData
          *
          * Sets the job data to be displayed on the dialog.
          *
          * @param job The job to be displayed
          */
        void setData(const Job& job)
        {
            m_job = job;
        }


    // Dialog Data
	    //{{AFX_DATA(JobDialog)
	enum { IDD = IDD_JOB_DIALOG };
    	CComboBox	    m_locationCtrl;
	    CComboBox	    m_profileCtrl;
	    CEdit	        m_nameCtrl;
	    CButton	        m_closeCtrl;
	    CButton	        m_saveNewCtrl;
	    CButton	        m_saveCtrl;
	    CButton	        m_selectJobButton;
	    CButton	        m_jobDetailsButton;
	    CComboBox	    m_jobTypeCtrl;
	    CEdit	        m_descriptionCtrl;
	    FooButton	    m_onceoffRadio;
        FooButton       m_dailyRadio;
        FooButton       m_weeklyRadio;
        FooButton       m_monthlyRadio;
        FooButton       m_yearlyRadio;
	    FooButton	    m_noEndDateRadio;
        FooButton       m_endAfterDateRadio;
        FooButton       m_endByDateRadio;
	    CStatic	        m_rangePanelText;
	    CButton	        m_rangePanel;
	    CEdit	        m_endAfterCtrl;
	    CDateTimeCtrl	m_endByCtrl;
	    CSpinButtonCtrl	m_endAfterSpinnerCtrl;
	    CStatic	        m_summaryCtrl;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(JobDialog)
	    public:
	    virtual BOOL DestroyWindow();
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(JobDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void onRadioNoRecurrence();
	    afx_msg void onRadioDailyRecurrence();
	    afx_msg void onRadioWeeklyRecurrence();
	    afx_msg void onRadioMonthlyRecurrence();
	    afx_msg void onRadioYearlyRecurrence();
	    afx_msg void onNoEndDate();
	    afx_msg void onEndAfter();
		//jinhua++
	    afx_msg void onEndBy( NMHDR* pNMHDR, LRESULT* pResult );
        afx_msg void onChangeOnceoff( NMHDR* pNMHDR, LRESULT* pResult );
	    afx_msg void onChangeDaily( NMHDR* pNMHDR, LRESULT* pResult );
	    afx_msg void onChangeWeekly( NMHDR* pNMHDR, LRESULT* pResult );
	    afx_msg void onChangeMonthlySpecificDay( NMHDR* pNMHDR, LRESULT* pResult );
	    afx_msg void onChangeMonthlyLastDay( NMHDR* pNMHDR, LRESULT* pResult );
		afx_msg void onChangeYearlySpecificDay( NMHDR* pNMHDR, LRESULT* pResult );
	    afx_msg void onChangeYearlyLastDay( NMHDR* pNMHDR, LRESULT* pResult );
		//++jinhua
		afx_msg void onChangeWeekly();
		afx_msg void onChangeMonthlyLastDay();
		afx_msg void onChangeMonthlySpecificDay();
		afx_msg void onChangeYearlySpecificDay();
		afx_msg void onChangeYearlyLastDay();
		afx_msg void onEndBy();
	    afx_msg void onSaveNew();
	    virtual void OnOK();
	    afx_msg void onSelectJob();
	    afx_msg void onJobDetails();
	    afx_msg void onSelchangeJobtype();
	    afx_msg void onChangeJobdescription();
    	afx_msg void onSelchangeProfile();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


	private:
		//jinhua++
		void changeOnceoff();
		void changeDaily();
		//++jinhua

    private:

        /** 
          * initialiseRadioNoRecurrence
          *
          * Displays the once off panel in the Recurrence Pattern panel and sets
          * the date and time if specified.
          *
          * @param theTime The time the job will run.
          * @param theDate The date the job will run.
          *
          */
	    void initialiseRadioNoRecurrence(COleDateTime* theTime = NULL, COleDateTime* theDate = NULL);


        /** 
          * initialiseRadioDailyRecurrence
          *
          * Displays the daily recurrence panel in the Recurrence Pattern panel and
          * sets the time that the job will run daily if they're specified.
          *
          * @param theTime The time the job will run.
          */
	    void initialiseRadioDailyRecurrence(COleDateTime* theTime = NULL);

    
        /** 
          * initialiseRadioWeeklyRecurrence
          *
          * Displays the weekly recurrence panel in the Recurrence Pattern panel and
          * sets the time and days that the job will run every week.
          *
          * @param theTime The time the job will run.
          * @param days This can be a combination of flags like 
          *        TA_Base_App::SchedulingManager::Sunday|TA_Base_App::SchedulingManager::Saturday.
          *        Is set to 0 by default to indicate no days are selected.
          */
        void initialiseRadioWeeklyRecurrence(COleDateTime* theTime = NULL, unsigned long days = 0);

    
        /** 
          * initialiseRadioMonthlyRecurrence
          *
          * Displays the monthly recurrence panel in the Recurrence Pattern panel.
          * If theDay is not 0 then the specific day is specified on the first radio
          * control.  Otherwise, the last day radio dial is selected.
          *
          * @param theTime The time the job will run.
          * @param theDay The day that the job will run.  If it is 0, then it will run on 
          *        the last day of the month, otherwise it will run on the specified day.
          *
          */
        void initialiseRadioMonthlyRecurrence(COleDateTime* theTime = NULL, unsigned long theDay = 1);


        /** 
          * initialiseRadioYearlyRecurrence
          *
          * Displays the yearly recurrence panel in the Recurrence Pattern panel.
          *
          * @param theTime The time the job will run.
          * @param day Set to run on the current date by default.  0 indicates it will run on the last day.
          * @param month The month that the job will run on.
          *
          */
	    void initialiseRadioYearlyRecurrence(COleDateTime* theTime = NULL, unsigned short theDay = USHRT_MAX, unsigned short theMonth = USHRT_MAX);



        /** 
          * initialiseDateTimeCtrl
          *
          * If the control is for date, then it will have the format of '26-Sep-04'.  If yearly is
          * set to true, then it will have the format of '26-Sep'
          * If the control is for time, it will have the format of '23:10'.
          *
          * @param itemId The resource Id
          * @param theDate The default date to be shown 
          * @param earliest The earliest date that the control allows
          * @param latest The latest date that the control allows
          * @param yearly Shows the day and year by default.  Have to set this to true if 
          *        don't want to have the day and year in the string
          */
        void initialiseDateTimeCtrl(const int itemId, const COleDateTime* theDate, const COleDateTime* earliest = NULL, const COleDateTime* latest = NULL, const bool yearly = false);


        /** 
          * initialiseEditAndSpinner
          *
          * Initialises the edit and associated spinner
          *
          * @param editId The resource Id of the edit box
          * @param spinnerId The resource Id of the spinner control
          * @param min minimum value the spinner could spin to
          * @param max maximum value the spinner could spin to
          * @param select the selected value.  If this is not set or is UINT_MAX, then no value will be selected.  
          *
          */
        void initialiseEditAndSpinner(const int editId, const int spinnerId, const int min, const int max, const int select = UINT_MAX);


        /** 
          * checkButton
          *
          * Checks the radio/check button
          *
          * @param type The type of button - radio or check
          * @param itemId The resource Id
          * @param wantCheck True if want to check
          *
          */
        void checkButton(const FooButton::Type type, const int itemId, const bool wantCheck);


        /** 
          * enableControl
          *
          * Enables/disables the control
          *
          * @param itemId The resource Id
          * @param enable Enables by default.  Disables if false.
          *
          */
        void enableControl(const int itemId, const int enable = true);


        void setReadOnly(const int itemId);


        /** 
          * setFrequencySummary
          *
          * Sets the frequency of recurrence on the summary panel.
          *
          * @param str
          */
        void setFrequencySummary(const CString& str, const TA_Base_App::SchedulingManager::ERecurrenceType);


        /** 
          * setPeriodSummary
          *
          * Sets the period of recurrence on the summary panel.
          *
          * @param str
          */
        void setPeriodSummary(const CString& str, const TA_Base_App::SchedulingManager::ERecurrenceRange range);


        /** 
          * disableAllControls
          *
          * Disable all controls to make the dialog read-only
          *
          */
        void disableAllControls();


        /** 
          * enableRangePanel
          *
          * Enables/Disables the controls in the Range of Recurrence panel
          *
          * @param enable If true, all controls will be enabled.  Otherwise,
          *        all except the OK button will be disabled.
          */
        void enableRangePanel(const bool enable);


        /** 
          * getDateTime
          *
          * Returns the date time object captured by the specified resource
          *
          * @param itemId The resource Id
          *
          * @return the date time object
          */
        COleDateTime getDateTime(const int itemId) const;


        /** 
          * getTimeString
          *
          * Returns the specified time in the form of a string (HH24:MI)
          *
          * @param theTime The time to be displayed
          *
          * @return The formatted time string
          */
        CString getTimeString(COleDateTime& theTime) const;


        /** 
          * getDateString
          *
          * Returns the date string in the format of Thursday, November 1, 2004 if
          * it is a non yearly date, and display it in the format of November 1 if
          * if it yearly.
          *
          * @param theDate The date to be displayed
          * @param showYear Will display day and year if this is true.
          *
          * @return The formatted date string
          *
          */
        CString getDateString(COleDateTime& theDate, const bool showYear = true) const;


        /** 
          * prepareDataToSave
          *
          * Gets the user input from the dialog and set the corresponding values in the m_job member
          *
          * @return false if user has not entered all required data
          */
        bool prepareDataToSave();


        /** 
          * initialiseProfileCombo
          *
          * Initialises the Profile combo box with the available profiles based on the 
          * current operator's rights.
          *
          */
        void initialiseProfileCombo();

        /** 
          * isJobDetailsButtonEnabled
          *
		  * Checks that a plan has been selected for this job, and that the user has the
		  * rights to both run the Plan Manager and control the selected plan. If all this 
		  * is so, then the Details button can be enabled.
          * 
          * @return true if the 'Details...' button for the selected plan should be enabled, false otherwise.
          */
		bool isJobDetailsButtonEnabled();

        // The title of the dialog box
        CString m_title;

        // array of job type names
        std::vector<std::string>& m_jobTypes;

        // This is used by the dialog to schedule new jobs.  Cache this so that we can use it easily
        CSchedulingManagerDoc& m_doc;

        // The job data
        Job m_job;

        // The component that performs DLL related tasks
        ComponentLibraryAccessor m_componentLibraryAccessor;

        // True if read only
        bool m_readOnly;

        // the frequency string on the summary panel
        CString m_frequencyStr;

        // the period string on the summary panel
        CString m_periodStr;

        // Schedule Type
        TA_Base_App::SchedulingManager::ERecurrenceType m_scheduleType;

        // The recurrence range
        TA_Base_App::SchedulingManager::ERecurrenceRange m_range;

        // The name of the group that encapsulates the recurrence pattern buttons
        static const CString RECURRENCE_PATTERN_GROUP;

        // The name of the group that encapsulates the range of recurrence buttons
        static const CString RANGE_OF_RECURRENCE_GROUP;

        // The first day of month is 1
        static const int FIRST_COMMON_DAY_OF_MONTH;

        // The last day in month that the user could input.  Set to 31. We allow
        // setting the recurrence date on a day that only applies to some months.
        static const int LAST_COMMON_DAY_OF_MONTH;

        // Minimum number of repeats
        static const int MIN_RECURRENCE;

        // Indicates whether there is an associated plugin for this instance of the Job Dialog
        bool m_hasAssociatedPlugin;

        // Location map that caches all locations so that this class doesn't need
        // to get associated locations from the database each time the profile changes
        LocationMap m_locationMap;

		// TD #12412
		// the rights library for checking if user can control the selected plan for this job
        TA_Base_Bus::RightsLibrary *m_rightsLibrary;
		unsigned long m_planManagerKey;

		bool m_onceOffExecuteNow;
		bool m_onceOffFirstExecution;
    };
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOBDIALOG_H__C511CFB6_9D19_442C_8F5D_68408FB89010__INCLUDED_)
