/*
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/CrystalReportsHelper.hpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2012/01/13 10:05:23 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This is the header file for the CrystalReportsHelper class.
  */

#ifndef CRYSTALREPORTSHELPER_HPP
#define CRYSTALREPORTSHELPER_HPP

#include <atlbase.h>
#include <string>
#include <vector>
#include "crystalreportviewer9.h"
#include "webbrowser2.h"
#include "EventLogger.hpp"
#include "AppSettings.hpp"

#import "../../../../cots/CrystalReports/dlls/craxdrt9.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

		// Forward reference.
		class CCrutilDlg;

		class CrystalReportsHelper
		{
		public:
			enum crhViewingType
			{
				crhOnDemand = 0,
				crhHistorical = 1
			};

		    /**
		      * CrystalReportsHelper
		      * 
		      * This is the constructor for the CrystalReportsHelper class.
		      *
		      */
			CrystalReportsHelper();

		    /**
		      * ~CrystalReportsHelper
		      * 
		      * This is the destructor for the CrystalReportsHelper class.
		      *
		      */
			~CrystalReportsHelper();

		    /**
		      * isReady
		      * 
		      * This method indicates whether or not Crystal Reports functionality
		      * is available.
		      *
		      * @return True if Crystal Reports functionality is available otherwise 
		      *         it returns false.
		      *
		      */
			bool isReady();

		    /**
		      * setReportName
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szReportName is string value that represents the Crystal
		      *        Report Template name.
		      *
		      */
			void setReportName( char* p_szReportName );

	        /**
	          * setViewerInstance
	          * 
	          * This method sets a member variable to the value passed in via the 
	          * supplied parameter.
	          *
	          * @param p_pReportViewer is a pointer to an instance of the Crystal Reports
	          *        Viewer control that is used to view report templates.
	          *
	          */
			void setViewerInstance( CCrystalReportViewer9* p_pReportViewer ); 

		    /**
		      * setBrowserInstance
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_pHistoryReportViewer is a pointer to an instance of the Web Browser
		      *        control that is used to view historical reports.
		      *
		      */
			void setBrowserInstance( CWebBrowser2* p_pHistoryReportViewer ); 

		    /**
		      * setDialogInstance
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_pCCrutilDlg is a pointer to an instance of the CCrutilDlg object
		      * that is the main window for this application.
		      *
		      */
			void setDialogInstance( CCrutilDlg* p_pCCrutilDlg ); 

		    /**
		      * setFormat
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szFormat is a string value that specifies the format for 
		      *        exported reports.
		      *
		      */
            void setFormat( char* p_szFormat );

		    /**
		      * setOutputFilename
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szOutFile is a string value that specifies the output filename
		      *        for exported reports.
		      *
		      */
			void setOutputFilename( char* p_szOutFile );

		    /**
		      * setOutputPath
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szOutPath is a string value that specifies the directory path
		      *        for exported reports.
		      *
		      */
			void setOutputPath( char* p_szOutPath );

		    /**
		      * enablePrompt
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_bPrompt is a boolean value that indicates whether or not to
		      *        display a prompting dialog when reports are either exported
		      *        or printed via command line options.
		      *
		      */
			void enablePrompt( bool p_bPrompt = true);

		    /**
		      * enableTimeStamping
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_bTimeStamping is a boolean value that indicates whether or not
		      *        to prepend a date/time stamp string to the filename of 
		      *        exported reports.
		      *
		      */
			void enableTimeStamping( bool p_bTimeStamping = true);

		    /**
		      * enableParameterPrompting
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_bEnableParameterPrompting is a boolean value that indicates whether
		      *        or not to enable report parameter prompting when viewing, exporting
		      *        or printing reports.
		      *
		      */
			void enableParameterPrompting( bool p_bEnableParameterPrompting = true);

		    /**
		      * setDLLName
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szDLLName is a string value that represents that name of 
		      *        of the Crytsal Reports DLL that provides connectivity to the
		      *        database.
		      *
		      */
            void setDLLName( char* p_szDLLName );

		    /**
		      * setServerName
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szServerName is a string value that represents the logical
		      *        server name where the database resides.
		      *
		      */
			void setServerName( char* p_szServerName );

		    /**
		      * setDatabaseName
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szDatabaseName is a string value that represents the logical
		      *        name of the database.
		      *
		      */
			void setDatabaseName( char* p_szDatabaseName );

		    /**
		      * setUserID
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szUserID is a string value that represents the user id that
		      *        is required to access the database.
		      *
		      */
			void setUserID( char* p_szUserID );

		    /**
		      * setPassword
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szPassword is a string value that represents the password that
		      *        is required to access the database.
		      *
		      */
			void setPassword( char* p_szPassword );

		    /**
		      * setViewingType
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_crhViewingType is a enumerated value that specifies the type
		      *        of report viewing that is required.
		      *
		      */
			void setViewingType( crhViewingType p_crhViewingType );

		    /**
		      * setSessionID
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szSessionId is a string value that represents that represents
              *        a TransActive Session ID.
		      *
		      */
			void setSessionID( char* p_szSessionID );

		    /**
		      * setOperatorName
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_szOperatorName is a string value that represents that 
              *        represents the currently logged in TransActive operator.
		      *
		      */
			void setOperatorName( char* p_szOperatorName );

		    /**
		      * getViewingType
		      * 
		      * This method returns the currently set report viewing type.
		      *
		      * @return The currently set report viewing type.as defined by the
		      *         crhViewingType enumeration.
		      *
		      * @param <parameters> Optional
		      *
		      */
			crhViewingType getViewingType( void );

		    /**
		      * setWindowWidth
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_cx is an integer value that specifies the width of the 
		      *        application window.
		      *
		      */
            void setWindowWidth( int p_cx );
			
			/**
		      * getWindowWidth
		      * 
		      * This method get the value of window width
		      *
			  * @return The currently window width
		      */
			int getWindowWidth()
			{
				return m_cx;
			}

		    /**
		      * setWindowHeight
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param p_cy is an integer value that specifies the height of the 
		      *        application window.
		      *
		      */
			void setWindowHeight( int p_cy );
			
			/**
		      * getWindowHeight
		      * 
		      * This method get the value of window height
		      *
			  * @return The currently window height
		      */

			int getWindowHeight()
			{
				return m_cy;
			}

			/**
			  * setTreeBorder
			  *
			  * This method sets a member variable to the value passed in via the 
			  * supplied parameter.
			  *
			  * @param p_treeBorder is an interger value that specifies the border value
			  * between report tree and report viewer.
			  *
			  */
			void setTreeBorder( int p_treeBorder );

		    /**
		      * getIsPrintFromGUI
		      * 
		      * This method returns boolean value of whether print function is called by 
		      * ReportManager GUI. 
		      *
			  * @return the currently m_isPrintFromGUI value
			  *         
		      */
			bool getIsPrintFromGUI();
			
		    /**
		      * setIsPrintFromGUI
		      * 
		      * This method sets a member variable to the value passed in via the 
		      * supplied parameter.
		      *
		      * @param isPrintFromGUI is an boolean value that specifies the whether 
		      *        print function is called by ReportManager GUI's print button.
		      *
		      */
			void setIsPrintFromGUI( bool isPrintFromGUI );

		    /**
		      * viewReport
		      * 
		      * This method displays the selected report. If the report is On Demand,
		      * then the Crystal Reports Viewer control is used to display the report
		      * template. If the report is historical, then the Web Browser control is 
		      * used to display the report.
		      *
		      * @return True if the report was successfully displayed otherwise it 
		      *         returns false.
		      */
            bool viewReport(std::vector<std::string>& parameterValuePairs);

		    /**
		      * printReport
		      * 
		      * This method prints the selected report to the default network printer.
		      *
		      * @return True if the report was successfully printed otherwise it 
		      *         returns false.
		      *
		      */
			bool printReport(std::vector<std::string>& parameterValuePairs);

		    /**
		      * printReport(unsigned long incidentid)
		      * 
		      * This method prints the selected report to the default network printer with the incident id.
              * this method is only used by the incident manager
		      *
		      * @return True if the report was successfully printed otherwise it 
		      *         returns false.
		      *
		      */
            bool printReport(std::string incidentid);


            /**
            * setupincidentId
            * 
            * this is function sets up the report id used by the crystal report, which is only used by the incident manager
            * 
            * @return bool 
            * @param : unsigned long incidentid
            * 
            * @exception <exceptions> Optional
            */
            bool setupincidentId(std::string incidentid);

            /**
		      * exportReport
		      * 
		      * This method exports the selected report to disk.
		      *
		      * @return True if the report was successfully exported otherwise it 
		      *         returns false.
		      *
		      */
			bool exportReport(std::vector<std::string>& parameterValuePairs);

		    /**
		      * refreshReport
		      * 
		      * This method refreshes the report that is currently being viewed. If the
		      * report being viewed is On Demand, then the content of the report will also
		      * be refreshed. If the report is historical, then the content of the report 
		      * remains the same.
		      *
		      */
			void refreshReport();

		    /**
		      * getReportName
		      * 
		      * This method strips off the directory path and returns the 
		      * report filename. 
		      *
		      * @return A pointer to a string that contains the report name.
		      *
		      */
			char* getReportName();

		    /**
		      * getHistoryReportName
		      * 
		      * This method strips off the directory path and returns the 
		      * history report filename. 
		      *
		      * @return A pointer to a string that contains the history report name.
		      *
		      */
			char* getHistoryReportName();

			//TD18040
		    // xxs add
            /**
			  * setHistoryReportPathFromCmdLine
		      * 
		      * This method sets the history report path from the command line
		      *
		      * @return A pointer to a string that contains the report's
		      *         directory path.
		      *
		      */
			void setHistoryReportPathFromCmdLine( const char * pathName );
			//end xxs add

            /**
		      * getReportPath
		      * 
		      * This method strips off the report filename and returns the
		      * directory path.
		      *
		      * @return A pointer to a string that contains the report's
		      *         directory path.
		      *
		      */
			char* getReportPath();

			//TD18040
			//xxs add
		    /**
		      * getHistoryReportPath
		      * 
		      * it can get path from commandLine
			  * or it can get path from report file name 
		      *
		      * @return A pointer to a string that contains the history 
		      *         report's directory path.
		      *
		      */
			CString getHistoryReportPath( bool useCmdLineParam );
			//end xxs add

		    /**
		      * sendAuditMessage
		      * 
		      * This method sends an Audit message to the TransActive
		      * database indicating that a report has been accessed.
		      *
		      * @return True if the method was successfully executed, 
		      *         otherwise it returns false.
		      *
		      */
			bool sendAuditMessage( char* p_szAuditMessage );

            /**
		      * setupParameters
		      * 
		      * This method initialises the reports parameters with those
		      * supplied by the user.
		      * Note that currently is not been actively called. It has 
		      * been used to prove the concept.
		      *
		      * @return True if the method was successfully executed, 
		      *         otherwise it returns false.
		      *
		      */
            void setupParameters(std::vector<std::string>& parameterValuePairs);

			void showWindowCorrectly(int left, int top);

			/**
			  * setDoubleClickTree
			  *
			  * When user double click report tree, set m_doubleClickTree to true.
			  *
			  */
			void setDoubleClickTree( bool value );

			/**
			  * getDoubleClickTree
			  *
			  * get the value of m_doubleClickTree
			  *
			  */
			bool getDoubleClickTree();

		protected:
		private:
		    /**
		      * initialise
		      * 
		      * This method initialises access to the Crystal Reports component and
		      * makes this access available via two member variables, m_pIApplication
		      * and m_pIReport.
		      *
		      * @return True if successful otherwise it returns false.
		      *
		      */
			bool initialise();

		    /**
		      * setupDefaults
		      * 
		      * This method initialises a set of member variables with the values
		      * that have been previously stored in the registry.
		      *
		      */
			void setupDefaults();

		    /**
		      * getHistoryReportPath
		      * 
		      * This method strips off the history report filename and 
		      * returns the directory path.
		      *
		      * @return A pointer to a string that contains the history 
		      *         report's directory path.
		      *
		      */
			char* getHistoryReportPath();

			/**
		      * openReport
		      * 
		      * This method open the report as specified by the m_ccbstrReportSource 
		      * member variable.
		      *
		      * @return True if the report was successfully opened otherwise it 
		      *         returns false.
		      *
		      */
			bool openReport();

		    /**
		      * stripOffExtension
		      *
		      * This method strips off the file extension.
		      * 
		      * @return True if successful otherwise it returns false. When succesful,
		      *         the p_szFullReportName parameter will contain the resultant string. 
		      *
		      * @param p_szFullReportName is a string value that specifies the filename
		      *        for which the extension is to be stripped off.
		      *
		      */
			bool stripOffExtension( char* p_szFullReportName );

		    /**
		      * getValueForFormat
		      * 
		      * This method returns an enumerated value the member variable
		      * m_cstrFormat. This member varibale represents the current 
		      * configuration for the export format. 
		      *
		      * @return The enumerated value for the currently configured
		      *         export format.
		      *
		      */
			enum CRExportFormatType getValueForFormat();

		    /**
		      * getExtensionForFormat
		      * 
		      * This method returns a string that contains the file extension
		      * that represents value of the member variable m_cstrFormat. This 
		      * member varibale represents the current configuration for the 
		      * export format. 
		      *
		      * @return A pointer to a string that contains the file extension
		      *         that represents the current export format.
		      *
		      */
            char* getExtensionForFormat();

		    /**
		      * getExportFileName
		      * 
		      * This method generates the export filename. If the date/time 
		      * stamping is required, then it is prepended to the filename.
		      *
		      * @return A pointer to a string that contains the export filename.
		      *
		      */
			char* getExportFileName();

		    /**
		      * getReportFileName
		      * 
		      * This method returns the report template file name.
		      *
		      * @return A pointer to a string that contains the report name.
		      *
		      */
			char* getReportFileName();

		    /**
		      * reportTemplateExists
		      * 
		      * This method checks for the existance of the report template as
		      * specified in m_ccbstrReportTemplate.
		      *
		      * @return True is the report template exists othherwise returns false.
		      *
		      */
			bool reportTemplateExists();

		    /**
		      * generateTimestamp
		      * 
		      * This method generates a date and time stamp based on the current
		      * date and time.
		      *
		      * @return A pointer a string that contains the generated date/time stamp.
		      *
		      */
			char* generateTimestamp();

		    /**
		      * setupGroupButton
		      * 
		      * This method interrogates the opened report to determines if the 
		      * report has groupings. If so, then the Groupings select button on
		      * the Crystal Reports control is enabled. If not, then the button is 
		      * disabled.
		      *
		      */
			void setupGroupButton();

		    /**
			  * reviewReportToShowCorrect
			  *
			  * Somtimes the CrystalReport open report file may have some issue(s), 
			  * and it said that 'The control is busy downloading data.' In case of this issue 
			  * happen, we need to reviewReport to show the report file in correct type.
			  */
			void reviewReportToShowCorrect(std::vector<std::string>& parameterValuePairs);

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * application object.
		      */
			IApplication* m_pIApplication;

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * report object.
		      */
			IReport* m_pIReport;

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * export options object.
		      */
			IExportOptions* m_pIExportOptions;

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * database object.
		      */
			IDatabase* m_pIDatabase;

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * database tables object.
		      */
			IDatabaseTables* m_pITables;

		    /**
		      * Stores the Crystal Report template file that is being accessed.
		      */
			CComBSTR m_ccbstrReportSource;

            /**
		      * A secondary storage for the Crystal Report template file that 
              * is being accessed.
		      */
			CComBSTR m_ccbstrReportTemplate;

		    /**
		      * Stores the logical name for the network printer driver.
		      */
			CComBSTR m_ccbstrPrinterDriver;

		    /**
		      * Stores the logical name for the network printer.
		      */
			CComBSTR m_ccbstrPrinterName;

		    /**
		      * Stores the network printer port.
		      */
			CComBSTR m_ccbstrPrinterPort;

		    /**
		      * Stores the report export format type.
		      */
			CString m_cstrFormat;

		    /**
		      * Stores the filename for the exported report.
		      */
			CComBSTR m_ccbstrOutputFilename;

		    /**
		      * Stores the directory path for the exported report.
		      */
			CComBSTR m_ccbstrOutputPath;

		    /**
		      * Stores the DLL filename for the Crystal Reports database driver.
		      */
			CComBSTR m_ccbstrDllName;

		    /**
		      * Stores the logical server name which contains the database 
              * being accessed.
		      */
			CComBSTR m_ccbstrServerName;

		    /**
		      * Stores the logical name for the database being accessed.
		      */
			CComVariant m_vtDatabaseName;

		    /**
		      * Stores the logon user ID for the database being accessed.
		      */
			CComVariant m_vtUserID;

		    /**
		      * Stores the logon password for the database being accessed.
		      */
			CComVariant m_vtPassword;

		    /**
		      * Used as a dummy variable in parameter 2 for the OpenReport
              * method provided by the Crystal Reports application object.
		      */
			CComVariant m_vtValue;

		    /**
		      * Stores a pointer to the instance of a Crystal Reports
              * Viewer ActiveX control.
		      */
			CCrystalReportViewer9* m_pReportViewer;

		    /**
		      * Stores a pointer to the instance of a Web Browser
              * ActiveX control.
		      */
			CWebBrowser2* m_pHistoryReportViewer;

		    /**
		      * A boolean variable that determines whether the Crystal Reports 
              * prompting dialog is displayed when printing or exporting reports,
		      */
			bool m_bDisplayPrompt;

		    /**
		      * A boolean variable that determines whether a date and time stamp
              * is prepended to the exported filename.
		      */
			bool m_bAddTimeStamp;

		    /**
		      * A boolean variable that determines whether the Crystal Reports 
              * parameter prompting dialog is displayed or not.
		      */
			VARIANT_BOOL m_bEnableParameterPrompting;

		    /**
		      * Used as a temporary string variable.
		      */
			CString m_cstrTemp;

		    /**
		      * Stores the date and time stamp string that is prepended
              * to the exported reports filename.
		      */
			CString m_cstrTimestamp;

		    /**
		      * Stores a pointer to the instance of an EventLogger object.
		      */
			EventLogger* m_pEventLogger;

		    /**
		      * Stores a pointer to the instance of an AppSettings object.
		      */
			AppSettings* m_pAppSettings;

		    /**
		      * Stores the currently selected report viewing type as 
              * determined by the crhViewingType enumeration.
		      */
			crhViewingType m_crhViewingType;

		    /**
		      * Stores the value for the current window width.
		      */
			int m_cx;

		    /**
		      * Stores the value for the current window height.
		      */
			int m_cy;

			/**
			  * Stores the border between report tree and report viewer.
			  */
			int m_treeBorder;

			/**
			  * Stores the value of whether print fuction is called by ReportManager GUI.
			  */
			bool m_isPrintFromGUI;

		    /**
		      * Stores a pointer to the instance of the application's
              * dialog object.
		      */
			CCrutilDlg* m_pCCrutilDlg;

		    /**
		      * Used to control whether the UserId and Password is used
              * to access the database.
		      */
			CString m_cstrUseLogon;

		    /**
		      * Stores the current ControlStation session ID.
		      */
			CString m_cstrSessionId;

		    /**
		      * Stores the operator name for the currently logged in
              * ControlStation session.
		      */
			CString m_cstrOperatorName;

			//TD18040
			//xxs add
		    /**
		      * Stores the  history report path
			*/
			CString m_cstrHistoryReportPath;
			//end xxs add

			/**
			  * When double click report-tree to open file, 
			  * if open file failed, the caption of ReportManager
			  * should not change.
			  */
			bool m_doubleClickTree;

		};

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

#endif

