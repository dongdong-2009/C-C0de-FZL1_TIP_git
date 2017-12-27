#if !defined(_DUTYRIGHTSSINGLETON_H__F2AC1BD9_2003_4D2A_8F85_8842CFD154DD__INCLUDED_)
#define _DUTYRIGHTSSINGLETON_H__F2AC1BD9_2003_4D2A_8F85_8842CFD154DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyRightsSingleton.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Singleton class to facilitate access to the operator's access rights for the Security subsystem.
  * Where the rights to a particular access controlled action cannot be determined, access is
  * automatically denied.
  *
  **/

namespace TA_Base_Bus
{
    class DutyRightsSingleton
    {
    public:
        /**
          * Returns the one and only instance of the DutyDataStoreSingleton. 
          *
          */
        static DutyRightsSingleton& getInstance();

        /**
          * Destructor
          *
          */
        ~DutyRightsSingleton();

        /**
          * Returns true if the current session has sufficient rights to transfer region duties.
          *
          */
        bool canTransferRegions();

		//TD19218---- LiDan 27,Jun,2007
        /**
		  * @param lActionId: the action id in db
          * Returns true if the current session has sufficient rights to transfer subsystem duties.
          *
          */	
        bool canTransferSubsystems(long lActionId);
		//----TD19218

        /**
          * Returns true if the current session has sufficient rights to perform system-wide duty delegation.
          *
          */
        bool canDegradeSubsystems();

    protected:
        /**
          * Processes session updates received from the SessionCache. Only profile changes
          * are of interest as they affect the operator's rights.
          *
          */
        void receiveSessionUpdate(const SessionUpdate& sessionUpdate);
        friend class SessionCache;

	private:
		DutyRightsSingleton();
		DutyRightsSingleton(const DutyRightsSingleton& dutyRightsSingleton);
		DutyRightsSingleton& operator=(const DutyRightsSingleton& dutyRightsSingleton);


    private:
		static DutyRightsSingleton *s_dutyRightsSingleton;
		static TA_Base_Core::ReEntrantThreadLockable s_singletonLock;

        unsigned long m_dutyAgentResourceKey;
        TA_Base_Bus::RightsLibrary *m_rightsLibrary;
    };

} // namespace TA_Base_App

#endif // !defined(_DUTYRIGHTSSINGLETON_H__F2AC1BD9_2003_4D2A_8F85_8842CFD154DD__INCLUDED_)
