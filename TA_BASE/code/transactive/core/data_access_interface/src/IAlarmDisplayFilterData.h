/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/IAlarmSeverityData.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 14:06:39 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * IAlarmSeverityData is an interface to a Alarm severity object. It allows the Alarm severity object implementation
  * to be changed without changing the code that uses this interface.
  */


#ifndef __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
#define __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__

namespace TA_Base_Core
{
	class IAlarmDisplayFilterData
	{
	public:

		typedef enum
		{
			EXCLUDE = 0,
			INLCUDE
		} DISPLAY_CONDITION;

		/**
         * getKey
         *
         * Returns the key for this display filter.
         *
         * @return The key for this display filter for as an unsigned long.
         */
		virtual unsigned long getKey() = 0;

		// LocationKey
        virtual unsigned long getLocationKey() = 0;
		virtual void setLocationKey(const int& _nLocationKey) = 0;

		// AlarmType
		virtual unsigned long getAlarmType() = 0; 
		virtual void setAlarmType(const unsigned long& _nAlarmType) = 0; 

		// alarm location
		virtual unsigned long getAlarmLocationKey () = 0;
		virtual void setAlarmLocationKey(const unsigned long& _nLocationKey) = 0;

		// Sub System
		virtual unsigned long getSubSystemKey() = 0; 
		virtual void setSubSystemKey(const unsigned long& _nSubSystemKey) = 0; 

		// Display Condition
		virtual DISPLAY_CONDITION getDisplayCondition() = 0;
		virtual void setDisplayCondition(const DISPLAY_CONDITION& _nDisplayCondition) = 0;

		// Deleted
		virtual unsigned long getDeleted() = 0;
		virtual void setDeleted(const unsigned long& _isDeleted) = 0;
		/*
		* virtual destructor
		*/
		virtual ~IAlarmDisplayFilterData() {}

	};


}//close namespace TA_Base_Core

#endif // __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
