#pragma once
#include "ATSEventWrapper.h"
#include "ActionCommand.h"
#include "FilterCriteria.h"
#include <vector>
#include <list>
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "IEventWorker.h"
/*#include "app/event/event_viewer/EventListComponent/src/IATSEventsAPI.h"*/
#include "../../../../../cots/IATSAlarmEvent/include/IATSEventsAPI.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"

using namespace TA_Base_Core;



class ATSRealTimeThread: public TA_Base_Core::Thread
{
	
	unsigned long seq;
public:

	ATSRealTimeThread();
	~ATSRealTimeThread();
	virtual void run();

	virtual void terminate();

	bool m_terminated;

	bool m_paused;
	


};

class ATSSimulator : public TA_Base_App::ATSEventWrapper
{
public:
	ATSSimulator(void);

	static ActionCommand  doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);


	static ActionCommand doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);
	static ActionCommand doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength);


	static void ATSSimulator::generateTimeStr ( 
		const TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter, 
		unsigned long pkeyBoundary ,
		bool onNext,
		std::stringstream & timeString);

	static IATSEventsAPI::ATSQueryError DoStartTimeEndTimeQuery_simulator(
		const TA_Base_Core::CombinedEventAccessFactory::EventFilter* const filterCriteria,
		const ISCSDateTime& startTime,
		const ISCSDateTime& endTime,
		std::list<ATSEvent*>& atsEvents);

	static ATSEvent* ATSSimulator::createATSEvent( unsigned long row, IData* data );

	static ActionCommand ATSSimulator::stopATSSubscription(void);

	static ATSRealTimeThread* ATSSimulator::getATSSubscriberThread();

	static ActionCommand ATSSimulator::startATSSubscription(
		FilterCriteria filterCriteria,
		const unsigned int backloglength,				
		std::vector<TA_Base_Core::ICombinedEventData*>& events);

	~ATSSimulator(void);

	static ATSRealTimeThread* m_atsRealTimeThread;
};


