/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/SummaryProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  *	The SummaryProcessor thread is responsible 
  * for generating events and submitting
  * them.
  *
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "bus/scada/datanode_library/src/SummaryProcessor.h"

namespace TA_Base_Bus
{
    SummaryProcessor*                        SummaryProcessor::m_singleton = NULL;
    TA_Base_Core::NonReEntrantThreadLockable SummaryProcessor::m_singletonLock;


    SummaryProcessor* SummaryProcessor::getInstance()
	{
		if ( m_singleton == NULL )
		{
			TA_THREADGUARD( m_singletonLock );
			if ( m_singleton == NULL )
			{
				m_singleton = new SummaryProcessor();
			}
		}
		return m_singleton;
	}	

    void SummaryProcessor::removeInstance( SummaryProcessor*& removeMe )
	{
		TA_THREADGUARD( m_singletonLock );
		if ( removeMe == m_singleton && m_singleton != NULL)
		{
			delete m_singleton;
			m_singleton = NULL;
			removeMe = NULL;
		}
	}


	SummaryProcessor::SummaryProcessor()
	:
	m_queueHandler(NULL)
	{  
		m_queueHandler = new SummaryQueueHandler();
	}

	SummaryProcessor::~SummaryProcessor()
	{
		if (m_queueHandler != NULL )
		{
			delete m_queueHandler;
			m_queueHandler = NULL;
		}
	}
    
	void SummaryProcessor::setEntityMap(const std::map<unsigned long, IEntity*>* entityMap)
	{
        TA_ASSERT( NULL != entityMap, "entityMap cannot be NULL" );

        m_queueHandler->setEntityMap(entityMap);
    }

    void SummaryProcessor::queueItemUnique(SummaryRequest item, bool forceHandle)
    {
        m_queueHandler->queueItemUnique(item, forceHandle);    
    }

	void SummaryProcessor::setOperatorMode(TA_Base_Core::EOperationMode operationMode)
	{
		m_queueHandler->setOperationMode(operationMode);
	}

	void SummaryProcessor::stopProcessor()
	{
		m_queueHandler->stopProcessing();
	}

	void SummaryProcessor::startHandler()
	{
		m_queueHandler->startProcessing();
	}

}
