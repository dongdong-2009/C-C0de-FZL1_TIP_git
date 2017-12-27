/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/MatrixException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Thrown by Matrix when an attempt is made to access a nonexisting element.
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/MatrixException.h"


namespace TA_Base_Core
{
    MatrixException::MatrixException() throw()
    {
    }


    MatrixException::MatrixException(const std::string& msg) throw() :
        TransactiveException(msg)
    {
    }


    MatrixException::~MatrixException() throw()
    {
    }
}
