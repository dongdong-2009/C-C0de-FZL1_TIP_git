//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_PLANMANAGERCORBADEFIMPL_H__07B694BA_E987_4433_B7B9_C170A)
#define AFX_PLANMANAGERCORBADEFIMPL_H__07B694BA_E987_4433_B7B9_C170A974C752__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/corba/src/ServantBase.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "PlanManagerCommonDefs.h"

#include "Timer.h"
#include "ITimerSubscriber.h"
#include "SynchronisedMap.h"

#include "PlanAgentAccess.h"

// Launching Issue, Yanrong++
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

// ++Yanrong


namespace TA_Base_App
{
    class InteractiveStep;

    /////////////////////////////////////////////////////////////////////////////
    // PlanManagerCorbaDefImpl

    class PlanManagerCorbaDefImpl : public virtual POA_TA_Base_Bus::IPlanManagerCorbaDef,
                                    public virtual TA_Base_Core::ServantBase,
                                    public ITimerSubscriber
    {
        // Operations
    public:
        PlanManagerCorbaDefImpl();
        virtual ~PlanManagerCorbaDefImpl();
        // haipeng added
        int toActivate();
        void init(int nIndex, std::string &error);
        // haipeng added
        /**
        * timerExpired
        *
        * Implements the ITimerSubscriber interface. Called whenever a Timer elapses that
        * the PlanAentAccess is subscribed to.
        *
        * @param Timer &timer    -    Reference to the elapsed Timer
        *
        */
        void timerExpired(Timer &timer);

        // IPlanManagerCorbaDef interface
        virtual void executeInteractiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

    protected:
        void executeStep(InteractiveStep& interactiveStep);

    private:
        PlanManagerCorbaDefImpl(const PlanManagerCorbaDefImpl& planManagerCorbaDefImpl);
        PlanManagerCorbaDefImpl& operator=(const PlanManagerCorbaDefImpl& planManagerCorbaDefImpl);

        // Returns the agent subscription Timer associated with the specified agent location. NULL if there is none.
      Timer* getAgentRegistrationTimer(LocationKey location);

    private:
        // Map Plan Agent timers to corresponding agent locations
        typedef SynchronisedMap<Timer*, unsigned long> TimerMap;
        TimerMap m_agentRegisterTimerMap;

        // haipeng added
        typedef std::vector<unsigned long> LocationList;
        LocationList m_locationList;
        // haipeng added

        // Launching Issue, Yanrong++
    private:
        class PlanRegisterThread : public TA_Base_Core::Thread
        {
        public:
            PlanRegisterThread( PlanManagerCorbaDefImpl* parent, unsigned long location, unsigned long interval);
            ~PlanRegisterThread();
            void run();
            void terminate();
            void reRegister();

        private:
            unsigned long m_location;
            unsigned long m_interval;
            PlanManagerCorbaDefImpl* m_parent;
            bool m_terminated;
            bool m_bRegistered;
			TA_Base_Core::Condition  m_waitObject;
        };

        friend class PlanRegisterThread;

        bool ThreadRegisterPlanManager( unsigned long location );
        void StartThreadRegisterPM( unsigned long location, unsigned long interval );
        void CloseAllRegisterThread();

        std::map<unsigned long, PlanRegisterThread* > m_registerThreads;
        // ++Yanrong
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANMANAGERCORBADEFIMPL_H__07B694BA_E987_4433_B7B9_C170A974C752__INCLUDED_)
