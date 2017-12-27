/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogRecordingAccessFactory.h $
  * @author:  
  * @version: $Revision:  $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * DatalogRecordingAccessFactory is a singleton that is used by the Data Log Agent to
  * record the Trending/Report Data into the database.
  *
  */

#if !defined(_DATALOG_RECORDING_ACCESS_FACTORY_H_)
#define _DATALOG_RECORDING_ACCESS_FACTORY_H_
#include <vector>
#include <string>
#include "DatalogRecordingData.h"
#include "DatalogConfigSettingsData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

    class DatalogRecordingAccessFactory
    {
    public:


        /**
         * getInstance
         *
         * Returns a reference to the only instance of CombinedEventAccessFactory.
         */
        static DatalogRecordingAccessFactory& getInstance();


		/** 
		 *
		 * return bool ture successful , false unsuccessful
		 */ 
		bool insertRecords(std::vector<DatalogRecordingData*>& records, std::string tableName); 

		bool insertRecord(DatalogRecordingData* record, std::string tableName);

		std::string getLocalDatabaseName();
  

    private:


        // Disabled methods.
        DatalogRecordingAccessFactory() 
        {
        }
        
        virtual ~DatalogRecordingAccessFactory()
        {
        }

        // Singleton object.
        static DatalogRecordingAccessFactory* m_instance;

		static NonReEntrantThreadLockable m_singletonLock;
		
    };
};

#endif // !defined(_DATALOG_RECORDING_ACCESS_FACTORY_H_)
