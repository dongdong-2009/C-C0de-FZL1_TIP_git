/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/scada_tree/src/IEntityFactory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifndef IENTITY_FACTORY_H
#define IENTITY_FACTORY_H

namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
	class IEntityFactory
	{
        public:
            virtual ~IEntityFactory(){};

            virtual std::vector<TA_Base_Core::IEntityData*> getChildEntitiesOf(unsigned long entityKey) =0;

            virtual std::vector<TA_Base_Core::IEntityData*> getEntitiesOfType(const std::string& entityTypeName) =0;

            virtual TA_Base_Core::IEntityData* getEntity(unsigned long entityKey) =0;

            virtual TA_Base_Core::IEntityData* getEntity(const std::string& entityName) =0;


            /**
             * hasChildren
             *
             * This determines whether the specified entity has children or not.
             * The caller passed in what it has determined so that this method can be lazy if needed
             *
             * @param unsigned long - the key of the entity to check
             * @param bool - This indicates what the entity item object calculated on construction
             */
            virtual bool hasChildren(unsigned long entityKey, bool hasChildren) =0;
	};
}


#endif // IENTITY_FACTORY_H
