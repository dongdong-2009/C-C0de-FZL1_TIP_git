/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DatabaseFactory.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * DatabaseFactory is a singleton that returns a pointer to an IDatabase object
  * The database object that it returns will always be connected.
  */

#if !defined(DatabaseFactory_A1678C40_570B_454d_ACB1_03A31C8FEF48__INCLUDED_)
#define DatabaseFactory_A1678C40_570B_454d_ACB1_03A31C8FEF48__INCLUDED_

#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/src/SimpleDbDatabase.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <map>

namespace TA_Base_Core
{
	class DbConnectionStrings;  // forward
	class DatabaseFactory
	{

	// This class is a singleton and so the constructor, copy constructor and assignment operator
	// are all made private;

	private:
		DatabaseFactory() { };
		DatabaseFactory( const DatabaseFactory& theDatabaseFactory);
		DatabaseFactory& operator=(const DatabaseFactory &);


	public:
		virtual ~DatabaseFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this class
         *
         * @return A reference to a class of type DatabaseFactory
         */
		static DatabaseFactory& getInstance();

		static void removeInstance();



        /**
         * getDatabase
         *
         * Creates and returns class that implements the IDatabase interface. This method
         * can be configured using pre-processor definitions, or a configuration file, to
         * load different database implementations based upon the individual requirements of
         * a project.
         *
         * @returns A pointer to a class that implements IDatabase. Note that the connect()
         * method has already been called on this database connection (any repeated calls
         * will simply do nothing)
         */
		IDatabase* getDatabase(EDataTypes, EDataActions);
		IDatabase* getDatabase(EDataTypes, EDataActions, IDatabase*);

        /**
         * delDatabase
         *
         * getDatabase will create different instances of IDatabase according to different threads.
         * Delete database in the same thread to avoid memory leak. This method should not be called
         * if the same thread is going to use the same database again. Called this method only if one thread
         * which has been connected to database, is going to be terminated.
         *
         */
        void delDatabase(EDataTypes, EDataActions);
        void delDatabase(IDatabase*);

        /**
         * cleanup
         *
         * Cleans up all cached database objects
         */
        void cleanup();

		/**  oyzl++
         * getDatabaseName
         *
         * get local database name based on connection file name
         */
		std::string getDatabaseName(const std::string&, const EDataTypes dataType, const EDataActions action);

	private:
        typedef std::map<std::string, IDatabase*> ConnectionMap;
        typedef std::map<int, ConnectionMap> ThreadMap;
		typedef std::map<std::string, DbConnectionStrings* > DbNameStrMap; 
        ThreadMap         m_databaseMap;
		DbNameStrMap      m_DbStrMap;

        static DatabaseFactory* m_instance;
        static NonReEntrantThreadLockable m_instanceLock;
        NonReEntrantThreadLockable m_getDatabaseLock;
		ReEntrantThreadLockable m_getDbNameLock;
	};
} // closes TA_Base_Core

#endif // !defined(DatabaseFactory_A1678C40_570B_454d_ACB1_03A31C8FEF48__INCLUDED_)
