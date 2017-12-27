#if !defined(DataPointStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define DataPointStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * DataPointStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements IItemStore and keeps a store of data point items.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include <map>

#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IItemLoader.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "boost/smart_ptr.hpp"

namespace TA_Base_App
{
    //
    // Forward declarations
    //
    class ItemLoaderThread;
    class ILoaderObserver;
    
    //
    // DataPointStore Class
    //
    class DataPointStore : public IItemStore, public IItemLoader, public TA_Base_Bus::IEntityUpdateEventProcessor
    {

    public:

        /**
          * DataPointStore    
          * 
          * Constructor.    
          *
          */
        DataPointStore(ILoaderObserver* loaderObserver);

        /**
          * ~DataPointStore
          * 
          * Destructor
          *
          */
        virtual ~DataPointStore();

        /**
          * loadItems
          * 
          * Starts a thread to load the data from the database. This function
          * returns immediately.
          */
        virtual void loadItems();

        /**
          * getAllItems
          * 
          * Returns a list of names of the loadable data points.
          *
          * @return std::vector<std::string> a vector containing data point names
          */
        virtual std::vector<std::string> getAllItems();

        /**
          * getItem
          * 
          * Return one item. Composite Store will manage the
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
          * retrieveFromDatabase
          * 
          * This method executes the sql query to retrieve the items from the database.
          */
        virtual void retrieveFromDatabase();
        
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

        /**
          * processEntityUpdateEvent
          * 
          * This method satisfies the IEntityUpdateEventProcessor interface. This method is called when
          * the datapoint that a proxy has been set up for has been updated. When this is called, it tells
          * us that datapoint configuration has been initialised and it is now ok to retrieve datapoint
          * information from the proxy.
          *
          * @param entityKey - pkey of the data point the update is for
          * @param updateType - the type of update this is
          */
        void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
        
    private:

        DataPointStore( const DataPointStore& theDataPointStore);
        DataPointStore& operator=(const DataPointStore &);

        std::string retrieveTypeIdentifier();

        void processAndStoreItem(unsigned long entityKey, const std::string& entityName, const std::string& dataPointType,
            double engineeringLimitLow, double engineeringLimitHigh, const std::string& unitLabel, int precision, const std::string& agentName, unsigned long locationKey);

        TA_Base_Core::IData* getData(TA_Base_Core::IDatabase* databaseConnection, const TA_Base_Core::SQLStatement& query, std::vector<std::string> columnNames);

        
    private:

        static const char* DRLP_DATA_POINT_IDENTIFIER;

        std::map<std::string, boost::shared_ptr<GenericItem> > m_dataPoints;    // Store of Generic Items representing data points.
        std::auto_ptr<ItemLoaderThread> m_itemLoaderThread;                     // Thread that loads the items from the datbase
        TA_Base_Core::ReEntrantThreadLockable m_lock;
        unsigned long m_boolCount;
        bool m_isTerminating;

        std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_dataPointProxyMap;   // Maps each datapoint proxy created to the entity key
                                                                                            // of the datapoint.
    };
}

#endif // !defined(DataPointStore_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
