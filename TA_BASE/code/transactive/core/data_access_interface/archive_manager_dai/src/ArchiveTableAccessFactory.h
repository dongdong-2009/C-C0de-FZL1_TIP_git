#if !defined(ArchiveTableAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ArchiveTableAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTableAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveTableAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the IArchiveTable interface.
  */

#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

// Forward declarations

namespace TA_Base_Core
{
    class IArchiveTable;
    class IConfigArchiveTable;
}

namespace TA_Base_Core
{
	class ArchiveTableAccessFactory
	{

	public:

        typedef std::vector< IArchiveTable* > IArchiveTableVector;

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      ArchiveTableAccessFactory&
         *              A reference to an instance of an ArchiveTableAccessFactory object.
         */

		static ArchiveTableAccessFactory& getInstance();

        /**
         * ~ArchiveTableAccessFactory
         *
         * Standard destructor.
         */

        virtual ~ArchiveTableAccessFactory() {};

        /**
         * getArchiveTable
         *
         * Retrieves the specified ArchiveTable.
         *
         * @param       unsigned long archiveTableKey 
         *              The unique key to the ArchiveTable to retrieve.
         * @param       bool isReadWrite
         *              True if the returned IArchiveTable can be configured.
         *              False otherwise.
         *
         * @return      IArchiveTable*
         *              A pointer to an object conforming to the IArchiveTable interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              IArchiveTable object.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if there is no ArchiveTable matching the supplied key.
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

        IArchiveTable* getArchiveTable( unsigned long archiveTableKey, bool isReadWrite );

        /**
         * getAllArchiveTables
         *
         * Retrieves the all the ArchiveTables.
         *
         * @param       bool isReadWrite
         *              True if the returned IArchiveTable can be configured.
         *              False otherwise.
         *
         * @return      IArchiveTableVector
         *              A vector of pointers to the objects conforming to the 
         *              IArchiveTable interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              IArchiveTable objects.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

        IArchiveTableVector getAllArchiveTables( bool isReadWrite );

        /**
         * createArchiveTable
         * 
         * Creates an empty IConfigArchiveTable object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigArchiveTable object.
         *
         * @return      IConfigArchiveTable*
         *              A pointer to an IConfigArchiveTable object.
         *
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

		IConfigArchiveTable* createArchiveTable();

	private:
		//
        // The one and only instance of this singleton class.
        //
		
        static ArchiveTableAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static ReEntrantThreadLockable m_singletonLock;

        /**
         * ArchiveTableAccessFactory
         *
         * Standard constructor.
         *
         * This class is a singleton, so use getInstance() to get the one and only
         * instance of this class.
         */

		ArchiveTableAccessFactory() {};

        //
        // Disable copy constructor and assignment operator.
        //

		ArchiveTableAccessFactory( const ArchiveTableAccessFactory& theArchiveTableAccessFactory );
		ArchiveTableAccessFactory& operator=( const ArchiveTableAccessFactory & );
	};

} // closes TA_Base_Core

#endif // !defined(ArchiveTableAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
