/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.cpp $
  * @author  HoaVu
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/22 14:27:54 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataPointDisplayClient::DataPointDisplayClient ( unsigned long clientID )
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_clientID ( clientID )
	{
		m_listOfUpdateDetails.clear();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataPointDisplayClient::~DataPointDisplayClient()
	{
		std::vector < TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator it;
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


	unsigned long DataPointDisplayClient::getClientID() const
	{
		return m_clientID;
	}

	unsigned long DataPointDisplayClient::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}

    void DataPointDisplayClient::saveDataPointDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> displayUpdate)
    {
		// save the new DataPointDisplayState to the list of current updates
        TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* dpupdate = 0;
        dpupdate = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState();
        *dpupdate = *displayUpdate;

		// stop multi threads accessing the update details list
		TA_Base_Core::ThreadGuard guard( m_updateDetailsLock );
        m_listOfUpdateDetails.push_back ( dpupdate );        
    }

    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* DataPointDisplayClient::getCurrrentDisplayUpdates()
    {

		FUNCTION_ENTRY ("getCurrrentDisplayUpdates" );
		std::vector <TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* > localList;
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

        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator currentUpdateItr;
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
