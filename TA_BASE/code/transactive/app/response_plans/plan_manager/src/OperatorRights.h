/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/OperatorRights.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton class to facilitate access to the operator's access rights for the Plans subsystem.
  * Where the rights to a particular access controlled action cannot be determined, access is
  * automatically denied.
  *
  **/

#if !defined(AFX_OPERATORRIGHTS_H__9BBE312F_8771_4369_825C_BEAA15A68CE7__INCLUDED_)
#define AFX_OPERATORRIGHTS_H__9BBE312F_8771_4369_825C_BEAA15A68CE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // OperatorRights singleton

    class OperatorRights
    {
    public:
        ~OperatorRights();

        static OperatorRights &getInstance();
        static void destroyInstance();

        bool canEditCategories();
        bool canEditPlans();
        bool canApprovePlans();
        bool canSchedulePlans();
        bool canControlPlans();
        bool canCustomisePlans();
        bool canTakePlanOwnership();
        bool canViewNonLocalActivePlans();

    private:
        OperatorRights();
        OperatorRights(const OperatorRights &other);
        OperatorRights& operator=(const OperatorRights &other);


    private:
        static OperatorRights *s_instance;
        static TA_Base_Core::ReEntrantThreadLockable s_singletonLock;

        unsigned long m_planMgrResourceKey;
        TA_Base_Bus::RightsLibrary *m_rightsLibrary;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_OPERATORRIGHTS_H__9BBE312F_8771_4369_825C_BEAA15A68CE7__INCLUDED_)
