/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"


namespace TA_Base_Core
{

    
    ConfigChangesTypes::ConfigChangesTypes()
    {
        FUNCTION_ENTRY("ConfigChangesTypes");
        FUNCTION_EXIT;
    }


    ConfigChangesTypes::~ConfigChangesTypes()
    {
        FUNCTION_ENTRY("~ConfigChangesTypes");
        FUNCTION_EXIT;
    }


    void ConfigChangesTypes::updateChanges( ItemChanges& itemChanges,
                                            const std::string& parameterName,
                                            const std::string& oldValue,
                                            const std::string& newValue )
    {
        FUNCTION_ENTRY("updateChanges");
        // Search for this name in the map
        ItemChanges::iterator matching = itemChanges.find(parameterName);

        //Find the matching changes if they exist,
        //update if necessary.
        if (matching != itemChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                itemChanges.erase(matching);
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
            itemChanges.insert( ItemChanges::value_type( parameterName, values ) );
        }

        FUNCTION_EXIT;
    }


}

