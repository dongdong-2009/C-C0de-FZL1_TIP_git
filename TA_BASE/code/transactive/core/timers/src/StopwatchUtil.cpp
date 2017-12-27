/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/StopwatchUtil.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * Class for peforming time-based operations with a
  * millisecond scale.
  */

#include "core/timers/src/StopwatchUtil.h"


namespace TA_Base_Core
{

	StopwatchUtil::StopwatchUtil()
	:
	m_total( 0 ),
	m_count( 0 )
	{

	}

	StopwatchUtil::~StopwatchUtil()
	{

	}

	void StopwatchUtil::startTiming()
	{
		ftime( & m_startTime );
	}

	unsigned long StopwatchUtil::stopTiming()
	{
		ftime( & m_stopTime );

		unsigned long diff =  ( ( m_stopTime.time * 1000 ) + m_stopTime.millitm ) -
							  ( ( m_startTime.time * 1000 ) + m_startTime.millitm );
	 //   if ( diff > 0 )
	  //  {
			m_total += diff;
			++m_count;
	  //  }
		return diff;
	}


	unsigned long StopwatchUtil::timeElapsed()
	{
		struct timeb tempTime;
		
		ftime( & tempTime );

		unsigned long diff =  ( ( tempTime.time * 1000 ) + tempTime.millitm ) -
							  ( ( m_startTime.time * 1000 ) + m_startTime.millitm );

		return diff;
	}

	void StopwatchUtil::reset()
	{
		m_count = 0;
		m_total = 0;
	}	


	double StopwatchUtil::getAverageTime()
	{
		return ( m_count > 0 ) ? ( double(m_total) / double(m_count) ): 0.0;
	}


	unsigned long StopwatchUtil::getTotalTime()
	{
		return m_total;
	}


	std::string StopwatchUtil::stats() 
	{
		unsigned long total = m_total;
		int totalHours = int(total / 3600000L);  total %= 3600000L;
		int totalMins  = int(total / 60000L);    total %= 60000L;
		int totalSecs  = int(total / 1000L);     total %= 1000L;
		int totalMSecs = int(total);

		double average = getAverageTime(); 
		long averageSecs  = long(average / 1000.0);
		double averageMSecs = average - averageSecs * 1000.0;

		char stats[128] = {0};
		sprintf(stats, 
				"[ Total: %dh %dm %ds %dms | Average: %lds %.3fms | Count: %lu ]",
				totalHours, totalMins, totalSecs, totalMSecs,
					averageSecs, averageMSecs, m_count);

		return std::string(stats);
	}

}
