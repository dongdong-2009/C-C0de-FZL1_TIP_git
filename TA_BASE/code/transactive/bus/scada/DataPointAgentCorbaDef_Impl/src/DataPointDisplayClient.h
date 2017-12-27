/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	Description
  *
  */

#ifndef DATAPOINT_DISPLAY_CLIENT_H
#define DATAPOINT_DISPLAY_CLIENT_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataPoint;
	class IDataPointAgent;

	class DataPointDisplayClient 
	{
	public:


		DataPointDisplayClient ( unsigned long clientID );
		virtual ~DataPointDisplayClient();

		//
		// operations specific to DataPointDisplayClient
		//


		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */	
		unsigned long getClientID() const;

		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those DataPoints assigned to the observer
          *
          */
		unsigned long getIdleTime();

	    /**
	      * getCurrrentDisplayUpdates
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataPointCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* getCurrrentDisplayUpdates();

	    /**
	      * saveDataPointDisplayUpdateDetails
	      * 
	      * save the current datapoint display update details for this client
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::DataPointCorbaDef::DataPointDisplayState* displayUpdate
	      * 
	      * @exception <exceptions> Optional
	      */
        void saveDataPointDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> displayUpdate);

	private:

		unsigned long m_clientID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;

        std::vector <TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *> m_listOfUpdateDetails;
        TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		DataPointDisplayClient() {};
		DataPointDisplayClient ( const DataPointDisplayClient & theInstance ) {};
		DataPointDisplayClient & operator= ( const DataPointDisplayClient & rhs ) {};
	};
}

#endif

