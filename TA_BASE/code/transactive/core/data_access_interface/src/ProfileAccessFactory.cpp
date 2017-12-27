/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/06/06 17:42:09 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * ProfileAccessFactory is a singleton that is used to retrieve Profile objects either from the
  * database or newly created. All Profile objects returned will adhear to the IProfile interface.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>
#include <sstream>

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ProfileHelper.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/data_access_interface/src/ConfigProfile.h"
 
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    // Related to TD 8604: This is the name of the profile that acts like a "God"
    // profile. If this name is changed in the database scripts, it must be changed
    // here.
    const std::string ProfileAccessFactory::TRANSACTIVE_SUPER_PROFILE("TransActiveSuperProfile");

	ProfileAccessFactory* ProfileAccessFactory::m_instance = 0;

    ProfileAccessFactory& ProfileAccessFactory::getInstance()
    {

        if( m_instance == 0 )
        {
            m_instance = new ProfileAccessFactory();
        }
        return *m_instance;
    }


    IProfile* ProfileAccessFactory::getProfile( const unsigned long key, const bool readWrite )
    {
		// Get a specific profile
		LOG( SourceInfo, DebugUtil::FunctionEntry, "ProfileAccessFactory::getProfile");

        if (readWrite)
        {
            return new ConfigProfile( key );
        }
		else
		{
			return new Profile( key );
		}
		
    }

	void ProfileAccessFactory::getProfileBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IProfile*>& profiles, const bool readWrite)
	{
		// Get a connection to the database
		//IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		TA_ASSERT(0 != pDatabase, "the database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEPROF_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("PROFILE_TYPE");
        columnNames.push_back("PROFILE_GROUP");
        columnNames.push_back("PROFILE_ITSI");
        //columnNames.push_back("PAPGRO_NAME");
        columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("DISPLAY_NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql, columnNames);
		
        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {

                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {

                    if (readWrite)
                    {
                        profiles.push_back( new ConfigProfile(i, *data) );
                    }

                    else
                    {
                        profiles.push_back( new Profile(i, *data) );
                    }
                }
				//data exception
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
					throw;
                }
            }

			delete data;
			data = NULL;
        }

        while (pDatabase->moreData(data));
	}


    // TD 10468
    unsigned long ProfileAccessFactory::getDeletedRawProfileType( unsigned long key)
    {
        FUNCTION_ENTRY("getDeletedRawProfileType");

		// Get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << "select a.SEPROF_ID, a.NAME, a.SEPTYP_ID, a.SEPGRO_ID, a.RAIGRO_NAME, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_PROFILE a where a.DELETED <> 0 and a.SEPROF_ID =" << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7551, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7551, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7551, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedRawProfileType: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, false);

        if ( 0 == profiles.size())
        {
            std::ostringstream message;
			message << "No data found for Profile with Key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );
        }
		
		TA_ASSERT(1 == profiles.size(), "Profile key unique constraint violated");

		IProfile* profile = profiles[0];
		unsigned long pkey = 0;

		try
		{
			pkey = profile->getKey();
			delete profile;
			profile = NULL;
		}
		//exception
        catch(...)
        {
            delete profile;
            profile = NULL;
            throw;
        }
		
        FUNCTION_EXIT;
        return pkey;
    }

    std::string ProfileAccessFactory::getDeletedProfileName( unsigned long key)
    {
        // Get specified field from a deleted profile
		FUNCTION_ENTRY("getDeletedProfileName");

		// Get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << "select a.SEPROF_ID, a.NAME, a.SEPTYP_ID, a.SEPGRO_ID, a.RAIGRO_NAME, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_PROFILE a where a.DELETED <> 0 and a.SEPROF_ID =" << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7551, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7551, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7551, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedProfileName: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, false);

        if ( 0 == profiles.size())
        {
            std::ostringstream message;
			message << "No data found for Profile with Key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }

		TA_ASSERT(1 == profiles.size(), "Profile key unique constraint violated");

		IProfile* profile = profiles[0];
		std::string name;
		try
		{
			name = profile->getName();
			delete profile;
			profile = NULL;
		}
		//exception
        catch(...)
        {
            delete profile;
            profile = NULL;
            throw;
        }

        FUNCTION_EXIT;
        return name;
    }


    unsigned long ProfileAccessFactory::getDeletedProfileGroup( unsigned long key)
    {
        FUNCTION_ENTRY("getDeletedProfileGroup");

		// Get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << "select a.SEPROF_ID, a.NAME, a.SEPTYP_ID, a.SEPGRO_ID, a.RAIGRO_NAME, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_PROFILE a where a.DELETED <> 0 and a.SEPROF_ID =" << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7551, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7551, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7551, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedProfileGroup: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, false);

        if ( 0 == profiles.size())
        {
            std::ostringstream message;
			message << "No data found for Profile with Key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }

		TA_ASSERT(1 == profiles.size(), "Profile key unique constraint violated");

		IProfile* profile = profiles[0];
        unsigned long profileGroup = 0;

		try
		{
			profileGroup = profile->getAccessControlGroup();
			delete profile;
			profile = NULL;
		}
		//exception
        catch(...)
        {
            delete profile;
            profile = NULL;
            throw;
        }

        FUNCTION_EXIT;
        return profileGroup;
    }

    bool ProfileAccessFactory::getDeletedProfileIsSystemProfile( unsigned long key)
    {
        FUNCTION_ENTRY("getDeletedProfileIsSystemProfile");

		// Get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
       /* std::ostringstream sql;
		sql << "select a.SEPROF_ID, a.NAME, a.SEPTYP_ID, a.SEPGRO_ID, a.RAIGRO_NAME, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_PROFILE a where a.DELETED <> 0 and a.SEPROF_ID =" << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7551, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7551, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7551, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedProfileIsSystemProfile: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, false);

        if ( 0 == profiles.size())
        {
            std::ostringstream message;
			message << "No data found for Profile with Key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );


        }
		
		TA_ASSERT(1 == profiles.size(), "Profile key unique constraint violated");

		IProfile* profile = profiles[0];
        bool isSystem = false;

		try
		{
			isSystem = profile->isSystemProfile();
			delete profile;
			profile = NULL;
		}
		//exception
        catch(...)
        {
            delete profile;
            profile = NULL;
            throw;
        }

        FUNCTION_EXIT;
        return isSystem;
    }


	std::vector<IProfile*> ProfileAccessFactory::getProfilesByOperator( unsigned long key, const bool readWrite )
	{
		FUNCTION_ENTRY("getProfilesByOperator( key )");

		// Get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
       /* std::ostringstream sql;
		sql << "select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<< " TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_OPERATOR_PROFILE sop, SE_PROFILE_V sp where sop.OPERATORKEY = " << key
			<< " and sp.SEPROF_ID = sop.SEPROF_ID";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_Oracle_SELECT_5051, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5051, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5051, key);
//		TA_ASSERT(sql.size() > 0, "getProfilesByOperator: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);
		
        // Return the vector
		FUNCTION_EXIT;
        return profiles;
	}

    std::vector<IProfile*> ProfileAccessFactory::getProfilesByOperator( const std::string& name, const bool readWrite )
	{
		FUNCTION_ENTRY("getProfilesByOperator( name )");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << "select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<< " TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_OPERATOR_PROFILE sop, SE_PROFILE_V sp, OPERATOR o where o.NAME = '"
			<< databaseConnection->escapeQueryString(name.c_str()) << "' and sop.OPERATORKEY = o.PKEY and sp.SEPROF_ID = sop.SEPROF_ID";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_Oracle_SELECT_5052,
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5052,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5052,
			databaseConnection->escapeQueryString(name.c_str()));
//		TA_ASSERT(sql.size() > 0, "getProfilesByOperator: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
    }       

    std::vector<IProfile*> ProfileAccessFactory::getProfilesByLocation(unsigned long locationId, const bool readWrite)
	{
        FUNCTION_ENTRY("getProfilesByLocation");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << "select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<< " TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SE_PROFILE_LOCATION spl, SE_PROFILE_V sp where spl.SEPROF_ID = sp.SEPROF_ID and spl.LOCATIONKEY = " << locationId;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_PROFILE_LOCATION_Oracle_SELECT_8051, locationId);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_LOCATION_SELECT_8051, locationId);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_LOCATION_SELECT_8051, locationId);
//		TA_ASSERT(sql.size() > 0, "getProfilesByLocation: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;

	}


	std::vector<IProfile*> ProfileAccessFactory::getNonSystemProfilesByOperator( unsigned long key, const bool readWrite )
	{
		FUNCTION_ENTRY("getNonSystemProfilesByOperator( key )");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql <<	"select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<<	" TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<<	" TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<<	" from SE_OPERATOR_PROFILE sop, SE_PROFILE_V sp where sop.OPERATORKEY = " << key
			<<	" and sp.SEPROF_ID = sop.SEPROF_ID and sp.IS_SYSTEM = 0";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_OPERATOR_PROFILE_Oracle_SELECT_5053, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5053, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5053, key);
//		TA_ASSERT(sql.size() > 0, "getNonSystemProfilesByOperator: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
	}


    std::vector<IProfile*> ProfileAccessFactory::getNonSystemProfilesByOperator( const std::string& name, const bool readWrite )
	{
		FUNCTION_ENTRY("getNonSystemProfilesByOperator( name )");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		
		// Create the query
        /*std::ostringstream sql;
		sql <<	"select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<<	" TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<<	" TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS') from SE_OPERATOR_PROFILE sop, SE_PROFILE_V sp, OPERATOR o where o.NAME = '"
			<< databaseConnection->escapeQueryString(name.c_str()) << "' and sop.OPERATORKEY = o.PKEY and sp.SEPROF_ID = sop.SEPROF_ID and sp.IS_SYSTEM = 0";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_OPERATOR_PROFILE_Oracle_SELECT_5054, databaseConnection->escapeQueryString(name.c_str()));
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5054, databaseConnection->escapeQueryString(name.c_str()));
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5054, databaseConnection->escapeQueryString(name.c_str()));
//		TA_ASSERT(sql.size() > 0, "getNonSystemProfilesByOperator: Get SQL Statement error");
		
		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
    }       


    std::vector<IProfile*> ProfileAccessFactory::getAllProfiles(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllProfiles");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql <<	"select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<<	" TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<<	" TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_V sp";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7552);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7552);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7552);
//		TA_ASSERT(sql.size() > 0, "getAllProfiles: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
    }


    std::vector<IProfile*> ProfileAccessFactory::getAllSystemProfiles( const bool readWrite)
    {
		FUNCTION_ENTRY("getAllSystemProfiles");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql <<	"select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<<	" TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<<	" TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_V sp where sp.IS_SYSTEM = 1";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7553);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7553);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7553);

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
    }


    std::vector<IProfile*> ProfileAccessFactory::getAllNonSystemProfiles( const bool readWrite)
    {
		FUNCTION_ENTRY("getAllNonSystemProfiles");   

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql <<	"select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<<	" TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<<	" TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_V sp where sp.IS_SYSTEM = 0";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7554);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7554);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7554);
//		TA_ASSERT(sql.size() > 0, "getAllNonSystemProfiles: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, readWrite);

        // Return the vector
		FUNCTION_EXIT;
        return profiles;
    }


    IConfigProfile* ProfileAccessFactory::createProfile()
    {            
        return new ConfigProfile();    
    }


    IConfigProfile* ProfileAccessFactory::copyProfile(const IConfigProfile* profileToCopy)
    {
        FUNCTION_ENTRY("copyProfile");

        TA_ASSERT(profileToCopy !=  NULL, "The profile to copy was NULL");
        
        const ConfigProfile* cast = dynamic_cast<const ConfigProfile*>(profileToCopy);

        TA_ASSERT(cast != NULL, "Profile passed could not be converted into a ConfigProfile");

        FUNCTION_EXIT;
        return new ConfigProfile(*cast);

    }


    std::vector<std::string> ProfileAccessFactory::getAllDistinctParameterNames()
    {
        FUNCTION_ENTRY("getAllDistinctParameterNames");

        // Yes I know this is a hack  :(
        // but then this whole concept should be refactored out of existence
        // and that means changes to the configuration editor that I don't have time for right now

        // the original intent was to provide a flexabily way to add extra parameters
        // but the configuratuin editor (DisplayListControl.cpp) expects these parameters and hardcodes the strings!!!
        // so what's the point??

        // Create the vector to return
        std::vector<std::string> parameterNames;

        // these are the currently supported parameters
        parameterNames.push_back(DISPLAY_1);
        parameterNames.push_back(DISPLAY_2);
        parameterNames.push_back(DISPLAY_3);
        parameterNames.push_back(LeftBannerDefaultPage);
        parameterNames.push_back(CentreBannerDefaultPage);
        parameterNames.push_back(RightBannerDefaultPage);

        FUNCTION_EXIT;

        // Return the vector
        return parameterNames;
    }


    IProfile* ProfileAccessFactory::getTransActiveSuperProfile()
    {
        FUNCTION_ENTRY("getTransActiveSuperProfile");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// Create the query
        /*std::ostringstream sql;
		sql << " select sp.SEPROF_ID, sp.NAME, sp.SEPTYP_ID, sp.SEPGRO_ID, sp.RAIGRO_NAME, sp.IS_SYSTEM,"
			<< " TO_CHAR(sp.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(sp.DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_V sp where sp.NAME = '"
			<< TRANSACTIVE_SUPER_PROFILE << "'";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_PROFILE_Oracle_SELECT_7555, TRANSACTIVE_SUPER_PROFILE);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7555, TRANSACTIVE_SUPER_PROFILE);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_PROFILE_SELECT_7555, TRANSACTIVE_SUPER_PROFILE);
//		TA_ASSERT(sql.size() > 0, "getTransActiveSuperProfile: Get SQL Statement error");

		std::vector<IProfile*> profiles;
		getProfileBySql(databaseConnection, sql, profiles, false);

        if ( 0 == profiles.size())
        {
            std::ostringstream message;
			message << "No data found for Profile with Name " << TRANSACTIVE_SUPER_PROFILE;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );
    
        }
		
		TA_ASSERT(1 == profiles.size(), "Profile key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return profiles[0];
    }


} // closes TA_Base_Core


