/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4669/transactive/bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2006/10/20 13:13:28 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is a class to serialise output to the screen
  * from multiple threads within the simulator
  *
  */

#include "core/utilities/src/ThreasdafeScreenOutput.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <iostream>

namespace TA_Base_Core
{
    NonReEntrantThreadLockable ThreasdafeScreenOutput::m_coutLock;
    NonReEntrantThreadLockable ThreasdafeScreenOutput::m_cerrLock;

    void ThreasdafeScreenOutput::outputToCout( const std::string& text )
    {
        ThreadGuard guard( m_coutLock );

        std::cout << text;
    }

    void ThreasdafeScreenOutput::outputToCerr( const std::string& text )
    {
        ThreadGuard guard( m_cerrLock );

        std::cerr << text;
    }

} // TA_Base_Core

