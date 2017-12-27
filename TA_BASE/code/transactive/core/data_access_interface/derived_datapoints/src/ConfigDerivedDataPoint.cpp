/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * This class provides read-write access to all attributes of a derived datapoint.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include <vector>

#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h"

#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedOutputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointHelper.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    const std::string ConfigDerivedDataPoint::INCLUDED = "Included";
    const std::string ConfigDerivedDataPoint::NOT_INCLUDED = "Not Included";

    ConfigDerivedDataPoint::ConfigDerivedDataPoint(unsigned long pkey, bool isWritable)
        : m_isValidData(false),
          m_isNew(false),
          m_key(pkey),
          m_areStatesSetManually(false),
          m_isWritable(isWritable)
    {
    }


    ConfigDerivedDataPoint::ConfigDerivedDataPoint()
        : m_isValidData(true),
          m_isNew(true),
          m_key(0),
          m_areStatesSetManually(false),
          m_isWritable(true)
    {
    }


    ConfigDerivedDataPoint::ConfigDerivedDataPoint(const ConfigDerivedDataPoint& obj)
        : m_isValidData(true),
          m_isNew(true),
          m_key(0),
          m_areStatesSetManually(obj.m_areStatesSetManually),
          m_isWritable(obj.m_isWritable)
    {
        for(InputAssociationMap::const_iterator input = obj.m_inputAssociations.begin(); input != obj.m_inputAssociations.end(); ++input)
        {
            ConfigConfiguredInputAssociation* inputAssoc = new ConfigConfiguredInputAssociation(*(input->second));
            m_inputAssociations.insert( InputAssociationMap::value_type( input->first, inputAssoc ) );
        }

        for(OutputAssociationMap::const_iterator output = obj.m_outputAssociations.begin(); output != obj.m_outputAssociations.end(); ++output)
        {
            ConfigDerivedOutputAssociation* outputAssoc = new ConfigDerivedOutputAssociation(*(output->second));
            m_outputAssociations.insert( OutputAssociationMap::value_type( output->first, outputAssoc ) );
        }

        for(StateMap::const_iterator state = obj.m_derivedStates.begin(); state != obj.m_derivedStates.end(); ++state)
        {
            ConfigConfiguredDerivedState* derivedState = new ConfigConfiguredDerivedState(*(state->second));
            m_derivedStates.insert( StateMap::value_type( state->first, derivedState ) );
        }
    }


    ConfigDerivedDataPoint::~ConfigDerivedDataPoint()
    {
        try
        {
            for (StateMap::iterator state = m_derivedStates.begin(); state != m_derivedStates.end(); ++state)
            {
                delete state->second;
                state->second = NULL;
            }
            m_derivedStates.clear();


            for (InputAssociationMap::iterator input = m_inputAssociations.begin(); input != m_inputAssociations.end(); ++input)
            {
                delete input->second;
                input->second = NULL;
            }
            m_inputAssociations.clear();


            for (OutputAssociationMap::iterator output = m_outputAssociations.begin(); output != m_outputAssociations.end(); ++output)
            {
                delete output->second;
                output->second = NULL;
            }
            m_outputAssociations.clear();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught in Destructor");
        }
    }


    ItemChanges ConfigDerivedDataPoint::getAllItemChanges()
    {
        return m_changes;
    }


    void ConfigDerivedDataPoint::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(!m_isNew, "This is a new derived data point we are trying to invalidate");

        m_isValidData = false;

        m_changes.clear();

        for (StateMap::iterator state = m_derivedStates.begin(); state != m_derivedStates.end(); ++state)
        {
            delete state->second;
            state->second = NULL;
        }
        m_derivedStates.clear();


        for (InputAssociationMap::iterator input = m_inputAssociations.begin(); input != m_inputAssociations.end(); ++input)
        {
            delete input->second;
            input->second = NULL;
        }
        m_inputAssociations.clear();


        for (OutputAssociationMap::iterator output = m_outputAssociations.begin(); output != m_outputAssociations.end(); ++output)
        {
            delete output->second;
            output->second = NULL;
        }
        m_outputAssociations.clear();

        FUNCTION_EXIT;
    }


    const ConfigDerivedDataPoint::InputAssociationMap& ConfigDerivedDataPoint::getInputAssociations()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_inputAssociations;
    }


    const ConfigDerivedDataPoint::OutputAssociationMap& ConfigDerivedDataPoint::getOutputAssociations()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_outputAssociations;
    }


    const ConfigDerivedDataPoint::StateMap& ConfigDerivedDataPoint::getDerivedStates()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_derivedStates;
    }

 
    void ConfigDerivedDataPoint::setNumberOfStates(int numberOfStates)
    {
        FUNCTION_ENTRY("setNumberOfStates");

        int currentNumberOfStates = m_derivedStates.size();

        m_areStatesSetManually = true;

        if (currentNumberOfStates == numberOfStates)
        {
            FUNCTION_EXIT;
            return;
        }


        if (currentNumberOfStates > numberOfStates)
        {
            // We must reduce the number of states
            for (int i = currentNumberOfStates -1; i >= numberOfStates; --i)
            {
                // We must remove those extra states now.
                StateMap::iterator matching = m_derivedStates.find( i );
                if ( matching != m_derivedStates.end() )
                {
                    std::ostringstream parameter;
                    parameter << ConfigConfiguredDerivedState::STATE << i;
                    updateChanges(parameter.str(), INCLUDED, NOT_INCLUDED);

                    delete matching->second;
                    matching->second = NULL;
                    m_derivedStates.erase( matching );
                }
            }
            FUNCTION_EXIT;
            return;
        }

        // else we need to increase the number of states
        for (unsigned int i = currentNumberOfStates; i < (unsigned int) numberOfStates; ++i)
        {
            ConfigConfiguredDerivedState* newState = new ConfigConfiguredDerivedState(*this, i);
            m_derivedStates.insert(StateMap::value_type( i, newState ) );

            std::ostringstream parameter;
            parameter << ConfigConfiguredDerivedState::STATE << i;
            updateChanges(parameter.str(), NOT_INCLUDED, INCLUDED);
        }

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::addInputAssociation(unsigned long entityKey)
    {
        FUNCTION_ENTRY("addInputAssociation");

        if (!m_isValidData)
        {
            reload();
        }

        InputAssociationMap::iterator input = m_inputAssociations.find(entityKey);
        if (input != m_inputAssociations.end())
        {
            // Already added so no need to add again
            FUNCTION_EXIT;
            return;
        }

        ConfigConfiguredInputAssociation* inputAssoc = new ConfigConfiguredInputAssociation( *this, entityKey );
        m_inputAssociations.insert (InputAssociationMap::value_type(entityKey, inputAssoc) );

        std::string parameter(ConfigConfiguredInputAssociation::INPUT_ASSOCIATION + inputAssoc->getEntityName());
        updateChanges(parameter, NOT_INCLUDED, INCLUDED);

        // Now adding an extra input association means more derived states are now required.
        // We must add these as well. Required number of states = 2^numOfInputAssocs
        int numOfInputAssocs = m_inputAssociations.size();
        int numberOfStatesRequired = 1;
        for( int n = 1; n <= numOfInputAssocs; ++n)
        {
            numberOfStatesRequired = numberOfStatesRequired * 2;
        }

        int numberOfActualStates = m_derivedStates.size();
        for (int i = numberOfActualStates; i < numberOfStatesRequired; ++i)
        {
            ConfigConfiguredDerivedState* newState = new ConfigConfiguredDerivedState(*this, i);
            m_derivedStates.insert(StateMap::value_type( i, newState ) );

            std::ostringstream parameter;
            parameter << ConfigConfiguredDerivedState::STATE << i;
            updateChanges(parameter.str(), NOT_INCLUDED, INCLUDED);
        }

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::removeInputAssociation(unsigned long entityKey)
    {
        FUNCTION_ENTRY("removeInputAssociation");

        if (!m_isValidData)
        {
            reload();
        }

        InputAssociationMap::iterator input = m_inputAssociations.find(entityKey);
        if (input == m_inputAssociations.end())
        {
            // Not found so no need to remove
            FUNCTION_EXIT;
            return;
        }

        std::string parameter(ConfigConfiguredInputAssociation::INPUT_ASSOCIATION + input->second->getEntityName());
        updateChanges(parameter, INCLUDED, NOT_INCLUDED);

        delete input->second;
        input->second = NULL;
        m_inputAssociations.erase( input );


        // Now removing an input association means less derived states are now required.
        // We must remove these as well. Required number of states = 2^numOfInputAssocs
        int numOfInputAssocs = m_inputAssociations.size();
        int numberOfStatesRequired = 1;
        for( int n = 1; n <= numOfInputAssocs; ++n)
        {
            numberOfStatesRequired = numberOfStatesRequired * 2;
        }

        // If there are 0 input associations then this will give us 1 state. Change this to 0
        if (numberOfStatesRequired == 1)
        {
            numberOfStatesRequired = 0;
        }

        
        int numberOfActualStates = m_derivedStates.size();
        for (int i = numberOfActualStates -1; i >= numberOfStatesRequired; --i)
        {
            // We must remove those extra states now.
            StateMap::iterator matching = m_derivedStates.find( i );
            if ( matching != m_derivedStates.end() )
            {
                std::ostringstream parameter;
                parameter << ConfigConfiguredDerivedState::STATE << i;
                updateChanges(parameter.str(), INCLUDED, NOT_INCLUDED);

                delete matching->second;
                matching->second = NULL;
                m_derivedStates.erase( matching );
            }

            // We also must remove any output associations that point to one of the now defunct states
            OutputAssociationMap::iterator iter = m_outputAssociations.find( i );
            while ( iter != m_outputAssociations.end() && iter->first == i)
            {
                std::ostringstream parameter;
                parameter << ConfigDerivedOutputAssociation::OUTPUT_ASSOCIATION << iter->second->getEntityName();
                updateChanges(parameter.str(), INCLUDED, NOT_INCLUDED);

                delete iter->second;
                iter->second = NULL;

                m_outputAssociations.erase(iter++);
            }
        }

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::addOutputAssociation(int state, unsigned long entityKey)
    {
        FUNCTION_ENTRY("addOutputAssociation");

        if (!m_isValidData)
        {
            reload();
        }

        ConfigDerivedOutputAssociation* outputAssoc = new ConfigDerivedOutputAssociation( *this, 0, entityKey, 0 );
        m_outputAssociations.insert (OutputAssociationMap::value_type(state, outputAssoc) );

        std::ostringstream parameter;
        parameter << ConfigDerivedOutputAssociation::OUTPUT_ASSOCIATION << outputAssoc->getEntityName();
        updateChanges(parameter.str(), NOT_INCLUDED, INCLUDED);

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::removeOutputAssociation(int state, unsigned long entityKey)
    {
        FUNCTION_ENTRY("removeOutputAssociation");

        if (!m_isValidData)
        {
            reload();
        }

        OutputAssociationMap::iterator matching = m_outputAssociations.find( state );
        while ( matching != m_outputAssociations.end() && matching->first == state )
        {
            if (matching->second->getOutputEntityKey() == entityKey)
            {
                std::ostringstream parameter;
                parameter << ConfigDerivedOutputAssociation::OUTPUT_ASSOCIATION << matching->second->getEntityName();
                updateChanges(parameter.str(), NOT_INCLUDED, INCLUDED);

                delete matching->second;
                matching->second = NULL;
                m_outputAssociations.erase( matching );

                FUNCTION_EXIT;
                return;
            }
            ++matching;
        }

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_changes.find(name);

        if (matching != m_changes.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_changes.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_changes.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::removeChanges(const std::string& name)
    {
        FUNCTION_ENTRY("removeChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_changes.find(name);

        if (matching != m_changes.end())
        {
            m_changes.erase(matching);
        }


        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_key != 0, "The key must be set on new derived data points before they are applied");

        DerivedDataPointHelper helper;

        if (m_isNew)
        {
            helper.addNew(m_key, m_inputAssociations, m_derivedStates, m_outputAssociations);
        }
        else
        {
            helper.modifyExisting(m_key, m_inputAssociations, m_derivedStates, m_outputAssociations);
        }

        m_changes.clear();
        m_isNew = false;
        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void ConfigDerivedDataPoint::deleteThisDataPoint()
    {
        DerivedDataPointHelper helper;  
        helper.deleteDerivedDataPoint( m_key );
    }


    void ConfigDerivedDataPoint::reload()
 {
        FUNCTION_ENTRY("reload");

        DerivedDataPointHelper helper;

        // First load all the input associations
    	InputAssociationList inputs = helper.getInputAssociations ( m_key );
        
        // Step through each input association and add an object to our map
        for (InputAssociationList::const_iterator iaIt(inputs.begin()); iaIt != inputs.end(); iaIt++)
        {
            ConfigConfiguredInputAssociation* inputAssoc = new ConfigConfiguredInputAssociation( *this,
                                                                                                 iaIt->inputAssociationKey,
                                                                                                 iaIt->inputDataPointKey,
                                                                                                 iaIt->inputCalculationOrder,
                                                                                                 iaIt->inputValue);

         if( !m_inputAssociations.insert( InputAssociationMap::value_type(iaIt->inputDataPointKey, inputAssoc)).second)
            {
            	delete inputAssoc;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Failed to insert inputAssoc to map,may be same inputDataPointKey [%d] aready exist",
					iaIt->inputDataPointKey);
            }
     }

        // Now load all the derived states
        DerivedStateList states = helper.getDerivedStates( m_key );

        // We must store all the derived state pkey's and state numbers as we go so we can load the output
        // associations next
        std::vector< StateDetails > stateDetails;
        stateDetails.reserve(states.size());

        // Step through each derived state and add an object to our map
        for (DerivedStateList::const_iterator dsIt(states.begin()); dsIt != states.end(); dsIt++)
        {
     	
            StateDetails details;
            details.stateKey = dsIt->stateKey;
            details.stateValue = dsIt->stateValue;
            stateDetails.push_back( details );
			
         ConfigConfiguredDerivedState* derivedState = new ConfigConfiguredDerivedState( *this,
                                                                                           dsIt->stateKey,
                                                                                           dsIt->stateValue,
                                                                                           dsIt->stateDescription,
                                                                                           dsIt->isAlarmEnabled,
                                                                                           dsIt->alarmDelay,
                                                                                           dsIt->alarmSeverity,
                                                                                           dsIt->alarmMessage );

         if(!m_derivedStates.insert( StateMap::value_type(dsIt->stateValue, derivedState) ).second)
         {
         	delete derivedState;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Failed to insert derivedState to map,may be same statevalue aready exist,[stateKey:%d,value:%d] ",
					dsIt->stateKey,dsIt->stateValue);
         }
        }

     // Finally load all the output associations. Step through each derived state and load the output associations for it; Just Output DataPoint need them.
        if(m_isWritable)
        {
	        for (std::vector< StateDetails >::const_iterator iter = stateDetails.begin(); iter != stateDetails.end(); ++iter )
	        {
	    	    OutputAssociationList outputs = helper.getDerivedOutputAssociations( iter->stateKey );
	        
	            // Step through each output association and add an object to our map
	            for (OutputAssociationList::const_iterator oaIt(outputs.begin()); oaIt != outputs.end(); oaIt++)
	            {
	                ConfigDerivedOutputAssociation* outputAssoc = new ConfigDerivedOutputAssociation( *this,
	                                                                                                  oaIt->outputAssociationKey,
	                                                                                                  oaIt->outputDataPointKey,
	                                                                                                  oaIt->outputValue);
	                m_outputAssociations.insert( OutputAssociationMap::value_type(iter->stateValue, outputAssoc) );
	                
	            }
	        }
   		}
     // If we have no input associations but there are derived states then this means we are dealing with
        // and output datapoint (and not a derived datapoint).
        if (m_inputAssociations.size() == 0 && m_derivedStates.size() != 0 )
        {
            m_areStatesSetManually = true;
        }

        m_isValidData = true;

        FUNCTION_EXIT;
    }



}
