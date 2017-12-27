/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaZoneHelper is an object that is held by PaZone and ConfigPaZone objects. 
  * It contains all data used by PaZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaZone and ConfigPaZone.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaZoneHelper.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
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
    // Added pazone identifier to end of table name (harmless to add in if not used)
    // - required for PaZoneAccessFactory::getPaZonesByGroupId to work (performs table joins)
    const std::string PaZoneHelper::PA_ZONE_TABLE_NAME             = "PA_ZONE";
    const std::string PaZoneHelper::KEY_COL                        = "PAZONE_ID";
    const std::string PaZoneHelper::PA_ZONE_SEQUENCE_COUNTER       = "PAZONE_SEQ";

    const std::string PaZoneHelper::PA_ZONE_HELPER_TABLE_NAME      = "PA_ZONE_HELPER";
    const std::string PaZoneHelper::PA_ZONE_HELPER_KEY_COL         = "PAZHEL_ID";
    const std::string PaZoneHelper::PA_ZONE_HELPER_ENTITY_NAME_COL = "PID_ZONE_EQUIPMENT";
    const std::string PaZoneHelper::PA_ZONE_HELPER_SEQUENCE_COUNTER = "PAZHEL_SEQ";

    const std::string PaZoneHelper::LOCATIONKEY_COL                = "LOCATIONKEY";
    const std::string PaZoneHelper::ID_COL                         = "ID";
    const std::string PaZoneHelper::LABEL_COL                      = "LABEL";
    //const std::string PaZoneHelper::EQUIPMENTENTITYKEY_COL         = "pazone.EQUIPMENT_ENTITYKEY";
    const std::string PaZoneHelper::STN_EQUIPMENTENTITYNAME_COL    = "STN_EQUIPMENT";
    const std::string PaZoneHelper::OCC_EQUIPMENTENTITYNAME_COL    = "OCC_EQUIPMENT";
    const std::string PaZoneHelper::DATEMODIFIED_COL               = "DATE_MODIFIED";
    const std::string PaZoneHelper::DATECREATED_COL                = "DATE_CREATED";

    /*
    const std::string PaZoneHelper::FOREIGN_ENTITY_TABLE_NAME      = "ENTITY entity";
    const std::string PaZoneHelper::FOREIGN_ENTITY_NAME_COL        = "entity.NAME";
    const std::string PaZoneHelper::FOREIGN_ENTITY_KEY_COL         = "entity.PKEY";
    */
    
	PaZoneHelper::PaZoneHelper( const PaZoneHelper& thePaZoneHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_locationKey(thePaZoneHelper.m_locationKey),          
          m_id(thePaZoneHelper.m_id),       
          m_label(thePaZoneHelper.m_label),
          m_stnEquipmentEntityName(thePaZoneHelper.m_stnEquipmentEntityName),
          m_occEquipmentEntityName(thePaZoneHelper.m_occEquipmentEntityName),
          //m_statusEntityKey(thePaZoneHelper.m_statusEntityKey),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
          //m_associatedEquipmentEntity(thePaZoneHelper.m_associatedEquipmentEntity),
          //m_associatedEquipmentEntitySet(thePaZoneHelper.m_associatedEquipmentEntitySet)
    {
	}


    PaZoneHelper::PaZoneHelper(const unsigned long key)
        : m_key(key),		  
          m_label(""),
          m_stnEquipmentEntityName(""),
          m_occEquipmentEntityName(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
          //m_associatedEquipmentEntitySet(false)
    {
    }


    PaZoneHelper::PaZoneHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_label(""),
          m_stnEquipmentEntityName(""),
          m_occEquipmentEntityName(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
          //m_associatedEquipmentEntitySet(false)
    {
    }


    PaZoneHelper::PaZoneHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_label(""),
          m_stnEquipmentEntityName(""),
          m_occEquipmentEntityName(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
          //m_associatedEquipmentEntitySet(false)
    {
        m_key = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    PaZoneHelper::~PaZoneHelper()
    {
    }
  

    unsigned long PaZoneHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    unsigned long PaZoneHelper::getLocationKey()
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
                // 4669FAT TD #782 - if this is a new zone that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }
            return m_locationKey.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_locationKey value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_locationKey value not set after reload");
            throw;
        }
    }

    void PaZoneHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }

    unsigned long PaZoneHelper::getId()
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
                // 4669FAT TD #782 - if this is a new zone that hasn't been saved yet, return a default
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


    void PaZoneHelper::setId(const unsigned long id)
    {
        FUNCTION_ENTRY("setId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_id.setValue(id);
        FUNCTION_EXIT;
    }


    std::string PaZoneHelper::getLabel()
    {
        FUNCTION_ENTRY("getLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_label;
    }


    void PaZoneHelper::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_label = label;
        FUNCTION_EXIT;
    }

    std::string PaZoneHelper::getStnEntityName()
    {
        FUNCTION_ENTRY("getStnEntityName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_stnEquipmentEntityName;
    }


    std::string PaZoneHelper::getOccEntityName()
    {
        FUNCTION_ENTRY("getOccEntityName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_occEquipmentEntityName;
    }


    void PaZoneHelper::setStnEntityName(const std::string& name)
    {
        FUNCTION_ENTRY("setStnEntityName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stnEquipmentEntityName = name;
        FUNCTION_EXIT;
    }


    void PaZoneHelper::setOccEntityName(const std::string& name)
    {
        FUNCTION_ENTRY("setOccEntityName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_occEquipmentEntityName = name;
        FUNCTION_EXIT;
    }

    time_t PaZoneHelper::getDateCreated()
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


    time_t PaZoneHelper::getDateModified()
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

    //std::list<std::string> PaZoneHelper::getAssociatedEquipmentEntities()
    //{
    //    if (!m_associatedEquipmentEntitySet)
    //    {
    //        loadAssociatedEquipmentEntityData();
    //    }

    //    if (!m_isValidData && !m_isNew)
    //    {
    //        reload();
    //    }

    //    return m_associatedEquipmentEntity;
    //}

    //void PaZoneHelper::setAssociatedEquipmentEntity( const std::string& entityName, bool isAssociated)
    //{
        //if (!m_associatedEquipmentEntitySet)
        //{
        //    loadAssociatedEquipmentEntityData();
        //}

        //FUNCTION_ENTRY("setAssociatedEquipmentEntity" );

        //if (!m_isValidData && !m_isNew)
        //{
        //    reload();
        //}

        //std::list<std::string>::iterator matchingEquipmentEntity = 
        //    std::find(m_associatedEquipmentEntity.begin(), m_associatedEquipmentEntity.end(), entityName);

        //if (isAssociated)
        //{
        //    // we need to add this profile
        //    if ( matchingEquipmentEntity == m_associatedEquipmentEntity.end() )
        //    {
        //        // This is definitely not already in the list so add it
        //        m_associatedEquipmentEntity.push_back(entityName);
        //    }
        //    // else the item is already in the list
        //}
        //else
        //{
        //    // we need to remove this profile
        //    if ( matchingEquipmentEntity != m_associatedEquipmentEntity.end() )
        //    {
        //        // This is definitely in the list so we must remove it
        //        m_associatedEquipmentEntity.erase(matchingEquipmentEntity);
        //    }
        //    // else the item has already been removed from the list
        //}

        //FUNCTION_EXIT;
    //}

    void PaZoneHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaZone");

        m_isValidData = false;
        //m_associatedEquipmentEntitySet = false;

        FUNCTION_EXIT;
    }

    
    const DatabaseQueryHelper::QueryData 
        PaZoneHelper::getBasicQueryData(const std::string& joinedTable)
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
        result.tableName        = PA_ZONE_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(ID_COL);
		result.columnNames.push_back(LABEL_COL);
		result.columnNames.push_back(STN_EQUIPMENTENTITYNAME_COL);
        result.columnNames.push_back(OCC_EQUIPMENTENTITYNAME_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);
        result.columnNames.push_back(DATECREATED_COL);

        std::ostringstream sql;
		//jinhua++TD17650 add the talbe name PA_ZONE_TABLE_NAME for the same field int the talbe PA_ZONE_TABLE_NAME and joinedTable
		/*sql << "select " 
            << PA_ZONE_TABLE_NAME << "." << KEY_COL << ","
            << PA_ZONE_TABLE_NAME << "." << LOCATIONKEY_COL << ","
            << PA_ZONE_TABLE_NAME << "." << ID_COL << ","
            << PA_ZONE_TABLE_NAME << "." << LABEL_COL << ","
            << PA_ZONE_TABLE_NAME << "." << STN_EQUIPMENTENTITYNAME_COL << ","
            << PA_ZONE_TABLE_NAME << "." << OCC_EQUIPMENTENTITYNAME_COL << ","
            << "TO_CHAR(nvl(" << PA_ZONE_TABLE_NAME << "." << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(" << PA_ZONE_TABLE_NAME << "." << DATECREATED_COL << ",'YYYYMMDDHH24MISS') "
            << "from "
            << PA_ZONE_TABLE_NAME;*/
		//++jinhua TD17650

        // Add in optional joined table       
        if (joinedTable.length() > 0)
        {
            sql << ", " << strlwr(const_cast<char*>(joinedTable.c_str()));
        }  
		TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql  = pDb->prepareSQLStatement(PA_ZONE_SELECT_44551, sql.str());
		SQLStatement strSql;
		pDb->prepareSQLStatement(result.sqlStatement, PA_ZONE_SELECT_44551, sql.str());
        //result.statement   = strSql;
        
        //waitforcheck delete by lin
		/*std::string strExSql;
		int nExDbType= pDb->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}*/

        return result;
    }


    void PaZoneHelper::writePaZoneData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaZoneData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        if (!m_id.hasBeenSet())
        {
            fieldNames.push_back(ID_COL);
        }
        if (m_label.empty())
        {
            fieldNames.push_back(LABEL_COL);
        }
        // 4669FAT TD #782: This entry may be null in database
        //if (!m_statusEntityKey.hasBeenSet())
        //{
        //    fieldNames.push_back(EQUIPMENTENTITYKEY_COL);
        //}        
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaZone data not fully specified. PaZone cannot be written to database",
                                                         fieldNames));
        }

        // Confirm the ID and location combination are unique within the database
        validateIdLocationPairUniqueness();
        validateEquipmentEntityNameUniqueness(OCC);
        validateEquipmentEntityNameUniqueness(STN);

        // Write the PaZone to the database

        try 
        {
            // Need to check if this is a new PaZone or an existing PaZone - 
            // as a new PaZone will be inserted, while an existing PaZone will be updated
            if (m_isNew) // This is a new PaZone
            {
                addNewPaZone();
            }
            else // This is an existing PaZone
            {
                modifyExistingPaZone();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaZone data not fully specified. PaZone cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    
    void PaZoneHelper::validateEquipmentEntityNameUniqueness(ELocationType locationType)
    {  
        std::string equipmentEntityName;
        std::string equipmentEntityNameColName;
        std::string locType;

        switch (locationType)
        {
        case OCC:
            equipmentEntityName = m_occEquipmentEntityName;
            equipmentEntityNameColName = OCC_EQUIPMENTENTITYNAME_COL;
            locType = "OCC";
            break;
        case STN:
            equipmentEntityName = m_stnEquipmentEntityName;
            equipmentEntityNameColName = STN_EQUIPMENTENTITYNAME_COL;
            locType = "Station";
            break;
        default:
            TA_ASSERT(false, "Unexpected case");
        };

        // TES #782 - improved feedback to operator from configuration editor
        if (equipmentEntityName.length() == 0)
        {
            // Nothing required
            return;
        }

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);  
        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_ZONE_TABLE_NAME 
            << " where " << databaseConnection->escapeQueryString(equipmentEntityNameColName)
            << " = '" << databaseConnection->escapeQueryString(equipmentEntityName) << "'";*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44503,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44503,
			databaseConnection->escapeQueryString(equipmentEntityNameColName), databaseConnection->escapeQueryString(equipmentEntityName));
            
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

            // Ignore ourself..
            if (duplicateItemKey != m_key)
            {
                std::ostringstream str;
                str << "Attempting to write Pa Zone record that doesn't "
                    << "have a unique " << locType << " equipment entity name (existing duplicate has pKey="
                    << duplicateItemKey
                    << ")";
                
                std::ostringstream exceptionStr;
                exceptionStr << locType << " Equipment Entity Key";

                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                        TA_Base_Core::DataException::NOT_UNIQUE,
                        exceptionStr.str().c_str()) );
            }
        }     
    }


    void PaZoneHelper::validateIdLocationPairUniqueness()
    {   
       /* std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_ZONE_TABLE_NAME 
            << " where " << LOCATIONKEY_COL << " = " << m_locationKey.getValue()
            << " and " << ID_COL << " = " << m_id.getValue();*/

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read); 
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44504, m_locationKey.getValue(), m_id.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44504, m_locationKey.getValue(), m_id.getValue());
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

            // Ignore ourself..
            if (duplicateItemKey != m_key)
            {
                std::ostringstream str;
                str << "Attempting to write Pa Zone record that doesn't "
                    << "have unique id/location (existing duplicate has pKey="
                    << duplicateItemKey
                    << ")";

                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                        TA_Base_Core::DataException::NOT_UNIQUE,
                        "LocationKey and Id") );
            }
        }
    }


    void PaZoneHelper::deletePaZone(bool cascade)
    {
        FUNCTION_ENTRY("deletePaZone");

        TA_ASSERT(!m_isNew, "This PaZone does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        //std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            PA_ZONE_TABLE_NAME,
                                            KEY_COL,
                                            m_key,
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    PA_ZONE_TABLE_NAME.c_str()));
            }
        }
        

        // Good to go...
       /* sql.str("");
        sql << "delete " << PA_ZONE_TABLE_NAME << " where " << KEY_COL << " = " << m_key;*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_INSERT_44901, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_INSERT_44901, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }

    //void PaZoneHelper::updateEquipmentEntityAssociations()
    //{
  //      if (!m_associatedEquipmentEntitySet)
  //      {
  //          loadAssociatedEquipmentEntityData();
  //      }

  //      FUNCTION_ENTRY("updateEquipmentEntityAssociations");

  //      // get a connection to the database
  //      TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

  //     /* std::ostringstream sql;
  //      sql << "select " << PA_ZONE_HELPER_ENTITY_NAME_COL 
  //          << " from " << PA_ZONE_HELPER_TABLE_NAME 
  //          << " where " 
  //          << KEY_COL << " = " << m_key;*/
		//std::string strSql = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44505, m_key);

  //      // Set up the vector to pass to exectueQuery
  //      std::vector<std::string> columnNames;
  //      columnNames.push_back(PA_ZONE_HELPER_ENTITY_NAME_COL);

  //      // execute the query
  //      TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

  //      std::list<std::string> associationsToAdd = m_associatedEquipmentEntity;
  //      std::list<std::string> associationsToRemove;

  //      // Now we step through the names retrieved from the database.  If we 
  //      // find the names in both lists then the database is up-to-date and 
  //      // nothing needs to be done to the lists (ie we dont' add it to the 
  //      // associationsToRemove list). At the end, the names left in the 
  //      // associationsToAdd list need to be added to the database and the 
  //      // names left in associationsToRemove must be removed from the
  //      // database.
  //      for (unsigned int i = 0; i < data->getNumRows(); ++i)
  //      {
  //          std::string currentName = data->getStringData(i, PA_ZONE_HELPER_ENTITY_NAME_COL);

  //          std::list<std::string>::iterator matching = std::find(associationsToAdd.begin(), associationsToAdd.end(), currentName);
  //          if (matching != associationsToAdd.end())
  //          {
  //              // This name has been found so it has not changed
  //              // We remove it from the associationsToRemove list as we do 
  //              // not need to remove it from the database
  //              associationsToAdd.erase( matching );
  //          }
  //          else
  //          {
  //              // This name has obviously been removed from our local list so 
  //              // it will need to be removed from the database
  //              associationsToRemove.push_back( currentName );
  //          }
  //      }

  //      delete data;
  //      data = NULL;

  //      // Now we add and delete from/to the database as required
  //     
  //      // First lets delete old values from the database
  //      if (!associationsToRemove.empty())
  //      {
  //          std::ostringstream sql;
  //         /* sql << "delete " << PA_ZONE_HELPER_TABLE_NAME 
  //              << " where " << PA_ZONE_HELPER_ENTITY_NAME_COL 
  //              << " in (";*/

  //          for (std::list<std::string>::iterator iterToRemove = associationsToRemove.begin();
  //               iterToRemove != associationsToRemove.end(); ++iterToRemove)
  //          {
  //              if ( iterToRemove != associationsToRemove.begin())
  //              {
  //                  // If this is not the first item then we must insert a comma
  //                  sql << ",";
  //              }
  //              sql << "'" << databaseConnection->escapeInsertString(*iterToRemove)
  //                  << "'";
  //          }    
  //        /*  sql << ") and "
  //              << KEY_COL << " = " << m_key;*/
		//	strSql = databaseConnection->prepareSQLStatement(PA_ZONE_INSERT_44902, sql.str(), m_key);

  //          databaseConnection->executeModification(strSql);
  //      }

  //      // Now lets add new values to the database
  //      if (!associationsToAdd.empty())
  //      {
  //          for (std::list<std::string>::iterator iterToAdd = associationsToAdd.begin();
  //               iterToAdd != associationsToAdd.end(); ++iterToAdd)
  //          {

  //              validateEquipmentEntityExistence(*iterToAdd);
  //              
  //              ////////////////////////////////////////////////////////////////////////////
  //              //
  //              // Good to go...
  //              //
  //              ////////////////////////////////////////////////////////////////////////////

  //            /*  std::ostringstream sql;
  //              sql << "insert into " << PA_ZONE_HELPER_TABLE_NAME << " ( " 
  //                  << PA_ZONE_HELPER_KEY_COL <<  " , " 
  //                  << PA_ZONE_HELPER_ENTITY_NAME_COL << " , "
  //                  << KEY_COL << " ) "
  //                  << "values ( PAZHEL_SEQ.NEXTVAL , '" 
  //                  << databaseConnection->escapeInsertString(*iterToAdd)
  //                  << "' , " << m_key << " )";*/
  //              // ^^ TD #3488, added '' quotes about the name data
		//		strSql = databaseConnection->prepareSQLStatement(PA_ZONE_INSERT_44851, 
		//			databaseConnection->escapeInsertString(*iterToAdd), m_key);

  //              databaseConnection->executeModification(strSql);
  //          }
  //      }
  //               
  //      FUNCTION_EXIT;
    //}

    void PaZoneHelper::modifyExistingPaZone() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaZone");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Zone %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

       /* std::ostringstream sql;

        sql << "update " << PA_ZONE_TABLE_NAME << " set " 
            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
            << ID_COL << " = " << m_id.getValue() << ", "
            << LABEL_COL << " = '"
            << (databaseConnection->escapeInsertString(m_label)).c_str() << "', " 
            << STN_EQUIPMENTENTITYNAME_COL << " = " << getDbStnEntityNameString() << ", " 
            << OCC_EQUIPMENTENTITYNAME_COL << " = " << getDbOccEntityNameString() << " "
            << "where " << KEY_COL << " = " << m_key;  */
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_ZONE_UPDATE_44651,  m_locationKey.getValue(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_UPDATE_44651,  m_locationKey.getValue(),
			m_id.getValue(), databaseConnection->escapeInsertString(m_label), getDbStnEntityNameString(), getDbOccEntityNameString(),m_key);

        databaseConnection->executeModification(strSql);

        // now need to update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(" << DATEMODIFIED_COL << ",'YYYYMMDDHH24MISS') "
            << "from " << PA_ZONE_TABLE_NAME << " where " << KEY_COL << " = " << m_key;*/
//		strSql = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44552, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44552, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

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

        // Now update the EquipmentEntity associations in the database
        //updateEquipmentEntityAssociations();

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void PaZoneHelper::addNewPaZone() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaZone");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA Zone %u is a new PaZone. It will be added to the database.",m_locationKey.getValue());

        validateLocationKeyExistence();        

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        // Obtain the primary key that should be used for this next entry
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(databaseConnection,
                                                                    PA_ZONE_SEQUENCE_COUNTER);

       /* std::ostringstream sql;
        sql << "insert into " << PA_ZONE_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << LOCATIONKEY_COL << ", "
            << ID_COL << ", "
            << LABEL_COL << ", "
            << STN_EQUIPMENTENTITYNAME_COL << ", "
            << OCC_EQUIPMENTENTITYNAME_COL << ") "
            << "values (" << sequenceNumber << ","
            << m_locationKey.getValue() << "," 
            << m_id.getValue() << ",'" 
            << (databaseConnection->escapeInsertString(m_label)).c_str() << "'," 
            << getDbStnEntityNameString() << ","
            << getDbOccEntityNameString() << ")";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_ZONE_INSERT_44801, sequenceNumber,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_INSERT_44801, sequenceNumber,
			m_locationKey.getValue(), m_id.getValue(), databaseConnection->escapeInsertString(m_label), getDbStnEntityNameString(),
			getDbOccEntityNameString());

        databaseConnection->executeModification(strSql);

       /* sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS') "
            << "from " << PA_ZONE_TABLE_NAME << " where "
            << KEY_COL << " = " << sequenceNumber;*/
//		strSql = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44553, sequenceNumber);

		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44553, sequenceNumber);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Zone key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,KEY_COL);
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

        // Now update the EquipmentEntity associations in the database
        //updateEquipmentEntityAssociations();

        FUNCTION_EXIT;
    }


    void PaZoneHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
       /* std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();	*/	
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7003,m_locationKey.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7003,m_locationKey.getValue());
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


  //  void PaZoneHelper::associateZoneWithEquipmentEntity(unsigned long zoneKey,
  //                                            const std::string& entityName)
  //  {
  //      // First off check this will be a unique entry
  //      validateNewZoneEntityNameUniqueness(zoneKey, entityName);

  //      // Calculate required sequence number (primary key) for insertion
  //      TA_Base_Core::IDatabase* databaseConnection = 
  //                  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

  //      unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
  //                                        databaseConnection,
  //                                        PA_ZONE_HELPER_SEQUENCE_COUNTER);

  //      // Now insert entry into the database...        
  //     /* std::ostringstream sql;
  //      sql << "insert into " << PA_ZONE_HELPER_TABLE_NAME << " (" 
  //          << PA_ZONE_HELPER_KEY_COL << ", "
  //          << PA_ZONE_HELPER_ENTITY_NAME_COL << ", "
  //          << KEY_COL << ") "
  //          << "values (" << sequenceNumber << ",'"
  //          << (databaseConnection->escapeInsertString(entityName)).c_str() << "'," 
  //          << zoneKey  << ")";*/
		//std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_INSERT_44802, sequenceNumber,
		//	databaseConnection->escapeInsertString(entityName), zoneKey);

  //      databaseConnection->executeModification(strSql);
  //  }


    void PaZoneHelper::validateNewZoneEntityNameUniqueness(unsigned long zoneKey,
                                                        const std::string& entityName)
    {
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);   
        /*std::ostringstream sql;
        sql << "select " << PA_ZONE_HELPER_KEY_COL
            << " from " << PA_ZONE_HELPER_TABLE_NAME 
            << " where " << KEY_COL << " = " << zoneKey
            << " and " << PA_ZONE_HELPER_ENTITY_NAME_COL << " = '"
            << (databaseConnection->escapeInsertString(entityName)).c_str() << "'";*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44506, zoneKey,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44506, zoneKey,
			databaseConnection->escapeInsertString(entityName));
            

        std::vector<std::string> columnNames;
        columnNames.push_back(PA_ZONE_HELPER_KEY_COL);
                  
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, PA_ZONE_HELPER_KEY_COL);

            std::ostringstream str;
            str << "Attempting to write Pa Zone record that doesn't "
                << "have unique zone/entityname (existing duplicate has pKey="
                << duplicateItemKey
                << ")";
    
            TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                                TA_Base_Core::DataException::NOT_UNIQUE,
                                "EntityName and ZoneKey") );            
        }
    }

    void PaZoneHelper::validateEquipmentEntityExistence(const std::string& equipmentEntity) 
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Check that the equipment entity name already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        std::ostringstream sql;

        // TD #3488: Added escapeInsertString to fix database check against the name
       /* sql << "select PKEY from ENTITY where NAME = '" 
            << (databaseConnection->escapeInsertString(equipmentEntity.c_str()))
            << "'";*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4503,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4503,
			databaseConnection->escapeInsertString(equipmentEntity.c_str()));

        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");

        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for equipment with entityname = " << equipmentEntity;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Equipment entity name" ) );
        }

        delete data;
        data = NULL;

    }


    // Added to resolve TD #3488
    std::string PaZoneHelper::getDbStnEntityNameString() const
    {
        // Because of referential integrity constraint on the status entity key,
        // must use null entries where the key is set to invalid
        if (0 == m_stnEquipmentEntityName.length())
        {
            return "null";
        }
        else
        {
            std::ostringstream ostr;
            ostr << "'" 
                 << TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read)->escapeInsertString(m_stnEquipmentEntityName).c_str()
                 << "'";
            return ostr.str();
        }
    }


    std::string PaZoneHelper::getDbOccEntityNameString() const
    {
        // Because of referential integrity constraint on the status entity key,
        // must use null entries where the key is set to invalid
        if (0 == m_occEquipmentEntityName.length())
        {
            return "null";
        }
        else
        {
            std::ostringstream ostr;
            ostr << "'" 
                 << TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read)->escapeInsertString(m_occEquipmentEntityName).c_str()
                 << "'";
            return ostr.str();
        }
    }
        
    void PaZoneHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_id.setValue(data.getUnsignedLongData( row, ID_COL ));
		m_label   = data.getStringData( row, LABEL_COL );
		m_stnEquipmentEntityName = data.getStringData( row, STN_EQUIPMENTENTITYNAME_COL, "");
        m_occEquipmentEntityName = data.getStringData( row, OCC_EQUIPMENTENTITYNAME_COL, "");
        m_dateCreated = data.getDateData( row, DATECREATED_COL );
		m_dateModified = data.getDateData( row, DATEMODIFIED_COL, 0);
        
        // Need to flag ourselves that we now have valid data
        m_isValidData = true;
    }

    
    //void PaZoneHelper::loadAssociatedEquipmentEntityData()
    //{
  //      // Set flag immediately, if load fails, means data will remain in validated state
  //      m_associatedEquipmentEntitySet = true;

  //      // create the SQL string to retrieve the data of the PaZone
  //      // based upon the key
  //    /*  std::ostringstream sql;

  //      sql << "select " << PA_ZONE_HELPER_ENTITY_NAME_COL
  //          << " from " << PA_ZONE_HELPER_TABLE_NAME << " where "
  //          << KEY_COL << " = " << m_key;	*/	

  //      // Set up the columnNames vector to be passed to executeQuery()
  //      std::vector<std::string> columnNames;
  //      columnNames.push_back(PA_ZONE_HELPER_ENTITY_NAME_COL);

  //      TA_Base_Core::IDatabase* databaseConnection = 
  //              TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		//std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44505, m_key);
  //      TA_Base_Core::IData* equipData = databaseConnection->executeQuery(strSql, columnNames);

  //      m_associatedEquipmentEntity.clear();

  //      for (unsigned int i = 0; i< equipData->getNumRows(); ++i)
  //      {
  //          m_associatedEquipmentEntity.push_back(equipData->getStringData( i, PA_ZONE_HELPER_ENTITY_NAME_COL ));
  //      }

  //      delete equipData;
  //      equipData = NULL;
    //}
} // closes TA_Base_Core

