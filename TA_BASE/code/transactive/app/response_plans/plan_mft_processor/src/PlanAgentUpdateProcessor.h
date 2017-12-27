/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#if !defined(AFX_PLANAGENTUPDATEPROCESSOR_H__INCLUDED_)
#define AFX_PLANAGENTUPDATEPROCESSOR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"


namespace TA_Base_Core
{
    struct ActivePlanDetail;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentUpdateProcessor

    class PlanAgentUpdateProcessor : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    // Operations
    public:
        ~PlanAgentUpdateProcessor();

        static PlanAgentUpdateProcessor &getInstance();
        static void destroyInstance();

    protected:
        void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

    private:
        PlanAgentUpdateProcessor();
        PlanAgentUpdateProcessor(const PlanAgentUpdateProcessor &other);
        PlanAgentUpdateProcessor &operator=(const PlanAgentUpdateProcessor &other);

    // Attributes
    private:
        static PlanAgentUpdateProcessor *s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
    };
}

#endif // !defined(AFX_PLANAGENTUPDATEPROCESSOR_H__2AC77A74_70F8_49BE_A459_C95D51962BC2__INCLUDED_)
