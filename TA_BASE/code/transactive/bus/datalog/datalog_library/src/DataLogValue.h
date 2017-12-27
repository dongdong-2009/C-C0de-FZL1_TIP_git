///////////////////////////////////////////////////////////
//  DataLogValue.h
//  Implementation of the Class DataLogValue
//  Created on:      13-Ê®¶þÔÂ-2010 14:08:00
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_AABFB47D_5361_418b_8F30_DD15BEFF5419__INCLUDED_)
#define EA_AABFB47D_5361_418b_8F30_DD15BEFF5419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>


namespace TA_Base_Bus
{
	/**
	 * value of data log
	 */
	class DataLogValue
	{

	public:
		DataLogValue( tm createTime, unsigned long entitykey, double value, tm planLogTime):
		  m_createTime(createTime),m_entityKey(entitykey), m_value(value), m_planLogTime(planLogTime){
		  }

		virtual ~DataLogValue();

		tm getCreateTime();
		
		tm getPlanLogTime();

		unsigned long getEntityKey();

		double getValue();


	private:
		/**
		 * create time of the value
		 */
		tm m_createTime;
		/**
		 * Pkey of Entity
		 */
		unsigned long m_entityKey;
		/**
		 * value of data point
		 */
		double m_value;

		tm m_planLogTime;

	};
}
#endif // !defined(EA_AABFB47D_5361_418b_8F30_DD15BEFF5419__INCLUDED_)
