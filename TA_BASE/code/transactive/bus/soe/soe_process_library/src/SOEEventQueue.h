#ifndef SOE_EVENT_QUEUE_INCLUDED
#define SOE_EVENT_QUEUE_INCLUDED

#include <core/threads/src/QueueProcessor.h>
#include <core/data_access_interface/soe/src/SOEEvent.h>

namespace TA_Base_Bus
{
	class SOEProcessor;

	class SOEEventQueue : public TA_Base_Core::QueueProcessor< TA_Base_Core::SOEEvent >
	{
	public:
		SOEEventQueue();
		virtual ~SOEEventQueue(void);

		virtual void processEvent( boost::shared_ptr<TA_Base_Core::SOEEvent> newEvent );

	};



} // namespace TA_Base_Bus

#endif // SOE_EVENT_QUEUE_INCLUDED