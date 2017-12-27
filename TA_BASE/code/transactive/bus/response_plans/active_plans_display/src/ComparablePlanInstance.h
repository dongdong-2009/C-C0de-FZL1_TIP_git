#if !defined(AFX_COMPARABLEPLANINSTANCE_H__BBFB50BE_39CF_49E4_B659_1DDFA5EFEA84__INCLUDED_)
#define AFX_COMPARABLEPLANINSTANCE_H__BBFB50BE_39CF_49E4_B659_1DDFA5EFEA84__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/ComparablePlanInstance.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TA_Base_Core::ActivePlanDetail wrapper that allows two plan instances to be compared.
  *
  **/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
    class ComparablePlanInstance
    {
    // Operations
    public:
		/** 
		  * Constructor
		  *
		  * This constructs an instance of ComparablePlanInstance from a TA_Base_Core::ActivePlanDetail.
		  */
        ComparablePlanInstance(const TA_Base_Core::ActivePlanDetail& activePlanDetail);

		/** 
		  * Constructor
		  *
		  * This constructs an instance of ComparablePlanInstance from another ComparablePlanInstance.
		  */
        ComparablePlanInstance(const ComparablePlanInstance& comparablePlanInstance);

		/** 
		  * Destructor
		  *
		  */
        virtual ~ComparablePlanInstance();

		/** 
		  * Assignment operator
          *
		  * Assign the specified TA_Base_Core::ActivePlanDetail to this ComparablePlanInstance.
		  */
        ComparablePlanInstance& operator=(const TA_Base_Core::ActivePlanDetail& activePlanDetail);

		/** 
		  * Assignment operator
          *
		  * Assign the specified ComparablePlanInstance to this ComparablePlanInstance.
		  */
        ComparablePlanInstance& operator=(const ComparablePlanInstance& comparablePlanInstance);

		/** 
		  * Equals operator
		  *
		  * Return true if this ComparablePlanInstance is equivalent to the specified TA_Base_Core::ActivePlanDetail.	
		  *
		  */
        virtual bool operator==(const TA_Base_Core::ActivePlanDetail& activePlanDetail);
       
		/** 
		  * Equals operator
		  *
		  * Return true if this ComparablePlanInstance is equivalent to the specified ComparablePlanInstance.	
		  *
		  */
        virtual bool operator==(const ComparablePlanInstance& comparablePlanInstance);

		/** 
		  * Less than operator
		  *
		  * Return true if this ComparablePlanInstance is less than the specified TA_Base_Core::ActivePlanDetail.	
		  *
		  */
        virtual bool operator<(const TA_Base_Core::ActivePlanDetail& activePlanDetail);
       
		/** 
		  * Less than operator
		  *
		  * Return true if this ComparablePlanInstance is less than the specified ComparablePlanInstance.	
		  *
		  */
        virtual bool operator<(const ComparablePlanInstance& comparablePlanInstance);
        
		/** 
		  * Greater than operator
		  *
		  * Return true if this ComparablePlanInstance is greater than the specified TA_Base_Core::ActivePlanDetail.	
		  *
		  */
        virtual bool operator>(const TA_Base_Core::ActivePlanDetail& activePlanDetail);
       
		/** 
		  * Greater than operator
		  *
		  * Return true if this ComparablePlanInstance is greater than the specified ComparablePlanInstance.	
		  *
		  */
        virtual bool operator>(const ComparablePlanInstance& comparablePlanInstance);

        /**
		  * Compares the IDs of two active plan instance state updates sent by Plan Agent.
          *
		  * Return:  0 if updateId1 is same as updateId2
          *         -1 if updateId1 is less recent than updateId2
          *         +1 if updateId1 is more recent than updateId2
          *
		  */ 
        static int compareUpdateIds(TA_Base_Core::UpdateId updateId1, TA_Base_Core::UpdateId updateId2);


    protected:
        /**
          * Return plan instance details
          *
          */
        virtual const TA_Base_Core::ActivePlanDetail& getActivePlanDetail() const;

        /**
          * Return a priority indicator value corresponding to the specified execution state. 
          * The lower the value, the higher the priority. -1 is returned for unknown execution states.
          *
          */
        virtual int getExecutionStatePriority(const TA_Base_Core::EActivePlanState& executionState);

        

    // Attributes
    private:
        TA_Base_Core::ActivePlanDetail m_activePlanDetail;

        static std::map<TA_Base_Core::EActivePlanState, int> m_execStatePriorityMap; 
        static TA_Base_Core::NonReEntrantThreadLockable m_execStatePriorityMapLock;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_COMPARABLEPLANINSTANCE_H__BBFB50BE_39CF_49E4_B659_1DDFA5EFEA84__INCLUDED_)
