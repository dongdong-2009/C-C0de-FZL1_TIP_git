/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemAccessFactory.h $
  * @author:  Lu Xiangmei
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/26 15:06:58 $
  * Last modified by:  $Author: huirong.luo $
  *
  * CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
  * retrieve all data required to be presented to the user. It allows filtering and
  * sorting to done on the database side, and returns a vector of partially loaded
  * data to improve the efficiency.
  *
  */

#if !defined(_SYSTEM_ACCESS_FACTORY_H_)
#define _SYSTEM_ACCESS_FACTORY_H_

#include <map>
#include <vector>
#include <sys/timeb.h>
#include <string>

#include "core/data_access_interface/src/IApplicationObserver.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "SystemData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

    class SystemData;  // for table Datalog_ConfigSettings


    class SystemAccessFactory 
    {
    public:

        /**
         * getInstance
         *
         * Returns a reference to the only instance of DatalogSettingsAccessFactory.
         */
        static SystemAccessFactory& getInstance();

		std::vector<SystemData*> getAllSystems ();


    private:


        // Disabled methods.
        SystemAccessFactory() 
		{
		}
        
        virtual ~SystemAccessFactory(){}

        SystemAccessFactory( const SystemAccessFactory& );
        SystemAccessFactory& operator=( const SystemAccessFactory& );

        // Singleton object.
        static SystemAccessFactory* m_instance;

		static NonReEntrantThreadLockable m_singletonLock;

        

       /**
         * createSystemData
         *
         * Creates a SystemData object from the IData object.
         *
         * @param row The row number to retrieve.
         * @param data The object containing the data.
         *
         * @return SystemData* The DatalogSettings data.
         */
        SystemData* createSystemData( unsigned long row, IData* data );

		
		
    };
};

#endif // !defined(_SYSTEM_ACCESS_FACTORY_H_)
