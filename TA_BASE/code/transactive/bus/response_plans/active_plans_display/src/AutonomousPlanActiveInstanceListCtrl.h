#if !defined(AFX_AUTONOMOUSPLANACTIVELISTCTRL_H__A048954D_899D_4085_ADCD_0ED7A6033AC6__INCLUDED_)
#define AFX_AUTONOMOUSPLANACTIVELISTCTRL_H__A048954D_899D_4085_ADCD_0ED7A6033AC6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/AutonomousPlanActiveInstanceListCtrl.h $
  * @author:  Rob Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Class derived from CPlanActiveInstanceListCtrl that self-manages the initial aquisition
  * and update of state for all currently active plan instances. This is a complete plugin module
  * for any client code wishing to display an MFC report style List Control of currently Active Response
  * Plans. To use, simply assign your List Control resource member to a class of this type.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.h"
#include "bus/response_plans/active_plans_display/src/IPlanInstanceFilter.h"
#include "bus/response_plans/active_plans_display/src/IActivePlanAccess.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentMap.h"


namespace TA_Base_Core
{
    struct ActivePlanId;
}


namespace TA_Base_Bus
{
	/////////////////////////////////////////////////////////////////////////////
	// CAutonomousPlanActiveListCtrl window

	class CAutonomousPlanActiveInstanceListCtrl : public CPlanActiveInstanceListCtrl, 
                                                  public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                                                  public IPlanInstanceFilter,
                                                  public IActivePlanAccess
	{
	// Construction
	public:
		/** 
		  * Constructor
		  *
		  * This constructs an instance of CAutonomousPlanActiveInstanceListCtrl.
		  */
		CAutonomousPlanActiveInstanceListCtrl();

		/** 
		  * Destructor
		  *
		  */
		virtual ~CAutonomousPlanActiveInstanceListCtrl();


    // Operations
    public:
		/**
		  * receiveSpecialisedMessage
		  * 
		  * Processes the specified comms message. Used for active plan detail updates.
		  *
          * @param  const TA_Base_Core::CommsMessageCorbaDef& message    -   comms message to process
          *
          */
		void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

		/** 
		  * showAllOwners
		  *
		  * Updates the list as specified by the value of allOwnersNeeded.
		  *
		  * param bool allOwnersNeeded - if true instances owned by operators other than the current one
          *                              are added to the list; if false other operators' instances are
          *                              removed from the list
		  *
		  */        
        void showAllOwners(bool allOwnersNeeded);

		/** 
		  * showAllLocations
		  *
		  * Updates the list as specified by the value of allLocationsNeeded.
		  *
		  * param bool allLocationsNeeded - if true instances at locations other than the current profile
          *                                 location are added to the list; if false other locations' instances
          *                                 are removed from the list
		  *
		  */        
        void showAllLocations(bool allLocationsNeeded);

		/** 
		  * allOwnersNeeded
		  *
		  * Indicate if all owners' plans are allowed in the list
		  *
		  * @return bool - true if all owners are included, false otherwise
		  *
		  */        
        bool allOwnersNeeded() const;

		/** 
		  * allLocationsNeeded
		  *
		  * Indicate if all locations' plans are allowed in the list
		  *
		  * @return bool - true if all locations are included, false otherwise
		  *
		  */
        bool allLocationsNeeded() const;

		/** 
		  * isRelevant
		  *
		  * Indicates whether the specified plan instance satisfies the filtering rules
		  *
		  * @return bool - true if instance accepted, false otherwise
		  *
		  */      
        bool isRelevant(const TA_Base_Core::ActivePlanId& activePlanId);

		/**
		  * getAllActivePlanDetailsAtLocation
		  * 
		  * Returns a Corba Container of all the current active instances at the specified location.
		  *
          * @param  unsigned long location          -   location key
          *
		  * @return TA_Base_Core::ActivePlanDetails *	-	The container of all active instances
		  *
		  * @exception	TransactiveException
		  */
        TA_Base_Core::ActivePlanDetails* getAllActivePlanDetailsAtLocation(unsigned long location);


    protected:
		/** 
		  * initialisePlanInstanceList
		  *
		  *	Helper method to subscribe to plan instance updates, populate the list of 
		  * current plan instances, and then activate the periodic processing of received
		  * updates.
		  */
		void InitialiseActivePlanList();

		/** 
		  * subscribeForLocalUpdates
		  *
		  * Subscribe for Active Plan Detail updates from Plan Agent at the login location only.
          * Throws an exception if the login location cannot be determined (see exception).
		  *
          * @exception DatabaseException - thrown if there are problems accessing location data
          * @exception DataException - thrown if there are problems accessing location data
          * @exception AuthenticationAgentException - thrown if there are problems accessing session information
          * @exception ObjectResolutionException - thrown if there are problems accessing session information
		  *
		  */
		void SubscribeForLocalUpdates();

		/** 
		  * subscribeForAllUpdates
		  *
		  * Subscribe for Active Plan Detail updates from all Plan Agents.
		  *
		  */
		void SubscribeForAllUpdates();

		/** 
		  * unsubscribe
		  *
		  * Unsubscribe from Active Plan Detail updates.
		  *
		  */
		void Unsubscribe();

    
    // Attributes:
    private:
        bool m_allOwnersNeeded;
        bool m_allLocationsNeeded;
        PlanAgentMap m_planAgentMap;

        std::map<unsigned long, int> m_planTypeMap;


	// Overrides
	protected:
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CAutonomousPlanActiveInstanceListCtrl)
	    //}}AFX_VIRTUAL

	// Generated message map functions
	protected:
		//{{AFX_MSG(CAutonomousPlanActiveInstanceListCtrl)
	    //}}AFX_MSG
		afx_msg void OnActivePlanListPostInitialise();
        afx_msg void OnDestroy();
		DECLARE_MESSAGE_MAP()
	};

} // namespace TA_Base_Bus

#endif // !defined(AFX_AUTONOMOUSPLANACTIVELISTCTRL_H__A048954D_899D_4085_ADCD_0ED7A6033AC6__INCLUDED_)
