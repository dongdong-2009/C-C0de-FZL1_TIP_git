/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessageTypes.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file is include by everything that sends messages
  * it is really just a way of type defining the 
  * MmessageContext and MessageTypeKey
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/types/MessageTypes.h"
#include <algorithm>

namespace TA_Base_Core
{
    void MessageContext::addTypeKey(const std::string& typeKey)
    {
        std::vector<std::string>::iterator iter = std::find(m_messageTypeNames.begin(), m_messageTypeNames.end(), typeKey);
        if ( iter == m_messageTypeNames.end() )
        {
            m_messageTypeNames.push_back(typeKey);
        }

        // TODO LPP: Adding the same type multiple types to a context might be inappropriate.
        // Perhaps a warning or assert would be worthwhile.
    }

}; // TA_Base_Core

