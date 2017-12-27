///////////////////////////////////////////////////////////
//  DataLogItem.h
//  Implementation of the Class DataLogItem
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:14
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_2A3275C0_F84C_4909_A774_3A8B35CCF693__INCLUDED_)
#define EA_2A3275C0_F84C_4909_A774_3A8B35CCF693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <time.h> 
#include "core/data_access_interface/datalog/src/DatalogDPData.h"


namespace TA_Base_Bus
{
	class DataLogDPItem
	{

	public:
// 		DataLogDPItem(unsigned unsigned long pkey,
// 			const std::string datapointName, 
// 			const int deltaValue, 
// 			const bool dpEnable,
// 			const bool groupEnable, 
// 			const unsigned long entityKey, 
// 			const unsigned long groupID ,
// 			const IntervalType intervaType,
// 			const unsigned long sampleInterval ,
// 			const time_t startTime):
// 		m_PKey(pkey),m_datapointName(datapointName), m_deltaValue(deltaValue), m_dpEnable(dpEnable),
// 			m_groupEnable(groupEnable), m_entityKey(entityKey), m_groupID(groupID), m_intervaType(intervaType),
// 			m_sampleInterval(sampleInterval), m_startTime(startTime),m_hasDelayedTimes(0)
// 		{
// 		}


		DataLogDPItem(TA_Base_Core::DatalogDPData* dpData , int timeourDelay=0);

		virtual ~DataLogDPItem();

		
		void setPlanLogTime(time_t time);
		time_t getPlanLogTime();
		time_t getPlanLogTimeAddByDelay(); //get 
		int getDelayedTimes();
		void setDelayedTimes(int hasDelayedTimes);
		unsigned long getEntityKey();

		void initNextLogTime();

		time_t calculateNextPlanLogTime();
		void moveToNextPlanLogTime();
		void delay();
		unsigned long getFactorWhenInterValTypeNotMonth();
		void setNAdjustDay(tm & tm1, int day_beforeAdust);
		void addOneMorePeriodForMonthInterval( tm & time_tm );
		void initNextLogTimeForMonthInterval();
		bool checkNewValueForDelta(double newValue);
		void setCurrentValue( double value );
	     
		/**
		 * PKey
		 */
		unsigned long getPKey()
		{
			return m_PKey;
		}

		double getDeltValue()
		{
			return m_deltaValue;
		}

		TA_Base_Core::IntervalType getIntervalType()
		{
			return m_intervaType;
		}

		unsigned long getSampleInterval()
		{
			return m_sampleInterval;
		}

		time_t getStartTime()
		{
			return m_startTime;
		}

	unsigned long getGrpID()
	{
		return m_groupID;
	}

	bool getGroupEnabled()
	{
		return m_groupEnable;
	}


	private:
		bool m_firstTimeCheck;
		double m_currentValue;

		unsigned long m_PKey;
		/**
		 * entity name
		 */
		std::string m_datapointName;
		/**
		 * Delta value, 0: ignore this parameter;
		 */
		double m_deltaValue;
		/**
		 * Whether enable datalog based on DP setting
		 */
		bool m_dpEnable;

		/**
		* Whether enable datalog based on group setting
		*/
		bool m_groupEnable;

		/**
		 * Entity Key
		 */
		unsigned long m_entityKey;
		/**
		 * Group ID
		 */
		unsigned long m_groupID;
		/**
		 * the type of interval can only be the following characters.
		 * 1>"S",  second
		 * 2>"MI", minute
		 * 3>"H", hour
		 * 4>"D",day
		 * 5>"M", Month
		 */
		TA_Base_Core::IntervalType m_intervaType;

		/**
		 * Interval of saving sample data, the unit is based on interval type
		 */
		unsigned long m_sampleInterval;


		time_t m_planLogTime; // strictly caculated by start time and time interval

		int m_delayedTimes;
		int m_timeoutDelay;


		/**
		 * start time
		 */
		time_t m_startTime;

		//TA_Base_Bus::DataPointProxySmartPtr	m_scadaProxy;
		

	};
}
#endif // !defined(EA_2A3275C0_F84C_4909_A774_3A8B35CCF693__INCLUDED_)
