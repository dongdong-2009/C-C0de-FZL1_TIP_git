#ifndef SCADAHISTORYAGENTENTITYDATA_H_INCLUDED
#define SCADAHISTORYAGENTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ScadaHistoryAgentEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class ScadaHistoryAgentEntityData : public EntityData
    {
    public:

        ScadaHistoryAgentEntityData() {};

        ScadaHistoryAgentEntityData(unsigned long key) :
              EntityData(key, getStaticType() ) {};

        static IEntityData* clone(unsigned long key)
        {
            return new ScadaHistoryAgentEntityData(key);        
        }

        std::string getType();
		static std::string getStaticType();

        int getMaxTrendDatapointsPerStation();
        

    /**
     * implement methods as required by the interfaces from
     * which this class inherits
     */
    private:

        ScadaHistoryAgentEntityData(ScadaHistoryAgentEntityData&);

        ScadaHistoryAgentEntityData& operator=(ScadaHistoryAgentEntityData&);

        void invalidate();

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string MAXTRENDDATAPOINTSPERSTATION;
    };
    
    typedef boost::shared_ptr<ScadaHistoryAgentEntityData> ScadaHistoryAgentEntityDataPtr;
}

#endif // SCADAHISTORYAGENTENTITYDATA_H_INCLUDED
