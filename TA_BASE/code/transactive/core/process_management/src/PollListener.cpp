/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/PollListener.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is used by ManagedProcess to respond to polls.
  * It declares a virtual method, poll(), that should just
  * return if the process is functioning correctly, and should
  * throw a PollException if a problem is detected. The
  * implementation of poll() in this class just returns, but this
  * class can be subclassed (and poll() overridden) to allow
  * a process to perform regular health checks while executing.
  *
  */

#include "PollListener.h"


namespace TA_Base_Core
{

    PollListener::PollListener()
    {
        // Nothing yet
    }

    void PollListener::poll()
    {
        // Just return to indicate that the process is functioning correctly.
        // This is the default functionality for all processes, but this method
        // can be overridden so that it can perform health checks for a specific
        // process.
        return;
    }

    PollListener::~PollListener()
    {
        // Nothing yet
    }

} // namespace TA_Base_Core
