/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportDirectory.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/11/16 18:23:22 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportDirectory is an implementation of IReportDirectory. It holds the data specific to an ReportDirectory entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(ReportDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ReportDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IReportDirectory.h"

namespace TA_Base_Core
{
    class ReportDirectory : public IReportDirectory
    {

    public:

        /**
         * Constructor
         *
         * Construct an ReportDirectory class based around a key.
         *
         * @param bool readOnly
         */
        ReportDirectory(bool isReadOnly);

        /** 
          * ReportDirectory
          *
          * Create a new report directory that needs to be written to the database (using applyChanges), 
		  * the ReportDirectoryID will be created automagically
          *
          * @param  unsigned long reportDirectoryID
          * @param std::string reportDirectoryName
		  * @param unsigned long reportParentDirectoryID
		  * @param bool isReadOnly
          */
		ReportDirectory( unsigned long reportDirectoryID, std::string reportDirectoryName,
			unsigned long reportParentDirectoryID, bool isReadOnly = true);	
		
        /**
         * Destructor
         */
        virtual ~ReportDirectory();

        /**
         * getReportDirectoryId
         *
         * Returns the report Directory ID for this REPORT_DIRECTORY entry.
         *
         * @return The Directory ID for this report as a unsigned long.
         */
        virtual unsigned long getReportDirectoryId();

        /**
         * getReportDirectoryName
         *
         * Returns the report directory name for this REPORT_DIRECTORY entry.
		 * Display this directory name on CTreeCtrl List
         *
         * @return The name for the report directory as an string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
		virtual std::string getReportDirectoryName();

        /**
         * getReportParentDirectoryId
         *
         * Returns the parent directory ID of the report directory 
         *
         * @return The ID for parent directory as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual unsigned long getReportParentDirectoryId();

        /**
         * invalidate
         *
         * Make the data contained by this ReportDirectory as invalid. The next call to get...() 
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
        ReportDirectory( const ReportDirectory& theReportDirectory);            
		ReportDirectory& operator=(const ReportDirectory &);
		ReportDirectory();

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

		unsigned long m_reportDirectoryId;
		std::string m_reportDirectoryName;
		unsigned long m_reportParentDirectoryId;

		bool m_isReadOnly;
		bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(ReportDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
