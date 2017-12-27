/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventHandlerManager.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventHandlerManager is ...
  *
  */

#if !defined(EA_99CD63B4_A375_48ed_98BE_2FEA99C5F2F5__INCLUDED_)
#define EA_99CD63B4_A375_48ed_98BE_2FEA99C5F2F5__INCLUDED_

#include "EventDisplayMode.h"
#include "EventHandler.h"
#include "app/event/event_viewer/src/GUIObserver.h"

#include "app/event/event_viewer/EventListComponent/src/Filter.h"


#include "core/data_access_interface/src/ICombinedEventData.h"

using namespace TA_Base_App;

namespace TA_Base_App{

	class FilterCriteria;
	enum EventDisplayMode;
	class EventHandler;


	class EventHandlerManager : public GUIObserver
	{

	public:

		virtual void reloadEvents();

		/** 
		 * constructor
		 */
		EventHandlerManager(Filter * filter);

		/** 
		 * desctructor
		 */ 
		virtual ~EventHandlerManager();

		/** 
		 * setParameter
		 *
		 * If the FilterCriteria and EventDisplayMode is just the same as last one, do nothing and return.
		 * Else, update UI as empty result first, and let m_historicalHandler (or m_realtimeHandler, 
		 * depending on the dispaly mode) stop current task and start the new task.
		 *
		 * @param searchfilter, the filter cirteria for the new task.
		 * @param mode, the display mode of the new task.
		 */
		void startNewTask(FilterCriteria searchfilter, EventDisplayMode mode);


		/** 
		 * initialHandler
		 * 
		 * create 2 handlers (m_realtimeHandler, m_historicalHandler), 
		 * and initialize observer for ATSRealTimeObserverManager
		 *
		 */
		void initialHandler();

		/** 
		 * getDisplayMode
		 *
		 * get the current displaymode and return.
		 * 
		 * @return EventDisplayMode, the current display mode.
		 */
		EventDisplayMode getDisplayMode();

		bool static isISCSSortingPriority();

		/** 
		 * refreshListView
		 *
		 * send message to EventViewer dialog to update the event list with the displaylist in current working handler.
		 * 
		 */
		virtual void refreshListView();
		
		/** 
		 * statusBarUpdateWhenStartQuery
		 * 
		 * send a message to eventViwer dialog to update the statusbar message(ATS status and ISCS status) 
		 * and set count information as 0. It is called at the beginning of a new task.
		 *
	     */
		virtual void statusBarUpdateWhenStartQuery();

    
		virtual void statusBarUpdate (const unsigned long eventCount,TA_Base_Core::EventCategory eventCategory , const string status );


		/**
		 * setMaxEvents
		 *
		 * Sets the eariles time from which this filter will retrieve database entries. 
         *
         * @param maxEvents The max number for this filter.
		 */
        virtual void setMaxEvents(const unsigned long maxvalue);
	    
		/**
		 * getMaxEvents
		 *
		 * Returns the maximum number of events that will be placed on a page.
		 *
		 * @return unsigned long The maximum number of events per page.
		 */
		static unsigned long getMaxEvents();

		/**
		 * setMaxISCSEvents
		 *
		 * Sets the max number of ISCS Event which the filter will retrieve. 
		 *
		 * @param maxEvents: The max number for ISCS event.
		 */
		static void setMaxISCSEvents( unsigned long maxISCS);

		/**
		 * getMaxISCSEvents
		 *
		 * Get the max number of ISCS Event which the filter will retrieve. 
		 *
		 * @return The max number for ISCS event.
		 */
		static unsigned long getMaxISCSEvents ();

		
		/**
		 * setMaxATSEvents
		 *
		 * Sets the max number of ATS Event which the filter will retrieve. 
		 *
		 * @param maxEvents: The max number for ATS event.
		 */
		static void setMaxATSEvents( unsigned long maxATS);
		
		/**
		 * getMaxATSEvents
		 *
		 * Get the max number of ATS Event which the filter will retrieve. 
		 *
		 * @return The max number for ATS event.
		 */
		static unsigned long getMaxATSEvents();
		
		static void setIsISCSSortingPriority();

		static bool getIsISCSSortingPriority();

		/**
		 * setRefreshRateDB
		 *
		 * Sets the refresh period of Events which the filter will use. 
		 *
		 * @param refreshRate: The refresh period.
		 */
		static void setRefreshRateDB( unsigned long refreshRate);

		/**
		 * setRefreshRateAgent
		 *
		 * Sets the refresh period from Agent
		 *
		 * @param refreshRate: The refresh period.
		 */
		static void setRefreshRateAgent( unsigned long refreshRate);

		static bool getRightsCheck();

		static int EventHandlerManager::getEventDetailInDebugLevel();

		/**
		 * getRefreshRateDB
		 *
		 * Get the refresh period of Events which the filter will use. 
		 *
		 * @return The refresh period.
		 */
		static unsigned long getRefreshRateDB();

		/**
		 * getRefreshRateAgent
		 *
		 * Get the refresh period from Event Agent 
		 *
		 * @return The refresh period.
		 */
		static unsigned long getRefreshRateAgent();
		
		void initialize_ATS_ISCS_MaxCount();
		//bool isSameTaskAsLastAndLoadReady(FilterCriteria searchfilter, EventDisplayMode mode);
		bool isStartNewTask(FilterCriteria searchfilter,EventDisplayMode mode);



		bool m_ATSHealthStatus;
		bool m_ISCSHealthStatus;
		bool m_ATSEnabled;
		bool m_ISCSEnabled;

		// this two message is for showing on the status bar on EventViewer dialog
		std::string m_msg_ISCS;
		std::string m_msg_ATS;
		std::string m_msg;

		EventHandler * m_realtimeHandler;
		EventHandler * m_historicalHandler;

		Filter * m_filter;

        
		// Configurable maximum event count limit
		static unsigned long s_maxEvents;
	private:
		EventDisplayMode m_curMode;
		long m_taskSeqID;
		
		void initialize2Switch();
		
		void setISCS_ATS_MaxByDefault();
		void initEventDetailLogLevel();
		void initRightsCheck();
	protected:
		// ISCS aoto refresh rate from DB:
		static unsigned long s_refreshRateDB;   // "ms"

		//ISCS refresh rate from Agent, by "ms"
		static unsigned long s_refreshRateAgent;

		static bool s_rightsCheck; //RPARAM_NO_VISIBILITY_RULE

		// Configurable maximum ISCS event count limit
		static unsigned long s_maxISCSEvents;

		// Configurable maximum ATS event count limit
		static unsigned long s_maxATSEvents;

        static bool s_isISCSSortingPriority;

		static int s_eventDetailDebugLevelInterval;
	};

};

#endif // !defined(EA_99CD63B4_A375_48ed_98BE_2FEA99C5F2F5__INCLUDED_)
