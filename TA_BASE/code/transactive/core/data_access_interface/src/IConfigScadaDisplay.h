/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigScadaDisplay.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is an interface for a Scada Display from the database that can be configured and modified
  */

#if !defined(IConfigScadaDisplay_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigScadaDisplay_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IProfile;

    class IConfigScadaDisplay : public IScadaDisplay, public virtual IConfigItem
    {

    public:

        /**
         * setPath
         *
         * Sets the path of this Scada display.
         *
         * @param string - The path for the display
         *
         * pre: This display has not been deleted
         */
        virtual void setPath(const std::string& path) = 0;


        /**
         * setLocation
         *
         * Sets the location of this Scada display.
         *
         * @param unsigned long - The key of the location for this display
         *
         * pre: This display has not been deleted
         */
        virtual void setLocation(unsigned long location) =0;
        

        /**
         * deleteThisObject
         *
         * Removes this display from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - apply() has already been called
         *      This display has not been deleted
         */
        virtual void deleteThisObject() = 0;


        virtual ~IConfigScadaDisplay() {};
    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigScadaDisplay_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
