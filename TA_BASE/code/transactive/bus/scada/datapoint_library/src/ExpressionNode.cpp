/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/ExpressionNode.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#include "time.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"
#include "app/scada/datapoint_library/src/InputAssociation.h"
#include "app/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "app/scada/datapoint_library/src/DataPointValue.h"
#include "app/scada/datapoint_library/src/DataPointState.h"
#include "app/scada/datapoint_library/src/ExpressionNode.h"


ExpressionNode::ExpressionNode ( IExpressionNodeNotification * user )
:
m_valueString ( "" ),
m_expressionOperator ( 0 ),
m_currentState ( 0 ),
m_left ( 0 ),
m_right ( 0 ),
m_inputAssociation ( 0 ),
m_user ( 0 )
{
	// initialise the state.
	DataPointValue dpv ( 0 );
	m_currentState = new DataPointState ( dpv, time ( NULL ), QUALITY_BAD_NOT_CONNECTED );

	m_user = user;
}


ExpressionNode::~ExpressionNode()
{
	// do not delete the reference to the client of interface IExpressionNodeNotification
	m_user = 0;

	if ( 0 != m_currentState )
	{
		delete m_currentState;
		m_currentState = 0;
	}

	if ( 0 != m_left )
	{
		delete m_left;
		m_left = 0;
	}

	if ( 0 != m_right )
	{
		delete m_right;
		m_right = 0;
	}

	if ( 0 != m_inputAssociation )
	{
		delete m_inputAssociation ;
		m_inputAssociation = 0;
	}
}


const DataPointState & ExpressionNode::getCurrentState() const
{
	return *m_currentState;
}


unsigned char ExpressionNode::getOperator() const
{
	return m_expressionOperator;
}


void ExpressionNode::setOperator ( unsigned char expressionOperator )
{
	m_expressionOperator = expressionOperator;
}


void ExpressionNode::setLeftExpressionNode ( ExpressionNode * left )
{
	if ( 0 != m_left )
	{
		delete m_left;
		m_left = 0;
	}

	m_left = left;
}


void ExpressionNode::setRightExpressionNode ( ExpressionNode * right )
{
	if ( 0 != m_right )
	{
		delete m_right;
		m_right = 0;
	}

	m_right = right;
}


ExpressionNode * ExpressionNode::getLeftExpression() const
{
	return m_left;
}


ExpressionNode * ExpressionNode::getRightExpression() const
{
	return m_right;
}


void ExpressionNode::setNumberString ( const std::string & numberString )
{
	m_valueString = numberString;

	DataPointValue dpv ( atof ( m_valueString.c_str() ) );

	updateCurrentState ( dpv, time ( NULL ), QUALITY_GOOD_NO_SPECIFIC_REASON );
}


void ExpressionNode::setDataPointNameString ( const std::string & dataPointName )
{
	m_valueString = dataPointName;
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
			std::auto_ptr<IEntityData> inputEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity ( dataPointName ));
            inputDataPointKey = inputEntityData->getKey();
            inputDataPointName = inputEntityData->getName();
            inputDataPointAgentName = inputEntityData->getAgentName();
            inputDataPointSubsystemKey = inputEntityData->getSubsystem();
            inputDataPointLocationKey = inputEntityData->getLocation();
        }
	}
	catch ( ... )
	{
		// error in obtaining the database entity, update current state
		// to indicate a configuration error
		DataPointValue dpv ( 0 );
		updateCurrentState ( dpv,
							 time ( NULL ),
							 QUALITY_BAD_CONFIGURATION_ERROR );
	}

	if ( 0 != inputDataPointKey )
	{
		// create an instance of ConfiguredInputAssociation
		TA_Base_Core::ConfiguredInputAssociation * configuredInputAssociation = 0;
		configuredInputAssociation = new TA_Base_Core::ConfiguredInputAssociation ( inputDataPointKey,
																			   inputDataPointName,
																			   inputDataPointAgentName,
																			   inputDataPointSubsystemKey,
																			   inputDataPointLocationKey );

		// create an instance of InputAssociation
		m_inputAssociation = new InputAssociation ( configuredInputAssociation );

		if ( 0 != m_inputAssociation )
		{
			m_inputAssociation->registerForStateChangeNotification ( this );

			updateCurrentState ( m_inputAssociation->getCurrentState().getValue(),
								 m_inputAssociation->getCurrentState().getTimestamp(),
								 m_inputAssociation->getCurrentState().getStatus() );
		}
	}
}


void ExpressionNode::processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType)
{
	// input data point has changed its internal state, update the current
	// state with the latest value
	updateCurrentState ( m_inputAssociation->getCurrentState().getValue(),
						 m_inputAssociation->getCurrentState().getTimestamp(),
						 m_inputAssociation->getCurrentState().getStatus() );
	
	// then notify client of the interface IExpressionNodeNotification
	if ( 0 != m_user )
	{
		m_user->processExpressionChanges();
	}	
}


void ExpressionNode::updateCurrentState ( const DataPointValue & dpv,
										  unsigned long tmstamp,
										  DataPointStatus status )
{
	// just update current state with the new info
	m_currentState->setValue ( dpv );
	m_currentState->setTimestamp ( tmstamp );
	m_currentState->setStatus ( status );
}
