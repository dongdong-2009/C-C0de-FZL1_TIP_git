/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/DatabaseQueryHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Database helper is a namespace containing a set of database helper functions
  *
  */

#if !defined(DatabaseQueryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define DatabaseQueryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/database/src/CommonType.h"


namespace TA_Base_Core
{

    class TA_Base_Core::IDatabase;

    namespace DatabaseQueryHelper
    {
    
        struct QueryData
        {
            // The actual SQL statement (to execute)
            //std::string statement;
			SQLStatement sqlStatement;
            // The column names associated with 'statement'
            std::vector<std::string> columnNames;
            // The primary key column name
            std::string primaryKeyCol;
            // Table the query is being executed on
            std::string tableName;            
        };

        /** 
         * getNextSequenceNumber
         *
         * Returns the next sequence number value for specified sequence counter (which
         *  will subsequently be automatically incremented)
         *
         * @param databaseConnection the database connection to use to perform query
         * 
         * @param seqName the name of the sequence whose value we want to retrieve, ie PAZONE_SEQ
         *
         * @return the next auto increment ID (and the sequence number will be updated in database)
         *
         * @exception DataException if the next auto increment ID value isn't available (error
         *                          occurred fetching the value
         *
         * @exception DatabaseException If there is an error with the database connection
         *
         */
        unsigned long getNextSequenceNumber(TA_Base_Core::IDatabase* databaseConnection, const std::string& seqName); //throw(TA_Core::TransactiveException);

        /**
         * checkIfExistLinksToTableRow
         *
         * Checks if any rows reference the input table/row combination
         *
         * @param checkTableName the name of the table to check for incoming references
         *
         * @param checkTablePrimaryKeyColName the name of the primary key column for check table
         *
         * @param checkTablePrimaryKeyValue the primary key of the row we're checking
         *
         * @param info [out] upon return of true, provides additional information about what table
         *      in the format of "A reference exists to checkTableName from externalTable..."
         *
         * @return true if there exist any references from other table rows to the check
         *      table row (ie referencing of the check primary key)
         *
         * @exception DataException if type conversion error
         *
         * @exception DatabaseException if database access error
         * 
         */
        bool checkIfExistLinksToTableRow(
                        const std::string& checkTableName,
                        const std::string& checkTablePrimaryKeyColName,
                        const std::string& checkTablePrimaryKeyValue,
                        std::string& info);

        /**
         * checkIfExistLinksToTableRow
         * 
         * Converts input key value to string key value and passes on to main function
         *
         * @see checkIfExistLinksToTableRow
         *
         */
        bool checkIfExistLinksToTableRow(
                        const std::string& checkTableName,
                        const std::string& checkTablePrimaryKeyColName,
                        unsigned long checkTablePrimaryKeyValue,
                        std::string& info);

        
        /**
         * appendWhereClause
         *
         * Appends a where clause to an SQL statement, which may or may not already have
         *      where clause elements (if none, a 'where' statement is added, otherwise
         *          the function appends 'and' rather than using 'where')
         * The function uses where surrounded by whitespace as presence indicator
         *
         * @param selectStatement [OUT] the select statement to modify
         *
         * @param whereClause the clause to add to the statement, in form COL=x
         *          Do not include 'where' tags in the whereclause, may use 'and' if 
         *          adding multiple conditions
         *
         * @pre there does not exist a 'where' tag in the whereClause string
         *
         */
        void appendWhereClause(SQLStatement& rSqlStatement, 
                                        const std::string& whereClause);


        /**
         * isWholeWordPresent
         *
         * @return true if the whole word is present in input string, and it's separated
         *          by whitespace (or at start or end of string)
         *
         * @param input string to search through
         * 
         * @param word the word to search for [case insensitive]
         *
         */
        bool isWholeWordPresent(std::string input, std::string word);


        /**
         * createBasicSelectStatement
         *
         * Creates a basic select statement, top select all the columns
         * from the input table
         *
         * @pre if requireDateColumns is true, then 
         *          there exists a "DATE_CREATED" and "DATE_MODIFIED" column
         *          in the input columns (case sensitive)
         *
         * @pre tableName is not empty
         *
         * @param columnNames the names of the columns to retrieve in query
         *
         * @param tableName the name of the table the query is to operate on
         *
         * @param requireDateColumns(optional) is a special case override parameter, to
         *              disable the date column presence verification
         */
        void createBasicSelectStatement(
							SQLStatement& rSqlStatement,
                            const std::vector<std::string>& columnNames,
                            const std::string& tableName,
                            bool requireDateColumns = true);

        /**
         * getDatabaseRecordsReturnedFromStatement
         *
         * @return a set of records returned from the input SQL statement
         *
         * @pre the queryData.primaryKeyCol exists as the first element
         *          in the queryData.columnNames structure
         *
         * @param queryData the data used to construct and execute query
         *
         * @param whereClause any optional where clause specifications for
         *          the select statement, empty if not required
         *
         * @param readWrite true to make objects of type TConfigurableRecordClass
         *          and place them into returned array, false to create 
         *          nonconfigurable objects of type TRecordClass in returned array
         *
         * @exception DataException if type conversion error
         *
         * @exception DatabaseException if database access error
         * 
         */
        template <class TRecordSet, class TConfigurableRecordClass, class TRecordClass>
        TRecordSet getDatabaseRecordsReturnedFromStatement(const QueryData& queryData,
                                                           const std::string& whereClause, 
                                                           const bool readWrite);
    }


    //////////////////////////////////////////////////
    // Inline definitions
    // Template functions need to be defined in header
    namespace DatabaseQueryHelper
    {
        template <class TRecordSet, class TConfigurableRecordClass, class TRecordClass>
        TRecordSet getDatabaseRecordsReturnedFromStatement(const QueryData& queryData,
                                                            const std::string& whereClause, 
                                                            const bool readWrite)
        {
            TA_ASSERT(queryData.columnNames[0].compare(queryData.primaryKeyCol) == 0,
                        "First column is not primary key column");
                
            // create the SQL string to retrieve the entire set
            /*std::ostringstream sql;
            sql << queryData.statement;*/
			QueryData v_queryData(queryData);
    
            if (whereClause.length() > 0)
            {
                //DatabaseQueryHelper::appendWhereClause(sql, whereClause);
				DatabaseQueryHelper::appendWhereClause(v_queryData.sqlStatement, whereClause);
            }        
    
            // get a connection to the database
            TA_Base_Core::IDatabase* databaseConnection = 
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
    
            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            
			//waitforcheck delete by lin
			/*SQLStatement strSql;
			strSql.strCommon = sql.str();*/
            TA_Base_Core::IData* data = databaseConnection->executeQuery(v_queryData.sqlStatement, queryData.columnNames);
        
            // create the vector of pointers to IEntityData
            TRecordSet recordPointers;
    
            do
            {
                for (unsigned long i = 0; i < data->getNumRows() ; ++i )
                {   
                    if (0 != data->getUnsignedLongData(i, queryData.primaryKeyCol))
                    {
                        if (readWrite)
                        {
                            recordPointers.push_back( new TConfigurableRecordClass(i, *data));
                        }
                        else
                        {
                            recordPointers.push_back( new TRecordClass(i, *data));
                        }
                    }
                }
    
                delete data;
                data = NULL;
            }
            while( databaseConnection->moreData(data) );
    
            return recordPointers;

            if (false)
            {
                // This is a test to confirm the client code
                // didn't accidentally pass in the non-configurable
                // object as the configurable one and vice-versa
                // Only the configurable object has the deleteThisObject function
                // (Compile time error if client code is incorrect - this
                // code is never intended to actually execute)
                TConfigurableRecordClass* nullObject = 0;
                nullObject->applyChanges();
            }
        }

    }   // END NAMESPACE
}   

#endif // !defined(DatabaseQueryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
