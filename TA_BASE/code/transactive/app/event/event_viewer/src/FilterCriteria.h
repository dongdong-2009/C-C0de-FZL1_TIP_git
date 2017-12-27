/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/FilterCriteria.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * FilterCriteria is the filter criteria for each display task( either real time mode or historical mode) 
  * in EventViewer. It will be used for filtering events in searching or refreshing.
  *
  */

#if !defined(EA_300D5A5F_57A7_4c67_BE4B_8FB6611F942C__INCLUDED_)
#define EA_300D5A5F_57A7_4c67_BE4B_8FB6611F942C__INCLUDED_


#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <ctime>

#include "../../../../../cots/IATSAlarmEvent/include/IATSEventsAPI.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"

using namespace std;


namespace TA_Base_App{

	typedef std::vector<unsigned long> NumericList;

    /**
	 * the direction in searching for the historical data in DB.
	 * Backward means we will care the later events more, 
	 * Forward measn we will care the earlier events more.
	 * This direction is a cutting strategy after we get data from DB 
	 * and the records' size is beyond the max size for GUI to show.
	 * 
	 */
	enum SearchDirection
	{
		Backward,
		Forward
	};

	struct ISCSDateTime
	{
		/// Year (eg 2010)
		short int Year;

		/// Month of year. Valid range 1-12 inclusive
		short int Month;

		/// Day of month. Valid range 1-31 inclusive
		short int Day;

		/// Hour of day. 24 hour format.  Valid range 0-23 inclusive.
		short int Hour;

		/// Minute of hour. Valid range 0-59 inclusive.
		short int Minute;

		/// Second of Minute. Valid range 0-59 inclusive.
		short int Second;

		/// Nanoseconds of second.	Valid range 0-999999999
		long int Nanosecond;

		ISCSDateTime(int year=0,int month=0, int day=0,
			int hour=0,int minute=0,int sec=0,long int nan=0):
		Year(year),Month(month),Day(day),Hour(hour),Minute(minute),Second(sec),Nanosecond(nan){
		}
		void addSec (short int sec);

		time_t getTime_t();

		ISCSDateTime(const ISCSDateTime& time);

		void setByATSDateTime(const IATSEventsAPI::ATSDateTime& time);

		ISCSDateTime & operator =(const ISCSDateTime& time);

		bool equalTo(ISCSDateTime & time);

	};

	enum FilterItermToCheck
	{
		StartTime,
		EndTime,
		Asset,
		SubsystemID,
		PhysicalSubsystemID,
		LocationID,
		OperatorID,
		SystemID,
		EventTypeID,
		TMType,
		Description,
		SeverityKey,
		AlarmType,
		AlarmID,
		EventLevel   //EventLevel is not uesd in 955 

	};

	class FilterCriteria
	{

	public:
		/// If true, parameter StartTimePeriod will be used in filter criteria.
		/// If false, StartTimePeriod will be ignored.
		bool StartTimePeriodDefined;

		/// This parameter allows the caller to define an underlying period of events to request from
		/// If defined, the ISCS will not include events raised before StartTimePeriod
		ISCSDateTime StartTimePeriod;

		/// If true, parameter EndTimePeriod will be used in filter criteria.
		/// If false, EndTimePeriod will be ignored.
		bool EndTimePeriodDefined;

		/// This parameter allows the caller to define an underlying period of events to request from
		/// If defined, the ISCS will not include events raised after EndTimePeriod
		ISCSDateTime EndTimePeriod;

		/// Filter by Asset
		list<string> Asset;

		/// Filter by Operator
		list<string> OperatorEqualOr; //changed by Luxiangmei 

		/// Filter by Location
		list<string> LocationEqualOr; //changed by Luxiangmei 

		/// Filter by Value
		string value;


		/// Filter by EventType (Alarmstates) 
		list<string> EventTypeEqualOr;

		/// Filter by SystemName
		list<string> SystemNameEqualOr;

		/// Filter by SubsystemName
		list<string> SubsystemNameEqualOr;

		/// Filter by AlarmID
		list<string> AlarmIDEqualOr;

		/// Filter by AlarmType
		list<unsigned long> AlarmTypeEqualOr;

		/// Filter by TMType
		list<string> TMTypeEqualOr; 

		string avalancheHeadID;// related to the avalanche head if applicable    

		/// Filter by AlarmSeverity
		list<unsigned long> AlarmSeverityEqualOr;

		list<string> Description; 

		NumericList systemIds;
		NumericList operatorIds;
		std::vector<int> severityIds;
		NumericList appSubsystemIds;
		NumericList physSubsystemIds;
		NumericList locationIds;
		NumericList alarmTypes;
		NumericList eventTypeIds;
		NumericList profiles;


		// Order in which to return the events.
		TA_Base_Core::CombinedEventAccessFactory::EventSortColumn sortColumn;
		TA_Base_Core::CombinedEventAccessFactory::EventSortOrder sortOrder;

		unsigned long eventLevel;

		SearchDirection searchDirection;

		std::string pkeyBoundary_ATS;   // for the case when searchMode=PreviousPage or NextPage
		unsigned long pkeyBoundary_ISCS;


// 		//Filter field check flag
// 		bool m_checkStartTime ;
// 		bool m_checkEndTime;
// 		bool m_checkAsset;
// 		bool m_checkSubsystemId ;
// 		bool m_checkLocId ;
// 		bool m_checkOperatorId ;
// 		bool m_checkSystemId ;
// 		bool m_checkEventTypeId ;
// 		bool m_checkTMType ;
// 		bool m_checkDesc ;
// 		bool m_checkSeveirtyKey;
// 		bool m_checkAlarmType;
// 		bool m_checkAlarmId ;
// 		bool m_checkEventLevel ;



		vector<FilterItermToCheck> m_checkItems;

		bool m_neverPass;


		/**
		 * Copy Constructor
		 */
		FilterCriteria(const FilterCriteria& f);
		
		/**
		 *  Constructor
		 */
		FilterCriteria();

		/**
		 *  Destructor
		 */
		~FilterCriteria();

		/**
		 *  equalTo
		 *
		 *  comparing function for compare this object to another FilterCriteria.
		 * 
		 *  @param filterCri, the one being compared with this object.
		 * 
		 *  @return true if the the FilterCriteria is equal, false if unequal.
		 */
		bool equalTo(FilterCriteria filterCri);

		/**
		 *  operator =
		 *
		 *  assign value to this object by another one.
		 * 
		 *  @param filterCri, the one used in assigning value.
		 * 
		 *  @return the FilterCriter object which is after being assigned value.
		 */
		FilterCriteria & operator =(const FilterCriteria& filterCri);


		/**
		 *  strListEqual
		 *
		 *  comparing 2 list<string>.
		 * 
		 *  @param list1 and list2, the 2 list to be compared
		 * 
		 *  @return true if same, false if different.
		 */
		bool strListEqual( std::list<string> & list1, std::list<string> & list2 );

		/** 
		 * setPrevious1dayForStartTime
		 *
		 * set startTimePeriod as currenttime-1day
		 */
		void setPrevious1dayForStartTime();

		bool isNeverPass();

		void analyzeFilter();

		static int s_totalAlarmSeverityNumber;

		static bool s_totalAlarmSeveirtyNumLoaded;

		void loadtotalAlarmSeverityNum();

	};


};

#endif // !defined(EA_300D5A5F_57A7_4c67_BE4B_8FB6611F942C__INCLUDED_)
