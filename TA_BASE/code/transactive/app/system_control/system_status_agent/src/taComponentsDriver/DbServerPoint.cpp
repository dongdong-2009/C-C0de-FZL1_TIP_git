/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/DbServerPoint.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * DbServerPoint class implimentation. See header file for class descrption
  */
#include "core/otl/src/otlconf.h"

#include "driver/mysql_public_iface.h"  // oyzl++ for mysql connection
#include "app/system_control/system_status_agent/src/taComponentsDriver/DbServerPoint.h"
#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"

#include "core/database/src/SimpleDb.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/database/src/DBException.h"

#include <sstream>
#include <time.h>
#include <string>

#ifdef WIN32
  #ifndef lint
    #define lint
  #endif
#endif

#ifdef USING_OTL
#include "core/otl/src/otlv4.h"
#else
#include "core/ocl/src/ocl.h"
using namespace ocl;
#endif

using namespace TA_Base_Core;

namespace TA_Base_App
{

#define  defOracleDBName          "Oracle"
#define  defMysqlDBName           "Mysql"
#define  defDBSrvDelim            "@"

#define  defOracleDBType           1
#define  defMysqlDBType            2

	std::vector<std::string> DbServerPoint::m_dbStatusNames;

	DbServerPoint::DbServerPoint
	( 
		TA_Base_Bus::BooleanDataPoint* const datapoint,
		const std::string& hostName
	)
		: m_datapoint(datapoint),
		  m_strServerName(hostName),
		  m_pOraConnection(0),
		  m_pOraQuery(0),
		  m_pMysqlDriver(0),
		  m_pMysqlConntion(0),
		  m_pMysqlQuery(0),
		  m_dbTimeOutSec(10),
		  m_isUpdating(false),
          m_reConnect(true)
    {
        // dbStatus:<database>:<base database>
        m_strBaseServerName = m_strServerName;
        size_t pos = m_strServerName.find( ":" );

        if ( pos != std::string::npos )
        {
            std::string serverName = m_strServerName.substr( 0, pos );
            std::string baseServerName = m_strServerName.substr( pos + 1 );

            if ( false == serverName.empty() && false == baseServerName.empty() )
            {
                m_strServerName = serverName;
                m_strBaseServerName = baseServerName;
            }
        }
    }
#ifdef USING_OTL
	DbServerPoint::~DbServerPoint()
	{
		FUNCTION_ENTRY("~DbServerPoint");
		if (NULL != m_pOraQuery)
		{
			delete m_pOraQuery;	
			m_pOraQuery = NULL;
		}
		if (NULL != m_pMysqlQuery)
		{
			delete m_pMysqlQuery;	
			m_pMysqlQuery = NULL;
		}

		if (m_pOraConnection != 0 && 1 == m_pOraConnection->connected)
		{
			m_pOraConnection->logoff();
			delete m_pOraConnection;
			m_pOraConnection = NULL;
		}
		if (m_pMysqlConntion != 0 && !m_pMysqlConntion->isClosed())
		{
			m_pMysqlConntion->close();
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
		}
		//detachDb();
		FUNCTION_EXIT;
	}

	void DbServerPoint::poll()
	{
		FUNCTION_ENTRY("poll");

		try
		{
			if ( m_reConnect )
			{
				if (!_Connect())
				{// if can not connect, continuely trying to connect DB in next loop.
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"DbServerPoint::Poll() connect failed for database server %s. Database set to UNAVAILABLE", m_strServerName.c_str());
					m_reConnect = true;// 
					return;
				}
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
				"DbServerPoint::Poll() Executing dummy select for database %s.", m_strServerName.c_str());
			//create a query to test DB server usability.
			switch ( m_nDBType )
			{
			case defOracleDBType:
				_DoOraQuery();
				break;
			case defMysqlDBType:
				_DoMysqlQuery();
				break;
			default:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::Connect() the database type do not support. the connection string: %s", m_strServerName.c_str());
				break;
			}

		}
		catch(otl_exception &e )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_strServerName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.code, (char*)(e.msg));
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
			{
				m_reConnect =  true;
				if ( m_pOraQuery && m_pOraQuery->good())
				{
					m_pOraQuery->close();
				}
				if (m_pOraConnection != 0 && 1 == m_pOraConnection->connected)
				{
					m_pOraConnection->logoff();
				}
			}	
		}
		catch(sql::SQLException &e )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_strServerName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.getErrorCode(), e.what());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
			{
				m_reConnect = true;
				if ( m_pMysqlQuery && !m_pMysqlQuery->isClosed() )
					m_pMysqlQuery->close();
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Caught exception while trying to connect to databse server %s.",m_strServerName.c_str());
			return ;
		}
	}

	bool DbServerPoint::_Connect()
	{
		FUNCTION_ENTRY("_Connect");
		if (m_reConnect)
		{
			if ( m_strUserName.empty() || m_strPassword.empty() )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "database <%s> has invalid schemaName or password <%s>:<%s>, no need to poll.", 
					m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
				FUNCTION_EXIT
					return false;
			}

			try
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"DbServerPoint::_Connect() create a new connection to database server %s...", m_strServerName.c_str());
				switch ( m_nDBType )
				{
				case defOracleDBType:
					_OraConnection();
					break;
				case defMysqlDBType:
					_MysqlConnect();
					break;
				default:
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
						"DbServerPoint::_Connect() the database type do not support. m_databaseName: %s",
						m_strServerName.c_str());
					break;
				}
			}
			catch(otl_exception & e)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s schema %s password %s. \nOCI_ERROR:[%s] ",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), (char*)(e.msg));
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}
			catch(std::exception & e)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s: %s",m_strServerName.c_str(), e.what());
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s. Returning false",m_strServerName.c_str());
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}

			m_reConnect = false;
		}

		FUNCTION_EXIT
			return true;
	}
	void DbServerPoint::_DetachDb()
	{
		FUNCTION_ENTRY("_DetachDb");
		// When delete m_dataBase, OraConnection destructor will be called.
		// Normally, destructor should not throw any exception, or it will cause
		// the destructor incomplete. But actually when we disconnect db by cutting
		// off the lan cable, seems the destructor of OraConnection will throw
		// exception, that's terrible, I'm not sure this would cause some weird 
		// problem, even if adding a try-catch block here, because once an exception
		// is thrown by destructor that means the destructor is not finished.
		try
		{
			if(m_pOraConnection && 1 == m_pOraConnection->connected)
				m_pOraConnection->logoff();

			delete m_pMysqlConntion;
			m_pMysqlConntion = 0;
		}
		catch(otl_exception & e)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s. OCI_ERROR:[%s] ",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), (char*)(e.msg));
		}
		catch(std::exception & e)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s: %s",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), e.what());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s.",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str() );
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"delete db of datapoint %d.",getDataPoint()->getEntityKey());
		FUNCTION_EXIT;
	}
	void DbServerPoint::_Init()
	{
		int nThreadedMode = 1;

		switch ( m_nDBType )
		{
		case defOracleDBType:
			// initialize OCI environment
			if (1 != otl_connect::otl_initialize(nThreadedMode))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "otl_connect::otl_initialize error!");
			}
			m_pOraConnection = new otl_connect();
			TA_ASSERT(NULL != m_pOraConnection, "Init():connection pointer is null.");
			//if connect to oracle db servr error! will throw exception, but not catch code here, logon later
			//m_pOraConnection->rlogon(m_strOTLConnection.c_str(), 0); // connect to Oracle
			m_pOraQuery = new otl_stream();
			break;
		case defMysqlDBType:
			m_pMysqlDriver = (sql::Driver*)get_driver_instance();
			break;
		}
	}

	void DbServerPoint::_OraConnection()
	{
		TA_ASSERT(NULL != m_pOraConnection, "connection pointer is null");
		if (1 == m_pOraConnection->connected)		
			return;		
		else		
			m_pOraConnection->rlogon(m_strOTLConnection.c_str(), 0); // connect to Oracle

	}
	void DbServerPoint::_DoOraQuery()
	{
		FUNCTION_ENTRY("DoOraQuery");

		TA_ASSERT(m_pOraConnection != NULL, "m_connection should not be NULL");
		std::string strSQL = "select * from V$version";

		m_pOraQuery->open(100, strSQL.c_str(), *m_pOraConnection);

		if (1 == m_pOraQuery->good() && 1 != m_pOraQuery->eof())
		{
			m_pOraQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"DbServerPoint::_DoOraQuery() Database poll successfully for database server %s. Database status AVAILABLE",m_strServerName.c_str());           
			FUNCTION_EXIT;
			return;
		}
		else
		{
			m_pOraQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"DbServerPoint::_DoOraQuery() Database returned empty result for database server  %s. Database status UNAVAILABLE",m_strServerName.c_str());
			m_reConnect = true;
		}

		FUNCTION_EXIT;
	}

#else
	//OCL
	DbServerPoint::~DbServerPoint()
	{
		FUNCTION_ENTRY("~DbServerPoint");
		if (NULL != m_pOraQuery)
		{
			delete m_pOraQuery;	
			m_pOraQuery = NULL;
	}
		if (NULL != m_pMysqlQuery)
	{
			delete m_pMysqlQuery;	
			m_pMysqlQuery = NULL;
		}

		if (m_pOraConnection != 0 && m_pOraConnection->isConnected())
		{
			m_pOraConnection->close();
			delete m_pOraConnection;
			m_pOraConnection = NULL;
		}
		if (m_pMysqlConntion != 0 && !m_pMysqlConntion->isClosed())
		{
			m_pMysqlConntion->close();
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
		}
        //detachDb();
		FUNCTION_EXIT;
	}
	void DbServerPoint::poll()
	{
		FUNCTION_ENTRY("poll");
        
		try
		{
			if ( m_reConnect )
		{
				if (!_Connect())
				{// if can not connect, continuely trying to connect DB in next loop.
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"DbServerPoint::Poll() connect failed for database server %s. Database set to UNAVAILABLE", m_strServerName.c_str());
					m_reConnect = true;// 
					return;
		}
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
				"DbServerPoint::Poll() Executing dummy select for database %s.", m_strServerName.c_str());
			//create a query to test DB server usability.
			switch ( m_nDBType )
		{
			case defOracleDBType:
				_DoOraQuery();
				break;
			case defMysqlDBType:
				_DoMysqlQuery();
				break;
			default:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::Connect() the database type do not support. the connection string: %s", m_strServerName.c_str());
				break;
			}

		}
		catch(ocl::OraException &e )
    {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_strServerName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.errorCode(), e.message().c_str());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
        {
				m_reConnect =  true;
				if ( m_pOraQuery && m_pOraQuery->isActive() )
				m_pOraQuery->close();
				m_pOraConnection = m_pOraQuery->connection();
			}	
		}
		catch(sql::SQLException &e )
            {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_strServerName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ; will re-connect database", e.getErrorCode(), e.what());
			//if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
            {
                m_reConnect = true;
				if ( m_pMysqlQuery && !m_pMysqlQuery->isClosed() )
				m_pMysqlQuery->close();
            }
        }
		catch (CRException& )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Query failed for database server %s. Database status UNAVAILABLE",m_strServerName.c_str());
            m_reConnect = true;
			if ( m_pOraQuery && m_pOraQuery->isActive() )
			m_pOraQuery->close();
			m_pOraConnection = m_pOraQuery->connection();
        }
		catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::Poll() Caught exception while trying to connect to databse server %s.",m_strServerName.c_str());
			return ;
        }
    }
	bool DbServerPoint::_Connect()
	{
		FUNCTION_ENTRY("_Connect");
		if (m_reConnect)
		{
			if ( m_strUserName.empty() || m_strPassword.empty() )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "database <%s> has invalid schemaName or password <%s>:<%s>, no need to poll.", 
					m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
				FUNCTION_EXIT
					return false;
			}

			try
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"DbServerPoint::_Connect() create a new connection to database server %s...", m_strServerName.c_str());
				switch ( m_nDBType )
				{
				case defOracleDBType:
					_OraConnection();
					break;
				case defMysqlDBType:
					_MysqlConnect();
					break;
				default:
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
						"DbServerPoint::_Connect() the database type do not support. m_databaseName: %s",
						m_strServerName.c_str());
					break;
				}
			}
			catch(ocl::OraException & e)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s schema %s password %s. \nOCI_ERROR:[%s] ",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), e.message().c_str());
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}
			catch(std::exception & e)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s: %s",m_strServerName.c_str(), e.what());
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"DbServerPoint::_Connect() Caught exception while trying to connect to databse server %s. Returning false",m_strServerName.c_str());
				_DetachDb();
				m_reConnect = true;
				FUNCTION_EXIT
					return false;
			}

			m_reConnect = false;
		}

		FUNCTION_EXIT
			return true;
	}
	void DbServerPoint::_DetachDb()
	{
		FUNCTION_ENTRY("_DetachDb");
		// When delete m_dataBase, OraConnection destructor will be called.
		// Normally, destructor should not throw any exception, or it will cause
		// the destructor incomplete. But actually when we disconnect db by cutting
		// off the lan cable, seems the destructor of OraConnection will throw
		// exception, that's terrible, I'm not sure this would cause some weird 
		// problem, even if adding a try-catch block here, because once an exception
		// is thrown by destructor that means the destructor is not finished.
		try
		{
			if(m_pOraConnection && m_pOraConnection->isConnected())
				m_pOraConnection->close();

			delete m_pMysqlConntion;
			m_pMysqlConntion = 0;
		}
		catch(ocl::OraException & e)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s. OCI_ERROR:[%s] ",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), e.message().c_str());
		}
		catch(std::exception & e)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s: %s",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), e.what());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"delete fail server %s schema %s password %s.",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str() );
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"delete db of datapoint %d.",getDataPoint()->getEntityKey());
		FUNCTION_EXIT;
	}

	void DbServerPoint::_Init()
	{
		switch ( m_nDBType )
		{
		case defOracleDBType:
			m_pOraQuery    =  new OraQuery;
			OraEnvironment::setInitMode(OraEnvironment::imObject);
			OraEnvironment::setInitMode(OraEnvironment::imThreaded);
			OraEnvironment::setThreaded(true);
			m_pOraConnection = new ocl::OraConnection();

			TA_ASSERT(NULL != m_pOraConnection, "Init():connection pointer is null.");
			m_pOraConnection->setConnectMode(cmNormal);
			m_pOraConnection->setUsername(m_strUserName); 
			m_pOraConnection->setPassword(m_strPassword); 
			m_pOraConnection->setServer(m_strServerName);
			break;
		case defMysqlDBType:
			m_pMysqlDriver = (sql::Driver*)get_driver_instance();
			break;
		}
	}

	void DbServerPoint::_OraConnection()
	{
		TA_ASSERT(NULL != m_pOraConnection, "connection pointer is null");
		if (m_pOraConnection->isConnected())		
			return;		
		else		
			m_pOraConnection->open();	

	}

	void DbServerPoint::_DoOraQuery()
	{
		FUNCTION_ENTRY("DoOraQuery");

		TA_ASSERT(m_pOraConnection != NULL, "m_connection should not be NULL");
		try
		{			
			m_pOraQuery->setConnection(*m_pOraConnection);
			m_pOraQuery->setCommandType(ocl::ctSQL);
			m_pOraQuery->setCached(false);  
			m_pOraQuery->setFetchRows(100);
			m_pOraQuery->setSQL("select * from V$version");
			m_pOraQuery->open();   
			m_pOraConnection = m_pOraQuery->connection();
			if (m_pOraQuery->isRowsReturn())
			{
				m_pOraQuery->close();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"DbServerPoint::_DoOraQuery() Database poll successfully for database server %s. Database status AVAILABLE",m_strServerName.c_str());           
				FUNCTION_EXIT;
				return;
			}
			else
			{
				m_pOraQuery->close();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
					"DbServerPoint::_DoOraQuery() Database returned empty result for database server  %s. Database status UNAVAILABLE",m_strServerName.c_str());
				m_reConnect = true;
			}
		}
		catch(ocl::OraException &e )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::_DoOraQuery() Query failed for database server %s. Database status UNAVAILABLE",
				m_strServerName.c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"Database Error code %d. Reason: %s ;  will re-connect database", e.errorCode(), e.message().c_str());
			m_reConnect = true;
		}
		catch (CRException& )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::_DoOraQuery() Query failed for database server %s. Database status UNAVAILABLE",
				m_strServerName.c_str());
			m_reConnect = true;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"DbServerPoint::_DoOraQuery() Caught exception while trying to connect to databse server %s.",
				m_strServerName.c_str());
			m_reConnect = true;
		}

		FUNCTION_EXIT;
	}


#endif
	

	ISelfResolvingPointContainer* DbServerPoint::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{

		FUNCTION_ENTRY("convertToIPointContainer");

		//see if this is a BooleanDataPoint:
		TA_Base_Bus::BooleanDataPoint* pBDP = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(dataPoint);

		if(0 == pBDP)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"convertToIPointContainer() refused non-BooleanDataPoint with pkey %u", dataPoint->getEntityKey());

			FUNCTION_EXIT;

			return 0;
		}

		//see if the address has the form "dbStatus:<host name>"

		std::string hostName = dataPoint->getDataPointAddress();
		std::transform(hostName.begin(), hostName.end(), hostName.begin(), ::toupper);

		//The true parameter in this call disables validation of the IP address portion of the string.
		//This is a good thing given that we're after the form dbStatus:<host name>
		if(!AbstractThreadedDriver::convertAddressFieldToIpAddress(hostName, "dbStatus:", true) )
		{
			FUNCTION_EXIT;

			return 0; //note that call already did logging so don't do it here
		}

		std::vector<std::string>::iterator findIt = std::find( m_dbStatusNames.begin(), m_dbStatusNames.end(), hostName ); 

		if (findIt != m_dbStatusNames.end())
		{
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "duplicated dbStatus configureation: %s, dbStatus Count=%d", 
				hostName.c_str(), m_dbStatusNames.size() );
			return 0;
		}
		else
		{
			m_dbStatusNames.push_back(hostName);
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "add dbStatus datapoint: %s, dbStatus Count=%d", 
				hostName.c_str(), m_dbStatusNames.size() );
		}

		DbServerPoint* newPoint = new DbServerPoint(pBDP, hostName);


		FUNCTION_EXIT;

		return newPoint;
	}

	TA_Base_Bus::DataPoint* const DbServerPoint::getDataPoint()
	{
		FUNCTION_ENTRY("getDataPoint");

		FUNCTION_EXIT;
		return m_datapoint;
	}

	std::string DbServerPoint::getAddressIdentifier()
	{
		FUNCTION_ENTRY("getAddressIdentifier");
		FUNCTION_EXIT;
		return m_strServerName;
	}

	/** 
	* getConnectionParameters
	*
	* retrieves the database schema name and password from the connection
	* string supplied by the framework
	*/
	void DbServerPoint::getConnectionParameters( std::string& username, std::string& password )
	{
		FUNCTION_ENTRY("getConnectionParameters");

		std::string filename = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);

		// can throw DbConnectionNoFile
		// no point in catching it cause there is nothing we can do with it
		TA_Base_Core::DbConnectionStrings connectStrings(filename);
		TA_Base_Core::DatabaseConnectionMap dbMap = connectStrings.getConnectionMap();


		TA_Base_Core::DatabaseConnectionMap::iterator it = dbMap.begin();
		while (it != dbMap.end() )
		{
			std::vector<TA_Base_Core::DataConnection>::iterator dcIt = it->second.begin();
			while (dcIt != it->second.end())
			{
				std::string strDbServer(dcIt->first);
				std::transform(strDbServer.begin(), strDbServer.end(), strDbServer.begin(), ::toupper);
				if ( strDbServer == m_strBaseServerName )
				{
					std::string  strConnection = dcIt->second;	

					std::vector<std::string> dbConnData;
					SimpleDb::ParseConnectStr(strConnection, dbConnData, ':');

					if (dbConnData.size() < 5) //The connection string is not valid
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
							"The database connection string=[%s] is not valid.",
							strConnection.c_str());
						FUNCTION_EXIT;
					}		

					_ParseDBType( dbConnData[0] );    // for supporting multiple DB server (Oracle & Mysql)
					m_strHostName  = dbConnData[4]; 
					m_strShema = dbConnData[1];
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
						"DbServerPoint::getConnectionParameters for serverName %s, userName %s,password %s, hostName: %s,",
						m_strServerName.c_str(),m_strUserName.c_str(), m_strPassword.c_str(), m_strHostName.c_str() );					

					username = dbConnData[2];
					password = dbConnData[3];

					//[User Name]/[Password]@[TNS Alias]
					//"IT271350_5/IT271350_5@TRANSACT"
					m_strOTLConnection = m_strUserName + "/" + m_strPassword + "@" + m_strShema;

					return;
				}

				dcIt++;
			}

			it++;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"cant get schame and password for database %s.", m_strServerName.c_str());

		FUNCTION_EXIT;

	}


	void DbServerPoint::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("onInit");

		//call this base class method so that the related ping point is resolved
		// AbstractPingDependentPoint::onInit(pEntityData); //limin-- CL-21689


		m_entityData = pEntityData;

		//Set the timeout
		m_dbTimeOutSec = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);

		//set the params as required
		getConnectionParameters( m_strUserName, m_strPassword  );

		//removed this so that time is not taken to resolve the obj during init
		//resolve();
		_Init();

		FUNCTION_EXIT;
	}

	bool DbServerPoint::updatePointBasedOnProcessStatus()
        {
		FUNCTION_ENTRY("updatePointBasedOnProcessStatus");

		//We will set the point false if we can not resolve the db Server
		bool couldPoll = false;		
		
		try 			
		{
			//This call throws if the db Server does not communicate
			if (resolve())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Db Server resolved on %s", m_strHostName.c_str());
				poll();
				//if it polled OK set the value to true
				couldPoll = true;
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Db Server poll() was OK on %s", m_strHostName.c_str());
			}	
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "resolve database fail on host %s. Datapoint pkey = %u. Will set state to 0 (false).", m_strServerName.c_str(), m_datapoint->getEntityKey());
                couldPoll = false;
                m_reConnect = true;
            }			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"threw when trying to retrieve status on host %s. Datapoint pkey = %u. Will set state to 0 (false).", m_strServerName.c_str(), m_datapoint->getEntityKey());
			couldPoll = false;
            m_reConnect = true;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"finish resolve and poll of  datapoint (pkey = %u).", getDataPoint()->getEntityKey());

		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		//TD18334 diiDBSVR-DBStopRun DP value 0: database down; 1: database running;
		dpValue.setBoolean( couldPoll );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"Set datapoint (pkey = %u) state (%i, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey(), (int)couldPoll);


		FUNCTION_EXIT;

		return couldPoll;

	}

	void DbServerPoint::updateDataPoint()
	{
    	FUNCTION_ENTRY("updateDataPoint");
		m_isUpdating = true;
		try
		{
			unsigned long key = getDataPoint()->getEntityKey();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updating datapoint (pkey = %u).", key);

			updatePointBasedOnProcessStatus();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"updated datapoint (pkey = %u).", key);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Threw when trying to update datapoint (pkey = %u).", getDataPoint()->getEntityKey());
			//If here datapoint write threw (not the process resolution) , so forget about it

		}
		m_isUpdating = false;
		FUNCTION_EXIT;
	}

	bool DbServerPoint::isResolved()
	{
		//If not null it is resolved
		return ( m_pOraConnection != 0 || m_pMysqlConntion != 0 );
	}

	bool DbServerPoint::isUpdating()
	{
		FUNCTION_ENTRY("isUpdating");
		FUNCTION_EXIT;
		return m_isUpdating;
	}
	/**
	* setStateForNoProcessResolution
	*
	* update the state of the data point to an appropriate state given that you can not read the status of the
	* related process.
	*
	* @throw if you can not write to the datapoint
	* 
	*/
	void DbServerPoint::setStateForNoProcessResolution()
	{
		FUNCTION_ENTRY("setStateForNoProcessResolution");

		//We will set the point false
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setBoolean( false );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Set datapoint (pkey = %u) state (false, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey());

		FUNCTION_EXIT;

		return;
	}

    
    //Bohong++, replace the function commented above
	bool DbServerPoint::resolve()
    {
        FUNCTION_ENTRY("resolve");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"resolve of datapoint %d gets called.",getDataPoint()->getEntityKey());

        if ( !_Connect() )
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "DbServerPoint::resolve() Caught exception while trying to connect to databse server %s username %s password %s.",m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str() );
            FUNCTION_EXIT;
            return false;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully opened database with <%s>:<%s>:<%s>", 
			m_strServerName.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
		
        FUNCTION_EXIT;		
		return true;
    }


	

	void DbServerPoint::onlineConfigUpdate(std::vector<std::string> changedParams)
            {
        FUNCTION_ENTRY("onlineConfigUpdate");

		//See if the DB_SERVER_TIMEOUT_PARAM has been changed...
		std::vector<std::string>::iterator pString;

		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);

		//If the DB_SERVER_TIMEOUT_PARAM has been changed...
		if(pString != changedParams.end())
            {
			m_dbTimeOutSec = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM);
			
            } 
            
		FUNCTION_EXIT;
	}



	void DbServerPoint::_MysqlConnect()
    {
		if ( NULL != m_pMysqlConntion )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"DbServerPoint::Connect() free Oracle old connection to Mysql database server %s...",m_strServerName.c_str());
			delete m_pMysqlConntion;
			m_pMysqlConntion = NULL;
		}
		sql::ConnectOptionsMap connection_properties;
		connection_properties["hostName"] = m_strHostName;
		connection_properties["userName"] = m_strUserName;
		connection_properties["password"] = m_strPassword;
		connection_properties["metadataUseInfoSchema"] = true;

		m_pMysqlConntion = m_pMysqlDriver->connect(connection_properties);
		
		m_pMysqlConntion->setSchema(m_strShema);
	}



	void DbServerPoint::_DoMysqlQuery()
	{
		TA_ASSERT(m_pMysqlConntion != NULL, "m_connection should not be NULL");		

		int nReadOnly = 0;
		std::auto_ptr< sql::Statement > stmt(m_pMysqlConntion->createStatement());
		//m_pMysqlQuery = stmt->executeQuery("select count(*) from db_baseline_version");  // select version() cannot check the NDBCluster down
		m_pMysqlQuery = stmt->executeQuery("select @@read_only from dual");
		m_pMysqlConntion = stmt->getConnection();		
		if (m_pMysqlQuery->rowsCount() > 0)
		{
			m_pMysqlQuery->first();
			nReadOnly = m_pMysqlQuery->getInt(1);
		}

		if ( !nReadOnly )
		{
			m_pMysqlQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"DbServerPoint::_DoMysqlQuery() Database poll successfull for database server %s. Database status AVAILABLE",m_strServerName.c_str());
			m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.

			return;
		}else
		{
			m_pMysqlQuery->close();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"DbServerPoint::_DoMysqlQuery() Database read only for database server  %s. Database status UNAVAILABLE",m_strServerName.c_str());
			m_reConnect = false; // if db read only and alive, we do not need to re connect the db again.
		}
	}

	void DbServerPoint::_ParseDBType(const std::string& strDBName)
	{
		if(strDBName.compare(defOracleDBName) == 0) //case insensitive
		{
			m_nDBType = defOracleDBType;
		}
		else if (strDBName.compare(defMysqlDBName) == 0)
		{
			m_nDBType = defMysqlDBType;
		}
		else   
		{    // should never reach here, if reach here, then raise a exception
			LOG_GENERIC( SourceInfo, 
				TA_Base_Core::DebugUtil::DebugError,
				"the DB type string in database config file wrong(should be: Oracle or Mysql): %s", strDBName.c_str() );
			TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
		}
	}

} //end namespace TA_Base_App
