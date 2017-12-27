/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ChangeParentEntityFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to retrieve entities for the Scada Tree. This ensures that the SCADA tree only shows
  * data nodes and does not show any data points.
  */


#if !defined(ChangeParentEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define ChangeParentEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "bus/scada/scada_tree/src/IEntityFactory.h"

// Forward declarations
namespace TA_Base_Core
{
    class IEntityData;
}


namespace TA_Base_App
{
    class ChangeParentEntityFactory : virtual public TA_Base_Bus::IEntityFactory
    {

    public:

        /**
         * Constructor
         */
        ChangeParentEntityFactory();


        /**
         * Destructor
         */
        virtual ~ChangeParentEntityFactory();


        virtual std::vector<TA_Base_Core::IEntityData*> getChildEntitiesOf(unsigned long entityKey);

        virtual std::vector<TA_Base_Core::IEntityData*> getEntitiesOfType(const std::string& entityTypeName);

        virtual TA_Base_Core::IEntityData* getEntity(unsigned long entityKey);

        virtual TA_Base_Core::IEntityData* getEntity(const std::string& entityName);

        /**
         * hasChildren
         *
         * This determines whether the specified entity has children or not.
         * The caller passed in what it has determined so that this method can be lazy if needed
         *
         * @param unsigned long - the key of the entity to check
         * @param bool - This indicates what the entity item object calculated on construction
         */
        virtual bool hasChildren(unsigned long entityKey, bool hasChildren);

    private:
        // This caches lookups determining each entity key and whether it has children
        std::map<unsigned long, bool> m_childCache;
    };
}

#endif // !defined(ChangeParentEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
