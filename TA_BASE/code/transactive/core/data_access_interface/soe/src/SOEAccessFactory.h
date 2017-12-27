#ifndef SOE_ACCESS_FACTORY_INCLUDED
#define SOE_ACCESS_FACTORY_INCLUDED

#include <vector>
#include <sys/timeb.h>

#include <core/synchronisation/src/NonReEntrantThreadLockable.h>
#include <core/synchronisation/src/ThreadGuard.h>

#include <core/data_access_interface/src/DatabaseFactory.h>
#include <core/data_access_interface/src/IData.h>
#include <core/data_access_interface/src/DbConnectionStrings.h>

#include "SOEEvent.h"
#include "SOEFilter.h"
//#include "ISOEEventReceiver.h"

namespace TA_Base_Core
{

	class SOEAccessFactory
	{
	private:
		SOEAccessFactory();
		
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		static SOEAccessFactory* m_self;
		
		std::vector<std::string> m_columns;	// columns list
		std::string m_strLocalDatabaseName;

        unsigned long getMaxPKey();

		// private method
		std::string getLocalDatabaseName();
	
	public:
		virtual ~SOEAccessFactory(void){}
		
		// get class singleton object
		static SOEAccessFactory& getInstance();
		
		void saveSOE( SOEEvent& evt );

		// retrieve SOE events
		//std::vector<SOEEvent*> retrieveSOEEvents( unsigned long* pAuditPKey, TA_Base_Core::SOEEventFilter* filter );
		std::vector<SOEEvent*> retrieveMoreSOEEvents();

		// generate time filter SQL
		//std::string generateTimeRangeSQL( SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection,time_t from, time_t to );

		//std::string generatePageTimeSQL( SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection,std::string strPageTime, bool bAscend );

		std::string generateLocationSQL(TA_Base_Core::IDatabase& databaseConnection, const std::vector<unsigned long>& locations );

		std::string generateAssetSQL(TA_Base_Core::IDatabase& databaseConnection,std::string strAsset );

		std::string generateDisplayValueSQL( TA_Base_Core::IDatabase& databaseConnection,std::string strDisplayValue );

		//std::string generateSortingSQL(SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection, std::string strSorColumn, bool bAscend );
		
	    
		/**
		* generateORList
		*
		* Generates a list of equality expressions separated by the OR operation.
		*
		* @param columnName The column name to compare.
		* @param keyList The list of keys to compare against.
		* @param databaseConnection Reference to the database object that is used.
		*
		* @return std::string The list of expressions separated by OR.
		*/
		std::string generateORList( const std::string& columnName, const std::vector<unsigned long>& keyList, TA_Base_Core::IDatabase& databaseConnection );


		//Li Zhongyan add for changing page record the event id bound
		std::string SOEAccessFactory::generateEventIDBoundSQL(double bottomEventId, bool bAscend);

		//Li Zhongyan add for getting recent coming event id
		//double getRecentEventID(IDatabase* db);

		unsigned long  m_maxReturnRow;

		unsigned long m_curMaxPkey;

        void generateWhereClause(SQLTypeAdapter& rSqlParam,TA_Base_Core::IDatabase& databaseConnection, const SOEEventFilter& filter, const std::string& timeRestrictionClause);
		std::vector<SOEEvent*> loadDesignatedEvents(const SOEEventFilter& filter, const std::string pageStartTime, const unsigned long endRow);
		std::vector<SOEEvent*> getDesignatedEventsSinceLastLoad();

		//std::string m_lastWhereClause;

		SOEEventFilter  m_lastFilter;

//		double get
	};


}	// namespace TA_Base_Core


#endif // #ifndef SOE_ACCESS_FACTORY_INCLUDED