/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * All exceptions in the Scheduling Agent project extend from
  * SchedulingException, which itself extends from TransactiveException.
  *
  */


#ifndef SCHEDULING_EXCEPTION_H
#define SCHEDULING_EXCEPTION_H

#include <string>
#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_App
{

class SchedulingException: public TA_Base_Core::TransactiveException
{

public:

    SchedulingException( const std::string & message )
    : m_message( message.c_str() )
    {
    }

    SchedulingException( const SchedulingException & rhs )
    {
        if(this != & rhs)
        {
            m_message = rhs.m_message ;
        }
    }

    SchedulingException()
    {
        m_message = "";
    }

    virtual ~SchedulingException() throw () {}

    virtual const char *what() const throw ()
    {
        return m_message.c_str();
    }

private:

    std::string m_message;
};


}; // TA_Base_App

#endif // SCHEDULING_EXCEPTION_H
