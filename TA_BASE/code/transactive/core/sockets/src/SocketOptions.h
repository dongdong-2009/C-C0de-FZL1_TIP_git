/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/SocketOptions.h $
  * @author:  A. Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Represents a collection of various socket options.
  * Ensures only one instance of a particular option is set, ie there is not two
  * occurences of TCPNoDelay etc.
  * Sets all options given a socket ID.
  *
  */

#if !defined(SOCKET_OPTIONS_H)
#define SOCKET_OPTIONS_H

#include "AbstractSocketOption.h"

#include <vector>

namespace TA_Base_Core
{

    class SocketOptions
    {

    public:

        /**
         * Default constructor.
         * Construct the socket options object with no options to set.
         */
	    SocketOptions();

        /**
         * Destructor.
         */
        virtual ~SocketOptions();

        /**
         * Given a socket option, add it to the list to be set.
         * Order is preserved, so for example if two conflicting options are set,
         * TCP_NODELAY, then TCP_CORK, TCP_CORK will be the second set, and will hold
         * its value, because it will override TCP_NODELAY, this reverses when they are
         * added in a different order.
         *
         * Example: options.addOption(new TA_Base_Core::TcpNoDelay(true));
         *
         * @param option    The pointer to the socket option to be set. It must be created
         *                  on the heap, it becomes the responsibility of this class to delete it.
         *
         */
	    void addOption(AbstractSocketOption* option);

        /**
         * Applies all socket options that have been set to the given socket ID.
         *
         * @param socketId  The ID of the socket to set options on.
         *
         */
	    void apply(int socketId) const;

        SocketOptions (const SocketOptions &rhs);

    private:
        SocketOptions &operator= (const SocketOptions &rhs);

        std::vector<AbstractSocketOption*> m_optionPointers;
        

    };

}

#endif // !defined(SOCKET_OPTIONS_H)
