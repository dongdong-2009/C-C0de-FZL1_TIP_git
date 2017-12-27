#ifndef OCIQueryAdapter_H
#define OCIQueryAdapter_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IQueryAdapter.h"

namespace ocl
{
    class OraQuery;
}

namespace TA_Base_Core
{

class OCIQueryAdapter : public IQueryAdapter
{
public:
    OCIQueryAdapter( ocl::OraQuery* pQuery );
    ~OCIQueryAdapter();

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
    OCIQueryAdapter();
    const OCIQueryAdapter& operator=( const OCIQueryAdapter& refSrc );
    OCIQueryAdapter( const OCIQueryAdapter& refSrc );

private:
    ocl::OraQuery* m_pQuery;
    bool m_bConnectionClosed;

    time_t m_tmLatestActionTime;

    TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

}

#endif // ifndef OCIQueryAdapter_H