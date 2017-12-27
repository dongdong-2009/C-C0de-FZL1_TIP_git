/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundCategoryHelper.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class contains the data associated with one
  * entry in the so_sound_category table.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "SoundCategoryHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include <sstream>

namespace TA_Base_Core
{

	SoundCategoryHelper::SoundCategoryHelper(unsigned long categoryKey)
		: m_categoryKey(categoryKey), m_name(""), m_position(-1),
		  m_isValidData(false), m_isNew(false)
	{
	}


	SoundCategoryHelper::SoundCategoryHelper()
		: m_categoryKey(0), m_name(""), m_position(-1), 
		  m_isValidData(false), m_isNew(true)
	{
	}


	SoundCategoryHelper::SoundCategoryHelper( const SoundCategoryHelper& rhs)
		: m_categoryKey(rhs.m_categoryKey), m_name(rhs.m_name), m_position(rhs.m_position),
		  m_isValidData(false), m_isNew(true)
	{
	}


	SoundCategoryHelper::~SoundCategoryHelper()
	{
	}


	unsigned long SoundCategoryHelper::getCategoryKey()
	{
		return m_categoryKey;
	}


	std::string SoundCategoryHelper::getName()
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
		return m_name;
	}


	unsigned long SoundCategoryHelper::getPosition()
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
		return m_position;
	}


	void SoundCategoryHelper::setCategoryKey(unsigned long pkey)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_categoryKey = pkey;
	}


	void SoundCategoryHelper::setName(std::string name)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_name = name;
	}


	void SoundCategoryHelper::setPosition(unsigned long position)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_position = position;
	}


	void SoundCategoryHelper::deleteThisSoundCategory()
	{
		FUNCTION_ENTRY("deleteThisSoundCategory");

        TA_ASSERT(!m_isNew, "This SoundCategory does not yet exist in the database, and therefore cannot be deleted");

        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);
        
        // We must now attempt to delete the SoundCategory.
       /* std::ostringstream sql;
        sql << "delete SO_SOUND_CATEGORY where SOSCAT_ID = " << m_categoryKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), so_sound_category_STD_DELETE_37901, m_categoryKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_CATEGORY_DELETE_37901, m_categoryKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_CATEGORY_DELETE_37901, m_categoryKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}


	void SoundCategoryHelper::applyChanges()
	{
        FUNCTION_ENTRY("applyChanges");
       
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.
        // Don't need to check key - the key will never be "invalid".

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
		if (m_position == -1)
		{
			fieldNames.push_back("Position");
		}

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("SoundCategory data not fully specified. SoundCategory cannot be written to database", fieldNames) );
        }

        // Write the SoundCategory to the database

        // Need to check if this is a new SoundCategory or an existing SoundCategory - 
        // as a new SoundCategory will be inserted, while an existing system controller will be updated.
        if (m_isNew)
        {
             // This is a new SoundCategory
            addNew();
        }
        else 
        {
            // This is an existing SoundCategory
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
	}


	void SoundCategoryHelper::invalidate()
	{
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(!m_isNew, "Cannot call invalidate on a new object");

        // Set the flag to false
        m_isValidData = false;

        FUNCTION_EXIT;
	}


	void SoundCategoryHelper::reload()
	{
        FUNCTION_ENTRY("reload");

        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Read);
        
        // Create the SQL string to retrieve the data of the SoundCategory
        // based upon the primary key.
       /* std::ostringstream sql;
        sql << "SELECT name, position ";
        sql << "FROM so_sound_category WHERE soscat_id = " << m_categoryKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), so_sound_category_STD_SELECT_37502, m_categoryKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_CATEGORY_SELECT_37502, m_categoryKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_CATEGORY_SELECT_37502, m_categoryKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
		std::string positionColumn = "POSITION";
        
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        columnNames.push_back(positionColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reason;
            reason << "No SoundCategory found for soscat_id = " << m_categoryKey;
            TA_THROW(DataException(reason.str().c_str(), DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::ostringstream reason;
            reason << "More than one SoundCategory found for soscat_id = " << m_categoryKey;
            TA_THROW(DataException(reason.str().c_str(), DataException::NOT_UNIQUE, ""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions.
		m_name = data->getStringData(0, nameColumn);
		m_position = data->getUnsignedLongData(0, positionColumn);

        // Now that we're done with the IData object, it's our responsibility to 
        // delete it.
        delete data;
        data = NULL;

        // Need to record that we now have valid data.
        m_isValidData = true; 

        FUNCTION_EXIT;
	}


	void SoundCategoryHelper::modifyExisting()
	{
        FUNCTION_ENTRY("modifyExisting");
    
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"SoundCategory with key %lu already exists. It's details will be updated.",
            m_categoryKey);

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);

      /*  std::ostringstream sql;
        sql << "update SO_SOUND_CATEGORY set ";
        sql << "NAME = '" << databaseConnection->escapeInsertString(m_name);
        sql << "', POSITION = " << m_position;
		sql << " where SOSCAT_ID = " << m_categoryKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), so_sound_category_STD_UPDATE_37651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_CATEGORY_UPDATE_37651,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_CATEGORY_UPDATE_37651,
			databaseConnection->escapeInsertString(m_name), m_position, m_categoryKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}


	void SoundCategoryHelper::addNew()
	{
        FUNCTION_ENTRY("addNew");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"SoundCategory with key %lu is a new SoundCategory. It will be added to the database.",
            m_categoryKey);

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);
     
        // Generate the SQL string to write the SoundCategory.
		/*std::ostringstream formatSQL;
        formatSQL << "insert into SO_SOUND_CATEGORY(SOSCAT_ID, NAME, POSITION) ";
        formatSQL << "values (";
		formatSQL << m_categoryKey << ", '";
        formatSQL << databaseConnection->escapeInsertString(m_name) << "', ";
		formatSQL << m_position << ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), so_sound_category_STD_INSERT_37801,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_CATEGORY_INSERT_37801,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_CATEGORY_INSERT_37801,
			m_categoryKey, databaseConnection->escapeInsertString(m_name), m_position);
 
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}

} // namespace TA_Base_Core
