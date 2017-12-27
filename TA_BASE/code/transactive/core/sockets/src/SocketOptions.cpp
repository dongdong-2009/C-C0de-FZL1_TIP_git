/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/SocketOptions.cpp $
  * @author:  A. Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */

#include "SocketOptions.h"

#include "core/utilities/src/DebugUtil.h"

TA_Base_Core::SocketOptions::SocketOptions()
{
	m_optionPointers.clear();
}


/*
 * Perform a deep copy
 */
TA_Base_Core::SocketOptions::SocketOptions (const SocketOptions &rhs)
{
	m_optionPointers.clear();

	std::vector < AbstractSocketOption * >::const_iterator i;
    for ( i = rhs.m_optionPointers.begin(); i != rhs.m_optionPointers.end(); i++ )
    {
		AbstractSocketOption * option = 0;
		option = (*i);

		if ( 0 != option )
		{
			// add a copy of the socket option
			m_optionPointers.push_back ( option->clone() );
		}
    }
}


TA_Base_Core::SocketOptions::~SocketOptions()
{
    // delete all options
	std::vector < AbstractSocketOption * >::iterator i;
    for ( i = m_optionPointers.begin(); i != m_optionPointers.end(); i++ )
    {
        delete (*i);
		(*i) = 0;
    }
	m_optionPointers.clear();
}


/*
 * Given a socket option, add it to the list to be set.
 */
void TA_Base_Core::SocketOptions::addOption(AbstractSocketOption* option)
{
	if ( 0 != option )
	{
		// add the new option
		m_optionPointers.push_back(option);
	}
}


/*
 * Applies all socket options that have been set to the given socket ID.
 */
void TA_Base_Core::SocketOptions::apply(int socketId) const
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Setting TCP socket options for socket ID %d", socketId);

    // apply all options
	std::vector < AbstractSocketOption * >::const_iterator i;
    for ( i = m_optionPointers.begin(); i != m_optionPointers.end(); i++ )
    {
		AbstractSocketOption * option = 0;
		option = (*i);

		if ( 0 != option )
		{
			option->apply(socketId);
		}
    }
}
