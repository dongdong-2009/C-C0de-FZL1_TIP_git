/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveOnlineAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the IArchiveOnline interface.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>
#include <memory>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnline.h"
#include "core/data_access_interface/archive_manager_dai/src/ConfigArchiveOnline.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineTable.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	// Initialise statics
		
    ReEntrantThreadLockable ArchiveOnlineAccessFactory::m_singletonLock;
	ArchiveOnlineAccessFactory* ArchiveOnlineAccessFactory::m_theClassInstance = 0;


    //
    // getInstance
    //
	ArchiveOnlineAccessFactory& ArchiveOnlineAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still 0.

		if ( m_theClassInstance == 0 )
		{
			// Create the one & only object
			
            m_theClassInstance = new ArchiveOnlineAccessFactory();
		}
		
		return *m_theClassInstance;
	}

    
    //
    // getArchiveOnline
    //
    IArchiveOnline* ArchiveOnlineAccessFactory::getArchiveOnlineEntry( bool isReadWrite )
    {
        // Get a connection to the database
        
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Check that there is one and only one ArchiveOnline entry in the table.

        /*std::stringstream selectSql;
        selectSql << "select " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN;
        selectSql << " from " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_STD_SELECT_30001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_SELECT_30001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_SELECT_30001);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN );
        
        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveOnline entry from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // Create the ArchiveOnline object to represent this object. Initially only populate it
        // with ArchiveOnline key. We will populate the rest of the data on demand.

        IArchiveOnline* theArchiveOnline;
        
        // Decide which type of ArchiveOnline object to return.
        // The new's may throw an exception, but it is okay because everything will
        // be cleaned up.

        if ( isReadWrite )  // Need a config object.
        {
            if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
            {
                theArchiveOnline = new ConfigArchiveOnline();
            }
            else
            {
            theArchiveOnline = new ConfigArchiveOnline( data->getUnsignedLongData( 0, TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );
        }
        }
        else                // Need a standard object.
        {
            if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
            {
                std::stringstream error;
                error << "No data found for ArchiveOnline entry.";			
                TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ));
            }
            theArchiveOnline = new ArchiveOnline( data->getUnsignedLongData( 0, TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );
        }

        // Return the appropriate object. 
        // The class that recieves this pointer is responsible for deleting it.

        return theArchiveOnline;
    }

} // closes TA_Base_Core


