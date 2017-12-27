#if !defined(BasicHelper_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define BasicHelper_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/BasicHelper.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Basic functions for validating/formatting dates, retrieving session information,
  * sending audit messages, checking rights and processing log files.  
  * All the functions within class are static.  They are used by multiple
  * classes.
  *
  * KT 10 Mar 04: PW #3161 & #3136: Some of the methods have been removed from this
  * class as a result of these issues.
  */

#include <string>
#include <map>
#include <afxdisp.h>
#include <afx.h>
#include "MediaStatus.h"
#include "core/message/types/MessageTypes.h"


namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_ArchiveManager
{
	const std::string RPARAM_CHECK_BURNED_FILES( "CheckFileExistence" );

    const double MEGABYTE( 1024.0 * 1024.0 );

    // 
    // If this changes, convertTimeTToFormattedString() should also be updated.
    //

    //const std::string REVERSE_DATE_FORMAT( "YYYY_MM_DD" ); //TD12474 

    //
    // The number of seconds in a day.
    //

    const time_t ONE_DAY( 60 * 60 * 24 );

    //
    // Levels of messages to be displayed to the user.
    //

    enum EMsgLevel
    {
        INFO    = 0,        // Information message - doesn't affect process.
        WARN    = 1,        // Warning message - choice or alternative paths.
        ERR     = 2         // Error message - process cannot continue.
    };


    ///////////////////////////////////////////////////////////////////////////
    //
    // Date formatting.
    //
    ///////////////////////////////////////////////////////////////////////////
    
    /**
      * convertCOleDateTimeToTimeT
      *
      * Converts a date in COleDateTime to a time_t.
      *
      * @return     time_t
      *             The date as a time_t.
      *
      * @param      const COleDateTime& date
      *             The date as a COleDateTime.
      *
      * @exception  ArchiveException
      *             Thrown if the mktime function fails.
      */

    time_t convertCOleDateTimeToTimeT( const COleDateTime& date );

    /**
      * convertCOleDateTimeToStringDate
      *
      * Converts a time_t to a date string.
      *
      * @return     const std::string& date
      *             The date as a string in the format specified by the current locale.
      *
      * @param      const time_t& date
      *             The date as a time_t.
      * @param      bool includeTime
      *             True if the time should be included in the output string.
      *             False otherwise.
      */

    std::string convertTimeTToStringDate( const time_t& date, bool includeTime = false );
  
    /**
      * convertTimeTToFormattedString
      *
      * Converts a time_t to a date string in the format REVERSE_DATE_FORMAT.
      * NB Changing this affects how the archive and log files are named!
      *
      * If the REVERSE_DATE_FORMAT changes, this method should also be updated and vice versa.
      *
      * @return     const std::string& date
      *             The date as a string in the format REVERSE_DATE_FORMAT.
      *
      * @param      const time_t& date
      *             The date as a time_t.
      */

    std::string convertTimeTToFormattedString( const time_t& date );
        
    /**
      * convertTimeTToYYMMDDString
      *
      * Converts a time_t to a date string in the format YYMMDD.
      *
      * @return     const std::string& date
      *             The date as a string in the format YYMMDD.
      *
      * @param      const time_t& date
      *             The date as a time_t.
      */

    std::string convertTimeTToYYMMDDString( const time_t& date );

	/**
      * convertTimeTToDDMMYYString
      *
      * Converts a time_t to a date string in the format DDMMYY.
      *
      * @return     const std::string& date
      *             The date as a string in the format DDMMYY.
      *
      * @param      const time_t& date
      *             The date as a time_t.
      */

    std::string convertTimeTToDDMMYYString( const time_t& date );

	std::string convertYYYYMMDDStringToDDMMYYString( const std::string& strSrc);


    ///////////////////////////////////////////////////////////////////////////
    //
    // File name processing.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
      * getDateFromFileName
      *
      * Extracts the date from the given file name. 
      * **NOTE** It is expected that the file name is in the format
      * AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix_V2.dmp, 
      * where YYYY is the year, MM is the month and DD is the day and V2 indicates
	  * file created by new Archive Manager Version.
      *
      * @return time_t
      *         The date as a time_t.
      *         KT 9 Mar 04: PW #3161 Changed to remove dependence on date format.
      *
      * @param  const std::string& fileName
      *         The name of the file to extract the date from.
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1. The file name is not in the format described above.
      *             2. The date cannot be converted into a time_t.
      */

    time_t getDateFromFileName( const std::string& fileName );
	
	//TD16705
	/**
      * getDateFromFileNameDDMMYY
      *
      * Extracts the date from the given file name. 
      * **NOTE** It is expected that the file name is in the format
      * ADDMMYYfilepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp, 
      * where YY is the year, MM is the month and DD is the day.
      *
      * @return time_t
      *         The date as a time_t.
      *         KT 9 Mar 04: PW #3161 Changed to remove dependence on date format.
      *
      * @param  const std::string& fileName
      *         The name of the file to extract the date from.
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1. The file name is not in the format described above.
      *             2. The date cannot be converted into a time_t.
      */

    time_t getDateFromFileNameDDMMYY( const std::string& fileName );

    /**
      * getPostfixFromFileName
      *
      * Extracts the postfix from the given file name. 
      *
      * Precondition:   It is expected that the file name is in the format
      *                 AYYYY_MM_DD_filepostfix_V2.dmp or MYYYY_MM_DD_filepostfix_V2.dmp, 
      *                 where YYYY is the year, MM is the month and DD is the day.
      *
      * @return std::string
      *         The file postfix.
      *
      * @param  const std::string& fileName
      *         The name of the file to extract the postfix from
      *         or ""
      */

    std::string getPostfixFromFileName( const std::string& fileName );

    /**
      * replaceBackslashes
      *
      * Replaces the \ in a file name with \\.
      *
      * @return     std::string
      *             File name to parse for \'s.
      *
      * @param      const std::string& fileName
      *             File name with \'s replaced with \'s.
      */
      
    std::string replaceBackslashes( const std::string& fileName );

    ///////////////////////////////////////////////////////////////////////////
    //
    // Operator Information.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
      * getOperatorNameFromSessionId
      *
      * Retrieves the operator name from the session id.
      *
      * @return     std::string
      *             The operator's name.
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1) There is a database error.
      *             2) The operator data cannot be determined from the session id.
      */

    std::string getOperatorNameFromSessionId();

    /**
      * getWorkstationNameFromSessionId
      *
      * Retrieves the workstation name from the session id.
      *
      * @return     std::string
      *             The workstation's name.
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1) There is a database error.
      *             2) The workstation data cannot be determined from the session id.
      */

    std::string getWorkstationNameFromSessionId();

    ///////////////////////////////////////////////////////////////////////////
    //
    // Audit message sending.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
      * sendAuditMessage
      *
      * Sends an audit message with the given type and description.
      *
      * @param      const TA_Base_Core::MessageType& messageType  
      *             The type of message to send.
      * @param      const std::string& reason
      *             The reason the process failed.
      *             Used when sending audit messages if a process fails.
      * @param      bool isManualArchive
      *             Only used for CopyArchive messages.
      *             True if the archive was automatically generated.
      *             False if it was manually generated.
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1) The AuditMessageSender could not be created.
      *             2) The entity key of the Archive Manager could not be determined.
      *             3) The operator name or workstation name could not be determined.
      */

    void sendAuditMessage( const TA_Base_Core::MessageType& messageType, const std::string& reason = "",
        bool isManualArchive = false );

    /**
      * sendAuditMessageIgnoreErrors
      *
      * Calls sendAuditMessage() but catches any exceptions thrown and ignores them.
      * This should ONLY be used when there is absolutely nothing we can do if the sending fails.
      *
      * @param      const TA_Base_Core::MessageType& messageType  
      *             The type of message to send.
      * @param      const std::string& reason
      *             The reason the process failed.
      *             Used when sending audit messages if a process fails.
      * @param      bool isManualArchive
      *             Only used for CopyArchive messages.
      *             True if the archive was automatically generated.
      *             False if it was manually generated.
      */

    void sendAuditMessageIgnoreErrors( const TA_Base_Core::MessageType& messageType, 
        const std::string& reason = "", bool isManualArchive = false );

    ///////////////////////////////////////////////////////////////////////////
    //
    // Determining rights.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
      * getRightsLibrary
      *
      * Returns the rights library used for all rights checking.
      *
      * @return  The rights library.
      */

    TA_Base_Bus::RightsLibrary* getRightsLibrary();

    /**
      * isActionPermitted
      *
      * Used to determine if the operator has the rights to perform the given action.
      *
      * @param      unsigned long actionKey
      *             The key identifying the action - from
      *             "bus/security/access_control/actions/src/AccessControlledActions.h"
      *
      * @exception  ArchiveException
      *             Thrown if:
      *             1) The resource key for the ArchiveManager cannot be found.
      *             2) The operators rights cannot be determined.
      */

    bool isActionPermitted( unsigned long actionKey );

    ///////////////////////////////////////////////////////////////////////////
    //
    // Checking file/drive/directory validity.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
      * isValidDirectory
      *
      * Verifies that the given directory exists.
      *
      * @return     bool
      *             True => the directory exists.
      *             False otherwise.
      * @param      const std::string& directory
      *             The directory to check for (incl. the full path).
      */

    bool isValidDirectory( const std::string& directory );

    /**
      * isValidFile
      *
      * Verifies that the given file exists.
      *
      * @return     bool
      *             True => the file exists.
      *             False otherwise.
      * @param      const std::string& file
      *             The file to check for (incl. the full path).
      */

    bool isValidFile( const std::string& file );

    /** 
      * getMediaLocations
      *
      * Retrieves all drive letters that are suitable for storing archives.
      *
      * @return  A vector of drives.
      */

    std::vector<std::string> getMediaLocations();


    ///////////////////////////////////////////////////////////////////////////
    //
    // Processing Log Files
    //
    ///////////////////////////////////////////////////////////////////////////


    // Enum for the tokens used when parsing the export log files.

    enum EErrorStatus
    {
        UNKNOWN       = 0,  // None of the other options
	    EX_SUCC       = 1,  // Export terminated successfully without warnings
	    EX_OKWARN     = 2,  // Export terminated successfully with warnings
	    EX_FAIL       = 3   // Export terminated unsuccessfully
    };
    typedef std::multimap< EErrorStatus, std::string > FileStatusMap;


    /** 
      * processFile
      *
      * Processes the given file to see if it contains any errors or warnings.
      * The strings that the files are parsed for assume that the Oracle 
      * imp.exe or exp.exe have been used to generate the files. I.e. if you
      * are using anything other than these programs to import or export,
      * the strings to parse for may be invalid.  It also assumes only one
      * of the message types will appear in the file.
      *
      * @return     EErrorStatus
      *             Specifies whether any warnings or errors occurred.
      *
      * @param      const std::string& file
      *             The name of the file including the full path.
      * @param      bool isImporting
      *             True if importing, false if exporting.
      *
      * @exception  ArchiveException
      *             Thrown if the file could not be opened.
      */

    TA_ArchiveManager::EErrorStatus processFile( const std::string& file, bool isImporting );
     
    /**
      * deleteLogFiles
      *
      * Deletes the specified log files.
      *
      * @param      const TA_ArchiveManager::FileStatusMap& files
      *             The files to be deleted.  These will include the full path.
      *
      * @exception  ArchiveException
      *             Thrown if any of the files could not be deleted.
      *             The exception message will contain a list of files
      *             that could not be deleted along with the reason
      *             for the failure.
      */

    void deleteLogFiles( const TA_ArchiveManager::FileStatusMap& files ); 


    ///////////////////////////////////////////////////////////////////////////
    //
    // Reading from registry
    //
    ///////////////////////////////////////////////////////////////////////////

    /** 
      * getOracleHomeDirectory
      *
      * Retrieves from the Windows registry Oracle's home directory
      *
      * @return The path name as a string.
      */

    std::string getOracleHomeDirectory();

	 /** 
	  * getVersionNumber
	  *
	  * Returns the Version number to be included in import/expot dump files,
	  * to avoid importing older version of dump to database.
	  *
	  * @return The version number as string
	  */
	std::string getVersionNumber();

	 /** 
      * Trim
      *
      * Trim leading and trailing whitespace
      *
      * @return trimmed string.
      */
	std::string Trim(std::string inputString);

	/** 
	  * getStringFromMediaStatus
	  *
	  * Returns the Media status in string format
	  *
	  * @param The MediaStatus enum
	  *
	  * @return The MediaStatus in CString format
	  */
	CString getStringFromMediaStatus(TA_Base_App::MediaStatus mediaStatus);

} // TA_ArchiveManager

#endif // BasicHelper_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
