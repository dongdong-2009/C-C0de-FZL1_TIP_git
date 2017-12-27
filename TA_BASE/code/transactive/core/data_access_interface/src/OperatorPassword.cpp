/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorPassword.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 17:38:47 $
  * Last modified by:  $Author: hui.wang $
  * 
  * Session is an implementation of ISession. It holds the data specific to an Session entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <ctime>
#include <string>
#include "core/data_access_interface/src/OperatorPassword.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SecuritySettingAccessFactory.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"
using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
	OperatorPassword::OperatorPassword(unsigned long operatorkey,bool isNew,bool changePassword,bool rightPassword)
		:m_operatorKey(operatorkey),
		m_latestLoginTime(NULL),
		 m_passwdModifiedDate(NULL),
		m_latestLockedTime(NULL),
		m_isPasswordLockout(INT_MAX),
		m_currentPasswdIndicator(INT_MAX),
		m_password0(""),
		m_password1(""),
		m_password2(""),
		m_isWritten(!isNew),
		m_isValidData(false),
		m_rightPassword(!isNew &&!changePassword&&rightPassword),
		m_isUpdateOnly(isNew),
		m_password(""),
		m_changePassword(!isNew &&changePassword),
		m_isNew(isNew)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"OperatorPassword::OperatorPassword");
	}

	OperatorPassword::~OperatorPassword(void)
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

	std::string OperatorPassword::getLocalDatabaseName()
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
				DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				DataConnections connections = connectionStrings.getConnectionList(Sessions_Ad, Write);
				m_localDatabase = connections[0].first;  
			}
			//exception
			catch(...) // most likely a file not found error
			{
				throw DatabaseException("Unable to find database connection infomation");
			}

		}
		return m_localDatabase;
	}

	std::string OperatorPassword::getPassword0() 
	{

		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten)
		{
			TA_ASSERT( true, "The Operatorkey must be set before it can be retrieved" );
		}

		if ( m_isWritten && ! m_isValidData && !m_isUpdateOnly)
		{
			reload();
		}

		return m_password0;
	}
	std::string OperatorPassword::getPassword1() 
	{

		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten)
		{
			TA_ASSERT( true, "The Operatorkey must be set before it can be retrieved" );
		}

		if ( m_isWritten && ! m_isValidData && !m_isUpdateOnly)
		{
			reload();
		}

		return m_password1;
	}
	std::string OperatorPassword::getPassword2() 
	{
		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten)
		{
			TA_ASSERT( true, "The Operatorkey must be set before it can be retrieved" );
		}

		if ( m_isWritten && ! m_isValidData)
		{
			reload();
		}

		return m_password2;
	}
	int OperatorPassword::getCurrentPasswordIndicator() 
	{
		// If the data isn't written & the operatorkey isn't set then we must reload

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly)
		{
			reload();
		}

		return m_currentPasswdIndicator;
	}
	int OperatorPassword::getLockedTimes() 
	{
		// If the data isn't written & the operatorkey isn't set then we must reload

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly)
		{
			reload();
		}

		return m_isPasswordLockout;
	}

	void OperatorPassword::reload()
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);

		// create the SQL string to retrieve the data of the operator
		// based upon the key
		//char sql[512];
		//sprintf(sql,"select TO_CHAR(LATEST_LOGIN_DATE,'YYYYMMDDHH24MISS'),TO_CHAR(PASSWD_MODIFIED_DATE,'YYYYMMDDHH24MISS'),TO_CHAR(LATEST_LOCKED_TIME,'YYYYMMDDHH24MISS'),IS_PASSWORD_LOCKOUT,CURRENT_PASSWD_INDICATOR,PASSWORD_0,PASSWORD_1,PASSWORD_2 from OPERATOR_PASSWORD where PKEY = %ld",
		//	m_operatorKey);

		 
//		std::string sql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_SELECT_74001,m_operatorKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, OPERATOR_PASSWORD_SELECT_74001,m_operatorKey);
//		TA_ASSERT(sql.size() > 0, "OperatorPassword::reload(): Prepare SQL Statement error");

		// Set up the columnNames vector to be passed to executeQuery()
		std::string latestLoginDateColumn = "LATEST_LOGIN_DATE";
		std::string passwdModifiedDateColumn  = "PASSWD_MODIFIED_DATE";
		std::string LatestLockedTimeColumn  = "LATEST_LOCKED_TIME";
		std::string IsPasswordLockoutColumn = "IS_PASSWORD_LOCKOUT";
		std::string currentPasswdIndicatorColumn   = "CURRENT_PASSWD_INDICATOR";
		std::string password0Column = "PASSWORD_0";
		std::string password1Column = "PASSWORD_1";
		std::string password2Column = "PASSWORD_2";

		std::vector<std::string> columnNames;
		columnNames.push_back(latestLoginDateColumn);
		columnNames.push_back(passwdModifiedDateColumn );
		columnNames.push_back(LatestLockedTimeColumn );
		columnNames.push_back(IsPasswordLockoutColumn);
		columnNames.push_back(currentPasswdIndicatorColumn  );
		columnNames.push_back(password0Column);
		columnNames.push_back(password1Column);
		columnNames.push_back(password2Column);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql,columnNames);
	
		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
		{
			delete data;
			data = NULL;
			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No operatorpassword found for pkey = %d", m_operatorKey );	
			
			TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
		}
		else if (1 < data->getNumRows()) // More than one entry found for the pkey
		{
			delete data;
			data = NULL;
			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one operatorpassword found with pkey = %d", m_operatorKey );
		
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
		}

		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		m_latestLoginTime = data->getDateData(0 ,latestLoginDateColumn,0 );
		m_passwdModifiedDate = data->getDateData(0 , passwdModifiedDateColumn);
		m_latestLockedTime = data->getDateData(0 ,LatestLockedTimeColumn ,0);
		m_isPasswordLockout = data->getIntegerData(0 ,IsPasswordLockoutColumn );
		m_currentPasswdIndicator = data->getIntegerData(0 , currentPasswdIndicatorColumn);
		m_password0 = data->getStringData(0 ,password0Column );
		m_password1 = data->getStringData(0 , password1Column);
		m_password2 = data->getStringData(0 , password2Column);

		// Now that we're done with the IData object, it's our responsibility to delete it
		delete data;
		data = NULL;

		// Need to record that we now have valid data
		m_isValidData = true;
		m_isWritten = true;
		m_isUpdateOnly = false;
		LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
	}

	void OperatorPassword::applyChanges()
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "OperatorPassword::applyChanges" );     
		//TA_ASSERT( m_operatorKey == 0, "OperatorPassword not set, can not apply" );

		// NOTE: Sessions can only be created or destroyed - nothing else. 
		// There is no need to handle updating records, only creating new records... UNLESS
		// were are updating the HAS_ENDED column ...

		// Write the session to the database

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Write);

/*
		time_t time_now = time(NULL);
		struct tm* now = localtime(&time_now);
		std::ostringstream timestring;
		timestring<<"TO_DATE('"<<now->tm_year+1900<<"-"<<now->tm_mon+1<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<"', 'YYYY-MM-DD HH24:MI:SS')";
*/


		if( m_isNew)
		{
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"Writing new OPERAOTR_PASSWORD (id: %d) to the database.",
				m_operatorKey);

			
// 			std::ostringstream formatSQL;
// 			formatSQL << "insert into OPERATOR_PASSWORD (PKEY,LATEST_LOGIN_DATE,PASSWD_MODIFIED_DATE,LATEST_LOCKED_TIME,IS_PASSWORD_LOCKOUT,CURRENT_PASSWD_INDICATOR,PASSWORD_0,PASSWORD_1,PASSWORD_2) ";
// 			formatSQL << "values (" << m_operatorKey << ",NULL,SYSDATE,NULL,"<< 0 <<","<< 0 <<",'"<< (databaseConnection->escapeInsertString(m_password)).c_str() <<"',NULL,NULL)";
		SQLStatement strSql;

//			strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_INSERT_74002, m_operatorKey ,0,0,			 

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_INSERT_74002, m_operatorKey ,0,0,
				databaseConnection->escapeInsertString( m_password ));

//			TA_ASSERT(strSql.size() > 0, "OperatorPassword::applyChanges(): Prepare SQL Statement error");
		 
			// This will throw an exception upon failure - ready to catch??
			databaseConnection->executeModification(strSql);

			// Set the data to be valid - no need to force a reload after a succesful write
			m_isValidData = true;
			m_isWritten = true;
			m_isUpdateOnly = false;
			m_changePassword = false;
		}
		else
		{
			if(m_changePassword)
			{
				int passwordReuseTime = SecuritySettingAccessFactory::getInstance().getPasswordReuseTime();
				reload();
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-11] changePassword %d",m_currentPasswdIndicator);
// 				std::ostringstream sql;
// 				sql << "update OPERATOR_PASSWORD set PASSWD_MODIFIED_DATE = SYSDATE, CURRENT_PASSWD_INDICATOR = "<<(++m_currentPasswdIndicator)%passwordReuseTime ;
		SQLStatement strSql;
				
				if(m_currentPasswdIndicator == 0)
					//sql<<",PASSWORD_1  = '"<<(databaseConnection->escapeInsertString(m_password)).c_str();
//					strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_PWD1_74004, getLocalDatabaseName(),(++m_currentPasswdIndicator)%passwordReuseTime , 			 

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_PWD1_74004, getLocalDatabaseName(),(++m_currentPasswdIndicator)%passwordReuseTime ,
					databaseConnection->escapeInsertString( m_password ), m_operatorKey);
				else if(m_currentPasswdIndicator == 1)
					//sql<<", PASSWORD_2 = '"<<(databaseConnection->escapeInsertString(m_password)).c_str();
//					strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_PWD2_74005, getLocalDatabaseName(),(++m_currentPasswdIndicator)%passwordReuseTime , 			 

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_PWD2_74005, getLocalDatabaseName(),(++m_currentPasswdIndicator)%passwordReuseTime ,
					databaseConnection->escapeInsertString( m_password ),m_operatorKey);
				else 
					//sql<<", PASSWORD_0 = '"<<(databaseConnection->escapeInsertString(m_password)).c_str();
//					strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_PWD0_74003, getLocalDatabaseName(), (++m_currentPasswdIndicator)%passwordReuseTime , 			 

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_PWD0_74003, getLocalDatabaseName(), (++m_currentPasswdIndicator)%passwordReuseTime ,
					databaseConnection->escapeInsertString( m_password ),m_operatorKey);
				
				//sql<< "' where pkey = " << m_operatorKey;
				
			
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-11] changePassword done %d",m_currentPasswdIndicator);
				databaseConnection->executeModification(strSql);
			}

			else
			{
				if(m_rightPassword)
				{
					//std::ostringstream sql;
					//sql << "update OPERATOR_PASSWORD set LATEST_LOGIN_DATE = SYSDATE,LATEST_LOCKED_TIME = NULL,IS_PASSWORD_LOCKOUT = 0 where pkey = " << m_operatorKey;
		SQLStatement strSql;
//					strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_LOGTIM_74006,  m_operatorKey); 			

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_LOGTIM_74006,  getLocalDatabaseName(), m_operatorKey);
					databaseConnection->executeModification(strSql);
				}
				else 
				{
					int password_reuse_max = TA_Base_Core::SecuritySettingAccessFactory::getInstance().getFailedLoginAttemtps();
					//std::ostringstream sql;
					//sql << "update OPERATOR_PASSWORD set IS_PASSWORD_LOCKOUT = ";
		SQLStatement strSql;
					reload();
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-11] OperatorPassword.cpp=> the m_islockout is %d",m_isPasswordLockout);
					if(m_isPasswordLockout < (password_reuse_max-1))
						//sql<<++m_isPasswordLockout<<", LATEST_LOCKED_TIME = NULL ";
//						strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_LOCKTIM_74007, (++m_isPasswordLockout), m_operatorKey); 		

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_LOCKTIM_74007,  getLocalDatabaseName(), (++m_isPasswordLockout), m_operatorKey);
					else if(m_isPasswordLockout == (password_reuse_max-1))
						//sql<<++m_isPasswordLockout<<", LATEST_LOCKED_TIME = SYSDATE ";
//						strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_LOCKTIM_74008, (++m_isPasswordLockout), m_operatorKey); 		

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_LOCKTIM_74008, getLocalDatabaseName(), (++m_isPasswordLockout), m_operatorKey);
					else
//						strSql  = databaseConnection->prepareSQLStatement(OPERATOR_PASSWORD_UPDATE_LOCKTIM_74009, (password_reuse_max) ,m_operatorKey); 		

		databaseConnection->prepareSQLStatement(strSql, OPERATOR_PASSWORD_UPDATE_LOCKTIM_74009, getLocalDatabaseName(), (password_reuse_max) ,m_operatorKey);
						//sql<<password_reuse_max<<", LATEST_LOCKED_TIME = SYSDATE ";

					//sql<<" where pkey = " << m_operatorKey;
					
					databaseConnection->executeModification(strSql);
				}


			}
			

		}
	}

	void OperatorPassword::changePassword(const std::string& password)
	{
		m_changePassword = true;
		m_isUpdateOnly = true;
		m_password = password;
	}

	void OperatorPassword::setPassword( const std::string& password )
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"OperatorPassword::setPassword");
		m_password = password;
		m_isUpdateOnly = true;
		m_isNew = true;
	}

	time_t OperatorPassword::getLatestLoginDate()
	{	
		// If the data isn't written & the operatorkey isn't set then we must reload

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly)
		{
			reload();
		}

		return m_latestLoginTime;
	}

	time_t OperatorPassword::getLatestModifiedDate()
	{	
		// If the data isn't written & the operatorkey isn't set then we must reload

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly)
		{
			reload();
		}

		return  m_passwdModifiedDate;
		
	}

	time_t OperatorPassword::getLatestLockedTime()
	{	
		// If the data isn't written & the operatorkey isn't set then we must reload

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly)
		{
			reload();
		}

		return m_latestLockedTime;
	}
}
