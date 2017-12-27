/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfigConfiguredInputAssociation provides read-write parameters that are specific
  * to a Derived DataPoint input association in the database
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>

#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const std::string ConfigConfiguredInputAssociation::INPUT_ASSOCIATION = "Input Association ";
    const std::string ConfigConfiguredInputAssociation::CALCULATION_ORDER = " Calculation Order";
    const std::string ConfigConfiguredInputAssociation::PREDEFINED_INPUT = " Predefined Input Value";

	ConfigConfiguredInputAssociation::ConfigConfiguredInputAssociation( ConfigDerivedDataPoint& parent,
                                                                        unsigned long key,
                                                                        unsigned long inputEntityKey,
                                                                        int inputCalculationOrder,
                                                                        double predefinedInputValue )
	: ConfiguredInputAssociation(0, "", inputEntityKey, "", "", 0, 0, inputCalculationOrder, predefinedInputValue),
	  m_parent(parent), m_key(key)
	{
	}


    ConfigConfiguredInputAssociation::ConfigConfiguredInputAssociation(ConfigDerivedDataPoint& parent,
                                                                       unsigned long inputEntityKey)
        : ConfiguredInputAssociation(0,"",inputEntityKey,"","",0,0,0,0),
          m_parent(parent),m_key(0)
    {
    }


    ConfigConfiguredInputAssociation::ConfigConfiguredInputAssociation ( const ConfigConfiguredInputAssociation & obj )
        : ConfiguredInputAssociation(0, "", obj.m_inputEntityKey, "", "", 0, 0,
                                     obj.m_inputCalculationOrder, obj.m_predefinedInputValue ),
          m_parent(obj.m_parent), m_key(0)
    {
    }


    ConfigConfiguredInputAssociation::~ConfigConfiguredInputAssociation()
    {
        try
        {
            // No longer need a record of any changes that were made so update
            // the changes to remove these
            std::ostringstream parameter;
            parameter << INPUT_ASSOCIATION << getEntityName() << CALCULATION_ORDER;
            m_parent.removeChanges(parameter.str());

            parameter.str("");
            parameter << INPUT_ASSOCIATION << getEntityName() << PREDEFINED_INPUT;
            m_parent.removeChanges(parameter.str());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught in the destructor");
        }
    }


    void ConfigConfiguredInputAssociation::setInputCalculationOrder(int calculationOrder)
    {
        std::ostringstream oldValue;
        std::ostringstream newValue;
        oldValue << m_inputCalculationOrder;
        newValue << calculationOrder;

        std::ostringstream parameter;
        parameter << INPUT_ASSOCIATION << getEntityName() << CALCULATION_ORDER;

        m_parent.updateChanges(parameter.str(), oldValue.str(), newValue.str());

        m_inputCalculationOrder = calculationOrder;
    }


    void ConfigConfiguredInputAssociation::setPredefinedInputValue(double predefinedInput)
    {
        std::ostringstream oldValue;
        std::ostringstream newValue;
        oldValue << m_predefinedInputValue;
        newValue << predefinedInput;

        std::ostringstream parameter;
        parameter << INPUT_ASSOCIATION << getEntityName() << PREDEFINED_INPUT;

        m_parent.updateChanges(parameter.str(), oldValue.str(), newValue.str());

        m_predefinedInputValue = predefinedInput;
    }

    
    std::string ConfigConfiguredInputAssociation::getEntityName()
    {
        return getEntityName(m_inputEntityKey);
    }


    std::string ConfigConfiguredInputAssociation::getEntityName(unsigned long key)
    {
        IEntityData* data = NULL;
        std::string name = "";
        try
        {
            data = EntityAccessFactory::getInstance().getEntity( key );
            name = data->getName();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve an entity name for the given entity key");
        }

        if (data != NULL)
        {
            delete data;
            data = NULL;
        }

        return name;
    }

}


