/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 10:53:03 $
  * Last modified by:  $Author: Ouyang $
  *
  * PaDvaMessageHelper is an object that is held by PaDvaMessage and ConfigPaDvaMessage objects. 
  * It contains all data used by PaDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaDvaMessage.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaDvaMessageHelper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

	static const std::string PA_TIS_MESSAGE_MAP_TABLE_NAME		= "PA_TIS_MESSAGE_MAP";
    static const std::string PA_STATION_DVA_MESSAGE_TABLE_NAME	= "PA_DVA_MESSAGE";
	
	static const std::string DVA_MSG_TABLE_KEY_COL				= "PA_DVA_MESSAGE.PADMES_ID";
	static const std::string DVA_MSG_TABLE_LOCATIONKEY_COL		= "PA_DVA_MESSAGE.LOCATIONKEY";
    static const std::string DVA_MSG_TABLE_ID_COL				= "PA_DVA_MESSAGE.ID";
	static const std::string DVA_MSG_TABLE_LABEL_COL			= "PA_DVA_MESSAGE.LABEL";
	static const std::string DVA_MSG_TABLE_TYPE_COL				= "PA_DVA_MESSAGE.TYPE";
	static const std::string DVA_MSG_TABLE_DESCRIPTION_COL		= "PA_DVA_MESSAGE.MSG_DESCRIPTION";
	
	static const std::string MAP_TABLE_KEY_COL					= "PA_TIS_MESSAGE_MAP.PKEY";
	static const std::string MAP_TABLE_LOCATIONKEY_COL			= "PA_TIS_MESSAGE_MAP.LOCATIONKEY";
	static const std::string MAP_TABLE_TISMESSAGETAG_COL		= "PA_TIS_MESSAGE_MAP.TIS_MESSAGE_TAG";
	static const std::string MAP_TABLE_TISLIBRARYSECTION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_SECTION";
	static const std::string MAP_TABLE_TISLIBRARYVERSION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_VERSION";
	
	
    static const std::string PA_DVA_MESSAGE_SEQUENCE_COUNTER	= "PADMES_SEQ";
	
    static const std::string DVA_MSG_TABLE_DATEMODIFIED_COL		= "PA_DVA_MESSAGE.DATE_MODIFIED";
    static const std::string DVA_MSG_TABLE_DATECREATED_COL		= "PA_DVA_MESSAGE.DATE_CREATED";
	
	
	PaDvaMessageHelper::PaDvaMessageHelper( const PaDvaMessageHelper& thePaDvaMessageHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_locationKey(thePaDvaMessageHelper.m_locationKey),          
          m_id(thePaDvaMessageHelper.m_id),       
          m_label(thePaDvaMessageHelper.m_label),
          m_description(thePaDvaMessageHelper.m_description),
          m_type(thePaDvaMessageHelper.m_type),
          //m_tisMessageTag(thePaDvaMessageHelper.m_tisMessageTag),
          //m_tisLibrarySection(thePaDvaMessageHelper.m_tisLibrarySection),
          //m_tisLibraryVersion(thePaDvaMessageHelper.m_tisLibraryVersion),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
		  m_localDatabase("")
    {	  
	}


    PaDvaMessageHelper::PaDvaMessageHelper(const unsigned long key)
        : m_key(key),		  
          m_label(""),
          m_description(""),
          m_type(),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false),
		  m_localDatabase("")
    {
    }


    PaDvaMessageHelper::PaDvaMessageHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, DVA_MSG_TABLE_KEY_COL)),		  
          m_label(""),
          m_description(""),
          m_type(),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false),
		  m_localDatabase("")
    {
        reloadUsing(row, data);
    }


    PaDvaMessageHelper::PaDvaMessageHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_label(""),
          m_description(""),
          m_type(),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
		  m_localDatabase("")
    {
    }


    PaDvaMessageHelper::~PaDvaMessageHelper()
    {
    }
  

    unsigned long PaDvaMessageHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    unsigned long PaDvaMessageHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        // Following the reload of data, all values should have been set
        try
        {
            if (isNew() && !m_locationKey.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_locationKey value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_locationKey value not set after reload");
            throw;
        }
    }

    void PaDvaMessageHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }

    unsigned long PaDvaMessageHelper::getId()
    {
        FUNCTION_ENTRY("getId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        FUNCTION_EXIT;

        try 
        {
            if (isNew() && !m_id.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_id.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_id value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_id value not set after reload");
            throw;
        }
    }


    void PaDvaMessageHelper::setId(const unsigned long id)
    {
        FUNCTION_ENTRY("setId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_id.setValue(id);
        FUNCTION_EXIT;
    }


    std::string PaDvaMessageHelper::getLabel()
    {
        FUNCTION_ENTRY("getLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_label;
    }
    
    
    std::string PaDvaMessageHelper::getDescription()
    {
        FUNCTION_ENTRY("getDescription" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_description;
    }


    void PaDvaMessageHelper::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_label = label;
        FUNCTION_EXIT;
    }
    
    void PaDvaMessageHelper::setDescription(const std::string& description)
    {
        FUNCTION_ENTRY("setDescription" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_description = description;
        FUNCTION_EXIT;
    }

    unsigned short PaDvaMessageHelper::getType()
    {
        FUNCTION_ENTRY("getType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_type.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (unsigned short)-1;
            }

            return m_type.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_tisLibraryVersion value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_tisLibraryVersion value not set after reload");
            throw;
        }
    }


    void PaDvaMessageHelper::setType(const unsigned short type)
    {
        FUNCTION_ENTRY("setType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_type.setValue(type);
        FUNCTION_EXIT;
    }

    //unsigned long PaDvaMessageHelper::getTisLibraryVersion()
    //{
    //    FUNCTION_ENTRY("getTisLibraryVersion");

    //    FUNCTION_EXIT;
    //    return (unsigned long)-1;

    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }

    //    try
    //    {
    //        if ( !m_tisLibraryVersion.hasBeenSet() ) 
    //        {
    //            // If the value is unset (a valid state), return -1
    //            return (unsigned long)-1;
    //        }

    //        return m_tisLibraryVersion.getValue();
    //    }
    //    catch (const TA_Base_Core::ValueNotSetException&)
    //    {
    //        // reload() should have either thrown exception or set this value
    //        TA_ASSERT(false, "m_tisLibraryVersion value not set after reload");
    //        throw;
    //    }
    //}


    //unsigned long PaDvaMessageHelper::getTisMessageTag()
    //{
    //    FUNCTION_ENTRY("getTisMessageTag" );

    //    FUNCTION_EXIT;
    //    return (unsigned long)-1;

    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }

    //    try
    //    {
    //        if ( !m_tisMessageTag.hasBeenSet() ) 
    //        {
    //            // If the value is unset (a valid state), return -1
    //            return (unsigned long)-1;
    //        }

    //        return m_tisMessageTag.getValue();
    //    }
    //    catch (const TA_Base_Core::ValueNotSetException&)
    //    {
    //        // reload() should have either thrown exception or set this value
    //        TA_ASSERT(false, "m_tisMessageTag value not set after reload");
    //        throw;
    //    }

    //    FUNCTION_EXIT;
    //}
    //

    //unsigned long PaDvaMessageHelper::getTisLibrarySection()
    //{
    //    FUNCTION_ENTRY("getTisLibrarySection" );

    //    FUNCTION_EXIT;
    //    return (unsigned long)-1;

    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }

    //    try
    //    {
    //        if ( !m_tisLibrarySection.hasBeenSet() ) 
    //        {
    //            // If the value is unset (a valid state), return -1
    //            return (unsigned long)-1;
    //        }

    //        return m_tisLibrarySection.getValue();
    //    }
    //    catch (const TA_Base_Core::ValueNotSetException&)
    //    {
    //        // reload() should have either thrown exception or set this value
    //        TA_ASSERT(false, "m_tisLibrarySection value not set after reload");
    //        throw;
    //    }

    //    FUNCTION_EXIT;
    //}


    //void PaDvaMessageHelper::setTisLibraryVersion(unsigned long version)
    //{
    //    FUNCTION_ENTRY("setTisLibraryVersion");

    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }
    //    m_tisLibraryVersion.setValue(version);
    //    FUNCTION_EXIT;
    //}    


    //void PaDvaMessageHelper::setTisMessageTag(const unsigned long tisMessageTag)
    //{
    //    FUNCTION_ENTRY("setTisMessageTag" );
    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }
    //    m_tisMessageTag.setValue(tisMessageTag);
    //    FUNCTION_EXIT;
    //}


    //void PaDvaMessageHelper::setTisLibrarySection(const unsigned long tisLibrarySection)
    //{
    //    FUNCTION_ENTRY("setTisLibrarySection" );
    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }
    //    m_tisLibrarySection.setValue(tisLibrarySection);
    //    FUNCTION_EXIT;
    //}


    time_t PaDvaMessageHelper::getDateCreated()
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


    time_t PaDvaMessageHelper::getDateModified()
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



    void PaDvaMessageHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessage");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, DVA_MSG_TABLE_LOCATIONKEY_COL ));
        m_id.setValue(data.getUnsignedLongData( row, DVA_MSG_TABLE_ID_COL ));
		m_label = data.getStringData( row, DVA_MSG_TABLE_LABEL_COL );
        m_description = data.getStringData( row, DVA_MSG_TABLE_DESCRIPTION_COL );
		m_type.setValue(static_cast<unsigned short>(data.getUnsignedLongData( row, DVA_MSG_TABLE_TYPE_COL )));

        m_dateCreated = data.getDateData( row, DVA_MSG_TABLE_DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DVA_MSG_TABLE_DATEMODIFIED_COL );
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


    void PaDvaMessageHelper::writePaDvaMessageData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaDvaMessageData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(DVA_MSG_TABLE_LOCATIONKEY_COL);
        }
        if (!m_id.hasBeenSet())
        {
            fieldNames.push_back(DVA_MSG_TABLE_ID_COL);
        }
        if (!m_type.hasBeenSet())
        {
            fieldNames.push_back(DVA_MSG_TABLE_TYPE_COL);
        }

        // These properties can be unset, 
        // translated to a value of -1 when stored in database
        // TISMESSAGETAG_COL, TISLIBRARYSECTION_COL, TISLIBRARYVERSION_COL
        
        
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException(
                "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                fieldNames));
        }

        //validateIdLocationPairUniqueness();

        // Write the PaDvaMessage to the database

        try 
        {
            // Need to check if this is a new PaDvaMessage or an existing PaDvaMessage - 
            // as a new PaDvaMessage will be inserted, while an existing PaDvaMessage will be updated
            if (m_isNew) // This is a new PaDvaMessage
            {
				//TA_ASSERT(false, "should not be create a new pa dva message")
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "should not be create a new pa dva message");
                //addNewPaDvaMessage();
            }
            else // This is an existing PaDvaMessage
            {
                modifyExistingPaDvaMessage();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::validateIdLocationPairUniqueness() 
    {   
//        std::ostringstream sql;
//        sql << "select " << KEY_COL
//            << " from " << PA_DVA_MESSAGE_TABLE_NAME 
//            << " where " << LOCATIONKEY_COL << " = " << m_locationKey.getValue()
//            << " and " << ID_COL << " = " << m_id.getValue();
//
//        std::vector<std::string> columnNames;
//        columnNames.push_back(KEY_COL);
//                  
//        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
//
//        if (0 != data->getNumRows()) 
//        {        
//            unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);
//
//            // Ignore ourself..
//            if (duplicateItemKey != m_key)
//            {
//                std::ostringstream str;
//                str << "Attempting to write Pa Dva Message record that doesn't "
//                    << "have unique id/location (existing duplicate has pKey="
//                    << duplicateItemKey
//                    << ")";
//    
//
//                delete data;
//                data = NULL;
//
//                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
//                            TA_Base_Core::DataException::NOT_UNIQUE,
//                            "LocationKey and Id") );
//            }
//        }
//
//        delete data;
//        data = NULL;

    }


    void PaDvaMessageHelper::deletePaDvaMessage(bool cascade)
    {
		//TA_ASSERT(false, "this function should not be called in distribute database");
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugError, "this function should not be called in distribute database.");
//        FUNCTION_ENTRY("deletePaDvaMessage");
//
//        TA_ASSERT(!m_isNew, "This PaDvaMessage does not yet exist in the database, and therefore cannot be deleted");
//        
//        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
//        std::ostringstream sql;
//        
//        if (!cascade)
//        {
//            std::string info;
//            
//            // If don't want cascade, must check for foreign links
//            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
//                                            PA_DVA_MESSAGE_TABLE_NAME,
//                                            KEY_COL,
//                                            m_key,
//                                            info))
//            {
//                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
//                                    TA_Base_Core::DataException::CANNOT_DELETE, 
//                                    PA_DVA_MESSAGE_TABLE_NAME.c_str()));
//            }
//        }
//
//        // Good to go...
//        sql.str("");
//        sql << "delete " << PA_DVA_MESSAGE_TABLE_NAME << " where " << KEY_COL << " = " << m_key;
//
//        databaseConnection->executeModification(sql.str());
//
//        FUNCTION_EXIT;
    }
   

    void PaDvaMessageHelper::modifyExistingPaDvaMessage() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaDvaMessage");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Zone %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
		
		/*std::ostringstream sql;

		sql << "BEGIN "
			<< "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','UPDATE', '"
			<< "update " << PA_STATION_DVA_MESSAGE_TABLE_NAME << " set " 
            << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
            << DVA_MSG_TABLE_ID_COL << " = " << m_id.getValue() << ", "
            << DVA_MSG_TABLE_LABEL_COL << " = ''"
            << (databaseConnection->escapeAQSQLString(m_label)).c_str() << "'', " 
            << DVA_MSG_TABLE_TYPE_COL << " = " << m_type.getValue() << " " 
            << "where " << DVA_MSG_TABLE_KEY_COL << " = " << m_key
			<< " ');"
			<< "END;"; */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_DVA_MESSAGE_Oracle_UPDATE_41701,getLocalDatabaseName(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_UPDATE_41701,getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_UPDATE_41701,getLocalDatabaseName(),
			m_locationKey.getValue(), m_id.getValue(),databaseConnection->escapeAQSQLString(m_label), 
            databaseConnection->escapeAQSQLString(m_description), m_type.getValue(), m_key);
		
        databaseConnection->executeModification(strSql);


        // now need to update the date fields
      /*  sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_STATION_DVA_MESSAGE_TABLE_NAME << " where " << DVA_MSG_TABLE_KEY_COL << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_DVA_MESSAGE_Oracle_SELECT_41551, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_SELECT_41551, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_SELECT_41551, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(DVA_MSG_TABLE_DATECREATED_COL);
        columnNames.push_back(DVA_MSG_TABLE_DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Zone key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, DVA_MSG_TABLE_DATECREATED_COL);
            m_dateModified = data->getDateData(0, DVA_MSG_TABLE_DATEMODIFIED_COL);
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


    //std::string PaDvaMessageHelper::getTisLibraryVersionDatabaseRepresentation()
    //{
    //    if ( m_tisLibraryVersion.hasBeenSet() )
    //    {
    //        // This internal field is valid so simply use that value
    //        std::ostringstream buf;

    //        buf << m_tisLibraryVersion.getValue();
    //        return buf.str();
    //    } 
    //    else
    //    {
    //        return "null";
    //    }
    //}


    //std::string PaDvaMessageHelper::getTisMessageTagDatabaseRepresentation()
    //{
    //    if ( m_tisMessageTag.hasBeenSet() )
    //    {
    //        // This internal field is valid so simply use that value
    //        std::ostringstream buf;

    //        buf << m_tisMessageTag.getValue();
    //        return buf.str();
    //    } 
    //    else
    //    {
    //        return "null";
    //    }
    //}


    //std::string PaDvaMessageHelper::getTisLibrarySectionDatabaseRepresentation()
    //{
    //    if ( m_tisLibrarySection.hasBeenSet() )
    //    {
    //        // This internal field is valid so simply use that value
    //        std::ostringstream buf;

    //        buf << m_tisLibrarySection.getValue();
    //        return buf.str();
    //    } 
    //    else
    //    {
    //        return "null";
    //    }
    //}


    void PaDvaMessageHelper::addNewPaDvaMessage() //throw(TA_Base_Core::TransactiveException)
    {		
//        FUNCTION_ENTRY("addNewPaDvaMessage");
//
//        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
//            "Station Mode %u is a new PaDvaMessage. It will be added to the database.",m_locationKey.getValue());
//
//        validateLocationKeyExistence();
//
//
//        ////////////////////////////////////////////////////////////////////////////
//        //
//        // Good to go...
//        //
//        ////////////////////////////////////////////////////////////////////////////
//        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
//        // Obtain the primary key that should be used for this next entry
//        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
//                                                                databaseConnection, 
//                                                                PA_DVA_MESSAGE_SEQUENCE_COUNTER);
//
//        std::ostringstream sql;
//        sql << "insert into " << PA_DVA_MESSAGE_TABLE_NAME << " (" 
//            << KEY_COL << ", "
//            << LOCATIONKEY_COL << ", "
//            << ID_COL << ", "
//            << LABEL_COL << ", "
//            << TYPE_COL << ", "
//            << TISMESSAGETAG_COL << ", "
//            << TISLIBRARYSECTION_COL << ", "
//            << TISLIBRARYVERSION_COL << ") "
//            << "values (" << sequenceNumber << ","
//            << m_locationKey.getValue() << "," 
//            << m_id.getValue() << ",'" 
//            << (databaseConnection->escapeInsertString(m_label)).c_str() << "','" 
//            << (databaseConnection->escapeInsertString(m_type)).c_str() << "'," 
//            << databaseConnection->escapeInsertString(getTisMessageTagDatabaseRepresentation()) << ", "
//            << databaseConnection->escapeInsertString(getTisLibrarySectionDatabaseRepresentation()) << ", "
//            << databaseConnection->escapeInsertString(getTisLibraryVersionDatabaseRepresentation()) << ")";
//
//        databaseConnection->executeModification(sql.str());
//
//        sql.str("");
//
//        // Obtain the newly created record
//        sql << "select " 
//            << KEY_COL << ", "
//            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
//            << "from " << PA_DVA_MESSAGE_TABLE_NAME << " where "
//            << KEY_COL << " = " << sequenceNumber;
//
//        std::vector<std::string> columnNames;
//        columnNames.push_back(KEY_COL);
//        columnNames.push_back(DATECREATED_COL);
//
//        // Execute the query. The method can throw a DatabaseException.
//        // This is documented in the comment of this method.
//        // We are responsible for deleting the returned IData object when we're done with it
//        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
//
//        if (0 == data->getNumRows()) 
//        {
//            // PA Zone record not found            
//            delete data;
//            data = NULL;
//
//            std::ostringstream reasonMessage;
//		    reasonMessage << "No data found for PA Zone key = " << m_key;			
//            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
//        }
//
//        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
//        // (as any other circumstance is specifically prevented by the above checks).
//        // This method can throw a DataException
//        try
//        {
//            m_key = data->getUnsignedLongData(0,KEY_COL);
//            m_dateCreated = data->getDateData(0,DATECREATED_COL);
//        }
//        catch ( ... )
//        {
//            delete data;
//            data = NULL;
//            throw;
//        }
//
//        // Now that we're finished with the IData object, we need to delete it.
//        delete data;
//        data = NULL;
//
//        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
       /* std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();*/
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7003, m_locationKey.getValue());
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7003, m_locationKey.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7003, m_locationKey.getValue());
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // Location key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Location key = " << m_locationKey.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
        }

        delete data;
        data = NULL;
    }

	void PaDvaMessageHelper::reload()
	{
		TA_ASSERT(!m_isNew, "This PaDvaMessage does not yet exist in the database. " 
			"Therefore it cannot be reloaded.");
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		
        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record        
      /*  std::ostringstream sql;        
        sql << "select " 
			<< DVA_MSG_TABLE_KEY_COL << ", "
			<< DVA_MSG_TABLE_LOCATIONKEY_COL << ", " 
			<< DVA_MSG_TABLE_ID_COL << ", "
			<< DVA_MSG_TABLE_LABEL_COL << ", "
			<< DVA_MSG_TABLE_TYPE_COL << ", "
			<< MAP_TABLE_KEY_COL << ", "
			<< MAP_TABLE_LOCATIONKEY_COL << ", "
			<< MAP_TABLE_ID_COL << ", " 
			<< MAP_TABLE_TISMESSAGETAG_COL << ", "
			<< MAP_TABLE_TISLIBRARYSECTION_COL << ", "
			<< MAP_TABLE_TISLIBRARYVERSION_COL << ", TO_CHAR("
			<< DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), TO_CHAR("
			<< DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "  
			<< PA_TIS_MESSAGE_MAP_TABLE_NAME 
            << " join "  
			<< PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " on "    
			<< DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << MAP_TABLE_LOCATIONKEY_COL
            << " and "    
			<< DVA_MSG_TABLE_ID_COL << " = " << MAP_TABLE_ID_COL
            << " where "
			<< DVA_MSG_TABLE_KEY_COL << " = " << m_key << std::endl;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TIS_MESSAGE_MAP_Oracle_SELECT_41055, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_SELECT_41554, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_SELECT_41554, m_key);
		
        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;
        columnNames.push_back(DVA_MSG_TABLE_KEY_COL);
        columnNames.push_back(DVA_MSG_TABLE_LOCATIONKEY_COL);
        columnNames.push_back(DVA_MSG_TABLE_ID_COL);
        columnNames.push_back(DVA_MSG_TABLE_LABEL_COL);
		columnNames.push_back(DVA_MSG_TABLE_DESCRIPTION_COL);
        columnNames.push_back(DVA_MSG_TABLE_TYPE_COL);
		columnNames.push_back(DVA_MSG_TABLE_DATECREATED_COL);
		columnNames.push_back(DVA_MSG_TABLE_DATEMODIFIED_COL);
		
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
        if (0 == data->getNumRows()) 
        {
            // TrackElement record not found
            std::ostringstream reason;
			reason  << "No record found for dva message with key = " << m_key 
				<< " in " << DVA_MSG_TABLE_KEY_COL << ".";
			
            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
				TA_Base_Core::DataException::NO_VALUE, "pa_station_dva_message key" ) );
        }
		
        TA_ASSERT(data->getNumRows() == 1, "Invalid number of pa_station_dva_message records."); 
        
        reloadUsing(0, *data);
	}

	std::string PaDvaMessageHelper::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Pa_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Pa_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }
			
        }
        return m_localDatabase;
    }


} // closes TA_Base_Core

