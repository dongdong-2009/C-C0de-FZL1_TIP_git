/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SystemControllerException.cpp $
  * @author:  <Rob Ashcroft>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Exception class used by the System Controller and
  * System Manager.
  */


#include "SystemControllerException.h"


namespace TA_Base_Core
{

	//
	// Default Constructor
	//
    SystemControllerException::SystemControllerException() throw()
    {

    }

	//
	// Constructor
	//
    SystemControllerException::SystemControllerException(const std::string& msg) throw() 
            : TA_Base_Core::TransactiveException(msg) , m_message(msg)
	        {

	        }
 
	//
	// Destructor
	//
    SystemControllerException::~SystemControllerException() throw()
    {

    }

	//
	// what
	//
    const char* SystemControllerException::what() const throw()
    {
        return m_message.c_str();
    }
    
};
