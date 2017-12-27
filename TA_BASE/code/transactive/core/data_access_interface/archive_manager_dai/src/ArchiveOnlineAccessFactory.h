#if !defined(ArchiveOnlineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ArchiveOnlineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveOnlineAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the IArchiveOnline interface.
  */

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

// Forward declarations

namespace TA_Base_Core
{
    class IArchiveOnline;
    class IConfigArchiveOnline;
}

namespace TA_Base_Core
{
	class ArchiveOnlineAccessFactory
	{

	public:

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      ArchiveOnlineAccessFactory&
         *              A reference to an instance of an ArchiveOnlineAccessFactory object.
         */

		static ArchiveOnlineAccessFactory& getInstance();

        /**
         * ~ArchiveOnlineAccessFactory
         *
         * Standard destructor.
         */

        virtual ~ArchiveOnlineAccessFactory() {};

        /**
         * getArchiveOnlineEntry
         *
         * Retrieves the only ArchiveOnline entry.
         *
         * @param       bool isReadWrite
         *              True if the returned IArchiveOnline can be configured.
         *              False otherwise.
         *
         * @return      IArchiveOnline*
         *              A pointer to an object conforming to the IArchiveOnline interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              IArchiveOnline object.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if:
         *              1) There is no ArchiveOnline entry in the table.
         *              2) There are multiple ArchiveOnline entries in the table.
         */

        IArchiveOnline* getArchiveOnlineEntry( bool isReadWrite = false );

	private:
		//
        // The one and only instance of this singleton class.
        //
		
        static ArchiveOnlineAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static ReEntrantThreadLockable m_singletonLock;

        /**
         * ArchiveOnlineAccessFactory
         *
         * Standard constructor.
         *
         * This class is a singleton, so use getInstance() to get the one and only
         * instance of this class.
         */

		ArchiveOnlineAccessFactory() {};

        //
        // Disable copy constructor and assignment operator.
        //

		ArchiveOnlineAccessFactory( const ArchiveOnlineAccessFactory& theArchiveOnlineAccessFactory );
		ArchiveOnlineAccessFactory& operator=( const ArchiveOnlineAccessFactory & );
	};

} // closes TA_Base_Core

#endif // !defined(ArchiveOnlineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
