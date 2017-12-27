/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfiguredInputAssociation provides read-write parameters that are specific
  * to a Derived DataPoint input association in the database
  */


#ifndef CONFIG_CONFIGURED_INPUT_ASSOCIATION_H
#define CONFIG_CONFIGURED_INPUT_ASSOCIATION_H

#include <string>

#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"


namespace TA_Base_Core
{
    class ConfigDerivedDataPoint;

    class ConfigConfiguredInputAssociation : public ConfiguredInputAssociation
	{
	public:

        /**
         * Constructor
         *
         * @param ConfigDerivedDataPoint& - This is the derived data point class that we can use to
         *                                  record any changes made to our members.
         * @param unsigned long - The key from the database for this object. This will be 0 if no key is known
         * @param unsigned long - The entity key of this input association
         * @param int - The order in which this input falls
         * @param double - The predefined input value of the input
         */
		ConfigConfiguredInputAssociation( ConfigDerivedDataPoint& parent,
                                          unsigned long key,
                                          unsigned long inputEntityKey,
                                          int inputCalculationOrder,
                                          double predefinedInputValue );


        /**
         * Constructor
         *
         * Create a new input association
         *
         * @param ConfigDerivedDataPoint& - This is the derived data point class that we can use to
         *                                  record any changes made to our members.
         * @param unsigned long - The entity key of this input association
         */
		ConfigConfiguredInputAssociation(ConfigDerivedDataPoint& parent,
                                         unsigned long inputEntityKey);


        /**
         * Copy Constructor
         */
		ConfigConfiguredInputAssociation ( const ConfigConfiguredInputAssociation & obj );


        /**
         * Destructor
         */
        virtual ~ConfigConfiguredInputAssociation();


        /**
         * getKey
         *
         * This retrieves the primary key for this input association
         *
         * @return unsigned long - The primary key. 0 if one has not been set
         */
        unsigned long getKey() const
        {
            return m_key;
        }


        /**
         * getEntityName
         *
         * This returns the name for the input entity
         *
         * @return string - The name of the entity
         */
        std::string getEntityName();


        /**
         * getEntityName
         *
         * This returns the name for the input entity
         *
         * @param unsigned long - The key of the entity to retrieve the name for
         * 
         * @return string - The name of the entity
         */
        std::string getEntityName(unsigned long key);


        /**
         * setInputCalculationOrder
         *
         * Sets the input calculation order for this input association
         *
         * @param int - The calculation order
         */
        void setInputCalculationOrder(int calculationOrder);


        /**
         * setPredefinedInputValue
         *
         * Sets the predefined input value
         *
         * @param double - The input value
         */
        void setPredefinedInputValue(double predefinedInput);


        /**
         * setKey
         *
         * This sets the database primary key for this input association
         *
         * @param unsigned long - The primary key
         */
        void setKey(unsigned long key)
        {
            m_key = key;
        }


        static const std::string INPUT_ASSOCIATION;
        static const std::string CALCULATION_ORDER;
        static const std::string PREDEFINED_INPUT;

    private:
		ConfigConfiguredInputAssociation & operator= ( const ConfigConfiguredInputAssociation & );


        ConfigDerivedDataPoint& m_parent;

        unsigned long m_key;
	};
}

#endif //CONFIG_CONFIGURED_INPUT_ASSOCIATION_H

