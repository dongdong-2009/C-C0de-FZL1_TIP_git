/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmStateAccessFactory.h $
 * @author:  Xiangmei.lu
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * AlarmTypeAccessFactory is a singleton that is used to retrieve AlarmTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#if !defined(AlarmStateAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
#define AlarmStateAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/AlarmStateData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class AlarmStateAccessFactory
	{

public:

        /**
         * getInstance
         *
         * Returns a reference to the only instance of DatalogSettingsAccessFactory.
         */
        static AlarmStateAccessFactory& getInstance();

		/**
		 * freeInstance
		 *
		 * deletes the object instance of this class.
		 *
		 * @return none.
		 */
		static void freeInstance();

		std::vector<AlarmStateData*> getAlarmStateData ();


    private:


        // Disabled methods.
        AlarmStateAccessFactory() 
		{
		}
        
        virtual ~AlarmStateAccessFactory(){}


        // Singleton object.
        static AlarmStateAccessFactory* m_instance;

		static NonReEntrantThreadLockable m_singletonLock;

        

       /**
         * createAlarmStateData
         *
         * Creates a AlarmStateData object from the IData object.
         *
         * @param row The row number to retrieve.
         * @param data The object containing the data.
         *
         * @return AlarmStateData* The AlarmStateData
         */
        AlarmStateData* createAlarmStateData( unsigned long row, IData* data );


	};
} // closes TA_Base_Core

#endif // !defined(AlarmStateAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
