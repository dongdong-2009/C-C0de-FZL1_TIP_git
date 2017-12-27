#include "SOEEventQueue.h"
#include <core/data_access_interface/soe/src/SOEAccessFactory.h>

namespace TA_Base_Bus
{


SOEEventQueue::SOEEventQueue()
{
}

SOEEventQueue::~SOEEventQueue(void)
{
	
}

void SOEEventQueue::processEvent( boost::shared_ptr<TA_Base_Core::SOEEvent> newEvent )
{
	TA_Base_Core::SOEAccessFactory::getInstance().saveSOE( *newEvent );
}

}
