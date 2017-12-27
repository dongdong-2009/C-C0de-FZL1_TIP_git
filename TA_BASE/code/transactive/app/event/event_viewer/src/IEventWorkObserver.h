/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/IEventWorkObserver.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * IEventWorkObserver is ...
  *
  */

#if !defined(EA_67D41B3C_404A_491e_A916_2DEFD1ED3A8E__INCLUDED_)
#define EA_67D41B3C_404A_491e_A916_2DEFD1ED3A8E__INCLUDED_

#include <vector>

#include "ActionCommand.h"

#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/event/src/EventItem.h"


using namespace TA_Base_App;


namespace TA_Base_App{


	class IEventWorkObserver
	{

	public:
		IEventWorkObserver();
		virtual ~IEventWorkObserver();

		virtual void ATSSubscriptionErrNotification() =0;

		virtual void setATSSubscriptionErr(long err)=0;
		virtual long getATSSubscriptionErr()=0;

		// merge the event list of ATS and ISCS together, Notify GUI to update listView, also responsibel for release the memory eventList
		virtual void EventNotification(TA_Base_Core::EventCategory eventCategory, 
			std::vector<TA_Base_Core::ICombinedEventData*> & eventlist, 
			ActionCommand command,long seq_ID=0, bool backLogLoadingJustFinished=false ) = 0;

		virtual void statusReadyNotification(TA_Base_Core::EventCategory eventCategory,long seq_ID=0) =0;

		virtual bool IsThisTaskStopped( long taskID) = 0;
		TA_Base_Core::ReEntrantThreadLockable m_displaylistLock;
		TA_Base_Core::ReEntrantThreadLockable m_storedlistLock;


		virtual int mergeEventFromAgent(TA_Base_Core::T_EventItemList* eventSeq)=0;	
		virtual void mergeEventFromDB( std::vector<TA_Base_Core::ICombinedEventData*> & eventlist) = 0;



		virtual void reload()=0;
	};

};

#endif // !defined(EA_67D41B3C_404A_491e_A916_2DEFD1ED3A8E__INCLUDED_)
