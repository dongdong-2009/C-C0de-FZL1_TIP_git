#if !defined(CompositeStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define CompositeStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * CompositeStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/CompositeStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements IItemStore and keeps a store of all items that can be trended.
  * This class is the main store of all the different types of items (eg. DataPoints)
  * and contains a number of IItemStore objects that represent these different types. If more
  * types are added, they also need to be added to this class.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/ILoaderObserver.h"
#include "app/trending/trend_viewer/src/GraphedItem.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
    //
    // Forward declarations
    //
    class ILoaderObserver;
    class DataPointStore;

    //
    // CompositeStore Class
    //
    class CompositeStore : public IItemStore, public ILoaderObserver
    {

    public:

        /**
          * CompositeStore
          * 
          * Constructor.
          *
          */
        CompositeStore(ILoaderObserver* loaderObserver);

        /**
          * ~CompositeStore
          * 
          * Destructor
          *
          */
        virtual ~CompositeStore();

        /**
          * loadItems
          * 
          * Starts threads to do the loading and returns immediately.
          */
        virtual void loadItems();

        /**
          * getAllItems
          * 
          * Returns a list of names of the loadable items.
          *
          * @return std::vector<std::string> a vector containing names of items.
          */
        virtual std::vector<std::string> getAllItems();

        /**
          * getItem
          * 
          * Return a pointer to a generic item. Composite Store will manage the
          * returned object so the user should not free it.
          *
          * @param name - name of the item to retrieve
          *
          * @return GenericItem* - pointer to the generic item object
          *
          * @exception TrendViewerException - thrown when an item with specified name
          * cannot be found.
          */
        virtual GenericItem* getItem(const std::string& itemName);

        /**
          * loadItems
          * 
          * Load all the items from the database.
          *
          * @param itemNames  - vector of the names of the items to load
          */
        virtual void loadItems(std::vector<std::string> itemNames);

        /**
          * loadingComplete
          * 
          * This method is called once all items have been loaded from the database.
          */
        virtual void loadingComplete();

        /**
          * processError
          * 
          * This method is called if there has been a database loading error.
          *
          * @param error - error message
          */
        virtual void processError(const std::string& error);

        /**
          * setStateLabels
          * 
          * For some items, string labels are displayed on the y-axis instead of numerical
          * values. This method retrieves/initialises the state labels for an item that
          * is graphed.
          *
          * @param genericItem - the item that is graphed
          */
        virtual void setStateLabels(IGenericItem* genericItem);

    private:

        CompositeStore( const CompositeStore& theCompositeStore);
        CompositeStore& operator=(const CompositeStore &);
        
    private:

        std::auto_ptr<DataPointStore> m_dataPointStore;         // Store of the data point items
    
        int m_completedLoads;                                   // Count of how many loads have been completed, where
                                                                // a completed load is all the items of one type has
                                                                // has been completely loaded.

        ILoaderObserver* m_loaderObserver;                      // Object to notify when loading is complete

        static int TOTAL_LOADERS;                               // Total number of loads that have to complete before
                                                                // all item types have been completely loaded. This
                                                                // value should correspond to how many item types there are.

        TA_Base_Core::ReEntrantThreadLockable m_lock;
    };
}

#endif // !defined(CompositeStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
