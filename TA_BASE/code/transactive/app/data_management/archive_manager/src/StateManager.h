/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/StateManager.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Manages the state for any GUI that has three states -
  * IDLE, PROCESSING and STOPPING. The state transitions are
  * as follows:
  * 
  *    IDLE --> PROCESSING -----------> IDLE
  *                 |         
  *                 --> STOPPING -----> IDLE
  */

#if !defined(AFX_STATEMANAGER_H__3C1DA7B8_2E3C_403B_A031_E14957561DBA__INCLUDED_)
#define AFX_STATEMANAGER_H__3C1DA7B8_2E3C_403B_A031_E14957561DBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
    class StateManager  
    {
    public:

        //
        // Enumeration of the available states.
        //

        enum EState
        {
            IDLE        = 0,    // The state will be idle before and after processing.
            PROCESSING  = 1,    // Used when work is being done during which stopping (or cancelling) has to be timed appropriately.
            STOPPING    = 2     // Used to identify that at an appropriate time, the work should be stopped (or cancelled).
        };


        /** 
          * StateManager
          * 
          * Standard constructor.
          */

	    StateManager();
        
        /** 
          * ~StateManager
          * 
          * Standard destructor.
          */

        virtual ~StateManager() {};

        /**
          * getState
          *
          * Gets the current state (with thread locking).
          *
          * @return     EState state
          *             The current state.
          */

        EState getState();

        /**
          * setState
          *
          * Sets the state (with thread locking).
          * Note: The state change will be ignored if the 
          * current state is IDLE and the new state is
          * STOPPING, because this doesn't fit the available
          * state changes listed in the header documentation.
          *
          * @param      EState state
          *             The new state.
          */

        void setState( EState state );

    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        StateManager( const StateManager& );
        StateManager& operator=( const StateManager& );
        
        //
        // The current state of the GUI and the lock for protecting this state.
        //

        EState volatile m_state;
        TA_Base_Core::ReEntrantThreadLockable m_stateLock;
    };
}

#endif // !defined(AFX_STATEMANAGER_H__3C1DA7B8_2E3C_403B_A031_E14957561DBA__INCLUDED_)
