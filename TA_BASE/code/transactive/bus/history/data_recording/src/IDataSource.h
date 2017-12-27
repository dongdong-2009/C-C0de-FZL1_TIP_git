#ifndef IDATA_SOURCE_H
#define IDATA_SOURCE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/IDataSource.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Interface for an object that supplies data to
 * an IDataSink implementation.
 *
 */

#include <set>

#include "bus/history/data_recording/src/IDataSink.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class IDataSource
    {

    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IDataSource() {}


        /**
         * startRecording
         *
         * Tells the IDataSource that the IDataSink
         * is enabled for recording data.
         *
         */
        virtual void startRecording() = 0;


        /**
         * stopRecording
         *
         * Tells the IDataSource that the IDataSink
         * is disabled for recording data.
         *
         */
        virtual void stopRecording() = 0;

    
        /**
         * setDataSink
         *
         * Assigns an IDataSink for the IDataSource to
         * supply data to
         *
         */
        virtual void setDataSink( IDataSink& sink ) = 0;
    };

    typedef boost::shared_ptr<IDataSource> IDataSourcePtr;
}

#endif // IDATA_SOURCE_H
