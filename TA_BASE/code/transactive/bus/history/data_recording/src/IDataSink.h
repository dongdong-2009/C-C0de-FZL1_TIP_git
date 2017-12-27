#ifndef IDATA_SINK_H
#define IDATA_SINK_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/IDataSink.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Interface for an object to store data retrieved
 * by IDataSource implementations.
 *
 */

#include <vector>

#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class Datum;

    class IDataSink
    {
    
    public:

        /**
         * Quality ratings for recorded data. This MUST
         * be in order of ascending quality.
         *
         */
        enum EQuality
        {
            BAD_QUALITY,
            GOOD_QUALITY
        };

        /**
         * Virtual destructor
         *
         */
        virtual ~IDataSink() {}


        /**
         * notifyData
         *
         * Callback for IDataSource implementations to update the data being
         * recorded for one Item.
         *
         * @param   id          unique identifier for Item
         * @param   timestamp   source time for data
         * @param   data        the data
         * @param   quality     confidence level in integrity of data
         *
         */
        virtual void notifyData( unsigned long id, time_t timestamp, const std::vector<Datum>& data, int quality ) = 0;


        /**
         * setMinimumAcceptableQuality
         *
         * @param   quality     the minimum quality for data to be
         *                      stored by the Sink. Data of lesser quality
         *                      will be ignored.
         *
         */
        virtual void setMinimumAcceptableQuality( int quality ) = 0;

    };

    typedef boost::shared_ptr<IDataSink> IDataSinkPtr;
}
#endif // IDATA_SINK_H
