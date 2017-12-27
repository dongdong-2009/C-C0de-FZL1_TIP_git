/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportFileAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportFileAccessFactory is a singleton that is used to retrieve ReportFile objects either from the
  * database or newly created. 
  */



#if !defined(ReportFileAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ReportFileAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IReportFile.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{

	class ReportFileAccessFactory
	{

	public:
        virtual ~ReportFileAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ReportFileAccessFactory object.
         */
		static ReportFileAccessFactory& getInstance();


		/**
         * getReportFile
         *
         * Retrieves the specified ReportFile and returns the associated data as an 
         * object conforming to the IReportFile interface.
         *
         * @param isReadOnly -- to the report file to retrieve, whether readonly or not
         *
         * @return A pointer to an object conforming to the IReportFile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IReportFile 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Database is not/cannot be opened
         *            2) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no ReportFile
         *            matching the supplied ID, or if there is more than one ReportFile
         *            matching the supplied ID. 
         */
		IReportFile* getReportFile(bool isReadOnly);


		/**
         * getAllReportFiles
         *
         * Retrieves all report files from the database.
         *
         * @param -- NULL
         *
         * @return A pointer to an object conforming to the IReportFile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IReportFile 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Database is not/cannot be opened
         *            2) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no ReportFile
         *            matching the supplied report file ID, or if there is more than one ReportFile
         *            matching the supplied report file ID. 
         */
		std::vector<IReportFile*> getAllReportFiles();
		std::vector<IReportFile*> getAllReportFilesInSpecifiedDirId( unsigned long dirId );


        /**
         * deleteReportFile
         *
         * This will remove a report file from the database. 
		 * Following a call to this method the IReportFile passed must then be deleted by
         * the caller.
         *
         * @param IReportFile* A pointer to a non-NULL report file to be removed from the database.
         *
		 * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopened.
         *            2) The execution of the sql query fails.
         */
        void deleteReportFile(IReportFile* reportFile);


		//return false if report ID is invalid
		static bool isValidReportFileId( unsigned long reportFileId );

        unsigned long getDirIdFromReportFileId( unsigned long reportFileId);
    private:
        //
        // These are private as this method is a singleton
        //
		ReportFileAccessFactory() { };
		ReportFileAccessFactory( const ReportFileAccessFactory& theReportFileAccessFactory);
		ReportFileAccessFactory& operator=(const ReportFileAccessFactory &);

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        // The name of the local database to send updates to
        std::string m_localDatabase;

		//void getReportFiles( const std::string& sql, std::vector<IReportFile*>& reportFiles );
		void getReportFiles( TA_Base_Core::SQLStatement& sql, std::vector<IReportFile*>& reportFiles );

        static ReportFileAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(ReportFileAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
