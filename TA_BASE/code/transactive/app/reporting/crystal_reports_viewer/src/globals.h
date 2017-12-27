/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/globals.h $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a header file that contains global application definitions.
  */

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef TRANSACTIVE_BUILD

//using namespace TA_Base_Core;

namespace TA_Base_Core
{
    // External versioning variables
    // See VersionGen.cpp
    //
    extern char buildDate[];
    extern char componentName[];
    extern char componentVersion[];
}
#endif // TRANSACTIVE_BUILD

const static char* COMPANY_NAME = "Ripple Systems";
const static char* EXECUTABLE_NAME = "ReportManager.exe";
const static char* APPLICATION_NAME = "ReportManager";

#ifdef TRANSACTIVE_BUILD
const static char* CAPTION_TITLE = TA_Base_Core::componentName;
const static char* APPLICATION_VERSION = TA_Base_Core::componentVersion;
const static char* BUILD_DATE = TA_Base_Core::buildDate;
#else
const static char* APPLICATION_VERSION = "1.04";
const static char* BUILD_DATE = "29/04/2004";
#endif // TRANSACTIVE_BUILD

#define APPLICATION_RC_VERSION 1,4,0,0

const static char* REG_USELOGON_LABEL = "UseLogon";
const static char* REG_DLLNAME_LABEL = "DllName";
const static char* REG_SERVERNAME_LABEL = "ServerName";
const static char* REG_DATABASENAME_LABEL = "DatabaseName";
const static char* REG_USERID_LABEL = "UserId";
const static char* REG_PASSWORD_LABEL = "Password";
const static char* REG_WINDOWTOP_LABEL = "WindowTop";
const static char* REG_WINDOWLEFT_LABEL = "WindowLeft";
const static char* REG_WINDOWBOTTOM_LABEL = "WindowBottom";
const static char* REG_WINDOWRIGHT_LABEL = "WindowRight";

const static char* VERSION_LABEL = "Version";
const static char* FULLSCREEN_LABEL = "FullScreen";
const static char* REPORTNAME_LABEL = "File";
const static char* VIEWREPORT_LABEL = "View";
const static char* PRINTREPORT_LABEL = "Print";
const static char* EXPORTREPORT_LABEL = "Export";
const static char* ENABLEPROMPT_LABEL = "Prompt";
const static char* ENABLETIMESTAMP_LABEL = "Timestamp";
const static char* EXPORTFORMAT_LABEL = "Format"; 
const static char* OUTFILE_LABEL = "OutFile"; 
const static char* OUTPATH_LABEL = "OutPath";
const static char* DLLNAME_LABEL = "DllName";
const static char* SERVERNAME_LABEL = "ServerName";
const static char* DATABASENAME_LABEL = "DatabaseName";
const static char* USERID_LABEL = "UserId"; 
const static char* PASSWORD_LABEL = "Password";
const static char* NOPARAMPROMPTING_LABEL = "NoParameters";
const static char* HELP_LABEL = "Help";
const static char* H_LABEL = "H";
const static char* QUESTION_LABEL = "?";
const static char* REPORTPARAMETER_LABEL = "ReportParameters";

const static char* YES_STRING = "yes";
const static char* NO_STRING = "no";

const static char* RPARAM_ENTITYKEY = "ReportManagerEntityKey";

const static char* PRINT_INCIDENT_LABEL = "PrintIncident";
const static char* INCIDENT_REPORT_ID_LABEL = "IncidentId";

//TD18040
//xxs add
const static char * HISTORY_REPORT_PATH_LABEL = "HistoryreportPath"; //--historyreport-path
//end xxs add


#endif
