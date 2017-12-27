/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigDerivedOutputAssociation.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * DerivedOutputAssociation provides read-write parameters that are specific
  * to a Derived DataPoint output association in the database
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>

#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedOutputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const std::string ConfigDerivedOutputAssociation::OUTPUT_ASSOCIATION = "Output Association ";
    const std::string ConfigDerivedOutputAssociation::OUTPUT_VALUE = " Output Value";
    const std::string ConfigDerivedOutputAssociation::STATE = " State ";

	ConfigDerivedOutputAssociation::ConfigDerivedOutputAssociation( ConfigDerivedDataPoint& parent,
                                                                    unsigned long key,
                                                                    unsigned long outputEntityKey,
									                                int outputValue)
		: DerivedOutputAssociation(outputEntityKey, "", "", outputValue),
		  m_parent(parent), m_key(key)
	{
	}


    ConfigDerivedOutputAssociation::ConfigDerivedOutputAssociation ( const ConfigDerivedOutputAssociation & obj )
        : DerivedOutputAssociation( obj.m_outputEntityKey, "", "", obj.m_outputValue ),
          m_parent(obj.m_parent), m_key(0)
    {
    }


    ConfigDerivedOutputAssociation::~ConfigDerivedOutputAssociation()
    {
        try
        {
            // No longer need a record of any changes that were made so update
            // the changes to remove these
            std::ostringstream parameter;
            parameter << OUTPUT_ASSOCIATION << getEntityName() << OUTPUT_VALUE;
            m_parent.removeChanges(parameter.str());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught in the destructor");
        }
    }


	void ConfigDerivedOutputAssociation::setOutputValue(int outputValue)
	{
        std::ostringstream oldValue;
        std::ostringstream newValue;
        oldValue << m_outputValue;
        newValue << outputValue;

        std::ostringstream parameter;
        parameter << OUTPUT_ASSOCIATION << getEntityName() << OUTPUT_VALUE;

        m_parent.updateChanges(parameter.str(), oldValue.str(), newValue.str());

        m_outputValue = outputValue;
	}


    std::string ConfigDerivedOutputAssociation::getEntityName()
    {
        return getEntityName(m_outputEntityKey);
    }


    std::string ConfigDerivedOutputAssociation::getEntityName(unsigned long key)
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

