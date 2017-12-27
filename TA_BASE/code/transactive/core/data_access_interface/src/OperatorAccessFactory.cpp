/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OperatorAccessFactory.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2017/07/10 17:20:51 $
  * Last modified by:  $Author: Ouyang $
  * 
  * OperatorAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IOperator interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif


#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "core/data_access_interface/src/Operator.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ConfigOperator.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Core
{
	OperatorAccessFactory::OperatorAccessFactory()
		:
		m_operatorToProfileLoad(false),
		m_operatorToRegionLoad(false)
	{
		initializeOperatorProfile();
		initializeOperatorRegion();
	}

	OperatorAccessFactory::OperatorAccessFactory(int type)
		:
		m_operatorToProfileLoad(false),
		m_operatorToRegionLoad(false)
	{
		// just to differentiate from OperatorAccessFactory(); 
		// in this constructor, we don't have to initialize m_operatorToProfileMap and m_operatorToRegionMap
	}

    OperatorAccessFactory* OperatorAccessFactory::m_instance = 0;

    OperatorAccessFactory& OperatorAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new OperatorAccessFactory();
        }

        return *m_instance;
    }

	/* This function is for getting an instance, 
	   and meanwhile avoid calling initializeOperatorProfile() and initializeOperatorRegion() in constructure, 
	   which would otherwise spend a lot of time.
	
	  In some situation, like in application EventViewer, we use this OperatorAccessFactory only for loading operators' simple info(id and name)
	  and we don't care the profile/region infomation. In this case, instead of getInstance(),we can call this funtion to get a "Simple" instance and then load what we need.
	  We can improve the performance in this way.
	*/
	OperatorAccessFactory& OperatorAccessFactory::getInstanceNoInit()
	{

		if( m_instance == 0 )
		{
			m_instance = new OperatorAccessFactory(1);
		}

		return *m_instance;
	}

	void OperatorAccessFactory::init()
	{
		initializeOperatorProfile();
		initializeOperatorRegion();
	}

	void OperatorAccessFactory::initializeOperatorProfile()
	{
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

		if (!m_operatorToProfileLoad)
		{
			ThreadGuard guard(m_operatorToProfileLock);

			if (!m_operatorToProfileLoad)
			{
				// get a connection to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
				
				/*std::ostringstream sql;
				sql << "select OPERATORKEY, SEPROF_ID from SE_OPERATOR_PROFILE";*/
		//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_SELECT_5001);
		//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5001);
				SQLStatement sql;
				databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5001);
		//		TA_ASSERT(sql.size() > 0, "initializeOperatorProfile: Get SQL Statement error");

				// Set up the columnNames vector to be passed to executeQuery()
				std::vector<std::string> columnNames;
				columnNames.push_back("OPERATORKEY");
				columnNames.push_back("SEPROF_ID");

				// Execute the query. The method can throw a DatabaseException.
				// This is documented in the comment of this method.
				// We are responsible for deleting the returned IData object when we're done with it
				IData* data = databaseConnection->executeQuery(sql, columnNames);

				do
				{
					for (unsigned long i = 0; i < data->getNumRows(); i++)
					{
						try
						{
							unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
							unsigned long profileId  = data->getUnsignedLongData(i, columnNames[1]);

							m_operatorToProfileMap[operatorId].push_back(profileId);
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
				while ( databaseConnection->moreData(data) );
				
				m_operatorToProfileLoad = true;
			}
		}
	}

	void OperatorAccessFactory::initializeOperatorRegion()
	{
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

		if (!m_operatorToRegionLoad)
		{
			ThreadGuard guard(m_operatorToRegionLock);

			if (!m_operatorToRegionLoad)
			{
				// get a connection to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
				
				/*std::ostringstream sql;
				sql << "select OPERATORKEY, SEREGI_ID from SE_OPERATOR_REGIONS";*/
		//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_REGIONS_STD_SELECT_5501);
		//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_REGIONS_SELECT_5501);
				SQLStatement sql;
				databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_REGIONS_SELECT_5501);
		//		TA_ASSERT(sql.size() > 0, "initializeOperatorRegion: Get SQL Statement error");

				// Set up the columnNames vector to be passed to executeQuery()
				std::vector<std::string> columnNames;
				columnNames.push_back("OPERATORKEY");
				columnNames.push_back("SEREGI_ID");
				// Execute the query. The method can throw a DatabaseException.
				// This is documented in the comment of this method.
				// We are responsible for deleting the returned IData object when we're done with it
				IData* data = databaseConnection->executeQuery(sql, columnNames);

				do
				{
					for (unsigned long i = 0; i < data->getNumRows(); i++)
					{
						try
						{
							unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
							unsigned long regionId  = data->getUnsignedLongData(i, columnNames[1]);
							m_operatorToRegionMap[operatorId].push_back(regionId);
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

				while ( databaseConnection->moreData(data) );
				m_operatorToRegionLoad = true;
			}
		}
	}


    void OperatorAccessFactory::getProfileByOperator(unsigned long operatorId, std::vector<unsigned long>& profiles)
    {
		FUNCTION_ENTRY("OperatorAccessFactory::getProfileByOperator");

		if (!m_operatorToProfileLoad)
		{
			initializeOperatorProfile();
		}
		OperatorMap::iterator it = m_operatorToProfileMap.find( operatorId );

		if ( it != m_operatorToProfileMap.end() )
		{
            profiles = it->second;
		}
		FUNCTION_EXIT;
    }
	

    void OperatorAccessFactory::getRegionByOperator(unsigned long operatorId, std::vector<unsigned long>& regions)
    {
		FUNCTION_ENTRY("OperatorAccessFactory::getRegionByOperator");

		if (!m_operatorToRegionLoad)
		{
			initializeOperatorRegion();
		}
		OperatorMap::iterator it = m_operatorToRegionMap.find( operatorId );

		if ( it != m_operatorToRegionMap.end() )
		{
            regions = it->second;
		}
		FUNCTION_EXIT;
    }


    void OperatorAccessFactory::getOperatorBySql( 
		IDatabase* pDatabase,
		const SQLStatement& sql,
		std::vector<IOperator*>& operators,
		const bool readWrite,
		const bool getCfgProfiles,
		const bool getCfgRegions,
		const bool getProfiles,
		const bool getRegions)
    {
        // get a connection to the database
    //    IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		TA_ASSERT(0 != pDatabase, "the database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PASSWD");
		columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);

        // Bohong++
        std::vector<unsigned long> operatorIds;
        std::vector<IData *>       iDatas;		

        do
        {

            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
                operatorIds.push_back(operatorId);
            }

            //delete data;
            iDatas.push_back(data);
            data = NULL;
        }

        while( pDatabase->moreData(data) );

        // Bohong++, we have to get out all the operatorId first, then new Operator one by one.
        // Cos the DatabaseFactory is a singleton, we should not do any db operation until previous
        // operation is finished
        std::vector<IData *>::iterator it = iDatas.begin();

        for( int j = 0; it != iDatas.end(); ++it, ++j )
        {

            for (unsigned long i = 0; i < (*it)->getNumRows() ; ++i )
            {

                std::vector<unsigned long> profiles;
                getProfileByOperator(operatorIds[i+(j*FETCH_MAX_ROWS)], profiles);

                std::vector<unsigned long> regions;
                getRegionByOperator(operatorIds[i+(j*FETCH_MAX_ROWS)], regions);

                if (readWrite) // need a config operator
                {
	                operators.push_back( new ConfigOperator(i, *(*it), profiles, regions) );
                }

                else // need a standard operator
                {
	                operators.push_back( new Operator(i, *(*it), profiles, regions) );
                }
            }

            delete *it;
        }
    }


	void OperatorAccessFactory::getOperatorInfoBySql( 
		IDatabase* pDatabase,
		const SQLStatement& sql,
		std::vector<IOperator*>& operators )
    {
        // get a connection to the database
      //  IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		TA_ASSERT(0 != pDatabase, "the database connection is null");
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PASSWD");
		columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);
		
         
        std::vector<unsigned long> operatorIds;
        std::vector<IData *>       iDatas;		
		
        do
        {
			std::auto_ptr<IData> datas( data );

            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
				std::string  operatorName = data->getStringData(i, columnNames[1]);                
				bool is_system=data->getBooleanData(i, columnNames[4]);           
				operators.push_back( new Operator(operatorId,operatorName,is_system ) );
            }          
            data = NULL;
        }

        while( pDatabase->moreData(data) );
		
       
         
    }


    IOperator* OperatorAccessFactory::getOperator(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getOperator");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where PKEY = " << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_Oracle_SELECT_6051, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6051, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6051, key);
//		TA_ASSERT(sql.size() > 0, "getOperator: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
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

		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return operators[0];
    }

	//TD15069++
	std::string OperatorAccessFactory::getDeletedOperatorName(const unsigned long key)
    {
		FUNCTION_ENTRY("getDeletedOperatorName");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where DELETED <> 0 and PKEY = " << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_Oracle_SELECT_6052, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6052, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6052, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedOperatorName: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, false);

        if ( 0 == operators.size())
		{
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
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
		
		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

		IOperator* OperatorAccess = operators[0];
		std::string name;
		try
		{
			name = OperatorAccess->getName();
			delete OperatorAccess;
			OperatorAccess = NULL;
		}
		//exception
        catch(...)
        {
            delete OperatorAccess;
            OperatorAccess = NULL;
            throw;
        }

		FUNCTION_EXIT;
        return name;
    }
	//++TD15069

	//TD15069++
	bool OperatorAccessFactory::getDeletedOperatorIsSystemOperator(const unsigned long key)
    {
		FUNCTION_ENTRY("getDeletedOperatorIsSystemOperator");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where DELETED <> 0 and PKEY = " << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_Oracle_SELECT_6052, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6052, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6052, key);
//		TA_ASSERT(sql.size() > 0, "getDeletedOperatorIsSystemOperator: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, false);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
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

		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

		IOperator* OperatorAccess = operators[0];
        bool isSystem = false;

		try
		{
			isSystem = OperatorAccess->isSystemOperator();
			delete OperatorAccess;
			OperatorAccess = NULL;
		}
		//exception
        catch(...)
        {
            delete OperatorAccess;
            OperatorAccess = NULL;
            throw;
        }

		FUNCTION_EXIT;
        return isSystem;
     }
	//++TD15069

    IOperator* OperatorAccessFactory::getOperator(const std::string& name,const bool readWrite)
    {
		FUNCTION_ENTRY("getOperator");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the name of the operator
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
		/*std::ostringstream sql;
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR_V where NAME = '"
			<< databaseConnection->escapeQueryString(name) << "'";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			OPERATOR_V_Oracle_SELECT_6551, databaseConnection->escapeQueryString(name));
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6551, databaseConnection->escapeQueryString(name));
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6551, databaseConnection->escapeQueryString(name));
//		TA_ASSERT(sql.size() > 0, "getOperator: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with name " << name;
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

		TA_ASSERT(1 == operators.size(), "Operator name unique constraint violated");

        // Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return operators[0];
    }

    IOperator* OperatorAccessFactory::getOperatorFromSession(const std::string& sessionId, const bool readWrite)
    {
		FUNCTION_ENTRY("getOperatorFromSession");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

		/*std::ostringstream sql;
		sql << " select a.PKEY, a.NAME, a.DESCRIPTION, a.PASSWD, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR a, TA_SESSION b where a.PKEY = b.OPERATORKEY and b.PKEY = '"
			<< databaseConnection->escapeQueryString(sessionId) << "'";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_Oracle_SELECT_6053,
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6053,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6053,
			databaseConnection->escapeQueryString(sessionId));
//		TA_ASSERT(sql.size() > 0, "getOperatorFromSession: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with SessionId " << sessionId;
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
		
		TA_ASSERT(1 == operators.size(), "Operator SessionKey unique constraint violated");

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators[0];
    }

    std::vector<IOperator*> OperatorAccessFactory::getAllOperators( const bool readWrite )
    {
		FUNCTION_ENTRY("getAllOperators");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V";*/ 
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6552);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6552);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6552);
//		TA_ASSERT(sql.size() > 0, "getAllOperators: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite, true, false, false, false);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


	std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsInfo(   )
    {
		FUNCTION_ENTRY("getAllOperators");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		
        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V";*/ 
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6552);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6552);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6552);
//		TA_ASSERT(sql.size() > 0, "getAllOperatorsInfo: Get SQL Statement error");
		
		std::vector<IOperator*> operators;
		getOperatorInfoBySql(databaseConnection, sql, operators);
		
		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsAndRegions( const bool readWrite )
    {
		FUNCTION_ENTRY("getAllOperatorsAndRegions");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V";*/ 
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6552);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6552);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6552);
//		TA_ASSERT(sql.size() > 0, "getAllOperatorsAndRegions: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite, true, false, false, true);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsAndAttributes()
    {
        return getAllOperators(false);
    }
    
    
    std::vector<IOperator*> OperatorAccessFactory::getAllNonSystemOperators( const bool readWrite, 
                                                                             const bool getProfiles, /*=true*/
                                                                             const bool getRegions /*=false*/ )
    {
		FUNCTION_ENTRY("getAllNonSystemOperators");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the guis
		/*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V where IS_SYSTEM = 0"; */
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6553);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6553);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6553);
//		TA_ASSERT(sql.size() > 0, "getAllNonSystemOperators: Get SQL Statement error");

		std::vector<IOperator*> operators;
		getOperatorBySql(databaseConnection, sql, operators, readWrite, getProfiles, false, getProfiles, getRegions);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    IConfigOperator* OperatorAccessFactory::createOperator()
    {            
        return new ConfigOperator();    
    }


    IConfigOperator* OperatorAccessFactory::copyOperator(const IConfigOperator* operatorToCopy)
    {
        FUNCTION_ENTRY("copyOperator");

        TA_ASSERT(operatorToCopy !=  NULL, "The operator to copy was NULL");
        
        const ConfigOperator* cast = dynamic_cast<const ConfigOperator*>(operatorToCopy);

        TA_ASSERT(cast != NULL, "Operator passed could not be converted into a ConfigOperator");

        FUNCTION_EXIT;
        return new ConfigOperator(*cast);
    }


} // closes TA_Base_Core


