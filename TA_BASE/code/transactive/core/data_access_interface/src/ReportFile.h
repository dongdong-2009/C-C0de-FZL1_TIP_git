/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportFile.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/11/16 18:23:22 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportFile is an implementation of IReportFile. It holds the data specific to an ReportFile entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(ReportFile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ReportFile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IReportFile.h"

namespace TA_Base_Core
{
    class ReportFile : public IReportFile
    {

    public:

        /**
         * Constructor
         *
         * Construct an ReportFile class based around a key.
         *
         * @param bool readOnly
         */
        ReportFile(bool isReadOnly);

        /** 
          * ReportFile
          *
          * Create a new report file that needs to be written to the database (using applyChanges), 
		  * the ReportFileID will be created automagically
          *
          * @param  unsigned long reportFileID
          * @param std::string reportFileName
          * @param std::string reportFilePhysicalPath
		  * @param unsigned long reportDirectoryID
		  * @param bool isReadOnly
          */
		ReportFile( unsigned long reportFileID, std::string reportFileName,
			std::string reportFilePhysicalPath, unsigned long reportDirectoryID, bool isReadOnly = true);	
		
        /**
         * Destructor
         */
        virtual ~ReportFile();

        /**
         * getReportFileId
         *
         * Returns the report file ID for this REPORT_FILE entry.
         *
         * @return The File ID for this report as a unsigned long.
         */
        virtual unsigned long getReportFileId();

        /**
         * getReportFileName
         *
         * Returns the report file name for this REPORT_FILE entry.
         *
         * @return The name for the report file as an string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual std::string getReportFileName();

        /**
         * getReportFilePhysicalPath
         *
         * Returns the physical path for the REPORT_FILE
         *
         * @return The physical path for the report file as an string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual std::string getReportFilePhysicalPath();

        /**
         * getReportFileDirectoryId
         *
         * Returns the directory ID of the report file 
         *
         * @return The ID for directory as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual unsigned long getReportFileDirectoryId();

        /**
         * invalidate
         *
         * Make the data contained by this ReportFile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        virtual void applyChanges();

    private:
        // Copy constructor and assignment operator are not used so they are made private
        ReportFile( const ReportFile& theReportFile);            
		ReportFile& operator=(const ReportFile &);
		ReportFile();

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, 
         *            or if the wrong ammount of data is retrieved.
         */
        void reload();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();
		
	private:

        // The name of the local database to send updates to
        std::string m_localDatabase;

		unsigned long m_reportFileId;
		std::string m_reportFileName;
		std::string m_reportFilePhysicalPath;
		unsigned long m_reportDirectoryId;

		bool m_isReadOnly;
		bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(ReportFile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
