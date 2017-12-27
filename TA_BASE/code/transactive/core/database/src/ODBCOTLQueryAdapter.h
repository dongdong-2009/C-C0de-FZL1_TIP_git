#ifndef ODBC_OTL_QueryAdapter_H
#define ODBC_OTL_QueryAdapter_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IQueryAdapter.h"
#include "core/otl/src/otlv4.h"


namespace TA_Base_Core
{

class ODBCOTLQueryAdapter : public IQueryAdapter
{
public:
    ODBCOTLQueryAdapter(otl_stream* pQuery );
    ~ODBCOTLQueryAdapter();

    bool hasMore();

    void fetchInitial( int numColumns, Buffer& data, long lMaxRows );

    void fetchMore( int numColumns, Buffer& data, long lMaxRows );

    bool cleanQuery();

    bool isActive( unsigned long ulMaxTimeoutInSecs );

private:
    void fillDataBuffer( int numColumns, Buffer& data, long lMaxRows );
    void updateLatestActionTime();

private:
    // Disable default constructor, copy constructor, operator =
    ODBCOTLQueryAdapter();
    const ODBCOTLQueryAdapter& operator=( const ODBCOTLQueryAdapter& refSrc );
    ODBCOTLQueryAdapter( const ODBCOTLQueryAdapter& refSrc );
private:
	std::string _GetColumnValue(otl_stream* pQuery, int nColumnIndex);
	std::string _GetColumnName(otl_stream* pQuery, int nColumnIndex);
	int _GetColumnCount(otl_stream* pQuery);
	
private:
    otl_stream* m_pQuery;
    bool m_bConnectionClosed;

    time_t m_tmLatestActionTime;

    TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

}

#endif // ifndef OCIQueryAdapter_H