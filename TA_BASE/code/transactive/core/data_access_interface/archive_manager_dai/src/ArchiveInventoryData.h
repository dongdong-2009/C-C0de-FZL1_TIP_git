#if !defined(ArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define ArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveInventoryData.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class contains the database retrievals and writes for Camera inventory items
  * for the Video Switch Agent.
  * It implements the IArchiveInventory interface.
  */

#include <map>
#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"

#include "core/utilities/src/DateFormat.h" // TD12474

namespace TA_Base_Core
{
    class ArchiveInventoryData : public IArchiveInventory
    {
    public:

        /**
         * ArchiveInventoryData
         *
         * Standard constructor.
         */

        ArchiveInventoryData();

        /**
         * ~ArchiveInventoryData
         *
         * Standard destructor.
         */

        virtual ~ArchiveInventoryData() {};
        
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

        virtual ArchiveInventoryItemMap getAllArchiveInventoryItems();

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

        virtual ArchiveInventoryItem getArchiveInventoryItem( unsigned long inventoryItemKey );

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

        virtual void setArchiveInventoryItem( const ArchiveInventoryItem& inventoryItem );

        /**
         * addArchiveInventoryItem
         *
         * Adds the given inventory item data or updates the item, if
         * it already exists.
         *
         * @param       ArchiveInventoryItem& inventoryItem
         *              The data for the inventory item.
         *              If the item does not already exist, the key of the inventoryItem 
         *              will be set with its new value.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The data cannot be converted to the correct format.
         */

        virtual void addArchiveInventoryItem( ArchiveInventoryItem& inventoryItem );

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
         *              1. A inventory item with the given key already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual void removeArchiveInventoryItem( unsigned long itemKey );
       
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
         *              1. A inventory item with the given key already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual bool isDateInInventory( const time_t& date );

        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate();

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        ArchiveInventoryData& operator=( const ArchiveInventoryData& );
        ArchiveInventoryData( const ArchiveInventoryData& );

        /**
         * reload
         *
         * This loads all ArchiveInventory from the database.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         *
         * @exception   DataException 
         *              Thrown if duplicate ArchiveInventory appear in the database.
         */

        void reload();

        /**
         * updateArchiveInventoryItemInDatabase
         *
         * This updates an ArchiveInventoryItem in the database.
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

        void updateArchiveInventoryItemInDatabase( const ArchiveInventoryItem& inventoryItem );

        /**
         * insertArchiveInventoryItemIntoDatabase
         *
         * This inserts a new inventory item into the database.
         *
         * @param       ArchiveInventoryItem& inventoryItem
         *              The data for the inventory item.
         *              The key of the inventoryItem will be set with its correct value.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        void insertArchiveInventoryItemIntoDatabase( ArchiveInventoryItem& inventoryItem );
 
        /**
         * removeArchiveInventoryItemFromDatabase
         *
         * Removes the inventory item with the given key from the database.
         *
         * @param       unsigned long itemKey
         *              The unique key of the item to delete.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         */
        
        void removeArchiveInventoryItemFromDatabase( unsigned long itemKey );

        //
        // Holds all the current, configured inventory items retrieved from the database.
        //

        IArchiveInventory::ArchiveInventoryItemMap m_inventory;

        //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //

        bool m_isValidData;

		TA_Base_Core::DateFormat m_DateFormat; // TD12474		
    };

} //end namespace TA_Base_Core

#endif // !defined(ArchiveInventoryData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
