#if !defined IPLAN_AGENT_DATA_HEADER_INCLUDED
#define IPLAN_AGENT_DATA_HEADER_INCLUDED

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IPlanAgentData.h $
 * @author:  Mahaveer Pareek
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */
#include "core/data_access_interface/entity_access/src/IEntityData.h"
namespace TA_Base_Core
{
    class IPlanAgentData : public IEntityData
    {
    public:
        virtual ~IPlanAgentData() {}
 		virtual std::string getAssetName() = 0;
    };
}

#endif // endif IPLAN_AGENT_DATA_HEADER_INCLUDED
