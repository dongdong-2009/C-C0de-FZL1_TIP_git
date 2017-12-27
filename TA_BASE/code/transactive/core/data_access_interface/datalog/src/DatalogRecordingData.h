/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogRecordingData.h $
  * @author:  
  * @version: $Revision:  $
  *
  * Last modification: $DateTime:  $
  * Last modified by:  $Author: $
  *
  *This class represents the information related to dat recording into the database.
  */

#if !defined(_DATALOG_RECORDING_DATA_H_)
#define _DATALOG_RECORDING_DATA_H_

#include "ace/OS.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Core
{
    class DatalogRecordingData 
    {
    public:
        /**
         * Constructor
         *
         * This constructs a DatalogRecordingData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         */
        DatalogRecordingData(const tm createTime,
                           const unsigned long entity_key,
                           const double value,
						   const unsigned long locKey,
						   const tm planLogTime,
						   const unsigned long key=0):
        m_createTime(createTime),m_entityKey(entity_key),m_value(value), 
			m_locationKey(locKey),m_key(key),m_planLogTime(planLogTime)
        {
        }

		const unsigned long getKey();

		const tm getCreateTime();

		const unsigned long getEntityKey();
			 
		const double getValue();

		const unsigned long getLocationKey();

		const tm getPlanLogTime();

    private:

        // Variables used to store event data.
        unsigned long      m_key;
        tm				   m_createTime;
		unsigned long      m_entityKey;
		double             m_value;
		unsigned long      m_locationKey;
		tm				   m_planLogTime;

    };
};

#endif // !defined(_DATALOG_RECORDING_DATA_H_)