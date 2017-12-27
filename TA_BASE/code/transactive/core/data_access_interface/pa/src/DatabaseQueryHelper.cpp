/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/DatabaseQueryHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Database helper is a simple class used to define static database helper functions
  */

#pragma warning(disable:4786 4290)

#include <vector>
#include <algorithm>

#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    unsigned long DatabaseQueryHelper::getNextSequenceNumber(TA_Base_Core::IDatabase* databaseConnection, const std::string& seqName) //throw(TA_Core::TransactiveException)
    {
        const std::string key_col("NEXTVAL");        
        //std::ostringstream sql;

        // This is required command to obtain sequence numbers
        /*sql << "SELECT " << seqName << ".NEXTVAL FROM DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33055, seqName);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33055, seqName);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33055, seqName);

        std::vector<std::string> columnNames;
        columnNames.push_back(key_col);

        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        try 
        {
            // Should always be single row result
            TA_ASSERT(data->getNumRows() == 1, "Invalid result from getNextSequenceNumber");

            // Try and retrieve the value of auto increment ID
            unsigned long result = data->getUnsignedLongData(0, key_col);

            // Clean up
            delete data;
            data = NULL;

            return result;
        }
        catch ( TA_Base_Core::DataException& ) 
        {
            // Clean up before rethrowing
            delete data;
            data = NULL;

            throw;
        }
    }


    bool DatabaseQueryHelper::checkIfExistLinksToTableRow(
                    const std::string& checkTableName,
                    const std::string& checkTablePrimaryKeyColName,
                    const std::string& checkTablePrimaryKeyValue,
                    std::string& info)
    {
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
    
        //const char* tableName = strlwr(const_cast<char*>(
        //    (databaseConnection->escapeInsertString(checkTableName)).c_str()));
        //const char* tablePrimaryKeyColName = 
        //    (databaseConnection->escapeInsertString(checkTablePrimaryKeyColName)).c_str();
        //const char* tablePrimaryKeyValue = 
        //    (databaseConnection->escapeInsertString(checkTablePrimaryKeyValue)).c_str();
		std::string strTableName = databaseConnection->escapeInsertString(checkTableName);
		std::string strTablePrimaryKeyColName = databaseConnection->escapeInsertString(checkTablePrimaryKeyColName);
		std::string strTablePrimaryKeyValue = databaseConnection->escapeInsertString(checkTablePrimaryKeyValue);
        
        // Use statement to extract all constraints for the check table
        //delete by lin
        //std::ostringstream sql;
        //sql << "select "
        //    << "b.table_name " // [Constraint name not needed] , b.constraint_name
        //    << "from "
        //    << "user_constraints b, user_constraints a "
        //    << "where "
        //    << "a.table_name='" << tableName << "'      and "
        //    << "b.r_owner = a.owner                     and "
        //    << "b.r_constraint_name = a.constraint_name and "
        //    << "a.constraint_type = 'P'";
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33056, tableName);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33056, tableName);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33056, strTableName.c_str());

        std::vector<std::string> columnNames;
        columnNames.push_back("table_name");

        unsigned int tableId;
        std::vector<std::string> externalTables;
        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
 
        for (tableId = 0; tableId < data->getNumRows(); tableId ++)
        {
            externalTables.push_back(data->getStringData( tableId, "table_name"));
        }

        delete data;
        data = 0;

        // For each constraint, perform query on that table to see if references exist
        for (tableId = 0; tableId < externalTables.size(); tableId ++)
        {
            //const char* exTableName = 
            //    strlwr( const_cast<char*>((databaseConnection->escapeInsertString(externalTables[tableId])).c_str()) );
			std::string strExTableName = databaseConnection->escapeInsertString(externalTables[tableId]);
			//delete by lin
            //sql.str("");
            // Select all rows from this external table whose foreign
            // key matches the check tables primary key value
            // Surround tablePrimaryKeyValue by quotes just in case a string            
            //sql << "select " << tablePrimaryKeyColName
            //    << " from " << exTableName
            //    << " where " << tablePrimaryKeyColName 
            //    << " = '" << tablePrimaryKeyValue << "'";

			//data = databaseConnection->executeQuery(sql.str(), columnNames);
			
			//waitforcheck delete by lin
			SQLStatement strSql;	
			databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_PRIVATE_VERSION_SELECT_42002,
				   strTablePrimaryKeyColName.c_str(), strExTableName.c_str(), strTablePrimaryKeyColName.c_str(),  strTablePrimaryKeyValue.c_str());
            data = databaseConnection->executeQuery(strSql, columnNames);

            int numLinks = data->getNumRows();

            delete data;
            data = 0;

            // May return early if links found
            if (numLinks > 0)
            {
                std::ostringstream msg;
                msg << numLinks << " row(s) in table " << strExTableName.c_str()
                    << " reference the row in table " << checkTableName.c_str() 
                    << " where the pKey=" << checkTablePrimaryKeyValue.c_str();
                                
                // Update output info reference
                info = msg.str();
                return true;
            }
        }        

        // No external references found
        return false;
    }

    bool DatabaseQueryHelper::checkIfExistLinksToTableRow(
                    const std::string& checkTableName,
                    const std::string& checkTablePrimaryKeyColName,
                    unsigned long checkTablePrimaryKeyValue,
                    std::string& info)
    {
        std::ostringstream buf;
        buf << checkTablePrimaryKeyValue;

        return checkIfExistLinksToTableRow(checkTableName, checkTablePrimaryKeyColName, buf.str().c_str(), info);
    }


    bool DatabaseQueryHelper::isWholeWordPresent(std::string input, std::string word)
    {
        std::transform(input.begin(), input.end(), input.begin(), toupper);
        std::transform(word.begin(), word.end(), word.begin(), toupper);

        std::string::size_type offset;

        while (std::string::npos != (offset = input.find(word)))
        {
            if (word.length() == input.length())
            {
                // Special case, both strings match
                return true;
            }

            // Keep searching for where, each instance, confirm whether or not
            // it's entirely by itself (whitespace separated)
            if (0 == offset)
            {
                // At head of string, only needs separation on right side
                if (input.at(offset + word.length()) == ' ')
                {
                    return true;
                }
            }
            else if (offset < input.size() - word.length())
            {
                // In middle, both sides must match
                if (input.at(offset - 1) == ' ' && 
                            input.at(offset + word.length()) == ' ')
                {
                    return true;
                }
            }
            else
            {
                // This should be satisfied by word.length() == input.length() test
                TA_ASSERT(offset >= 1, "Invalid internal state");

                // At end of input string, only needs separation on left side
                if (input.at(offset - 1) == ' ')
                {
                    return true;
                }
            }
        }

        // No match found
        return false;
    }


    void DatabaseQueryHelper::appendWhereClause(SQLStatement& rSqlStatement, 
                                                const std::string& whereClause)
    {        
        TA_ASSERT(!isWholeWordPresent(whereClause, "where"), 
                                "User should not include where clause");
		std::string strStatement;
		if (!rSqlStatement.strCommon.empty())
		{
			strStatement = rSqlStatement.strCommon;
		}
		else
		{
			strStatement = rSqlStatement.strOracle;
		}

		SQLTypeAdapter sqlParam;

        if (isWholeWordPresent(strStatement, "where"))
        {
            // There is already a where clause
            //selectStatement << " and " << whereClause;
			sqlParam.add(rSqlStatement, std::string(" and ") + whereClause);
        }
        else
        {
            // There was no existing where clause
            //selectStatement << " where " << whereClause;
			sqlParam.add(rSqlStatement, std::string(" where ") + whereClause);
        }
    }


    void DatabaseQueryHelper::createBasicSelectStatement(
							SQLStatement& rSqlStatement,
                            const std::vector<std::string>& columnNames,
                            const std::string& tableName,
                            bool requireDateColumns)
    {
        TA_ASSERT(tableName.length() > 0, "NULL tablename");

        const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
        const std::string DATECREATED_COL = "DATE_CREATED";
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		//int  nDbType = databaseConnection->getDbServerType();

		 const char* exTableName =  // make sure that the table name in lower case oyzl++
			 strlwr(const_cast<char*>(tableName.c_str()));  

        /*std::ostringstream sql;
		sql << "select ";*/
		SQLTypeAdapter sqlParam;

        enum { DATE_MOD_CHECK = 1, DATE_CREATE_CHECK = 2 };
        int checkFlags = 0;

        // Add in all column names to select statement
        for (unsigned int i = 0; i < columnNames.size(); i ++)
        {
            if (columnNames[i].compare(DATEMODIFIED_COL) == 0)
            {
				 // Special case
				/*if ( enumOracleDb == nDbType )
				{
					sql << "TO_CHAR(nvl(" 
						<< DATEMODIFIED_COL 
						<< ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as DATE_MODIFIED";   
				}
				else if ( enumMysqlDb == nDbType )
				{
					sql << "DATE_FORMAT(IF("
						<< DATEMODIFIED_COL
						<< " IS NULL,STR_TO_DATE('12:00:00 01/01/1990','%H:%i:%S %d/%m/%Y'),"
						<< DATEMODIFIED_COL
						<< "),'%Y%m%d%H%i%S') as DATE_MODIFIED";
				}*/
				sqlParam.add(enumSyntax_Format_3);

                checkFlags |= DATE_MOD_CHECK;
            }
            else if (columnNames[i].compare(DATECREATED_COL) == 0)
            {
                // Special case
				/*if ( enumOracleDb == nDbType )
				{
					sql << "TO_CHAR(" 
						<< DATECREATED_COL 
						<< ",'YYYYMMDDHH24MISS')";
				}
				else if ( enumMysqlDb == nDbType )
				{
					sql << "DATE_FORMAT("
						<< DATECREATED_COL
						<< ",'%Y%m%d%H%i%S')";
				} */  
				sqlParam.add(DATECREATED_COL, enumDateFormat_4, enumFuncFormat_10);

                checkFlags |= DATE_CREATE_CHECK;
            }
            else
            {
                //sql << columnNames[i].c_str();
				sqlParam.add(columnNames[i]);
            }
            
            // Append commas for all but final element
            if (i < (columnNames.size() - 1))
            {
                //sql << ", ";
				sqlParam.add(std::string(", "));
            }
        }

        // Confirm both flags set
        if (requireDateColumns)
        {
            TA_ASSERT(checkFlags & (DATE_MOD_CHECK | DATE_CREATE_CHECK), "Internal check failed");
        }
        
        //sql << " from " << exTableName;
		databaseConnection->prepareSQLStatement(rSqlStatement, PA_FIRE_COUNTDOWN_SELECT_OCCFIREABORT_75005, sqlParam, exTableName);

        //return sql.str();
    }    
}
