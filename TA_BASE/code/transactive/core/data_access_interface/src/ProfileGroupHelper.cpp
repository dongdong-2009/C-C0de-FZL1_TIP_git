/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileGroupHelper.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/06/12 10:49:32 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This is the base Profile Group class that supports all get and set operaion on profiles
  *
  * NOTE: This will also directly read the SE_PROFILE_GROUP or RA_ITSI_GROUP table to decide 
  * if the profile group requires a display. It will also access the SE_PROFILE to check whether
  * the profile or ITSI group is being used before allowing delete
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4284)
#endif

// ProfileGroupHelper.cpp: implementation of the ProfileGroupHelper class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <set>
#include <ctime>
#include <algorithm>

#include "core/data_access_interface/src/ProfileGroupHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ProfileGroupAccessFactory.h"
#include "core/data_access_interface/radio/src/ITSIGroupAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const unsigned long ProfileGroupHelper::DEFAULT_NAME_MAX_LENGTH = 80;
	const std::string ProfileGroupHelper::PROFILE = "Profile";
	const std::string ProfileGroupHelper::ITSI = "ITSI";

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    /**
     * Constructor
     * 
     * This constructor creates a new ProfileGroupHelper for the specified key.
     *
     * @param key The key into the SE_PROFILE_GROUP or RA_ITSI_GROUP table for this Gui
     */
    ProfileGroupHelper::ProfileGroupHelper(std::string type, unsigned long key)
		: m_isValidData( false ),
          m_isNew(false),
          m_profileKey(0),
		  m_ITSIkey(0),
          m_name( "" ),
          m_groupType(type),
          m_dateCreated( 0 ),
          m_dateModified( 0 )
	{
		m_isProfileGroup = false;
		m_isITSIGroup = false;
		
		if (type.compare(ProfileGroupHelper::PROFILE) == 0)
		{
			m_profileKey = key;
			m_isProfileGroup = true;
		}

		else if (type.compare(ProfileGroupHelper::ITSI) == 0)
		{
			m_ITSIkey = key;
			m_isITSIGroup = true;
		}

		initialiseMaxFieldLengths();
	}

    /**
     * Constructor
     * 
     * This constructor creates a new ProfileGroupHelper for configuration, initiailising
     * the key to 0 - it will be specified once this Profile Group is written to the database
     * for the first time.
     */
    ProfileGroupHelper::ProfileGroupHelper(std::string type /*=PROFILE_TYPE*/)
		: m_isValidData( false ),
          m_isNew(true),
          m_profileKey(0),
		  m_ITSIkey(0),
          m_name( "" ),
          m_groupType(type), // Default the type to Profile group type
          m_dateCreated( 0 ),
          m_dateModified( 0 )
	{
		m_isProfileGroup = false;
		m_isITSIGroup = false;
		
		if (type.compare(ProfileGroupHelper::PROFILE) == 0)
		{
			m_isProfileGroup = true;
		}

		else if (type.compare(ProfileGroupHelper::ITSI) == 0)
		{
			m_isITSIGroup = true;
		}

		initialiseMaxFieldLengths();
	}


    ProfileGroupHelper::ProfileGroupHelper(
			const std::string& type,
			const unsigned long row,
			TA_Base_Core::IData& data
		)
		:   
		m_isNew(false),
        m_isValidData(false),
		m_isProfileGroup(false),
		m_isITSIGroup(false),
		m_name( "" ),
		m_groupType(type)
    {
        FUNCTION_ENTRY("ProfileGroupHelper(const unsigned long, TA_Base_Core::IData&)");        

		if (type.compare(ProfileGroupHelper::PROFILE) == 0)
		{
			m_isProfileGroup = true;
		}

		else if (type.compare(ProfileGroupHelper::ITSI) == 0)
		{
			m_isITSIGroup = true;
		}

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }
	

    /**
     * Copy Constructor
     */
    ProfileGroupHelper::ProfileGroupHelper( const ProfileGroupHelper& theHelper)
        : m_isValidData(false),
          m_isNew(true),
		  m_profileKey(theHelper.m_profileKey),
		  m_ITSIkey(theHelper.m_ITSIkey),
		  m_name(theHelper.m_name),
          m_groupType(theHelper.m_groupType),
          m_dateCreated(0),
          m_dateModified(0),
		  m_nameMaxLength(theHelper.m_nameMaxLength),
		  m_isProfileGroup(theHelper.m_isProfileGroup),
		  m_isITSIGroup(theHelper.m_isITSIGroup)
	{
	}


    /**
     * Destructor
     */
	ProfileGroupHelper::~ProfileGroupHelper()
	{
	}

	void ProfileGroupHelper::initialiseMaxFieldLengths()
    {
        FUNCTION_ENTRY("initialiseMaxFieldLengths");
        m_nameMaxLength = 0;
		
		unsigned long maxProfileLength = getProfileGroupNameLength();
		unsigned long maxITSILength = getITSIGroupNameLength();

		// set to the smaller one in case the name is used for both tables
		if (maxProfileLength > maxITSILength)
		{ 
			m_nameMaxLength = maxITSILength;
		}

		else
		{
			m_nameMaxLength = maxProfileLength;
		}

        FUNCTION_EXIT;
    }

	unsigned long ProfileGroupHelper::getProfileGroupNameLength()
	{
		FUNCTION_ENTRY("getProfileGroupNameLength");
		
		unsigned long maxlength(DEFAULT_NAME_MAX_LENGTH);

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        /*std::ostringstream sql;
        sql << "select COLUMN_NAME, DATA_LENGTH from USER_TAB_COLS where TABLE_NAME = 'SE_PROFILE_GROUP' ";
        sql << "and COLUMN_NAME = 'NAME'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19553);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19553);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19553);

        std::string columnName = "COLUMN_NAME";
        std::string columnLength = "COLUMN_LENGTH";
        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);
        columnNames.push_back(columnLength);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No is retrieved from database
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve column lengths. Setting to default values");

            FUNCTION_EXIT;

            return maxlength;
        }

        else if (1 < data->getNumRows()) // Can't be more than 1 row. Data retrieved is not valid
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving column lengths. Setting to default values");

            FUNCTION_EXIT;

            return maxlength;
        }
        
        try // only 1 row is retrieve and set to the name char length
        {
            maxlength = data->getUnsignedLongData(0,columnLength);
		}
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            
            return maxlength;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

		return maxlength;
	}

	unsigned long ProfileGroupHelper::getITSIGroupNameLength()
	{
		FUNCTION_ENTRY("getITSIGroupNameLength");
		
		unsigned long maxlength(DEFAULT_NAME_MAX_LENGTH);

        // get a connection to the database
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        /*std::ostringstream sql;
        sql << "select COLUMN_NAME, DATA_LENGTH from USER_TAB_COLS where TABLE_NAME = 'RA_ITSI_GROUP' ";
        sql << "and COLUMN_NAME = 'NAME'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_Oracle_SELECT_29051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29051);

        std::string columnName = "COLUMN_NAME";
        std::string columnLength = "COLUMN_LENGTH";
        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);
        columnNames.push_back(columnLength);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No is retrieved from database
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve column lengths. Setting to default values");

            FUNCTION_EXIT;

            return maxlength;
        }

        else if (1 < data->getNumRows()) // Can't be more than 1 row. Data retrieved is not valid
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving column lengths. Setting to default values");

            FUNCTION_EXIT;

            return maxlength;
        }
        
        try // only 1 row is retrieve and set to the name char length
        {
            maxlength = data->getUnsignedLongData(0,columnLength);
		}
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            
            return maxlength;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

		return maxlength;
	}

    /**
     * getKey
     *
     * Returns the key for current initialised Profile group.
     *
     * @return The key for this Profile group as an unsigned long.
     *
     * pre: Either - this gui was initially loaded from the database
     *      OR     - writeGuiData() has already been called
     */
	unsigned long ProfileGroupHelper::getKey()
    {

		// This method cannot be called until the key has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		if (m_isProfileGroup)
		{
			return m_profileKey;
		}

		else if (m_isITSIGroup)
		{
			return m_ITSIkey;
		}

		return 0; // this is an unknown case
    }
	

	/**
     * getName
     *
     * Returns the name of this Profile Group. If this is the first time data for this profile group
     * has been requested, all the data is loaded from the database.
     *
     * @return The name for this profile group as a std::string
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be 
     *            converted to the required format (e.g. ACCESSLEVEL should be an 
     *            unsigned long), or if the wrong ammount of data is retrieved.
     *
     * pre: Either - this profile group was initially loaded from the database
     *      OR     - setName() has already been called
     */
    std::string ProfileGroupHelper::getName()
    {

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


    /**
     * setName
     *
     * Sets the name of this profile group. This should only be used by the ConfigProfileGroup class
     *
     * @param name The name to give this profile group.
     *
     * pre: deleteThisObject() has NOT been called
     */
	void ProfileGroupHelper::setName( const std::string& name )
	{
        FUNCTION_ENTRY( "setName" );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_name = name;
        
		FUNCTION_EXIT;
	}


	/**
     * getGroupType
     *
     * This method retrieves indicating the profile group type
     *
     * @return A string / name of the profile group type either PROFILE or ITSI currently
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be 
     *            converted to the required format (e.g. ACCESSLEVEL should be an 
     *            unsigned long), or if the wrong ammount of data is retrieved.
     */
	std::string ProfileGroupHelper::getGroupType()
	{

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_groupType;
	}

	/**
     * setGroupType
     *
     * Sets the type of this profile group which is either PROFILE or ITSI currently
     *
     * @param string - The type of profile group
     *
     */
    void ProfileGroupHelper::setGroupType(std::string type)
	{
        FUNCTION_ENTRY("setGroupType");

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_groupType = type;

        FUNCTION_EXIT;
	}

    /**
     * getDateCreated
     *
     * Returns the date created for this profile group
     *
     * @return The date created for this profile group as a time_t.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be 
     *            converted to the required format (e.g. TYPEKEY should be an 
     *            unsigned long), or if the wrong ammount of data is retrieved, or if the
     *            the entity key is invalid (and this is not a new entity).
     */
    time_t ProfileGroupHelper::getDateCreated()
	{

        // This method cannot be called until the name has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_dateCreated;
	}

    /**
     * getDateModified
     *
     * Returns the date modified for this profile group.
     *
     * @return The date modified for this profile group as a time_t.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be 
     *            converted to the required format (e.g. TYPEKEY should be an 
     *            unsigned long), or if the wrong ammount of data is retrieved, or if the
     *            the entity key is invalid (and this is not a new entity).
     */
    time_t ProfileGroupHelper::getDateModified()
    {

        // This method cannot be called until the name has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_dateModified;
    }

    /**
     * invalidate
     *
     * Make the data contained by this profile as invalid. The next call to get...() 
     * following a call to invalidate() will cause all the data to be reloaded from
     * the database.
     * It is a precondition that writeProfileGroupData() has been called prior to calling
     * this method on a new gui, as it does not make any sense to invalidate an
     * gui that has not yet been written to the database.
     *
     * pre: Either - this profile group was initially loaded from the database
     *      OR     - writeGuiData() has already been called
     */
    void ProfileGroupHelper::invalidate()
	{
		FUNCTION_ENTRY("invalidate()");

        // This method cannot be called until the key has been set
        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new Profile");

        m_isValidData = false;

        FUNCTION_EXIT;
	}


    /**
     * writeData
     * 
     * Write this profile group to the database. If this is a new profile group, this will populate
     * the m_key field (which can be retrieved using getKey()).
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a 
     *            problem writing the data to the database.
     * @exception DataConfigurationException If the data contained in the Profile group object
     *            is not sufficent to create an entry in the database, a 
     *            DataConfigurationException will be thrown. This is thrown as an exception
     *            and not tested as an assert as it can be directly linked to GUI components
     *            and may be the result of a user action that occurs prior to filling in 
     *            all sections.
     *
     */
    void ProfileGroupHelper::writeData()
	{
        FUNCTION_ENTRY("writeData");

        // Check all values exist. If not add them to the structure / data
        std::vector<std::string> fieldNames;

        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }

		if (m_isProfileGroup == false && m_isITSIGroup == false)
		{
			fieldNames.push_back("ProfileGroupCheck");
		}

		// There MUST be a name and either the profile or ITSI group is checked
        if ( m_name.empty() == true && (m_isProfileGroup == false && m_isITSIGroup == false) ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Profile data not fully specified. Profile cannot be written to database",
                                                         fieldNames));
        }

        // Write the Profile to the database

        // Need to check if this is a new profile or an existing profile - 
        // as a new profile will be inserted, while an existing profile will be updated
        if (m_isNew) // This is a new profile
        {
            if (m_isProfileGroup) addNewProfileToProfileGroup();
			else if (m_isITSIGroup) addNewProfileToITSIGroup();
        }

        else // This is an existing profile
        {
            if (m_isProfileGroup) modifyExistingProfileGroup();
			else if (m_isITSIGroup) modifyExistingITSIGroup();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
	}

    void ProfileGroupHelper::addNewProfileToProfileGroup()
    {
        FUNCTION_ENTRY("addNewProfileToProfileGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Profile group %s is a new profile group. It will be added to the database.",m_name.c_str());

		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // First check there is not alreay an operator with this name or 
        // description (it is a uniqueness constraint).

        /*std::ostringstream formatSQL;
        formatSQL << "select SEPGRO_ID from SE_PROFILE_GROUP where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_STD_SELECT_19502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19502,
			databaseConnection->escapeQueryString(m_name) );

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name or description already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A profile group already exists with name " << m_name;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // delete the IData pointer now that it's finised with
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "insert into SE_PROFILE_GROUP (SEPGRO_ID, NAME) ";
        sql << "values (SECURITY_SEQ.NEXTVAL,'";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "'" << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_INSERT_19851,
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_INSERT_19851,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_INSERT_19851,
            databaseConnection->getNextPkey("SECURITY_SEQ"),
			databaseConnection->escapeInsertString(m_name) );

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*sql.str("");
        sql << "select SEPGRO_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_GROUP where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19554,
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19554,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19554,
			databaseConnection->escapeQueryString(m_name) );

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

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
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // Retrieve the pkey as an unsigned long - the name of this operator - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_profileKey = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
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

        FUNCTION_EXIT;
    }

    void ProfileGroupHelper::addNewProfileToITSIGroup()
    {
        FUNCTION_ENTRY("addNewProfileToITSIGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Profile group %s is a new ITSI group. It will be added to the database.",m_name.c_str());

		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        // First check there is not alreay an operator with this name or 
        // description (it is a uniqueness constraint).

        /*std::ostringstream formatSQL;
        formatSQL << "select RAIGRO_ID from RA_ITSI_GROUP where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_STD_SELECT_29001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29001,
			databaseConnection->escapeQueryString(m_name) );

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name or description already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A ITSI group already exists with name " << m_name;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "insert into RA_ITSI_GROUP (RAIGRO_ID, NAME) ";
        sql << "values (SECURITY_SEQ.NEXTVAL,'";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "'" << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_Oracle_SELECT_29052,
//		strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_INSERT_29052,

		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_INSERT_29052,
            databaseConnection->getNextPkey("SECURITY_SEQ"),
			databaseConnection->escapeInsertString(m_name) );

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*sql.str("");
        sql << "select RAIGRO_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from RA_ITSI_GROUP where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_Oracle_SELECT_29053, 
//		strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29053, 

		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29053,
			databaseConnection->escapeQueryString(m_name) );

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

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
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // Retrieve the pkey as an unsigned long - the name of this operator - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_ITSIkey = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
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

		FUNCTION_EXIT;
	}

    /**
     * deleteGroup
     *
     * Remove this profile group from the database. Once this method has been executed, the
     * ProfileHelper object should be destroyed.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a 
     *            problem writing the data to the database.
     *
     * pre: Either - this profile group was initially loaded from the database
     *      OR     - writeProfileData() has already been called
     */
    void ProfileGroupHelper::deleteGroup()
	{
        FUNCTION_ENTRY("deleteGroup");

        TA_ASSERT(!m_isNew, "This profile group does not yet exist in the database, and therefore cannot be deleted");
        
		if (m_isProfileGroup) // this is present in the profile group
		{
			TA_ASSERT(!isGroupUsedInProfile(m_profileKey), "This profile group is used in the profile database, and therefore cannot be deleted");
			deleteProfileGroup();
		}

		else if (m_isITSIGroup)
		{
			TA_ASSERT(!isGroupUsedInProfile(m_ITSIkey), "This ITSI group is used in the profile database, and therefore cannot be deleted");
			deleteITSIGroup();
		}

        FUNCTION_EXIT;
	}

	void ProfileGroupHelper::deleteProfileGroup()
	{
        FUNCTION_ENTRY("deleteProfileGroup");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        /*std::ostringstream sql;
        sql << "delete from SE_PROFILE_GROUP where SEPGRO_ID = " << m_profileKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_STD_DELETE_19901, m_profileKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_DELETE_19901, m_profileKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_DELETE_19901, m_profileKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}

	void ProfileGroupHelper::deleteITSIGroup()
	{
        FUNCTION_ENTRY("deleteITSIGroup");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        /*std::ostringstream sql;
        sql << "delete from RA_ITSI_GROUP where RAIGRO_ID = " << m_ITSIkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_STD_DELETE_29401, m_ITSIkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_DELETE_29401, m_ITSIkey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_DELETE_29401, m_ITSIkey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
	}

    /**
     * isGroupIsUsedInProfile
     *
     * Checks whether this profile group is used in profile table from the database. 
     * The calling application MUST NOT delete this object, as it will affect the
	 * constraint / relationship in the profile table.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a 
     *            problem writing the data to the database.
     *
     * pre: Either - this profile was initially loaded from the database
     *      OR     - apply() has already been called
     *      This profile group is to be deleted
     */
  	bool ProfileGroupHelper::isGroupUsedInProfile(unsigned long key)
	{
		FUNCTION_ENTRY("isGroupIsUsedInProfile");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        /*std::ostringstream sql;
		sql << "select SEPROF_ID from SE_PROFILE where SEPGRO_ID = " << key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7504, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7504, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7504, key);
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::string profileIdColumn = "PROFILE_ID";
        std::vector<std::string> columnNames;
        columnNames.push_back(profileIdColumn);

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (1 <= data->getNumRows()) // More than one or more entry found for the pkey
        {
            delete data;
            data = NULL;

			FUNCTION_EXIT;
			return true;
		}

		else if (0 == data->getNumRows()) // Not in profile group id. May be in ITSI group id
		{
            delete data;
            data = NULL;

			//sql.str(""); // clear the string stream
			//sql << "select SEPROF_ID from SE_PROFILE where RAIGRO_ID = " << key;
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7505, key);
//			strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7505, key);

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7505, key);
			IData* data = databaseConnection->executeQuery(strSql,columnNames);

			if (1 <= data->getNumRows()) // More than one or more entry found for the pkey
			{
				delete data;
				data = NULL;

				FUNCTION_EXIT;
				return true;
			}

			else if (0 == data->getNumRows()) // Not in ITSI group id. It is confirmed not present
			{
				delete data;
				data = NULL;

				FUNCTION_EXIT;
				return false;
			}
		}

		FUNCTION_EXIT;
		return true;
	}


	void ProfileGroupHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("SEPGRO_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}	


    void ProfileGroupHelper::reload()
    {
        FUNCTION_ENTRY("reload");
        
		// Ensure the gui has already been written to the database. It doesn't make
        // any sense to reload an gui that doesn't exist in the database.
        TA_ASSERT( !m_isNew, "This profile does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        std::ostringstream sql;
		SQLStatement strSql;

		if (m_isProfileGroup)
		{
			/*sql << " select SEPGRO_ID, NAME,"
				<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
				<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
				<< " from SE_PROFILE_GROUP where SEPGRO_ID = " << m_profileKey;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19555, m_profileKey);
//			strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19555, m_profileKey);

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19555, m_profileKey);
		}

		else if (m_isITSIGroup)
		{
			/*sql << " select RAIGRO_ID, NAME,"
				<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
				<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
				<< " from RA_ITSI_GROUP where RAIGRO_ID = " << m_ITSIkey;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_Oracle_SELECT_29054, m_ITSIkey);
//			strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29054, m_ITSIkey);

		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29054, m_ITSIkey);
		}

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql,columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;

			if (m_isProfileGroup)
			{
				reasonMessage << "No profile found for pkey = " << m_profileKey;
			}

			else if (m_isITSIGroup)
			{
				reasonMessage << "No profile found for pkey = " << m_ITSIkey;
			}
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }


		TA_ASSERT(data->getNumRows() == 1, "Invalid number of profile group records."); 
        reloadUsing(0, *data);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::reload" );
    }


	void ProfileGroupHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		if ( m_isProfileGroup )
		{
			m_profileKey = data.getUnsignedLongData( row, columnNames[0] );
		}

		else if ( m_ITSIkey )
		{
			m_ITSIkey = data.getUnsignedLongData( row, columnNames[0] );
		}

        m_name = data.getStringData( row, columnNames[1] );
        m_dateModified = data.getDateData(row, columnNames[2], 0);
        m_dateCreated = data.getDateData( row, columnNames[3] );

		initialiseMaxFieldLengths();

        m_isValidData = true;
        FUNCTION_EXIT;
    }
	

    void ProfileGroupHelper::modifyExistingProfileGroup()
    {
        FUNCTION_ENTRY("modifyExistingProfileGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Profile Group %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select SEPGRO_ID from SE_PROFILE_GROUP where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_STD_SELECT_19502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19502,
			databaseConnection->escapeQueryString(m_name) );

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "PKEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_profileKey != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A profile group already exists with name = " << m_name;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }

        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one profile group exists with name = " << m_name;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
        }

       /* std::ostringstream sql;
        sql << "update SE_PROFILE_GROUP set NAME = '";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "' where SEPGRO_ID = " << m_profileKey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_STD_UPDATE_19651,
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_UPDATE_19651,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_UPDATE_19651,
			databaseConnection->escapeInsertString(m_name), m_profileKey);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from SE_PROFILE_GROUP where SEPGRO_ID = " << m_profileKey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19556, m_profileKey);
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19556, m_profileKey);

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19556, m_profileKey);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
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

        FUNCTION_EXIT;
    }

	void ProfileGroupHelper::modifyExistingITSIGroup()
	{
        FUNCTION_ENTRY("modifyExistingITSIGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "ITSI Group %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select RAIGRO_ID from RA_ITSI_GROUP where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_STD_SELECT_29001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29001,
			databaseConnection->escapeQueryString(m_name) );

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "PKEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_ITSIkey != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A ITSI group already exists with name = " << m_name;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one ITSI group exists with name = " << m_name;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
        }

        /*std::ostringstream sql;
        sql << "update RA_ITSI_GROUP set NAME = '";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "' where RAIGRO_ID = " << m_ITSIkey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_STD_UPDATE_29151,
//		strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_UPDATE_29151,

		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_UPDATE_29151,
			databaseConnection->escapeInsertString(m_name), m_ITSIkey);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from RA_ITSI_GROUP where RAIGRO_ID = " << m_ITSIkey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_ITSI_GROUP_Oracle_SELECT_29055, m_ITSIkey);
//		strSql  = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29055, m_ITSIkey);

		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29055, m_ITSIkey);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
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

        FUNCTION_EXIT;
	}
}
