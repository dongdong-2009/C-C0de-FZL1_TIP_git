/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	DataPointAccessCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */


#include "time.h"

#include "core/naming/src/Naming.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"


//TD15618, use timer to trigger if the observers are timeout
namespace TA_Base_Bus
{

	DataNodeAccessCorbaDef_Impl::DataNodeAccessCorbaDef_Impl ( const std::string & agentName, TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent )
	:
	m_servantActivated(false),
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::NotApplicable ),
	m_specificAgent ( 0 )
	{
		// save the pointer to the data point agent for later usage
		m_specificAgent = specificDataNodeAgent;
	}


	DataNodeAccessCorbaDef_Impl::~DataNodeAccessCorbaDef_Impl()
	{
		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificAgent = 0;
		// CL-17309 ++
		if ( m_servantActivated )
		{
			deactivateServant();
		}
		// ++ CL-17309
		//deactivateAndDeleteServant();
	}

	void DataNodeAccessCorbaDef_Impl::setToControl()
	{
		m_operationMode = TA_Base_Core::Control;
		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			// activate the ServantBase of the class DataNodeAccessCorbaDef_Impl
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_ACCESS_INTERFACE );
		}
	}


	void DataNodeAccessCorbaDef_Impl::setToMonitor()
	{
		m_operationMode = TA_Base_Core::Monitor;
		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			// activate the ServantBase of the class DataNodeAccessCorbaDef_Impl
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_ACCESS_INTERFACE );
		}
	}

	void DataNodeAccessCorbaDef_Impl::setToStandby()
	{
		m_operationMode = TA_Base_Core::Standby;
		if ( m_servantActivated )
		{
			m_servantActivated = false;
			deactivateServant();
		}
	}

	void DataNodeAccessCorbaDef_Impl::changeDataNodeTag(CORBA::ULong dnKey, const char* sessionId, 
		DataNodeCorbaTypes::ETagState tagState, ScadaCorbaTypes::EOutputInhibitState outputInhibitState, 
		ScadaCorbaTypes::EInputInhibitState inputInhibitState, const char* ptwId, const char* comment)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::changeDataNodeTag");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);

			if(0 != dataNode)
			{
				TA_Base_Bus::ETagState newTagState;
				convertETagState(tagState, newTagState);
				TA_Base_Bus::EInputInhibitState newInputInhibitState;
				convertInputInhibitState(inputInhibitState, newInputInhibitState);
				TA_Base_Bus::EOutputInhibitState newOutputInhibitState;
				convertOutputInhibitState(outputInhibitState, newOutputInhibitState);
				dataNode->changeDataNodeTag(sessionId, newTagState, 
					newOutputInhibitState, newInputInhibitState, ptwId, comment);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataNodeAccessCorbaDef_Impl::setNotes(CORBA::ULong dnKey, const char* sessionId, const char* notes)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::setNotes");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);
			
			if(0 != dataNode)
			{
				dataNode->setNotes(sessionId, notes);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataNodeAccessCorbaDef_Impl::setInhibitMMS(CORBA::ULong dnKey, const char* sessionId, CORBA::Boolean inhibitMms)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::setInhibitMMS");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);
			
			if(0 != dataNode)
			{
				dataNode->setInhibitMMS(sessionId, inhibitMms);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		
		FUNCTION_EXIT;
	}

	//wenching++ (TD13621)
    //Get flexible timeout value for the data node named object
    unsigned short DataNodeAccessCorbaDef_Impl::getFlexibleTimeOutValue()
    {
		if(m_operationMode == TA_Base_Core::Control)
		{
			unsigned short flexibleTimeout = 300;
			return flexibleTimeout;
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

    }
    //++wenching (TD13621)

	void DataNodeAccessCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if ( m_operationMode != mode )
		{
			m_operationMode = mode;

			switch (m_operationMode)
			{
			case TA_Base_Core::Monitor:
				setToMonitor();
				break;
			case TA_Base_Core::Control:
				setToControl();
				break;
			case TA_Base_Core::Standby:
				setToStandby();
				break;
			default:
				break;
			}
		}
	}
	
}
