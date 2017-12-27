/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportDirectoryAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportDirectoryAccessFactory is a singleton that is used to retrieve ReportDirectory objects either from the
  * database or newly created. 
  */



#if !defined(ReportDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ReportDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IReportDirectory.h"
#include "core/data_access_interface/src/SQLCode.h"


namespace TA_Base_Core
{

	class ReportDirectoryAccessFactory
	{

	public:
        virtual ~ReportDirectoryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ReportDirectoryAccessFactory object.
         */
		static ReportDirectoryAccessFactory& getInstance();


		/**
         * getReportDirectory
         *
         * Retrieves the specified ReportDirectory and returns the associated data as an 
         * object conforming to the IReportDirectory interface.
         *
         * @param isReadOnly -- to the report file to retrieve, whether readonly or not
         *
         * @return A pointer to an object conforming to the IReportDirectory interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IReportDirectory 
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
		IReportDirectory* getReportDirectory(bool isReadOnly);


		/**
         * getAllReportDirectories
         *
         * Retrieves all report files from the database.
         *
         * @param -- NULL
         *
         * @return A pointer to an object conforming to the IReportDirectory interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IReportDirectory 
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
		std::vector<IReportDirectory*> getAllReportDirectories();
		std::vector<IReportDirectory*> getAllReportDirsInSpecifiedParentDirId( unsigned long parentDirId );


        /**
         * deleteReportDirectory
         *
         * This will remove a report file from the database. 
		 * Following a call to this method the IReportDirectory passed must then be deleted by
         * the caller.
         *
         * @param IReportDirectory* A pointer to a non-NULL report file to be removed from the database.
         *
		 * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopened.
         *            2) The execution of the sql query fails.
         */
        void deleteReportDirectory(IReportDirectory* reportDirectory);


		//return false if report ID is invalid
		static bool isValidReportDirectoryId( unsigned long reportDirectoryId );

        unsigned long getParentDirIdFromReportDirectoryId( unsigned long reportDirectoryId);
    private:
        //
        // These are private as this method is a singleton
        //
		ReportDirectoryAccessFactory() { };
		ReportDirectoryAccessFactory( const ReportDirectoryAccessFactory& theReportDirectoryAccessFactory);
		ReportDirectoryAccessFactory& operator=(const ReportDirectoryAccessFactory &);

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        // The name of the local database to send updates to
        std::string m_localDatabase;

		//void getReportDirectorys( const std::string& sql, std::vector<IReportDirectory*>& reportDirectorys );
		void getReportDirectorys(TA_Base_Core::SQLStatement& sql, std::vector<IReportDirectory*>& reportDirectorys );
		

        static ReportDirectoryAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(ReportDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
