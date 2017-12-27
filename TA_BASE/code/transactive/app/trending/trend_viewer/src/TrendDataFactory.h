#if !defined(TrendDataFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define TrendDataFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * TrendDataFactory.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendDataFactory.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a factory to create concrete classes for classes that implement the IGraph interface.
  *
  */

#include <memory>

//#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD17935

namespace TA_Base_App
{
    // 
    // Forward declaration
    //
    class IItemStore;
    class IArchiveDataStore;
    class IDataLoader;
    class HistoricalDataLoader;
    class ILoaderObserver;

    //
    // TrendDataFactory
    //
    class TrendDataFactory 
    {

    public:

        static TrendDataFactory& getInstance();
    
        std::auto_ptr<IItemStore> createCompositeStore(ILoaderObserver* loaderObserver);

        std::auto_ptr<IArchiveDataStore> createArchiveDataStore();

        std::auto_ptr<IDataLoader> createHistoricalDataLoader();

    private:

        TrendDataFactory() {}
        TrendDataFactory(const TrendDataFactory&) {}
        TrendDataFactory& operator=(const TrendDataFactory&) {}

    private:

        // the one and only class instance
        static TrendDataFactory* s_trendDataFactory;
        
        // protect singleton creation
		//TD17935 - no need to protect singleton creation thru a thread
		// static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock; 
    };

}

#endif // TrendDataFactory_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_
