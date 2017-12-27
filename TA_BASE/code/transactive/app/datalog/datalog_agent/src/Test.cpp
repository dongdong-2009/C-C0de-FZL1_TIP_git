#include "Test.h"
#include "DataLogManager.h"
#include "DataLogCategory.h"
#include "DataLogDPItem.h"

#include "core/data_access_interface/datalog/src/DatalogSettingsAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "ace/OS.h" 

/************************************************************************/
/* Only for testing                                                     */
/************************************************************************/
namespace TA_Base_App
{

	Test::Test(void)
	{
		typedef std::map<unsigned long, DataLogCategory*> DataLogCategoryMap;
		DataLogCategoryMap m_CategoryMap;

		//load configuration setting from database
		const unsigned long agentLocationKey = 1;


		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading Config Setting data from DB starts.");
		std::vector<DatalogConfigSettingsData*> configSettings=TA_Base_Core::DatalogSettingsAccessFactory::getInstance().getDatalogConfigSettingsData(agentLocationKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading Config Setting data from DB ends. Get %d configSettings", configSettings.size());

		//create DataLogCategories, load & create datalog item from database 
		DataLogCategory* configData = NULL;
		for (int i=0; i<configSettings.size(); i++)
		{	
			configData = DataLogManager::getInstance()->createDataLogCategoryByConfigSetting(configSettings[i]);
			m_CategoryMap.insert(std::map<unsigned long,DataLogCategory*>::value_type(configData->getPKey(), configData) );
			configData->Init();

			std::map<unsigned long, DataLogDPItem*> dpMap = configData->getDPItemMap();

			//for each dp in the map, caculate the list of its planlogtime; 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Caculating next 30 plan log time for each DP.");
			for (std::map<unsigned long, DataLogDPItem*>::iterator itr=dpMap.begin(); itr!=dpMap.end(); itr++)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Caculating next 30 plan log time for DP.Pkey=%ld.",itr->second->getPKey() );

				for(int i=0; i<30; i++)
				{
					itr->second->moveToNextPlanLogTime();
					time_t nextLogTime = itr->second->getPlanLogTime();
					struct tm nextLogTime_tm ;
					ACE_OS::localtime_r( &nextLogTime, &nextLogTime_tm);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "For DP.pkey = %ld, next DP.planlogTime is %02d/%02d/%04d %02d:%02d:%02d", 
						itr->second->getPKey(),
						nextLogTime_tm.tm_mday, nextLogTime_tm.tm_mon + 1,
						nextLogTime_tm.tm_year + 1900, nextLogTime_tm.tm_hour, nextLogTime_tm.tm_min, nextLogTime_tm.tm_sec);

				}


			}

		}

	}

	Test::~Test(void)
	{
	}
}

