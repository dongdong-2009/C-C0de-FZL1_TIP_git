/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSHistoricalWorker.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSHistoricalWorker is ...
  *
  */

#if !defined(EA_3EC3ACF5_AF89_4521_A270_3A1D4CDF0190__INCLUDED_)
#define EA_3EC3ACF5_AF89_4521_A270_3A1D4CDF0190__INCLUDED_

#include "IEventWorker.h"
#include "IEventWorkObserver.h"
#include "EventHandlerManager.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_App
{
	class ATSHistoricalWorker : public IEventWorker
	{

	public:
		ATSHistoricalWorker(IEventWorkObserver* observer);
		virtual ~ATSHistoricalWorker();
		virtual void run();
	/*	virtual void terminate();*/
		virtual void doCurrentTask();

		//virtual void terminate();
	
	};
};
#endif // !defined(EA_3EC3ACF5_AF89_4521_A270_3A1D4CDF0190__INCLUDED_)
