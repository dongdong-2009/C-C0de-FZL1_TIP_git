#if !defined(ItemLoaderThread_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
#define ItemLoaderThread_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_

/**
  * ItemLoaderThread.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ItemLoaderThread.h $
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

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{
    //
    // Forward Declarations
    //
    class ILoaderObserver;
    class IItemLoader;

    //
    // ItemLoaderThread
    //
    class ItemLoaderThread : public TA_Base_Core::Thread
    {

    public:

        /**
          * ItemLoaderThread
          * 
          * Constructor.
          */
        ItemLoaderThread(ILoaderObserver* loaderObserver, IItemLoader* itemLoader);

        /**
          * ItemLoaderThread
          * 
          * Destructor.
          */
        ~ItemLoaderThread() {}

        /**
          * run
          * 
          * Load all the items from the database.
          */
        virtual void run();
        
        /**
          * terminate
          * 
          * Stop loading from the database.
          */
        virtual void terminate();

    private:

        ItemLoaderThread( const ItemLoaderThread& itemLoaderThread);
        ItemLoaderThread& operator=(const ItemLoaderThread &);

    private:

        bool volatile m_loaded;                 // Indicates whether items have finished loading
        bool volatile m_terminated;             // Indicates whether thread has been terminated
        IItemLoader* m_itemLoader;              // Item Loader than does the actual loading
        ILoaderObserver* m_loaderObserver;      // Observer to notify when loading is complete
    };
}

#endif // !defined(ItemLoaderThread_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
