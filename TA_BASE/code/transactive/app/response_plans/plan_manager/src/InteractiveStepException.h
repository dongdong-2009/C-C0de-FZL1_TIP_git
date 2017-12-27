/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStepException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Thrown to indicate problems while executing an interactive step.
  */

#if !defined(AFX_INTERACTIVESTEPEXCEPTION_H__08B2828B_7A4A_41F3_9C3B_91902B8E6465__INCLUDED_)
#define AFX_INTERACTIVESTEPEXCEPTION_H__08B2828B_7A4A_41F3_9C3B_91902B8E6465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanManagerException.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // InteractiveStepException

    class InteractiveStepException : public PlanManagerException
    {
    // Operations
    public:
        /**
          * Constructor (default)
          *
          */
        InteractiveStepException() throw();

        /**
          * Constructor (with message only)
          *
          * Construct exception and include message.
          *
          * @param    std::string msg    -    The message
          */
        InteractiveStepException(std::string msg) throw();

        /**
          * Constructor (with message and detail only)
          *
          * Construct exception and include message and detail.
          *
          * @param    std::string msg        -    The message
          *
          * @param    std::string detail    -    The additional detail String
          */
        InteractiveStepException(std::string msg, std::string detail) throw();

        /**
          * Constructor (with message, detail, and comment)
          *
          * Construct exception and include message and detail.
          *
          * @param    std::string msg        -    The message
          *
          * @param    std::string detail    -    The additional detail String
          *
          * @param    std::string comment    -    The additional comment String
          */
        InteractiveStepException(std::string msg, std::string detail, std::string comment) throw();

        /**
          * Destructor
          *
          */
        virtual ~InteractiveStepException() throw();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_INTERACTIVESTEPEXCEPTION_H__08B2828B_7A4A_41F3_9C3B_91902B8E6465__INCLUDED_)
