#ifndef RECORDABLE_ITEM_ACCESS_FACTORY_H
#define RECORDABLE_ITEM_ACCESS_FACTORY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <vector>
#include <string>

#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class RecordableItemAccessFactory
    {

    public:

        /** 
         * Constant strings for building SQL statements
         */ 
        static const std::string VIEW_NAME;
        static const std::string FIELD_ID;
        static const std::string FIELD_TYPEKEY;
        static const std::string FIELD_ENABLED;
        static const std::string VALUE_ENABLED;


        /**
         * getInstance
         *
         */
        static RecordableItemAccessFactory& getInstance();


        /**
         * getRecordableItem
         *
         */
        IRecordableItemPtr getRecordableItem( unsigned long itemId );


        /**
         * getRecordableItems
         *
         */
        std::vector<IRecordableItemPtr> getRecordableItems();


        /**
         * getRecordableItemsByType
         *
         */
        std::vector<IRecordableItemPtr> getRecordableItemsByType( unsigned long typeKey );


    private:

        /**
         * Constructor
         *
         */
        RecordableItemAccessFactory();


        /**
         * Destructor
         *
         */
        ~RecordableItemAccessFactory();


        /**
         * Prevent copying
         */
        RecordableItemAccessFactory( const RecordableItemAccessFactory& copyMe );
        RecordableItemAccessFactory& operator=( const RecordableItemAccessFactory& copyMe );


        static RecordableItemAccessFactory*         s_singleton;
        static TA_Base_Core::NonReEntrantThreadLockable  s_singletonLock;

    };
}

#endif // RECORDABLE_ITEM_ACCESS_FACTORY_H
