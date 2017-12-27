/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Katherine Thomson
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Performs back end work for the Archive Manager restoration process.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 4503 ) 
#endif // defined( WIN32 )

#include <memory>
#include <algorithm>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/WriteArchiveHelper.h"
#include "app/data_management/archive_manager/src/WriteArchiveDlg.h"
#include "app/data_management/archive_manager/src/DateRangeDlg.h"
#include "app/data_management/archive_manager/src/RetrievingDataDlg.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/resource.h"

#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/database/src/SimpleDb.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ArchiveManagerEntityData.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DateFormat.h" // TD12474 
using namespace TA_Base_Core;
namespace TA_Base_App
{
	// Assumes the maximum capacity of a DVD disk to be Mode 5 single side, 
	// single layer as 4.7 GB.
    const unsigned short WriteArchiveHelper::MAX_CD_SIZE_IN_MB = 640;
	unsigned short WriteArchiveHelper::MAX_DVD_SIZE_IN_MB = 4700;


    // Path of Oracle export program relative to Oracle home directory.
    const std::string WriteArchiveHelper::EXPORT_PROGRAM = "\\bin\\exp.exe";

    //
    // This can be used as a command line parameter if you don't want the export process
    // to log to the default file name (which is based on the DEBUGFILE_RUNPARAM ).
    //

    const std::string WriteArchiveHelper::EXPORT_LOG_FILE_RUNPARAM( "ExportLogFile" );

	const std::string WriteArchiveHelper::CD_ARCHIVE("CdArchive" );

    //
    // WriteArchiveHelper
    //
    WriteArchiveHelper::WriteArchiveHelper( TA_Base_Bus::IGUIAccess& guiAccessor )
        : m_guiAccessor( guiAccessor ),
          m_exportLogFileName( "" )
    {
		m_dvdvolume = MAX_DVD_SIZE_IN_MB;
        m_genericExpCommand.str("");
        
        // We may want the logging to go to a separate file from our debug log file,
        // allow this to be set on the command line.

        m_exportLogFileName = TA_Base_Core::RunParams::getInstance().get( EXPORT_LOG_FILE_RUNPARAM.c_str() );
        if ( m_exportLogFileName.empty() )
        {
            m_exportLogFileName = TA_Base_Core::RunParams::getInstance().get( RPARAM_DEBUGFILE );
        }
        if ( !m_exportLogFileName.empty() )
        {
            // Strip off .log if necessary and add _E (for export).

            unsigned int lastDot = m_exportLogFileName.rfind( ".log" );
            if ( lastDot == m_exportLogFileName.size() - 4 )
            {
                m_exportLogFileName.erase( lastDot, 4 );
            }
            m_exportLogFileName += "_E";    // E => export
        }
        else
        {
            m_exportLogFileName = "Archive_E";
        }
    }


    //
    // writeScheduledArchives
    //
    void WriteArchiveHelper::writeScheduledArchives( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel )
    {
        messageForUser = "";
        messageLevel = TA_ArchiveManager::ERR;

		/*
        // Send an audit message.

        try
        {
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveRequest ); 
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            messageForUser = e.what();
            return;
        }
		*/

        // Check that the user has permission.

        try
        {
            if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_TRANSFER ) )
            {
                CString error;
                try
                {
                    error.FormatMessage( UE050079_INSUF_PRIV_TRANSFER,
                        TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                        TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
                }
                catch( TA_Base_Core::ArchiveException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                    error.LoadString( UE050123_INSUF_PRIV_TRANSFER );
                }
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                messageForUser = error;
                return;
            }
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
            messageForUser = e.what();
            return;
        }

        // Open the dialog for writing scheduled archives to removable media.

        WriteArchiveDlg dlg( m_guiAccessor );
        int status = dlg.DoModal();

        // Process the returned status.

        if ( IDOK == status )
        {
			/*
            // Send an audit message.

            try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveCopied ); 
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                messageForUser = e.what();
                messageLevel = TA_ArchiveManager::WARN;
                return;
            }
			*/
			
        }
        else if ( IDCANCEL == status )
        {
            CString message;
            try
            {
                message.FormatMessage( UI050030_WRITE_ABORTED, 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UI050060_TRANSFER_ABORTED );
            }

            // Send an audit message.

            try
            {
               TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveCancelled );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            }
            messageForUser = std::string( message );
            messageLevel = TA_ArchiveManager::INFO;
            return;
        }

        // If the copy process fails at any stage, the return value will be IDABORT,
        // an audit message will be sent and a message will be displayed to the user, 
        // so there will not be anything left to do here.
    }


    //
    // createManualArchive
    //
    void WriteArchiveHelper::createManualArchive( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel )
    {
        messageForUser = "";
        messageLevel = TA_ArchiveManager::ERR;

		/*
        // Send an audit message.

        try
        {
            // True => isManualArchive
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveRequest, "", true ); 
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            messageForUser = e.what();
            return;
        }
		*/

        // Check that the user has permission.

        try
        {
            if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_MANUAL ) )
            {
                CString error;
                try
                {
                    error.FormatMessage( UE050021_INSUF_PRIV_MAN_ARCHIVE,
                        TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                        TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
                }
                catch( TA_Base_Core::ArchiveException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                    error.LoadString( UE050124_INSUF_PRIV_MAN_ARCHIVE );
                }
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                messageForUser = error;
                return;
            }
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            messageForUser = e.what();
            return;
        }
        
        // Get the date range.
		setDvdVolumeNum();
		DateRangeDlg rangeDlg;
		rangeDlg.setDvdVolume(m_dvdvolume);

		// ++yangguang TD16713

        int status = rangeDlg.DoModal();
        if ( IDOK == status )
        {
            // Get the dates that were selected in the DateRangeDlg.

            time_t fromDate( rangeDlg.getSelectedFromDate() );
            time_t toDate( rangeDlg.getSelectedToDate() );

            // Get the data for that date range.
        
            RetrievingDataDlg retDlg( m_guiAccessor, fromDate, toDate );
            status = retDlg.DoModal();
            if ( IDOK == status )
            {
                // The toDate may have changed if the retrieval process was stopped partway through,
                // so get the toDate from the RetrievingDataDlg.
                
                retDlg.getToDate();

                // Write the data to CD.

                WriteArchiveDlg writeDlg( m_guiAccessor, fromDate, toDate );
                status = writeDlg.DoModal();
            }
        }
		// liborm 1/07/2004 - fix for TD defect #3511
		// dialogue is closed and control returns to Archive Manager
		else if ( IDCANCEL == status )
        {
			// marvin++
			// TD13245
			CString message;
            try
            {
                message.FormatMessage( UI050030_WRITE_ABORTED, 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UI050060_TRANSFER_ABORTED );
            }

            // Send an audit message.

            /*try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveCancelled );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            }*/
			
            messageForUser = std::string( message );
            messageLevel = TA_ArchiveManager::INFO;
			// TD13245
			// ++marvin

			return;
		}

        // Process the returned status.

        if ( IDOK == status )
        {
			/*
            // Send an audit message.

            try
            {
				// True => isManualArchive
				TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveCopied, "", true ); 
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                messageForUser = e.what();
                messageLevel = TA_ArchiveManager::WARN;
                return;
            }
			*/
        }
        else if ( IDCANCEL == status )
        {
            // If the user cancelled at any stage, send an audit message.

            CString message;
            try
            {
                message.FormatMessage( UI050030_WRITE_ABORTED, 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UI050060_TRANSFER_ABORTED );
            }

            // Send an audit message.

            /*try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveCancelled );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            }*/
			
            messageForUser = std::string( message );
            messageLevel = TA_ArchiveManager::INFO;
            return;
        }

        // If the copy process fails at any stage, the return value will be IDABORT,
        // an audit message will be sent and a message will be displayed to the user, 
        // so there will not be anything left to do here.
    }
	void WriteArchiveHelper::setDvdVolumeNum()
	{
	        // yangguang++ TD16713
			TA_Base_Core::IEntityData* data = m_guiAccessor.getGUIEntity();
			if ( 0 == data )
	        {
	             CString reason;
	             reason.LoadString( UE050104_REASON_ENTITY_DATA );
	             CString error;
	             error.FormatMessage( UE050104_PATH_ERROR, reason );
	             TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
	        }
		
	        TA_Base_Core::ArchiveManagerEntityData* amData = dynamic_cast< TA_Base_Core::ArchiveManagerEntityData* >( data );
	        if ( 0 == amData )
	        {
	             CString reason;
	             reason.LoadString( UE050104_REASON_CAST_FAILED );
	             CString error;
	             error.FormatMessage( UE050104_PATH_ERROR, reason );
	             TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
	        }

			//16713++
			int m_dvdvolume = atoi(amData->getVolumeofDVD().c_str());
			std::string dvdpath = amData->getManualArchiveDirectory();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[XF] dvd volume is : %i", m_dvdvolume);
			//++16713

			WriteArchiveHelper::setMaxDvdSize(m_dvdvolume);
	}


    //
    // retrieveDataForDate
    //
    void WriteArchiveHelper::retrieveDataForDate( const time_t& date, 
        TA_ArchiveManager::FileStatusMap& fileStatusMap, FilePathsVector& archiveFiles )
    {
        FUNCTION_ENTRY( "retrieveFilesForDate" );
        
        TA_Base_Core::ArchiveTableAccessFactory::IArchiveTableVector tables;
        TA_Base_Core::ArchiveTableAccessFactory::IArchiveTableVector::iterator tableIt;
        try
        {
            // Get the tables to export.
    
            tables = TA_Base_Core::ArchiveTableAccessFactory::getInstance().getAllArchiveTables( false );
            if ( tables.empty() )
            {
                CString message;
                message.FormatMessage( UI050049_NO_TABLES, TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME.c_str() );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
            }

            // Load the generic part of the export command.  This will return immediately
            // if the details have already been loaded.
        
            loadExportCommand();

            std::string reverseDateStr;
            std::stringstream exportCommand;
            std::stringstream logFile;
            std::stringstream archiveFile;
            TA_ArchiveManager::EErrorStatus errorStatus = TA_ArchiveManager::UNKNOWN;
            for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
            {
                reverseDateStr = TA_ArchiveManager::convertTimeTToFormattedString( date );
                archiveFile.str( "" );
                archiveFile << (getArchiveDirectory(true))[0] << "\\M" << reverseDateStr;
                archiveFile << "_" << (*tableIt)->getFilePostfix() << TA_ArchiveManager::getVersionNumber() << ".dmp"; // The M is for manual.
                
                // Fill in the details for the export command.
                // This is the format of the export command.
                // "c:\oracle\ora92\bin\exp" $USER/$USER@$SID tables=$TABLE feedback=5000 
                //     log=c:\transactive\log\Log_ArchiveManager_export_2003_04_03_$FILEPOSTFIX.log
                //     query=\\\"where $QUERY_COLUMN like trunc(to_date('$ARCHIVE_DATE','YYYY-MM-DD'))" 
                //     and file=M$ARCHIVE_DATE-$FILEPOSTFIX_V2.dmp

                exportCommand.str( "" );
                exportCommand << m_genericExpCommand.str();
                exportCommand << "tables=" << (*tableIt)->getName() << " ";
                exportCommand << "feedback=5000 ";
                if ( !m_exportLogFileName.empty() )
                {
                    logFile.str("");
                    logFile << m_exportLogFileName << "_" << reverseDateStr;
                    logFile << "_" << (*tableIt)->getFilePostfix() << ".log";
                    exportCommand << " log=" << logFile.str();
                }
				// liborm - 27 Oct 2004 - Fix for TD #4683
				// turn off statistics to stop warning 'EXP-00091'

				// TD12474 ++
				/*exportCommand << " statistics=none ";
                exportCommand << " query=\\\"where " << (*tableIt)->getQueryColumn();
                exportCommand << " like trunc(to_date('" << reverseDateStr;
                exportCommand << "','" << TA_ArchiveManager::REVERSE_DATE_FORMAT << "'))\\\" ";
                exportCommand << "file=" << archiveFile.str();*/

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat3);
				// ++ TD12474

                exportCommand << " statistics=none ";
                exportCommand << " query=\\\"where " << (*tableIt)->getQueryColumn();
				//date should not use like
                //exportCommand << " like trunc(to_date('" << reverseDateStr;
                //exportCommand << "','" << dateFormatStr << "'))\\\" ";
				exportCommand << ">=to_date('" << reverseDateStr;
				exportCommand << "','" << dateFormatStr<<"') and "<<(*tableIt)->getQueryColumn();
				exportCommand <<" < (TO_date('" << reverseDateStr<< "','" << dateFormatStr<<"') + interval '1' day "<< ")\\\" ";
                exportCommand << "file=" << archiveFile.str();
				// ++ TD12474 

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, exportCommand.str().c_str() );

                archiveFiles.push_back( archiveFile.str() );

                // Export the data.

                STARTUPINFO StartupInfo = {0};
                StartupInfo.cb = sizeof(StartupInfo);
                PROCESS_INFORMATION ProcessInformation;
                BOOL res = CreateProcess(NULL,
                                         CString( exportCommand.str().c_str() ).GetBuffer(0),
                                         NULL,
                                         NULL, 
                                         FALSE,
                                         IDLE_PRIORITY_CLASS | DETACHED_PROCESS, 
                                         NULL, 
                                         NULL, 
                                         &StartupInfo,
                                         &ProcessInformation );

                // Clean up after the export.

                if (res == TRUE)
                {
                    WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
                    CloseHandle(ProcessInformation.hProcess);
                }
                else
                {
                    LPVOID lpMsgBuf;
                    FormatMessage( 
                        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                        FORMAT_MESSAGE_FROM_SYSTEM | 
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL 
                    );

                    CString error;
                    error.FormatMessage( UE050100_RETRIEVE_FAILED, 
                        TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), 
                        std::string( (LPCTSTR)lpMsgBuf ).c_str() );
                    LocalFree( lpMsgBuf );
                    
                    for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
                    {
                        delete *tableIt;
                        *tableIt = 0;
                    }

                    TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
                }

                // Process the log file for this export.

                try
                {
                    errorStatus = TA_ArchiveManager::processFile( logFile.str(), false );   // False => Exporting
                    fileStatusMap.insert( std::make_pair( errorStatus, logFile.str() ) );
                }
                catch( TA_Base_Core::ArchiveException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                    fileStatusMap.insert( std::make_pair( TA_ArchiveManager::UNKNOWN, logFile.str() ) );
                }
            }
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );

            for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
            {
                delete *tableIt;
                *tableIt = 0;
            }

            CString error;
            error.FormatMessage( UE050100_RETRIEVE_FAILED, 
                TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );

            for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
            {
                delete *tableIt;
                *tableIt = 0;
            }

            CString error;
            error.FormatMessage( UE050100_RETRIEVE_FAILED, 
                TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
        {
            delete *tableIt;
            *tableIt = 0;
        }

        FUNCTION_EXIT;
    }


    //
    // getAllFileDetails
    //
    WriteArchiveHelper::FileDetailsVector WriteArchiveHelper::getAllFileDetails( 
        const std::string& archiveDirectory, const time_t& fromDate,
        const time_t& toDate, bool limitDateRange )
    { 
        // KT 9 Mar 04: PW #3161: Converted to use time_t to reduce dependency on date format.

        TA_ASSERT( TA_ArchiveManager::isValidDirectory( archiveDirectory ), "The archive directory is invalid." );
        TA_ASSERT( fromDate <= toDate, "The fromDate must be equal to or before the toDate." );

        // Look for files that fit the given wildcard string.
        // This could throw an ArchiveException!

        std::string strWildcard = "";
        if( limitDateRange )        // The date range is only limited for manual archives.
        {
            strWildcard = archiveDirectory + "\\M*.dmp"; 
        }
        else
        {
            strWildcard = archiveDirectory + "\\A*.dmp"; 
        }
            
        // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.
        
        std::string logMsg( "Looking for archive files like:" + strWildcard );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logMsg.c_str() );
        LOGMORE( SourceInfo, "The file names should be in the format: AYYYY_MM_DD_filepostfix[_V2].dmp or MYYYY_MM_DD_filepostfix[_V2].dmp." );

        CFileFind finder;
        FileDetailsVector fileDetailsVector;
        FileDetails details;

        BOOL areMoreFiles = finder.FindFile( strWildcard.c_str() );
        while( areMoreFiles )
        {
            areMoreFiles = finder.FindNextFile();
            
            if ( finder.IsDots() || finder.IsDirectory() )
            {
                continue;
            }

            // Extract the date from the file name then add the date and file details into the map.
            // File name format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp.
            // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.
        
            try
            {
                details.date = TA_ArchiveManager::getDateFromFileName( std::string( finder.GetFileName() ) );   
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                continue;
            }

            // If we're looking for files in a given date range, make sure it
            // is in the range.

            if ( limitDateRange )
            {
                if ( ( details.date < fromDate ) || ( details.date > toDate ) )
                {
                    continue;
                }
            }

            details.name = finder.GetFileName();
            details.nameWithPath = finder.GetFilePath();
            // details.sizeInMB = finder.GetLength64() / TA_ArchiveManager::MEGABYTE;
			details.sizeInMB = finder.GetLength() / TA_ArchiveManager::MEGABYTE;

            fileDetailsVector.push_back( details );
        }
        finder.Close();

        // Sort the details by date.

        std::sort( fileDetailsVector.begin(), fileDetailsVector.end(), FileDetails() ); 

        return fileDetailsVector;
    }


    //
    // getAllFileDetails
    //
    WriteArchiveHelper::FileDetailsVector WriteArchiveHelper::getAllFileDetails(
        const std::string& archiveDirectory )
    { 
        // KT 9 Mar 04: PW #3161: Converted to use time_t to reduce dependency on date format.

        time_t fromDate;
        time_t toDate;
        return getAllFileDetails( archiveDirectory, fromDate, toDate, false );
    }


    //
    // splitIntoCds
    //
    WriteArchiveHelper::CdsMap WriteArchiveHelper::splitIntoCds( const FileDetailsVector& fileDetails )
    {
        TA_ASSERT( !fileDetails.empty(), "There are no files to split up." );

        // The process used here is not the most efficient way, but it is the most maintainable.
        // The files will be grouped by date and then split into CD groups.

        // Group all the files by date.
   
        FilesForDateVector allFilesByDate;
        groupFilesByDate( fileDetails, allFilesByDate );
        
        TA_ASSERT( !allFilesByDate.empty(), "There are no files to split into CDs." );

        // Now group the dates into CDs following the rules:
        //
        // 1.	Archive files shall be stored in chronological order.
        // 2.	The files for more than one date can be stored on a single CD.
        // 3.	All of the files for a single date shall be stored on the same CD if their combined size
        //      is less than the capacity of a CD.
        // 4.	If the files for a single date will not all fit on a single CD, they shall be put on 
        //      multiple consecutive CDs.  The CD labels shall be labelled with the date and the part number.  
        //      So if the files for 01/01/2004 fit on two CDs the labels would be:  
        //      "01/01/2004 Part 1 of 2" and "01/01/2004 Part 2 of 2".  
        // 5.	A CD shall only contain a complete set of files for one or more dates, 
        //      or a partial set of files for a single date.  There shall never be a partial set of files 
        //      for a single date on a CD that contains files for another date.
        // 6.	If a single archive file exists that is too big to fit on a single CD, an error shall be 
        //      presented to the operator and the process aborted.  It is expected that a single archive 
        //      file will always be small enough to fit on a single CD.

        CdsMap cdsMap;
        double usedSizeInMB = 0;
        FilesForDateVector::iterator firstSetOfFilesForCdIt = allFilesByDate.begin();
        FilesForDateVector::iterator lastSetOfFilesForCdIt = allFilesByDate.begin();
		if( !TA_Base_Core::RunParams::getInstance().isSet(CD_ARCHIVE.c_str()))	// Using DVD
		{
			FilesForDateVector::iterator currentSetOfFilesIt = allFilesByDate.begin();
			for( ; currentSetOfFilesIt != allFilesByDate.end(); ++currentSetOfFilesIt )
			{
				// First determine if we have reached the maximum number of files that fit on this CD.
				// Will all the files for this date fit on this CD?

				if ( MAX_DVD_SIZE_IN_MB < usedSizeInMB + (*currentSetOfFilesIt).sizeInMB )
				{
					// No. The files for this date don't all fit on the current CD.
					// Are there already dates on the CD?

					if ( usedSizeInMB != 0 )
					{
						// Yes.  This means we've reached the maximum number of files that we can put on this CD.
						// Close off the current CD.

						createCd( allFilesByDate, firstSetOfFilesForCdIt, lastSetOfFilesForCdIt, cdsMap );

						// Reset the details used to keep track of the latest CD we are creating.

						usedSizeInMB = 0;
						firstSetOfFilesForCdIt = currentSetOfFilesIt;
						lastSetOfFilesForCdIt = currentSetOfFilesIt; 
					}
				}

				// Now either:
				// 1. The current date fits on the existing CD.
				// 2. The current date needs to be split up onto multiple CDs.

				// Will the current date fit on the existing CD?

				if ( MAX_DVD_SIZE_IN_MB >= (*currentSetOfFilesIt).sizeInMB )
				{
					// Yes. Just increase the used size by the size of the current date's files.

					usedSizeInMB += (*currentSetOfFilesIt).sizeInMB;
					lastSetOfFilesForCdIt = currentSetOfFilesIt;
				}
				else
				{
					// No. This date on its own will take up multiple CDs, so the
					// files need splitting up.

					createCdsForSingleDate( (*currentSetOfFilesIt).fileDetails, cdsMap );

					// Reset the details used to keep track of the latest CD we are creating.

					usedSizeInMB = 0;
					firstSetOfFilesForCdIt = currentSetOfFilesIt;
					lastSetOfFilesForCdIt = currentSetOfFilesIt; 
				}
			}
			// Once we get to here, we need to add everything that is left to a final CD.

			TA_ASSERT( currentSetOfFilesIt == allFilesByDate.end(), "This is not the last set of files." ); 
			currentSetOfFilesIt--;
			createCd( allFilesByDate, firstSetOfFilesForCdIt, currentSetOfFilesIt, cdsMap );

			return cdsMap;
        }
		else	// Using CD
		{
			FilesForDateVector::iterator currentSetOfFilesIt = allFilesByDate.begin();
			for( ; currentSetOfFilesIt != allFilesByDate.end(); ++currentSetOfFilesIt )
			{
				// First determine if we have reached the maximum number of files that fit on this CD.
				// Will all the files for this date fit on this CD?

				if ( MAX_CD_SIZE_IN_MB < usedSizeInMB + (*currentSetOfFilesIt).sizeInMB )
				{
					// No. The files for this date don't all fit on the current CD.
					// Are there already dates on the CD?

					if ( usedSizeInMB != 0 )
					{
						// Yes.  This means we've reached the maximum number of files that we can put on this CD.
						// Close off the current CD.

						createCd( allFilesByDate, firstSetOfFilesForCdIt, lastSetOfFilesForCdIt, cdsMap );

						// Reset the details used to keep track of the latest CD we are creating.

						usedSizeInMB = 0;
						firstSetOfFilesForCdIt = currentSetOfFilesIt;
						lastSetOfFilesForCdIt = currentSetOfFilesIt; 
					}
				}

				// Now either:
				// 1. The current date fits on the existing CD.
				// 2. The current date needs to be split up onto multiple CDs.

				// Will the current date fit on the existing CD?

				if ( MAX_CD_SIZE_IN_MB >= (*currentSetOfFilesIt).sizeInMB )
				{
					// Yes. Just increase the used size by the size of the current date's files.

					usedSizeInMB += (*currentSetOfFilesIt).sizeInMB;
					lastSetOfFilesForCdIt = currentSetOfFilesIt;
				}
				else
				{
					// No. This date on its own will take up multiple CDs, so the
					// files need splitting up.

					createCdsForSingleDate( (*currentSetOfFilesIt).fileDetails, cdsMap );

					// Reset the details used to keep track of the latest CD we are creating.

					usedSizeInMB = 0;
					firstSetOfFilesForCdIt = currentSetOfFilesIt;
					lastSetOfFilesForCdIt = currentSetOfFilesIt; 
				}
			}
	        // Once we get to here, we need to add everything that is left to a final CD.

			TA_ASSERT( currentSetOfFilesIt == allFilesByDate.end(), "This is not the last set of files." ); 
			currentSetOfFilesIt--;
			createCd( allFilesByDate, firstSetOfFilesForCdIt, currentSetOfFilesIt, cdsMap );

			return cdsMap;
		}
    }


    //
    // groupFilesByDate
    //
    void WriteArchiveHelper::groupFilesByDate( const FileDetailsVector& fileDetails, FilesForDateVector& allFilesByDate )
    {
        // Group all the files by date.
   
        FilesForDate theFilesForDate;
        time_t currentDate;
		if( !TA_Base_Core::RunParams::getInstance().isSet(CD_ARCHIVE.c_str()))	// Using DVD
		{
			for( FileDetailsVector::const_iterator it = fileDetails.begin(); it != fileDetails.end(); )
			{
				currentDate = (*it).date;

				theFilesForDate.sizeInMB = 0;
				theFilesForDate.fileDetails.clear();
				theFilesForDate.date = (*it).date;

				// Get all the files for a particular date.

				while( ( fileDetails.end() != it ) && ( currentDate == (*it).date ) )
				{
					if ( MAX_DVD_SIZE_IN_MB < (*it).sizeInMB )
					{
						CString error;
						error.FormatMessage( UE050102_FILE_TOO_BIG, (*it).name.c_str(), (*it).sizeInMB, "DVD" );
						TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
					}
                
					theFilesForDate.sizeInMB += (*it).sizeInMB;
					theFilesForDate.fileDetails.push_back( *it );
					++it;
				}

				// Add the files for the current date to the vector.

				allFilesByDate.push_back( theFilesForDate );
			}   
		}
		else	// Using CD
		{
			for( FileDetailsVector::const_iterator it = fileDetails.begin(); it != fileDetails.end(); )
			{
				currentDate = (*it).date;

				theFilesForDate.sizeInMB = 0;
				theFilesForDate.fileDetails.clear();
				theFilesForDate.date = (*it).date;

				// Get all the files for a particular date.

				while( ( fileDetails.end() != it ) && ( currentDate == (*it).date ) )
				{
					if ( MAX_CD_SIZE_IN_MB < (*it).sizeInMB )
					{
						CString error;
						error.FormatMessage( UE050102_FILE_TOO_BIG, (*it).name.c_str(), (*it).sizeInMB, "CD" );
						TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
					}
                
					theFilesForDate.sizeInMB += (*it).sizeInMB;
					theFilesForDate.fileDetails.push_back( *it );
					++it;
				}

				// Add the files for the current date to the vector.

				allFilesByDate.push_back( theFilesForDate );
			}   
		}
    }


    //
    // createCd
    //
    void WriteArchiveHelper::createCd( const FilesForDateVector& allFilesByDate, 
        const FilesForDateVector::iterator& firstSetOfFilesForCdIt, 
        const FilesForDateVector::iterator& lastSetOfFilesForCdIt,
        CdsMap& cdsMap )
    {
        TA_ASSERT( firstSetOfFilesForCdIt != allFilesByDate.end(), "The first iterator points to the end of the vector." );
        TA_ASSERT( lastSetOfFilesForCdIt != allFilesByDate.end(), "The last iterator points to the end of the vector." );

        // Create the volume label and the CD label (more compact than the volume label). Format:
        // 1. <single date>                 and YYYYMMDD
        // 2. <first date> to <last date>   and YYYYMMDDYYYYMMDD
        //
        // KT 9 Mar 04: PW #3161 Changed to use time_t to reduce dependency on date format.
        // KT 10 Mar 04: PW #3136 Added reformatted volume label for CD (CDLabel).

        std::string volumeLabel( "" );
        std::string cdLabel( "" );
        //volumeLabel = TA_ArchiveManager::convertTimeTToStringDate( (*firstSetOfFilesForCdIt).date );
		volumeLabel = TA_ArchiveManager::convertTimeTToDDMMYYString( (*firstSetOfFilesForCdIt).date );
        cdLabel = TA_ArchiveManager::convertTimeTToYYMMDDString( (*firstSetOfFilesForCdIt).date );
        if ( firstSetOfFilesForCdIt != lastSetOfFilesForCdIt )
        {
			// TD19034
            volumeLabel += " to ";
            //volumeLabel += TA_ArchiveManager::convertTimeTToStringDate( (*lastSetOfFilesForCdIt).date );
			volumeLabel += TA_ArchiveManager::convertTimeTToDDMMYYString( (*lastSetOfFilesForCdIt).date );
            cdLabel += TA_ArchiveManager::convertTimeTToYYMMDDString( (*lastSetOfFilesForCdIt).date );
        }

        // Add all the files for each date in the range defined by the given iterators.
        // The lastSetOfFilesForCdIt iterator will always point to the last set of
        // files that need to be added, so don't forget to get them too!

        FileDetailsVector filesForCD;
		FilesForDateVector::iterator it = firstSetOfFilesForCdIt;
        for( ; it != lastSetOfFilesForCdIt; ++it )
        {
            filesForCD.insert( filesForCD.end(), (*it).fileDetails.begin(), (*it).fileDetails.end() );
        }
        filesForCD.insert( filesForCD.end(), (*it).fileDetails.begin(), (*it).fileDetails.end() );
    
        // Add the new CD info to the set of CDs.

        cdsMap[ volumeLabel ] = std::make_pair( cdLabel, filesForCD );
    }


    //
    // createCdsForSingleDate
    //
    void WriteArchiveHelper::createCdsForSingleDate( const FileDetailsVector& fileDetailsForDate, CdsMap& cdsMap )
    {
        // Find out how many CDs will be required for this date.

        unsigned short numCdsForDate = getNumberOfCdsForDate( fileDetailsForDate );
        
        // Go through and group the files for this date into CDs.
        // The volume label and CD label (more compact version) will include the date, 
        // the index of this CD in the set for this date and how many CDs are in the set for this date.
        
        // KT 10 Mar 04: PW #3136 Added reformatted volume label for CD (CDLabel).

        std::string dateInLocalFormat( TA_ArchiveManager::convertTimeTToStringDate( fileDetailsForDate.begin()->date ) );
        std::string dateInYYMMDDFormat( TA_ArchiveManager::convertTimeTToYYMMDDString( fileDetailsForDate.begin()->date ) );
        std::stringstream volumeLabel;      // Format: <date> Part <index> of <numCds>
        std::stringstream cdLabel;          // Format: YYYYMMDDIINN
        FileDetailsVector filesForCD;
        double usedSizeInMB = 0;
        unsigned short index = 1;
		if( !TA_Base_Core::RunParams::getInstance().isSet(CD_ARCHIVE.c_str()))	// Using DVD
		{
			for( FileDetailsVector::const_iterator aFileIt = fileDetailsForDate.begin(); aFileIt != fileDetailsForDate.end(); ++aFileIt )
			{
				// TD19038
				TA_ASSERT( MAX_DVD_SIZE_IN_MB >= (*aFileIt).sizeInMB, "The file is too big to fit on a single DVD." );  // Already checked up further.
				TA_ASSERT( index <= numCdsForDate, "There required number of CDs is wrong." );

				// Will this file fit on the current CD?

				if ( MAX_DVD_SIZE_IN_MB >= usedSizeInMB + (*aFileIt).sizeInMB )
				{
					// Yes. Add this file to the CD.

					filesForCD.push_back( *aFileIt );
					usedSizeInMB += (*aFileIt).sizeInMB;
				}
				else
				{
					// No. Add the current set of files to the CD and close of that CD.
                
					volumeLabel.str( "" );
					cdLabel.str( "" );
					volumeLabel << dateInLocalFormat << " Part " << index << " of " << numCdsForDate;
					cdLabel << dateInYYMMDDFormat;
					if ( index < 10 )
					{
						cdLabel << "0";
					}
					cdLabel << index;
					if ( numCdsForDate < 10 )
					{
						cdLabel << "0";
					}
					cdLabel << numCdsForDate;

					cdsMap[ volumeLabel.str() ] = std::make_pair( cdLabel.str(), filesForCD );
             
					// Set everything up for the next CD.

					index++;
					filesForCD.clear();
					usedSizeInMB = 0;

					// Now add the current file to the new CD.

					filesForCD.push_back( *aFileIt );
					usedSizeInMB += (*aFileIt).sizeInMB;
				}
			}

		}
		else	// Using CD
		{
			for( FileDetailsVector::const_iterator aFileIt = fileDetailsForDate.begin(); aFileIt != fileDetailsForDate.end(); ++aFileIt )
			{
				TA_ASSERT( MAX_CD_SIZE_IN_MB >= (*aFileIt).sizeInMB, "The file is too big to fit on a single CD." );  // Already checked up further.
				TA_ASSERT( index <= numCdsForDate, "There required number of CDs is wrong." );

				// Will this file fit on the current CD?

				if ( MAX_CD_SIZE_IN_MB >= usedSizeInMB + (*aFileIt).sizeInMB )
				{
					// Yes. Add this file to the CD.

					filesForCD.push_back( *aFileIt );
					usedSizeInMB += (*aFileIt).sizeInMB;
				}
				else
				{
					// No. Add the current set of files to the CD and close of that CD.
                
					volumeLabel.str( "" );
					cdLabel.str( "" );
					volumeLabel << dateInLocalFormat << " Part " << index << " of " << numCdsForDate;
					cdLabel << dateInYYMMDDFormat;
					if ( index < 10 )
					{
						cdLabel << "0";
					}
					cdLabel << index;
					if ( numCdsForDate < 10 )
					{
						cdLabel << "0";
					}
					cdLabel << numCdsForDate;

					cdsMap[ volumeLabel.str() ] = std::make_pair( cdLabel.str(), filesForCD );
             
					// Set everything up for the next CD.

					index++;
					filesForCD.clear();
					usedSizeInMB = 0;

					// Now add the current file to the new CD.

					filesForCD.push_back( *aFileIt );
					usedSizeInMB += (*aFileIt).sizeInMB;
				}
			}
		}
    }


    //
    // getNumberOfCdsForDate
    //
    unsigned short WriteArchiveHelper::getNumberOfCdsForDate( const FileDetailsVector& fileDetailsForDate )
    {
        if ( fileDetailsForDate.empty() )
        {
            return 0;
        }

        unsigned short numCds = 1;
        double usedSizeInMB = 0;

        for( FileDetailsVector::const_iterator it = fileDetailsForDate.begin(); it != fileDetailsForDate.end(); ++it )
        {
            // Will this file fit on the current CD?

            if ( MAX_CD_SIZE_IN_MB >= usedSizeInMB + (*it).sizeInMB )
            {
                // Yes.

                usedSizeInMB += (*it).sizeInMB;
            }
            else
            {
                // No.  Okay then we're going to need another CD.

                numCds++;
                usedSizeInMB = 0;
            }
        }
        return numCds;
    }


    //
    // deleteArchiveFiles
    //
    void WriteArchiveHelper::deleteArchiveFiles( const FileDetailsVector& files )
    {
        std::stringstream error;
        for( WriteArchiveHelper::FileDetailsVector::const_iterator it = files.begin(); it != files.end(); ++it )  
        {
            if( !::DeleteFile( (*it).nameWithPath.c_str() ) )
            {
                // If an error occurs while deleting, add the filename and the reason to
                // the message and then continue processing the rest of the files.

                LPVOID lpMsgBuf;
                FormatMessage( 
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM | 
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL 
                );
                
                error << "\n" << (*it).nameWithPath << " : " << std::string( (LPCTSTR)lpMsgBuf );
                LocalFree( lpMsgBuf );
            }
        }

        if( !error.str().empty() )
        {
            CString errorStr;
            errorStr.FormatMessage( UW050002_DELETE_FILES_FAILED, error.str().c_str() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( errorStr ) ) );
        }
    }
 
    
    //
    // deleteArchiveFiles
    //
    void WriteArchiveHelper::deleteArchiveFiles( const FilePathsVector& files )
    {
        std::stringstream error;
        for( WriteArchiveHelper::FilePathsVector::const_iterator it = files.begin(); it != files.end(); ++it )  
        {
            if( !::DeleteFile( (*it).c_str() ) )
            {
                // If an error occurs while deleting, add the filename and the reason to
                // the message and then continue processing the rest of the files.

                LPVOID lpMsgBuf;
                FormatMessage( 
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM | 
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL 
                );
                
                error << "\n" << (*it) << " : " << std::string( (LPCTSTR)lpMsgBuf );
                LocalFree( lpMsgBuf );
            }
        }

        if( !error.str().empty() )
        {
            CString errorStr;
            errorStr.FormatMessage( UW050002_DELETE_FILES_FAILED, error.str().c_str() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( errorStr ) ) );
        }
    }

   
    //
    // getArchiveDirectory
    //
    std::vector<std::string> WriteArchiveHelper::getArchiveDirectory( bool isManualArchive )
    {
        std::string archiveDirectory( "" );
        try
        {
            TA_Base_Core::IEntityData* data = m_guiAccessor.getGUIEntity();
            if ( 0 == data )
            {
                CString reason;
                reason.LoadString( UE050104_REASON_ENTITY_DATA );
                CString error;
                error.FormatMessage( UE050104_PATH_ERROR, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            TA_Base_Core::ArchiveManagerEntityData* amData = dynamic_cast< TA_Base_Core::ArchiveManagerEntityData* >( data );
            if ( 0 == amData )
            {
                CString reason;
                reason.LoadString( UE050104_REASON_CAST_FAILED );
                CString error;
                error.FormatMessage( UE050104_PATH_ERROR, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        
            if ( isManualArchive )
            {
                archiveDirectory = amData->getManualArchiveDirectory();
            }
            else
            {
                archiveDirectory = amData->getAutoArchiveDirectory();
				
            }
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException." );
            CString error;
            error.FormatMessage( UE050104_PATH_ERROR, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException." );
            CString error;
            error.FormatMessage( UE050104_PATH_ERROR, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        if ( archiveDirectory.empty() )
        {
            CString reason;
            if ( isManualArchive )
            {
                reason.LoadString( UE050104_REASON_MAN_PARAM_NOT_SET );
            }
            else
            {
                reason.LoadString( UE050104_REASON_AUTO_PARAM_NOT_SET );
            } 
            CString error;
            error.FormatMessage( UE050104_PATH_ERROR, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        
        // Break up the archive directory into multiple directories if they exist
        std::vector<std::string> archiveDirectories;
        
        std::string::size_type separatorPos = archiveDirectory.find(",");
		while (separatorPos != std::string::npos)
        {
            // Extract the first string
            std::string singleDir = archiveDirectory.substr(0,separatorPos);
            archiveDirectories.push_back(singleDir);
            
            // Update the archiveDirectory to remove the first string and separator
            archiveDirectory = archiveDirectory.substr(separatorPos+1, archiveDirectory.size() - separatorPos);

            // Now search again
            separatorPos = archiveDirectory.find(",");
        }

        archiveDirectories.push_back(archiveDirectory);


        // Now check each directory
        for(unsigned int i = 0; i < archiveDirectories.size(); ++i)
        {
            if ( !TA_ArchiveManager::isValidDirectory( archiveDirectories[i] ) )
            {
                CString reason;
                if ( isManualArchive )
                {
                    reason.FormatMessage( UE050104_REASON_INVALID_MAN_DIR, archiveDirectories[i].c_str() );
                }
                else
                {
                    reason.FormatMessage( UE050104_REASON_INVALID_AUTO_DIR, archiveDirectories[i].c_str() );
                }
                CString error;
                error.FormatMessage( UE050104_PATH_ERROR, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }
        
        return archiveDirectories;
    }
   

    //
    // getExportLogFileName
    //
    std::string WriteArchiveHelper::getExportLogFileName()
    {
        return m_exportLogFileName;
    }


    //
    // loadExportCommand
    //
    void WriteArchiveHelper::loadExportCommand()
    {
        // If it is not empty, it has already been loaded.

        if ( !m_genericExpCommand.str().empty() )
        {
            return;
        }

        // Build up the export command string.
        // The specific details (eg. table, query, date etc) will be added on during the export phase.

        m_genericExpCommand.str("");
        try
        {
            TA_Base_Core::IEntityData* data = m_guiAccessor.getGUIEntity();
            if ( 0 == data )
            {
                CString reason;
                reason.LoadString( UE050101_REASON_ENTITY_DATA );
                CString error;
                error.FormatMessage( UE050101_EXP_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            TA_Base_Core::ArchiveManagerEntityData* amData = dynamic_cast< TA_Base_Core::ArchiveManagerEntityData* >( data );
            if ( 0 == amData )
            {
                CString reason;
                reason.LoadString( UE050101_REASON_CAST_FAILED  );
                CString error;
                error.FormatMessage( UE050101_EXP_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            
            std::string homeDir = TA_ArchiveManager::getOracleHomeDirectory();
            std::string exp = homeDir + EXPORT_PROGRAM;

            if ( homeDir.empty() )
            {
                CString reason;
                reason.LoadString( UE050101_REASON_ORACLE_HOME_UNKNOWN );
                CString error;
                error.FormatMessage( UE050101_EXP_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            if( !TA_ArchiveManager::isValidFile( exp ) )
            {
                CString reason;
                reason.FormatMessage( UE050101_REASON_INVALID_EXP, exp.c_str() );
                CString error;
                error.FormatMessage( UE050101_EXP_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            TA_ArchiveManager::replaceBackslashes( exp );

            // Add quotations marks around the executable to avoid security issues when running CreateProcess.
            // If we don't add the quotes it is possible for the wrong program to be run if there are spaces
            // in the path for the exe.
            
            m_genericExpCommand << "\"" << exp << "\" ";
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
           
            m_genericExpCommand.str("");
            
            CString error;
            error.FormatMessage( UE050101_EXP_LOAD_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
           
            m_genericExpCommand.str("");
            
            CString error;
            error.FormatMessage( UE050101_EXP_LOAD_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        
        // Get the database details from the RunParams.

        //std::string dbConnectionString( TA_Base_Core::RunParams::getInstance().get( RPARAM_DBCONNECTION ) );
		std::string dbConnectionString;
		TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::Archiving_Cd, TA_Base_Core::Read, dbConnectionString);
        std::vector<std::string> components;
        SimpleDb::ParseConnectStr( dbConnectionString,	components );
        if ( 4 > components.size() || components[1].empty() 
			|| components[2].empty() || components[3].empty())
        {
            m_genericExpCommand.str("");
            CString reason;
            reason.LoadString( UE050101_REASON_INVALID_DB );
            CString error;
            error.FormatMessage( UE050101_EXP_LOAD_FAILED, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        m_genericExpCommand << components[2] << "/" << components[3] << "@" << components[1] << " ";
		m_genericExpCommand << "INDEXES=N ";
		m_genericExpCommand << "CONSTRAINTS=N ";
        m_genericExpCommand << "GRANTS=N ";
		m_genericExpCommand << "TRIGGERS=N ";	

        
        std::string logMsg( "Export command minus details: " + m_genericExpCommand.str() );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logMsg.c_str() );
    }

	void WriteArchiveHelper::setMaxDvdSize(unsigned short dvdSize)
	{
		MAX_DVD_SIZE_IN_MB = dvdSize;
	}

} // TA_Base_App
