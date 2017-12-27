#ifndef PlanStepDetailException_H_INCLUDED
#define PlanStepDetailException_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/PlanStepDetailException.h $
  *  @author:  San Teo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class PlanStepDetailException : public TA_Base_Core::TransactiveException
	{
	public:

        /** 
          * PlanStepDetailException
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param type The type of step detail
          * @param stepId The step Id
          *
          */
        PlanStepDetailException(const std::string& msg, 
                                const TA_Base_Core::EPlanStepDetailType type,
                                const TA_Base_Core::EPlanStepDetailError error, 
                                const unsigned long stepId) throw();

		
        /** 
          * getErrorType
          *
          * Return the type of error.
          *
          *
          * @return the type of error
          */
        const TA_Base_Core::EPlanStepDetailError getErrorType() const
        {
            return m_errorCode;
        }


        /** 
          * getDetailType
          *
          * Return the detail which has the error
          *
          *
          * @return the detail which has the error
          */
        const TA_Base_Core::EPlanStepDetailType getDetailType() const
        {
            return m_type;
        }


        /** 
          * getStepNumber
          *
          * Return the step number for the plan that the error occurred in.
          *
          *
          * @return the step number for the plan.
          */
        const unsigned long getStepNumber() const
        {
            return m_stepNum;
        }


        virtual ~PlanStepDetailException() throw();
	private:

        /** 
          * PlanStepDetailException
          *
          * Force the exception to be constructed using the only constructor above
          *
          */
        PlanStepDetailException() throw();
		
        const TA_Base_Core::EPlanStepDetailError m_errorCode; 
        const TA_Base_Core::EPlanStepDetailType m_type;
        const unsigned long m_stepNum;
	};
}; // TA_Base_Core

#endif // PlanStepDetailException_H_INCLUDED



