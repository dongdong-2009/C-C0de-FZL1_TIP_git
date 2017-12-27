/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/EventDetailsFactory.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	This class provides a single point to get the Asset Name associated with the specified entity key.
  * It maintains a list of the proxies that provide the asset names and a map between the proxy pkey
  * and the asset name.  This class also handles the callbacks from the proxies when the asset names
  * are changed and updates the map accordingly.
  *
  */

#ifndef EVENTDETAILSFACTORY_H
#define EVENTDETAILSFACTORY_H

#include <string>
#include <vector>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"

namespace TA_Base_Bus
{

	class EventDetailsFactory
	{
	public:

		/**
		*
		* getInstance
		*
		* Get an instance of EventDetailsFactory.  Caller is responsible for destroying
		* the instance by calling removeInstance(), passing in the instance created
		* by this method.
		*
		*/
		static EventDetailsFactory * getInstance();


		/**
		*
		* removeInstance
		*
		* Remove an instance of EventDetailsFactory that has been created by the getInstance()
		*
		* @param	instance	The instance of the EventDetailsFactory to be destroyed
		*
		*/
		static void removeInstance ( EventDetailsFactory * instance );


		/**
		*
		* getEventDetails
		*
		* Get the asset name associated with the specified entity key
		*
		* @param	entityKey	The pkey of the entity providing the asset name
		*
		*/
        const TA_Base_Core::DataPointEventDetails& getEventDetails ( unsigned long entityKey, const std::string& agentName = std::string(""));

        void populateEventDetails (const std::string agentName);


	private:


		static EventDetailsFactory *						m_eventDetailsFactory;
		static unsigned int									m_refCounter;
		static TA_Base_Core::ReEntrantThreadLockable		m_lock;
		TA_Base_Core::ReEntrantThreadLockable		        m_mapLock;
        static std::vector<std::string>                     m_agentNameMap;
		static TA_Base_Core::PkeyEventDetailsMap			m_pkeyEventDetailsMap;
		static TA_Base_Core::DataPointEventDetails			m_defaultEventDetails;

		virtual ~EventDetailsFactory();
		EventDetailsFactory();
		EventDetailsFactory ( const EventDetailsFactory & theInstance );
		EventDetailsFactory & operator= ( const EventDetailsFactory & rhs );
	};

} // close namespace TA_Base_Bus

#endif
