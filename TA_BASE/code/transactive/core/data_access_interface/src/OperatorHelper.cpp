/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OperatorHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2017/07/10 17:20:51 $
  * Last modified by:  $Author: Ouyang $
  *
  * OperatorHelper is an object that is held by Operator and ConfigOperator objects. 
  * It contains all data used by operators, and manipulation
  * methods for the data. It helps avoid re-writing code for both Operator and ConfigOperator.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/src/OperatorHelper.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/data_access_interface/src/OperatorPassword.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string OperatorHelper::DELETED_FLAG("#");
    const unsigned long OperatorHelper::DEFAULT_NAME_MAX_LENGTH = 85;
    const unsigned long OperatorHelper::DEFAULT_DESCRIPTION_MAX_LENGTH = 80;
    volatile unsigned long OperatorHelper::m_nameMaxLength = 0;
    TA_Base_Core::NonReEntrantThreadLockable OperatorHelper::m_nameMaxLengthLock;

	OperatorHelper::OperatorHelper( const OperatorHelper& theOperatorHelper)
		: m_name(theOperatorHelper.m_name),
		  m_description(theOperatorHelper.m_description),
          m_password(theOperatorHelper.m_password),
          m_isSystemOperator(theOperatorHelper.m_isSystemOperator),
          m_dateCreated(0),
          m_dateModified(0),
          m_loadProfiles(theOperatorHelper.m_loadProfiles),
          m_loadRegions(theOperatorHelper.m_loadRegions),
          m_associatedProfiles(theOperatorHelper.m_associatedProfiles),
		  m_associatedRegions(theOperatorHelper.m_associatedRegions),
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isValidRegionData(false),
          m_isNew(true),
          m_descriptionMaxLength(theOperatorHelper.m_descriptionMaxLength)
    {	  
	}


    OperatorHelper::OperatorHelper(const unsigned long key)
        : m_key(key),
          m_dateCreated(0),
          m_dateModified(0),
          m_loadProfiles(true),
          m_loadRegions(true),
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isValidRegionData(false),         
          m_isNew(false)
    {
    }


    OperatorHelper::OperatorHelper()
        : m_isSystemOperator(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_loadProfiles(true),
          m_loadRegions(true),
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isValidRegionData(false),         
          m_isNew(true)
    {
        initialiseMaxFieldLengths();
    }


    OperatorHelper::OperatorHelper(
		const unsigned long key,
		const std::string& name,bool isSystemOperator):
	      m_key(key),
          m_name(name.c_str()),
		  m_isSystemOperator(isSystemOperator),
		  m_dateModified(false),
          m_loadProfiles(false ),
          m_loadRegions(false ),
          m_isValidData(true),
          m_isValidProfileData(false),
          m_isValidRegionData(false),
          m_isNew(false)
	{

	}


    OperatorHelper::OperatorHelper(
		const unsigned long key,
		const std::string& name,
		const std::string& description,
		const std::string& password,
		bool isSystemOperator,
		time_t dateCreated,
		time_t dateModified, 
		bool loadProfiles,
		bool loadRegions
		)
        : m_key(key),
          m_name(name),
          m_description(description),
          m_password(password),
          m_isSystemOperator(isSystemOperator),
          m_dateCreated(dateCreated),
          m_dateModified(dateModified),
          m_loadProfiles(loadProfiles),
          m_loadRegions(loadRegions),
          m_isValidData(true),
          m_isValidProfileData(false),
          m_isValidRegionData(false),
          m_isNew(false)
    {
        initialiseMaxFieldLengths();
    }



	OperatorHelper::OperatorHelper(unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions)
        : 
		m_loadProfiles(false),
		m_loadRegions(false),
		m_isValidData(false),
		m_isValidProfileData(false),
		m_isValidRegionData(false),
		m_isNew(false)
    {
        FUNCTION_ENTRY("OperatorHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data, profiles, regions);
        FUNCTION_EXIT;
    }


    OperatorHelper::~OperatorHelper()
    {

        try
        {
        }
		//exception
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long OperatorHelper::getKey()
    {
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"The data must be written to the database before the key can be retrieved");
        
        return m_key;
    }


    std::string OperatorHelper::getName()
    {

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


    void OperatorHelper::setName(const std::string& name)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::setName" );
        
		if (!m_isNew)
        {

            if (!m_isValidData)
            {
                reload();
            }

            if (!m_isValidProfileData && m_loadProfiles)
            {
                reloadProfiles();
            }
        }
        m_name = name;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::setName" );
    }


    std::string OperatorHelper::getDescription()
    {

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_description;
    }


    void OperatorHelper::setDescription(const std::string& description)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::setDescription" );
      
		if (!m_isNew)
        {

            if (!m_isValidData)
            {
                reload();
            }

            if (!m_isValidProfileData && m_loadProfiles)
            {
                reloadProfiles();
            }
        }
        m_description = description;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::setDescription" );
    }


	std::string OperatorHelper::getPassword()
	{

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_password;
	}


	void OperatorHelper::setPassword( const std::string password )
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::setPassword" );
       
		if (!m_isNew)
        {

            if (!m_isValidData)
            {
                reload();
            }

            if (!m_isValidProfileData && m_loadProfiles)
            {
                reloadProfiles();
            }
        }
        m_password = password;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::setPassword" );
	}


    bool OperatorHelper::isSystemOperator()
	{

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_isSystemOperator;
	}


    time_t OperatorHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_dateCreated;
    }


    time_t OperatorHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_dateModified;
    }


    std::vector<IProfile*> OperatorHelper::getAssociatedProfiles()
	{
        TA_ASSERT(!m_isNew,"The data must be written to the database before the associated profiles can be retrieved");

        // create the list of IProfile based on the associated profile keys
        std::vector<unsigned long> keys = getAssociatedProfileKeys();
        std::vector<IProfile*> ret;

		std::vector<unsigned long>::const_iterator it;

        for (it=keys.begin(); it!=keys.end(); it++)
        {
            ret.push_back( new Profile(*it) );
        }

        return ret;
	}

    std::vector<unsigned long> OperatorHelper::getAssociatedRegionKeys()
	{

        if (!m_isValidRegionData && !m_isNew)
        {
            reloadRegions();
        }

        return m_associatedRegions;
	}

    std::vector<unsigned long> OperatorHelper::getAssociatedProfileKeys()
    {

        if (!m_isValidProfileData && !m_isNew)
        {
            reloadProfiles();
        }

        return m_associatedProfiles;
    }



    void OperatorHelper::setAssociatedProfile( unsigned long profileKey, bool isAssociated)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::setAssociatedProfile" );

        if (!m_isNew)
        {

            if (!m_isValidData)
            {
                reload();
            }

            if (!m_isValidProfileData)
            {
                reloadProfiles();
            }
        }

        std::vector<unsigned long>::iterator matchingProfile = std::find(m_associatedProfiles.begin(), m_associatedProfiles.end(), profileKey);

        if (isAssociated)
        {
            // we need to add this profile
            if ( matchingProfile == m_associatedProfiles.end() )
            {
                // This is definitely not already in the list so add it
                m_associatedProfiles.push_back(profileKey);
            }
            // else the item is already in the list
        }

        else
        {

            // we need to remove this profile
            if ( matchingProfile != m_associatedProfiles.end() )
            {
                // This is definitely in the list so we must remove it
                m_associatedProfiles.erase(matchingProfile);
            }
            // else the item has already been removed from the list
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::setAssociatedProfile" );
    }


    void OperatorHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new operator");

        m_isValidData = false;
        m_isValidProfileData = false;
        m_isValidRegionData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::invalidate" );
    }

	void OperatorHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PASSWD");
		columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

    void OperatorHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::reload" );
        // Ensure the operator has already been written to the database. It doesn't make
        // any sense to reload an operator that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*std::ostringstream sql;
		sql << "select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM OPERATOR_V WHERE PKEY = " << m_key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			OPERATOR_V_Oracle_SELECT_6554, m_key);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6554, m_key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6554, m_key);
//		TA_ASSERT(sql.size() > 0, "reload: Get SQL Statement error");
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql,columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
			reasonMessage << "No operator found for pkey = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of operator records."); 

		// loading profiles by operator key
		std::vector<unsigned long> profiles;
		IData* data2 = NULL;

        // Now load the associated profiles.
        if (!m_isValidProfileData && m_loadProfiles)
		{
			//sql.str("");
			//sql << " select SEPROF_ID from SE_OPERATOR_PROFILE where OPERATORKEY = " << m_key;
//			sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_SELECT_5002, m_key);
//			sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5002, m_key);

		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5002, m_key);
			
			// Set up the columnNames vector to be passed to executeQuery()
			columnNames.clear();
			columnNames.push_back("SEPROF_ID");

			data2 = databaseConnection->executeQuery(sql, columnNames);
			
			do
			{

				for (unsigned long i = 0; i < data2->getNumRows(); i++)
				{

					try
					{
						unsigned long profileId  = data2->getUnsignedLongData(i, columnNames[0]);
						m_associatedProfiles.push_back(profileId);
					}
					//data exception
					catch (TA_Base_Core::DataException&)
					{
						// Clean up the data pointer
						delete data2;
						data2 = NULL;
						throw;
					}
				}
				delete data2;
				data2 = NULL;
			}

			while ( databaseConnection->moreData(data2) );
		}

		
		// loading regions by operator key
		std::vector<unsigned long> regions;

		// Now load the assocaited regions
        if (!m_isValidRegionData && m_loadRegions)
		{
			/*sql.str("");
			sql << " select SEREGI_ID from SE_OPERATOR_REGIONS where OPERATORKEY = " << m_key;*/
//			sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_SELECT_5002, m_key);
//			sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5002, m_key);

		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5002, m_key);

			// Set up the columnNames vector to be passed to executeQuery()
			columnNames.clear();
			columnNames.push_back("SEREGI_ID");

			data2 = databaseConnection->executeQuery(sql, columnNames);
			do
			{

				for (unsigned long i = 0; i < data2->getNumRows(); i++)
				{

					try
					{
						unsigned long regionId  = data2->getUnsignedLongData(i, columnNames[0]);
						m_associatedRegions.push_back(regionId);
					}
					//catch exception
					catch (TA_Base_Core::DataException&)
					{
						// Clean up the data pointer
						delete data2;
						data2 = NULL;
						throw;
					}
				}
				delete data2;
				data2 = NULL;
			}

			while ( databaseConnection->moreData(data2) );
		}
		
        reloadUsing(0, *data, profiles, regions);
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::reload" );
    }


	void OperatorHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key  = data.getUnsignedLongData(row, columnNames[0] );
        m_name = data.getStringData(row, columnNames[1] );
		m_description = data.getStringData(row, columnNames[2] );
		m_password = data.getStringData( row, columnNames[3] );
        m_isSystemOperator = data.getBooleanData(row,  columnNames[4]);

        m_dateModified = data.getDateData(row, columnNames[5], 0);
        m_dateCreated = data.getDateData( row, columnNames[6]);
        
        // Now load the associated profiles.
        if (!m_isValidProfileData && m_loadProfiles)
        {
			m_associatedProfiles = profiles;
			m_isValidProfileData = true;
        }

		// Now load the assocaited regions
        if (!m_isValidRegionData && m_loadRegions)
        {
            m_associatedRegions = regions;
			m_isValidRegionData = true;
        }

        initialiseMaxFieldLengths();

        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void OperatorHelper::reloadProfiles()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::reloadProfiles" );

        // Ensure the operator has already been written to the database. It doesn't make
        // any sense to reload profiles for an operator that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database. "
                            "Therefore the associated profiles cannot be reloaded.");

		m_associatedProfiles = getProfileKeys();
        m_isValidProfileData = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::reloadProfiles" );
    }


    void OperatorHelper::reloadRegions()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::reloadRegions" );

        // Ensure the operator has already been written to the database. It doesn't make
        // any sense to reload regions for an operator that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database. "
                            "Therefore the associated regions cannot be reloaded.");

        std::vector<IRegion*> regions = RegionAccessFactory::getInstance().getRegionsByOperator( m_key);
		std::vector<IRegion*>::iterator regIt;

		m_associatedRegions.clear();
		m_associatedRegions.reserve(regions.size());

        for( regIt = regions.begin(); regIt != regions.end(); ++regIt )
        {
            m_associatedRegions.push_back( (*regIt)->getKey() );
			delete *regIt;
        }

        m_isValidRegionData = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::reloadRegions" );
    }


    void OperatorHelper::writeOperatorData()
    {       
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::writeOperatorData" );     

        // Check all values exist. If not add them to the dataConfiguraitonException
        std::vector<std::string> fieldNames;

        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }

        if (m_description.empty())
        {
            fieldNames.push_back("Description");
        }

        if ( m_password.empty() )
        {
            fieldNames.push_back("Password");
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Operator data not fully specified. Operator cannot be written to database",
                                                         fieldNames));
        }

        // TD11778: Check that the name doesn't contain any whitespace.
        if (m_name.find_first_of(" \t\r\n") != std::string::npos)
        {
            TA_THROW(TA_Base_Core::DataException("Operator name contains whitespace. Operator cannot be written to database",
                TA_Base_Core::DataException::INVALID_VALUE, "Name"));
        }

        // Write the Operator to the database

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // Need to check if this is a new operator or an existing operator - 
        // as a new operator will be inserted, while an existing operator will be updated
        if (m_isNew) // This is a new Operator
		{
            addNewOperator();
		}

        else // This is an existing Operator
		{
            modifyExistingOperator();
		}
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::writeOperatorData" );
    }


    void OperatorHelper::deleteOperator()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorHelper::deleteEntity" );
        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

		//libo 
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		//libo 
     
        // We must retrieve the new name and description for the operator when it is deleted
        std::string newOperatorName = retrieveDeletedField(m_name, "NAME", m_nameMaxLength);
        std::string newDescription = retrieveDeletedField(m_description, "DESCRIPTION", m_descriptionMaxLength);

        /*std::ostringstream sql;
        sql << "update OPERATOR set DELETED = 1, name = '" << databaseConnection->escapeInsertString(newOperatorName);
        sql << "' , description = '" << databaseConnection->escapeInsertString(newDescription) << "' where pkey = " << m_key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_STD_UPDATE_6151,
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_UPDATE_6151,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_UPDATE_6151,
			databaseConnection->escapeInsertString(newOperatorName), databaseConnection->escapeInsertString(newDescription), m_key);
//		TA_ASSERT(sql.size() > 0, "deleteOperator: Get SQL Statement error");

        databaseConnection->executeModification(sql);

        /*sql.str("");
        sql << "delete SE_OPERATOR_PROFILE where OPERATORKEY = " << m_key;*/
//		sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_DELETE_5401, m_key);
//		sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_DELETE_5401, m_key);

		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_DELETE_5401, m_key);

        databaseConnection->executeModification(sql);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::deleteOperator" );
    }


    std::vector<unsigned long> OperatorHelper::getProfileKeys()
	{
        // get a connection to the database

        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

       /* std::ostringstream sql;
        sql << "select SEPROF_ID from SE_OPERATOR_PROFILE where OPERATORKEY = " << m_key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_SELECT_5002, m_key);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_SELECT_5002, m_key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_SELECT_5002, m_key);
//		TA_ASSERT(sql.size() > 0, "getProfileKeys: Get SQL Statement error");

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string profileKeyColumn = "KEY";
        columnNames.push_back(profileKeyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        std::vector<unsigned long> profileKeys;

        // Now we step through the keys retrieved from the database.
        do
		{
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
		{
                profileKeys.push_back(data->getUnsignedLongData(i, profileKeyColumn));
		}
            
            delete data;
            data = NULL;
		} 
		
		while (databaseConnection->moreData(data) );

        return profileKeys;
	}


    void OperatorHelper::updateProfileAssociations()
    {
        FUNCTION_ENTRY("updateProfileAssociations");

        std::vector<unsigned long> profileKeys = getProfileKeys();
        std::vector<unsigned long> localProfilesToAdd = m_associatedProfiles;
        std::vector<unsigned long> databaseProfilesToRemove;

        // Now we step through the keys retrieved from the database.
        // If we find the key in both lists then the database is up-to-date and nothing needs to be done to the
        // lists (ie we dont' add it to the databaseProfilesToRemove list). At the end the keys left in the localProfilesToAdd
        // list need to be added to the database and the profiles left in databaseProfilesToRemove must be removed from the
        // database.
        for (unsigned int i = 0; i < profileKeys.size(); ++i)
        {

            std::vector<unsigned long>::iterator matching = std::find(localProfilesToAdd.begin(), localProfilesToAdd.end(), profileKeys[i]);
           
			if (matching != localProfilesToAdd.end())
            {
                // This key has been found so it has not changed
                // We remove it from the localProfilesToRemove list as we do not need to remove it from the database
                localProfilesToAdd.erase(matching);
            }
            else
            {
                // This key has obviously been removed from our local list so it will need to be removed from the database
                databaseProfilesToRemove.push_back(profileKeys[i]);
            }
        }

        // Now we add and delete from/to the database as required
       
        // get a connection to the database
       // IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

		//libo 
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		
		//libo 
        // First lets delete old values from the database
        if (!databaseProfilesToRemove.empty())
        {
            std::ostringstream sqlVar;
            //sql << "delete SE_OPERATOR_PROFILE where OPERATORKEY = " << m_key << " and SEPROF_ID in (";
           
			for (std::vector<unsigned long>::iterator iterToRemove = databaseProfilesToRemove.begin();
                 iterToRemove != databaseProfilesToRemove.end(); ++iterToRemove)
            {

                if ( iterToRemove != databaseProfilesToRemove.begin())
                {
                    // If this is not the first item then we must insert a comma
                    sqlVar << ",";
                }
                sqlVar << *iterToRemove;
            }    
      //      sql << ")";
//			std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_OPERATOR_PROFILE_STD_DELETE_5402, sqlVar.str());
//			std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_DELETE_5402, m_key, sqlVar.str());
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_DELETE_5402, m_key, sqlVar.str());

            databaseConnection->executeModification(sql);
        }

        // Now lets add new values to the database
        if (!localProfilesToAdd.empty())
        {
            for (std::vector<unsigned long>::iterator iterToAdd = localProfilesToAdd.begin();
                 iterToAdd != localProfilesToAdd.end(); ++iterToAdd)
            {
                 /*std::ostringstream sql;
                 sql << "insert into SE_OPERATOR_PROFILE(SEOPRO_ID, OPERATORKEY, SEPROF_ID) ";
                 sql << "values(SECURITY_SEQ.NEXTVAL, " << m_key << "," << *iterToAdd << ")";*/
//				std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 					SE_OPERATOR_PROFILE_Oracle_INSERT_5351, m_key, *iterToAdd);
//				std::string sql  = databaseConnection->prepareSQLStatement(SE_OPERATOR_PROFILE_INSERT_5351, m_key, *iterToAdd);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_OPERATOR_PROFILE_INSERT_5351,
            databaseConnection->getNextPkey("SECURITY_SEQ"), m_key, *iterToAdd);

                databaseConnection->executeModification(sql);
            }
        }
                 

        FUNCTION_EXIT;
    }


    void OperatorHelper::modifyExistingOperator()
    {
        FUNCTION_ENTRY("modifyExistingOperator");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Operator %s already exists. It's data will be updated.",m_name.c_str());
		
		std::string DBPassword;

        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

		
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select PKEY ,PASSWD from operator_V where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "' or DESCRIPTION = '"
            + databaseConnection->escapeQueryString(m_description) + "'";*/
//		std::string formatSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_STD_SELECT_6501,
//		std::string formatSQL  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6501,
		SQLStatement formatSQL;
		databaseConnection->prepareSQLStatement(formatSQL, OPERATOR_V_SELECT_6501,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_description));
//		TA_ASSERT(formatSQL.size() > 0, "modifyExistingOperator: Get SQL Statement error");

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

		std::string pswColumn = "PASSWD";
		columnNames.push_back(pswColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(formatSQL,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An operator already exists with name = " << m_name;
                reasonMessage << " or description = " << m_description;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description");
            }
			
			DBPassword = data->getStringData(0,pswColumn);
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }

        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one operator exists with name = " << m_name;
            reasonMessage << " or description = " << m_description;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description");
        }

        /*std::ostringstream sql;
        sql << "update OPERATOR set NAME = '";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "', DESCRIPTION = '";
        sql << (databaseConnection->escapeInsertString(m_description)).c_str();
        sql << "', PASSWD = '";
        sql << (databaseConnection->escapeInsertString(m_password)).c_str();
        sql << "' where PKEY = " << m_key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_STD_UPDATE_6651,
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_UPDATE_6651, getLocalDatabaseName(),
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_UPDATE_6651, getLocalDatabaseName(),
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_description),
			databaseConnection->escapeInsertString(m_password), m_key);
//		TA_ASSERT(sql.size() > 0, "modifyExistingOperator: Get SQL Statement error");

        databaseConnection->executeModification(sql);

		//update OperatorPassword

       /*  FZL1 does not use opeartor password table, so disable it
		if(!DBPassword.empty() && DBPassword.compare(m_password) != 0)
		{
			std::auto_ptr<TA_Base_Core::IOperatorPassword> operatorPassword(TA_Base_Core::OperatorPasswordAccessFactory::getInstance().getOperatorPassword(m_key,true,false));
			operatorPassword->changePassword(m_password);
			operatorPassword->applyChanges();
		}
       */
        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from OPERATOR_V where PKEY = " << m_key;*/
//		sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6555, m_key);
//		sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6555, m_key);

		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6555, m_key);
		
        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(sql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Now update the profiles in the database
        updateProfileAssociations();

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void OperatorHelper::addNewOperator()
    {
        FUNCTION_ENTRY("addNewOperator");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Operator %s is a new operator. It will be added to the database.",m_name.c_str());

        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		//libo
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
		//libo 

        // First check there is not alreay an operator with this name or 
        // description (it is a uniqueness constraint).

        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from OPERATOR where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";
        formatSQL << " or DESCRIPTION = '" << databaseConnection->escapeQueryString(m_description) + "'";*/
//		std::string formatSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_STD_SELECT_6001,
//		std::string formatSQL  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6001,
		SQLStatement formatSQL;
		databaseConnection->prepareSQLStatement(formatSQL, OPERATOR_SELECT_6001,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_description));
//		TA_ASSERT(formatSQL.size() > 0, "addNewOperator: Get SQL Statement error");

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(formatSQL,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name or description already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An operator already exists with name " << m_name << " or description " << m_description;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description"));
        }

        // delete the IData pointer now that it's finished with
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "insert into OPERATOR_V (PKEY, NAME, DESCRIPTION, PASSWD) ";
        sql << "values (OPERATOR_SEQ.NEXTVAL,'";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "','" << (databaseConnection->escapeInsertString(m_description)).c_str();
		sql << "','" << (databaseConnection->escapeInsertString(m_password)).c_str();
        sql << "')";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_INSERT_6851,
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_INSERT_6851,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_INSERT_6851,
            databaseConnection->getNextPkey("OPERATOR_SEQ"),
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_description), 
			databaseConnection->escapeInsertString(m_password));
//		TA_ASSERT(sql.size() > 0, "addNewOperator: Get SQL Statement error");

        databaseConnection->executeModification(sql);


        //now need to get the PKEY back out for this location and update the date fields
        /*sql.str("");
        sql << "select PKEY, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_V_Oracle_SELECT_6556,
//		sql  = databaseConnection->prepareSQLStatement(OPERATOR_V_SELECT_6556,

		databaseConnection->prepareSQLStatement(sql, OPERATOR_V_SELECT_6556,
			databaseConnection->escapeQueryString(m_name));

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "No data found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "More than one entry found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description"));
        }

        // Retrieve the pkey as an unsigned long - the name of this operator - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);

	/*
		
				std::ostringstream formatSQL;
						formatSQL << "insert into OPERATOR_PASSWORD (PKEY,LATEST_LOGIN_DATE,PASSWD_MODIFIED_DATE,LATEST_LOCKED_TIME,IS_PASSWORD_LOCKOUT,CURRENT_PASSWD_INDICATOR,PASSWORD_0,PASSWORD_1,PASSWORD_2) ";
						formatSQL << "values (" << m_key << ",NULL,SYSDATE,NULL,"<< 0 <<","<< 0 <<",'"<< (databaseConnection->escapeInsertString(m_password)).c_str() <<"',NULL,NULL)";
						// This will throw an exception upon failure - ready to catch??
						databaseConnection->executeModification(formatSQL.str());
			*/
			std::auto_ptr<TA_Base_Core::IOperatorPassword> operatorPassword(TA_Base_Core::OperatorPasswordAccessFactory::getInstance().CreateOperatorPassword(m_key));
			operatorPassword->setPassword(m_password);
			operatorPassword->applyChanges();
	
        }
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Now update the profiles in the database
        updateProfileAssociations();

        FUNCTION_EXIT;
    }


    std::string OperatorHelper::retrieveDeletedField(const std::string& currentField, const std::string& columnName, unsigned long maxFieldSize)
    {
        FUNCTION_ENTRY("retrieveDeletedField");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);


        std::string textToSearchFor = currentField + DELETED_FLAG;

        if(textToSearchFor.size() > maxFieldSize)
        {
            // Remove the character before the delete flag
            textToSearchFor.erase(textToSearchFor.size() - 3, 2);
        }

        /*std::ostringstream sql;		
        sql << "select " << databaseConnection->escapeQueryString(columnName) << " from OPERATOR where " 
            << databaseConnection->escapeQueryString(columnName) << " like '" 
            << databaseConnection->escapeQueryString(textToSearchFor) << "%'";
        sql << " and DELETED <> 0";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_STD_SELECT_6002,
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6002,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6002,
			databaseConnection->escapeQueryString(columnName), databaseConnection->escapeQueryString(columnName),
			databaseConnection->escapeQueryString(textToSearchFor));
//		TA_ASSERT(sql.size() > 0, "retrieveDeletedField: Get SQL Statement error");

        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        // If no rows are returned then we haven't got any deleted items by teh same name. This means
        // we can just append the first number.
        if (data->getNumRows() == 0)
        {
            std::string newField(currentField);

            // If the field has already reached its max length, remove the last character so it can still
            // be inserted in to the database.
            if(newField.size() + 1 > maxFieldSize)
			{
                newField.erase(newField.size() - 2, 2);
			}

            newField += DELETED_FLAG;
            newField += "1";

            delete data;
            data= NULL;

            FUNCTION_EXIT;
            return newField;
        }

        // If we get to here then we have other entities by the same name that have already been
        // deleted. We must determine the next number to append to the end. We must step through
        // each returned item and find the highest number.
        unsigned long highestAppendedNumber = 0;

        do
        {

            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {

                std::string field = data->getStringData(i,columnName);
                std::istringstream appendedNumber;

                // Retrieve all the text on the string after the text matching the nameToSearchFor
                appendedNumber.str( field.substr( textToSearchFor.size(), field.length() - textToSearchFor.length() ) );
                
                // Convert the string into an unsigned long
                unsigned long number;
                appendedNumber >> number;

                if (number > highestAppendedNumber)
                {
                    highestAppendedNumber = number;
                }
            }

            delete data;
            data = NULL;
        } 
		
		while ( databaseConnection->moreData(data) );

        // If the field has already reached its max length, remove the last few characters so it can still
        // be inserted in to the database.
        std::ostringstream numberString;
        numberString << highestAppendedNumber+1;
        if(textToSearchFor.size() + numberString.str().size() > maxFieldSize)
        {
            unsigned long trimLength = maxFieldSize - (textToSearchFor.size() + numberString.str().size());
			std::string::size_type deletedFlagPos = textToSearchFor.find(DELETED_FLAG);
            if(deletedFlagPos != std::string::npos)
            {
                textToSearchFor.erase(deletedFlagPos - trimLength, trimLength);
            }
        }

        std::ostringstream newField;
        newField << textToSearchFor << highestAppendedNumber+1;

        FUNCTION_EXIT;
        return newField.str();
    }


    void OperatorHelper::initialiseMaxFieldLengths()
    {
        FUNCTION_ENTRY("initialiseMaxFieldLengths");

        TA_Base_Core::ThreadGuard guard(m_nameMaxLengthLock);
        if (m_nameMaxLength > 0)
        {
            // we've already gotten the max length from last time, so don't need to fetch it again
            FUNCTION_EXIT;
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        /*std::ostringstream sql;
        sql << "select COLUMN_NAME, DATA_LENGTH from USER_TAB_COLS where TABLE_NAME = 'OPERATOR' ";
        sql << "and (COLUMN_NAME = 'NAME' or COLUMN_NAME = 'DESCRIPTION') ORDER BY 1";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), OPERATOR_Oracle_SELECT_6054);
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_SELECT_6054);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_SELECT_6054);
//		TA_ASSERT(sql.size() > 0, "initialiseMaxFieldLengths: Get SQL Statement error");

        std::string columnName = "COLUMN_NAME";
        std::string columnLength = "COLUMN_LENGTH";
        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);
        columnNames.push_back(columnLength);

        IData* data = databaseConnection->executeQuery(sql,columnNames);

        if (NULL == data || 0 == data->getNumRows())
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve column lengths. Setting to default values");

            m_descriptionMaxLength = DEFAULT_DESCRIPTION_MAX_LENGTH;
            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

			FUNCTION_EXIT;
			return;
        }

        else if (2 < data->getNumRows()) 
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving column lengths. Setting to default values");

            m_descriptionMaxLength = DEFAULT_DESCRIPTION_MAX_LENGTH;
            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

			FUNCTION_EXIT;
			return;
        }
        
        try
        {
            m_descriptionMaxLength = data->getUnsignedLongData(0,columnLength);
            m_nameMaxLength = data->getUnsignedLongData(1,columnLength);
        }
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            
            m_descriptionMaxLength = DEFAULT_DESCRIPTION_MAX_LENGTH;
            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


	std::string OperatorHelper::getLocalDatabaseName()
	{
		if (m_localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				throw DatabaseException("db-connection-file not set");
			}
			try
			{
				std::string strDbConnFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);

				/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				DataConnections connections = connectionStrings.getConnectionList(Alarm_Sd, Write);
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
