/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/RightsChecker.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class is used to determine if a user has access rights for a certain item.
  */

#ifndef RIGHTS_CHECKER_H_THU_19_02_04
#define RIGHTS_CHECKER_H_THU_19_02_04

#include <map>

#include "bus/security/rights_library/src/RightsLibrary.h"

namespace TA_Base_Core
{
    class RightsLibrary;
}


namespace TA_Base_Bus
{
	class RightsChecker
	{
	public:
		/**
		 * Constructor
         *
         * @exception GenericGUIException - This may throw a GenericGUI exception of type
         *                                  UNKNOWN_ACCESS_RIGHTS if the rights cannnot be set up
         *                                  correctly.
		 */
		RightsChecker();


		/**
		 * Destructor
		 */
		~RightsChecker();


        /**
         * canPerformAction
         *
         * This checks if the specified action can be performed on the Configuration Editor
         * by the current session. If there is no session because we are running connected straight
         * to the database this will always return true
         *
         * @param unsigned long - The entity key to test the action for
         * @param unsigned long - The action to test
         *
         * @return bool - true if the action can be performed, false otherwise
         */
        bool canPerformAction(unsigned long entityKey, unsigned long action);


    private:
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;


        // This is a cache of resource Id's for entities so we don't have to go back to the
        // database every time. The map is basically map<entitykey, resourceId>
        std::map<unsigned long, unsigned long> m_resourceIds;
	};
}

#endif RIGHTS_CHECKER_H_THU_19_02_04
