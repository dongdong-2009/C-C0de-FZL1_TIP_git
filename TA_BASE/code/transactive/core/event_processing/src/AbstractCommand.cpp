/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/AbstractCommand.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AbstractCommand implements functionality that is required of all scheduled commands,
  * namely the ability to retrieve the priority of the command, the delay before executing the command,
  * and the time at which the command was added to the scheduler.
  *
  * It also defines two function objects used to decide the order of the command objects in the
  * priority queues.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "ace/ACE.h"
#include "ace/OS_NS_sys_time.h"

#include "core/event_processing/src/AbstractCommand.h"

namespace TA_Base_Core
{
    AbstractCommand::AbstractCommand( const unsigned long delay ) :
        m_delay(delay)
    {
        // Calculate the time when the command should execute.
        m_scheduledTime = ACE_OS::gettimeofday()+ACE_Time_Value( 0, m_delay*1000 );
    }


    AbstractCommand::~AbstractCommand()
    {
    }


    unsigned long AbstractCommand::getDelay() const
    {
        return m_delay;
    }


    const ACE_Time_Value& AbstractCommand::getScheduledTime() const
    {
        return m_scheduledTime;
    }


    bool AbstractCommand::operator<( const AbstractCommand* rhs )
    {
        // ACE_Time_Value::operator< seems to return an int!
        return ( m_scheduledTime < rhs->m_scheduledTime )!=0;
    }


    bool AbstractCommand::operator<( const AbstractCommand& rhs )
    {
        // ACE_Time_Value::operator< seems to return an int!
        return ( m_scheduledTime < rhs.m_scheduledTime )!=0;
    }


    bool AbstractCommand::operator>( const AbstractCommand* rhs )
    {
        // ACE_Time_Value::operator> seems to return an int!
        return ( m_scheduledTime > rhs->m_scheduledTime )!=0;
    }


    bool AbstractCommand::operator>( const AbstractCommand& rhs )
    {
        // ACE_Time_Value::operator> seems to return an int!
        return ( m_scheduledTime > rhs.m_scheduledTime )!=0;
    }
};
