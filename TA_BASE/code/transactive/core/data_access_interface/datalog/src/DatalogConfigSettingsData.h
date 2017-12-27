/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogConfigSettingsData.h $
  * @author:  
  * @version: $Revision: $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * DatalogConfigSettingsData Class respresnts data related to DataLog_Configsettings Table. 
  */

#if !defined(_DATALOG_CONFIG_SETTINGS_DATA_H_)
#define _DATALOG_CONFIG_SETTINGS_DATA_H_

#include <string>



namespace TA_Base_Core
{
	enum ConfigType
	{
		Trending,
		Report
	};

    class DatalogConfigSettingsData 
    {
    public:
        /**
         * Constructor
         *
         * This constructs a DatalogConfigSettingsData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         */
        DatalogConfigSettingsData(const unsigned long pkey,
						   const unsigned long locationKey,
                           const ConfigType configType,
                           const std::string& dpTabelName,
                           const std::string& dpGrpTabelName,
                           const std::string& dpLogTabelName,
						   const unsigned long versionNum,
						   const std::string& dpViewTableName) :
        m_pkey(pkey),m_locationKey(locationKey), m_configType(configType),m_dpTableName(dpTabelName),
			m_dpGrpTableName(dpGrpTabelName), m_dpLogTableName(dpLogTabelName), m_versionNum(versionNum), 
			m_dpViewTableName(dpViewTableName)
        {
        }

		unsigned long      getPkey();
		unsigned long      getLocationKey();
		ConfigType         getConfigType();
		std::string        getDPTableName();
		std::string        getDPGrpTableName();
		std::string        getDPLogTableName();
		unsigned long      getVersionNum();
		std::string        getDPViewTableName();



    private:
   
        // Variables used to store event data.
        unsigned long      m_pkey;
		unsigned long      m_locationKey;
        ConfigType         m_configType;
        std::string        m_dpTableName;
        std::string        m_dpGrpTableName;
        std::string        m_dpLogTableName;
        unsigned long      m_versionNum;
		std::string        m_dpViewTableName;


    };
};

#endif // !defined(_DATALOG_CONFIG_SETTINGS_DATA_H_)