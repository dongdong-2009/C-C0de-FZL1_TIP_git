#if !defined(ItemLoaderThreadCpp_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
#define ItemLoaderThreadCpp_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_

/**
  * ItemLoaderThread.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ItemLoaderThread.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a thread that retrieves data from the database. It will notify a Loader Observer once
  * all data has been retrieved from the database.
  *
  */

#include "app/trending/trend_viewer/src/ItemLoaderThread.h"
#include "app/trending/trend_viewer/src/ILoaderObserver.h"
#include "app/trending/trend_viewer/src/IItemLoader.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/TrendViewerException.h"

namespace TA_Base_App
{
    //
    // ItemLoaderThread
    //
    ItemLoaderThread::ItemLoaderThread(ILoaderObserver* loaderObserver, IItemLoader* itemLoader)
        : m_loaded(false),
          m_terminated(false),
          m_loaderObserver(loaderObserver),
          m_itemLoader(itemLoader)
    {
        FUNCTION_ENTRY("ItemLoaderThread");

        FUNCTION_EXIT;
    }

   
    //
    // run
    //
    void ItemLoaderThread::run()
    {
        FUNCTION_ENTRY("run");

        try
        {
            m_itemLoader->retrieveFromDatabase();
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TA_Base_Core::TrendViewerException::DATABASE_ERROR)
            {
                // Notify main store that there has been a database error
                m_loaderObserver->processError(ex.what());
            }
            else
            {
                m_loaderObserver->processError("An unexpected error occurred while loading items.");    
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            m_loaderObserver->processError("An unexpected error occurred while loading items.");    
        }
        
        // Only notify the observer if we're not terminating
        if(!m_terminated)
        {
            // Notify main store that loading is complete
            m_loaderObserver->loadingComplete();
        }

        m_loaded = true;

        FUNCTION_EXIT;
    }
    
 
    //
    // terminate
    //
    void ItemLoaderThread::terminate()
    {
        FUNCTION_ENTRY("terminate");

        m_terminated = true;

        // Don't terminate the thread until the loading has completed
        while(!m_loaded)
        {
            const int SLEEP_IN_MILLISECS = 1000;
            sleep(SLEEP_IN_MILLISECS);
        }

        FUNCTION_EXIT;
    }

}

#endif // !defined(ItemLoaderThreadCpp_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
