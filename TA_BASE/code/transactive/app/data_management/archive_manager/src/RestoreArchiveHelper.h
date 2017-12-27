#if !defined(RESTORE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_)
#define RESTORE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RestoreArchiveHelper.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Performs back end work for the Archive Manager restoration process.
  */

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h"


// Forward declaration

namespace TA_Base_Core
{
    class IArchiveOnline;
    class IArchiveTable;
}

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
    class RestoreArchiveHelper
    {
    public:
        //
        // This can be used as a command line parameter if you don't want the import process
        // to log to the default file name (which is based on the DEBUGFILE_RUNPARAM ).
        //

        static const std::string IMPORT_LOG_FILE_RUNPARAM;

        //
        // Oracle import program
        //

        static const std::string IMPORT_PROGRAM;

        //
        // Used to find archived data.
        //

        static const std::string ARCHIVE_FILE_WILDCARD;


        // 
        // Static constants defining the state of the archives for a particular date.
        //
        
        static const std::string RESTORABLE;
        static const std::string EXISTS;
        static const std::string UNKNOWN;
        static const std::string IMPORTING;
        static const std::string COMPLETE;
        static const std::string RESTORE_FAILED;
        static const std::string CANCELLED;
		static const std::string NOTPROCESSED;

        struct FileDetails
        {
            time_t date;
            std::string status;

            // Used to sort the FileDetailsVector.

            bool operator()( FileDetails& details1, FileDetails& details2 )
            {
                return details1.date < details2.date;
            }
        };
        
        typedef std::vector< FileDetails > FileDetailsVector;
        typedef std::vector< time_t > DatesVector;

		typedef std::vector< std::string > InvalidArchiveFile;
        /**
          * RestoreArchiveHelper
          *
          * Standard constructor.
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             The object used to access the ArchiveManager entity data.
          */

        RestoreArchiveHelper( TA_Base_Bus::IGUIAccess& guiAccessor );

        /**
          * ~RestoreArchiveHelper
          *
          * Standard destructor.
          */

        virtual ~RestoreArchiveHelper() {};

        /** 
          * restoreArchives
          *
          * Performs the restore archives process:
          * 1. Sends the required audit messages.
          * 2. Checks the users privileges.
          * 3. Opens the RestoreArchiveDlg.
          * 4. Processes the returned status from that dialog.
          *
          * @param      std::string& messageForUser
          *             Will be populated with the message to display to the user.
          *             If empty, no message is required.
          * @param      TA_ArchiveManager::EMsgLevel& messageLevel
          *             Will be populated with the level to display the message at.
          */

        void restoreArchives( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel );

		 /*
		  * @return     true    there are invalid files in the disk
          *             
          *             
       
          */

		bool hasInvalidArchiveFile();

        /**
          * getAllFileDetails
          *
          * Gets the dates and status' for all the files on the given drive. The file
          * names must be in the format AYYYY_MM_DD_filepostfix_V2.dmp or 
          * MYYYY_MM_DD_filepostfix_V2.dmp otherwise they will be ignored.
          *
          * All the files for a given date must be restored at the same time, so
          * each date is only returned once with the status for all the files for that date.
          *
          * @param      drive  The drive
          *
          * @return     FileDetailsVector
          *             The dates and status' of the files matching the
          *             given format on the CD. These are sorted in date order.
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The path of the CD Burner is empty.
          */
        
        FileDetailsVector getAllFileDetails(const std::string& drive);

        /**
          * restoreFilesForDate
          *
          * Restores the archive files for a given date using the imp.exe
          * program for oracle.
          *
          * @param      const time_t& date
          *             The date to restore.
          *             KT 9 Mar 04: PW #3161 Changed to remove dependence on date formatting.
          * @param      const std::string& volumeLabel
          *             The CD volume label.
          * @param      TA_ArchiveManager::FileStatusMap& fileStatusMap
          *             The log files that contain errors or warnings
          *             will be added to this map, so they can
          *             be displayed to the user.
          *
          * @exception  TA_Base_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ImportProgram entity parameter cannot be retrieved or is empty.
          *             4) The DbConnection RunParam is not set or in the format DB:USERNAME:PASSWORD.
          *             5) The import program cannot be found.
          *             6) The audit message cannot be sent.
          *             7) The user does not have sufficient privileges.
          */

        void restoreFilesForDate( const time_t& date, const std::string& volumeLabel, 
            TA_ArchiveManager::FileStatusMap& fileStatusMap );

        /**
          * getImportLogFileName
          *
          * Gets the name of the file (inc. path) that the import program logs to.
          *
          * @return     std::string
          *             The file name & path.
          */

        std::string getImportLogFileName() { return m_importLogFileName; };

        /**
          * getMediaLocations
          *
          * Returns a vector of string containing all drives that have archive files available
          * for restoring.
          *
          * @return A vector of drive letters.
          */

        std::vector<std::string> getMediaLocations();

		
		/**
          * getArchiveStatusFromTables
          *
          * Determines whether the data for the given date can be restored.
          * The data cannot be restored if:
          * The database contains data that has the given date in its query column (see AR_TABLES)
		  *  The Single status check method is spilted into two - inorder to improve performance.
          *
          * @return     std::string
          *             The status of the archive for the given date.
          *
          * @param      const time_t& date
          *             The date to check the status for.
          */
		std::string getArchiveStatusFromTables( const time_t& date );

 

    private:

        //
        // Disable copy constructor and assignment operator.
        //

		RestoreArchiveHelper( const RestoreArchiveHelper& theArchiveHelper );
		RestoreArchiveHelper& operator=( const RestoreArchiveHelper& );
    
        /**
          * updateInventory
          *
          * Updates the AR_INVENTORY table in the database with the details
          * of the given date that have been restored.
          *
          * Precondition:   TA_ArchiveManager::isDateInCorrectFormat( date )
          *
          * @param      const time_t& date
          *             The date that has been restored.
          * @param      const std::string& volumeLabel
          *             The CD volume label.
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The ArchiveInventory DAI object cannot be obtained.
          *             2) The operator name cannot be determined from the session id.
          */

        void updateInventory( const time_t& date, const std::string& volumeLabel );

 
        /**
          * loadImportCommand
          *
          * Loads the information for the import command from the database
          * and the RunParams into m_genericImpCommand.  Note that the
          * file to import is not loaded, because that will be determined 
          * during the restore phase.
          *
          * This method will be called from restoreFilesForDate() each time, 
          * but it will return immediately if the command has already been loaded.
          *
          * @exception  TA_Base_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ImpProgramPath entity parameter cannot be retrieved or is empty.
          *             4) The DbConnection RunParam is not set or in the format DB:USERNAME:PASSWORD.
          */

        void loadImportCommand();


        /** 
          * retrieveConfiguration
          *
          * Retrieves configuration information from the AR_INVENTORY, AR_ONLINE, and AR_TABLES tables.
          *
          * @exception TA_Base_Core::ArchiveException  Thrown if an error occurs retrieving data from the
          *                                            database.
          */
        void retrieveConfiguration();

        /**
          * getArchiveStatusForDate
          *
          * Determines whether the data for the given date can be restored.
          * The data cannot be restored if:
          * 1. There is an entry in the AR_INVENTORY table for the given date.
          * 2. The database contains data that has the given date in its query column (see AR_TABLES)
          * 3. The date is within the online period for the given tables.
		  * In this method we check only 1 & 3 condition and 2 condition is checked in another method to
		  * improve performance.
          *
          * @return     std::string
          *             The status of the archive for the given date.
          *
          * @param      const time_t& date
          *             The date to check the status for.
          *             KT 9 Mar 04: PW #3161 Changed to remove dependence on date formatting.
          */

        std::string getArchiveStatusForDate( const time_t& date );

        //
        // The object used to access the ArchiveManager entity data.
        //

        TA_Base_Bus::IGUIAccess& m_guiAccessor;

        //
        // The archive files on the CD.
        //

        typedef std::multimap< time_t, std::string > DateToFileMap;
        DateToFileMap m_archiveFiles;

        //
        // This is the import command to use.  It will be filled out with all the
        // database details.
        //

        std::stringstream m_genericImpCommand;

        //
        // The name of the file (inc. path) that the import program logs to.
        //

        std::string m_importLogFileName;

        //
        // Configuration retrieved from the database
        //

        std::auto_ptr< TA_Base_Core::IArchiveOnline > m_archiveOnlineEntry;
        std::auto_ptr< TA_Base_Core::IArchiveInventory > m_archiveInventory;
        std::vector< TA_Base_Core::IArchiveTable* > m_archiveTables;
		InvalidArchiveFile invalidArchiveFileVector;
		bool hasInvalidArchive;
        
    };
}

#endif // RESTORE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_
