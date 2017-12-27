/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/TcpNoDelay.cpp $
  * @author:  A. Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */



// include the necessary files for setting socket options
#if defined( WIN32 )
#include <winsock2.h>
#else
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netinet/tcp.h>
	#if defined(LINUX)
	#include    <netinet/in.h>
	#endif
#endif // !WIN32

#include "TcpNoDelay.h"
#include "core/utilities/src/DebugUtil.h"
/*
 * Construct the option and optionally specify a value
 * set the member variable to the specified value.
 */
TA_Base_Core::TcpNoDelay::TcpNoDelay(bool enable /*=false*/)
: m_enable(enable)
{
}

/*
 * Copy this object (so it can be passed by value)
 */
TA_Base_Core::AbstractSocketOption* TA_Base_Core::TcpNoDelay::clone() const
{
    // create a new object with the same value as this
    return (new TcpNoDelay(m_enable));
}

/*
 * Apply the settings tio the given socket.
 */
void TA_Base_Core::TcpNoDelay::apply(int socketId) const
{
    // log that we are changing socket options
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
        "Setting TCP_NODELAY socket option for socket ID %d to %d", socketId, m_enable);

    // used to collect the result
    int result = 0;

#if defined( WIN32 )
// WINDOWS

    // result will hold the error code if non zero
    result = setsockopt( socketId, IPPROTO_TCP, TCP_NODELAY,
               ( char * )& m_enable,  sizeof( m_enable ) );

#else
// LINUX AND UNIX
    
    // convert the boolean value to an int
    int value = (m_enable)?1:0;

    // result will be zero or -1
    result = setsockopt( socketId, IPPROTO_TCP, TCP_NODELAY,
               & value, sizeof(value) );

#endif // WIN32

    // if there was an error, log the fact, and give the code as well
    if (result != 0)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
        "Setting TCP_NODELAY socket option for socket ID %d to %d failed with code %d ", socketId, m_enable, result);
    }
}

