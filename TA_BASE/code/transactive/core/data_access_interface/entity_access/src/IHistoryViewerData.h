#ifndef IHISTORYVIEWERDATA_H_INCLUDED
#define IHISTORYVIEWERDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IHistoryViewerData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "core/data_access_interface/entity_access/src/IEntityData.h"


namespace TA_Base_Core
{
	class IHistoryViewerData : public virtual IEntityData
    {

    public:    
        
        IHistoryViewerData() {}

        virtual ~IHistoryViewerData() {}
                  
        // Parameter Methods here...
        
    };

}

#endif // IHISTORYVIEWERDATA_H_INCLUDED
