#if !defined(_DUTYDATASTORESINGLETON_H__2B3EB547_F74C_4740_9D9E_B323922E46CC__INCLUDED_)
#define _DUTYDATASTORESINGLETON_H__2B3EB547_F74C_4740_9D9E_B323922E46CC__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyDataStoreSingleton.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Consolidates all containers of duty-related data. This includes the config data
  * cache, active session cache, default region assignment matrix, active region duty
  * matrix and active session duty matrix.
  * To ensure that only one copy of each container is created and maintained, this
  * class has been implemented as a singleton.
  *
  */

namespace TA_Base_Bus
{
    class DutyDataStoreSingleton
    {
    public:
        /**
          * Returns the one and only instance of the DutyDataStoreSingleton. 
          *
          */
        static DutyDataStoreSingleton& getInstance();

        /**
          * Destructor
          *
          */
        ~DutyDataStoreSingleton();


		static void cleanUp();
        /**
          * Initialises the contents of the default region assignment, active region duty
          * and active subsystem duty matrices.
          * If ignoreSystemOperators == true, the default region assignment and active region duty
          * matrices will not contain information pertaining to system operators (agents).
          * If ignoreAppSubsystems == true, the active subsystem duty matrix will not contain 
          * information related to application (non-physical) subsystems.
		  * If ignoreOtherSesSubDuties == true, the active subsystem duty matrix will not contain
		  * information related to other sessions
          * Throws a DutyMonitorFrameworkException if any of these containers fails to initialise.
          *
          */
        void populate(bool ignoreSystemOperators, bool ignoreAppSubsystems, bool ignoreOtherSesSubDuties = true);

		void setPopulateCondition(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSessionSubsystemDuties=true);

		void populateStaticData();

		void populateDynamicData();
        /**
          * Indicates whether duty information related to system operators should be discarded.
          *
          */
        bool ignoreSystemOperators() const;

        /**
          * Indicates whether duty information related to non-physical subsystems should be discarded.
          *
          */
        bool ignoreAppSubsystems() const;

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

    private:
        DutyDataStoreSingleton();
        DutyDataStoreSingleton(const DutyDataStoreSingleton& dutyDataStoreSingleton);
        DutyDataStoreSingleton& operator=(const DutyDataStoreSingleton& dutyDataStoreSingleton);


    private:
        static DutyDataStoreSingleton* s_dutyDataStoreSingleton;
		static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        bool m_ignoreSystemOperators;
        bool m_ignoreAppSubsystems;
		bool m_ignoreOtherSesSubDuties;

        DataCache m_dataCache;
        SessionCache m_sessionCache;
        DefaultRegionAssignmentMatrix m_defRegionAssignmentMatrix;
        RegionDutyMatrix m_regionDutyMatrix;
        SubsystemDutyMatrix m_subsystemDutyMatrix;
    };
}

#endif // !defined(_DUTYDATASTORESINGLETON_H__2B3EB547_F74C_4740_9D9E_B323922E46CC__INCLUDED_)
