/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ISubsystemState.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ISubsystemState is an interface to a SubsystemState object. It allows the SubsystemState 
  * object implementation to be changed (e.g. if necessary due to a database schema change) 
  * without changing code that uses it.
  */

#if !defined(ISubsystemState_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ISubsystemState_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

namespace TA_Base_Core
{

    class ISubsystemState
    {

    public:
        virtual ~ISubsystemState() { };

		
        /**
         * getKey
         *
         * Returns the subsystem state key for this stubsystem state.
         *
         * @return The subsystem state key for this subsystem state as an unsigned long
         */
        virtual unsigned long getKey() = 0;

        
        /**
         * getName
         *
         * Returns the subsystem state name for this subsystem state.
         *
         * @return The subsystem state name for these rights as string
         */
        virtual std::string getName() = 0;

        /**
         * getDescription
         *
         * Returns the subsystem state description for this subsystem state.
         *
         * @return The subsystem state description for these subsystem state as string
         */
        virtual std::string getDescription() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this subsystemState as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(ISubsystemState_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
