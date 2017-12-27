/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSHistoricalWorker.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSHistoricalWorker is ...
  *
  */

#if !defined(EA_7A29FAD5_E199_4652_8ACC_F8A82634ABAF__INCLUDED_)
#define EA_7A29FAD5_E199_4652_8ACC_F8A82634ABAF__INCLUDED_

#include "IEventWorker.h"
#include "IEventWorkObserver.h"

namespace TA_Base_App{

	class ISCSHistoricalWorker : public IEventWorker
	{

	public:
		ISCSHistoricalWorker(IEventWorkObserver* observer);
		virtual ~ISCSHistoricalWorker();
/*		virtual void run();*/
		//virtual void terminate();
		virtual void doCurrentTask();
		void load(FilterCriteria filterCriteria, long taskID);
// 		int submitLoadedEventAndGetNextEvents(long taskID, 
// 			std::vector<TA_Base_Core::ICombinedEventData*> events, bool loadingFinished);

		//testing purpose
		void setNeedToTerminated(bool val){m_needToTerminate = val;};

	};

};

#endif // !defined(EA_7A29FAD5_E199_4652_8ACC_F8A82634ABAF__INCLUDED_)
