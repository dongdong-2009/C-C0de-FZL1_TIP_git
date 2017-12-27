#ifndef DB_DATA_ACCESS_H
#define DB_DATA_ACCESS_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataRecorderDbDataAccess.h $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides an interface for accessing data recording items from the database. IRecordableData
 * (located in the data recording dai) is an interface to the actual objects that have can have trending
 * enabled, eg. Datapoints, live properties. IRecordableItem (located in data recording dai) is an
 * interface to the objects that actually have trend enabled set to true.
 *
 */

#include <ctime>
#include <string>

#include "bus/history/data_recording/src/IDataRecorderDbAccess.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"


namespace TA_Base_Bus
{
    class DataRecorderDbDataAccess : public IDataRecorderDbAccess
    {

    public:

        /**
         * Constructor
         *
         */
        DataRecorderDbDataAccess() {}


        /**
         * Destructor
         *
         */
        virtual ~DataRecorderDbDataAccess() {}


        /************************************************
         * 
         * IDataRecorderDbAccess functions
         * 
         ************************************************/

        /**
         * write 
         *
         * @param   typeKey    the type key of the recordable item
         *
         */
        virtual std::vector<TA_Base_Core::IRecordableDataPtr> getRecordableDataByType( unsigned long typeKey );


        virtual std::vector<TA_Base_Core::IRecordableItemPtr> getRecordableItemsByType( unsigned long typeKey );

    };
}

#endif // DB_DATA_ACCESS_H
