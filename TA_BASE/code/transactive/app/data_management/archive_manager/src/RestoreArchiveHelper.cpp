/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RestoreArchiveHelper.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Performs back end work for the Archive Manager restoration process.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 ) 
#endif // defined( WIN32 )

#include <memory>
#include <algorithm>
#include <sstream>
#include <afx.h>
// #include <fstream.h>
#include <fstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/RestoreArchiveHelper.h"
#include "app/data_management/archive_manager/src/RestoreArchiveDlg.h"
#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core/database/src/SimpleDb.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h"
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

using namespace TA_Base_Core;
namespace TA_Base_App
{
    //
    // This can be used as a command line parameter if you don't want the import process
    // to log to the default file name (which is based on the DEBUGFILE_RUNPARAM ).
    //

    const std::string RestoreArchiveHelper::IMPORT_LOG_FILE_RUNPARAM( "ImportLogFile" );

    //
    // Path of Oracle import program relative to Oracle home directory.
    //

    const std::string RestoreArchiveHelper::IMPORT_PROGRAM = "\\bin\\imp.exe";

    //
    // Constant specifying the file type of archives
    //
    
	const std::string RestoreArchiveHelper::ARCHIVE_FILE_WILDCARD = "*"+TA_ArchiveManager::getVersionNumber()+".dmp";

    // 
    // Static constants defining the state of the archives for a particular date.
    //
    /*      
    const std::string RestoreArchiveHelper::RESTORABLE     ( "Restorable" );
    const std::string RestoreArchiveHelper::EXISTS         ( "Exists"     );
    const std::string RestoreArchiveHelper::UNKNOWN        ( "Unknown"    );
    const std::string RestoreArchiveHelper::IMPORTING      ( "Importing"  );
    const std::string RestoreArchiveHelper::COMPLETE       ( "Complete"   );
    const std::string RestoreArchiveHelper::RESTORE_FAILED ( "Failed"     );
    const std::string RestoreArchiveHelper::CANCELLED      ( "Cancelled"  );
	*/
	const std::string RestoreArchiveHelper::RESTORABLE     ( "可恢复" );
	const std::string RestoreArchiveHelper::EXISTS         ( "已存在"     );
	const std::string RestoreArchiveHelper::UNKNOWN        ( "未知"    );
	const std::string RestoreArchiveHelper::IMPORTING      ( "正导入"  );
	const std::string RestoreArchiveHelper::COMPLETE       ( "完成"   );
	const std::string RestoreArchiveHelper::RESTORE_FAILED ( "失败"     );
	const std::string RestoreArchiveHelper::CANCELLED      ( "取消"  );
	const std::string RestoreArchiveHelper::NOTPROCESSED      ( ""  );

    //
    // RestoreArchiveHelper
    //
    RestoreArchiveHelper::RestoreArchiveHelper( TA_Base_Bus::IGUIAccess& guiAccessor )
        : m_guiAccessor( guiAccessor ),
          m_importLogFileName( "" ),
		  hasInvalidArchive(false),
          m_archiveOnlineEntry( NULL ),
          m_archiveInventory( NULL ),
          m_archiveTables( NULL )
    {
        m_genericImpCommand.str("");

        // We may want the logging to go to a separate file from our debug log file,
        // allow this to be set on the command line.

        m_importLogFileName = TA_Base_Core::RunParams::getInstance().get( IMPORT_LOG_FILE_RUNPARAM.c_str() );
        if ( m_importLogFileName.empty() )
        {
            m_importLogFileName = TA_Base_Core::RunParams::getInstance().get( RPARAM_DEBUGFILE );
        }
        if ( !m_importLogFileName.empty() )
        {
            // Strip off .log if necessary and add _I (for import).

            unsigned int lastDot = m_importLogFileName.rfind( ".log" );
            if ( lastDot == m_importLogFileName.size() - 4 )
            {
                m_importLogFileName.erase( lastDot, 4 );
            }
            m_importLogFileName += "_I";        // I => import
        }
        else
        {
            m_importLogFileName = "Archive_I";
        }
    }


    //
    // restoreArchive
    //
    void RestoreArchiveHelper::restoreArchives( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel )
    {
        messageForUser = "";
        messageLevel = TA_ArchiveManager::ERR;

        // Send an audit message.

        try
        {
            if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_RESTORE ) )
            {
                // UE-050034
                CString error;
                try
                {
                    error.FormatMessage( UE050034_INSUF_PRIV_RESTORE,
                        TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                        TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
                }
                catch( TA_Base_Core::ArchiveException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                    error.LoadString( UE050122_INSUF_PRIV_RESTORE );
                }
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::RestoreArchiveFailed, std::string( error ) ); 
                messageForUser = std::string( error );
                return;
            }
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::RestoreArchiveFailed, e.what() ); 
            messageForUser = e.what();
            return;
        }

        // Open the dialog for restoring archives from removable media.

        RestoreArchiveDlg dlg( m_guiAccessor );
        int status = dlg.DoModal();
        if ( IDOK == status )
        {
            // Send an audit message.

            try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveRestored ); 
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                messageForUser = e.what();
                messageLevel = TA_ArchiveManager::WARN;
                return;
            }
        }
        else if ( IDCANCEL == status )
        {
            CString message;
            try
            {
                message.FormatMessage( UI050045_RESTORE_ABORTED, 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UI050062_RESTORE_ABORTED );
            }


            /*try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::RestoreArchiveCancelled );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            }*/
			
            messageForUser = std::string( message );
            messageLevel = TA_ArchiveManager::INFO;
            return;
        }

        // If the restoration process fails at any stage, the return value will be IDABORT,
        // an audit message will be sent and a message will be displayed to the user, 
        // so there will not be anything left to do here.
    }


    //
    // getAllFileDetails
    //
    RestoreArchiveHelper::FileDetailsVector RestoreArchiveHelper::getAllFileDetails(const std::string& drive)
    {        
        // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.

        // Look for files that fit the given wildcard string.

        std::string strWildcard( drive + "\\" + ARCHIVE_FILE_WILDCARD ); 
        std::string logMsg( "Looking for files like: " + strWildcard );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logMsg.c_str() );
        LOGMORE( SourceInfo, "The file names should be in the format: AYYYY_MM_DD_filepostfix_V2.dmp or MYYYY_MM_DD_filepostfix_V2.dmp." );

        CFileFind finder;
        FileDetailsVector fileDetailsVector;
        FileDetailsVector::iterator it;
        FileDetails details;

        retrieveConfiguration();

        m_archiveFiles.clear();

        BOOL areMoreFiles = finder.FindFile( strWildcard.c_str() );
        while( areMoreFiles )
        {
            areMoreFiles = finder.FindNextFile();
            
            if ( finder.IsDots() || finder.IsDirectory() )
            {
                continue;
            }

			//TD19031
			std::string fileName = finder.GetFileName();
			std::string fullpath = drive + "\\" + fileName;
			bool isValidData = false;
			//TD19031
            // Extract the date from the file name then add the date and file details into the map.
            // File name format AYYYY_MM_DD_filepostfix_V2.dmp or MYYYY_MM_DD_filepostfix_V2.dmp.
            
            try
            {
				// TD16705
				//TD19031
				std::ifstream ifs(fullpath.c_str());
				
				char ch[20];
				memset(ch,0,20);
				ifs.getline(ch,20);
				if(strstr(ch,"TEXPORT"))
				{
					memset(ch,0,20);
					ifs.getline(ch,2); 
					if(ch[0]=='U')
						isValidData = true;					
				}
				
				ifs.close();
				//TD19031

				// CL-16719 ++
				//Refer to WriterArchiveHelper.retrieveDataForDate, the .dmp
				// name format is MYYYY_MM_DD_postfix.dmp or AYYYY_MM_DD_postfix.dmp
				// change this...
               //details.date = TA_ArchiveManager::getDateFromFileNameDDMMYY( fileName );
			   details.date = TA_ArchiveManager::getDateFromFileName( fileName );
				// ++ CL-16719
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
				//TD19031
				if(isValidData == false)
					invalidArchiveFileVector.push_back(fileName);
                continue;
            }

            details.status = getArchiveStatusForDate( details.date );

            // All the files for a given date must be restored at the same time, so
            // each date is only inserted once into the fileDetailsVector.

            it = fileDetailsVector.begin();
            while( ( fileDetailsVector.end() != it ) && ( details.date != it->date ) )
            {
                ++it;
            }
            if ( fileDetailsVector.end() == it )
            {
                fileDetailsVector.push_back( details );
            }

            // All files are inserted into this map, because when we restore we
            // need to know all the files for the given date.

            m_archiveFiles.insert( std::make_pair( details.date, finder.GetFilePath() ) );
        }
        finder.Close();

        std::sort( fileDetailsVector.begin(), fileDetailsVector.end(), FileDetails() ); 
		if((!fileDetailsVector.empty()) && (!invalidArchiveFileVector.empty()))
		{
			hasInvalidArchive = true;       
            
            TA_Base_Bus::TransActiveMessage userMsg;
          
            UINT selectedButton = userMsg.showMsgBox( IDS_UW_050001,CAPTION_ARCHIVE_MANAGER);
		}

        return fileDetailsVector;
    }


    //
    // restoreFilesForDate
    //
    void RestoreArchiveHelper::restoreFilesForDate( const time_t& date, const std::string& volumeLabel, 
        TA_ArchiveManager::FileStatusMap& fileStatusMap )
    {   
        FUNCTION_ENTRY( "restoreFilesForDate" );

        // KT 2 Mar 04: PW #3133 Update the AR_INVENTORY table with this date.  
        // Then restore the files for this date.

        updateInventory( date, volumeLabel );

        loadImportCommand();

        // Find the archive files relating to the given date and import them.

        std::pair< DateToFileMap::iterator, DateToFileMap::iterator > range( m_archiveFiles.equal_range( date ) ); 

        std::stringstream importCommand;
        std::stringstream logFile;
        TA_ArchiveManager::EErrorStatus errorStatus = TA_ArchiveManager::UNKNOWN;
        for( DateToFileMap::iterator it = range.first; it != range.second; ++it )
        {
            importCommand.str( "" );
            importCommand << m_genericImpCommand.str();
            if ( !m_importLogFileName.empty() )
            {
                logFile.str("");
                logFile << m_importLogFileName << "_";
                logFile << TA_ArchiveManager::convertTimeTToFormattedString( date ) << "_";
                logFile << TA_ArchiveManager::getPostfixFromFileName( it->second ) << ".log ";
                importCommand << " log=" << logFile.str();
            }
            importCommand << " file=" << it->second;
            
            // Import the data.

            STARTUPINFO StartupInfo = {0};
            StartupInfo.cb = sizeof(StartupInfo);
            PROCESS_INFORMATION ProcessInformation;
            BOOL res = CreateProcess(NULL,
                                     CString( importCommand.str().c_str() ).GetBuffer(0),
                                     NULL,
                                     NULL, 
                                     FALSE,
                                     IDLE_PRIORITY_CLASS | DETACHED_PROCESS, 
                                     NULL, 
                                     NULL, 
                                     &StartupInfo,
                                     &ProcessInformation );

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
                error.FormatMessage( UE050093_RESTORE_FAILED, 
                    TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(),
                    std::string( (LPCTSTR)lpMsgBuf ).c_str() );
                LocalFree( lpMsgBuf );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            // Process the log file to see if the import was successful or not.

            try
            {
                errorStatus = TA_ArchiveManager::processFile( logFile.str(), true );    // True => Importing
                fileStatusMap.insert( std::make_pair( errorStatus, logFile.str() ) );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                fileStatusMap.insert( std::make_pair( TA_ArchiveManager::UNKNOWN, logFile.str() ) );
            }
        }

        FUNCTION_EXIT;
    }


    //
    // updateInventory
    //
    void RestoreArchiveHelper::updateInventory( const time_t& date, const std::string& volumeLabel )
    {
        FUNCTION_ENTRY( "updateInventory" );

        // KT 27 Feb 04: PW #3133: This method has been changed so that users can remove
        // an individual days worth of restored data from the database.
        // It now inserts an entry into the AR_INVENTORY table for the given day, instead of
        // for a date range.

        // Set up the inventory item. These fields will be the same for all archives.

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItem inventoryItem;
        inventoryItem.dataDate = date;
        inventoryItem.volumeLabel = volumeLabel;
        inventoryItem.isDeleting = false;
        time( &inventoryItem.restoredDate );
        
        // Get the operator name from the session id. This may throw an exception,
        // but it is documented in the comment for this method.

        inventoryItem.restoredBy = TA_ArchiveManager::getOperatorNameFromSessionId();

        try
        {
            std::auto_ptr< TA_Base_Core::IArchiveOnline > archiveOnlineEntry( 
                TA_Base_Core::ArchiveOnlineAccessFactory::getInstance().getArchiveOnlineEntry() );
            if( 0 == archiveOnlineEntry.get() )
            {
                CString reason;
                reason.LoadString( UE050094_REASON_INV_FAILED );
                CString error;
                error.FormatMessage( UE050094_UPDATE_FAILED, 
                    TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME.c_str(), reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            // Try and get the archive inventory dai object.

            std::auto_ptr< TA_Base_Core::IArchiveInventory > inventory( 
                    TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory() );
            if( 0 == inventory.get() )
            {
                // If we can't get the inventory object, we can't add the new items.
            
                CString reason;
                reason.LoadString( UE050094_REASON_INV_FAILED );
                CString error;
                error.FormatMessage( UE050094_UPDATE_FAILED, 
                    TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME.c_str(), reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            time_t currentTime;
            time( &currentTime );
            inventoryItem.deletionDueDate = currentTime + 
                archiveOnlineEntry->getDataExpiryInDays() * TA_ArchiveManager::ONE_DAY;
            inventory->addArchiveInventoryItem( inventoryItem );
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050094_UPDATE_FAILED, 
                TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME.c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050094_UPDATE_FAILED, 
                TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME.c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        
        FUNCTION_EXIT;
    }


    //
    // loadImportCommand
    //
    void RestoreArchiveHelper::loadImportCommand()
    {
        // If it is not empty, it has already been loaded.

        if ( !m_genericImpCommand.str().empty() )
        {
            return;
        }

        // Build up the import command string.  This is an example string:
        // "c:\\oracle\\ora92\\bin\\imp.exe SYSTEM/MANAGER@TRANSACT fromuser=TRA touser=TRA ignore=Y log=DebugFile.log file="
        // The file will be added on during the import phase.

        m_genericImpCommand.str("");

		//To Store the fromUser and ToUser value from database
		std::string fromUser( "" );
		std::string toUser( "" );

        try
        {
            TA_Base_Core::IEntityData* data = m_guiAccessor.getGUIEntity();
            if ( 0 == data )
            {
                CString reason;
                reason.LoadString( UE050095_REASON_ENTITY_DATA );
                CString error;
                error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            TA_Base_Core::ArchiveManagerEntityData* amData = dynamic_cast< TA_Base_Core::ArchiveManagerEntityData* >( data );
            if ( 0 == amData )
            {
                CString reason;
                reason.LoadString( UE050095_REASON_CAST_FAILED );
                CString error;
                error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            
            std::string homeDir = TA_ArchiveManager::getOracleHomeDirectory();
            std::string imp = homeDir + IMPORT_PROGRAM;

            if ( homeDir.empty() )
            {
                CString reason;
                reason.LoadString( UE050101_REASON_ORACLE_HOME_UNKNOWN );
                CString error;
                error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            if( !TA_ArchiveManager::isValidFile( imp ) )
            {
                CString reason;
                reason.FormatMessage( UE050095_REASON_INVALID_IMP, imp.c_str() );
                CString error;
                error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            m_genericImpCommand << TA_ArchiveManager::replaceBackslashes( imp ) << " ";

			//Fetch the fromUser and toUser value from database and
			//Trim leading and trailing whitespaces
			fromUser = TA_ArchiveManager::Trim(amData->getImpFromUser());
			toUser = TA_ArchiveManager::Trim(amData->getImpToUser());

			std::string logMsg( "FromUser=" + fromUser + " ToUser=" + toUser );
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,logMsg.c_str() );

        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            m_genericImpCommand.str("");
            CString error;
            error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            m_genericImpCommand.str("");
            CString error;
            error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        // Get the database details from the RunParams.
        
        //std::string dbConnectionString( TA_Base_Core::RunParams::getInstance().get( RPARAM_DBCONNECTION ) );

		std::string dbConnectionString;
		TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::Archiving_Cd, TA_Base_Core::Write, dbConnectionString);

        std::vector<std::string> components;
        SimpleDb::ParseConnectStr( dbConnectionString,	components );
        if ( 4 > components.size() || components[1].empty() 
			|| components[2].empty() || components[3].empty() )
        {
            m_genericImpCommand.str("");
            CString reason;
            reason.FormatMessage( UE050095_REASON_INVALID_DB, dbConnectionString.c_str() );
            CString error;
            error.FormatMessage( UE050095_IMP_CMD_LOAD_FAILED, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
		
		//if fromUser or toUser is empty use the username from databaseConnection string
		//else use fromUser or toUser
        m_genericImpCommand << components[2] << "/" << components[3] << "@" << components[1] << " ";
        m_genericImpCommand << "fromuser=" << (fromUser.empty()? components[2] : fromUser) << " ";
        m_genericImpCommand << "touser=" <<  (toUser.empty()? components[2] : toUser) << " ";
        m_genericImpCommand << "ignore=Y ";
		m_genericImpCommand << "INDEXES=N ";
		m_genericImpCommand << "CONSTRAINTS=N ";
        m_genericImpCommand << "GRANTS=N ";

		if( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_IMPBUFFERSIZE))
		{
			m_genericImpCommand << "buffer="<< TA_Base_Core::RunParams::getInstance().get(RPARAM_IMPBUFFERSIZE)<<" ";
		}
        
        std::string logMsg( "Import command minus details: " + m_genericImpCommand.str() );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logMsg.c_str() );
    }


    //
    // getMediaLocations
    //
    std::vector<std::string> RestoreArchiveHelper::getMediaLocations()
    {
        // Get all CD-ROM/DVD drives
        typedef std::vector<std::string> DriveVector;
        DriveVector drives = TA_ArchiveManager::getMediaLocations();

        DriveVector archiveDrives;

        // Check which have archive files
        CFileFind finder;
        for (DriveVector::iterator iter = drives.begin(); iter != drives.end(); iter++)
        {
                std::string archiveFiles = (*iter) + "\\" + ARCHIVE_FILE_WILDCARD;
                if (finder.FindFile(archiveFiles.c_str()))
                {
                    archiveDrives.push_back(*iter);
                }
        }

        return archiveDrives;
    }


    //
    // getArchiveStatusForDate
    //
    std::string RestoreArchiveHelper::getArchiveStatusForDate( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161: Converted to use time_t to reduce dependence on date formatting.

        // Get the status of the archive for this date. The data cannot be restored if:
        // 1. The archive has an entry in the AR_INVENTORY table.
        // 2. The archive date is within the online period for the given tables.

        try
        {
            // KT 27 Feb 04: PW #3135: The Online Period has been moved from AR_TABLES
            // to AR_ONLINE.

            // Check if the archive date is within the last ONLINE_PERIOD days.
            // If so we can't restore the data, because it hasn't been deleted from the
            // system yet.

            if( 0 == m_archiveOnlineEntry.get() )
            {
                return UNKNOWN;
            }
            else
            {
                time_t currentTime;
                time( &currentTime );
                /*
                 *  grantt: 18 June 2004 - Unit test defect #60.
                 *
                 *  Fix: Ensures number of seconds is stored in numDaysSinceArchiveDate - not minutes (i.e. 24 * 60).
                 */ 
                long numDaysSinceArchiveDate = ( currentTime - date ) / ( 24 * 60 * 60);        // 24 * 60 * 60 to change secs into days.
                if( m_archiveOnlineEntry->getOnlinePeriodInDays() >= numDaysSinceArchiveDate )
                {
                    return EXISTS;
                }
            }

            // Check if the data has already been restored.

            if( 0 == m_archiveInventory.get() )
            {
                return UNKNOWN;
            }
            else if ( m_archiveInventory->isDateInInventory( date ) )
            {
                return EXISTS;
            }

            
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString error;
            error.FormatMessage( UE050098_UNKNOWN_STATUS, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            return UNKNOWN;
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString error;
            error.FormatMessage( UE050098_UNKNOWN_STATUS, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            return UNKNOWN;
        }

        return NOTPROCESSED;
    }


	std::string RestoreArchiveHelper::getArchiveStatusFromTables( const time_t& date )
    {
		// Get the status of the archive for this date. The data cannot be restored if:
        // The database contains data that has the archive date in its query column (see AR_TABLES)
		try
		{
			// Check if the database contains tables that have the archive date in the query column (see AR_TABLES)
            // for any of the table's data.
			std::vector< TA_Base_Core::IArchiveTable* >::iterator it;
            for( it = m_archiveTables.begin(); it != m_archiveTables.end(); it++ )
            {
                if( (*it)->isDateInQueryColumn( date ) )
                {
                    return EXISTS;
                }
            }
		}
		catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString error;
            error.FormatMessage( UE050098_UNKNOWN_STATUS, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            return UNKNOWN;
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString error;
            error.FormatMessage( UE050098_UNKNOWN_STATUS, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            return UNKNOWN;
        }
		return RESTORABLE;
	}


    void RestoreArchiveHelper::retrieveConfiguration()
    {
        try
        {
            m_archiveOnlineEntry = std::auto_ptr< TA_Base_Core::IArchiveOnline >
                ( TA_Base_Core::ArchiveOnlineAccessFactory::getInstance().getArchiveOnlineEntry() );
            m_archiveInventory = std::auto_ptr< TA_Base_Core::IArchiveInventory >
                ( TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory() );
            m_archiveTables = TA_Base_Core::ArchiveTableAccessFactory::getInstance().getAllArchiveTables( false );
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
//             CString error = UE050136_DATABASE_ERROR;
			std::ostringstream error;
			error << UE050136_DATABASE_ERROR;
            TA_THROW( TA_Base_Core::ArchiveException( error.str() ) );
        }        
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
         //    CString error = UE050136_DATABASE_ERROR;
			std::ostringstream error;
			error << UE050136_DATABASE_ERROR;
            TA_THROW( TA_Base_Core::ArchiveException( error.str() ) );
        }
    }

	bool RestoreArchiveHelper::hasInvalidArchiveFile()
	{
		return this->hasInvalidArchive;
	}

} // TA_Base_App
