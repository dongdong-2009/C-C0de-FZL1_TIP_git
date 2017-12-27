#ifndef IDBCONNECTION_H
#define IDBCONNECTION_H
#include "CommonType.h"
#include <omnithread.h>  // This must be included before oci.h to avoid redefinition errors.
// It is usually located in $ORACLE_HOME/rdbms/demo
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "DBException.h"

NS_BEGIN(TA_Base_Core)

class IQueryAdapter;

class IDbConnection
{
public:
	// Various errors we are interested in.
	enum 
	{
		UNIQUE_CONSTRAINT_VIOLATED_ERROR     =     1,
			SESSION_KILLED                   =    12,
			NOT_LOGGED_IN                    =  1012,
			COLUMN_NULL_ERROR                =  1405,
			END_OF_FILE_ON_COMMS_CHANNEL     =  3113,
			NO_LISTENER                      = 12541,
			NO_SUCH_DATABASE                 = 12545,
			ALREADY_CONNECTED_ERROR          = 24309,
			ALREADY_AUTHENTICATED            = 24313,
			REQUIRE_EXPLICT_ATTACH           = 24327
		};

	IDbConnection( const std::string& connectionStr, 
		const std::string& databaseName, 
		const std::string& userName, 
		const std::string& password, 
		const std::string& hostName /*= ""*/ )
		: m_connectionStr( connectionStr ),
		m_databaseName( databaseName ),
		m_userName( userName ),
		m_password( password ),
		m_hostName( hostName ),
		m_usageCount( 0 ),
		m_executionCount( 0 )
	{

	}

	 virtual bool isAnyQueryActive( unsigned long ulMaxTimeoutInSecs ) = 0;

	 virtual std::string getConnectionString() const = 0;

	 virtual void open()     = 0;

	 virtual bool isOpen()   = 0;

	 virtual void close()    = 0;

	 virtual void closeAll() = 0;

	 virtual  void exec( 
		 const SQLStatement& rSqlObj, 
		 IQueryAdapter*& pQueryAdapter, 
		 bool isQuery = false, 
		 int prefetch = 10
		 ) = 0;

	 virtual void fetchBLOB(
		 const std::string &strSQL, 
		 const std::string &colomnName, 
		 DataClobArray& data
		 ) = 0;

	 virtual void updateBLOB(
		 const std::string &strTableName, 
		 const std::string &colomnName,
	 	 const std::vector<unsigned char>& data, 
		 const std::string &strWhereClause
		) = 0;

	 virtual void incrementExecutionCount() = 0;

	 virtual void decrementExecutionCount() = 0;

	 virtual int getExecutionCount()  = 0;

//	 virtual bool isConnectionInUse() = 0;

	 virtual void cleanQuery( IQueryAdapter*& pQuery ) = 0;

	 virtual ~IDbConnection() {};

protected:

	// Database connection data	
	const std::string m_connectionStr;
	const std::string m_databaseName;
	const std::string m_userName;
	const std::string m_password;
	const std::string m_hostName;

	//ocl::OraQuery      *m_currentQuery;
    // use map for faster find method
    std::map<IQueryAdapter*, long> m_mapActiveQuery;
	// The number of SimpleDb objects (or other database access objects)
	// that are sharing this connection.
	unsigned long m_usageCount;
	
	// The count of threads that are using this connection right now.
	// The count does not include threads that have this connection open,
	// but are not executing on it right now (ie. m_usageCount)	
	unsigned long m_executionCount;
	
	// Enables thread guarding
	TA_Base_Core::ReEntrantThreadLockable m_lock;
	TA_Base_Core::ReEntrantThreadLockable m_countLock;
	TA_Base_Core::ReEntrantThreadLockable m_queryLock;

};

NS_END(TA_Base_Core)

#endif /// IDBCONNECTION_H