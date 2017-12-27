/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/IncidentException.cpp $
  * @author San Teo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * Incident Manager. It is derived from TransactiveException.
  * This exception class will encompass all errors that
  * can possibly occur.
  */

#include "core\exceptions\src\IncidentException.h"
#include "core\utilities\src\TAAssert.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //////////////////////////////////////////////////////////////////////
    // Constants
    //////////////////////////////////////////////////////////////////////

     const std::string IncidentException::ACCESS_DENIED_STRING = "Operator has insufficient privilege to perform action";
     const std::string IncidentException::DATABASE_ERROR_STRING = "Unable to contact the database or encountered some kind of db error";
     const std::string IncidentException::DATA_ERROR_STRING = "Incident Manager is not configured correctly in the database";
     const std::string IncidentException::INCIDENT_EXIST_STRING = "The alarm is already associated with an incident";
     const std::string IncidentException::ALARM_NOT_EXIST_STRING = "The alarm that is associated with this incident does not exist";
     const std::string IncidentException::PLAN_NOT_EXIST_STRING = "The plan that is associated with this incident does not exist";
     const std::string IncidentException::ASSET_NOT_EXIST_STRING = "The asset that is associated with this incident does not exist";
     const std::string IncidentException::LOCATION_NOT_EXIST_STRING = "The location that is associated with this incident does not exist";
     const std::string IncidentException::INCIDENT_NOT_EXIST_STRING = "The incident does not exist";
     const std::string IncidentException::GENERAL_ERROR_STRING = "Some unknown error has occurred";

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////


    IncidentException::IncidentException(const EFailType failType) 
        : m_failType(failType),
          TransactiveException()
    {
        logException();
    }	

    IncidentException::IncidentException(const std::string& what, const EFailType failType)
        : m_failType(failType),
          TransactiveException(what)
    {
        logException();
    }	

    IncidentException::~IncidentException()
    {

    }

    //////////////////////////////////////////////////////////////////////
    // Member Functions
    //////////////////////////////////////////////////////////////////////

    void IncidentException::logException()
    {
        switch(m_failType)
        {
        case (ACCESS_DENIED):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", ACCESS_DENIED_STRING.c_str());
	        break;

        case (DATABASE_ERROR):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", DATABASE_ERROR_STRING.c_str());
	        break;

        case (DATA_ERROR):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", DATA_ERROR_STRING.c_str());
	        break;

        case (INCIDENT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", INCIDENT_EXIST_STRING.c_str());
	        break;

        case (ALARM_NOT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", ALARM_NOT_EXIST_STRING.c_str());
	        break;

        case (PLAN_NOT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", PLAN_NOT_EXIST_STRING.c_str());
	        break;

        case (ASSET_NOT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", ASSET_NOT_EXIST_STRING.c_str());
	        break;

        case (LOCATION_NOT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", LOCATION_NOT_EXIST_STRING.c_str());
	        break;

        case (INCIDENT_NOT_EXIST):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", INCIDENT_NOT_EXIST_STRING.c_str());
	        break;

        case (GENERAL_ERROR):
	        LOG(SourceInfo, DebugUtil::ExceptionDetails, "IncidentException", GENERAL_ERROR_STRING.c_str());
	        break;
    
        default:
            TA_ASSERT(false,"Unknown Incident Exception Type");
        }
    }

} // End TA_Base_Core
