/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/GenericDaiHelper.inl $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Provides common functionality between Data Access Interface helper
  *     classes (to enforce consistency between classes)
  *
  */



#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/DatabaseKey.h"

namespace TA_Base_Core
{   
    template <class DerivedClass>
    DatabaseQueryHelper::QueryData 
        GenericDaiHelper<DerivedClass>::getBasicQueryData()
    {
        // NOTE: If the template class compilation is failing (C2352)
        // on this line, it means the developer hasn't defined
        // the static member funciton getBasicQueryData() (once
        // this is done, will work fine)
        return DerivedClass::getBasicQueryData();
    }    


    template <class DerivedClass>
    void GenericDaiHelper<DerivedClass>::reload()
    {
        reload(getBasicQueryData());
    }


    template <class DerivedClass>
    void GenericDaiHelper<DerivedClass>::reload(
                const DatabaseQueryHelper::QueryData& queryData)
    {
        FUNCTION_ENTRY("reload" );

        unsigned long key = TA_Base_Core::DatabaseKey::getInvalidKey();

        try
        {
            key = getKeyDirect();
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
        }

        // Ensure the record has already been written to the database. It doesn't make
        // any sense to reload an record that doesn't exist in the database.
        TA_ASSERT(TA_Base_Core::DatabaseKey::isValidKey(key), 
                    "This DB Object does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the data of the record
        // based upon the key
       /* std::ostringstream sql;
        sql << queryData.statement;*/
		DatabaseQueryHelper::QueryData v_queryData(queryData);
        
        std::ostringstream whereClause;
        whereClause <<  queryData.primaryKeyCol.c_str() << " = " << key;

        //DatabaseQueryHelper::appendWhereClause(sql, whereClause.str());
		DatabaseQueryHelper::appendWhereClause(v_queryData.sqlStatement, whereClause.str());
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        //TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), queryData.columnNames);
                                                       
                                                       
		//waitforcheck delete by lin
		/*SQLStatement strSql;
		strSql.strCommon = sql.str();*/
		TA_Base_Core::IData* data = databaseConnection->executeQuery(v_queryData.sqlStatement, queryData.columnNames);
                                                      

        if (0 == data->getNumRows()) 
        {
            // record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for record from table " 
                         << queryData.tableName.c_str() 
                         << " with key = " << key;
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Primary key" ) );
        }

        reloadUsing(0, *data);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }
}
