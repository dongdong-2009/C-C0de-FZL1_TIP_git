/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/corba/src/VisitType.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Allows the component parts of a Corba type to be visited.  For example, this could
  * be used to determine the structure of a Corba Any.
  */

#ifndef VISIT_TYPE_H
#define VISIT_TYPE_H

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/corba/src/CorbaUtil.h"
#include "omniORB4/CORBA.h"

namespace TA_Base_Core
{
    // Recursively navigates the given TypeCode, returning the structure and values
    // formatted as a string.  The string can be formatted with each element on a
    // separate line (if multiline == true) or as a single line (if multiline == false).
    // To use, a type code needs to be obtained - for example:
    //  CORBA::Any myAny;
    //  CORBA::TypeCode_var tc;
    //  tc = myAny.type();
    //  std::string str = gTypeCodeToStr(tc, true);

    std::string gTypeCodeToStr(CORBA::TypeCode_ptr tcp, bool multiline);

}; // namespace TA_Base_Core

#endif // VISIT_TYPE_H
