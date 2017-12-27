/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/PersistRequest.h $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	PersistRequest represents a request to the PersistProcessor
  * to write a persisted datapoint value
  *
  */

#ifndef PERSISTREQUEST_H
#define PERSISTREQUEST_H

#include <string>
#include "bus/scada/common_library/src/EPersistedItem.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

namespace TA_Base_Bus
{

    class PersistRequest
	{
	public:

		/**
		 * PersistRequest::PersistRequest()
		 *
		 * This constructor is for a persist request
		 *
		 * @param item			    
		 * @param entityKey			The pkey of the entity associated with this request
		 * @param value		        The datapoint value to be persisted
		 * @param quality			The quality of datapoint value to be persisted
         * @param persistence       Share ptr to the persistence object
		 *
		 */
        PersistRequest( EPersistedItem item, unsigned long entityKey, const std::string & value, EQualityStatus quality )
		: m_entityKey( entityKey ),
          m_item( item ),
		  m_value( value ),
		  m_quality( quality )
		{
		}


		~PersistRequest()
		{
		}


		/**
		 * PersistRequest::getEntityKey()
		 *
		 * @return 	the pkey for the entity related to this event
		 *
		 */
		unsigned long getEntityKey() const
		{ 
			return m_entityKey;
		}


		/**
		 * PersistRequest::getItem()
		 *
		 * @return	the persisted item type associated with this persisted event
		 *
		 */
		EPersistedItem getItem() const
		{
			return m_item;
		}


		/**
		 * PersistRequest::getValue()
		 *
		 * @return	the value of the datapoint to be persisted
		 *
		 */
		const std::string & getValue() const 
		{ 
			return m_value;
		}


		/**
		 * PersistRequest::getQuality()
		 *
		 * @return	the quality of the datapoint value to be persisted
		 *
		 */
		EQualityStatus getQuality() const 
		{ 
			return m_quality;
		}

	private:
		unsigned long       m_entityKey;
        EPersistedItem      m_item;
		std::string 	    m_value;
		EQualityStatus      m_quality;
	};

}

#endif	
