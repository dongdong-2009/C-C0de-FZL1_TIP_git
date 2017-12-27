#ifndef EXCEPTION_ON_WRITE_ERROR_POLICY_H
#define EXCEPTION_ON_WRITE_ERROR_POLICY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/ExceptionOnWriteErrorPolicy.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of IDataWritePolicy that throws
 * a DataRecordingException when an error occurs.
 *
 */

#include "bus/history/data_recording/src/IDataWritePolicy.h"
#include "bus/history/data_recording/src/DataCache.h"

namespace TA_Base_Bus
{
    class ExceptionOnWriteErrorPolicy : public IDataWritePolicy
    {

    public:

        /**
         * Constructor
         *
         */
        ExceptionOnWriteErrorPolicy();


        /**
         * Destructor
         *
         */
        ~ExceptionOnWriteErrorPolicy();


        /**
         * notifyDataWriteSuccess
         *
         * Callback from the DataRecorder when data is written
         * successfully. Does nothing.
         *
         */
        virtual void notifyDataWriteSuccess();


        /**
         * notifyDataWriteError
         *
         * Callback from the DataRecorder when an error is
         * detected attempting to write data. Throws a
         * DataRecordingException.
         *
         * @param   unwrittenCache     the data that the DataRecorder failed to write
         *
         */
        virtual void notifyDataWriteError( DataCachePtr& unwrittenCache );

    };
}

#endif // EXCEPTION_ON_WRITE_ERROR_POLICY_H
