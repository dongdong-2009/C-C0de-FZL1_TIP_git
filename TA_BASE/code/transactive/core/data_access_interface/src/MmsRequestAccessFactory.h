/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAccessFactory.h $
 * @author:  Luo HuiRong
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * MmsRequestAccessFactory is a singleton that is used to retrieve MmsRequestData objects from the database.
 * It provides data retrieving, data saving functions. 
 *  
 */


#if !defined(MmsRequestAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
#define MmsRequestAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_

#include <vector>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/MmsRequestAlarmData.h"
#include "core/data_access_interface/src/MmsRequestScheduledData.h"
#include "core/data_access_interface/src/MmsRequestSchematicData.h"
 

namespace TA_Base_Core
{
	class IDatabase;
	class MmsRequestAccessFactory
	{

		public:
            virtual ~MmsRequestAccessFactory() {};

			/**
			 * getInstance
			 *
			 * Creates and returns an instance of this object.
			 *
			 * @return A reference to an instance of an MmsRequestAccessFactory object.
			 */
			static MmsRequestAccessFactory& getInstance();

			/**
			* saveMmsRequestAlarms
			*
			* Saving a group of MMS request of alarm into database		 
			*
			* @param MmsRequests The MmsRequestAlarmData object to be saved.
			*
			* @return true means the data are saved into database successfully, fails means failed.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/

			//bool saveMmsRequestAlarms(std::vector<MmsRequestAlarmData*>& MmsRequests );
			

			/**
			* saveMmsRequestAlarm
			*
			* Saving MMS request of alarm into database		 
			*
			* @param MmsRequests The MmsRequestAlarmData object to be saved.
			*
			* @return true means the data is saved into database successfully, fails means failed.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/

			bool saveMmsRequestAlarm(MmsRequestAlarmData& MmsRequest);




			/**
			* saveMmsRequestSchematic
			*
			* Saving MMS request from schematic into database		 
			*
			* @param MmsRequests The MmsRequestSchematicData object to be saved.
			*
			* @return true means the data is saved into database successfully, fails means failed.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/

			bool saveMmsRequestSchematic(MmsRequestSchematicData& MmsRequest);



			/**
			* MmsRequestScheduledData
			*
			* Saving MMS request for preventive into database		 
			*
			* @param MmsRequests The MmsRequestScheduledData object to be saved.
			*
			* @return true means the data is saved into database successfully, fails means failed.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/
			bool saveMmsRequestScheduled(MmsRequestScheduledData& MmsRequest);

		
			/**
			* getMmsRequestAlarmData
			*
			* get MMS request of alarm	 
			*
			* @param maxSize, The max records will be gotten from database
			*
			* @return a list of MmsRequestAlarmData.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/
			std::vector<MmsRequestAlarmData*> getMmsRequestAlarmData(unsigned long maxSize=10000);
			std::vector<MmsRequestSchematicData*> getMmsRequestSchematicData(unsigned long maxSize=10000);
			std::vector<MmsRequestScheduledData*> getMmsRequestScheduledData(unsigned long maxSize=10000);

			
			/**
			* getMmsRequestAlarmData
			*
			* get MMS request of alarm	 
			*
			* @param maxSize, The max records will be gotten from database
			*
			* @return a list of MmsRequestAlarmData.
			*
			* @exception DatabaseException A DatabaseException will be thrown if there are
			*            any problems in communicating with the database. The possible 
			*            reasons are:
			*            1) Invalid connection string (as stored in RunParams)
			*            2) Database is not/cannot be opened
			*            3) Error while attempting to execute a database query		 
			*/
			bool deleteMmsRequestAlarmData(std::vector<MmsRequestAlarmData*>& vec );
			bool deleteMmsRequestSchematicData(std::vector<MmsRequestSchematicData*>& vec);
			bool deleteMmsRequestScheduledData(std::vector<MmsRequestScheduledData*>& vec);
			
		private:
            MmsRequestAccessFactory() {};
			MmsRequestAccessFactory( const MmsRequestAccessFactory& theMmsRequestAccessFactory);
			MmsRequestAccessFactory& operator=(const MmsRequestAccessFactory &);

			MmsRequestAlarmData* createMmsRequestAlarmData( unsigned long row, IData* data );
			MmsRequestSchematicData* createMmsRequestSchematicData( unsigned long row, IData* data );
			MmsRequestScheduledData* createMmsRequestScheduledData( unsigned long row, IData* data );

			/**
			* getInsertSQL
			*
			* get insertion SQL of MmsRequestAlarmData		 
			*
			* @param MmsRequests The MmsRequestAlarmData object need to be inserted.
			*
			* @return insertion SQL.
			*		 
			*/
			//std::string  getInsertSQL(IDatabase* databaseConnection, MmsRequestAlarmData& MmsRequest);

			/**
			* getInsertSQL
			*
			* get insertion SQL of MmsRequestSchematicData		 
			*
			* @param MmsRequests The MmsRequestSchematicData object need to be inserted.
			*
			* @return insertion SQL.
			*		 
			*/
			//std::string  getInsertSQL(IDatabase* databaseConnection, MmsRequestSchematicData& MmsRequest);
			
			/**
			* getInsertSQL
			*
			* get insertion SQL of MmsRequestScheduledData		 
			*
			* @param MmsRequests The MmsRequestScheduledData object need to be inserted.
			*
			* @return insertion SQL.
			*		 
			*/
			//std::string  getInsertSQL(IDatabase* databaseConnection, MmsRequestScheduledData& MmsRequest);

			/**
			* getDeleteSQL
			*
			* get delete SQL of MmsRequestAlarmData		 
			*
			* @param MmsRequests The MmsRequestAlarmData object need to be deleted.
			*
			* @return insertion SQL.
			*		 
			*/
			std::string  getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestAlarmData*>& MmsRequests);

		    /**
			* getDeleteSQL
			*
			* get delete SQL of MmsRequestSchematicData		 
			*
			* @param MmsRequests The MmsRequestSchematicData object need to be deleted.
			*
			* @return insertion SQL.
			*		 
			*/
			std::string  getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestSchematicData*>& MmsRequest);
			
			/**
			* getDeleteSQL
			*
			* get delete SQL of MmsRequestScheduledData		 
			*
			* @param MmsRequests The MmsRequestScheduledData object need to be deleted.
			*
			* @return insertion SQL.
			*		 
			*/
			std::string  getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestScheduledData*>& MmsRequest);

			/**
			* getLocalDatabaseName
			*
			* returns the name of the local database
			*/
			std::string getLocalDatabaseName();

            static MmsRequestAccessFactory* m_instance;

			// The name of the local database to send updates to
			std::string m_localDatabase;

	};
} // closes TA_Base_Core

#endif // !defined(MmsRequestAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
