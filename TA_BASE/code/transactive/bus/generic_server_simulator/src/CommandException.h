/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/CommandException.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  */

#ifndef CommandException_H
#define CommandException_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
        class CommandException : public TA_Base_Core::TransactiveException  
    {
    public:
        /**
          * Default constructor.
          */
        CommandException() throw();

        /**
          * Constructor which takes a message string as an argument.
          *
          * @param message  An exception message describing a problem.
          */
        CommandException( const std::string & message ) throw();

        /**
          * Destructor.
          */
        virtual ~CommandException() throw();
    };    
}

#endif // CommandException_H
