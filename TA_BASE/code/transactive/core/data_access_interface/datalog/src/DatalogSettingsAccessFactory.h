/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogSettingsAccessFactory.h $
  * @author:  
  * @version: $Revision: $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * DatalogSettingsAccessFactory is a Singleton Class used by DataLog Agent to retrive the Config Settings
  * of the Trending/Report.
  *
  */

#if !defined(_DATA_LOG_SETTINGS_ACCESS_FACTORY_H_)
#define _DATA_LOG_SETTINGS_ACCESS_FACTORY_H_

#include <map>
#include <vector>
#include <sys/timeb.h>
#include <string>

#include "core/data_access_interface/src/IApplicationObserver.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "DatalogConfigSettingsData.h"
#include "DatalogDPData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "DatalogSettingsAccessFactory.h"
namespace TA_Base_Core
{

    class DatalogConfigSettingsData;  // for table Datalog_ConfigSettings
    class IDatabaseCallbackUser;

    class DatalogSettingsAccessFactory 
    {
    public:

        /**
         * getInstance
         *
         * Returns a reference to the only instance of DatalogSettingsAccessFactory.
         */
        static DatalogSettingsAccessFactory& getInstance();

		std::vector<DatalogConfigSettingsData*> getDatalogConfigSettingsData (unsigned long locationKey );
		std::vector<DatalogConfigSettingsData *> getDatalogConfigSettingsData ();

		std::vector<DatalogConfigSettingsData *> getNewDatalogConfigSettingsData(unsigned long locationKey, std::vector<unsigned long> existedPkeys);

		std::vector<DatalogDPData*> getDatalogDPData ( unsigned long locationKey, std::string tableName);

		bool loadVersionNumByKey(unsigned long pkey, unsigned long & newVersion);


    private:


        // Disabled methods.
        DatalogSettingsAccessFactory() 
		{
		}
        
        virtual ~DatalogSettingsAccessFactory(){}

        DatalogSettingsAccessFactory( const DatalogSettingsAccessFactory& );
        DatalogSettingsAccessFactory& operator=( const DatalogSettingsAccessFactory& );

        // Singleton object.
        static DatalogSettingsAccessFactory* m_instance;

		static NonReEntrantThreadLockable m_singletonLock;

        

       /**
         * createDatalogSettingData
         *
         * Creates a DatalogSettingsData object from the IData object.
         *
         * @param row The row number to retrieve.
         * @param data The object containing the data.
         *
         * @return DatalogSettingsData* The DatalogSettings data.
         */
        DatalogConfigSettingsData* createDatalogConfigSettingsData( unsigned long row, IData* data );

		DatalogDPData* createDatalogDPData( unsigned long row, IData* data );
		
    };
};

#endif // !defined(_DATA_LOG_SETTINGS_ACCESS_FACTORY_H_)
