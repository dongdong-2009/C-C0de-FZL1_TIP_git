#if !defined(IItemStore_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
#define IItemStore_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_

/**
  * IItemStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IItemStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between any class that needs retrieve items from the database
  * and an actual Item Store. Methods will be called on this interface to load items from the
  * database and to retrieve the items as a string or a GenericItem object.
  *
  */

#include <string>

#include "app/trending/trend_viewer/src/GenericItem.h"
#include "app/trending/trend_viewer/src/GraphedItem.h"

namespace TA_Base_App
{
    //
    // IItemStore
    //
    class IItemStore
    {

    public:

        /**
          * ~IItemStore
          * 
          * Destructor.
          */
        virtual ~IItemStore() {}

        /**
          * loadItems
          * 
          * Load all the items from the database.
          */
        virtual void loadItems() =0;
        
        /**
          * getAllItems
          * 
          * Return a vector of all the items.
          *
          * @return std::vector<std::string> a vector containing all the items.
          */
        virtual std::vector<std::string> getAllItems() =0;
        
        /**
          * getItem
          * 
          * Return one item
          *
          * @param name - name of the item to retrieve
          *
          * @return GenericItem* - pointer to the generic item object
          */
        virtual GenericItem* getItem(const std::string& name) =0;

        /**
          * loadItems
          * 
          * Load all the items from the database.
          *
          * @param itemNames  - vector of the names of the items to load
          */
        virtual void loadItems(std::vector<std::string> itemNames) =0;

        /**
          * setStateLabels
          * 
          * For some items, string labels are displayed on the y-axis instead of numerical
          * values. This method retrieves/initialises the state labels for an item that
          * is graphed.
          *
          * @param genericItem - the item that is graphed
          */
        virtual void setStateLabels(IGenericItem* genericItem) =0;
    };
}

#endif // !defined(IItemStore_BB300304_B20E_4065_985E_0D23388D96A4__INCLUDED_)
