/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFileHelper.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class contains the data associated with one
  * entry in the so_sound_file table.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "ISoundCategory.h"
#include "SoundCategoryAccessFactory.h"
#include "SoundFileHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include <memory>
#include <sstream>

namespace TA_Base_Core
{

	SoundFileHelper::SoundFileHelper(unsigned long categoryKey, unsigned long priority)
		: m_categoryKey(categoryKey), m_priority(priority), m_isValidData(false),
          m_isNew(false), m_description(""), m_soundFile(""), m_dateCreated(0), m_dateModified(0)
	{
	}


	SoundFileHelper::SoundFileHelper()
		: m_categoryKey(0), m_priority(0), m_isValidData(false), m_isNew(true), m_description(""),
          m_soundFile(""), m_dateCreated(0), m_dateModified(0)
	{
	}


	SoundFileHelper::SoundFileHelper( const SoundFileHelper& rhs)
		: m_categoryKey(rhs.m_categoryKey), m_priority(rhs.m_priority),
		  m_isValidData(false), m_isNew(true), m_description(rhs.m_description), m_soundFile(rhs.m_soundFile),
          m_dateCreated(rhs.m_dateCreated), m_dateModified(rhs.m_dateModified)
	{
	}


	SoundFileHelper::~SoundFileHelper()
	{
	}


	unsigned long SoundFileHelper::getCategoryKey()
	{
		return m_categoryKey;
	}


	unsigned long SoundFileHelper::getPriority()
	{
		return m_priority;
	}


    std::string SoundFileHelper::getDescription()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
		return m_description;
    }


	std::string SoundFileHelper::getSoundFile()
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
		return m_soundFile;
	}


    time_t SoundFileHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t SoundFileHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }


	void SoundFileHelper::setCategoryKey(unsigned long pkey)
	{
        // Category should only be changed when it is a new item, otherwise it
        // will change what row is being modified in the database!
        TA_ASSERT(m_isNew,"Category can only be set for new sound files.");

		m_categoryKey = pkey;
	}


	void SoundFileHelper::setPriority(unsigned long priority)
	{
        // Priority should only be changed when it is a new item, otherwise it
        // will change what row is being modified in the database!
        TA_ASSERT(m_isNew,"Priority can only be set for new sound files.");

		m_priority = priority;
	}


    void SoundFileHelper::setDescription(std::string description)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_description = description;
    }


	void SoundFileHelper::setSoundFile(std::string file)
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_soundFile = file;
	}


	void SoundFileHelper::deleteThisSoundFile()
	{
		FUNCTION_ENTRY("deleteThisSoundFile");

        TA_ASSERT(!m_isNew, "This SoundFile does not yet exist in the database, and therefore cannot be deleted");

        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);
        
        // We must now attempt to delete the SoundFile.
        /*std::ostringstream sql;
        sql << "delete SO_SOUND_FILE where SOSCAT_ID = " << m_categoryKey;
		sql << " and PRIORITY = " << m_priority;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_STD_DELETE_38401,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_DELETE_38401,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_DELETE_38401,
			m_categoryKey, m_priority);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}


	void SoundFileHelper::applyChanges()
	{
        FUNCTION_ENTRY("applyChanges");
       
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
		if (m_categoryKey == 0)
		{
			fieldNames.push_back("Category key");
		}
		if (m_priority == 0)
		{
			fieldNames.push_back("Priority");
		}
        if (m_soundFile.empty())
        {
            fieldNames.push_back("Sound File");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("SoundFile data not fully specified. SoundFile cannot be written to database", fieldNames) );
        }

        // Write the SoundFile to the database

        // Need to check if this is a new SoundFile or an existing SoundFile - 
        // as a new SoundFile will be inserted, while an existing system controller will be updated.
        if (m_isNew)
        {
             // This is a new SoundFile
            addNew();
        }
        else 
        {
            // This is an existing SoundFile
            modifyExisting();
        }

        // Make sure the dates are refreshed.
        reload();

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
	}


	void SoundFileHelper::invalidate()
	{
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(!m_isNew, "Cannot call invalidate on a new object");

        // Set the flag to false
        m_isValidData = false;

        FUNCTION_EXIT;
	}


	void SoundFileHelper::reload()
	{
        FUNCTION_ENTRY("reload");

        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Read);
        
        // Create the SQL string to retrieve the data of the SoundFile
        // based upon the category key and priority.
       /* std::ostringstream sql;
        sql << "SELECT description, sound_file, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "FROM so_sound_file WHERE soscat_id = " << m_categoryKey;
		sql << " and priority = " << m_priority;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_Oracle_SELECT_38051,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_SELECT_38051,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_SELECT_38051,
			m_categoryKey, m_priority);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string descriptionColumn = "DESCRIPTION";
        std::string soundFileColumn = "SOUND_FILE";
        std::string dateCreatedColumn = "DATE_CREATED";
        std::string dateModifiedColumn = "DATE_MODIFIED";
        
        std::vector<std::string> columnNames;
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(soundFileColumn);
        columnNames.push_back(dateCreatedColumn);
        columnNames.push_back(dateModifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        std::auto_ptr<IData> data( databaseConnection->executeQuery(strSql,columnNames) );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reason;
            reason << "No SoundFile found for soscat_id = " << m_categoryKey;
			reason << " and priority = " << m_priority;
            TA_THROW(DataException(reason.str().c_str(), DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::ostringstream reason;
            reason << "More than one SoundFile found for soscat_id = " << m_categoryKey;
			reason << " and priority = " << m_priority;
            TA_THROW(DataException(reason.str().c_str(), DataException::NOT_UNIQUE, ""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions.
        m_description = data->getStringData(0, descriptionColumn);
		m_soundFile = data->getStringData(0, soundFileColumn);
        m_dateCreated = data->getDateData(0, dateCreatedColumn);

        try
        {
            m_dateModified = data->getDateData(0,dateModifiedColumn);
        }
        catch (DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Need to record that we now have valid data.
        m_isValidData = true; 

        FUNCTION_EXIT;
	}


	void SoundFileHelper::modifyExisting()
	{
        FUNCTION_ENTRY("modifyExisting");
    
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"SoundFile in category %lu with priority %lu already exists. It's details will be updated.",
            m_categoryKey, m_priority);

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);

       /* std::ostringstream sql;
        sql << "update SO_SOUND_FILE set ";
        sql << "DESCRIPTION = '" << databaseConnection->escapeInsertString(m_description) << "', ";
        sql << "SOUND_FILE = '" << databaseConnection->escapeInsertString(m_soundFile) << "' ";
        sql << "where SOSCAT_ID = " << m_categoryKey << " ";
        sql << "and PRIORITY = " << m_priority;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_STD_UPDATE_38151,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_UPDATE_38151,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_UPDATE_38151,
			databaseConnection->escapeInsertString(m_description), databaseConnection->escapeInsertString(m_soundFile),
			m_categoryKey, m_priority);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}


	void SoundFileHelper::addNew()
	{
        FUNCTION_ENTRY("addNew");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"SoundFile with category %lu and priority %lu is a new SoundFile. It will be added to the database.",
            m_categoryKey, m_priority);

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Write);
     
        // Generate the SQL string to write the SoundFile.
		/*std::ostringstream formatSQL;
        formatSQL << "insert into SO_SOUND_FILE(SOSCAT_ID, PRIORITY, DESCRIPTION, SOUND_FILE) ";
        formatSQL << "values (";
		formatSQL << m_categoryKey << ", ";
        formatSQL << m_priority << ", '";
        formatSQL << databaseConnection->escapeInsertString(m_description) << "', '";
		formatSQL << databaseConnection->escapeInsertString(m_soundFile) << "'";
        formatSQL << ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_STD_INSERT_38301, m_categoryKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_INSERT_38301, m_categoryKey,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_INSERT_38301, m_categoryKey,
			m_priority, databaseConnection->escapeInsertString(m_description), databaseConnection->escapeInsertString(m_soundFile));
 
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}

    std::string SoundFileHelper::retrieveCategoryName( const unsigned long key )
    {
        FUNCTION_ENTRY( "retrieveCategoryName" );

        std::string name("");

        try
        {
            // Retrieve the category information.
            std::auto_ptr<ISoundCategory> category( SoundCategoryAccessFactory::getInstance().getSoundCategory( key ) );

            // Now get the name.
            name = category->getName();
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the entity name");
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the entity name");
        }        

        FUNCTION_EXIT;
        return name;
    }

} // namespace TA_Base_Core
