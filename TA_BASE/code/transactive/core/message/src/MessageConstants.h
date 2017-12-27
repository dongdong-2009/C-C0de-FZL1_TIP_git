#ifndef MESSAGECONSTANTS_H
#define MESSAGECONSTANTS_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessageConstants.h $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Common constant values for the Messages subsystem
  */

namespace TA_Base_Core
{
    //
    // Names used for Filterable Body Fields
    //
    const char* const ENTITYKEY_FILTER_NAME       = "EntityKey";
    const char* const ASSOC_ENTITYKEY_FILTER_NAME = "AssocEntityKey";
    const char* const SUBSYSTEM_FILTER_NAME       = "Subsystem";
    const char* const REGION_FILTER_NAME          = "Region";
    const char* const AGENTNAME_FILTER_NAME       = "AgentName";

    const char* const MESSAGETYPE_FILTER_NAME     = "MessageType";
}

#endif // MESSAGECONSTANTS_H
