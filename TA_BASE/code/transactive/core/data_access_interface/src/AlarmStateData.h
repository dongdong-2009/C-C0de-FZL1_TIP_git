/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmStateData.h $
 * @author:  Xiangmei.lu
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2011/1/6 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * AlarmStateData is an implementation of IItem. It holds the data specific to a Alarm
 * state entry in the database.
 */

#if !defined(AlarmStateData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)
#define AlarmStateData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_

#include <string>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

	class AlarmStateData 
	{

	public:

		AlarmStateData( unsigned long key,
            const std::string& name,
            const std::string& displayName,
			unsigned long typeKey): m_key(key), m_name(name),m_displayName(displayName),m_typeKey(typeKey)
		{}


        /**
		 * AlarmStateData (deconstructor)
		 *
		 * Deconstruct a AlarmStateData class.
		 *
		 */
		virtual ~AlarmStateData(){};

	
		/**
         * getKey
         *
         * Returns the key for this AlarmState.
         *
         * @return The key for this AlarmState as an unsigned long.
         */
		unsigned long getKey();


		std::string getName();


		std::string getDisplayName();

		unsigned long getTypeKey();


	private:
		unsigned long m_key;
		std::string m_name;
		std::string m_displayName;
		unsigned long m_typeKey;

	};
} // closes TA_Base_Core

#endif // !defined(AlarmStateData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)
