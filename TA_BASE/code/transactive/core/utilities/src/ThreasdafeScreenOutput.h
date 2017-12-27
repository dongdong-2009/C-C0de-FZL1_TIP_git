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


#ifndef ThreasdafeScreenOutput_H
#define ThreasdafeScreenOutput_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>
#include <sstream>


#define COUT( loggableData )\
{\
    std::ostringstream outputStream;\
    outputStream << loggableData;\
    TA_Base_Core::ThreasdafeScreenOutput::outputToCout( outputStream.str() );\
}\


#define CERR( loggableData )\
{\
    std::ostringstream outputStream;\
    outputStream << loggableData;\
    TA_Base_Core::ThreasdafeScreenOutput::outputToCerr( outputStream.str() );\
}\


namespace TA_Base_Core
{

    class ThreasdafeScreenOutput
    {
    
    public:

        static void outputToCout( const std::string& text );

        static void outputToCerr( const std::string& text );

    private:

        static NonReEntrantThreadLockable m_coutLock;

        static NonReEntrantThreadLockable m_cerrLock;

    };

} // TA_Base_Core

#endif // ThreasdafeScreenOutput_H