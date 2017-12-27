/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/CommonContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2015/02/09 18:11:39 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	Base class for BooleanContext and RealNumberContext...
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "bus/scada/datapoint_library/src/InputAssociation.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"
#include "bus/scada/datapoint_library/src/CommonContext.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	CommonContext::CommonContext()
	    :
	    m_userOfExpression ( 0 ),
	    m_toCallback ( false ),
        m_eventDetailsFactory (NULL)
	{
		// initialise the list of input dependencies
		m_listOfInputDependencies.clear();
	}


	CommonContext::CommonContext ( IExpressionNodeNotification * userOfExpression )
	    :
	    m_userOfExpression ( 0 ),
        m_toCallback ( false ),
        m_eventDetailsFactory (NULL)
	{
		// initialise the list of input dependencies
		m_listOfInputDependencies.clear();

		// save pointer pointing to the user for callback later
		m_userOfExpression = userOfExpression;
	}


	CommonContext::~CommonContext()
	{
		// don't delete the object since it is not created/owned by us
		m_userOfExpression = 0;

        // clean up the list of input dependencies
		std::vector < InputAssociation * >::iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  itr++ )
		{
			delete (*itr);
			(*itr) = NULL;
		}
		m_listOfInputDependencies.clear();
        
        if (m_eventDetailsFactory)
        {
            EventDetailsFactory::removeInstance( m_eventDetailsFactory );
            m_eventDetailsFactory = NULL;
        }
	}

	InputAssociation * CommonContext::findInputDependency ( const std::string & inputName )
	{
		InputAssociation* ipAssocation = 0;
		bool foundExistingInput = false;

        TA_Base_Core::ThreadGuard guard( m_lock );
       
		std::vector < InputAssociation * >::iterator itr;
		for ( itr = m_listOfInputDependencies.begin(); itr != m_listOfInputDependencies.end(); itr++ )
		{
			if ( inputName == (*itr)->getInputDataPointName() )
			{
				ipAssocation = (*itr);
				foundExistingInput = true;
				break;
			}
		}

		if ( false == foundExistingInput )
		{
			ipAssocation = createInputDependency ( inputName );

			if ( 0 != ipAssocation )
			{
				ipAssocation->setOperationMode ( true );
				m_listOfInputDependencies.push_back ( ipAssocation );
			}
		}

		return ipAssocation;
	}


	InputAssociation * CommonContext::createInputDependency ( const std::string & inputName )
	{
		InputAssociation * ipAssocation = 0;

        unsigned long   inputDataPointKey = 0;
        std::string     inputDataPointName;
        std::string     inputDataPointAgentName;
        unsigned long   inputDataPointSubsystemKey = 0;
        unsigned long   inputDataPointLocationKey = 0;

		try
		{
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(inputName);
            if (entityData.get() != NULL)
            {
                inputDataPointKey = entityData->getKey();
                inputDataPointName = entityData->getName();
                inputDataPointAgentName = entityData->getAgentName();
                inputDataPointSubsystemKey = entityData->getSubsystem();
                inputDataPointLocationKey = entityData->getLocation();
            }
            else
            {
			    // get configuration data from database
			    std::auto_ptr<IEntityData> inputEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity ( inputName ));
                inputDataPointKey = inputEntityData->getKey();
                inputDataPointName = inputEntityData->getName();
                inputDataPointAgentName = inputEntityData->getAgentName();
                inputDataPointSubsystemKey = inputEntityData->getSubsystem();
                inputDataPointLocationKey = inputEntityData->getLocation();
            }
		}
		catch ( ... )
		{
            std::string exceptionMsg = "Failed to get datapoint: " + inputName + " from database";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", exceptionMsg);
		}

		if ( 0 != inputDataPointKey )
		{
			TA_Base_Core::ConfiguredInputAssociation * configuredInputAssociation = 0;
			configuredInputAssociation = new TA_Base_Core::ConfiguredInputAssociation ( inputDataPointKey,
																				   inputDataPointName,
																				   inputDataPointAgentName,
																				   inputDataPointSubsystemKey,
																				   inputDataPointLocationKey );
			ipAssocation = new InputAssociation ( configuredInputAssociation );

			if ( 0 != ipAssocation )
			{
				ipAssocation->registerForStateChangeNotification ( this );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "InputAssociation created for %s", inputName.c_str());
            }
		}
		else
		{
			std::ostringstream ss;
            ss << "Invalid input entity data (" << inputName << "), no input dependency to be created.";
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, ss.str().c_str());

			m_invalidDependencyErrorString = inputName + " is an invalid input dependency";
		}

		return ipAssocation;
	}


	EQualityStatus CommonContext::getCombinedStatus() const
	{
		EQualityStatus combinedStatus = QUALITY_BAD_NOT_CONNECTED;
		EQualityStatus worstStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
		bool forcedInputDataPoint = false;

        // for each of the input data point
		std::vector < InputAssociation * >::const_iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  itr++ )
		{
			// if current status of input data point is less than the
			// worst status, reassign the worst status
			if ( (*itr)->getCurrentState().getStatus() < worstStatus )
			{
				worstStatus = (*itr)->getCurrentState().getStatus();

                //TD16581
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "InputAssociation for %s is in bad quality %d", (*itr)->getInputDataPointName().c_str(), worstStatus);
            }

			// if current status of the input data point is forced
			if ( QUALITY_GOOD_LOCAL_OVERRIDE == (*itr)->getCurrentState().getStatus() )
			{
				// set the flag to indicate so
				forcedInputDataPoint = true;
			}
		}

		// assign the current status of the derived data point to the
		// worst status
		combinedStatus = worstStatus;

		// if all input data points have good status
		if ( QUALITY_GOOD_NO_SPECIFIC_REASON == worstStatus )
		{
			// and if one of the input data point is forced
			if ( true == forcedInputDataPoint )
			{
				// assign the current status of the derived data point to this
				combinedStatus = QUALITY_GOOD_LOCAL_OVERRIDE;
			}
		}

		return combinedStatus;
	}


	std::string CommonContext::getExpressionResultString()
	{
		if (false == m_invalidDependencyErrorString.empty())
		{
			return m_invalidDependencyErrorString;
		}

		std::ostringstream result;

		// for each of the input data point
		std::vector < InputAssociation * >::const_iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  ++itr )
		{
			if ( m_listOfInputDependencies.begin() != itr )
			{
				result << ", ";
			}

            // get the handle to an instance of the EventDetailsFactory if not already done so
            if ( NULL == m_eventDetailsFactory )
            {
                m_eventDetailsFactory = EventDetailsFactory::getInstance();
            }
            
            const TA_Base_Core::DataPointEventDetails& eventDetails = 
                m_eventDetailsFactory->getEventDetails((*itr)->getInputDataPointKey(), "");

            // TD16211, add asset_name before datapoint description
            result << eventDetails.assetName << " ";
            
            result << (*itr)->getDescription() << " ÊÇ " << (*itr)->getValueAsString();

            if ((*itr)->getCurrentState().getStatus() < QUALITY_GOOD_NO_SPECIFIC_REASON)
            {
                result << "(BAD QUALITY)";
            }
		}

		return result.str();
	}


	bool CommonContext::isGoodCombinedStatus() const
	{
		bool result = false;
		EQualityStatus combinedStatus = getCombinedStatus();

		// if the combined status is either one of these
		if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == combinedStatus ) ||
			 ( QUALITY_GOOD_LOCAL_OVERRIDE == combinedStatus ) )
		{
			// then set the flag
			result = true;
		}

		return result;
	}


	void CommonContext::processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType )
	{
		// lets assume that the update event is destined for this context,
		// then notify the user of the expression that the value of one of
		// the variable in the expression has changed, without having to check for
		// validity of the specified entity key
		if ( 0 != m_userOfExpression)
		{
			// only proceed if callback is enabled
			if ( true == m_toCallback )
			{
				m_userOfExpression->processExpressionChanges();
			}
		}
		// do nothing if there is no user for callback operation
	}



	void CommonContext::enableCallback ( bool enabled )
	{
		m_toCallback = enabled;
	}

	bool CommonContext::getBooleanValue(const std::string& name)
	{
		InputAssociation * ipAssocation = findInputDependency(name);

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		if ( ipAssocation->getInputDataPointDataType() != DPT_BOOL_TYPE)
		{
			std::string msg = name + " can't be converted to boolean value";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		bool booleanValue = false;
		EQualityStatus qualityStatus = ipAssocation->getCurrentState().getStatus();

		if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == qualityStatus ) ||
			 ( QUALITY_GOOD_LOCAL_OVERRIDE == qualityStatus ) )
		{
			booleanValue = ipAssocation->getCurrentState().getValue().getBoolean();
		}

		return booleanValue;
	}

	double CommonContext::getRealNumberValue(const std::string& name)
	{
		InputAssociation* ipAssocation =  findInputDependency(name);

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		EDataPointDataType dataType = ipAssocation->getInputDataPointDataType();
		if (DPT_NULL_TYPE == dataType || DPT_TEXT_TYPE == dataType)
		{
			std::string msg = name + " can't convert to double value";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		double doubleValue = -99999999;
		EQualityStatus qualityStatus = ipAssocation->getCurrentState().getStatus();
		if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == qualityStatus ) ||
			 ( QUALITY_GOOD_LOCAL_OVERRIDE == qualityStatus ) )
		{
			switch (dataType)
			{
			/*	
			case DPT_BOOL_TYPE:
				{
					doubleValue = (double) ipAssocation->getCurrentState().getValue().getBoolean();
					break;
				}
			*/
			case DPT_ENUM_TYPE:
				{
					doubleValue = (double) ipAssocation->getCurrentState().getValue().getEnum();
					break;
				}

			default:
				{
					doubleValue = ipAssocation->getCurrentState().getValue().getFloat();
					break;
				}
			}

			return doubleValue;
		}

		return doubleValue;
	}

	std::string CommonContext::getStringValue(const std::string& name)
	{
		InputAssociation* ipAssocation =  findInputDependency(name);

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		if (ipAssocation->getInputDataPointDataType() != DPT_TEXT_TYPE)
		{
			std::string msg = name + " not string value";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		std::string retValue("");
		EQualityStatus qualityStatus = ipAssocation->getCurrentState().getStatus();
		if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == qualityStatus ) ||
			( QUALITY_GOOD_LOCAL_OVERRIDE == qualityStatus ) )
		{
			retValue = ipAssocation->getCurrentState().getValue().getText();
		}
		return retValue;
	}

	bool CommonContext::isBooleanVariable(const std::string& name)
	{
		InputAssociation * ipAssocation = findInputDependency ( name );

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		return ( DPT_BOOL_TYPE == ipAssocation->getInputDataPointDataType() );
	}

	bool CommonContext::isRealNumberVariable(const std::string& name)
	{
		InputAssociation * ipAssocation = findInputDependency(name);

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	
		EDataPointDataType dataType = ipAssocation->getInputDataPointDataType();
		bool isReal = false;

		if (/*DPT_BOOL_TYPE == dataType || */DPT_INT16_TYPE == dataType || 
			DPT_INT32_TYPE == dataType || DPT_INT16S_TYPE == dataType ||
			DPT_INT32S_TYPE == dataType || DPT_UINT16_TYPE == dataType ||
			DPT_UINT32_TYPE == dataType || DPT_IEEE32_TYPE == dataType ||
			DPT_ENUM_TYPE == dataType )
		{
			isReal = true;
		}
		return isReal;
	}

	bool CommonContext::isStringVariable(const std::string& name)
	{
		InputAssociation * ipAssocation = findInputDependency(name);

		if (0 == ipAssocation)
		{
			std::string msg = name + " is not exist";
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		return ( DPT_TEXT_TYPE == ipAssocation->getInputDataPointDataType() );
	}
}


