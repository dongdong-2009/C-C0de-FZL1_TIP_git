/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	DataNodeAccessCorbaDef_Impl is an abstract base class that implements
  * the DataNodeAgentCorbaDef interface, providing DataNode configurations
  * and updates to remote clients.
  *
  */

#ifndef DATANODE_ACCESS_CORBA_IMPL_H
#define DATANODE_ACCESS_CORBA_IMPL_H

#include <map>
#include <sys/timeb.h>

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAccessCorbaDef.h"

namespace TA_Base_Bus
{
	class DataNode;
	class IDataNodeAgent;

	//TD15618, use timer to trigger if the observers are timeout
	class DataNodeAccessCorbaDef_Impl : public virtual TA_Base_Core::ServantBase,
										public virtual POA_TA_Base_Bus::IDataNodeAccessCorbaDef
	{
	public:


		DataNodeAccessCorbaDef_Impl ( const std::string & agentName,
									  TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent );


		virtual ~DataNodeAccessCorbaDef_Impl();


		//
		// Operations required by TA_Base_Bus::IDataNodeAccessCorbaDef interface
		//

		virtual void changeDataNodeTag(CORBA::ULong dnKey, const char* sessionId, 
			DataNodeCorbaTypes::ETagState tagState, ScadaCorbaTypes::EOutputInhibitState outputInhibitState, 
			ScadaCorbaTypes::EInputInhibitState inputInhibitState, const char* ptwId, const char* comment);
		virtual void setNotes(CORBA::ULong dnKey, const char* sessionId, const char* notes);
		virtual void setInhibitMMS(CORBA::ULong dnKey, const char* sessionId, CORBA::Boolean inhibitMms);
		virtual CORBA::UShort getFlexibleTimeOutValue();


		/**
		 * setToControl
		 *
		 * This method let the DataNodeAccessCorbaDef_Impl instance knows that it
		 * must activate its servant base and start receiving incoming requests
		 *
		 */
		virtual void setToControl();


		/**
		 * setToMonitor
		 *
		 * This method let the DataNodeAccessCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToMonitor();

		/**
		 * setToStandby
		 *
		 * This method let the DataNodeAccessCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToStandby();

		/**
		 * setOperationMode
		 *
		 * This method let the DataNodeAccessCorbaDef_Impl instance into new mode
		 *
		 */
		void setOperationMode(TA_Base_Core::EOperationMode mode);

	private:

		bool		m_servantActivated;
		std::string m_agentName;
		TA_Base_Core::EOperationMode m_operationMode;
		TA_Base_Bus::IDataNodeAgent * m_specificAgent;
		
        DataNodeAccessCorbaDef_Impl() {};
		DataNodeAccessCorbaDef_Impl ( const DataNodeAccessCorbaDef_Impl & theInstance ) {};
		DataNodeAccessCorbaDef_Impl & operator= ( const DataNodeAccessCorbaDef_Impl & rhs ) {};
	};
}

#endif //DATANODE_ACCESS_CORBA_IMPL_H

