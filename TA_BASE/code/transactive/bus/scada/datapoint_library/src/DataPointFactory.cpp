/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointFactory.cpp $
  * @author  J. Welton
  * @version $Revision: #4 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *
  * Creates DataPoint objects and registers them with
  * the Naming Service.
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include <ctime>

#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/AnalogueDataPoint.h"
#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"
#include "bus/scada/datapoint_library/src/CombinedOutputDataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedDataPoint.h"
#include "bus/scada/datapoint_library/src/EnumDataPoint.h"
#include "bus/scada/datapoint_library/src/MeteredDataPoint.h"
#include "bus/scada/datapoint_library/src/MultiBitDataPoint.h"
#include "bus/scada/datapoint_library/src/SafetyOutputDataPoint.h"
#include "bus/scada/datapoint_library/src/TextDataPoint.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointStateUpdateServant.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"
#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    DataPointFactory::DataPointFactory(TA_Base_Bus::IDataPointAgent* dpAgent)
	:
    m_eventDetailsFactory (NULL),
    m_scadaRootEntityData ( NULL ),
    m_dpAgent (dpAgent),
    m_dpStateUpdateSender (NULL),
    m_commandProcessor (NULL),
    m_dpStateUpdateServant (NULL),
	m_initialized(false)
	{
        //to start alarmAckStateProcessor thread
        AlarmAckStateProcessor::getInstance();
    }

	DataPointFactory::~DataPointFactory()
	{
        if ( NULL != m_scadaRootEntityData )
        {
            delete m_scadaRootEntityData;
            m_scadaRootEntityData = NULL;
        }

        // AlarmAckStateProcessor has no reference count, so just need to call once here
        AlarmAckStateProcessor::removeInstance();
        
        if (m_dpStateUpdateSender != NULL)
        {
            delete m_dpStateUpdateSender;
			m_dpStateUpdateSender = NULL;
            delete m_commandProcessor;
			m_commandProcessor = NULL;
            delete m_dpStateUpdateServant;
			m_dpStateUpdateServant = NULL;
        }
        m_eventDetailsFactory->removeInstance(m_eventDetailsFactory);
    }


	DataPoint * DataPointFactory::createDataPoint ( TA_Base_Core::DataPointEntityDataPtr cfg )
	{
		if ( !m_initialized )
		{
			unsigned long agentKey = cfg->getAgent();
			std::string agentName = cfg->getAgentName();
			initialize(agentKey, agentName);
		}
		
		DataPoint * newDp = 0;
		{
			// get specific DataPoint Type
			TA_Base_Bus::EDataPointType dpType = stringToDataPointType( cfg->getDataPointType() );

			// DataPoint data type
			TA_Base_Bus::EDataPointDataType dpDataType = stringToDataPointDataType ( cfg->getDataType() );

			//
			// Create DataPoint objects and do type specific configuration
			//

			switch( dpType )
			{
				case TA_Base_Bus::DPT_ANALOGUE :
				case TA_Base_Bus::DPT_CALCULATED:
				{
					AnalogueDataPoint * newADP = new AnalogueDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );
					configureEntityAssociations( cfg, newADP );

					newDp = newADP;
					break;
				}
				case TA_Base_Bus::DPT_METERED :
				{
					MeteredDataPoint * newDDP = new MeteredDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );
					configureEntityAssociations( cfg, newDDP );

					newDp = newDDP;
					break;
				}
				case TA_Base_Bus::DPT_COMBINED_OUTPUT :
				{
					CombinedOutputDataPoint * newDDP = new CombinedOutputDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );

					newDp = newDDP;
					break;
				}
				case TA_Base_Bus::DPT_SAFETY_OUTPUT :
				{
					SafetyOutputDataPoint * newDDP = new SafetyOutputDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );

					newDp = newDDP;
					break;
				}
				case TA_Base_Bus::DPT_DERIVED :
				{
					DerivedDataPoint * newDDP = new DerivedDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );

					newDp = newDDP;
					break;
				}
				case TA_Base_Bus::DPT_ENUM :
				{
					EnumDataPoint * newDDP = new EnumDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );

					newDp = newDDP;
					break;
				}
				case TA_Base_Bus::DPT_BOOLEAN :
				{
					BooleanDataPoint * newBDP = new BooleanDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );
					configureEntityAssociations( cfg, newBDP );

					newDp = newBDP;
					break;
				}
				case TA_Base_Bus::DPT_TEXT :
				{
					TextDataPoint * newTDP = new TextDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );
					configureTextDataPoint( cfg, newTDP );
					configureEntityAssociations( cfg, newTDP );

					newDp = newTDP;
					break;
				}
				case TA_Base_Bus::DPT_MULTIBIT:
				{
					MultiBitDataPoint * newTDP = new MultiBitDataPoint( cfg, dpType, dpDataType, m_persistence, *m_scadaRootEntityData, m_mmsScadaUtility, *m_commandProcessor );
					newDp = newTDP;
					break;
				}
				default :
				{
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown DataPointType (%d) for %s",
                        dpType, cfg->getName().c_str() );
					break;
				}
			}
		}

		return newDp;
	}


	void DataPointFactory::configureAnalogueDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
													   AnalogueDataPoint * point )
	{
	}

	void DataPointFactory::configureBooleanDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
													  BooleanDataPoint * point )
	{
	}


	void DataPointFactory::configureTextDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
												   TextDataPoint * point )
	{
		// text data point has no alarm...
	}


	void DataPointFactory::configureEntityAssociations( TA_Base_Core::DataPointEntityDataPtr cfg,
														DataPoint * point )
	{
		parseEntityAssociations( point, cfg->getEntityAssociation() );
	}

	void DataPointFactory::configureAlarm( DataPoint * point,
										   const TA_Base_Bus::EDataPointAlarms & almType,
										   bool almEnabledField,
										   TA_Base_Bus::DpValue alarmValue,
										   const std::string & almMsgField,
										   int almDelayField,
										   int almSeverityField,
										   const std::string & engUnit,
										   const std::string & almMMSConfiguration,
										   double alarmThresholdConstant )
	{
		point->setAlarmDetails( almType,
								almEnabledField,
								almDelayField,
								almSeverityField,
								almMsgField,
								alarmValue,
								engUnit,
								almMMSConfiguration,
								alarmThresholdConstant );
	}


	TA_Base_Bus::EDataPointType DataPointFactory::stringToDataPointType(
											const std::string & typeStr ) const
	{
		if ( DPT_BOOLEAN_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_BOOLEAN;
		}
		else if ( DPT_TEXT_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_TEXT;
		}
		else if ( DPT_ANALOGUE_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_ANALOGUE;
		}
		else if ( DPT_DERIVED_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_DERIVED;
		}
		else if ( DPT_ENUM_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_ENUM;
		}
		else if ( DPT_COMBINED_OUTPUT_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_COMBINED_OUTPUT;
		}
		else if ( DPT_SAFETY_OUTPUT_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_SAFETY_OUTPUT;
		}
		else if ( DPT_METERED_ANALOGUE_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_METERED;
		}
		else if ( DPT_CALCULATED_TYPE_STRING == typeStr )
		{
			return TA_Base_Bus::DPT_CALCULATED;
		}
		else if(DPT_MULTI_BIT_TYPE_STRING == typeStr)
		{
			return TA_Base_Bus::DPT_MULTIBIT;
		}
		else
		{
			return TA_Base_Bus::DPT_UNKNOWN_TYPE;
		}
	}


	TA_Base_Bus::EDataPointDataType DataPointFactory::stringToDataPointDataType(
											const std::string & dataTypeStr ) const
	{
		if ( dataTypeStr == DPT_CFG_BOOL )
		{
			return TA_Base_Bus::DPT_BOOL_TYPE;
		}
		else if ( ( dataTypeStr == DPT_CFG_INT16 ) || ( "INT" == dataTypeStr ) )
		{
			return TA_Base_Bus::DPT_INT16_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_INT32 )
		{
			return TA_Base_Bus::DPT_INT32_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_INT16S )
		{
			return TA_Base_Bus::DPT_INT16S_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_INT32S )
		{
			return TA_Base_Bus::DPT_INT32S_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_UINT16 )
		{
			return TA_Base_Bus::DPT_UINT16_TYPE;
		}
		else if ( ( dataTypeStr == DPT_CFG_UINT32 ) || ( "UINT" == dataTypeStr ) )
		{
			return TA_Base_Bus::DPT_UINT32_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_IEEE32 )
		{
			return TA_Base_Bus::DPT_IEEE32_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_TEXT )
		{
			return TA_Base_Bus::DPT_TEXT_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_DERIVED )
		{
			return TA_Base_Bus::DPT_ENUM_TYPE;
		}
		else if ( dataTypeStr == DPT_CFG_MULTIBIT )
		{
			return TA_Base_Bus::DPT_ENUM_TYPE;
		}
		else
		{
			return TA_Base_Bus::DPT_NULL_TYPE;
		}
	}


	void DataPointFactory::parseEntityAssociations( DataPoint * point,
												 const std::string & associations )
	{
		// only proceed if the association is not empty
		if ( false == associations.empty() )
		{
			point->clearAssociations();

			//
			// Parse the configuration, the format is:
			//      entityName:preset;entityName:preset;...
			//
			unsigned int currentPos = 0;
			while ( currentPos < associations.length() )
			{
				//
				// Locate the next ':' and ';'
				//
				std::string::size_type colonPos = associations.find( ":", currentPos );
				std::string::size_type semiColonPos = associations.find( ";", currentPos );

				if ( colonPos == std::string::npos ||
					 colonPos > semiColonPos )
				{
					//
					// Couldn't locate ':' or ';' precedes ':'
					//
					return;
				}

				if ( semiColonPos == std::string::npos )
				{
					//
					// Couldn't locate a ';' character. Maybe the last
					// semi colon has been left off,  ie it looks like
					// "entityName:preset"
					//
					if ( colonPos != ( associations.length() -1 ) )
					{
						semiColonPos = associations.length();
					}
					else
					{
						return;
					}
				}


				//
				// We have found an association
				//
				std::string entityName =
						associations.substr( currentPos, colonPos - currentPos );
				std::string preset =
						associations.substr( colonPos+1, semiColonPos-colonPos - 1 );

				point->addAssociation( entityName, preset );
				currentPos = semiColonPos + 1;
			}
		}
	}


    void DataPointFactory::registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr monitorAgentStateUpdateRef)
    {
        if (m_commandProcessor == NULL || m_dpStateUpdateSender == NULL || m_dpStateUpdateServant == NULL)
        {
            //this indicates there is no datapoints configured for this agent, just ingore
            return;
        }

        m_commandProcessor->registerMonitorAgent(uuid, monitorAgentStateUpdateRef);
    }

    
    void DataPointFactory::setToControl()
    {
        if (m_commandProcessor == NULL || m_dpStateUpdateSender == NULL || m_dpStateUpdateServant == NULL)
        {
            //this indicates there is no datapoints configured for this agent, just ingore
            return;
        }
		
        AlarmAckStateProcessor::getInstance().setOperationMode(TA_Base_Core::Control);
		
        m_dpStateUpdateServant->setToControl();
        m_commandProcessor->setToControl();
        m_dpStateUpdateSender->setToControl();
    }

    
    void DataPointFactory::setToMonitor()
    {
        if (m_commandProcessor == NULL || m_dpStateUpdateSender == NULL || m_dpStateUpdateServant == NULL)
        {
            //this indicates there is no datapoints configured for this agent, just ingore
            return;
        }
		
        AlarmAckStateProcessor::getInstance().setOperationMode(TA_Base_Core::Monitor);
		
        m_dpStateUpdateServant->setToMonitor();
        m_commandProcessor->setToMonitor();
        m_dpStateUpdateSender->setToMonitor();
    }

	void DataPointFactory::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if (m_commandProcessor == NULL || m_dpStateUpdateSender == NULL || m_dpStateUpdateServant == NULL)
        {
            //this indicates there is no datapoints configured for this agent, just ingore
            return;
        }
        AlarmAckStateProcessor::getInstance().setOperationMode(mode);
        m_dpStateUpdateServant->setOperationMode(mode);
        m_commandProcessor->setOperationMode(mode);
        m_dpStateUpdateSender->setOperationMode(mode);
	}

	TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr DataPointFactory::getDataPointStateUpdateServant()
    {
		if (NULL != m_dpStateUpdateServant)
		{
			return m_dpStateUpdateServant->getDataPointStateUpdateServant();
		}
		
		return TA_Base_Bus::IDataPointStateUpdateCorbaDef::_nil();
    }

	void DataPointFactory::initialize(unsigned long agentKey, std::string& agentName)
	{
		if ( !m_initialized )
		{
			TA_THREADGUARD( m_lockInit );

			if ( !m_initialized )
			{
				// get the handle to an instance of the EventDetailsFactory if not already done so
				if ( NULL == m_eventDetailsFactory )
				{
					m_eventDetailsFactory = EventDetailsFactory::getInstance();
					// prepopulate all the event details under this agent.
					m_eventDetailsFactory->populateEventDetails(agentName);
				}
				// create ScadaPersistence if not already done so
				if ( m_persistence.get() == NULL )
				{
					m_persistence = ScadaPersistencePtr( ScadaPersistence::createScadaPersistenceForAgent( ScadaPersistence::DATAPOINT, agentKey ) );
					m_persistProcessor.setScadaPersistencePtr( m_persistence );
				}

				// create DataPointStateUpdateSender if not done so
				if (m_dpStateUpdateSender == NULL)
				{
					m_dpStateUpdateSender = new DataPointStateUpdateSender(agentName, *this);
					m_commandProcessor = new CommandProcessor(m_dpStateUpdateSender, m_dpAgent);
					m_dpStateUpdateServant = new DataPointStateUpdateServant(agentName + DP_STATE_UPDATE_INTERFACE, m_commandProcessor);
				}

				// Load Scada Root Entity Data if not already done so
				if ( NULL == m_scadaRootEntityData )
				{
					TA_Base_Core::IEntityDataList entityDataList = EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::ScadaRootEntityData::getStaticType());

					TA_Base_Core::IEntityDataList::const_iterator it = entityDataList.begin();
					if (it == entityDataList.end())
					{
						std::ostringstream msg;
						msg << "Cannot get entity of type " << TA_Base_Core::ScadaRootEntityData::getStaticType();

						TA_THROW(TA_Base_Core::FatalConfigurationException(msg.str()));
					}

					m_scadaRootEntityData = dynamic_cast<ScadaRootEntityData*>(entityDataList[0]);
					TA_ASSERT(NULL != m_scadaRootEntityData, "Failed to cast entityDataList[0] into ScadaRootEntityData*");

					for (++it; it != entityDataList.end(); ++it)
					{
						// there should be only 1 EntityData for this type
						delete (*it);   // delete all others to prevent memory leak.
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Deleting extra entity data of type %s",
							TA_Base_Core::ScadaRootEntityData::getStaticType().c_str());
					}

					entityDataList.clear();
				}

				m_mmsScadaUtility.createMmsGlobal();
				m_mmsScadaUtility.initDNProxyNotLocal(agentKey);

				m_initialized = true;
			}
		}
	}

	unsigned long DataPointFactory::getScadaRootKey()
	{
		if (m_initialized)
		{
			return m_scadaRootEntityData->getKey();
		}
		else
		{
			return ULONG_MAX;
		}
	}

	MmsScadaUtility& DataPointFactory::getMmsScadaUtility()
	{
		return m_mmsScadaUtility;
	}
}
