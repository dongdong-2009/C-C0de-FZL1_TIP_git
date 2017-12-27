/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	Description
  *
  */

#ifndef DATANODE_DISPLAY_CLIENT_H
#define DATANODE_DISPLAY_CLIENT_H

#include <map>


#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"


namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataNode;
	class IDataNodeAgent;

	class DataNodeDisplayClient
	{
	public:


		DataNodeDisplayClient ( unsigned long clientID );
		virtual ~DataNodeDisplayClient();

		//
		// operations specific to DataNodeDisplayClient
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
		  * those DataNodes assigned to the observer
          *
          */
		unsigned long getIdleTime();

	    /**
	      * getCurrrentDisplayUpdates
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataNodeCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* getCurrrentDisplayUpdates();

	    /**
	      * saveDataNodeDisplayUpdateDetails
	      * 
	      * save the current datanode display update details for this client
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::DataNodeCorbaDef::DataNodeDisplayState* displayUpdate
	      * 
	      * @exception <exceptions> Optional
	      */
        void saveDataNodeDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> displayUpdate);

	private:

		unsigned long m_clientID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;

        std::vector <TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *> m_listOfUpdateDetails;
        TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		DataNodeDisplayClient() {};
		DataNodeDisplayClient ( const DataNodeDisplayClient & theInstance ) {};
		DataNodeDisplayClient & operator= ( const DataNodeDisplayClient & rhs ) {};
	};
}

#endif

