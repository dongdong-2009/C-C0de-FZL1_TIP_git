/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DatabaseKey.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * DatabaseKey is small helper class to unify definition of 
  * invalid and valid database primary keys (some disrepancies
  * as to whether they should be 0 or -1 to indicate invalid)
  *
  */

#ifndef _DatabaseKey_HEADER_
#define _DatabaseKey_HEADER_

namespace TA_Base_Core
{
    class DatabaseKey
    {
    public:
        /**
         * getInvalidKey
         *
         * @return a reserved key value that is invalid (ie assumed not to be 
         *          used in a database as a primary key)
         *
         */
        static unsigned long getInvalidKey() { return -1; }

        /**
         * isInvalidKey
         *
         * @param pKey the key value to test
         *
         * @return true if the key is invalid (meaning shouldn't be used in database),
         *          which means pKey == getInvalidKey()
         *
         */
        static bool isInvalidKey(unsigned long pKey) { return getInvalidKey() == pKey; }

        /**
         * isInvalidKey
         *
         * @param pKey the key value to test
         *
         * @return true if the key is valid
         *
         */
        static bool isValidKey(unsigned long pKey) { return !isInvalidKey(pKey); }
    };
}

#endif // _DatabaseKey_HEADER_
