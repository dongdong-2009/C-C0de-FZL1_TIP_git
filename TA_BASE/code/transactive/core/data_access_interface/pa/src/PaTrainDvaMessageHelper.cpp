/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTrainDvaMessageHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaTrainDvaMessageHelper is an object that is held by PaTrainDvaMessage and ConfigPaTrainDvaMessage objects. 
  * It contains all data used by PaTrainDvaMessageGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaTrainDvaMessage and ConfigPaTrainDvaMessage.
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaTrainDvaMessageHelper.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string PA_TRAIN_DVA_MESSAGE_TABLE_NAME = "PA_TRAIN_DVA_MESSAGE";
    static const std::string PA_TRAIN_DVA_MESSAGE_TABLE_SHORT_NAME = "PATDME";
    static const std::string KEY_COL = "PATDME_ID";

    static const std::string LABEL_COL = "LABEL";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaTrainDvaMessageHelper::PaTrainDvaMessageHelper( const PaTrainDvaMessageHelper& thePaTrainDvaMessageHelper)
		: // m_key(),   // Don't set the key upon copy..
          m_label(thePaTrainDvaMessageHelper.m_label),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaTrainDvaMessageHelper::PaTrainDvaMessageHelper(const unsigned long key)
        : m_key(key),		  
          m_label(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaTrainDvaMessageHelper::PaTrainDvaMessageHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
          m_label(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaTrainDvaMessageHelper::PaTrainDvaMessageHelper()
        : m_label(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaTrainDvaMessageHelper::~PaTrainDvaMessageHelper()
    {
    }
  

    unsigned long PaTrainDvaMessageHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        if (!m_key.hasBeenSet())
        {
            if (isNew())
            {
                // New configuration items won't necessarily have a key
                return TA_Base_Core::DatabaseKey::getInvalidKey();
            }
        
            TA_THROW(TA_Base_Core::DataException("Key not set",  
                            TA_Base_Core::DataException::NO_VALUE, "Primary key"));
        }        

        FUNCTION_EXIT;
        return m_key.getValue();
    }


    std::string PaTrainDvaMessageHelper::getLabel()
    {
        FUNCTION_ENTRY("getLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_label;
    }


    void PaTrainDvaMessageHelper::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_label = label;
        FUNCTION_EXIT;
    }

    unsigned long PaTrainDvaMessageHelper::getModifiedKey()
    {
        if (m_modifiedKey.hasBeenSet())
        {
            return m_modifiedKey.getValue();
        }
        else
        {
            return getKey();
        }
    }

    void PaTrainDvaMessageHelper::setKeyToUse(unsigned long keyToUse)
    {
        FUNCTION_ENTRY("setKeyToUse" );

        if (!isNew())
        {
            // Not setting for a new object (key already allocated)
            m_modifiedKey.setValue(keyToUse);
            return;
        }

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_key.setValue(keyToUse);
        FUNCTION_EXIT;
    }


    time_t PaTrainDvaMessageHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t PaTrainDvaMessageHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }



    void PaTrainDvaMessageHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaTrainDvaMessage");

        m_isValidData = false;
        m_modifiedKey.resetValue();

        FUNCTION_EXIT;
    }



    DatabaseQueryHelper::QueryData 
        PaTrainDvaMessageHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_TRAIN_DVA_MESSAGE_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LABEL_COL);
        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        /*result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);*/
		
		DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
			result.tableName);

        return result;
    }


    void PaTrainDvaMessageHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_label   = data.getStringData( row, LABEL_COL );
        m_dateCreated = data.getDateData( row, DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void PaTrainDvaMessageHelper::writePaTrainDvaMessageData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaTrainDvaMessageData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;        
        if (m_label.empty())
        {
            fieldNames.push_back(LABEL_COL);
        }
        // User defined key (only for new objects)
        if (m_isNew && !m_key.hasBeenSet())
        {
            fieldNames.push_back(KEY_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaTrainDvaMessage data not fully specified. PaTrainDvaMessage cannot be written to database",
                                                         fieldNames));
        }

        // Write the PaTrainDvaMessage to the database

        try 
        {
            // Need to check if this is a new PaTrainDvaMessage or an existing PaTrainDvaMessage - 
            // as a new PaTrainDvaMessage will be inserted, while an existing PaTrainDvaMessage will be updated
            if (m_isNew) // This is a new PaTrainDvaMessage
            {
                validateKeyUniqueness(m_key.getValue());
                addNewPaTrainDvaMessage(m_key.getValue());
            }
            else // This is an existing PaTrainDvaMessage
            {
                modifyExistingPaTrainDvaMessage();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaTrainDvaMessage data not fully specified. PaTrainDvaMessage cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    
    void PaTrainDvaMessageHelper::validateKeyUniqueness(unsigned long keyToUse)
    {   
        if (isKeyDuplicate(keyToUse))
        {                    
            std::ostringstream str;
            str << "Attempting to write Pa Train Dva Message record that doesn't "
                << "have unique label (existing duplicate has pKey="
                << keyToUse
                << ")";

            TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                        TA_Base_Core::DataException::NOT_UNIQUE,
                        "Label") );
        }
    }   


    bool PaTrainDvaMessageHelper::isKeyDuplicate(unsigned long keyToUse)
    {
        if (TA_Base_Core::DatabaseKey::isInvalidKey(keyToUse))
        {
            return false;
        }

       /* std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME 
            << " where " << KEY_COL << " = " << keyToUse;	*/	
            
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_SELECT_44001, keyToUse);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_SELECT_44001, keyToUse);
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 != data.get() && 0 == data->getNumRows()) 
        {   
            // Must not be a duplicate, no rows returned
            return false;
        }
        else
        {
            // Otherwise have to assume it's a duplicate
            return true;
        }
    }

    void PaTrainDvaMessageHelper::deletePaTrainDvaMessage(bool cascade)
    {
        FUNCTION_ENTRY("deletePaTrainDvaMessage");

        TA_ASSERT(!m_isNew, "This PaTrainDvaMessage does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        //std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            PA_TRAIN_DVA_MESSAGE_TABLE_NAME,
                                            KEY_COL,
                                            m_key.getValue(),
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    PA_TRAIN_DVA_MESSAGE_TABLE_NAME.c_str()));
            }
        }

        // Good to go...
      /*  sql.str("");
        sql << "delete " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_DELETE_44401, m_key.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_DELETE_44401, m_key.getValue());

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void PaTrainDvaMessageHelper::modifyExistingPaTrainDvaMessage() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaTrainDvaMessage");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Train Dva Message %u already exists. It's data will be updated.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        //std::ostringstream sql;

        unsigned long updatedKeyVal = m_key.getValue();

        // If a modified key has been specified, then we update the key column
        if (m_modifiedKey.hasBeenSet())
        {
            updatedKeyVal = m_modifiedKey.getValue();
        }


       /* sql << "update " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " set " 
            << LABEL_COL << " = '"
            << (databaseConnection->escapeInsertString(m_label)).c_str() << "', " 
            << KEY_COL << " = "
            << updatedKeyVal
            << " where " << KEY_COL << " = " << m_key.getValue();  */   
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_UPDATE_44151,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_UPDATE_44151,
			databaseConnection->escapeInsertString(m_label), updatedKeyVal, m_key.getValue());

        databaseConnection->executeModification(strSql);

        // Update the internal key value, as it may have changed by the update request
        m_key.setValue(updatedKeyVal);
        
        // now need to update the date fields (remember to fetch the 'updatedKeyVal')
        /*sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << 
            " where " << KEY_COL << " = " << m_key.getValue();*/
//		strSql = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_SELECT_44052, m_key.getValue());

		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_SELECT_44052, m_key.getValue());

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Train Dva Message record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Train Dva Message key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Train Dva Message key" ) );
        }

        try
        {
            // Clear the modified key flag, as now current key will reflect modified key
            // (if in fact the modified key was set)
            m_modifiedKey.resetValue();
        
            m_dateCreated = data->getDateData(0, DATECREATED_COL);
            m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void PaTrainDvaMessageHelper::addNewPaTrainDvaMessage(unsigned long keyToUse) 
                    //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaTrainDvaMessage");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA Train DVA Message %u is a new PaTrainDvaMessage. It will be added to the database.", keyToUse);


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

       /* std::ostringstream sql;
        sql << "insert into " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << LABEL_COL << ") "
            << "values (" << keyToUse << ", '"
            << (databaseConnection->escapeInsertString(m_label)).c_str() 
            << "')";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_INSERT_44301,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_INSERT_44301,
			keyToUse, databaseConnection->escapeInsertString(m_label));
            
        databaseConnection->executeModification(strSql);

       /* sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " where "
            << KEY_COL << " = " << keyToUse;*/
//		strSql = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_SELECT_44053, keyToUse);

		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_SELECT_44053, keyToUse);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Train DVA Message record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Train DVA Message key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Train DVA Message key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key.setValue(data->getUnsignedLongData(0,KEY_COL));
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }
} // closes TA_Base_Core

