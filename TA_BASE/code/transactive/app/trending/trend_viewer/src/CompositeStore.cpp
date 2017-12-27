#if !defined(CompositeStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
#define CompositeStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_

/**
  * CompositeStore.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/CompositeStore.cpp $
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

#pragma warning(disable: 4355)  // 'this' : used in base member initializer list

#include "app/trending/trend_viewer/src/CompositeStore.h"
#include "app/trending/trend_viewer/src/DataPointStore.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/TrendViewerException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    int CompositeStore::TOTAL_LOADERS = 1;

    //
    // CompositeStore
    //
    CompositeStore::CompositeStore(ILoaderObserver* loaderObserver) 
        : m_dataPointStore(new DataPointStore(this)),
          m_completedLoads(0),
          m_loaderObserver(loaderObserver)
    {
        FUNCTION_ENTRY("CompositeStore");

        FUNCTION_EXIT;
    }

    
    //
    // ~CompositeStore
    //
    CompositeStore::~CompositeStore() 
    {
        FUNCTION_ENTRY("~CompositeStore");
		DataPointStore *pStore = m_dataPointStore.release();
		delete pStore;
		pStore = NULL;
        FUNCTION_EXIT;
    }

    
    //
    // loadItems
    //
	void CompositeStore::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        m_dataPointStore->loadItems();

        FUNCTION_EXIT;
    }

    
    //
    // getAllItems
    //
    std::vector<std::string> CompositeStore::getAllItems()
    {
        FUNCTION_ENTRY("getAllItems");

        // Retrieve the data point items
        std::vector<std::string> items = m_dataPointStore->getAllItems();

        FUNCTION_EXIT;  
        return items;
    }   
    
    
    //
    // getItem
    //
    GenericItem* CompositeStore::getItem(const std::string& itemName)
    {
        FUNCTION_ENTRY("getItem");
        
        try
        {
            // Search the data point store first
            return m_dataPointStore->getItem(itemName);
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            if(ex.getType() == TA_Base_Core::TrendViewerException::ITEM_NOT_FOUND)
            {
                // Item wasn't found in the data point store.
				std::string msg("Data point store does not contain: ");
				msg += itemName;
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::ITEM_NOT_FOUND, msg.c_str()));
            }
        }
        catch(...)
        {
			// If execution gets down here, an unexpected exception had been caught while attempting to
			// retrieve the item.
			std::string msg("An unexpected error occurred while retrieving: ");
			msg += itemName;
			TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::ITEM_NOT_FOUND, msg.c_str()));
        }
		// This won't get called, it's just to keep the compiler warning-free
		return 0;
    }
     

    //
    // loadItems
    //
    void CompositeStore::loadItems(std::vector<std::string> itemNames)
    {
        FUNCTION_ENTRY("loadItems");

        m_dataPointStore->loadItems(itemNames);

        FUNCTION_EXIT;
    }


    //
    // loadingComplete
    //
    void CompositeStore::loadingComplete()
    {
        FUNCTION_ENTRY("loadingComplete");

        TA_Base_Core::ThreadGuard guard(m_lock);

        ++m_completedLoads;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Completed load: %d", m_completedLoads);

        if(m_completedLoads == TOTAL_LOADERS)
        {
            // Notify the GUI that loading is now complete
            m_loaderObserver->loadingComplete();
        }

        FUNCTION_EXIT;
    }


    //
    // processError
    //
    void CompositeStore::processError(const std::string& error)
    {
        FUNCTION_ENTRY("processError");

        // Notify the GUI that there has been a loading error
        m_loaderObserver->processError(error);

        FUNCTION_EXIT;
    }

    
    //
    // setStateLabels
    //
    void CompositeStore::setStateLabels(IGenericItem* genericItem)
    {
        FUNCTION_ENTRY("setStateLabels");
        
        m_dataPointStore->setStateLabels(genericItem);

        FUNCTION_EXIT;
    }

}

#endif // !defined(CompositeStoreCpp_C9C9EACC_65F9_4a9f_801F_9902C4D201D9__INCLUDED_)
