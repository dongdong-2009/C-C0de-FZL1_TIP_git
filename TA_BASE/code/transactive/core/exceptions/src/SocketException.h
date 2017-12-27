/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SocketException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
    class SocketException : public TA_Base_Core::TransactiveException  
    {
    public:
        SocketException() throw();
        SocketException(const std::string& msg) throw();
        virtual ~SocketException() throw();
    };
}

#endif
