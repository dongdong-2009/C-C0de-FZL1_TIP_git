//////////////////////////////////////////////////////////////////////
///		@file		MysqlQueryAdapter.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    Implementation of the interface IQueryAdapter. 
///					use mysql database server to implement the query  
///                 operations.
//////////////////////////////////////////////////////////////////////
#ifndef MYSQLQUERYADAPTER_H
#define MYSQLQUERYADAPTER_H
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IQueryAdapter.h"

NS_BEGIN(sql)

class ResultSet;

NS_END(sql)


NS_BEGIN(TA_AQ_Lib)

class CMysqlQueryAdapter : public IQueryAdapter
{
public:
	CMysqlQueryAdapter( sql::ResultSet* pQuery );
	~CMysqlQueryAdapter();
	
    bool hasMore();
	
    void fetchInitial( int numColumns, Buffer& data, long lMaxRows );
	
    void fetchMore( int numColumns, Buffer& data, long lMaxRows );
	
    bool cleanQuery();
	
    bool isActive();

private:
	void _FillDataBuffer( int numColumns, Buffer& data, long lMaxRows );

private:
    CMysqlQueryAdapter();
	DEF_DISABLE_COPY_AND_ASSIGNMENT(CMysqlQueryAdapter);

private:
    
    bool                                  m_bConnectionClosed;	
    time_t                                m_tmLatestActionTime;
	sql::ResultSet*                       m_pQuery;
    TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

NS_END(TA_AQ_Lib)


#endif /// MYSQLQUERYADAPTER_H

















