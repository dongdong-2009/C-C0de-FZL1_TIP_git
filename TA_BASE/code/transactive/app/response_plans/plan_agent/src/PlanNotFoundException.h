#ifndef PLANNOTFOUNDEXCEPTION_H_INCLUDED
#define PLANNOTFOUNDEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_App
{
    class PlanNotFoundException : public TA_Base_Core::TransactiveException
	{
	public:
        /** 
          * PlanNotFoundException
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param planId The node id of the plan
          * @param instanceId The instance number
          *
          */
        PlanNotFoundException(  const std::string& msg, const int error, const unsigned long planId = 0, const unsigned long instanceId = 0) throw();

		
        /** 
          * PlanNotFoundException
          *
          *
          */
        PlanNotFoundException() throw();
		
		
        /** 
          * getErrorType
          *
          * Returns the error type
          *
          *
          * @return the error type
          */
        const int getErrorType() const
        {
            return m_errorCode;
        }


        /** 
          * getPlanId
          *
          * Returns the Plan id associated with the error
          *
          *
          * @return the Plan Id associated with the error
          */
        const unsigned long getPlanId() const
        {
            return m_planId;
        }


        /** 
          * getInstanceId
          *
          * Returns the Instance id associated with the error
          *
          *
          * @return the Instance Id associated with the error
          */
        const unsigned long getInstanceId() const
        {
            return m_instanceId;
        }

		virtual ~PlanNotFoundException() throw();

	private:
        int m_errorCode;
        unsigned long m_planId;
        unsigned long m_instanceId;
	};

}; // TA_Base_App



#endif // PLANNOTFOUNDEXCEPTION_H_INCLUDED





