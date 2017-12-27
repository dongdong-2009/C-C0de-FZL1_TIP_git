/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointStateUpdate.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	DataPointStateUpdate represents a request to the PLCEventProcesor
  * to generate and submit a new event for a DataPoint's state change 
  *
  *
  */

#ifndef DATAPOINTSTATEUPDATE_H
#define DATAPOINTSTATEUPDATE_H

#include	<string>

#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"


namespace TA_Base_Bus
{

	class DataPointStateUpdate
	{
	public:

		/**
		 * DataPointStateUpdate::DataPointStateUpdate()
		 *
		 * This constructor is for a state change event that is sent but
		 * not logged.
		 *
		 * @param dataPointEntityKey	the pkey for the DataPoint entity 
		 *								related to this new event
		 *
		 * @param newState				a DataPointCorbaDef structure to be 
		 *								attached to the new event, reflecting
		 *								the new state of the DataPoint
		 */
		DataPointStateUpdate( unsigned long subsystemKey,
							  unsigned long locationKey,
							  unsigned long  dataPointEntityKey,
			  				  TA_Base_Bus::DataPointCorbaDef::DataPointState & newState )
		: m_key( dataPointEntityKey ),
		  m_state( newState ),
		  m_subsystemKey ( subsystemKey ),
		  m_locationKey ( locationKey )
		{
			// Nothing else to do
		}

		virtual ~DataPointStateUpdate() { }


		/**
		 * DataPointStateUpdate::getEntityKey()
		 *
		 * @return 	the pkey for the entity related to this event
		 *
		 */
		virtual unsigned long getEntityKey() const { return m_key; }

		
		/**
		 * DataPointStateUpdate::getSubystemKey()
		 *
		 * @return 	the subsystem key for the entity related to this event
		 *
		 */
		virtual unsigned long getSubystemKey() const { return m_subsystemKey; }

		
		/**
		 * DataPointStateUpdate::getLocationKey()
		 *
		 * @return 	the location key for the entity related to this event
		 *
		 */
		virtual unsigned long getLocationKey() const { return m_locationKey; }

		
		/**
		 * DataPointStateUpdate::getDataPointState()
		 *
		 * @return	the new state for the DataPoint, to be attached
		 *			to the Event's state field 
		 *
		 */
		virtual TA_Base_Bus::DataPointCorbaDef::DataPointState &	getDataPointState() 
		{ 
			return m_state; 
		}

	protected:

		unsigned long							m_key;
		unsigned long							m_subsystemKey;
		unsigned long							m_locationKey;
		TA_Base_Bus::DataPointCorbaDef::DataPointState 	m_state;
	};

}

#endif	
