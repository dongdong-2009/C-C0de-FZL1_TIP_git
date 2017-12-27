/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ReportManagerException.cpp $
  *  @author:  A. Lee
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  */

#include "ReportManagerException.h"

namespace TA_Base_Core
{
		ReportManagerException::ReportManagerException() throw()
		{
		}

		ReportManagerException::ReportManagerException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* ReportManagerException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		ReportManagerException::~ReportManagerException() throw()
		{
		}

} // TA_Base_Core
