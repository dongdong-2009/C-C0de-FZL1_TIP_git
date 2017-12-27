#ifndef IRECORDABLE_ITEM_H
#define IRECORDABLE_ITEM_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/IRecordableItem.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <string>

#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class IRecordableItem
    {

    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IRecordableItem() {}

        
        /**
         * getTypeKey
         *
         * @return  the type key for this RecordableItem
         *
         */
        virtual unsigned long getTypeKey() = 0;


        /**
         * getId
         *
         * @return  the id for this RecordableItem
         *
         */
        virtual unsigned long getId() = 0;


        /**
         * isRecordingEnabled
         *
         * @return whether or not recording is enabled for the item
         *
         */
        virtual bool isRecordingEnabled() = 0;


        /**
         * invalidate
         *
         * Marks data values as potentially redundant and requiring
         * a refresh from the database.
         *
         */
        virtual void invalidate() = 0;
    };

    typedef boost::shared_ptr<IRecordableItem> IRecordableItemPtr;
}

#endif // IRECORDABLE_ITEM_H
