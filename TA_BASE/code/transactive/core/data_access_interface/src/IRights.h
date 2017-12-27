/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IRights.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IRights is an interface to a Rights object. It allows the Rights object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

namespace TA_Base_Core
{

    class IRights
    {

    public:
        virtual ~IRights() { };

		
        /**
         * getProfileKey
         *
         * Returns the profilekey for this rights configuration.
         *
         * @return The profile key for these rights as an unsigned long
         */
        virtual unsigned long getProfileKey() = 0;

        
        /**
         * getSubsystemKey
         *
         * Returns the subsystem key for this rights configuration.
         *
         * @return The subsystem key for these rights as an unsigned long
         */
        virtual unsigned long getSubsystemKey() = 0;

		
		virtual unsigned long getSubsystemStateKey() = 0;

        /**
         * getActionGroupKey
         *
         * Returns the ActionGroup key for this rights configuration.
         *
         * @return The ActionGroup key for these rights as an unsigned long
         */
        virtual long getActionGroupKey() = 0;


        /**
         * isPhysicalSubsystem
         *
         * This determines whether this particular right is for a physical or non-physical
         * subsystem
         *
         * @return bool - True if this right is for a physical subsystem. False otherwise
         */
        virtual bool isPhysicalSubsystem() =0;


    };
} //close namespace TA_Base_Core

#endif // !defined(IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
