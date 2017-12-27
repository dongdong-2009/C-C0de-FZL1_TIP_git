/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/AlarmDisplayFilterData.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 14:06:39 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * AlarmDisplayFilterData is an interface to the Alarm_Display_Filter table. It provides both read and write access
  * for agents and user applicaitons.
  */


#ifndef __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
#define __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__

#include "core/data_access_interface/src/IAlarmDisplayFilterData.h"
#include <string>

namespace TA_Base_Core
{
	class AlarmDisplayFilterData
		: public IAlarmDisplayFilterData
	{
	public: //Methods
		
		/*
		* Constructor
		*/
		AlarmDisplayFilterData()
			: m_isValidData(false)
			, m_pkey(0)
			, m_isNew(true)
			, m_isLocationKeyModified(false)
			, m_isAlarmLocationKeyModified(false)
			, m_isAlarmTypeModified(false)
			, m_isSubSystemKeyModified(false)
			, m_isDisplayConditionModified(false)
			, m_isDeletedModified(false)
		{}

		/*
		* Constructor
		*/
		AlarmDisplayFilterData(unsigned long _pkey)
			: m_isValidData(false)
			, m_pkey(_pkey)
			, m_isNew(false)
			, m_isLocationKeyModified(false)
			, m_isAlarmLocationKeyModified(false)
			, m_isAlarmTypeModified(false)
			, m_isSubSystemKeyModified(false)
			, m_isDisplayConditionModified(false)
			, m_isDeletedModified(false)
		{};

		/*
		* Constructor
		*/
		AlarmDisplayFilterData(
				unsigned long _pkey,
				unsigned long _locationKey,
				unsigned long _alarmType,
				unsigned long _subSystemKey,
				unsigned long _alarmLocationKey,
				unsigned long _displayCondition,
				unsigned long _deleted);


		/*
		* Destructor
		*/
		virtual ~AlarmDisplayFilterData(){};

		/**
         * getKey
         */
		virtual unsigned long getKey();

		/**
         * getLocationKey
         */
		virtual unsigned long getLocationKey();

		/**
         * setLocationKey
         */
		virtual void setLocationKey(const int& _nLocationKey);

		/**
         * setAlarmLocationKey
         */
		virtual unsigned long getAlarmLocationKey ();
		
		/**
         * getAlarmLocationKey
         */
		virtual void setAlarmLocationKey(const unsigned long& _nLocationKey);

		/**
         * getAlarmType
         */
		virtual unsigned long getAlarmType(); 
		
		/**
         * setAlarmType
         */
		virtual void setAlarmType(const unsigned long& _nAlarmType); 

		/**
         * getSubSystemKey
         */
		virtual unsigned long getSubSystemKey(); 
		
		/**
         * setSubSystemKey
         */
		virtual void setSubSystemKey(const unsigned long& _nSubSystemKey); 

		/**
         * getAlarmLocationKey
         */
		virtual IAlarmDisplayFilterData::DISPLAY_CONDITION getDisplayCondition();
		
		/**
         * setDisplayCondition
         */
		virtual void setDisplayCondition(const DISPLAY_CONDITION& _nDisplayCondition);

		/**
         * getDeleted
         */
		virtual unsigned long getDeleted();
		
		/**
         * setDeleted
         */
		virtual void setDeleted(const unsigned long& _isDeleted);

		/**
         * applyChanges
         */
		void applyChanges();

		/**
         * invalidate
         */
		void invalidate();

	private: // Methods
		void _reload();
		void _update();
		void _createNew();
		std::string getLocalDatabaseName();

	private: //Variables
		unsigned long m_pkey;
		unsigned long m_locationKey;
		unsigned long m_alarmType;
		unsigned long m_alarmLocationKey;
		unsigned long m_subSystemKey;
		DISPLAY_CONDITION m_displayCondition;
		unsigned long m_deleted;

		bool m_isLocationKeyModified;
		bool m_isAlarmLocationKeyModified;
		bool m_isSubSystemKeyModified;
		bool m_isDisplayConditionModified;
		bool m_isDeletedModified;
		bool m_isAlarmTypeModified;

		bool m_isValidData;
		bool m_isNew;
		std::string m_localDatabase;
	};

}//close namespace TA_Base_Core

#endif // __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
