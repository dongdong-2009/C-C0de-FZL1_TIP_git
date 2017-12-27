/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/BackgroundCommsObserver.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  */

#ifdef WIN32
#pragma warning (disable : 4786)

#include <io.h>

#endif // #ifdef WIN32

#include <fcntl.h>

#include "bus/generic_server_simulator/src/BackgroundCommsObserver.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

// defined in ConsoleDetachUtility.cpp
extern int pipe_in[2];
extern int pipe_out[2];
extern int pipe_err[2];

#if defined( WIN32 )
#define PipeWrite(handle,buffer,count) _write(handle,buffer,count)
#else // !defined( WIN32 )
#define PipeWrite(handle,buffer,count) write(handle,buffer,count)
#endif // !defined( WIN32 )


BackgroundCommsObserver::BackgroundCommsObserver(GssCommsObserver::ObservedSocketType * socket)
: TelnetCommsObserver(socket)
{
}

BackgroundCommsObserver::~BackgroundCommsObserver()
{
}

void BackgroundCommsObserver::processLine(const TelnetBuffer & line)
{
    if ( ( line == reinterpret_cast<const unsigned char *>("logout\n") ) || ( line == reinterpret_cast<const unsigned char *>("bye\n") ) )
    {
        // kill this comms observer
        getSocket()->terminate();
        return;
    }

    //
    // otherwise, go ahead and process the line
    PipeWrite(pipe_in[WriteEnd], line.c_str(), line.size());

    TelnetCommsObserver::processLine(line);
}


