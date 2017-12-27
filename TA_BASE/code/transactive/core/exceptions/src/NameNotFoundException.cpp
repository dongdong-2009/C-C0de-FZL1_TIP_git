/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/NameNotFoundException.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Used by TA_Naming, thrown when a CORBA object can-not be resolved
  */

#include "NameNotFoundException.h"

namespace TA_Base_Core
{
    NameNotFoundException::NameNotFoundException() throw()
    {

    }

    NameNotFoundException::NameNotFoundException(const std::string& msg) throw() 
        : TransactiveException(msg)
	{

	}

    NameNotFoundException::~NameNotFoundException() throw()
    {

    }
};
