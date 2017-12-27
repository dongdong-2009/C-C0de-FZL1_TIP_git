//////////////////////////////////////////////////////////////////////
///		@file		IDbConnection.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    the interface to access various database servers 
///                 such as MySQL and Oracle etc. for the database 
///                 synchronization module.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef IDBCONNECTION_H
#define IDBCONNECTION_H

#include "AQCommonData.h"
#include "DbCommonData.h"
#include <map>

NS_BEGIN(TA_AQ_Lib)

class IQueryAdapter;
class CMessage;

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

	IDbConnection(const std::string& strDbName, const std::string& strUserName,
				  const std::string& strPwd, const std::string& strHost = "")
				  : m_strSchema(strDbName), m_strUserName(strUserName)
				  , m_strPwd(strPwd), m_strHostName(strHost){}

	 virtual bool isAnyQueryActive() = 0;

	 virtual void open()     = 0;

	 virtual bool isOpen()   = 0;

	 virtual void close()    = 0;

	 virtual void closeAll() = 0;

	 virtual void setAutoCommit(bool bAutoCommit) = 0;
	 virtual bool getAutoCommit() = 0;

	 virtual void rollback() = 0;
	 virtual void commit()   = 0;

	 virtual  void exec( 
		 const std::string& statement, 
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

	 virtual void cleanQuery( IQueryAdapter*& pQuery ) = 0;

	 virtual ~IDbConnection() {};

public:
	virtual int    enqMsg(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt) = 0;
	virtual int	   enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt ) = 0;
	virtual int    deqMsg(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg) = 0;
	virtual bool   listen(const std::string& strQName, u_int  nSubID) = 0;
	virtual bool   listen(const std::string& strQName, const std::string& strSubName) = 0;
	virtual int    aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair) = 0;
	virtual int    aqCommit(const std::string& strQName, const std::string& strMsgID) = 0;
	virtual int    aqGetSrvID(){ return Ret_Success; }


protected:

	typedef  std::vector<std::string>                         StrVecT;
	typedef  std::vector<std::string>::const_iterator         StrVeccIterT;

protected:

    // use map for faster find method
    std::map<IQueryAdapter*, long> m_mapActiveQuery;

	std::string     m_strSchema;
	std::string     m_strUserName;
	std::string     m_strPwd;
	std::string     m_strHostName;

};

NS_END(TA_AQ_Lib)

#endif /// IDBCONNECTION_H














