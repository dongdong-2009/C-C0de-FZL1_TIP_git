/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogDPData.h $
  * @author:  
  * @version: $Revision:  $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * This class represents DataLog Agent's Datapoint Data.
  */

#if !defined(_DATALOG_DP_DATA_H_)
#define _DATALOG_DP_DATA_H_

#include <string>
#include <time.h>

namespace TA_Base_Core
{

	enum IntervalType
	{
		Second,
		Minute,
		Hour,
		Day,
		Month
	};
    class DatalogDPData 
    {
		
    public:
        /**
         * Constructor
         *
         * This constructs a DatalogConfigSettingsData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         */
        DatalogDPData(const unsigned long entity_key,
						   const std::string&  DP_Name,
						   bool                DP_Enabled,
						   const unsigned long DP_GRP_key,
						   const std::string&  DP_GRP_Name,
						   const std::string&  DP_GRP_Desc,
						   unsigned long       sample_interval,
						   IntervalType        interval_type,
						   time_t              start_time,
						   bool                DP_GRP_Enabled,
						   double       delta_value,
						   unsigned long       location_key,
						   const unsigned long DP_Key = 0):   // when DP_Key = 0 , then this object is for inserting
        m_Entity_Key(entity_key), m_DP_Name(DP_Name), m_DP_Enabled(DP_Enabled), m_DP_GRP_Key(DP_GRP_key),
			m_DP_GRP_Name(DP_GRP_Name), m_DP_GRP_Desc(DP_GRP_Desc), m_Sample_Interval(sample_interval),
			m_Interval_Type(interval_type),m_Start_Time(start_time), m_DP_GRP_Enabled(DP_GRP_Enabled),
			m_Delta_Value(delta_value), m_Location_Key(location_key),m_DP_Key(DP_Key)
        {
        }

		unsigned long getDPKey();
		unsigned long getEntityKey();
		std::string   getDPName();
		bool          getDPEnabled();
		unsigned long getDPGrpKey();
		std::string   getDPGrpName();
		std::string   getDPGrpDesc();
		unsigned long getSampleInterval();
		IntervalType  getIntervalType();
		time_t		  getStartTime();
		bool          getDPGrpEnabled();
		double getDeltaValue();
		unsigned long getLocationKey();

    private:

        // Variables used to store data point data.
        unsigned long      m_DP_Key;
		unsigned long      m_Entity_Key;
        std::string        m_DP_Name;
		bool               m_DP_Enabled;
		unsigned long      m_DP_GRP_Key;
		std::string        m_DP_GRP_Name;
		std::string        m_DP_GRP_Desc;
		unsigned long      m_Sample_Interval;
		IntervalType       m_Interval_Type;
		time_t             m_Start_Time;
		bool               m_DP_GRP_Enabled;
		double      m_Delta_Value;
		unsigned long      m_Location_Key;

    };
};

#endif // !defined(_DATALOG_DP_DATA_H_)