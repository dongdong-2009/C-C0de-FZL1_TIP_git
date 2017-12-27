/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to retrieve entities for the Scada Tree. This ensures that the SCADA tree only shows
  * boolean data nodes and does not show other select data points.
  */


#if !defined(SelectAssociationEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define SelectAssociationEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

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
    class SelectAssociationEntityFactory : virtual public TA_Base_Bus::IEntityFactory
    {

    public:

        /**
         * Constructor
         *
         * @param vector<unsigned long> - This is a list of all keys that should be ignored and not
         *                                displayed in the tree.
         * @param bool - This indicates whether the tree should only show boolean data points or whether
         *               it should show all data points'
         * @param bool - This indicates whether the tree should only show datapoints that are output
         *               datapoints. If this is set to false then the tree will only show input datapoints.
         */
        SelectAssociationEntityFactory(std::vector<unsigned long> dataPointsToIgnore,
                                       bool shouldOnlyShowBoolean,
                                       bool shouldOnlyShowOutput);


        /**
         * Destructor
         */
        virtual ~SelectAssociationEntityFactory();


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
        bool m_shouldOnlyShowBoolean;
        bool m_shouldOnlyShowOutput;
        std::vector<unsigned long> m_dataPointsToIgnore;

        // This caches lookups determining each entity key and whether it has children
        std::map<unsigned long, bool> m_childCache;
    };
}

#endif // !defined(SelectAssociationEntityFactory_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
