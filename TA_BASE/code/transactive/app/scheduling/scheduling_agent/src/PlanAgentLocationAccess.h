/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanAgentLocationAccess.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Thread Safe Singleton to extract location data for Plan Agent entities.
  */

#if !defined(AFX_PLANAGENTLOCATIONACCESS_H__2970C81D_598E_4456_8BE6_E9DF3224A5C0__INCLUDED_)
#define AFX_PLANAGENTLOCATIONACCESS_H__2970C81D_598E_4456_8BE6_E9DF3224A5C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>
#include <string>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentLocationAccess

    class PlanAgentLocationAccess
    {
    public:
		/** 
		  * Destructor
		  *
		  */
        ~PlanAgentLocationAccess();

		/**
		  * getInstance
		  * 
		  * This method allows the one and only PlanAgentLocationAccess to be accessed.
		  *
		  * @return PlanAgentLocationAccess&	-	a reference to the PlanAgentLocationAccess
          *
          * @exception DatabaseException - thrown if there are problems accessing location or profile data
          * @exception DataException - thrown if there are problems accessing location or profile data
          * @exception AuthenticationAgentException - thrown if there are problems accessing session information
          * @exception ObjectResolutionException - thrown if there are problems accessing session information
          *
		  */
		static PlanAgentLocationAccess& getInstance();

		/**
		  * getDefaultPlanAgentLocation
		  * 
		  * Returns the location of the default Plan Agent. The default Plan Agent is the one used for reading
          * and writing plan configuration, as well as for starting plans.
          *
          * For a local profile this is the current login location.
          *
          * For a central profile this is the current physical location.
		  *
		  * @return unsigned long 	-	location key of the default Plan Agent
		  *
		  */       
        unsigned long getDefaultPlanAgentLocation();

		/**
		  * getAllPlanAgentLocations
		  * 
		  * Returns the location keys of all locations for which a Plan Agent is configured.
		  *
		  * @return std::vector<unsigned long> 	-	vector of all Plan Agent location keys
		  *
		  */       
        std::vector<unsigned long> getAllPlanAgentLocations();

		/**
		  * getNonDefaultPlanAgentLocations
		  * 
		  * Returns the location keys of all Plan Agent locations other than the default Plan Agent location.
		  *
		  * @return std::vector<unsigned long> 	-	vector of all non-default Plan Agent location keys
		  *
		  */       
        std::vector<unsigned long> getNonDefaultPlanAgentLocations();

		/**
		  * getDefaultPlanAgentLocationName
		  * 
		  * Returns the name of the location of the default Plan Agent.
		  *
		  * @return std::string 	-	name of the default Plan Agent location
		  *
		  */       
        std::string getDefaultPlanAgentLocationName();
        
		/**
		  * getLocationName
		  * 
		  * Returns the name of the location corresponding to the specified key.
		  *
          * @param unsigned long locationKey - location key of which the name is required
          *
		  * @return std::string 	-	name of the location
		  *
		  */       
        std::string getLocationName(unsigned long locationKey);


	private:
		// Singleton so has to be privately constructed
		PlanAgentLocationAccess();

		// Declare the copy constructor private so the object can't be cloned
		PlanAgentLocationAccess(const PlanAgentLocationAccess& locationAccess);

		// Declare the operator= function so the compiler doesn't create a default
		// implementation.
		PlanAgentLocationAccess& operator=(const PlanAgentLocationAccess& locationAccess);

        // Set up the profile location and the location map.
        void initialise();


    private:
		// The one and only Object of this Class
		static PlanAgentLocationAccess* m_locationAccess;
	
		// Semaphore to thread guard this singleton
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        typedef std::map<unsigned long, std::string> LocationMap; // map location key to location name
        LocationMap m_locationMap;

        unsigned long m_defaultLocation;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANAGENTLOCATIONACCESS_H__2970C81D_598E_4456_8BE6_E9DF3224A5C0__INCLUDED_)
