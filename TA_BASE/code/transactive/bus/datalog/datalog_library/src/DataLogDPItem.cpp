///////////////////////////////////////////////////////////
//  DataLogItem.cpp
//  Implementation of the Class DataLogItem
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:14
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#include "ace/OS.h" 

#include "DataLogDPItem.h"
#include <sys/timeb.h>

namespace TA_Base_Bus
{

	const int MINUTE_INTERVAL = 60 ; //(IN SECS)
	const int HOUR_INTERVAL = 3600 ; //(IN SECS)
	const int DAY_INTERVAL = 86400 ; //(IN SECS)


	DataLogDPItem::DataLogDPItem(TA_Base_Core::DatalogDPData* dpData , int timeoutDealy /*=0*/)
	{
		m_PKey = dpData->getDPKey();
		m_datapointName = dpData->getDPName();

		m_deltaValue = dpData->getDeltaValue();
		if(m_deltaValue<0)
		{
			m_deltaValue = (-1)*m_deltaValue;
		}

		m_dpEnable = dpData->getDPEnabled();
		m_groupEnable = dpData->getDPGrpEnabled();
		m_entityKey = dpData->getEntityKey();
		m_groupID = dpData->getDPGrpKey();
		m_intervaType = dpData->getIntervalType();
		m_sampleInterval = dpData->getSampleInterval();
		m_startTime = dpData->getStartTime();
		m_currentValue = 0;
		m_firstTimeCheck = true;

		m_timeoutDelay = timeoutDealy;
		m_delayedTimes = 0;
		
	}

	void DataLogDPItem::initNextLogTime()
	{
		struct timeb current_timeb;
		m_delayedTimes = 0;
		ftime(&current_timeb);
		time_t current_timet = current_timeb.time;
		//time_t tt=time(NULL);
		if( m_startTime >= current_timet)
		{
			m_planLogTime = m_startTime;
		}
		else
		{
			if( m_intervaType != TA_Base_Core::Month )
			{
				unsigned long factor = getFactorWhenInterValTypeNotMonth();
				unsigned long remainder = (current_timet - m_startTime) % (m_sampleInterval * factor);
				if (remainder == 0)
				{
					m_planLogTime = current_timet;
				}
				else
				{
					m_planLogTime = current_timet + ( m_sampleInterval * factor - remainder );
				}
			}
			else /* m_intervaType == TA_Base_Core::Month*/
			{
				initNextLogTimeForMonthInterval();
			}
		}

		
		
	}

	void DataLogDPItem::initNextLogTimeForMonthInterval()
	{
		struct timeb current_timeb;
		ftime(&current_timeb);
		time_t current_timet = current_timeb.time;

		struct tm startTime_tm;
		struct tm planLogTime_tm;
		struct tm current_tm;
		ACE_OS::localtime_r(& m_startTime, & startTime_tm);
		ACE_OS::localtime_r( &current_timet,&current_tm);

		planLogTime_tm.tm_sec = startTime_tm.tm_sec;
		planLogTime_tm.tm_min = startTime_tm.tm_min;
		planLogTime_tm.tm_hour = startTime_tm.tm_hour;
		planLogTime_tm.tm_isdst = -1;

		unsigned long month_Remainder = 
			((current_tm.tm_year-startTime_tm.tm_year)*12 + current_tm.tm_mon - startTime_tm.tm_mon) % m_sampleInterval;

		if (month_Remainder == 0)
		{
			planLogTime_tm.tm_year = current_tm.tm_year;
			planLogTime_tm.tm_mon = current_tm.tm_mon;
			setNAdjustDay(planLogTime_tm, startTime_tm.tm_mday);

			//if this planLogTime_tm < current_time, should add one more period to it
			if( mktime(&planLogTime_tm) < mktime(&current_tm))
			{
				planLogTime_tm.tm_mday = startTime_tm.tm_mday; //set back to startTime's day
				addOneMorePeriodForMonthInterval( planLogTime_tm);
			}

		}else
		{
			planLogTime_tm.tm_year = current_tm.tm_year + ( current_tm.tm_mon + m_sampleInterval - month_Remainder ) / 12;
			planLogTime_tm.tm_mon = ( current_tm.tm_mon + m_sampleInterval - month_Remainder ) % 12;
			int day_beforeAdjust = startTime_tm.tm_mday;
			setNAdjustDay( planLogTime_tm , day_beforeAdjust);
		}

		// change tm to time_t
		m_planLogTime = mktime(& planLogTime_tm);
	}
	
	// just for interval type == Month, add one more period(several month) to the given time
	void DataLogDPItem::addOneMorePeriodForMonthInterval( tm & time_tm )
	{
		time_tm.tm_year = time_tm.tm_year + (time_tm.tm_mon + m_sampleInterval) /12;
		time_tm.tm_mon = (time_tm.tm_mon + m_sampleInterval) % 12;
		setNAdjustDay( time_tm, time_tm.tm_mday);

	}

	// if day(day_beforeAdust) is beyond the last day in the month, set earlier date to tm1
	void DataLogDPItem::setNAdjustDay(tm & tm1, int day_beforeAdust)
	{
		if(day_beforeAdust<=28)
		{
			tm1.tm_mday = day_beforeAdust;
			return;
		}

		else
		{
			// time_tm set day as 28 first, then add time_t: ( day_beforeAdust - 28)* 86400 seconds,
			// if the result's month changed, then time_tm minus time_t: 86400 seconds, if month change back to the old month, then ok.
			// if still havn't change back, minus again.

			int old_month = tm1.tm_mon;
			tm1.tm_mday = 28;
			time_t timet1 = mktime(&tm1);
			tm tm_temp;
			
			for (int tryTimes = 0; tryTimes<day_beforeAdust-28; tryTimes++)
			{
				timet1 = timet1 + 86400;
				ACE_OS::localtime_r( &timet1,&tm_temp );
				if ( tm_temp.tm_mon != tm1.tm_mon )
				{
					return;
				}else
				{
					tm1.tm_mday ++; 
				}
			}
		}

	}
	unsigned long DataLogDPItem::getFactorWhenInterValTypeNotMonth()
	{
		unsigned long factor = 1;
		if( m_intervaType == TA_Base_Core::Minute )
		{
			factor = MINUTE_INTERVAL;
		}
		else if( m_intervaType == TA_Base_Core::Hour )
		{
			factor = HOUR_INTERVAL;
		}
		else if( m_intervaType == TA_Base_Core::Day )
		{
			factor = DAY_INTERVAL;
		}else /*if( m_intervaType == TA_Base_Core::Second )*/
		{
			factor = 1;
		}

		return factor;
	
	}
	


	time_t DataLogDPItem::calculateNextPlanLogTime()
	{

		// go back to the logtime before delayed if it has been delayed.
		time_t res;
		res = m_planLogTime;

		if(m_intervaType == TA_Base_Core::Second )
		{
			res = res + m_sampleInterval;
		}
		else if(m_intervaType == TA_Base_Core::Minute )
		{
			res = res + m_sampleInterval*MINUTE_INTERVAL;
		}
		else if(m_intervaType == TA_Base_Core::Hour )
		{
			res = res + m_sampleInterval*HOUR_INTERVAL;
		}
		else if(m_intervaType == TA_Base_Core::Day)
		{
			res = res + m_sampleInterval*DAY_INTERVAL;  /* 3600*24  */
		}else if(m_intervaType == TA_Base_Core::Month )
		{
			struct tm res_tm; 
			ACE_OS::localtime_r( &res ,&res_tm );
			addOneMorePeriodForMonthInterval( res_tm );
			res = mktime( &res_tm);
		}

		return res;
	}

	void DataLogDPItem::moveToNextPlanLogTime()
	{
		setPlanLogTime(calculateNextPlanLogTime());
		m_delayedTimes = 0;
	}

	bool DataLogDPItem::checkNewValueForDelta(double newValue)
	{
		if (m_firstTimeCheck)
		{
			m_firstTimeCheck = false;
			return true;			
		}
		if( (newValue>= m_currentValue && newValue - m_currentValue >= m_deltaValue) 
			|| (newValue <= m_currentValue && m_currentValue - newValue >= m_deltaValue) )
		
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	void DataLogDPItem::setCurrentValue( double value )
	{
		m_currentValue = value;
	}

	time_t DataLogDPItem::getPlanLogTimeAddByDelay()
	{
		return m_planLogTime + m_delayedTimes*m_timeoutDelay ;
	}

	void DataLogDPItem::setPlanLogTime(time_t time)
	{
		m_planLogTime = time;
	}

	time_t DataLogDPItem::getPlanLogTime()
	{
		return m_planLogTime;
	}

	unsigned long DataLogDPItem::getEntityKey()
	{
		return m_entityKey;
	}

	int DataLogDPItem::getDelayedTimes()
	{
		return m_delayedTimes;
	}

	void DataLogDPItem::setDelayedTimes(int hasDelayedTimes)
	{
		m_delayedTimes = hasDelayedTimes;
	}

	void DataLogDPItem::delay()
	{
		m_delayedTimes ++;
	}
	DataLogDPItem::~DataLogDPItem()
	{

	}
	 
}