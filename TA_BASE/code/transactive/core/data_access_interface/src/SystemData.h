/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemData.h $
  * @author:  Lu Xiangmei
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/25 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is a data structure for "System" data, which also correspond with the table "System"
  *
  * All data is populated when the object is constructed and is read-only.
  */

#if !defined(_SYSTEM_DATA_H_)
#define _SYSTEM_DATA_H_

#include <string>



namespace TA_Base_Core
{
	
    class SystemData 
    {
    public:
        /**
         * Constructor
         *
         * This constructs a DatalogConfigSettingsData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         */
        SystemData
			(const unsigned long pkey,
							std::string name,
						   const bool is_physical,
						   const bool is_exclusive) :
        m_pkey(pkey),m_name(name), m_isPhysical(is_physical), m_isExclusive(is_exclusive)
        {
        }

		unsigned long      getPkey();
		std::string        getName();
		bool			   getIsPhysical();
		bool               getIsExclusive();


    private:
   
        // Variables used to store event data.
        unsigned long      m_pkey;
		std::string        m_name;
		bool               m_isPhysical;
		bool               m_isExclusive;

    };
};

#endif // !defined(_SYSTEM_DATA_H_)