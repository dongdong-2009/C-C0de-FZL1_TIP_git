/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/shared_alarm/src/SubsytemDataCache.h$
  * @author:  Lin Weikun
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/10/03 10:07:40 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * SubsystemDataCache is a singleton  that get subsystem from subsystem table
  */
#if !defined(SUBSYSTEMCACHE_H_H_H)
#define SUBSYSTEMCACHE_H_H_H

#include <vector>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/Subsystem.h"

#include "core/data_access_interface/src/SystemData.h"
#include "core\data_access_interface\src\SystemAccessFactory.h"

namespace TA_Base_Bus
{
	class SubsystemDataCache
	{
	public:
        virtual ~SubsystemDataCache();

		/**
		 * getInstance
		 *
		 * Creates and returns an instance of this object.
		 *
		 * @return A reference to an instance of an SubsystemDataCache object.
		 */
		static SubsystemDataCache& getInstance();
		static void releaseInstance();
		std::vector<TA_Base_Core::ISubsystem*> getSubsystemsFromSystems(std::vector<unsigned long> systemKeys);
		unsigned long getSystemKeyFromSubsystem(unsigned long subsystemKey);
		std::string getSystemNameFromSubsystem(unsigned long subsystemKey);
		std::string getSubSystemName(unsigned long subsystemKey);
		void InitializeSubsystem();
		void InitializeSystem();
	private:
		//std::vector<TA_Base_Core::ISubsystem*> m_subsystems;

		typedef std::map<unsigned long, TA_Base_Core::SystemData*> SystemT;
		typedef std::map<unsigned long, TA_Base_Core::ISubsystem*> SubSystemT;
		
		SystemT m_systems;
		SubSystemT m_subSystems;
		//
        // These are private as this method is a singleton
        //
		SubsystemDataCache();

		static SubsystemDataCache* m_instance;
		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif 