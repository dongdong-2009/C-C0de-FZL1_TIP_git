#include "SOEProcessor.h"

#include <core/utilities/src/TAAssert.h>



namespace TA_Base_Bus
{
	TA_Base_Core::NonReEntrantThreadLockable SOEProcessor::m_singletonLock;
	SOEProcessor* SOEProcessor::m_self = NULL;
	
	SOEProcessor::SOEProcessor()
		: m_queue( 0 )
	{
		m_queue = new SOEEventQueue();
		m_queue->start();
	}
	
	SOEProcessor& SOEProcessor::getInstance()
	{
		if( m_self ) return *m_self;

		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if(! m_self ) m_self = new SOEProcessor();
		return *m_self;
	}
	
	void SOEProcessor::submitSOE( SOEEvent& evt )
	{
		TA_ASSERT( evt.isNew(), "Only new soe event can be submitted." );
		//SOEAccessFactory::getInstance().saveSOE( evt );
		
		boost::shared_ptr<SOEEvent> pNewEvt( new SOEEvent(evt) );
		m_queue->insert( pNewEvt );
	}
}
