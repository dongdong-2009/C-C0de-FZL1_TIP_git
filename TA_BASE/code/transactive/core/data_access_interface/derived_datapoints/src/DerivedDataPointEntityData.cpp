/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * DerivedDataPointEntityData provides read-only parameters that are specific
  * to a Derived DataPoint entity in database
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredDerivedState.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedOutputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"

namespace TA_Base_Core
{

	DerivedDataPointEntityData::DerivedDataPointEntityData ( unsigned long derivedDataPoint_pkey,
															 const std::string & derivedDataPoint_name,
															 bool isWritable):
	    m_pkey ( derivedDataPoint_pkey ),
	    m_name ( derivedDataPoint_name ),
	    m_isWritable(isWritable)
	{
	}



	DerivedDataPointEntityData::~DerivedDataPointEntityData()
	{
	}


	void DerivedDataPointEntityData::getInputAssociations ( std::vector < ConfiguredInputAssociation * > & listOfInputAssociations )
	{
		listOfInputAssociations.clear();

		// get the input associations via the helper
		InputAssociationList inputAssociations(m_derivedDataPointHelper.getInputAssociations(m_pkey));

		// for each of the input associations returned
		for (InputAssociationList::const_iterator iaIt(inputAssociations.begin()); iaIt != inputAssociations.end(); iaIt++)
		{
			if (0 == iaIt->inputDataPointKey)
			{
                continue;
            }

			try
			{
                std::string inputDataPointName;
                std::string inputDataPointAgentName;
                unsigned long inputDataPointSubsystemKey = 0;
                unsigned long inputDataPointLocationKey = 0;
                // first try to get the entity from DataNodeAccessFactory cache
                TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(iaIt->inputDataPointKey);
                if (entityData.get() != NULL)
                {
                    inputDataPointName = entityData->getName();
                    inputDataPointAgentName = entityData->getAgentName();
                    inputDataPointSubsystemKey = entityData->getSubsystem();
                    inputDataPointLocationKey = entityData->getLocation();
                }
                else
                {
			        // if not found, then get other parameters from EntityAccessFactory to load from db
			        // get other parameters from EntityAccessFactory
				    std::auto_ptr<IEntityData> inputEntity(EntityAccessFactory::getInstance().getEntity (iaIt->inputDataPointKey));
                    inputDataPointName = inputEntity->getName();
                    inputDataPointAgentName = inputEntity->getAgentName();
                    inputDataPointSubsystemKey = inputEntity->getSubsystem();
                    inputDataPointLocationKey = inputEntity->getLocation();
                }

				// create ConfiguredInputAssociation to return
				listOfInputAssociations.push_back ( 
					new ConfiguredInputAssociation ( m_pkey,
													 m_name,
													 iaIt->inputDataPointKey,
													 inputDataPointName,
													 inputDataPointAgentName,
													 inputDataPointSubsystemKey,
													 inputDataPointLocationKey,
													 iaIt->inputCalculationOrder,
													 iaIt->inputValue ) );

			}
			catch ( ... )
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "Failed to retrieve entity matching key %lu", iaIt->inputDataPointKey);
			}
		}
	}


	void DerivedDataPointEntityData::getConfiguredDerivedStates ( std::vector < ConfiguredDerivedState * > & listOfDerivedStates )
	{
		listOfDerivedStates.clear();
		std::vector < DerivedOutputAssociation * > listOfOutputAssociation;

		// get the derived states via the helper
		DerivedStateList derivedStates(m_derivedDataPointHelper.getDerivedStates(m_pkey));

		// for each of the derived states returned
		for (DerivedStateList::const_iterator dsIt(derivedStates.begin()); dsIt != derivedStates.end(); dsIt++)
		{
			if (0 == dsIt->stateKey)
            {
                continue;
            }

			// get the list of output entities associated with
			// this derived state
			
			if(m_isWritable)
			{
				listOfOutputAssociation.clear();
				getDerivedOutputAssociation ( dsIt->stateKey, listOfOutputAssociation );
			}

			ConfiguredDerivedState * cds = 
                new ConfiguredDerivedState ( m_pkey,
											 dsIt->stateValue,
											 dsIt->stateDescription,
											 dsIt->isAlarmEnabled,
											 dsIt->alarmDelay,
											 dsIt->alarmSeverity,
											 dsIt->alarmMessage,
											 listOfOutputAssociation,
											 dsIt->stateReturnCondition, //TD15516
											 dsIt->alarmMms);

			if (0 != cds)
			{
				//TD15516, use initialise list instead of call member functions.
				// PW# 3701
				//cds->setStateReturnCondition (  );
				//cds->setAlarmMMS ( dsIt->alarmMms );

				listOfDerivedStates.push_back ( cds );
			}
		}
	}


	void DerivedDataPointEntityData::getDerivedOutputAssociation ( unsigned long derivedState_pkey, std::vector < DerivedOutputAssociation * > & listOfOutputEntities )
	{
		listOfOutputEntities.clear();

		// get the DerivedOutputAssociation from the helper
		OutputAssociationList outputAssociations(m_derivedDataPointHelper.getDerivedOutputAssociations(derivedState_pkey));

		// for each of the output association returned
		for (OutputAssociationList::const_iterator oaIt(outputAssociations.begin()); oaIt != outputAssociations.end(); oaIt++)
		{
			if (0 == oaIt->outputDataPointKey)
			{
                continue;
            }

			try
			{
                std::string outputDataPointName;
                std::string outputDataPointAgentName;
                // first try to get the entity from DataNodeAccessFactory cache
                TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(oaIt->outputDataPointKey);
                if (entityData.get() != NULL)
                {
                    outputDataPointName = entityData->getName();
                    outputDataPointAgentName = entityData->getAgentName();
                }
                else
                {
			        // if not found, then get other parameters from EntityAccessFactory to load from db
				    std::auto_ptr<IEntityData> outputEntity(EntityAccessFactory::getInstance().getEntity(oaIt->outputDataPointKey));
				    outputDataPointName = outputEntity->getName();
				    outputDataPointAgentName = outputEntity->getAgentName();
                }

				// create a DerivedOutputAssociation to return
				listOfOutputEntities.push_back (
					new DerivedOutputAssociation ( oaIt->outputDataPointKey,
												   outputDataPointName,
												   outputDataPointAgentName,
												   oaIt->outputValue ) );
			}
			catch ( ... )
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "Failed to retrieve entity matching key %lu", oaIt->outputDataPointKey);
			}
		}
	}

} //close namespace TA_Base_Core

