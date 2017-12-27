#ifndef IRECORDABLE_DATA_H
#define IRECORDABLE_DATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/IRecordableData.h $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Interface for a data recording object that has data that is written to the database.
 *
 */
#include <string>

#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class IRecordableData
    {

    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IRecordableData() {}

		/**
         * getName
         *
         * @return  the name for this recordable object.
         *
         */
        virtual std::string getName() = 0;
	
        /**
         * getIdentifier
         *
         * @return  the identifier for this recordable object.
         *
         */
        virtual std::string getIdentifier() = 0;
                 
        /**
         * getTable
         *
         * @return  the table name for this recordable object.
         *
         */
        virtual std::string getTable() = 0;
                 
        /**
         * getColumn
         *
         * @return  the column for this recordable object.
         *
         */
        virtual std::string getColumn() = 0;
                 
        /**
         * getStatus
         *
         * @return  the status for this recordable object.
         *
         */
        virtual std::string getStatus() = 0;
        
        /**
         * getIntervalBetweenWrites
         *
         * @return  the interval between writes for this recordable object.
         *
         */
        virtual unsigned long getIntervalBetweenWrites() = 0;
        

        static const char* STATUS_AVAILABLE;
    };

    typedef boost::shared_ptr<IRecordableData> IRecordableDataPtr;
}

#endif // IRECORDABLE_DATA_H
