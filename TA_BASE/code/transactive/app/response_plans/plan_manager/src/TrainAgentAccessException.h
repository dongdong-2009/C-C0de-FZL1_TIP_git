/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainAgentAccessException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Derived TransActive Exception, adds detail and comment members, along with related
  * accesor methods.
  */

#if !defined(AFX_TRAINAGENTACCESSEXCEPTION_H__D922D21F_E4B2_4CC5_97C8_11E06163AB72__INCLUDED_)
#define AFX_TRAINAGENTACCESSEXCEPTION_H__D922D21F_E4B2_4CC5_97C8_11E06163AB72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanManagerException.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TrainAgentAccessException

    class TrainAgentAccessException : public PlanManagerException
    {
    // Operations
    public:
        /**
          * Constructor (default)
          *
          */
        TrainAgentAccessException() throw();

        /**
          * Constructor (with message only)
          *
          * Construct exception and include message.
          *
          * @param    std::string msg    -    The message
          */
        TrainAgentAccessException(std::string msg) throw();

        /**
          * Constructor (with message and detail only)
          *
          * Construct exception and include message and detail.
          *
          * @param    std::string msg        -    The message
          *
          * @param    std::string detail    -    The additional detail String
          */
        TrainAgentAccessException(std::string msg, std::string detail) throw();

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
        TrainAgentAccessException(std::string msg, std::string detail, std::string comment) throw();

        /**
          * Destructor
          *
          */
        virtual ~TrainAgentAccessException() throw();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_TRAINAGENTACCESSEXCEPTION_H__D922D21F_E4B2_4CC5_97C8_11E06163AB72__INCLUDED_)
