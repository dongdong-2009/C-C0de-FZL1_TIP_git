#if !defined(TAuuid_BA0F8A17_4BA7_4d9a_8A6D_339DB2B0B0AF__INCLUDED_)
#define TAuuid_BA0F8A17_4BA7_4d9a_8A6D_339DB2B0B0AF__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/uuid/src/TAuuid.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TA_Uuid is used to generate and compare UUID's (Universally Unique Identifier).
  */

#include <string>

namespace TA_Base_Core
{
    namespace TA_Uuid
    {
        /**
	      * generateUUID()
	      * 
	      * generateUUID will generate a UUID and return it as a std::string
	      *
	      * @return std::string representation of the UUID
	      */
	    std::string generateUUID();


	    /**
	      * compareUUID()
	      * 
	      * compareUUID will compare to strings representing UUIDs.
	      *
	      * @return bool true if UUIDs are the same, false otherwise
	      */
	    bool compareUUID(const std::string& subject, const std::string& test);
    }
};

#endif // !defined(TAuuid_BA0F8A17_4BA7_4d9a_8A6D_339DB2B0B0AF__INCLUDED_)
