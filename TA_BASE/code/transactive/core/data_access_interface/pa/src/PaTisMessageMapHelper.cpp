/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisMessageMapHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaTisMessageMapHelper is an object that is held by PaTisMessageMap and ConfigPaTisMessageMap objects. 
  * It contains all data used by PaTisMessageMaps, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaTisMessageMap and ConfigPaTisMessageMap.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaTisMessageMapHelper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

	static const std::string PA_TIS_MESSAGE_MAP_TABLE_NAME		= "PA_TIS_MESSAGE_MAP";
    static const std::string PA_STATION_DVA_MESSAGE_TABLE_NAME	= "PA_DVA_MESSAGE";

	static const std::string MAP_TABLE_KEY_COL					= "PA_TIS_MESSAGE_MAP.PKEY";
	static const std::string MAP_TABLE_PA_DVA_MESSAGE_KEY_COL	= "PA_TIS_MESSAGE_MAP.PADMES_ID";
	static const std::string MAP_TABLE_TISMESSAGETAG_COL		= "PA_TIS_MESSAGE_MAP.TIS_MESSAGE_TAG";
	static const std::string MAP_TABLE_TISLIBRARYSECTION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_SECTION";
	static const std::string MAP_TABLE_TISLIBRARYVERSION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_VERSION";

    static const std::string PA_TIS_MESSAGE_MAPPING_SEQUENCE_COUNTER	= "PATMMAP_SEQ";

    static const std::string MAP_TABLE_DATEMODIFIED_COL		= "PA_TIS_MESSAGE_MAP.DATE_MODIFIED";
    static const std::string MAP_TABLE_DATECREATED_COL		= "PA_TIS_MESSAGE_MAP.DATE_CREATED";
	
	
	PaTisMessageMapHelper::PaTisMessageMapHelper( const PaTisMessageMapHelper& thePaTisMessageMapHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_ulPaDvaMessageKey(thePaTisMessageMapHelper.m_ulPaDvaMessageKey),
          m_tisMessageTag(thePaTisMessageMapHelper.m_tisMessageTag),
          m_tisLibrarySection(thePaTisMessageMapHelper.m_tisLibrarySection),
          m_tisLibraryVersion(thePaTisMessageMapHelper.m_tisLibraryVersion),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaTisMessageMapHelper::PaTisMessageMapHelper(const unsigned long key)
        : m_key(key),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaTisMessageMapHelper::PaTisMessageMapHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, MAP_TABLE_KEY_COL)),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaTisMessageMapHelper::PaTisMessageMapHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaTisMessageMapHelper::~PaTisMessageMapHelper()
    {
    }
  

    unsigned long PaTisMessageMapHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }


    unsigned long PaTisMessageMapHelper::getPaDvaMessageKey()
    {
        FUNCTION_ENTRY("getId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        FUNCTION_EXIT;

        try 
        {
            if (isNew() && !m_ulPaDvaMessageKey.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_ulPaDvaMessageKey.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_id value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_id value not set after reload");
            throw;
        }
    }


    void PaTisMessageMapHelper::setPaDvaMessageKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_ulPaDvaMessageKey.setValue(id);
        FUNCTION_EXIT;
    }


    unsigned long PaTisMessageMapHelper::getTisLibraryVersion()
    {
        FUNCTION_ENTRY("getTisLibraryVersion");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisLibraryVersion.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (unsigned long)-1;
            }

            return m_tisLibraryVersion.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_tisLibraryVersion value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_tisLibraryVersion value not set after reload");
            throw;
        }
    }


    unsigned long PaTisMessageMapHelper::getTisMessageTag()
    {
        FUNCTION_ENTRY("getTisMessageTag" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisMessageTag.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (unsigned long)-1;
            }

            return m_tisMessageTag.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_tisMessageTag value not set after reload");
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_tisMessageTag value not set after reload");
            throw;
        }

        FUNCTION_EXIT;
    }
    

    unsigned long PaTisMessageMapHelper::getTisLibrarySection()
    {
        FUNCTION_ENTRY("getTisLibrarySection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisLibrarySection.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (unsigned long)-1;
            }

            return m_tisLibrarySection.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_tisLibrarySection value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_tisLibrarySection value not set after reload");
            throw;
        }

        FUNCTION_EXIT;
    }


    void PaTisMessageMapHelper::setTisLibraryVersion(unsigned long version)
    {
        FUNCTION_ENTRY("setTisLibraryVersion");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisLibraryVersion.setValue(version);
        FUNCTION_EXIT;
    }    


    void PaTisMessageMapHelper::setTisMessageTag(const unsigned long tisMessageTag)
    {
        FUNCTION_ENTRY("setTisMessageTag" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisMessageTag.setValue(tisMessageTag);
        FUNCTION_EXIT;
    }


    void PaTisMessageMapHelper::setTisLibrarySection(const unsigned long tisLibrarySection)
    {
        FUNCTION_ENTRY("setTisLibrarySection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisLibrarySection.setValue(tisLibrarySection);
        FUNCTION_EXIT;
    }


    time_t PaTisMessageMapHelper::getDateCreated()
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


    time_t PaTisMessageMapHelper::getDateModified()
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



    void PaTisMessageMapHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaTisMessageMap");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void PaTisMessageMapHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_ulPaDvaMessageKey.setValue(data.getUnsignedLongData( row, MAP_TABLE_PA_DVA_MESSAGE_KEY_COL ));
        m_tisMessageTag.setValue(data.getUnsignedLongData( row, MAP_TABLE_TISMESSAGETAG_COL ));
        m_tisLibrarySection.setValue(data.getUnsignedLongData( row, MAP_TABLE_TISLIBRARYSECTION_COL ));
        m_tisLibraryVersion.setValue(data.getUnsignedLongData( row, MAP_TABLE_TISLIBRARYVERSION_COL ));

        m_dateCreated = data.getDateData( row, MAP_TABLE_DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, MAP_TABLE_DATEMODIFIED_COL );
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


    void PaTisMessageMapHelper::writePaTisMessageMapData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaTisMessageMapData" );     

        // Write the PaTisMessageMap to the database

        try 
        {
            // Need to check if this is a new PaTisMessageMap or an existing PaTisMessageMap - 
            // as a new PaTisMessageMap will be inserted, while an existing PaTisMessageMap will be updated
            if (m_isNew) // This is a new PaTisMessageMap
            {
				//TA_ASSERT(false, "should not be create a new pa dva message")
                addNewPaTisMessageMap();
            }
            else // This is an existing PaTisMessageMap
            {
                modifyExistingPaTisMessageMap();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaTisMessageMap data not fully specified. PaTisMessageMap cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaTisMessageMapHelper::deletePaTisMessageMap(bool cascade)
    {
        FUNCTION_ENTRY("deletePaTisMessageMap");

        TA_ASSERT(!m_isNew, "This PaTisMessageMap does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        
        //if (!cascade)
        //{
        //    std::string info;
        //    
        //    // If don't want cascade, must check for foreign links
        //    if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
        //                                    PA_DVA_MESSAGE_TABLE_NAME,
        //                                    KEY_COL,
        //                                    m_key,
        //                                    info))
        //    {
        //        TA_THROW(TA_Base_Core::DataException(info.c_str(), 
        //                            TA_Base_Core::DataException::CANNOT_DELETE, 
        //                            PA_DVA_MESSAGE_TABLE_NAME.c_str()));
        //    }
        //}

        // Good to go...
//        std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_DELETE_41153, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_DELETE_41153, m_key);

        databaseConnection->executeModification(strSql);


        // Check if deleting complete
//        strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(MAP_TABLE_DATECREATED_COL);
        columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {
            delete data;
            data = NULL;

            // PA Zone record not found
            std::ostringstream reasonMessage;
            reasonMessage << "Can not delete pa_tis_message_map pkey = " << m_key;			
            TA_THROW( TA_Base_Core::DatabaseException(reasonMessage.str()) );

        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }
   

    void PaTisMessageMapHelper::modifyExistingPaTisMessageMap() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaTisMessageMap");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "pa_tis_message_map %u already exists. It's data will be updated.", m_key);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
		
		/*std::ostringstream sql;

		sql << "update " << PA_TIS_MESSAGE_MAP_TABLE_NAME << " set " 
			<< MAP_TABLE_TISMESSAGETAG_COL << " = " << getTisMessageTagDatabaseRepresentation() << ", " 
            << MAP_TABLE_TISLIBRARYSECTION_COL << " = " << getTisLibrarySectionDatabaseRepresentation() << ", " 
            << MAP_TABLE_TISLIBRARYVERSION_COL << " = " << getTisLibraryVersionDatabaseRepresentation() << " " 
			<< " where " << MAP_TABLE_KEY_COL << " = " << m_key;*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_UPDATE_41151,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_UPDATE_41151,
			getTisMessageTagDatabaseRepresentation(), getTisLibrarySectionDatabaseRepresentation(), getTisLibraryVersionDatabaseRepresentation(),m_key);
		
        databaseConnection->executeModification(strSql);


        // now need to update the date fields
     /*   sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_TIS_MESSAGE_MAP_TABLE_NAME << " where " << MAP_TABLE_KEY_COL << " = " << m_key;*/
//		strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(MAP_TABLE_DATECREATED_COL);
        columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);

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
		    reasonMessage << "No data found for pa_tis_message_map key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, MAP_TABLE_DATECREATED_COL);
            m_dateModified = data->getDateData(0, MAP_TABLE_DATEMODIFIED_COL);
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


    std::string PaTisMessageMapHelper::getTisLibraryVersionDatabaseRepresentation()
    {
        if ( m_tisLibraryVersion.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisLibraryVersion.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    std::string PaTisMessageMapHelper::getTisMessageTagDatabaseRepresentation()
    {
        if ( m_tisMessageTag.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisMessageTag.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    std::string PaTisMessageMapHelper::getTisLibrarySectionDatabaseRepresentation()
    {
        if ( m_tisLibrarySection.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisLibrarySection.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    void PaTisMessageMapHelper::addNewPaTisMessageMap() //throw(TA_Base_Core::TransactiveException)
    {		
        FUNCTION_ENTRY("addNewPaTisMessageMap");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Pa tis message map %ul will be added to the database.", getPaDvaMessageKey());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        // Obtain the primary key that should be used for this next entry
        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
                                                                databaseConnection, 
                                                                PA_TIS_MESSAGE_MAPPING_SEQUENCE_COUNTER);
        std::ostringstream pkeyBuf;
        pkeyBuf << sequenceNumber;
        m_key = sequenceNumber;

        std::ostringstream padmesidBuf;
        padmesidBuf << getPaDvaMessageKey();
		
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_INSERT_41152,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_INSERT_41152,
			pkeyBuf.str(), padmesidBuf.str(), getTisMessageTagDatabaseRepresentation(), getTisLibrarySectionDatabaseRepresentation(), getTisLibraryVersionDatabaseRepresentation());
		
        databaseConnection->executeModification(strSql);


        // now need to update the date fields
     /*   sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_TIS_MESSAGE_MAP_TABLE_NAME << " where " << MAP_TABLE_KEY_COL << " = " << m_key;*/
//		strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_SELECT_41054, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(MAP_TABLE_DATECREATED_COL);
        columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);

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
		    reasonMessage << "No data found for pa_tis_message_map key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, MAP_TABLE_DATECREATED_COL);
            m_dateModified = data->getDateData(0, MAP_TABLE_DATEMODIFIED_COL);
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


	void PaTisMessageMapHelper::reload()
	{
		TA_ASSERT(!m_isNew, "This PaTisMessageMap does not yet exist in the database. " 
			"Therefore it cannot be reloaded.");
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		
        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaTisMessageMap record        
      /*  std::ostringstream sql;        
        sql << "select " 
			<< DVA_MSG_TABLE_KEY_COL << ", "
			<< DVA_MSG_TABLE_LOCATIONKEY_COL << ", " 
			<< DVA_MSG_TABLE_ID_COL << ", "
			<< DVA_MSG_TABLE_LABEL_COL << ", "
			<< DVA_MSG_TABLE_TYPE_COL << ", "
			<< MAP_TABLE_KEY_COL << ", "
			<< MAP_TABLE_LOCATIONKEY_COL << ", "
			<< MAP_TABLE_PA_DVA_MESSAGE_KEY_COL << ", " 
			<< MAP_TABLE_TISMESSAGETAG_COL << ", "
			<< MAP_TABLE_TISLIBRARYSECTION_COL << ", "
			<< MAP_TABLE_TISLIBRARYVERSION_COL << ", TO_CHAR("
			<< MAP_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), TO_CHAR("
			<< MAP_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "  
			<< PA_TIS_MESSAGE_MAP_TABLE_NAME 
            << " join "  
			<< PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " on "    
			<< DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << MAP_TABLE_LOCATIONKEY_COL
            << " and "    
			<< DVA_MSG_TABLE_ID_COL << " = " << MAP_TABLE_PA_DVA_MESSAGE_KEY_COL
            << " where "
			<< MAP_TABLE_KEY_COL << " = " << m_key << std::endl;*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41056, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_SELECT_41056, m_key);
		
        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;
        columnNames.push_back(MAP_TABLE_KEY_COL);
		columnNames.push_back(MAP_TABLE_PA_DVA_MESSAGE_KEY_COL);
		columnNames.push_back(MAP_TABLE_TISMESSAGETAG_COL);
        columnNames.push_back(MAP_TABLE_TISLIBRARYSECTION_COL);
		columnNames.push_back(MAP_TABLE_TISLIBRARYVERSION_COL);
		columnNames.push_back(MAP_TABLE_DATECREATED_COL);
		columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);
		
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
        if (0 == data->getNumRows()) 
        {
            // TrackElement record not found
            std::ostringstream reason;
			reason  << "No record found for dva message with key = " << m_key 
				<< " in " << MAP_TABLE_KEY_COL << ".";
			
            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
				TA_Base_Core::DataException::NO_VALUE, "pa_tis_message_map key" ) );
        }
		
        TA_ASSERT(data->getNumRows() == 1, "Invalid number of pa_station_dva_message records."); 
        
        reloadUsing(0, *data);
	}


} // closes TA_Base_Core

