/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/StateManager.cpp $
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

#include "app/data_management/archive_manager/src/StateManager.h"
#include "core/synchronisation/src/ThreadGuard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    StateManager::StateManager()
        : m_state( IDLE )
    {
        // Do nothing.
    }


    StateManager::EState StateManager::getState()
    {
        TA_Base_Core::ThreadGuard guard( m_stateLock );
        return m_state;
    }


    void StateManager::setState( EState state )
    {
        TA_Base_Core::ThreadGuard guard( m_stateLock );

        // The state can only be set to STOPPING if the 
        // current state is PROCESSING.

        if ( ( STOPPING == state) && ( PROCESSING != m_state ) )
        {
            return;
        }
        m_state = state;
    }
}
