#ifndef __C_ORACLE_QUERY_ADAPTER_HH__
#define __C_ORACLE_QUERY_ADAPTER_HH__

#include "ocilib.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IQueryAdapter.h"

NS_BEGIN(TA_AQ_Lib)


// namespace TA_Base_Core
// {

class COracleQueryAdapter : public IQueryAdapter
{
public:
	COracleQueryAdapter( OCI_Statement* pStatement );
	~COracleQueryAdapter();

	bool hasMore();

	void fetchInitial( int numColumns, Buffer& data, long lMaxRows );

	void fetchMore( int numColumns, Buffer& data, long lMaxRows );

	bool cleanQuery();

	bool isActive();

private:
	void _FillDataBuffer( int numColumns, Buffer& data, long lMaxRows );
	void _UpdateLatestActionTime();

private:
	// Disable default constructor, copy constructor, operator =
	COracleQueryAdapter();
	const COracleQueryAdapter& operator=( const COracleQueryAdapter& refSrc );
	COracleQueryAdapter( const COracleQueryAdapter& refSrc );

private:
	OCI_Statement* m_pStatement;
	OCI_Resultset* m_pResultset;
	
	bool m_bConnectionClosed;

	time_t m_tmLatestActionTime;

	TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

//}

NS_END(TA_AQ_Lib)


#endif // ifndef __C_ORACLE_QUERY_ADAPTER_HH__














