/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/PlanAgentMap.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Maps location keys to PlanAgentLibrary objects. Facilitates access to Plan Agent based on location.
  * 
  */

#if !defined(AFX_PLANAGENTMAP_H__48B57372_B558_40A9_9A7A_7DD1E525415D__INCLUDED_)
#define AFX_PLANAGENTMAP_H__48B57372_B558_40A9_9A7A_7DD1E525415D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"


namespace TA_Base_Bus
{
    class PlanAgentMap
    {
    // Operations
    public:
		/** 
		  * Constructor
		  *
		  */
        PlanAgentMap();

		/** 
		  * Destructor
		  *
		  */
        ~PlanAgentMap();

		/** 
		  * getPlanAgent
		  *
		  * Returns the plan agent corresponding to the specified location
		  *
		  * @param unsigned long location   -	location key
          *
		  * @return PlanAgentLibrary&       -	reference to the relevant plan agent library
		  *
          * @exception DatabaseException - thrown if there are problems accessing configuration data
          * @exception DataException - thrown if there are problems accessing configuration data
          * @exception PlanAgentEntityNotFoundException - thrown if no plan agent entity is defined for specified location
          *
		  */
        PlanAgentLibrary& getPlanAgent(unsigned long location);


		/** 
		  * getDefaultPlanAgent
		  *
		  * Returns the plan agent at the default location, i.e. the login location for local profiles and
          * physical location for central ones.
		  *          *
		  * @return PlanAgentLibrary&       -	reference to the relevant plan agent library
		  *
          * @exception DatabaseException - thrown if there are problems accessing configuration data
          * @exception DataException - thrown if there are problems accessing configuration data
          * @exception PlanAgentEntityNotFoundException - thrown if no plan agent entity is defined for current location
          * @exception AuthenticationAgentException - thrown if there are problems accessing session information
          * @exception ObjectResolutionException - thrown if there are problems accessing session information
          *
		  */
        PlanAgentLibrary& getDefaultPlanAgent();


    // Attributes
    private:
		// Plan agent map gives access to the appropriate plan agent based on its location.
        typedef std::map<unsigned long, TA_Base_Bus::PlanAgentLibrary *> LocationToPlanAgentMap;
        LocationToPlanAgentMap m_locationToPlanAgentMap;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANAGENTMAP_H__48B57372_B558_40A9_9A7A_7DD1E525415D__INCLUDED_)
