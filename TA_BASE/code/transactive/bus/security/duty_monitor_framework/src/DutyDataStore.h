#if !defined(_DUTYDATASTORE_H__50CB0C73_A491_4B76_8B7A_42933CAE2DC0__INCLUDED_)
#define _DUTYDATASTORE_H__50CB0C73_A491_4B76_8B7A_42933CAE2DC0__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyDataStore.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Hides the singleton implementation of DutyDataStoreSingleton from clients.
  *
  */

namespace TA_Base_Bus
{
	//TD18095, replace class "DutyDataStore" with namespace in same name. to faint the coding man who originaly wrote those rubish code!
    namespace DutyDataStore
    {
		void cleanUp();
        /**
          * Initialises the contents of the default region assignment, active region duty
          * and active subsystem duty matrices.
          * If ignoreSystemOperators == true, the default region assignment and active region duty
          * matrices will not contain information pertaining to system operators (agents).
          * If ignoreAppSubsystems == true, the active subsystem duty matrix will not contain 
          * information related to application (non-physical) subsystems.
          * Throws a DutyMonitorFrameworkException if any of these containers fails to initialise.
          *
          */
        void populate(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSessionSubsystemDuties=true);

		void setPopulateCondition(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSessionSubsystemDuties=true);

		void populateStaticData();

		void populateDynamicData();

        /**
          * Indicates whether duty information related to system operators should be discarded.
          *
          */
        bool ignoreSystemOperators() ;

        /**
          * Indicates whether duty information related to non-physical subsystems should be discarded.
          *
          */
        bool ignoreAppSubsystems() ;

        /**
          * Facilitates access to the configuration data cache.
          *
          */
        DataCache& getDataCache();

        /**
          * Facilitates access to the active session detail cache.
          *
          */
        SessionCache& getSessionCache();

        /**
          * Facilitates access to the default region assignments matrix.
          *
          */
        DefaultRegionAssignmentMatrix& getDefRegionAssignmentMatrix();

        /**
          * Facilitates access to the region duties matrix.
          *
          */
        RegionDutyMatrix& getRegionDutyMatrix();

        /**
          * Facilitates access to the subsystem duties matrix.
          *
          */
        SubsystemDutyMatrix& getSubsystemDutyMatrix();
    };
}

#endif // !defined(_DUTYDATASTORE_H__50CB0C73_A491_4B76_8B7A_42933CAE2DC0__INCLUDED_)
