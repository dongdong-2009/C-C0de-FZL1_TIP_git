/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionCommand.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSEventWrapper is a wrapped class for ATS interface. In this class, 
  * functions accept general data type, e.g., ISCS types. And will transform them to ATS type when needed.
  * And it return result as general types ( which means it will transform ATS result to general type before it returns).
  *
  *
  */

#if !defined(EA_6C8ABB49_B7C9_4b51_9550_17DFFF482DC7__INCLUDED_)
#define EA_6C8ABB49_B7C9_4b51_9550_17DFFF482DC7__INCLUDED_

#include "ActionCommand.h"
#include "FilterCriteria.h"
#include <vector>
#include <list>
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "IEventWorker.h"
#include "../../../../../cots/IATSAlarmEvent/include/IATSEventsAPI.h"


using namespace IATSEventsAPI;

namespace TA_Base_App
{

	/** 
	 * eventSubscriptionCallback
	 * 
	 * the globle function used for ATS call back 
	 * when it sends the refreshing data in real time mode.
	 * 
	 * @param error, the status for this set of data 
	 * @param atsEvents, the refresh data gotten by ATS
	 *
	 */
	void eventSubscriptionCallback (const ATSSubscriptionError error, std::list<ATSEvent*>& atsEvents);

	class ATSEventWrapper
	{
		

	public:
		
		/**
		 * Constructor
		 *
		 */
		ATSEventWrapper();

		/**
		 * Destructor
		 *
		 */
		virtual ~ATSEventWrapper();


		/** 
		 * 
		 * startATSSubscription
		 * Starts a subscription for real time events with the ATS server.
		 * 
		 * @param filterCriteria, the filter used in this subscription.
		 * @param backloglength, the number of events to return in the backlog
		 * @param events. Reference to std::vector of events, which the function will populate.  
		 * Will contain the subscription's event backlog.
		 *
		 * @return ActionCommand, Error code for the subscription
		 */
		static ActionCommand startATSSubscription(
			FilterCriteria filterCriteria,
			const unsigned int backloglength,				
			std::vector<TA_Base_Core::ICombinedEventData*>& events);

		/**
		 * stopATSSubscription
		 *
		 * Stops a subscription.
		 *
		 * return ActionCommand , the erro code
		 */
		static ActionCommand stopATSSubscription(void);

		/** 
		 * get ths static object: ATSEventSubscription if it's already existed
		 * If it doesn't exist, create an ATSEventSubscription object and return.
		 *
		 * @return the point of ATSEventSubscription
		 */
		static IATSEventsAPI::ATSEventSubscription* getATSSubscriber();

	   /**
		* transformToISCSEvents
		* 
		* transform ATSEvent list to CombinedEventData vetor, and free the ATSEvent list.
		* 
		* @param atsEvents the ATSEvent list to be transformed
		* @param events. The CombinedEventData vetor to save the transformed data
		* @param lastKey, the last key value in this task. The key value is dynamic created in each searching task
		* and it's used for sorting.
		* 
		*/
		static void transformToISCSEvents ( 
			std::list<IATSEventsAPI::ATSEvent*>& atsEvents, 
			std::vector<TA_Base_Core::ICombinedEventData*>& events,  
			unsigned long lastKey=0 );

	/**
	 * ATSDateTimeCompare
	 * 
	 * comparing the 2 ATSDataTime. 
	 *
	 * @param time1,time2: the 2 ATSDataTime to be compared.
	 *
	 * @return bool. Ture if the 2 parameter are equal. Flase if unequal.
	 */
	static bool ATSDateTimeCompare(ATSDateTime time1 , ATSDateTime time2);
	
	/**
     * compare_ats_events
     *
     * This struct implements the std::binary_function template. It defines a method for comparing two DisplayItem pointers
     * to deterimine correct sort or positioning order.
     *
     * By using the std::binary_function template, this function can be used in conjunction with standard template functors
     * such as bind_1st and bind_2nd, which makes it quite powerful.
     */
    static struct compare_ats_events : public std::binary_function<ATSEvent*,ATSEvent*,bool>
    {    
        /**
         * operator()
         *
         * Determines the sort order of two DisplayItem pointers. By default, standard template functions will only do
         * pointer comparrisons (not useful), so this method de-references the pointers.
         *
         * @param arg1 The "left hand side" of the function
         * @param arg2 The "right hand side" of the function
         *
         * @return True if arg1 should come before arg2 under current sorting conditions.
         *
         * NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
         */
        bool operator()(ATSEvent* arg1,ATSEvent* arg2) const
        {
            return ATSDateTimeCompare( arg1->Timestamp , arg2->Timestamp);
        }
    };

	/**
	 * truncateATSEvents
	 *
	 * This funciton is used for cutting some records from ATSEvent List if the length is beyond the limit
	 * This function must be used on the premise that the atsEvents has been sorted by time ascending.
	 * 
	 * @param atsEvents, the list of ATSEvent which will be truncate if necessary.
	 * @param atsMaxLength, the length limit for the list.
	 * @param cutSmallTime, if true cut the earilier record when size beyond the limit, if false cut the later records.
	 * 
	 */
	static void truncateATSEvents (std::list<ATSEvent*>&atsEvents, unsigned long atsMaxLength, bool cutSmallTime = true);
	
	/**
	 * doStartTimeEndTimeQuery
	 *
	 * This function is used to query the ATS server for events.
	 * The function shall return a single std::vector of events.  The std::vector will contain all 
	 * events that were raised at, and after, “filterCriteria.startTime?and before “filterCriteria.endTime? that 
	 * match the filter criteria. But will be cut the earlier ones if the length is beyond the limit
	 *
	 * @return ActionCommand, Error code for the query
	 *
	 * @param  events, Reference to std::list of events, which the function will populate. 
	 * @param filterCriteria, the filter criteria
	 * @param atsMaxlength, the max size of the events. 
	 *
	 */
	static ActionCommand  doStartTimeEndTimeQuery(
		std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);
    
	/** 
	 * doPreviousPageQuery
	 * 
	 * This function is used to query the ATS server for events.
	 * The function shall return a single std::vector of events.  The std::vector will contain all 
	 * events that were raised at, and after, “filterCriteria.startTime?and before “filterCriteria.endTime? that 
	 * match the filter criteria, and also before the ID boundary in filtercriteria. The earlier events will be cut if the length is beyond the limit
	 *
	 * @return ActionCommand, Error code for the query
	 *
	 * @param  events, Reference to std::list of events, which the function will populate. 
	 * @param filterCriteria, the filter criteria
	 * @param atsMaxlength, the max size of the events. 
	 *
	 */ 
	static ActionCommand doPreviousPageQuery(
		std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);

	/** 
	 * doNextPageQuery
	 * 
	 * This function is used to query the ATS server for events.
	 * The function shall return a single std::vector of events.  The std::vector will contain all 
	 * events that were raised at, and after, “filterCriteria.startTime?and before “filterCriteria.endTime? that 
	 * match the filter criteria, and also after the ID boundary in filtercriteria. The later events will be cut if the length is beyond the limit
	 *
	 * @return ActionCommand, Error code for the query
	 *
	 * @param  events, Reference to std::list of events, which the function will populate. 
	 * @param filterCriteria, the filter criteria
	 * @param atsMaxlength, the max size of the events. 
	 *
	 */ 
	static ActionCommand doNextPageQuery(
		std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);

	/** 
	 * restartATSSubscriptionForRefresh
	 * 
	 * This function is for restarting ATS subscription. 
	 * It's called when ATS refreshing data returns error status.
	 */
	static void restartATSSubscriptionForRefresh();


	static bool ATSFilterTimeSpanLessThan1Sec(ATSFilterCriteria atsFilter);
	
	static void add1SecToATSFilterStartTime(ATSFilterCriteria & atsFilter);


protected:

	/** 
	 * transformToATSFilter
	 *
	 * This function is to transform FilterCriteria to ATSFilterCriteria type.
	 * 
	 * @param atsfilter, reference to ATSFilterCriteria which is the destination of the transformation.
	 * @param filterCriteria, the FilterCriatia which is the source of the transformation.
	 */ 
	static void transformToATSFilter(IATSEventsAPI::ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria);
	
	/** 
	 * setTMTypeToATSFilter
	 *
	 * This function is to set TMType in ATSFilterCriteria based on the FilterCriteria.
	 * 
	 * @param atsfilter, reference to ATSFilterCriteria which is the destination of the value setting.
	 * @param filterCriteria, the FilterCriatia which is the source of the value setting.
	 */ 
	static void  setTMTypeToATSFilter(IATSEventsAPI::ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria);
	
	/** 
	 * setStartEndTimeToATSFilter
	 *
	 * This function is to set startTimePeriod and endTimePeriod in ATSFilterCriteria based on the FilterCriteria.
	 * 
	 * @param atsfilter, reference to ATSFilterCriteria which is the destination of the value setting.
	 * @param filterCriteria, the FilterCriatia which is the source of the value setting.
	 */ 
	static void  setStartEndTimeToATSFilter(IATSEventsAPI::ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria);
	
	static TA_Base_Core::ICombinedEventData* createCombinedEvent ( ATSEvent* atsEvent,unsigned long key);

	static void cutIfIDEqualOrSmallerThan ( std::string pkeyBoundary, std::list<ATSEvent*> & atsEvents );
	
	static void cutIfIDEqualOrBiggerThan ( std::string pkeyBoundary, std::list<ATSEvent*> & atsEvents );
	
	static void mergeATSEvnts(std::list<ATSEvent*> & atsEvents1, std::list<ATSEvent*> & atsEvents2);

	static IATSEventsAPI::ATSEventSubscription* atsSubscriber;
	static bool ATS_func_debug_level;

	static void adjustATSFilterTime(IATSEventsAPI::ATSFilterCriteria & atsfilter);

		
	};
};
#endif // !defined(EA_6C8ABB49_B7C9_4b51_9550_17DFFF482DC7__INCLUDED_)
