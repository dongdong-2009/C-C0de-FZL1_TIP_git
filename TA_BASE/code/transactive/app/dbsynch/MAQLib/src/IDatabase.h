//////////////////////////////////////////////////////////////////////
///		@file		IDBConnector.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    the interface to access various database servers 
///                 such as MySQL and Oracle etc. for the database 
///                 synchronization module.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  IDATABASE_H
#define  IDATABASE_H

#include "AQCommonData.h"
#include "sqldef.h"
#include <string>
#include <vector>

NS_BEGIN(TA_AQ_Lib)

class IDbData;
class SQLTypeAdapter;
class CMessage;

class IDatabase
{
    public:  
		virtual int      setDbConStr(const std::string& strConStr) = 0;

        virtual void     connect()    = 0;        
        virtual void     disconnect() = 0;       
		virtual void     cleanQuery() = 0;        
        virtual IDbData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames, 
                                    const u_long numRows) = 0;
        virtual IDbData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames) = 0;        
        virtual void     executeModification(const std::string& sql) = 0; 
        virtual void     executeProcedure(const std::string& sql)    = 0;
        
        virtual bool     moreData(IDbData*& returnData) = 0;		
		virtual void     readBLOB(const std::string& fieldName, const std::string& tableName,
							    const std::string& whereClause, std::vector<u_char>& data )  = 0;		
		virtual void     writeBLOB(const std::string& fieldName, const std::string& tableName,
							     const std::string& whereClause, std::vector<u_char>& data ) = 0;        
        virtual void     beginTransaction()    = 0;        
        virtual void     commitTransaction()   = 0;        
        virtual void     rollbackTransaction() = 0;  
		virtual int      getDbServerType()     = 0;
		virtual int      getDBServerID()       = 0;

        virtual std::string escapeInsertString(const std::string& theString) = 0;        
        virtual std::string escapeQueryString(const std::string& theString)  = 0;		
		virtual std::string escapeAQSQLString(const std::string& theString)  = 0;					
		
		PURE_SQL_FUNCS(void, prepareSQLStatement);

        virtual ~IDatabase() {};
        
public:
	virtual int    enqMsg(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt) = 0;
	virtual int	   enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt ) = 0;

	virtual int    deqMsg(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg) = 0;	
	virtual bool   listen(const std::string& strQName, u_int  nSubID) = 0;
	virtual bool   listen(const std::string& strQName, const std::string& strSubName) = 0;
	virtual int    aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& nSeqIDPair) = 0;
	virtual int    aqCommit(const std::string& strQName, const std::string& strMsgID) = 0;

	
protected:
	int                      m_nDbType;         // the database server type
	std::string              m_strUserName;     // the database user name
	std::string              m_strPwd;          // the database password
	std::string              m_strSchema;       // the schema name
	std::string              m_strHostName;     // the hostName or IP address of the database
};

NS_END(TA_AQ_Lib)

#endif  // IDATABASE_H





























