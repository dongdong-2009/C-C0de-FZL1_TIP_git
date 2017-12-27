#if !defined(IArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define IArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class contains the database retrievals and writes for Camera tables
  * for the Video Switch Agent.
  */

// KT, 26 Feb 04, PW# 3133: Deletion Due Date has been added.
// KT, 26 Feb 04, PW# 3135: References to Start Date and End Date have been replaced with Data Date.

#include <string>
#include <map>
#include <ctime>

namespace TA_Base_Core
{
    class IArchiveInventory
    {
    public:
        //
        // Struct defining an entry in the AR_TABLES table.
        //

        // NB: The time_t's have the year from 1900 and the months go 00->11 not 01->12.

        struct ArchiveInventoryItem
        {
            unsigned long   key;            // The unique id of the inventory entry.
            time_t          dataDate;       // The date of the archive.
            std::string     volumeLabel;    // The name given to the archive.
            bool            isDeleting;     // Set to true if the data is being deleted.
            time_t          restoredDate;   // The date that the data was restored.
            std::string     restoredBy;     // The username of the person who restored the data.
            time_t          deletionDueDate;// The date that the data should be deleted if it is restored data.
        };

        //
        // Map of the tableKey to the table information.
        //

        typedef std::map< unsigned long, ArchiveInventoryItem > ArchiveInventoryItemMap;

        /**
         * ~IArchiveInventory
         *
         * Standard destructor.
         */

        virtual ~IArchiveInventory() {};
        
        /**
         * getAllArchiveInventoryItems
         *
         * Gets all the inventory items to be archived.
         *
         * @return      ArchiveInventoryItemMap
         *              The map of inventory item indices to names.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        virtual ArchiveInventoryItemMap getAllArchiveInventoryItems() = 0;

        /**
         * getArchiveInventoryItem
         *
         * Gets the data for the inventory item with the given key.
         *
         * @return      ArchiveInventoryItem
         *              The data for the inventory item.
         *
         * @param       unsigned long inventoryItemKey
         *              The unique id of the inventory item.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The inventory item key was not found.
         *              2. The data cannot be converted to the correct format.
         */

        virtual ArchiveInventoryItem getArchiveInventoryItem( unsigned long inventoryItemKey ) = 0;

        /**
         * setArchiveInventoryItem
         *
         * Sets the data for the inventory item with the given key.  If the key does
         * not exist in the database already, the key is added.  If it already
         * exists, the existing entry is updated.
         *
         * @param       const ArchiveInventoryItem& inventoryItem
         *              The data for the inventory item.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        virtual void setArchiveInventoryItem( const ArchiveInventoryItem& inventoryItem ) = 0;

        /**
         * addArchiveInventoryItem
         *
         * Adds the given inventory item data.
         *
         * @param       ArchiveInventoryItem& inventory item
         *              The data for the inventoryItem.
         *              If the item does not already exist, the key of the inventoryItem 
         *              will be set with its new value.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. An inventory item with the given key already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual void addArchiveInventoryItem( ArchiveInventoryItem& inventoryItem ) = 0;
                
        /**
         * removeArchiveInventoryItem
         *
         * Removes the given inventory item data from the database.
         *
         * @param       unsigned long itemKey
         *              The unique id for the inventory item.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. An inventory item with the given key already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual void removeArchiveInventoryItem( unsigned long itemKey ) = 0;

        /**
         * isDateInInventory
         *
         * Returns true if the data for a given date has been restored (ie is in 
         * the inventory list).
         *
         * @return      bool
         *              True if the data has been restored.
         * 
         * @param       const time_t& date
         *              The date to check.
         * 
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. An inventory item with the given key already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual bool isDateInInventory( const time_t& date ) = 0;
        
        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate() = 0;
    };

} //end namespace TA_Base_Core

#endif // !defined(IArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
