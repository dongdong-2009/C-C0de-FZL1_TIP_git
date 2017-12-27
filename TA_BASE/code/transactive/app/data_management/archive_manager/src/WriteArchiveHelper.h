/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/WriteArchiveHelper.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Performs back end work for the Archive Manager writing process.
  */

#if !defined(WRITE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_)
#define WRITE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <afx.h>

#include "app/data_management/archive_manager/src/BasicHelper.h"

// Forward declaration

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
    class WriteArchiveHelper
    {
    public:
        
        //
        // This can be used as a command line parameter if you don't want the export process
        // to log to the default file name (which is based on the DEBUGFILE_RUNPARAM ).
        //

        static const std::string EXPORT_LOG_FILE_RUNPARAM;


		static const std::string CD_ARCHIVE;
       
        //
        // Struct containing all the details about an archived file.
        //

        struct FileDetails
        {
            time_t date;                // The date extracted from the file name.
            std::string name;           // The file name.
            std::string nameWithPath;   // The file name with the full path.
            double sizeInMB;            // The file size in MB.

            bool operator()( FileDetails& details1, FileDetails& details2 )
            {
                return details1.date < details2.date;
            }
        };
        typedef std::vector< FileDetails > FileDetailsVector;
        
        //
        // Vector of file names including the full path.
        //

        typedef std::vector< std::string > FilePathsVector;
        
        //
        // Map of volume label to the label that will be put on the DVD (more compact
        // than the volume label) and the files on that volume.
        //

        typedef std::map< std::string, std::pair< std::string, FileDetailsVector > > CdsMap;

        /**
          * WriteArchiveHelper
          *
          * Standard constructor.
          *
          * @param      TA_Base_Bus::IGUIAccess& guiAccessor
          *             The object used to access the ArchiveManager entity data.
          */

        WriteArchiveHelper( TA_Base_Bus::IGUIAccess& guiAccessor );

        /**
          * ~WriteArchiveHelper
          *
          * Standard destructor.
          */

        virtual ~WriteArchiveHelper() {};

        /** 
          * writeScheduledArchives
          *
          * Performs the scheduled archive process:
          * 1. Sends the required audit messages.
          * 2. Checks the users privileges.
          * 3. Opens the WriteArchiveDlg.
          * 4. Processes the returned status from that dialog.
          *
          * @param      std::string& messageForUser
          *             Will be populated with the message to display to the user.
          *             If empty, no message is required.
          * @param      TA_ArchiveManager::EMsgLevel& messageLevel
          *             Will be populated with the level to display the message at.
          */

        void writeScheduledArchives( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel );
		void setDvdVolumeNum();

        /** 
          * createManualArchive
          *
          * Performs the manual archive process:
          * 1. Sends the required audit messages.
          * 2. Checks the users privileges.
          * 3. Opens the DateRangeDlg, the RetrievingDataDlg, then the WriteArchiveDlg.
          * 4. Processes the returned status' from those dialogs.
          *
          * @param      std::string& messageForUser
          *             Will be populated with the message to display to the user.
          *             If empty, no message is required.
          * @param      TA_ArchiveManager::EMsgLevel& messageLevel
          *             Will be populated with the level to display the message at.
          */

        void createManualArchive( std::string& messageForUser, TA_ArchiveManager::EMsgLevel& messageLevel );

        /**
          * retrieveDataForDate
          *
          * Retrieves the data from the database for the given date.
          * 
          * @param      const time_t& date
          *             The date to retrieve.
          *             KT 9 Mar 04: PW #3161 Changed to remove dependence on date formatting.
          * @param      TA_ArchiveManager::FileStatusMap& fileStatusMap
          *             The log files that contain errors or warnings
          *             will be added to this map, so they can
          *             be displayed to the user.
          * @param      FilePathsVector& archiveFiles
          *             The full paths of the generated archive files will
          *             be added to this vector.
          *
          * @exception  TA_Base_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ExportProgram entity parameter cannot be retrieved or are empty.
          *             4) The DbConnection RunParam is not set or in the format DB:USERNAME:PASSWORD.
          *             5) The export program cannot be found.
          *             6) The audit message cannot be sent.
          */

        void retrieveDataForDate( const time_t& date, 
            TA_ArchiveManager::FileStatusMap& fileStatusMap, FilePathsVector& archiveFiles );

        /**
          * retrieveDataForDate
          *
          * Retrieves the data from the database for a range of given datea.
          * 
          * @param      time_t& startDate
          *             The start date to retrieve.
          * @param      time_t& finishDate
          *             The finish date to retrieve.
		  * @param      TA_ArchiveManager::FileStatusMap& fileStatusMap
          *             The log files that contain errors or warnings
          *             will be added to this map, so they can
          *             be displayed to the user.
          * @param      FilePathsVector& archiveFiles
          *             The full paths of the generated archive files will
          *             be added to this vector.
          * @param      CStatic & guiDateTextBox
          *             The GUI date text box to update in real time as each export program is launched
		  *
          * @exception  TA_Base_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ExportProgram entity parameter cannot be retrieved or are empty.
          *             4) The DbConnection RunParam is not set or in the format DB:USERNAME:PASSWORD.
          *             5) The export program cannot be found.
          *             6) The audit message cannot be sent.
          */

        void retrieveDataForDate( const time_t& startDate, const time_t& finishDate, 
            TA_ArchiveManager::FileStatusMap& fileStatusMap, FilePathsVector& archiveFiles, 
			CStatic & guiDateTextBox );

        /**
          * getAllFileDetails
          *
          * Extracts the file date from the file name and returns the date,
          * name and size of the files in the specified directory. The file
          * names must be in the format AYYYY_MM_DD_filepostfix.dmp or 
          * MYYYY_MM_DD_filepostfix.dmp otherwise they will be ignored.
          * 
          * Preconditions:  TA_ArchiveManager::isValidDirectory( archiveDirectory )
          *                 fromDate <= toDate
          *
          * @return     FileDetailsVector
          *             The dates, names and sizes of the files matching the
          *             given format in the Archive Directory. These are sorted in date order.
          *
          * @param      const std::string& archiveDirectory
          *             Specifies the directory to load the files from.
          * @param      const time_t& startDate
          *             Only find files with this date or after in their file name.    
          * @param      const time_t& toDate
          *             Only find files with this date or before in their file name.
          * @param      bool limitDateRange
          *             True if the dates should be limited to the given range.
          *             False if all files in the archive directory should be returned.
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ArchiveDirectory is not a directory.
          *             4) The toDate is after the startDate.
          */
        
        FileDetailsVector getAllFileDetails( const std::string& archiveDirectory, 
            const time_t& startDate, const time_t& toDate, bool limitDateRange = true );
        
        /**
          * getAllFileDetails
          *
          * Extracts the file date from the file name and returns the date,
          * name and size of the ALL the files in the specified directory.
          * names must be in the format AYYYY_MM_DD_filepostfix.dmp or 
          * MYYYY_MM_DD_filepostfix.dmp otherwise they will be ignored.
          *
          * All files will be returned.
          *
          * Preconditions:  TA_ArchiveManager::isValidDirectory( archiveDirectory )
          *                 fromDate <= toDate
          *
          * @param      const std::string& archiveDirectory
          *             Specifies the directory to load the files from.
          *
          * @return     FileDetailsVector
          *             The dates, names and sizes of the files matching the
          *             given format in the Archive Directory. These are sorted in date order.
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The ArchiveDirectory is not a directory.
          */

        FileDetailsVector getAllFileDetails( const std::string& archiveDirectory );

        /**
          * splitIntoCds
          *
          * Splits the file names up into sets that are small enough to fit on 
          * a single DVD.  All files from a single date are kept together on the
          * same DVD, unless they are too big, in which case they may be split
          * across DVD's.
          *
          * Precondition:   !fileDetails.empty()
          *
          * @return     CdsMap
          *             The file names split into DVD sized groups referenced
          *             by the volume label for the DVD.  The volume label
          *             will be in the format "startDate To toDate" or just
          *             "startDate" if there is only one date on the DVD.
          *
          * @param      const FileDetailsVector& fileDetails
          *             The dates, names and sizes of the files matching the
          *             given format in the Archive Directory. 
          *             ******* These must be sorted in date order. *******
          *
          * @exception  ArchiveException
          *             Thrown if there is a file that will not fit on a single DVD.
          */

        WriteArchiveHelper::CdsMap splitIntoCds( const FileDetailsVector& fileDetails );

        /**
          * deleteArchiveFiles
          *
          * Deletes the specified archive files.
          *
          * @param      const FileDetailsVector& files
          *             The files to be deleted.  These will include the full path.
          *
          * @exception  ArchiveException
          *             Thrown if any of the files could not be deleted.
          *             The exception message will contain a list of files
          *             that could not be deleted along with the reason
          *             for the failure.
          */

        void deleteArchiveFiles( const FileDetailsVector& files );
 
        /**
          * deleteArchiveFiles
          *
          * Deletes the specified archive files.
          *
          * @param      const FilePathsVector& files
          *             The files to be deleted.  These will include the full path.
          *
          * @exception  ArchiveException
          *             Thrown if any of the files could not be deleted.
          *             The exception message will contain a list of files
          *             that could not be deleted along with the reason
          *             for the failure.
          */

        void deleteArchiveFiles( const FilePathsVector& files );

        /**
          * getArchiveDirectory
          *
          * Gets the full path of the archive directory from the
          * Archive Manager entity parameter MediaLocation.
          *
          * @return     vector<string>
          *             The archive directories on all servers
          *
          * @param      bool isManualArchive
          *             True => Get the ManualArchiveDirectory
          *             False => Get the AutomaticArchiveDirectory
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The Archive Directory path is empty.
          *             4) The directory name is not a valid directory.
          */

        std::vector<std::string> getArchiveDirectory( bool isManualArchive );

        /**
          * getExportLogFileName
          *
          * Gets the name of the file (inc. path) that the export program logs to.
          *
          * @return     std::string
          *             The file name & path.
          */

        std::string getExportLogFileName();

        /**
          * loadExportCommand
          *
          * Loads the information for the export command from the database
          * and the RunParams into m_genericExpCommand.  Note that the
          * date to export is not loaded, because that will be determined 
          * during the retrieve phase.
          *
          * This method will be called from retrieveData() each time, 
          * but it will return immediately if the command has already been loaded.
          *
          * @exception  TA_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The DatabaseSystemUserName, DatabaseSystemPassword and then ImpProgramPath
          *                entity parameters cannot be retrieved or are empty.
          *             4) The DbConnection RunParam is not set or in the format DB:USERNAME:PASSWORD.
          */

        void loadExportCommand();

		static void setMaxDvdSize(unsigned short dvdSize);

    private:
        //
        // The maximum capacity of the DVD's in megabytes.
        //

        static const unsigned short MAX_CD_SIZE_IN_MB;
		static unsigned short MAX_DVD_SIZE_IN_MB;

        //
        // Oracle export program
        //

        static const std::string EXPORT_PROGRAM;

        //
        // Used for grouping all files for a particular date in the splitIntoCds() method.
        //

        struct FilesForDate
        {
            time_t date;                    // The date for all the files, extracted from their filenames.
            double sizeInMB;                // The total size of all the files.
            FileDetailsVector fileDetails;  // The details for each individual file.
        };
        typedef std::vector< FilesForDate > FilesForDateVector;
       
        //
        // Disable copy constructor and assignment operator.
        //

		WriteArchiveHelper( const WriteArchiveHelper& theArchiveHelper );
		WriteArchiveHelper& operator=( const WriteArchiveHelper& );
    
        /**
          * groupFilesByDate
          *
          * Takes a set of files and groups them into separate dates.
          *
          * @param      const FileDetailsVector& fileDetails
          *             The files to split up in chronological order.
          * @param      FilesForDateVector& allFilesByDate
          *             Will be filled with the files grouped by date in chronological order.
          *
          * @exception  ArchiveException
          *             Thrown if there is a file that will not fit on a single DVD.
          */             

        void groupFilesByDate( const FileDetailsVector& fileDetails, FilesForDateVector& allFilesByDate );
   
        /**
          * createCd
          *
          * Groups all of the files for the dates that are in the range specified by
          * the two iterators into a DVD group and adds it to the map of DVD's.
          *
          * @param      const FilesForDateVector& allFilesByDate
          *             The vector of all files that are to be put on DVD.
          * @param      const FilesForDateVector::iterator& firstSetOfFilesForCdIt
          *             The first date in the allFilesByDate vector that should
          *             go on this particular DVD.
          * @param      const FilesForDateVector::iterator& lastSetOfFilesForCdIt
          *             The last date in the allFilesByDate vector that should
          *             go on this particular DVD.
          * @param      CdsMap& cdsMap
          *             The new DVD will be added to this map.
          */

        void createCd( const FilesForDateVector& allFilesByDate, 
            const FilesForDateVector::iterator& firstSetOfFilesForCdIt, 
            const FilesForDateVector::iterator& lastSetOfFilesForCdIt,
            CdsMap& cdsMap );

        /**
          * createCdsForSingleDate
          *
          * Splits up the files for a single date onto multiple DVD's.
          * This is called if the files for a date will not fit on a single DVD.
          *
          * @param      const FileDetailsVector& fileDetailsForDate
          *             The files for the given date.
          * @param      CdsMap& cdsMap
          *             The new DVD's will be added to this map.
          */
        
        void createCdsForSingleDate( const FileDetailsVector& fileDetailsForDate, CdsMap& cdsMap );

        /**
          * getNumberOfCdsForDate
          *
          * Gets the number of DVD's that will be required to fit the given files.
          *
          * @return     unsigned short
          *             The required number of DVD's.
          */ 

        unsigned short getNumberOfCdsForDate( const FileDetailsVector& fileDetailsForDate );

        //
        // The object used to access the ArchiveManager entity data.
        //

        TA_Base_Bus::IGUIAccess& m_guiAccessor;

        //
        // The details of the files that will be written to DVD.
        //
        
        typedef std::multimap< std::string, FileDetailsVector > DateToFileDetailsMap;
        DateToFileDetailsMap m_fileDetails;
		int m_dvdvolume;

        //
        // This is the export command to use.  It will be filled out with all the
        // database details.
        //

        std::stringstream m_genericExpCommand;
        
        //
        // The name of the file (inc. path) that the export program logs to.
        //

        std::string m_exportLogFileName;
		
    };
}

#endif // WRITE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_
