/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/AlarmDisplayFilterAccessFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 14:06:39 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * AlarmDisplayFilterAccessFactory is an interface to the Alarm_Display_Filter table. It provides both read and write access
  * for agents and user applicaitons.
  */


#ifndef __ALARM_DISPLAY_FILTER_ACCESS_FACTORY_H_INCLUDED__
#define __ALARM_DISPLAY_FILTER_ACCESS_FACTORY_H_INCLUDED__

#include "core/data_access_interface/src/IAlarmDisplayFilterData.h"

#include <string>
#include <vector>

namespace TA_Base_Core
{
	class AlarmDisplayFilterAccessFactory
	{
	public: // Methods
		virtual ~AlarmDisplayFilterAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AlarmDisplayFilterAccessFactory object.
         */
        static AlarmDisplayFilterAccessFactory& getInstance();

		/**
		 * freeInstance
		 *
		 * deletes the object instance of this class.
		 *
		 * @return none.
		 */
		static void freeInstance();
	
		/**
		 * getFilterData
		 *
		 * Retrieves the Alarm Display Filter data based on the pkey.
		 *
		 * @return pointer to IAlarmDisplayFilterData.
		 *
		 * @note: Calling users is responsible for releasing the memory return by this function call.
		 */
		IAlarmDisplayFilterData* getFilterData(const unsigned long& _pkey);

		/**
		 * getLocationsFilterData
		 *
		 * Retrieves all the Alarm Display Filter data that matches the location key parameter.
		 *
		 * @return a vector of IAlarmDisplayFilterData pointer.
		 *
		 * @note: Calling users is responsible for releasing the memory return by this function call.
		 */
		std::vector<IAlarmDisplayFilterData*> getLocationsFilterData(const unsigned long& _locationKey);

		/**
		 * getAllFilterData
		 *
		 * Retrieves the all the Alarm Display Filter data from database.
		 *
		 * @return a vector of IAlarmDisplayFilterData pointer.
		 *
		 * @note: Calling users is responsible for releasing the memory return by this function call.
		 */
		std::vector<IAlarmDisplayFilterData*> getAllFilterData();


	private: // Methods
		AlarmDisplayFilterAccessFactory(){};
		//Disable
		AlarmDisplayFilterAccessFactory(const AlarmDisplayFilterAccessFactory& rhs);
		AlarmDisplayFilterAccessFactory& operator=(const AlarmDisplayFilterAccessFactory& rhs);

		std::string getLocalDatabaseName();

	private: // Variables
		static AlarmDisplayFilterAccessFactory* m_pInstance;
		std::string m_localDatabase;
	};


}//close namespace TA_Base_Core

#endif // __ALARM_DISPLAY_FILTER_ACCESS_FACTORY_H_INCLUDED__
