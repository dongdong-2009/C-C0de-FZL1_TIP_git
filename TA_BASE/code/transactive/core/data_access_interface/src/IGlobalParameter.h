/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IGlobalParameter.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IGlobalParameter is an interface to a GlobalParameter object. It allows the GlobalParameter object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  * NOTE: This is temporary and must be changed later.
  */

#if !defined(IGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IGlobalParameter : public IItem
    {

    public:
        virtual ~IGlobalParameter() { };

		virtual unsigned long getKey() = 0;

        virtual std::string getValue() = 0;

        virtual unsigned long getSubsystemKey() = 0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
