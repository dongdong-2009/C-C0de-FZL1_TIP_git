/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *	DataPointAccessCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */

#ifndef DATAPOINT_ACCESS_CORBA_IMPL_H
#define DATAPOINT_ACCESS_CORBA_IMPL_H

#include <map>
#include <sys/timeb.h>

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class IDataPointAgent;

	//TD15618, use timer to trigger if the observers are timeout
	class DataPointAccessCorbaDef_Impl : public virtual TA_Base_Core::ServantBase,
										 public virtual POA_TA_Base_Bus::IDataPointAccessCorbaDef
	{
	public:


		DataPointAccessCorbaDef_Impl ( const std::string & agentName,
									  TA_Base_Bus::IDataPointAgent * specificDataPointAgent );


		virtual ~DataPointAccessCorbaDef_Impl();


		//
		// Operations required by TA_Base_Bus::IDataPointAccessCorbaDef interface
		//

        virtual void registerMonitorAgentForStateUpdate(const char* uniqueId, IDataPointStateUpdateCorbaDef_ptr dpStateUpdateServantRef);
        virtual void setAlarmProperty(CORBA::ULong dpKey, const char* sessionId, const DataPointCorbaTypes::SAlarmProperty& newAlarmProperty);
		virtual ScadaCorbaTypes::UTag* getFieldValue(CORBA::ULong dpKey);
		virtual ScadaCorbaTypes::UTag* getRawValue(CORBA::ULong dpKey);
		virtual void writeDataPointValue(CORBA::ULong dpKey, const char* sessionId, const ScadaCorbaTypes::UTag& value, CORBA::ULong triggeringEntity, const char* triggeringEntityValue);
		virtual void writeDataPointValueWithoutLoggingEvent(CORBA::ULong dpKey, const char* sessionId, const ScadaCorbaTypes::UTag& value, CORBA::ULong triggeringEntity, const char* triggeringEntityValue);
		virtual void setOverride(CORBA::ULong dpKey, const char* sessionId, const ScadaCorbaTypes::UTag& value);
		virtual void removeOverride(CORBA::ULong dpKey, const char* sessionId);
		virtual void setMeterCorrectionValue(CORBA::ULong dpKey, const char* sessionId, CORBA::Double correctionValue);
		virtual void setInputInhibitState(CORBA::ULong dpKey, const char* sessionId, ScadaCorbaTypes::EInputInhibitState inhibit);
		virtual void setOutputInhibitState(CORBA::ULong dpKey, const char* sessionId, ScadaCorbaTypes::EOutputInhibitState inhibit);
		virtual ScadaCorbaTypes::SLastOperatorInfo* getLastOperatorInfo(CORBA::ULong dpKey);
		virtual CORBA::Boolean getIsMMSAlarmInhibited(CORBA::ULong dpKey) ;
		virtual ScadaCorbaTypes::EInputInhibitState getInputInhibitState(CORBA::ULong dpKey) ;
		virtual ScadaCorbaTypes::EOutputInhibitState getOutputInhibitState(CORBA::ULong dpKey) ;
		virtual CORBA::Boolean isWriteable(CORBA::ULong dpKey) ;

		/**
		 * setToControl
		 *
		 * This method let the DataPointAccessCorbaDef_Impl instance knows that it
		 * must activate its servant base and start receiving incoming requests
		 *
		 */
		virtual void setToControl();


		/**
		 * setToMonitor
		 *
		 * This method let the DataPointAccessCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToMonitor();

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);

	private:

		std::string m_agentName;
		TA_Base_Core::EOperationMode m_operationMode;
		TA_Base_Bus::IDataPointAgent * m_specificAgent;
		
        DataPointAccessCorbaDef_Impl() {};
		DataPointAccessCorbaDef_Impl ( const DataPointAccessCorbaDef_Impl & theInstance ) {};
		DataPointAccessCorbaDef_Impl & operator= ( const DataPointAccessCorbaDef_Impl & rhs ) {};
	};
}

#endif //DATAPOINT_ACCESS_CORBA_IMPL_H

