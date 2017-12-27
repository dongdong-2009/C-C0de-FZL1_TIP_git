#if !defined(TrendDataFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define TrendDataFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * TrendDataFactory.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendDataFactory.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 16:11:24 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is a factory to create concrete classes for classes that implement the IGraph interface.
  *
  */

#include "app/trending/trend_viewer/src/TrendDataFactory.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/CompositeStore.h"
#include "app/trending/trend_viewer/src/ArchiveDataStore.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"
#include "app/trending/trend_viewer/src/IDataLoader.h"
#include "app/trending/trend_viewer/src/HistoricalDataLoader.h"
#include "app/trending/trend_viewer/src/ILoaderObserver.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{
    TrendDataFactory* TrendDataFactory::s_trendDataFactory = NULL;
	// TA_Base_Core::NonReEntrantThreadLockable TrendDataFactory::s_singletonLock; //TD17935

    //
    // getInstance
    //
    TrendDataFactory& TrendDataFactory::getInstance()
    {
        // protect the instance check / creation
		// TA_Base_Core::ThreadGuard guard(s_singletonLock); //TD17935

        if(s_trendDataFactory == 0)
        {
            s_trendDataFactory = new TrendDataFactory();
        }
        
        return *s_trendDataFactory;
    }


    //
    // getItemStore
    //
    std::auto_ptr<IItemStore> TrendDataFactory::createCompositeStore(ILoaderObserver* loaderObserver)
    {
        FUNCTION_ENTRY("createCompositeStore");

        return std::auto_ptr<IItemStore>(new CompositeStore(loaderObserver));

        FUNCTION_EXIT;
    }


    //
    // getArchiveDataStore
    //
    std::auto_ptr<IArchiveDataStore> TrendDataFactory::createArchiveDataStore()
    {
        FUNCTION_ENTRY("createArchiveDataStore");

        return std::auto_ptr<IArchiveDataStore>(new ArchiveDataStore);

        FUNCTION_EXIT;
    }


    //
    // getHistoricalDataLoader
    //
    std::auto_ptr<IDataLoader> TrendDataFactory::createHistoricalDataLoader()
    {
        FUNCTION_ENTRY("createHistoricalDataLoader");

        return std::auto_ptr<IDataLoader>(new HistoricalDataLoader);

        FUNCTION_EXIT;
    }

}

#endif // TrendDataFactoryCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_
