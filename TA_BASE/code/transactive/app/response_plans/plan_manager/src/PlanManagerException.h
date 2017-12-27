//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerException.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_)
#define AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanManagerException

        class PlanManagerException : public TA_Base_Core::TransactiveException
    {
    // Operations
    public:
        PlanManagerException() throw();
        PlanManagerException(std::string msg) throw();
        PlanManagerException(std::string msg, std::string detail) throw();
        PlanManagerException(std::string msg, std::string detail, std::string comment) throw();
        virtual ~PlanManagerException() throw();

        virtual std::string getDetail() const throw();
        virtual std::string getComment() const throw();


    // Attributes:
    private:
        std::string m_detail;  // Additional detail to accompany the message
        std::string m_comment; // Remarks that may be helpful in correcting the problem
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_)
