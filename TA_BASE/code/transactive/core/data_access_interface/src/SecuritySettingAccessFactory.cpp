
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/SecuritySettingAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
	SecuritySettingAccessFactory* SecuritySettingAccessFactory::m_instance = 0;

	SecuritySettingAccessFactory::SecuritySettingAccessFactory()
		:m_AUTOMATIC_LOGOUT_TIME(INT_MAX),
		m_FAILED_LOGIN_ATTEMPTS(INT_MAX),
		m_PASSWORD_REUSE_MAX(INT_MAX),
		m_PASSWORD_WARN_TIME(INT_MAX),
		m_PASSWORD_REUSE_TIME(INT_MAX),
		m_PASSWORD_LIFE_TIME(INT_MAX),
		m_PASSWORD_LOCK_TIME(INT_MAX),
		m_hasLoad(false)
	{

	}

	SecuritySettingAccessFactory& SecuritySettingAccessFactory::getInstance()
	{

		if( m_instance == 0 )
		{
			m_instance = new SecuritySettingAccessFactory();
		}
		return *m_instance;
	}

	void SecuritySettingAccessFactory::reload()
	 {
		
		 IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);
		 //std::ostringstream sql;
		 std::vector<std::string> columnNames;
		columnNames.push_back("SETTING_NAME");
		columnNames.push_back("SETTING_VALUE");

		 //sql<< "select SETTING_NAME,SETTING_VALUE from SE_SECURITY_SETTING";
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_SECURITY_SETTING_SELECT_78001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_SECURITY_SETTING_SELECT_78001);
//		TA_ASSERT(strSql.size() > 0, "SecuritySettingAccessFactory::reload: Prepare SQL Statement error");

		 IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		 if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
		 {
			 delete data;
			 data = NULL;

			 return ;
		 }

		 // Loop for each row returned
		 do
		 {

			 for (unsigned long i = 0; i < data->getNumRows(); i++)
			 {

				 // The getUnsignedLongData() call can throw an exception. Need to catch
				 // it to do pointer clean up.
				 try
				 {
					 // Create the subsystem
					 if((data->getStringData(i,columnNames[0])).compare("AUTOMATIC_LOGOUT_TIME") == 0)
						m_AUTOMATIC_LOGOUT_TIME  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("FAILED_LOGIN_ATTEMPTS") == 0)
						 m_FAILED_LOGIN_ATTEMPTS  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("PASSWORD_REUSE_MAX") == 0)
						 m_PASSWORD_REUSE_MAX  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("PASSWORD_WARN_TIME") == 0)
						 m_PASSWORD_WARN_TIME  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("PASSWORD_REUSE_TIME") == 0)
						 m_PASSWORD_REUSE_TIME  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("PASSWORD_LIFE_TIME") == 0)
						 m_PASSWORD_LIFE_TIME  = data->getIntegerData(i,columnNames[1]);
					 else if ((data->getStringData(i,columnNames[0])).compare("PASSWORD_LOCK_TIME") == 0)
						 m_PASSWORD_LOCK_TIME  = data->getIntegerData(i,columnNames[1]);

				 }

				//data exception
				 catch (const TA_Base_Core::DataException&)
				 {
					 // Clean up the data pointer
					 delete data;
					 data = NULL;
					 throw;
				 }
			 }
			 delete data;
			 data = NULL;
		 }

		 while ( databaseConnection->moreData(data) );
	
		 m_hasLoad = true;
	 }

	int SecuritySettingAccessFactory::getAutomaticLogoutTime()
	{

		if(!m_hasLoad && m_AUTOMATIC_LOGOUT_TIME ==INT_MAX)
		{
			reload();
		}
		return m_AUTOMATIC_LOGOUT_TIME;
	}

	int SecuritySettingAccessFactory::getFailedLoginAttemtps()
	{

		if(!m_hasLoad && m_FAILED_LOGIN_ATTEMPTS == INT_MAX)
		{
			reload();
		}
		return m_FAILED_LOGIN_ATTEMPTS;
	}

	int SecuritySettingAccessFactory::getPasswordReuseMax()
	{
		if(!m_hasLoad && m_PASSWORD_REUSE_MAX == INT_MAX)
		{
			reload();
		}
		return m_PASSWORD_REUSE_MAX;
	}

	int SecuritySettingAccessFactory::getPasswordWarnTime()
	{

		if(!m_hasLoad && m_PASSWORD_WARN_TIME == INT_MAX)
		{
			reload();
		}
		return m_PASSWORD_WARN_TIME;
	}

	int SecuritySettingAccessFactory::getPasswordReuseTime()
	{
		if(!m_hasLoad &&m_PASSWORD_REUSE_TIME == INT_MAX)
		{
			reload();
		}
		return m_PASSWORD_REUSE_TIME;
	}

	int SecuritySettingAccessFactory::getPasswordLifeTime()
	{

		if(!m_hasLoad && m_PASSWORD_LIFE_TIME == INT_MAX)
		{
			reload();
		}
		return m_PASSWORD_LIFE_TIME;
	}

	int SecuritySettingAccessFactory::getPasswordLockTime()
	{

		if(!m_hasLoad && m_PASSWORD_LOCK_TIME ==INT_MAX)
		{
			reload();
		}
		return m_PASSWORD_LOCK_TIME;
	}

	} // closes TA_Base_Core