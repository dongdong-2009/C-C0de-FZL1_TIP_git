/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/ConfigUpdateDetails.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds all the details of a configuration update ready for processing.
  */


#if !defined(ConfigUpdateDetails_2536FFA6_17E2_4c6a_A497_D4C29B2F8826__INCLUDED_)
#define ConfigUpdateDetails_2536FFA6_17E2_4c6a_A497_D4C29B2F8826__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include <string>

#include "core/configuration_updates/src/EObjectType.h"

#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"

namespace TA_Base_Core
{
    class ConfigUpdateDetails
    {
    public:

        /**
          * Constructor
          * 
          * @param ConfigUpdateMessage& updateDetails - The message holding the update details.
          * @param EObjectType type - The type of object this update is for
          */
        ConfigUpdateDetails(const TA_Base_Core::ConfigUpdateMessageCorbaDef& updateDetails, const EObjectType type);

        
        /**
          * Destructor
          */
        virtual ~ConfigUpdateDetails(){ };


        /**
          * getKey
          * 
          * Returns the key of the object this update is for.
          *
          * @return unsigned long
          */
        unsigned long getKey() const
        {
            return m_key;
        };
        

        /**
          * getType
          * 
          * Returns the type of object this update is for.
          *
          * @return EObjectType
          */
        EObjectType getType() const
        {
            return m_type;
        };

        /**
          * getModification
          * 
          * This returns the type of modification that occurred. 
          *
          * @return EModificationType 
          */
        TA_Base_Core::EModificationType getModifications() const
        {
            return m_modification;
        };
        

        /**
          * getChangedParams
          * 
          * This returns the list of parameters changed.
          *
          * @return const vector<string>&
          */
        const std::vector<std::string>& getChangedParams() const 
        {
            return m_changedParams;
        };


    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        ConfigUpdateDetails( const ConfigUpdateDetails& theConfigUpdateDetails);
        ConfigUpdateDetails& operator=(const ConfigUpdateDetails &);


        //
        // Holds the key of the object that has been updated.
        //
        unsigned long m_key;

        //
        // Holds the type of object that has been updated.
        //
        EObjectType m_type;

        //
        // Holds how the object was modified.
        //
        TA_Base_Core::EModificationType m_modification;

        //
        // Holds a list of all the attributes changed in the object
        //
        std::vector< std::string > m_changedParams;
    };
}

#endif // !defined(ConfigUpdateDetails_2536FFA6_17E2_4c6a_A497_D4C29B2F8826__INCLUDED_)
