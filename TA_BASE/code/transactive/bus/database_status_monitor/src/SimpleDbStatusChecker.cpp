/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/database_status_monitor/src/SimpleDbStatusChecker.cpp $
 * @author:  J.Chevalley
 * @version: $Revision: #8 $
 *
 * Last modification: $DateTime: 2015/04/07 14:28:48 $
 * Last modified by:  $Author: Ouyang $
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "bus/database_status_monitor/src/SimpleDbStatusChecker.h"
#include "bus/database_status_monitor/src/IDatabaseStatusObserver.h"
#include "core/database/src/SimpleDb.h"
#include "core/database/src/DBException.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/otl/src/otlconf.h"
#include "driver/mysql_public_iface.h"  // oyzl++ for mysql connection
#include <boost/algorithm/string.hpp>

#ifdef USING_OTL
    //to replace the OCL with OTL
    #include "core/otl/src/otlv4.h"
#else
    #include "core/ocl/src/ocl.h"
    using namespace ocl;
#endif

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
#define  defOracleDBName          "Oracle"
#define  defMysqlDBName           "Mysql"

#define  defOracleDBType           1
#define  defMysqlDBType            2

    SimpleDbStatusChecker::SimpleDbStatusChecker(const std::string& connectionInfo, unsigned long periodInSeconds, IDatabaseStatusObserver* observer)
        : m_dbConnectionInfo(connectionInfo),
          m_checkPeriodInSeconds(periodInSeconds),
          m_dbStatusObserver(observer),
          m_dbStatus(DatabaseStatusCorbaDef::statusUnknown),
          m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
          m_reConnect(false),
          m_pOraConnection(0),
          m_pMysqlConntion(0),  // oyzl++ for mysql connection
          m_pMysqlDriver(0),
          m_pMysqlQuery(0)
    {
        std::vector<std::string> components;
        SimpleDb::ParseConnectStr(connectionInfo, components, ':');
        TA_ASSERT(5 == components.size(), "Invalid connection string.");

        ParseDBType(components[CON_STR_DBType]); // oyzl++ for supporting multiple DB server (Oracle & Mysql)
        m_schemaName  = components[CON_STR_SchemaName];
        m_strUserName = components[CON_STR_UserName];
        m_dbPassword  = components[CON_STR_Passwd];
        m_hostName    = components[CON_STR_Hostname];
        //[User Name]/[Password]@[TNS Alias]
        //"IT271350_5/IT271350_5@TRANSACT"
        m_strOTLConnection = m_strUserName + "/" + m_dbPassword + "@" + m_schemaName;

        Init();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "[DEBUG marvin]SimpleDbStatusChecker::Connect() created for hostname %s, schemaName %s, userName: %s, password %s",
                    m_hostName.c_str(), m_schemaName.c_str(), m_strUserName.c_str(), m_dbPassword.c_str());
    }

#ifdef USING_OTL
    //OTL
    SimpleDbStatusChecker::~SimpleDbStatusChecker()
    {
        delete m_pOraQuery;   m_pOraQuery = NULL;
        delete m_pMysqlQuery; m_pMysqlQuery = NULL;

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
    }

    bool SimpleDbStatusChecker::Connect()
    {
        // not currently connected - try to connect
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "SimpleDbStatusChecker::Connect() create a new connection to database server %s...", m_schemaName.c_str());

            switch (m_nDBType)
            {
            case defOracleDBType:
                OraConnection();
                break;

            case defMysqlDBType:
                MysqlConnect();
                break;

            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s", m_dbConnectionInfo.c_str());
                break;
            }
        }
        catch (otl_exception & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. OTL_ERROR:[%s] ", m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), (char*)(e.msg));

            if (1 == m_pOraConnection->connected)
                m_pOraConnection->logoff();

            return false;
        }
        catch (sql::SQLException & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. Mysql_ERROR:[%s] ", m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.what());
            delete m_pMysqlConntion;
            m_pMysqlConntion = NULL;
            return false;
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s. Returning false", m_schemaName.c_str());

            if (1 == m_pOraConnection->connected)
                m_pOraConnection->logoff();

            delete m_pMysqlConntion;
            m_pMysqlConntion = NULL;
            return false;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "SimpleDbStatusChecker::Connect() Successfull connection to databse server %s", m_schemaName.c_str());
        return true;
    }

    void SimpleDbStatusChecker::Poll()
    {
        try
        {
            if (true == m_reConnect)
            {
                if (!Connect())
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
						"SimpleDbStatusChecker::Poll() connect failed for database server %s. Database set to UNAVAILABLE",m_schemaName.c_str());
					setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);	
                    m_reConnect = true;//
                    return;
                }
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Executing dummy select for database %s.", m_schemaName.c_str());

            //create a query to test DB server usability.
            switch (m_nDBType)
            {
            case defOracleDBType:
                DoOraQuery();
                break;

            case defMysqlDBType:
                DoMysqlQuery();
                break;

            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s", m_dbConnectionInfo.c_str());
                break;
            }
        }
        catch (otl_exception &e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE", m_schemaName.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Database Error code %d. Reason: %s ; will re-connect database", e.code, (char*)(e.msg));
            //if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
            {
                m_reConnect =  true;

                if (m_pOraQuery && m_pOraQuery->good())
                {
                    m_pOraQuery->close();
                }

                if (m_pOraConnection != 0 && 1 == m_pOraConnection->connected)
                {
                    m_pOraConnection->logoff();
                }
            }
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }
        catch (sql::SQLException &e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Query failed for database server %s:%s. Database status UNAVAILABLE", m_schemaName.c_str(), m_hostName.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Database Error code %d. Reason: %s ; will re-connect database", e.getErrorCode(), e.what());
            //if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
            {
                m_reConnect =  true;

                if (m_pMysqlQuery && !m_pMysqlQuery->isClosed())
                    m_pMysqlQuery->close();
            }
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }

        catch (...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Caught exception while trying to connect to databse server %s.", m_schemaName.c_str());
            return ;
        }
    }

    void SimpleDbStatusChecker::OraConnection()
    {
        TA_ASSERT(NULL != m_pOraConnection, "connection pointer is null");

        if (1 == m_pOraConnection->connected)
        {
            return;
        }
        else
        {
            m_pOraConnection->rlogon(m_strOTLConnection.c_str(), 0); // connect to Oracle
        }
    }

    void SimpleDbStatusChecker::DoOraQuery()
    {
        TA_ASSERT(m_pOraConnection != NULL, "m_connection should not be NULL");

        if (1 != m_pOraConnection->connected)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                        "SimpleDbStatusChecker::Poll() Database is not connected %s. Database status UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
            return;
        }

        std::string strSQL = "select * from V$version";
        m_pOraQuery->open(100, strSQL.c_str(), *m_pOraConnection);

        if (1 == m_pOraQuery->good() && 1 != m_pOraQuery->eof())
        {
            m_pOraQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database poll successfull for database server %s. Database status AVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
            m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.
            return;
        }
        else
        {
            m_pOraQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database returned empty result for database server  %s. Database status UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
        }
    }

    void SimpleDbStatusChecker::Init()
    {
        int nThreadedMode = 1;

        switch (m_nDBType)
        {
        case defOracleDBType:

            // initialize OCI environment
            if (1 != otl_connect::otl_initialize(nThreadedMode))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "otl_connect::otl_initialize error!");
            }

            m_pOraConnection = new otl_connect();
            TA_ASSERT(NULL != m_pOraConnection, "Init():connection pointer is null.");
            //disable below line due to it causes crashing of system controller/Control station if one of databases is died.
            //m_pOraConnection->rlogon(m_strOTLConnection.c_str(), 0); // connect to Oracle
            m_pOraQuery = new otl_stream();

            break;

        case defMysqlDBType:
            m_pMysqlDriver = (sql::Driver*)get_driver_instance();
            break;
        }
    }

#else
    //OCL

    SimpleDbStatusChecker::~SimpleDbStatusChecker()
    {
        delete m_pOraQuery;   m_pOraQuery = NULL;
        delete m_pMysqlQuery; m_pMysqlQuery = NULL;

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
    }

    bool SimpleDbStatusChecker::Connect()
    {
        // not currently connected - try to connect
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "SimpleDbStatusChecker::Connect() create a new connection to database server %s...", m_schemaName.c_str());

            switch (m_nDBType)
            {
            case defOracleDBType:
                OraConnection();
                break;

            case defMysqlDBType:
                MysqlConnect();
                break;

            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s", m_dbConnectionInfo.c_str());
                break;
            }
        }
        catch (ocl::OraException & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. OCI_ERROR:[%s] ", m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.message().c_str());

            if (m_pOraConnection->isConnected())
                m_pOraConnection->close();

            return false;
        }
        catch (sql::SQLException & e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s schema %s password %s. Mysql_ERROR:[%s] ", m_hostName.c_str(), m_schemaName.c_str(), m_dbPassword.c_str(), e.what());
            delete m_pMysqlConntion;
            m_pMysqlConntion = NULL;
            return false;
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Connect() Caught exception while trying to connect to databse server %s. Returning false", m_schemaName.c_str());

            if (m_pOraConnection->isConnected())
                m_pOraConnection->close();

            delete m_pMysqlConntion;
            m_pMysqlConntion = NULL;
            return false;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "SimpleDbStatusChecker::Connect() Successfull connection to databse server %s", m_schemaName.c_str());
        return true;
    }

    void SimpleDbStatusChecker::Poll()
    {
        try
        {
            if (true == m_reConnect)
            {
                if (!Connect())
                {
                    // if can not connect, continuely trying to connect DB in next loop.
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                                "SimpleDbStatusChecker::Poll() connect failed for database server %s. Database set to UNAVAILABLE", m_schemaName.c_str());
                    setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
                    m_reConnect = true;//
                    return;
                }
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                        "SimpleDbStatusChecker::Poll() Executing dummy select for database %s.", m_schemaName.c_str());

            //create a query to test DB server usability.
            switch (m_nDBType)
            {
            case defOracleDBType:
                DoOraQuery();
                break;

            case defMysqlDBType:
                DoMysqlQuery();
                break;

            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "SimpleDbStatusChecker::Connect() the database type do not support. the connection string: %s", m_dbConnectionInfo.c_str());
                break;
            }
        }
        catch (ocl::OraException &e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE", m_schemaName.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Database Error code %d. Reason: %s ; will re-connect database", e.errorCode(), e.message().c_str());
            //if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
            {
                m_reConnect =  true;

                if (m_pOraQuery && m_pOraQuery->isActive())
                    m_pOraQuery->close();

                m_pOraConnection = m_pOraQuery->connection();
            }
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }
        catch (sql::SQLException &e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE", m_schemaName.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Database Error code %d. Reason: %s ; will re-connect database", e.getErrorCode(), e.what());
            //if (12630 < e.errorCode() && e.errorCode() > 12510)// which are relavent TNS error, whatever they are network or listener problems
            {
                m_reConnect =  true;

                if (m_pMysqlQuery && !m_pMysqlQuery->isClosed())
                    m_pMysqlQuery->close();
            }
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
        }
        catch (CRException&)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Query failed for database server %s. Database status UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect =  true;

            if (m_pOraQuery && m_pOraQuery->isActive())
                m_pOraQuery->close();

            m_pOraConnection = m_pOraQuery->connection();
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "SimpleDbStatusChecker::Poll() Caught exception while trying to connect to databse server %s.", m_schemaName.c_str());
            return ;
        }
    }

    void SimpleDbStatusChecker::OraConnection()
    {
        TA_ASSERT(NULL != m_pOraConnection, "connection pointer is null");

        if (m_pOraConnection->isConnected())
            return;
        else
            m_pOraConnection->open();
    }

    void SimpleDbStatusChecker::DoOraQuery()
    {
        TA_ASSERT(m_pOraConnection != NULL, "m_connection should not be NULL");

        if (!m_pOraConnection->isConnected())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                        "SimpleDbStatusChecker::Poll() Database is not connected %s. Database status UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
            return;
        }

        m_pOraQuery->setConnection(*m_pOraConnection);
        m_pOraQuery->setCommandType(ocl::ctSQL);
        m_pOraQuery->setCached(false);  // noncached mode
        m_pOraQuery->setFetchRows(100);
        m_pOraQuery->setSQL("select * from V$version");
        m_pOraQuery->open();   // query result rows
        m_pOraConnection = m_pOraQuery->connection();

        if (m_pOraQuery->isRowsReturn())
        {
            m_pOraQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database poll successfull for database server %s. Database status AVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
            m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.
            return;
        }
        else
        {
            m_pOraQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database returned empty result for database server  %s. Database status UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
        }
    }

    void SimpleDbStatusChecker::Init()
    {
        switch (m_nDBType)
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
            m_pOraConnection->setPassword(m_dbPassword);
            m_pOraConnection->setServer(m_schemaName);
            break;

        case defMysqlDBType:
            m_pMysqlDriver = (sql::Driver*)get_driver_instance();
            break;
        }
    }

#endif//USING_OTL

    void SimpleDbStatusChecker::setDatabaseStatus(DatabaseStatusCorbaDef::EState status)
    {
        //Updates the stored database status.
        //If there has been a change in status
        //the observer is notified.
        if (m_dbStatusObserver && m_dbStatus != status)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "SimpleDbStatusChecker::setDatabaseStatus() - Notify status change for database server %s. Database set to %s",
                        m_schemaName.c_str(), DatabaseStatusCorbaDef::statusAvailable == status ? "AVAILABLE" : "UNAVAILABLE");

            m_dbStatusObserver->notifyStatusChange(m_dbConnectionInfo, status);
            m_dbStatus = status;
        }
    }

    void SimpleDbStatusChecker::run()
    {
        // are/can we connect?
        if (!Connect())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                        "SimpleDbStatusChecker::run() connect failed for database server %s. Database set to UNAVAILABLE", m_schemaName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
            //return;
        }

        m_timerUtility.startPeriodicTimeOutClock(this, 300000);
        m_run = true;

        while (m_run)
        {
            Poll();

            // thread sleep is in milliseconds
            for (size_t i = 0, STEP = 100; m_run && i < m_checkPeriodInSeconds * 1000; i += STEP)
            {
                sleep(STEP);
            }
        }

        m_timerUtility.stopPeriodicTimeOutClock(this);
    }

    void SimpleDbStatusChecker::MysqlConnect()
    {
        if (NULL != m_pMysqlConntion)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "SimpleDbStatusChecker::Connect() free old connection to Mysql database server %s...", m_hostName.c_str());
            delete m_pMysqlConntion;
            m_pMysqlConntion = NULL;
        }

        TA_ASSERT(NULL != m_pMysqlDriver, "the mysql driver is null");
        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = m_hostName;
        connection_properties["userName"] = m_strUserName;
        connection_properties["password"] = m_dbPassword;
        connection_properties["metadataUseInfoSchema"] = true;

        m_pMysqlConntion = m_pMysqlDriver->connect(connection_properties);

        m_pMysqlConntion->setSchema(m_schemaName);
    }

    void SimpleDbStatusChecker::DoMysqlQuery()
    {
        TA_ASSERT(m_pMysqlConntion != NULL, "m_connection should not be NULL");

        if (m_pMysqlConntion->isClosed())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                        "SimpleDbStatusChecker::Poll() Database is not connected %s:%s. Database status UNAVAILABLE", m_schemaName.c_str(), m_hostName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = true;
            return;
        }

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

        if (!nReadOnly)
        {
            m_pMysqlQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database poll successfull for database server %s:%s. Database status AVAILABLE", m_schemaName.c_str(), m_hostName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusAvailable);
            m_reConnect = false;// if db is connected and the fetching data is in normal, we do not need to re connect the db.

            return;
        }
        else
        {
            m_pMysqlQuery->close();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDatabase,
                        "SimpleDbStatusChecker::Poll() Database returned read only for database server  %s:%s. Database status UNAVAILABLE or Read Only", m_schemaName.c_str(), m_hostName.c_str());
            setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
            m_reConnect = false; // if db just read only and alive, we do not need to re connect the db again.
        }
    }

    void SimpleDbStatusChecker::ParseDBType(const std::string& strDBName)
    {
        if (boost::iequals(strDBName, defOracleDBName))
        {
            m_nDBType = defOracleDBType;
        }
        else if (boost::iequals(strDBName, defMysqlDBName))
        {
            m_nDBType = defMysqlDBType;
        }
        else
        {
            // should never reach here, if reach here, then raise a exception
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "the DB type string in database config file wrong(should be: Oracle or Mysql): %s", strDBName.c_str());
            TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
        }
    }

    void SimpleDbStatusChecker::terminate()
    {
        m_run = false;
    }

    void SimpleDbStatusChecker::timerExpired(long timerId, void* userData)
    {
        if (m_dbStatusObserver)
            m_dbStatusObserver->notifyStatusChange(m_dbConnectionInfo, m_dbStatus);
    }

    void SimpleDbStatusChecker::setDatabaseOffLine()
    {
        setDatabaseStatus(DatabaseStatusCorbaDef::statusUnAvailable);
    }
}//namespace
