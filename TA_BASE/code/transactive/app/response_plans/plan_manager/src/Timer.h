#if !defined(AFX_TIMER_H__C3F92A54_EE92_4E46_AA1D_AB0C9D55C581__INCLUDED_)
#define AFX_TIMER_H__C3F92A54_EE92_4E46_AA1D_AB0C9D55C581__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/Timer.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Utility class to manage the Win32 SDK Timer functions and callback. To use, implement
  * the ITimerSubscriber pure virtual class; subscribe to object of this type;
  * then call "start(unsigned int timercycle = 50)" on the object of this type.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class ITimerSubscriber; // Forward declaration

    class Timer
    {
    // Operations
    public:
        /**
          * Constructor
          *
          * This constructs an instance of Timer.
          */
        Timer();

        /**
          * Destructor
          *
          */
        ~Timer();

        /**
          * subscribe
          *
          * Subscribe to the Timer object (for timeout callbacks) using ITimerSubscriber interface.
          *
          * @param ITimerSubscriber* subscriber -    This is a pointer to the ITimerSubscriber to
          *                                            be called upon timeout callback.
          *
          */
        void subscribe(ITimerSubscriber* subscriber);

        /**
          * unsubscribe
          *
          * unsubscribe to the Timer object (clears pointer to currently subscibed ITimerSubscriber)
          *
          */
        void unsubscribe();

        /**
          * start
          *
          * Start the Timer object.
          *
          * @param unsigned int timerCycle -    Duration (in milliseconds) of each timer cycle
          *                                        (Default value = 50).
          *
          */
        void start(unsigned int timerCycle = 50 /*ms*/);

        /**
          * stop
          *
          * Stop the Timer object.
          *
          */
        void stop();

        /** onTimeout
          *
          * Method to be called by SDK timer callback function. It subsequently
          * calls the currently subscribed ITimerSubscriber.
          *
          */
        void onTimeout();


    // Attributes
    private:
        // 
        // The pointer to the currently subscribed ITimerSubscriber
        // 
        ITimerSubscriber *m_subscriber;

        // 
        // This stores the timer identifier
        unsigned long m_timerId;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_TIMER_H__C3F92A54_EE92_4E46_AA1D_AB0C9D55C581__INCLUDED_)
