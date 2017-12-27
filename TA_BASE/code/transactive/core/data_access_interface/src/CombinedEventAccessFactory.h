/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/CombinedEventAccessFactory.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/27 12:49:16 $
  * Last modified by:  $Author: shurui.liu $
  *
  * CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
  * retrieve all data required to be presented to the user. It allows filtering and
  * sorting to done on the database side, and returns a vector of partially loaded
  * data to improve the efficiency.
  *
  */

#if !defined(_COMBINED_EVENT_ACCESS_FACTORY_H_)
#define _COMBINED_EVENT_ACCESS_FACTORY_H_

#include <map>
#include <vector>
#include <list>
#include <sys/timeb.h>

#include "core/data_access_interface/src/IApplicationObserver.h"
#include "core/data_access_interface/src/IDatabase.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class IData;
    class ICombinedEventData;
    class IDatabaseCallbackUser;

    class CombinedEventAccessFactory : public IApplicationObserver
    {
    public:
        enum EventSortColumn
        {
            SORT_CREATETIME,
            SORT_SEVERITY,
            SORT_ASSET,
			SORT_ALARM_STATUS,
            SORT_DESCRIPTION,
            SORT_VALUE,
			SORT_OPERATION_MODE,
            SORT_MMS,
            SORT_DSS,
            SORT_AVL,
            SORT_OPERATOR,
            SORT_COMMENT
        };

        enum EventSortOrder
        {
            SORT_ASCENDING,
            SORT_DESCENDING
        };

        typedef std::vector<unsigned long> NumericList;
		struct PhysicalSubsystemInfo{
			unsigned long	physicalSubsystemId;
			unsigned long	physicalSubsystemState;
		};	// Used for event visibility rules
		typedef std::vector< PhysicalSubsystemInfo > SubSysList;	// Used for event visibility rules
		typedef std::map< unsigned long, SubSysList > FullDutyMap;	// Used for event visibility rules
        typedef std::map<unsigned long,NumericList> NumericMap;

		struct LatestKeyNID
		{
			unsigned long auditPkey;
			unsigned long datapointPkey;
			unsigned long maxPkey;
			std::string event_id;
			LatestKeyNID()
			{
				auditPkey=0;
				datapointPkey=0;
				maxPkey=0;
				event_id = "";
			}
		};





        struct EventFilter
        {
            // The event level.
            unsigned long eventLevel;

            // Duty information required for filtering.
            bool dutyEnabled;
            NumericMap duty;
			FullDutyMap fullDuty;	// Used for event visibility rule
            // The time range.
            time_t fromTime;    // If 0 'from' will be 24 hours before 'to'.
            time_t toTime;      // If 0 'to' will be the current time.

            // Flags used for sorting.
            bool showNonAlarmEvents;

            // Strings to filter by.
            std::string description;
            std::string assetName;
			std::string tmType;   // added for 955
			std::vector<std::string> alarmId;

            // Keys to filter by.
            NumericList eventTypeIds; 
			NumericList systemIds;    // added for 955
            NumericList operatorIds;
            NumericList severityIds;
            NumericList appSubsystemIds;
            NumericList physSubsystemIds;
            NumericList locationIds;
            NumericList alarmTypes;

            // Order in which to return the events.
            EventSortColumn sortColumn;
            EventSortOrder sortOrder;

			std::string sessionId;	// Used for event visibility rule

			// TD15196 Wenguang++
			std::vector<unsigned long> profiles;	// Used for event visibility rule
        };

        /**
         * getInstance
         *
         * Returns a reference to the only instance of CombinedEventAccessFactory.
         */
        static CombinedEventAccessFactory& getInstance();

        /**
         * getEarliestEventTime
         *
         * Returns the time of the earliest event in the temporary table used
         * by the event viewer.
         *
         * @return time_t The time of the earliest event.
         */
        time_t getEarliestEventTime();

		/** 
		 * getLastEventID
		 *  
		 * get the last eventId 
		 */
		std::string getLastEventID();

        /**
         * getNextEvents
         *
         * Gets the next set of CombinedEventData objects resulting from the last call
         * to getFirstEventsOnPage.
         *
         * @return std::vector<ICombinedEventData*> The next set of events.
         */
        std::vector<ICombinedEventData*> getNextEvents();
		 

        /**
         * getEventsSinceLastLoad
         *
         * Returns a list of events that have occurred since the last time getEventsOnPage was called.
         * Only events which satisfy the provided filter will be returned.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
        std::vector<ICombinedEventData*> getEventsSinceLastLoad();

        /**
         * getDesignatedEventsSinceLastLoad
         *
         * Returns a list of new events for real-time refresh.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
     	std::vector<ICombinedEventData*> getDesignatedEventsSinceLastLoad(bool enableVisibilitRule);

		/** 
		 * updatePkey
		 *
		 * Update m_curMaxPkey when necessary by the given pkey.
		 * 
		 * @param pkey, the pkey value of a event.
		 *
		 */
		void updatePkey(unsigned long  pkey);

		// if (pkeyBoundary ==0) do the normal historicl query
		// if (pkeyBoundary !=0 && onNext == true), query "On Next page"
		// if (pkeyBoundary !=0 && onNext == false), query "On Previous page"
		std::vector<ICombinedEventData*> loadDesignatedEvents(
			bool enableVisibilitRule, 
			const EventFilter& filter, 
			const std::string pageStartTime, 
			const unsigned long endRow,
			unsigned long pkeyBoundary = 0,
			bool onNext = false);

		// if (pkeyBoundary ==0) do the normal historicl query
		// if (pkeyBoundary !=0 && onNext == true), query "On Next page"
		// if (pkeyBoundary !=0 && onNext == false), query "On Previous page"
		std::vector<ICombinedEventData*> loadDesignatedEventsInASection(
			unsigned long queryStartPkey,
			unsigned long queryEndPkey,
			std::string & firstPartSQLCache,
			bool enableVisibilitRule, 
			const EventFilter& filter, 
			const unsigned long endRow,
			unsigned long pkeyBoundary = 0,
			bool onNext = false);

        void resetStopFlag();

        //-------- Methods from IApplicationObserver --------//

        virtual void applicationTerminating();
        virtual void applicationPausing();
        virtual void applicationResuming();

		LatestKeyNID getMaxEventPKeyNEventId();

		/**
         * getLast24hrsEvents
         *
         * Returns a list of events that have occurred since for the past 24 hours.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
        std::list<ICombinedEventData*> getLatest24hrsEvents(unsigned long ulMaxEvents,unsigned long & ulLatestSeqId);

		/**
         * getLatestEvents
         *
         * Returns a list of events that have occurred based on the last sequence number provided in the parameter.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
		std::list<ICombinedEventData*> getLatestEvents(unsigned long ulLastSequenceNum, unsigned long & ulLatestSeqId);

		void CombinedEventAccessFactory::setMaxEventPkeyNEventId(CombinedEventAccessFactory::LatestKeyNID latestKeyNId);

		/** 
		 * getPkeySequenceMaxValue
		 *
		 * get the max value of sequence EV_COMBINED_SEQ
		 */
		unsigned long CombinedEventAccessFactory::getPkeySequenceMaxValue();

    private:

        // Disabled methods.
        CombinedEventAccessFactory() : m_stopLoad(false), m_pauseLoad(false), m_curMaxPkey(0),
              m_isLoadingAll(false),m_lastWhereClauseAlwaysFalse(false), m_latestEventID("")
        {
        }
        
        virtual ~CombinedEventAccessFactory()
        {
        }

        CombinedEventAccessFactory( const CombinedEventAccessFactory& );
        CombinedEventAccessFactory& operator=( const CombinedEventAccessFactory& );

        // Singleton object.
        static CombinedEventAccessFactory* m_instance;

        /**
         * getLatestPkeys
         *
         * Retrieves information on the newest element of each type in the database.
         *
         * @exception DatabaseException Thrown if there was a problem executing the query.
         *
         * @exception DataException Thrown if there was a problem extracting data.
         */
        LatestKeyNID getLatestPkeys();

       /**
         * createCombinedEvent
         *
         * Creates a CombinedEventData object from the IData object.
         *
         * @param row The row number to retrieve.
         * @param data The object containing the data.
         *
         * @return CombinedEventData* The combined event data.
         */
        ICombinedEventData* createCombinedEvent( unsigned long row, IData* data );
		ICombinedEventData* newCreateCombinedEvent( unsigned long row, IData* data );



        /**
         * generateWhereClause
         *
         * Generates conditions that can be placed in the WHERE clause to
         * retrieve only the required events.
         *
         * @param filter The filter to use when generating the WHERE clause.
         * @param databaseConnection Reference to the database object that is used.
         * @param timeRestrictionClause 
		 *
         * @return std::string The SQL that can be placed directly into the WHERE clause.
         */
        void generateWhereClause( 
			SQLTypeAdapter& rSqlParam,
			const EventFilter& filter, 
			const std::string& timeRestrictionClause, 
			TA_Base_Core::IDatabase& databaseConnection,
			unsigned long pkeyBoundary = 0,
			bool onNext = true);


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
        std::string generateORList( const std::string& columnName, const NumericList& keyList, TA_Base_Core::IDatabase& databaseConnection );

        /**
         * generateLocAndSubsystemExpr
         *
		 *
         * Generates the expressions for correctly matching the specified locations and
         * subsystems.
		 * This function is only used when visibility rule is not enabled
         *
         * @param filter The filter.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The expression.
         */
        std::string generateLocAndSubsystemExpr( const EventFilter& filter, TA_Base_Core::IDatabase& databaseConnection);


        /**
         * joinExpressions
         *
         * Joins the two expressions with the specified operation
         *
         * @param expr1 The first expression.
         * @param operation The operation.
         * @param expr2 The second expression.
         * @param bracketExpr Places each expression inside brackets.
         *
         * @return std::string The joined expression.
         */
        std::string joinExpressions( const std::string& expr1, const std::string& operation,
                                     const std::string& expr2, bool bracketExpr =true );

        /**
         * filterExcludesAll
         *
         * Determines if the specified filter is guaranteed to return no results.
         *
         * @param filter The filter to check.
         *
         * @return bool True if the filter will never return any rows.
         */
        bool filterExcludesAll( const EventFilter& filter, bool isVisibilityEnabled );

		void generateDesignatedWhereClause( 
			SQLTypeAdapter& rSqlParam,
			const EventFilter& filter, 
			const std::string& timeRestrictionClause ,
			TA_Base_Core::IDatabase& databaseConnection ,
			unsigned long pkeyBoundary = 0,
			bool onNext = true);

		void generateTimeStr ( 
			const EventFilter& filter, 
			const std::string& timeRestrictionClause ,
			unsigned long pkeyBoundary ,
			bool onNext,
			/*std::stringstream & timeString*/
			SQLTypeAdapter& rSqlTypeApt);
 

        // Variables required for application states.
        bool m_stopLoad;
        bool m_pauseLoad;



		unsigned long m_curMaxPkey;
		//unsigned long m_nextPkey;
        // Variable to remember the where clause.
        bool m_lastWhereClauseAlwaysFalse;
		EventFilter m_lastFilter;
		std::vector<ICombinedEventData*> loadAllEvents( const EventFilter& filter);

		// Variable to remember whether load all is selected.
		bool m_isLoadingAll;

		// Variables to remember the 3 where clause used in the query
		std::string m_lastDutyWhereClause;
		std::string m_lastOwnerWhereClause;
		std::string m_lastSystemWhereClause;
		std::string m_latestHourTimestamp;
		
		std::string m_latestEventID;
    };
};

#endif // !defined(_COMBINED_EVENT_ACCESS_FACTORY_H_)
