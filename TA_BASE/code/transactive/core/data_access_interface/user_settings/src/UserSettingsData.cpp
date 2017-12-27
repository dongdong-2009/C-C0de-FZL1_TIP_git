/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/user_settings/src/UserSettingsData.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * <description>
  *
  */
#pragma warning(disable:4786)

#include "core\data_access_interface\user_settings\src\UserSettingsData.h"
#include "core\data_access_interface\src\IDatabase.h"
#include "core\data_access_interface\src\IData.h"
#include "core\data_access_interface\src\DatabaseFactory.h"
#include "core\exceptions\src\DataUserSettingException.h"
#include "core\utilities\src\DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SQLCode.h"

using TA_Base_Core::DataUserSettingException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string UserSettingsData::NAME_COLUMN = "Name";
    const std::string UserSettingsData::VALUE_COLUMN = "Value";

    UserSettingsData::UserSettingsData(std::string sessionId, unsigned long appType)
        : m_sessionId(sessionId), m_applicationType(appType), m_isValidData(false)
    {

    }


    UserSettingsData::~UserSettingsData()
    {

    }


    std::string UserSettingsData::getSettingAsString(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingAsString");
        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter == m_settings.end() )
        {
            std::string error ("Requested setting could not be found: ");
            error += name;
            TA_THROW( DataUserSettingException(error.c_str()) );
        }            

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingAsString");
        return iter->second;
    }


    unsigned long UserSettingsData::getSettingAsUnsignedLong(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingAsUnsignedLong");

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter == m_settings.end() )
        {
            std::string error ("Requested setting could not be found: ");
            error += name;
            TA_THROW( DataUserSettingException(error.c_str()) );
        }            

        unsigned long value;
        try
        {
            value = stringToUnsignedLong(iter->second);
        }
        catch ( DataUserSettingException& )
        {
            std::string error ("Requested setting could not be converted into an unsigned long: ");
            error += name;
            TA_THROW( DataUserSettingException(error.c_str()) );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingAsUnsignedLong");
        return value;
    }


    void UserSettingsData::setSetting(const std::string& name,const std::string& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setSetting");

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter == m_settings.end() )
        {
            std::string error ("Requested setting could not be found: ");
            error += name;
                TA_THROW( DataUserSettingException(error.c_str(), DataUserSettingException::SETTING_NOT_FOUND) );
        }            

        updateSettingInDatabase(name,value);

        iter->second = value;

        LOG( SourceInfo, DebugUtil::FunctionExit, "setSetting");
    }


    void UserSettingsData::setSetting(const std::string& name,const unsigned long value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setSetting");

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter == m_settings.end() )
        {
            std::string error ("Requested setting could not be found: ");
            error += name;
                TA_THROW( DataUserSettingException(error.c_str(), DataUserSettingException::SETTING_NOT_FOUND) );
        }            

        updateSettingInDatabase(name,unsignedLongToString(value) );

        iter->second = unsignedLongToString(value);

        LOG( SourceInfo, DebugUtil::FunctionExit, "setSetting");
    }

    void UserSettingsData::addSetting(const std::string& name, const std::string& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "addSetting");

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter != m_settings.end() )
        {
            std::string error ("Requested setting already exists: ");
            error += name;
            TA_THROW( DataUserSettingException(error.c_str()) );
        }            

        insertSettingIntoDatabase(name,value);

        m_settings.insert(UserSettingsData::Settings::value_type(name, value));

        LOG( SourceInfo, DebugUtil::FunctionExit, "addSetting");
    }

    void UserSettingsData::addSetting(const std::string& name, const unsigned long value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "addSetting");

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        UserSettingsData::Settings::iterator iter = m_settings.find(name);
        if ( iter != m_settings.end() )
        {
            std::string error ("Requested setting already exists: ");
            error += name;
            TA_THROW( DataUserSettingException(error.c_str()) );
        }            

        std::string strValue = unsignedLongToString(value);
        insertSettingIntoDatabase(name,strValue);

        m_settings.insert(UserSettingsData::Settings::value_type(name, strValue));

        LOG( SourceInfo, DebugUtil::FunctionExit, "addSetting");
    }

    void UserSettingsData::reload()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "reload");

       /* std::string sql("select U.SETTING_NAME, U.SETTING_VALUE from");
        sql += " USER_SETTINGS U, TA_SESSION T where U.OP_KEY = T.OPERATORKEY and T.PKEY = '";
        sql += m_sessionId;
        sql += "' and U.AP_TYPE_KEY = ";
        sql += unsignedLongToString(m_applicationType);
        sql += " order by U.SETTING_NAME";	*/	

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(UserSettingsData::NAME_COLUMN);
        columnNames.push_back(UserSettingsData::VALUE_COLUMN);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), USER_SETTINGS_STD_SELECT_40501,
//		std::string strSql  = databaseConnection->prepareSQLStatement(USER_SETTINGS_SELECT_40501,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, USER_SETTINGS_SELECT_40501,
			m_sessionId, unsignedLongToString(m_applicationType));
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        do // loop for each lot of IData that has been retrieved
        {
            for (unsigned long row = 0; row < data->getNumRows(); ++row)
            {
                // Retrieve the setting name
                std::string name = data->getStringData(row,UserSettingsData::NAME_COLUMN);

                // Check that setting does not already exist in the map. If it does we
                // have an error so throw an exception.
                UserSettingsData::Settings::iterator iter = m_settings.find(name);
                if ( iter != m_settings.end() )
                {
                    // This setting is a duplicate. This is an error so throw an exception
                    std::string error ("A duplicate setting was found in the database: ");
                    error += name;
                    TA_THROW( DataUserSettingException(error.c_str()) );
                }

                // Retrieve the setting value
                std::string value = data->getStringData(row,UserSettingsData::VALUE_COLUMN);
        
                // Add the setting to the map.
                m_settings.insert(UserSettingsData::Settings::value_type(name, value));
                LOGMORE( SourceInfo, "Name: %s Setting: %s",name.c_str(), value.c_str() );
            }

            // Delete ready to retrieve the next lot of data
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
        }while ( databaseConnection->moreData(data) );

        // Log the user settings if the debug level is less than or equal to INFO.
        if( DebugUtil::DebugInfo <= DebugUtil::getInstance().getLevel() )
        {
            UserSettingsData::Settings::iterator iter (m_settings.begin());
            UserSettingsData::Settings::iterator iterEnd (m_settings.end());

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Loaded Settings are:" );
            while (iter != iterEnd)
            {
                LOGMORE( SourceInfo, "Name: %s Setting: %s",iter->first.c_str(), iter->second.c_str() );
	            ++iter;
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "reload");
    }


    void UserSettingsData::updateSettingInDatabase(const std::string& name, const std::string& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "updateSettingInDatabase");
        
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Write);
       /* std::string sql("update USER_SETTINGS set SETTING_VALUE = '");
        sql += databaseConnection->escapeInsertString(value);
        sql += "' where OP_KEY = (select OPERATORKEY from TA_SESSION where PKEY = '";
        sql += m_sessionId;
        sql += "') and AP_TYPE_KEY = ";
        sql += unsignedLongToString(m_applicationType);
        sql += " AND SETTING_NAME = '";
        sql += databaseConnection->escapeQueryString(name);
        sql += "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), USER_SETTINGS_STD_UPDATE_40651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(USER_SETTINGS_UPDATE_40651,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, USER_SETTINGS_UPDATE_40651,
			databaseConnection->escapeInsertString(value), m_sessionId, unsignedLongToString(m_applicationType), 
			databaseConnection->escapeQueryString(name));

        // Execute the modification. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(strSql);

        LOG( SourceInfo, DebugUtil::FunctionExit, "updateSettingInDatabase");
    }


    void UserSettingsData::insertSettingIntoDatabase(const std::string& name, const std::string& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "insertSettingIntoDatabase");

        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Write);
      /*  std::string sql("insert into USER_SETTINGS(OP_KEY,AP_TYPE_KEY,SETTING_NAME,SETTING_VALUE) ");
        sql += "values(";
        sql += " (select OPERATORKEY from TA_SESSION where PKEY = '";
        sql += m_sessionId;
        sql += "'),";
        sql += unsignedLongToString(m_applicationType);
        sql += ",'";
        sql += databaseConnection->escapeInsertString(name);
        sql += "','";
        sql += databaseConnection->escapeInsertString(value);
        sql += "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), USER_SETTINGS_STD_INSERT_40801, m_sessionId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(USER_SETTINGS_INSERT_40801, m_sessionId,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, USER_SETTINGS_INSERT_40801, m_sessionId,
			unsignedLongToString(m_applicationType), databaseConnection->escapeInsertString(name), databaseConnection->escapeInsertString(value));

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(strSql);

        LOG( SourceInfo, DebugUtil::FunctionExit, "insertSettingIntoDatabase");
    }


    std::string UserSettingsData::unsignedLongToString(unsigned long x)
    {
        //The maximum size of an unsigned long is 4294967295
        //which is 10 digits so
        char buffer[12] = {0}; //just to be safe
        sprintf(buffer,"%u",x);
        return std::string(buffer);
    }

    unsigned long UserSettingsData::stringToUnsignedLong(const std::string& str)
    {
        if ( str.empty() )
        {
            TA_THROW( DataUserSettingException("String could not be converted") );
        }

		// The following checks need to be performed on the string: 
		// 1) Check there are no characters apart from 0-9
		// NB: leading spaces will cause the string to fail, and an exception will be generated
		std::string::size_type validCharacters = str.find_first_not_of("0123456789");

		// The first test ensures there are no invalid characters
		// The second test ensures the returned number is not null
		if (std::string::npos != validCharacters)
		{
            TA_THROW( DataUserSettingException("String could not be converted") );
		}

		// The behaviour of atol is undefined in the event of overflow, therefore we need to test that
		// the string is not too long. The maximum permitted range for an integer is 
		// from 0 to 4294967295.
		// We can use a string lexicographical compare to determine if the retrieved integer is 
		// useable.
		
		static char tempBuffer[256] = {0};
		sprintf(tempBuffer,"%lu",ULONG_MAX);
		static const std::string maxul(tempBuffer);


		if (str.size() >= maxul.size()) // this could be a problem
		{
			int numberComparison = 0;
			if (str.size() == maxul.size())
			{
				numberComparison = str.compare(maxul);	// will return a negative number if
														        	// target < maxInt
			}
			else
			{
				numberComparison = 1;	// set this to 1, as the number is too big, and doesn't need
										// to be compared
			}

			// final comparison
			if (numberComparison > 0)
			{
                TA_THROW( DataUserSettingException("String could not be converted") );
			}
		}

		// atol will return 0 both when the string is 0, and when there's an error.
		// However, as we have already tested above, and thrown an exception if there 
		// are any non-numeric characters in the string we can assume this will succeed.
        return atol(str.c_str());		
    }


    void UserSettingsData::deleteSetting(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "deleteSetting");

        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Write);
       /* std::string sql("delete from USER_SETTINGS where OP_KEY = (select OPERATORKEY from TA_SESSION where PKEY = '");
        sql += m_sessionId;
        sql += "') and AP_TYPE_KEY = ";
        sql += unsignedLongToString(m_applicationType);
        sql += " AND SETTING_NAME = '";
        sql += databaseConnection->escapeQueryString(name);
        sql += "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), USER_SETTINGS_STD_DELETE_40901, m_sessionId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(USER_SETTINGS_DELETE_40901, m_sessionId,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, USER_SETTINGS_DELETE_40901, m_sessionId,
			unsignedLongToString(m_applicationType), databaseConnection->escapeQueryString(name));

        // Execute the modification. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(strSql);

        LOG( SourceInfo, DebugUtil::FunctionExit, "deleteSetting");
    }


    std::vector<std::string> UserSettingsData::getSettingsLike(const std::string& prefix)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getSettingsOfNameLike");

        std::vector<std::string> retval;

        if (!m_isValidData)
        {
            reload();
            m_isValidData = true;
        }

        for (UserSettingsData::Settings::iterator iter = m_settings.begin();
             iter != m_settings.end();
             iter++)
        {
            std::string name = iter->first;
            if (name.find(prefix) == 0)
            {
                retval.push_back(iter->second);
            }
        }            

        LOG( SourceInfo, DebugUtil::FunctionExit, "getSettingsOfNameLike");
        return retval;
    }

	std::string UserSettingsData::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Application_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Application_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

} //end namespace TA_Base_Core
