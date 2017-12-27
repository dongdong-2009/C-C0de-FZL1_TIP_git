/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanAgentUpdateProcessor.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Plan Manager's one and only subscriber of Plan Agent updates for plan execution, customisation and configuration.
  * Update consumers should register with PlanAgentUpdateProcessor to be notified of updates as they are received.
  *
  **/

#if !defined(AFX_PLANAGENTUPDATEPROCESSOR_H__2AC77A74_70F8_49BE_A459_C95D51962BC2__INCLUDED_)
#define AFX_PLANAGENTUPDATEPROCESSOR_H__2AC77A74_70F8_49BE_A459_C95D51962BC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "SynchronisedVector.h"


namespace TA_Base_App
{
    class IActivePlanObserver;
    class IPlanConfigObserver;
    class IPlanCustomisationObserver;
    class TreeNode;


    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentUpdateProcessor

    class PlanAgentUpdateProcessor : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    // Operations
    public:
        ~PlanAgentUpdateProcessor();

        static PlanAgentUpdateProcessor &getInstance();
        static void destroyInstance();

        void subscribeForActivePlanUpdates(IActivePlanObserver *observer);
        void subscribeForPlanConfigUpdates(IPlanConfigObserver *observer);
        void subscribeForPlanCustomisationUpdates(IPlanCustomisationObserver *observer);
        void unsubscribeFromActivePlanUpdates(IActivePlanObserver *observer);
        void unsubscribeFromPlanConfigUpdates(IPlanConfigObserver *observer);
        void unsubscribeFromPlanCustomisationUpdates(IPlanCustomisationObserver *observer);

    protected:
        void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

        void processActivePlanDetailUpdate(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        void processPlanConfigUpdate(const TA_Base_Core::PlanConfigUpdateData &planConfigData);
        void processPlanCustomisationUpdate(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail);

        void updateTreeNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData, TreeNode *treeNode);
        void updateAncestorNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData, TreeNode *ancestorNode);
        void updateRootNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData);

    private:
        PlanAgentUpdateProcessor();
        PlanAgentUpdateProcessor(const PlanAgentUpdateProcessor &other);
        PlanAgentUpdateProcessor &operator=(const PlanAgentUpdateProcessor &other);


    // Attributes
    private:
        static PlanAgentUpdateProcessor *s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        SynchronisedVector<IActivePlanObserver *> m_activePlanObservers;
        SynchronisedVector<IPlanConfigObserver *> m_planConfigObservers;
        SynchronisedVector<IPlanCustomisationObserver *> m_planCustomisationObservers;
    };
}

#endif // !defined(AFX_PLANAGENTUPDATEPROCESSOR_H__2AC77A74_70F8_49BE_A459_C95D51962BC2__INCLUDED_)
