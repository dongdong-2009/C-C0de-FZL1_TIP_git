/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigDerivedOutputAssociation.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * DerivedOutputAssociation provides read-write parameters that are specific
  * to a Derived DataPoint output association in the database
  */


#ifndef CONFIG_DERIVED_OUTPUT_ASSOCIATION_H
#define CONFIG_DERIVED_OUTPUT_ASSOCIATION_H

#include <string>

#include "core/data_access_interface/derived_datapoints/src/DerivedOutputAssociation.h"

namespace TA_Base_Core
{ 
    class ConfigDerivedDataPoint;

    class ConfigDerivedOutputAssociation : public DerivedOutputAssociation
	{
	public:

        /**
         * Constructor
         *
         *
         * @param ConfigDerivedDataPoint& - This is the derived data point class that we can use to
         *                                  record any changes made to our members.
         * @param unsigned long - The key from the database for this object. This will be 0 if no key is known
         * @param unsigned long - The entity key of this output association
         * @param int - The output value for this association
         */
		ConfigDerivedOutputAssociation( ConfigDerivedDataPoint& parent,
                                        unsigned long key,
                                        unsigned long outputEntityKey,
									    int outputValue );


        /**
         * Copy Constructor
         */
        ConfigDerivedOutputAssociation ( const ConfigDerivedOutputAssociation & obj );


        /**
         * Destructor
         */
		virtual ~ConfigDerivedOutputAssociation();


        /**
         * setOutputValue
         *
         * Set the value to be set to the output entity associated with a derived state
         *
         * @param int - The new output value
         */
		void setOutputValue(int outputValue);


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
         * This returns the name for the output entity
         *
         * @return string - The name of the entity
         */
        std::string getEntityName();


        /**
         * getEntityName
         *
         * This returns the name for the specified entity
         *
         * @param unsigned long -The key of the entity to retrieve a name for
         *
         * @return string - The name of the entity
         */
        std::string getEntityName(unsigned long key);



        static const std::string OUTPUT_ASSOCIATION;
        static const std::string OUTPUT_VALUE;
        static const std::string STATE;


	private:

        ConfigDerivedOutputAssociation& operator=(const ConfigDerivedOutputAssociation&);

        ConfigDerivedDataPoint& m_parent;

        unsigned long m_key;
	};

}

#endif //CONFIG_DERIVED_OUTPUT_ASSOCIATION_H

