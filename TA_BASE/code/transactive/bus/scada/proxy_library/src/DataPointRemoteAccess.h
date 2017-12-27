/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointRemoteAccess.h $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	DataPoint CORBA interface, used to work with DataPoints
  *
  */

#ifndef DPNAMEDOBJACCESS_H
#define DPNAMEDOBJACCESS_H

#include "core/naming/src/NamedObject.h"
#include "bus/scada/proxy_library/src/IDataPointAccess.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class DataPointRemoteAccess : public IDataPointAccess
   {

	   public:

		   DataPointRemoteAccess(TA_Base_Core::DataPointEntityDataPtr dpEntityData);

		   virtual ~DataPointRemoteAccess();


         /**
		 *
		 * GetFieldValue
		 *
		 * Gets the  field value of the DataPoint
		 *
		 */
		virtual boost::shared_ptr<TA_Base_Bus::DataPointValue>	getFieldValue();


		/**
		 *
		 * WriteDataPointValue
		 *
		 * Change the value associated with the point
		 * (audit message to be logged)
		 *
		 */
        virtual void writeDataPointValue ( const std::string& sessionId, TA_Base_Bus::DataPointValue& value, unsigned long triggeringEntityKey, const char * triggeringEntityValue ); //TD12418

		/**
		 *
		 * WriteDataPointValueWithoutLoggingEvent
		 *
		 * Change the value associated with the point
		 * (audit message NOT to be logged)
		 *
		 */
        virtual void writeDataPointValueWithoutLoggingEvent ( const std::string& sessionId, TA_Base_Bus::DataPointValue& value, unsigned long triggeringEntityKey, const char * triggeringEntityValue );


		/**
		 *
		 * SetIsForcedState
		 *
		 * Set the data point manual state to the specified indication
		 * Throw AccessDeniedException exception if the client has no authorisation to do so.
		 *
		 * @param    sessionId	The session ID of the client issueing the command
		 * @param    indication	True to enable data point's manual override state,
		 *						false otherwise.
		 *
		*/
		virtual void setIsForcedState ( const std::string& sessionId, bool indication );


		/**
		 *
		 * setForcedStateWithValue
		 *
		 * Force the data point to manual state and update data point's value with the
		 * specified manually entered value.  Throw the following exceptions:
		 *
		 *		OperationModeException, if the data point is in monitor mode,
		 *		AccessDeniedException, if the client has no authorisation to do so,
		 *		NotWriteAble, if the specified value cannot be updated
		 *		BadParameterException, if the specified value is out of range.
		 *
		 * @param    sessionId	The session ID of the client issueing the command
		 * @param    value		The manually entered value to be written to the data point
		 *
		*/
		virtual void setForcedStateWithValue ( const std::string& sessionId, const TA_Base_Bus::DataPointValue & value );


		/**
		  * setAlarmProperties
		  *
		  * This method changes the alarm properties for the datapoint.
		  * Throw the following exceptions:
		  *
		  *		OperationModeException, if the data point is in monitor mode,
		  * 	BadParameterException, if the specified value is out of range.
		  *
		  * @param	newAlarmProperty	The new alarm property of the data point
		  * @param	sessionId			The operator's session id
		  *
		  */
		virtual void setAlarmProperty ( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperty );


		/**
		 *
		 * setInputInhibitState
		 *
		 * Change the data point's service state.  Throw AccessDeniedException exception
		 * if the client has no authorisation to do so.
		 *
		 * @param    sessionId	The session ID of the client issueing the command
		 * @param    indication	True to put the data point in service, false
		 *						to take the data point out of service
		 *
		 */
	    virtual void setInputInhibitState ( const std::string&  sessionId, EInputInhibitState inputInhibitState );

		/**
		 *
		 * setOutputInhibitState
		 *
		 * Inhibit or un-inhibit the data point's control.  Throw AccessDeniedException exception
		 * if the client has no authorisation to do so.
		 *
		 * @param    sessionId	The session ID of the client issueing the command
		 * @param    indication	True to inhibit the data point's control, false
		 *						to un-inhibit the control
		 *
		 */
	    virtual void setOutputInhibitState ( const std::string&  sessionId, EOutputInhibitState outputInhibitState );

		virtual void setMeteredCorrectionValue ( const std::string& sessionId, double newValue );

	   private:
           typedef TA_Base_Core::NamedObject<IDataPointAccessCorbaDef, 
                                            IDataPointAccessCorbaDef_ptr, 
                                            IDataPointAccessCorbaDef_var> DpAccessInterface;

           DpAccessInterface* m_dpAccessInterface;

           TA_Base_Core::DataPointEntityDataPtr m_entityData;
    };
};

#endif
