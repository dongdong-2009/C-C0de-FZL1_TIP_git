#ifndef SOE_PROCESSOR_INCLUDED
#define SOE_PROCESSOR_INCLUDED



#include "SOEEventQueue.h"

using TA_Base_Core::SOEEvent;
using TA_Base_Bus::SOEEventQueue;

namespace TA_Base_Bus
{
	
	
	class SOEProcessor
	{
	private:
		SOEProcessor();
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		static SOEProcessor* m_self;

		SOEEventQueue* m_queue;


	public:
		virtual ~SOEProcessor(){}

		// get singleton method
		static SOEProcessor& getInstance();
		
		// submiit SOEEvent
		void submitSOE( SOEEvent& evt );


	};



} // namespace TA_Base_Bus


#endif // #ifndef SOE_PROCESSOR_INCLUDED
