/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Thread Safe Singleton to wrap access to the Plan Agent's CORBA Interface
  */

#if !defined(AFX_PLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_)
#define AFX_PLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "bus/response_plans/active_plans_display/src/IActivePlanAccess.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentMap.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "SynchronisedMap.h"
#include "Timer.h"
#include "ITimerSubscriber.h"

#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "PlanAgentAccessException.h"



namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentAccess

    class PlanAgentAccess : public TA_Base_Bus::IActivePlanAccess,
                            public ITimerSubscriber
    {
    public:
        /**
          * Destructor
          *
          */
        ~PlanAgentAccess();

        /**
          * getInstance
          *
          * This method allows the one and only PlanAgentAccess to be accessed.
          *
          * @return PlanAgentAccess&    -    a reference to the PlanAgentAccess
          */
        static PlanAgentAccess& getInstance();

        /**
          * destroyInstance
          *
          * This method destroys the one and only PlanAgentAccess.
          */
        static void destroyInstance();

        /**
          * timerExpired
          *
          * Implements the ITimerSubscriber interface. Called whenever a Timer elapses that
          * the PlanAfentAccess is subscribed to.
          *
          * @param Timer &timer    -    Reference to the elapsed Timer
          *
          */
        void timerExpired(Timer &timer);

        /**
          * getRootChildNodes
          *
          * Returns a Corba Container of all child nodes of the plan tree root.
          *
          * @return TA_Base_Core::SiblingNodes *     -    The container of child nodes.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::SiblingNodes *getRootChildNodes();

        /**
          * getChildNodes
          *
          * Returns a Corba Container of all child nodes of the specified parent node.
          * The caller is responsible for destroying the container.
          *
          * @param    TA_Base_Core::NodeId parentNodeId    -    ID of the parent node
          *
          * @return TA_Base_Core::SiblingNodes *         -    The container of child nodes.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::SiblingNodes *getChildNodes(TA_Base_Core::NodeId parentNodeId);

        /**
          * getNodeSummary
          *
          * Returns summary information for the specified node.
          *
          * @param    TA_Base_Core::NodeId nodeId  -    ID of the node
          *
          * @return TA_Base_Core::NodeSummary *     -    Node summary.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::NodeSummary *getNodeSummary(TA_Base_Core::NodeId nodeId);

        /**
          * getNodeDetail
          *
          * Returns detailed information for the specified node.
          *
          * @param    TA_Base_Core::NodeId nodeId  -    ID of the node
          *
          * @return TA_Base_Core::NodeDetail *     -    Node detail.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::NodeDetail *getNodeDetail(TA_Base_Core::NodeId nodeId);

        /**
          * getStepDetails
          *
          * Returns a Corba Container of all steps of the specified plan node.
          *
          * @param    TA_Base_Core::NodeId planNodeId  -    ID of the plan node
          *
          * @return TA_Base_Core::StepDetails *      -    The container of plan steps.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::StepDetails *getStepDetails(TA_Base_Core::NodeId planNodeId);

        /**
          * getParameterNames
          *
          * Returns a Corba Container of parameter names associated with the specified step type.
          *
          * @param    TA_Base_Core::EStep stepType     -    Type of step
          *
          * @return TA_Base_Core::ParameterNames *     -    The container of parameter names.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ParameterNames *getParameterNames(TA_Base_Core::EStep stepType);

        /**
          * getStepParameters
          *
          * Returns a Corba Container of all step parameters for the specified plan step.
          *
          * @param    TA_Base_Core::StepId stepId     -    ID of the plan step
          *
          * @return TA_Base_Core::StepParameters *     -    The container of step parameters
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::StepParameters *getStepParameters(TA_Base_Core::StepId stepId);

        /**
          * getStepParameters
          *
          * Returns a Corba Container of all step parameters for the specified plan step.
          *
          * @param    TA_Base_Core::NodeId planNodeId       -    ID of the plan
          * @param    TA_Base_Core::StepNumber stepNumber   -    Position of the step
          *
          * @return TA_Base_Core::StepParameters *          -    The container of step parameters
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::StepParameters *getStepParameters(TA_Base_Core::NodeId planNodeId, TA_Base_Core::StepNumber stepNumber);

        /**
          * getNodeId
          *
          * Returns node ID corresponding to the specified node path.
          * Nodes in the path are separated with '/'. The path has a leading '/' to denote the root.
          *
          * @param    std::string nodePath    -    Path of the tree node.
          *
          * @return TA_Base_Core::NodeId         -    ID of the tree node.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::NodeId getNodeId(std::string nodePath);

        /**
          * getNodePath
          *
          * Returns a string denoting the absolute path of the specified plan tree node.
          * Nodes in the path are separated with '/'. The path has a leading '/' to denote the root.
          *
          * @param    TA_Base_Core::NodeId nodeId   -    ID of tree node.
          *
          * @return std::string               -    Path of the tree node.
          *
          * @exception    PlanDataAccessException
          */
        std::string getNodePath(TA_Base_Core::NodeId nodeId);

        /**
          * getNodeIdsInNodePath
          *
          * Returns a Corba Container of node IDs corresponding to nodes in the path of the
          * specified plan tree node.
          *
          * @param    TA_Base_Core::NodeId nodeId   -    ID of tree node.
          *
          * @return TA_Base_Core::NodeIds *       -    The container of node IDs.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::NodeIds *getNodeIdsInNodePath(TA_Base_Core::NodeId nodeId);

        /**
          * getStepNumber
          *
          * Returns the position of the specified plan step.
          *
          * @param    TA_Base_Core::StepId stepId   -    ID of the plan step.
          *
          * @return TA_Base_Core::StepNumber      -    The step's position in the plan.
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::StepNumber getStepNumber(TA_Base_Core::StepId stepId);

        /**
          * getActivePlanDetails
          *
          * Returns a Corba Container of all the current ActivePlanDetail Structs for a
          * particular Plan at the profile location.
          *
          * @param    TA_Base_Core::NodeId planNodeId        -    ID of the Plan
          *
          * @return TA_Base_Core::ActivePlanDetails *    -    The container of ActivePlanDetails for the Plan
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ActivePlanDetails *getActivePlanDetails(TA_Base_Core::NodeId planNodeId);

        /**
          * getAllActivePlanDetails
          *
          * Returns a Corba Container of all the current ActivePlanDetail Structs at the profile location.
          *
          * @return TA_Base_Core::ActivePlanDetails *    -    The container of all ActivePlanDetails
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ActivePlanDetails *getAllActivePlanDetails();

        /**
          * getActivePlanDetailsAtLocation
          *
          * Returns a Corba Container of all the current ActivePlanDetail Structs for a
          * particular Plan at the specified location.
          *
          * @param    TA_Base_Core::NodeId planNodeId        -    ID of the Plan
          * @param  unsigned long location          -   location key
          *
          * @return TA_Base_Core::ActivePlanDetails *    -    The container of ActivePlanDetails for the Plan
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ActivePlanDetails *getActivePlanDetailsAtLocation(TA_Base_Core::NodeId planNodeId, unsigned long location);

        /**
          * getAllActivePlanDetailsAtLocation - implementation of TA_Base_Bus::IActivePlanAccess::getAllActivePlanDetailsAtLocation()
          *
          * Returns a Corba Container of all the current ActivePlanDetail Structs at the specified location.
          *
          * @param  unsigned long location          -   location key
          *
          * @return TA_Base_Core::ActivePlanDetails *    -    The container of all ActivePlanDetails
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ActivePlanDetails *getAllActivePlanDetailsAtLocation(unsigned long location);


        /**
          * getAssocAlarmPlanDetail - implementation of TA_Base_Bus::IActivePlanAccess::getAssocAlarmPlanDetail()
          *
          * Returns a Corba Container of the current ActivePlanDetail Structs for the plan assocaited with the alarm
          *
          * i.e. plan which raised the alarm
          *
        * @param  std::string alarmId  -- alarmId of the alarm raised by a plan
        *
          * @return TA_Base_Core::ActivePlanDetail*    -    The container of ActivePlanDetail
          *
          * @exception    PlanDataAccessException
         */
          TA_Base_Core::ActivePlanDetail* PlanAgentAccess::getAssocAlarmPlanDetail( const std::string& alarmId, const unsigned long ulLocation );

        /** getExecutionHistory
          *
          * Returns the execution history of a particular active plan instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @return TA_Base_Core::ExecutionHistory *         -    The container of execution history details
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::ExecutionHistory *getExecutionHistory(TA_Base_Core::ActivePlanId activePlanId);

        /** getCustomisationHistory
          *
          * Returns the customisation history of a particular active plan instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId   -    The Plan and Instance ID
          *
          * @return TA_Base_Core::CustomisationHistory *     -    The container of customisation history details
          *
          * @exception    PlanDataAccessException
          */
        TA_Base_Core::CustomisationHistory *getCustomisationHistory(TA_Base_Core::ActivePlanId activePlanId);

        /** getCustomisationHistory
          *
          * Returns an indication of whether the current session owns the specified active plan instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId   -    The Plan and Instance ID
          *
          * @return bool                                 -    True if current session owns the instance.
          *
          * @exception    PlanDataAccessException
          */
        bool isOwner(TA_Base_Core::ActivePlanId activePlanId);

        /**
          * startPlan
          *
          * Start a new Plan Instance.
          *
          * @param TA_Base_Core::NodeId planNodeId   -    ID of the plan to be started
          *
          * @exception    PlanControlException
          */
        void startPlan(TA_Base_Core::NodeId planNodeId);

        /**
          * customisePlan
          *
          * Customise a new Plan Instance.
          *
          * @param TA_Base_Core::NodeId planNodeId   -    ID of the plan to be started in customisation mode
          *
          * @exception    PlanControlException
          */
        void customisePlan(TA_Base_Core::NodeId planNodeId);

        /** pausePlan
          *
          * Pause a Plan Instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @exception    PlanControlException
          */
        void pausePlan(TA_Base_Core::ActivePlanId activePlanId);

        /** resumePlan
          *
          * Resume a Plan Instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @exception    PlanControlException
          */
        void resumePlan(TA_Base_Core::ActivePlanId activePlanId);

        /** stopPlan
          *
          * Stop a Plan Instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @exception    PlanControlException
          */
        void stopPlan(TA_Base_Core::ActivePlanId activePlanId);

        /** takeControlOfPlan
          *
          * Take control of a Plan Instance from the current owner.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @exception    PlanControlException
          */
        void takeControlOfPlan(TA_Base_Core::ActivePlanId activePlanId);

        /** customiseStep
          *
          * Customise the specified step for the specified plan instance.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId                -    The Plan and Instance ID
          * @param TA_Base_Core::CustomisableStepDetail customStepDetail -    Custom step details
          *
          * @exception    PlanControlException
          */
        void customiseStep(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::StepCustomisableDetail customStepDetail);

        /** reportDecisionStepResult
          *
          * Propagate the Decision Step result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Decision Step Result
          *
          * @exception    PlanControlException
          */
        void reportDecisionStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status);


        // lizettejl++ (DP-changes)
        /** resumePlanAtStep
          *
          * Resume a Plan Instance at a specified step.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId    -    The Plan and Instance ID
          *
          * @param TA_Base_Core::StepNumber stepNumber            -    Step number to resume plan
          *
          * @exception    PlanControlException
          */
        void resumePlanAtStep(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::StepNumber stepNumber);

        /** reportFailureSummaryStepResult
          *
          * Propagate the Decision Step result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Failure Summary Step Result
          *
          * @exception    PlanControlException
          */
        void reportFailureSummaryStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);
        // ++lizettejl

        /** reportPromptStepResult
          *
          * Propagate the Prompt Step result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Prompt Step Result
          *
          * @exception    PlanControlException
          */
        void reportPromptStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status);

        /** reportLogStepResult
          *
          * Propagate the Log Step result and string to be logged to the Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Log Step Result
          *
          * @param const char *logMessage                        -    string to be logged
          *
          * @exception    PlanControlException
          */
        void reportLogStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, std::string logMessage);

        /** reportRunPlanStepResult
          *
          * Propagate the Run Plan step execution result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Run Plan step result
          *
          * @param const std::string& error                            -    Description of error if result indicates a failure.
          *                                                             Leave blank if successful.
          *
          * @exception    PlanControlException
          */
        void reportRunPlanStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);

        /** reportLaunchGuiAppStepResult
          *
          * Propagate the Launch GUI Application step execution result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Launch GUI Application step result
          *
          * @param const std::string& error                            -    Description of error if result indicates a failure.
          *                                                             Leave blank if successful.
          * @exception    PlanControlException
          */
        void reportLaunchGuiAppStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);

        /** reportLaunchSchematicStepResult
          *
          * Propagate the Launch Schematic step execution result to Plan Agent.
          *
          * @param TA_Base_Core::ActivePlanId activePlanId            -    The Plan and Instance ID
          *
          * @param TA_Base_Core::EPlanStepCompletionStatus status    -    Enum code of Launch Schematic step result
          *
          * @param const std::string& error                            -    Description of error if result indicates a failure.
          *                                                             Leave blank if successful.
          * @exception    PlanControlException
          */
        void reportLaunchSchematicStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);

        /** setPlanApprovalState
          *
          * Modify the Approval State of a Plan
          *
          * @param TA_Base_Core::NodeId planNodeId        -    The Plan ID
          *
          * @param TA_Base_Core::EApprovalState state    -    Enum Code of Approval State
          *
          * @exception    PlanDataEditException
          */
        void setPlanApprovalState(TA_Base_Core::NodeId planNodeId, TA_Base_Core::EApprovalState state);

        /** getNextStepId
          *
          * Get the next step Id for a new step being created
          *
          * @return TA_Base_Core::StepId           -    The new step id
          *
          * @exception    PlanDataEditException
          */
        TA_Base_Core::StepId getNextStepId();

        /** getEditPlanLock
          *
          * Obtains an editing lock for the specified plan or category node. Throws an exception if the
          * node is already locked by a different session.
          *
          * @param TA_Base_Core::NodeId nodeId        -    The node ID of the plan or category to be locked
          *
          * @exception    PlanDataEditException
          */
        void getPlanEditingLock(TA_Base_Core::NodeId nodeId);

        /** releaseEditPlanLock
          *
          * Releases the editing lock on the specified plan node.
          *
          * @param TA_Base_Core::NodeId planNodeId        -    The node ID of the plan to be unlocked
          *
          * @exception    PlanDataEditException
          */
        void releasePlanEditingLock(TA_Base_Core::NodeId planNodeId);

        /** addPlan
          *
          * Adds a new plan to the specified category. The plan is given a default name.
          *
          * @param TA_Base_Core::NodeId categoryNodeId            -    The node ID of the target category
          *
          * @exception    PlanDataEditException
          */
        void addPlan(TA_Base_Core::NodeId categoryNodeId);

        /** addPlanToRoot
          *
          * Adds a new plan to the root directory. The plan is given a default name.
          *
          * @exception    PlanDataEditException
          */
        void addPlanToRoot();

        /** deletePlan
          *
          * Removes the specified plan node.
          *
          * @param TA_Base_Core::NodeId planNodeId        -    The node ID of the plan to be removed
          *
          * @exception    PlanDataEditException
          */
        void deletePlan(TA_Base_Core::NodeId planNodeId);

        /** deletePlans
          *
          * Removes the specified plan nodes.
          *
          * @param TA_Base_Core::NodeIds planNodeIds        -    The node IDs of the plans to be removed
          *
          * @exception    PlanDataEditException
          */
        void deletePlans(TA_Base_Core::NodeIds planNodeIds);

        /** movePlan
          *
          * Moves the specified plan node to another category.
          *
          * @param TA_Base_Core::NodeId planNodeId        -    The node ID of the plan to be moved
          * @param TA_Base_Core::NodeId categoryNodeId    -    The node ID of the target category
          * @param bool overwrite                       -   Replace an existing plan by the same name
          *
          * @exception    PlanDataEditException
          */
        void movePlan(TA_Base_Core::NodeId planNodeId, TA_Base_Core::NodeId categoryNodeId, bool overwrite);

        /** movePlanToRoot
          *
          * Moves the specified plan node to the root category.
          *
          * @param TA_Base_Core::NodeId planNodeId        -    The node ID of the plan to be moved
          * @param bool overwrite                       -   Replace an existing plan by the same name
          *
          * @exception    PlanDataEditException
          */
        void movePlanToRoot(TA_Base_Core::NodeId planNodeId, bool overwrite);

        /** copyPlan
          *
          * Copies the specified plan node to another category.
          *
          * @param TA_Base_Core::NodeId planNodeId                -    The node ID of the plan to be copied
          * @param TA_Base_Core::NodeId categoryNodeId            -    The node ID of the target category
          * @param bool overwrite                               -   Replace an existing plan by the same name
          *
          * @exception    PlanDataEditException
          */
        void copyPlan(TA_Base_Core::NodeId planNodeId, TA_Base_Core::NodeId categoryNodeId, bool overwrite);

        /** copyPlanToRoot
          *
          * Copies the specified plan node to the root category.
          *
          * @param TA_Base_Core::NodeId planNodeId              -    The node ID of the plan to be copied
          * @param bool overwrite                               -   Replace an existing plan by the same name
          *
          * @exception    PlanDataEditException
          */
        void copyPlanToRoot(TA_Base_Core::NodeId planNodeId, bool overwrite);

        /** updatePlan
          *
          * Updates the details of the specified plan and its steps.
          *
          * @param TA_Base_Core::Plan plan                -    The new plan details
          *
          * @exception    PlanDataEditException
          */
        void updatePlan(TA_Base_Core::Plan plan);

        /** addCategory
          *
          * Adds a new sub-category to the specified category. The sub-category is given a default name.
          *
          * @param TA_Base_Core::NodeId categoryNodeId    -    The node ID of the target category
          *
          * @exception    PlanDataEditException
          */
        void addCategory(TA_Base_Core::NodeId categoryNodeId);

        /** addCategoryToRoot
          *
          * Adds a new sub-category to the root category. The sub-category is given a default name.
          *
          * @exception    PlanDataEditException
          */
        void addCategoryToRoot();

        /** deleteCategory
          *
          * Removes the specified category node.
          *
          * @param TA_Base_Core::NodeId categoryNodeId   -    The node ID of the category to be removed
          *
          * @exception    PlanDataEditException
          */
        void deleteCategory(TA_Base_Core::NodeId categoryNodeId);

        /** updateCategory
          *
          * Updates the details of the specified category.
          *
          * @param TA_Base_Core::CategoryDetail categoryDetail   -    The new category details
          *
          * @exception    PlanDataEditException
          */
        void updateCategory(TA_Base_Core::CategoryDetail categoryDetail);

        /** exportPlan
          *
          * exports the details of the specified plan and its steps.
          *
          * @return    TA_Base_Core::AuxPlanDetail* plan                -    The auxilary plan details from existing plan
          *
          * @exception    PlanDataExportException
          */
        TA_Base_Core::AuxPlanDetail* PlanAgentAccess::exportPlan(TA_Base_Core::NodeId planNodeId);

        /** importPlan
          *
          * imports the details of the specified plan and its steps
          *
          * @param TA_Base_Core::AuxPlanDetail plan                -    The auxilary plan details obtained from .pln file
          * @param TA_Base_Core::NodeId    parentId                    -  ID of the Category node the plan is to be imported under
          *
          * @exception    PlanDataExportException
          */
        void importPlan(TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId);

        /** registerPlanManager
        *
        * return true if PlanManager is registered with Plan Agent at location - location
        *
        * @param unsigned long location - location of Plan Agent as obtained from DB
        */
        bool isRegisteredWithAgent (unsigned long location);

        /** registerPlanManager
          *
          * Specify the Plan Manager object reference to be used for executing interactive steps
          *
          * @param TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerObjRef  -    Plan Manager object reference
          */
        void registerPlanManager(TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerObjRef, unsigned long location);

         /** deregisterPlanManager
          *
          * Invalidate the Plan Manager object reference used for executing interactive steps
          */
        void deregisterPlanManager(unsigned long location);

        /** generateReport
        *   launches Report Manager to generate report of broken links for a plan
        *   @planPath std::string - planPath used by report to find links in DB
        */
        void generateReport (const std::string& planPath);

		//libo 
		TA_Base_Bus::PlanAgentMap& getPlanAgentMap();

		// Returns the current session ID.
		// make public
        std::string getSessionId(bool useRunparam = false) const;

        void setPlanType(TA_Base_Core::NodeId planNodeId, TA_Base_Core::EPlanType state);

    private:
        // Singleton so has to be privately constructed
        PlanAgentAccess();

        // Declare the copy constructor private so the object can't be cloned
        PlanAgentAccess(const PlanAgentAccess& planAgentAccess);

        // Declare the operator= function so the compiler doesn't create a default
        // implementation.
        PlanAgentAccess& operator=(const PlanAgentAccess& planAgentAccess);

        // Returns the edit lock Timer associated with the specified node ID. NULL if there is none.
        Timer *getEditLockTimer(TA_Base_Core::NodeId nodeId);

        // Returns the current session ID.
        //std::string getSessionId(bool useRunparam = false) const;

        // Plan agent map gives access to the appropriate plan agent based on its location.
        //TA_Base_Bus::PlanAgentMap m_planAgentMap;
    private:
        // The one and only Object of this Class
        static PlanAgentAccess* m_thePlanAgentAccess;

        // Semaphore to thread guard this singleton
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        // Plan agent map gives access to the appropriate plan agent based on its location.
        TA_Base_Bus::PlanAgentMap m_planAgentMap;

        // Map edit lock timers to corresponding node IDs
        typedef SynchronisedMap<Timer*, TA_Base_Core::NodeId> TimerMap;
        TimerMap m_editLockTimerMap;

        // Session ID used to deregister Plan Manager from Plan Agents (TD 10849)
        mutable std::string m_sessionID;

        // TD15049++
        enum ETimeOut
        {
            PLAN_AGENT_REGISTER_TIMEOUT  = 20,
            PLAN_AGENT_PLAN_SHORT_TIMEOUT  = 30,
            PLAN_AGENT_PLAN_MEDIUM_TIMEOUT    = 150,
            PLAN_AGENT_PLAN_LONG_TIMEOUT   = 300
        };
        // ++TD15049

    };


	#define PAA_BEGIN_TRY    try

	#define PAA_END_TRY(paaFn)                                                           \
	catch (const PlanAgentAccessException& ex)                                                        \
	{                                                                                                                \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught PlanAgentAccessException by ISMAR: %s", ex.what());\
		TA_THROW (PlanAgentAccessException (ex.what(), ex.getDetail ()))                                                        \
	}                                                                                                                \
	catch (const TA_Base_Core::ObjectResolutionException &ex)                                 \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught ObjectResolutionException: %s", ex.what());                          \
		throw;          \
	}                                                                                    \
	catch (const TA_Base_Core::TransactiveException &ex)                                      \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught TransactiveException: %s", ex.what());                               \
		throw;                                                                           \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError &ex)                         \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());              \
		TA_THROW(TA_Base_Core::PlanAgentException(ex.what.in(), ex.errorType, ex.itemId));    \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError &ex)                      \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanNotFound: %s", ex.what.in());                \
		TA_THROW(TA_Base_App::PlanNotFoundException(ex.what.in(), ex.errorType, ex.planId, ex.instanceId)); \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanStepDetailError &ex)                    \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanStepDetailError: %s", ex.what.in());         \
		TA_THROW(TA_Base_Core::PlanStepDetailException(ex.what.in(), ex.detailType, ex.errorType, ex.stepId)); \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::NoPermission &ex)                           \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::NoPermission: %s", ex.what.in());                \
		TA_THROW(TA_Base_Core::RightsException(ex.what.in()));                                \
	}                                                                                    \
	catch (const CORBA::Exception &ex)                                                   \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught CORBA::Exception: %s",                                               \
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());            \
		TA_THROW(TA_Base_Core::TransactiveException(std::string("CORBA::Exception: ") +       \
				 TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));              \
	}                                                                                    \
	catch (...)                                                                          \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). Unknown exception.");              \
		TA_THROW(TA_Base_Core::TransactiveException("Unknown error"));                        \
	}

} // namespace TA_Base_App

#endif // !defined(AFX_PLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_)
