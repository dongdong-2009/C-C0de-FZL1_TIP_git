/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/database_status_monitor/src/SimpleDbStatusChecker.h $
 * @author:  J.Chevalley
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/23 16:19:20 $
 * Last modified by:  $Author: Ouyang $
 *
 */

#if !defined(SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_)
#define SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_

#include <string>
#include "core/otl/src/otlconf.h"
//
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "boost/shared_ptr.hpp"

#ifdef USING_OTL
class otl_connect;
class otl_stream;
#else
namespace ocl
{
	class OraConnection;
	class OraQuery;
}
#endif

namespace sql
{
	class Driver;
	class Connection;
	class ResultSet;
}

namespace TA_Base_Core
{
	class SingletonTimerUtil;
}

namespace TA_Base_Bus
{
	class IDatabaseStatusObserver;
    class SimpleDbStatusChecker : public TA_Base_Core::Thread,
								  public virtual TA_Base_Core::ITimeoutCallback
    {
    public:
	    /**
	     * Constructor.
	     * @param connectionInfo    A connection string in the form "DATABASE:USER:PWD"
	     * @param periodInSeconds    The database status check period in seconds.
	     * @param observer    A reference to the status observer.
	     *
	     */
	    SimpleDbStatusChecker(const std::string& connectionInfo, unsigned long periodInSeconds, IDatabaseStatusObserver *observer);
        virtual ~SimpleDbStatusChecker();

    public:
	    /**
	     * The thread's main loop.
	     */
	    virtual void run();
	    virtual void terminate();

		//Implement for ITimeoutCallback;
		void timerExpired(long timerId, void* userData);

		void setDatabaseOffLine();

    private:
	    /**
	     * Updates the stored database status.
	     * @param status
	     *
	     */
        void setDatabaseStatus(DatabaseStatusCorbaDef::EState status);
        bool Connect();
        void Poll();
		// oyzl++
		void OraConnection();
		void MysqlConnect();
		void DoOraQuery();
		void DoMysqlQuery();
		void ParseDBType(const std::string& strDBName);
		void Init();

    private:
        DatabaseStatusCorbaDef::EState m_dbStatus;
        std::string m_dbConnectionInfo;
        std::string m_hostName;     // for mysql, network name or IP Address
        std::string m_schemaName;   // for oracle, TNS name
		std::string m_strUserName;
        std::string m_dbPassword;
		sql::Connection*     m_pMysqlConntion;
		sql::Driver*         m_pMysqlDriver;
	    IDatabaseStatusObserver *m_dbStatusObserver;
	    unsigned long m_checkPeriodInSeconds;
        bool volatile m_run;
		int  m_nDBType; // oyzl++

		// Wenguang++
		TA_Base_Core::SingletonTimerUtil& m_timerUtility;
		sql::ResultSet* m_pMysqlQuery;
		bool m_reConnect;
		std::string m_strOTLConnection;
#ifdef USING_OTL
		otl_connect* m_pOraConnection;
		otl_stream* m_pOraQuery;
#else
		ocl::OraConnection*  m_pOraConnection;
		ocl::OraQuery* m_pOraQuery;
#endif
    };
} //namespace
#endif // !defined(SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_)
