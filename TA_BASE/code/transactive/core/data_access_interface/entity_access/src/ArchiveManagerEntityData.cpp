/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ArchiveManagerEntityData.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2015/05/25 17:45:22 $
  * Last modified by: $Author: raghu.babu $
  * 
  * ArchiveManagerEntityData provides access to ArchiveManagerEntityData entity data
  * and a specific framework for ArchiveManagerEntityData entity parameters.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/ArchiveManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    // 
    // The entity parameters for this entity type.
    //
  
    const std::string AUTO_ARCHIVE_DIRECTORY        = "AutoArchiveDirectory";
    const std::string MANUAL_ARCHIVE_DIRECTORY      = "ManualArchiveDirectory";
	const std::string VOLUME_OF_DVD               = "No of Kbytes per media"; // yangguang TD16713
	const std::string IMP_FROM_USER					= "FromUser";
	const std::string IMP_TO_USER					= "ToUser";
}


namespace TA_Base_Core
{
    //
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    //

    const std::string ArchiveManagerEntityData::ENTITY_TYPE = "ArchiveManager";
   
    //
    // ArchiveManagerEntityData
    //
    ArchiveManagerEntityData::ArchiveManagerEntityData()
        : EntityData()
    {
        FUNCTION_ENTRY( "ArchiveManagerEntityData constructor" );
    }


    //
    // ArchiveManagerEntityData
    //
    ArchiveManagerEntityData::ArchiveManagerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
        FUNCTION_ENTRY( "ArchiveManagerEntityData constructor" );
    }



    //
    // isArchiveManagerAlreadyRunning
    //
    bool ArchiveManagerEntityData::isArchiveManagerAlreadyRunning( std::string& workstationName )
    {
        // KT 3 Mar 04, PW #3136: Added to allow only a single instance of the Archive Manager.
        
        FUNCTION_ENTRY( "isArchiveManagerAlreadyRunning" );

		/*
        std::string dbConnection = TA_Base_Core::RunParams::getInstance().get( RPARAM_DBCONNECTION );
        int beginOfSchema = dbConnection.find_first_of( ':' ) + 1;
        int endOfSchema = dbConnection.find_first_of( ':', beginOfSchema );

        std::string schemaName = dbConnection.substr( beginOfSchema, endOfSchema - beginOfSchema );
		*/

		std::string dbConnectionString;
		TA_Base_Core::DbConnection::getInstance().getConnectionString(Archiving_Cd, Read, dbConnectionString);

        std::vector<std::string> components;
        SimpleDb::ParseConnectStr( dbConnectionString,	components );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Schema to check is %s", components[2].c_str() );

        // Order by login time to ensure that our connection comes at the end and any other connections
        // started before us come first. That way we know we will select the correct machine name and not our
        // own machine name.

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Read);

        //delete by lin
        //std::string sql("select TERMINAL from local_sessions where program like '%ArchiveManager%' and schemaname = UPPER('");
        //sql += databaseConnection->escapeQueryString(components[1]);
        //sql += "') and status in ('ACTIVE','INACTIVE') order by logon_time";
        
        std::vector< std::string > columns;
        columns.push_back("machine");

        try
        {
		
		// ****************************** NOTE: ************************************************
		// Components is dependent on the connection string, should the connection string change
		// please consider the index of the schema name if it change please updated accordingly
		// because schema name is used in the query below.
		// ****************************** NOTE: ************************************************
			//waitforcheck delete by lin
			SQLStatement strSql;		
			databaseConnection->prepareSQLStatement(strSql, LOCAL_SESSIONS_SELECT_79001,
				  databaseConnection->escapeQueryString(components[2]));
			
            std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columns ) );
            if ( 0 == data.get() )
            {
                // If we cannot determine then we must return true for safety.

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, 
                    "Could not determine if an Archive Manager is already connected to this schema" );
                return true;
            }

            if ( data->getNumRows() == 1 )
            {
                // This means only we are connected.
                // No Archive Manager's are currently connected to this schema.

                return false;
            }
            else
            {
                // An Archive Manager is already connected to this schema.

                workstationName = data->getStringData( 0, "machine" );
                return true;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not determine if an Archive Manager is already connected to this schema" );
        }

        // If we cannot determine then we must return true for safety.  Better to have none running than two.

        return true;
    }


    //
    // getAutoArchiveDirectory
    //
    std::string ArchiveManagerEntityData::getAutoArchiveDirectory()
    {
        return getHelper()->getParameter(AUTO_ARCHIVE_DIRECTORY);
    }

       
    //
    // setAutoArchiveDirectory
    //
    void ArchiveManagerEntityData::setAutoArchiveDirectory( const std::string& autoArchiveDirectory )
    {
        setParameter( AUTO_ARCHIVE_DIRECTORY, autoArchiveDirectory);
    }

	// yangguang++ TD16713
	//
    // getVolumeofDVD
    //
    std::string ArchiveManagerEntityData::getVolumeofDVD()
    {
        return getHelper()->getParameter(VOLUME_OF_DVD);
    }

       
    //
    // settVolumeofDVD
    //
    void ArchiveManagerEntityData::setVolumeofDVD( const std::string& autoArchiveDirectory )
    {
        setParameter( VOLUME_OF_DVD, autoArchiveDirectory);
    }
	// ++yangguang TD16713


    //
    // getManualArchiveDirectory
    //
    std::string ArchiveManagerEntityData::getManualArchiveDirectory()
    {
        return getHelper()->getParameter(MANUAL_ARCHIVE_DIRECTORY);
    }

       
    //
    // setManualArchiveDirectory
    //
    void ArchiveManagerEntityData::setManualArchiveDirectory( const std::string& manualArchiveDirectory )
    {
        setParameter( MANUAL_ARCHIVE_DIRECTORY, manualArchiveDirectory);
    }

	//
	// getImpFromUser
	//
	std::string ArchiveManagerEntityData::getImpFromUser()
	{
		return getHelper()->getParameter(IMP_FROM_USER);
	}

	//
	// getImpToUser
	//
	std::string ArchiveManagerEntityData::getImpToUser()
	{
		return getHelper()->getParameter(IMP_TO_USER);
	}


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string ArchiveManagerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string ArchiveManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* ArchiveManagerEntityData::clone( unsigned long key )
    {
        return new ArchiveManagerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void ArchiveManagerEntityData::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "invalidate" );
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
        LOG_FUNCTION_EXIT( SourceInfo, "invalidate" );
    }

        
    ///////////////////////////////////////////////////////////////////////
    //
    // Protected Methods
    //
    ///////////////////////////////////////////////////////////////////////



    //
    // validateData
    //
    void ArchiveManagerEntityData::validateData()
    {
    }


    //
    // setParameter (for string parameters)
    //
    void ArchiveManagerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );

        // Set the member variable.
    }

} // TA_Base_Core
