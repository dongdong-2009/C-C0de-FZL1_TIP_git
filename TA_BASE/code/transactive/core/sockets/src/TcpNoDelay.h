/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/TcpNoDelay.h $
  * @author:  A. Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This represents the TCP_NODELAY option. It takes care of setting this option for
  * each target environment.
  * 
  * From man tcp (7)
  * If set, disable the Nagle algorithm. This means that segments are always sent
  * as soon as possible, even if  there is only a small amount of data. When not
  * set, data is buffered until there is a sufficient amount to send out, thereby
  * avoiding the frequent sending of small packets, which results in poor
  * utilization of the network. This option cannot be used at the same time as the
  * option TCP_CORK.
  *
  * This option is disabled by default, ie the Nagle algorithm is used.
  */

#if !defined(TCP_NO_DELAY_H)
#define TCP_NO_DELAY_H

#include "AbstractSocketOption.h"

#include <iostream>

namespace TA_Base_Core
{

    class TcpNoDelay : public AbstractSocketOption
    {

    public:

        /**
         * Default constructor with a default value.
         * If it is used with the default value, it will have no effect
         * since the default value is the default setting for windows,
         * solaris, and linux.
         * Although the set operation will still be performed just in case.
         *
         * @param enable    The value to set the TCP_NODELAY option to.
         */
        TcpNoDelay(bool enable = false);

        /**
         * Destructor
         */
        ~TcpNoDelay() {};

        /**
         * Apply the changes to the given socket ID
         * Uses setsockopt, the socket ID, and the value set in the constructor
         * to set the socket option.
         *
         * @param socketId  The ID of the socket to set the options on
         */
	    virtual void apply(int socketId) const;

        /**
         * Copy this object (so it can be passed by value)
         */
        virtual AbstractSocketOption* clone() const;

    private:

        /**
         *  This is the value to set the TCP_NODELAY option to
         */
        bool m_enable;

    };

}

#endif // !defined(TCP_NO_DELAY_H)
