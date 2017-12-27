/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrintCfgAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by:  $Author: huirong.luo $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER


#include "core/data_access_interface/src/OnlinePrintCfgAccessFactory.h"
#include "core/data_access_interface/src/OnlinePrintCfg.h"
#include "core/data_access_interface/src/ConfigOnlinePrintCfg.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    static const std::string DEFAULT_PRINTER_COL = "DEF_PRINTER";
    static const std::string LOCATION_COL = "LOCATIONKEY";
    static const std::string LOCATION_NAME_COL = "LOCATION.NAME";
	static const std::string LOCATION_DISPALYNAME_COL = "LOCATION.DISPLAY_NAME";

    OnlinePrintCfgAccessFactory* OnlinePrintCfgAccessFactory::m_instance = 0;

    OnlinePrintCfgAccessFactory& OnlinePrintCfgAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new OnlinePrintCfgAccessFactory();
        }
        return *m_instance;
    }


    void OnlinePrintCfgAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    void OnlinePrintCfgAccessFactory::getOnlinePrintCfgBySql(IDatabase* pDatabase, const SQLStatement& sql, IOnlinePrintCfgs& OnlinePrintCfgs, const bool readWrite)
    {
        // get a connection to the database
        //TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(DEFAULT_PRINTER_COL);
        columnNames.push_back(LOCATION_COL);
        columnNames.push_back(LOCATION_NAME_COL);
		columnNames.push_back(LOCATION_DISPALYNAME_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = pDatabase->executeQuery(sql, columnNames);
        
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                if (0 == data->getUnsignedLongData(i, LOCATION_COL) )
                {
					continue;
				}

                if (readWrite)
                {
                    OnlinePrintCfgs.push_back(new ConfigOnlinePrintCfg(i, *data));
                }
                else
                {
                    OnlinePrintCfgs.push_back(new OnlinePrintCfg(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while( pDatabase->moreData(data) );
    }

    std::vector<OnlinePrintCfgAccessFactory::LocationRecord> OnlinePrintCfgAccessFactory::getUnusedLocations()
    {
		FUNCTION_ENTRY("getUnusedLocations");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7001);
//		TA_ASSERT(strSql.size() > 0, "OnlinePrintCfgAccessFactory::getUnusedLocations(): Prepare SQL Statement error");

        /*std::ostringstream sql;
		sql << "select PKEY, NAME from LOCATION"
            << " where not exists (select 0 from ONLINE_PRINTCFG where LOCATIONKEY = LOCATION.PKEY)"
            << " order by NAME";*/

        std::vector<std::string> columnNames;
		columnNames.push_back("PKEY");
		columnNames.push_back("NAME");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        std::vector<LocationRecord> ret;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            { 
                unsigned long id = data->getUnsignedLongData(i, "PKEY");
                if (id != 0)
                {
                    LocationRecord rec;
                    rec.key = id;
                    rec.name = data->getStringData(i, "NAME");
                    ret.push_back(rec);
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
        return ret;
    }
    
    IOnlinePrintCfg* OnlinePrintCfgAccessFactory::getOnlinePrintCfg(const unsigned long idKey,const bool readWrite)
    {
		FUNCTION_ENTRY("getOnlinePrintCfg");

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_SELECT_18501, DEFAULT_PRINTER_COL,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18501, DEFAULT_PRINTER_COL,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18501, DEFAULT_PRINTER_COL,
			LOCATION_COL, LOCATION_NAME_COL, LOCATION_COL, LOCATION_COL, idKey );
//		TA_ASSERT(strSql.size() > 0, "OnlinePrintCfgAccessFactory::getOnlinePrintCfg(): Prepare SQL Statement error");

        // create the SQL string to retrieve the name of the OnlinePrintCfg
        // this is a check to ensure an OnlinePrintCfg with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
		sql << "select " << DEFAULT_PRINTER_COL << "," << LOCATION_COL << "," << LOCATION_NAME_COL
            << " from ONLINE_PRINTCFG, LOCATION where " 
			<< LOCATION_COL << " = LOCATION.PKEY and "
            << LOCATION_COL << " = " << idKey;*/

		IOnlinePrintCfgs OnlinePrintCfgs;
		getOnlinePrintCfgBySql(databaseConnection, strSql, OnlinePrintCfgs, readWrite);

        if ( 0 == OnlinePrintCfgs.size())
        {
            std::ostringstream message;
			message << "No data found for OnlinePrintCfg with key " << idKey;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
            
			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }

		TA_ASSERT(1 == OnlinePrintCfgs.size(), "OnlinePrintCfg key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;			
		return OnlinePrintCfgs[0];
    }
    
    IOnlinePrintCfgs OnlinePrintCfgAccessFactory::getAllOnlinePrintCfgs(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllOnlinePrintCfgs");

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_SELECT_18502, DEFAULT_PRINTER_COL,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18502, DEFAULT_PRINTER_COL,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18502, DEFAULT_PRINTER_COL,
			LOCATION_COL, LOCATION_NAME_COL, LOCATION_COL, LOCATION_COL );
//		TA_ASSERT(strSql.size() > 0, "OnlinePrintCfgAccessFactory::getAllOnlinePrintCfgs(): Prepare SQL Statement error");

        // create the SQL string to retrieve the data of the OnlinePrintCfg
        // based upon the key
       /* std::ostringstream sql;
		sql << "select " << DEFAULT_PRINTER_COL << ", " << LOCATION_COL << "," << LOCATION_NAME_COL
            << " from ONLINE_PRINTCFG, LOCATION where "
            << LOCATION_COL << " = LOCATION.PKEY"
            << " ORDER BY " << LOCATION_COL;*/

		IOnlinePrintCfgs OnlinePrintCfgs;
		getOnlinePrintCfgBySql(databaseConnection, strSql, OnlinePrintCfgs, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;			
		return OnlinePrintCfgs;
    }
    

    IConfigOnlinePrintCfg* OnlinePrintCfgAccessFactory::createOnlinePrintCfg()
    {            
        return new ConfigOnlinePrintCfg();
    }


    IConfigOnlinePrintCfg* OnlinePrintCfgAccessFactory::copyOnlinePrintCfg(const IConfigOnlinePrintCfg* OnlinePrintCfgToCopy)
    {
        TA_Assert(false);
        return NULL;
    }


} // closes TA_Core


