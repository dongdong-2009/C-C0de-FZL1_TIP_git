#ifndef DATA_RECORDING_EXCEPTION_H
#define DATA_RECORDING_EXCEPTION_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataRecordingException.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Exception used internally within Data Recording
 *
 */

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class DataRecordingException : public TA_Base_Core::TransactiveException
    {
    public:

        DataRecordingException( const std::string& what )
        : TransactiveException( what )
        {
            // nothing else
        }
    };
}

#endif // DATA_RECORDING_EXCEPTION_H
