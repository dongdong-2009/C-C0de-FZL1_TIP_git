/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportDataAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include "EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{
	EquipReportDataAccessFactory* EquipReportDataAccessFactory::m_instance = 0;

	EquipReportDataAccessFactory& EquipReportDataAccessFactory::getInstance()
	{
        if( m_instance == 0 )
        {
            m_instance = new EquipReportDataAccessFactory();
        }
        return *m_instance;
	}

	void EquipReportDataAccessFactory::getEquipReportDatumBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IEquipReportData*>& reportDatum, const bool readWrite)
	{
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("SCEREN_ID");
		columnNames.push_back("SCEREP_ID");
		columnNames.push_back("DATANODE_ID");
		columnNames.push_back("DATAPOINT_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        if ("0" ==  data->getStringData( i, columnNames[0]))
                        {
                            // if we have the equipment report where the key is "0" then we are not interested in it
                            // The user cannot configure this particular equipment report
                            continue;
                        }

                        reportDatum.push_back(new ConfigEquipReportData(i, *data));
                    }
                    else
                    {
                        reportDatum.push_back(new EquipReportData(i, *data));
                    }
                }
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


    IEquipReportData *EquipReportDataAccessFactory::getEquipReportDataByKey(std::string& aEquipReportDataKey,  const bool readWrite)
	{
		FUNCTION_ENTRY("getEquipReportDataByKey");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_STD_SELECT_15001, aEquipReportDataKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15001, aEquipReportDataKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15001, aEquipReportDataKey);
//		TA_ASSERT(strSql.size() > 0, "EquipReportDataAccessFactory::getEquipReportDataByKey(): Prepare SQL Statement error");

		// set up the sql string
        /*std::ostringstream sql;
        sql << " select SCEREN_ID, SCEREP_ID, DATANODE_ID, DATAPOINT_ID";
		sql << " from SC_EQUIP_REPORT_ENTITIES where SCEREN_ID = '" << aEquipReportDataKey << "'";*/

		std::vector<IEquipReportData*> reportDatum;
		getEquipReportDatumBySql(databaseConnection, strSql, reportDatum, readWrite);

        if ( 0 == reportDatum.size())
        {
            std::ostringstream message;
			message << "No data found for EquipReportData with key " << aEquipReportDataKey;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
            
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

		TA_ASSERT(1 == reportDatum.size(), "EquipReportData key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reportDatum[0];
	}

	IConfigEquipReportData* EquipReportDataAccessFactory::copyEquipReportData(const IConfigEquipReportData *aEquipReportDataToCopy)
	{
		const ConfigEquipReportData* cast = dynamic_cast<const ConfigEquipReportData*>(aEquipReportDataToCopy);

		return new ConfigEquipReportData(*cast);
	}

	void EquipReportDataAccessFactory::copyEquipReportData(std::string& aBaseReportKey, std::string& aNewReportKey)
	{
		 FUNCTION_ENTRY("copyEquipReportData");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_STD_SELECT_15002, aBaseReportKey, aNewReportKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15002, aBaseReportKey, aNewReportKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15002, aBaseReportKey, aNewReportKey);
//		TA_ASSERT(strSql.size() > 0, "EquipReportDataAccessFactory::copyEquipReportData(): Prepare SQL Statement error");

        /*std::ostringstream sqlProcedureCall;
        sqlProcedureCall << "call PROC_COPY_EQUIP_REPORT_DATA('" << aBaseReportKey;
        sqlProcedureCall << "', '" << aNewReportKey << "')";*/

        databaseConnection->executeProcedure(strSql);

        FUNCTION_EXIT;
	}


	IConfigEquipReportData* EquipReportDataAccessFactory::createEquipReportData()
	{
		return new ConfigEquipReportData(); 
	}

	IEquipReportData* EquipReportDataAccessFactory::getEquipReportDataKey(std::string& aReportKey, unsigned long aEntityKey)
	{
		if (aEntityKey == 0)
		{
			return NULL;
		}

		FUNCTION_ENTRY("getEquipReportDataKey");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_STD_SELECT_15003, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15003, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15003,
			aReportKey, aEntityKey, aEntityKey);
//		TA_ASSERT(strSql.size() > 0, "EquipReportDataAccessFactory::getEquipReportDataKey(): Prepare SQL Statement error");

		// create the SQL string to retrieve one Equipment Report Data for a particular ReportKey
		// and entity key it contains
        /*std::ostringstream sql;
        sql << " select SCEREN_ID, SCEREP_ID, DATANODE_ID, DATAPOINT_ID FROM SC_EQUIP_REPORT_ENTITIES"
			<< " where SCEREP_ID = '" << aReportKey << "'"
			<< " and (DATANODE_ID = " << aEntityKey << " or DATAPOINT_ID = " << aEntityKey << ")";*/
  
		std::vector<IEquipReportData*> reportDatum;
		getEquipReportDatumBySql(databaseConnection, strSql, reportDatum, false);

        if ( 0 == reportDatum.size())
        {
            std::ostringstream message;
			message << "No data found for EquipReportData with ReportKey = " << aReportKey << " and EntityKey = " << aEntityKey;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
            
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
		
		TA_ASSERT(1 == reportDatum.size(), "EquipReportData ReportKey and EntityKey unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reportDatum[0];
	}

	std::vector<IEquipReportData* >EquipReportDataAccessFactory::getAllEquipReportDatumByReportKey(std::string& aReportKey, const bool readWrite)
	{
		FUNCTION_ENTRY("getAllEquipReportDatumByReportKey");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_STD_SELECT_15004, aReportKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15004, aReportKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15004, aReportKey);
//		TA_ASSERT(strSql.size() > 0, "EquipReportDataAccessFactory::getAllEquipReportDatumByReportKey(): Prepare SQL Statement error");

		// create the SQL string to retrieve all Equipment Report Datum for a particular ReportKey
       /* std::ostringstream sql;
        sql << "select SCEREN_ID, SCEREP_ID, DATANODE_ID, DATAPOINT_ID FROM SC_EQUIP_REPORT_ENTITIES "
			<< "where SCEREP_ID = '" << aReportKey << "'";*/

		std::vector<IEquipReportData*> reportDatum;
		getEquipReportDatumBySql(databaseConnection, strSql, reportDatum, false);
		
		// Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reportDatum;
	}
}
