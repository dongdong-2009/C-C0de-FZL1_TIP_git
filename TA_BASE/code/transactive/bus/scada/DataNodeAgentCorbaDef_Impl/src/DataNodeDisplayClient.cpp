/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 14:27:54 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataNodeDisplayClient::DataNodeDisplayClient ( unsigned long clientID )
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_clientID ( clientID )
	{
		m_listOfUpdateDetails.clear();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataNodeDisplayClient::~DataNodeDisplayClient()
	{
		std::vector < TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator it;
		for ( it = m_listOfUpdateDetails.begin(); it != m_listOfUpdateDetails.end(); ++it)
		{
			delete (*it);
			(*it) = 0;
		}
		m_listOfUpdateDetails.clear();
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

	}


	unsigned long DataNodeDisplayClient::getClientID() const
	{
		return m_clientID;
	}

	unsigned long DataNodeDisplayClient::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}

    void DataNodeDisplayClient::saveDataNodeDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> displayUpdate)
    {
		// save the new DataNodeDisplayState to the list of current updates
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState* dpupdate = 0;
        dpupdate = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState();
        *dpupdate = *displayUpdate;

		// stop multi threads accessing the update details list
		TA_Base_Core::ThreadGuard guard( m_updateDetailsLock );
        m_listOfUpdateDetails.push_back ( dpupdate );        
    }

    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* DataNodeDisplayClient::getCurrrentDisplayUpdates()
    {

		FUNCTION_ENTRY ("getCurrrentDisplayUpdates" );
		std::vector <TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState* > localList;
		localList.clear();

		{
			// stop multi threads accessing the update details list
			TA_Base_Core::ThreadGuard guard( m_updateDetailsLock );

			if ( 0 < m_listOfUpdateDetails.size() )
			{
				// clear the update list
                localList.swap( m_listOfUpdateDetails );
			}
			// else do nothing
		}

        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence();
		displaySequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator currentUpdateItr;
		for ( currentUpdateItr = localList.begin();
			  currentUpdateItr != localList.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence
			(*displaySequence)[sequenceIndex++] = **currentUpdateItr;

			// delete the update details from the list
			delete (*currentUpdateItr);
			(*currentUpdateItr) = 0;
		}

		// restart the timer
		m_purgeExistingUpdatesTimer->startTiming();

		localList.clear();

		return displaySequence;    
    }

}
