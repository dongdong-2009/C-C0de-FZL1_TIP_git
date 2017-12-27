/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigGlobalParameter.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigGlobalParameter is an interface to a GlobalParameter object. It allows the GlobalParameter object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it. This particular object is a configuration GlobalParameter and allows modifications to be
  * performed in the database.
  */

#if !defined(IConfigGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include "core/data_access_interface/src/IGlobalParameter.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigGlobalParameter : public IConfigItem, public IGlobalParameter
    {

    public:
        virtual ~IConfigGlobalParameter() { };


        /**
         * getDateCreated
         *
         * Returns the date created for this GlobalParameter
         *
         * @return The date created for this GlobalParameter as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this GlobalParameter.
         *
         * @return The date modified for this GlobalParameter as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;


        /**
         * deleteThisGlobalParameter
         *
         * Removes this GlobalParameter from the database. 
         * The calling application MUST then delete this GlobalParameter object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this GlobalParameter was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This GlobalParameter has not been deleted
         */
        virtual void deleteThisGlobalParameter() =0;

        virtual void setValue(const std::string& value) =0;

        virtual void setSubsystemKey(unsigned long subsystemKey) =0;

        
        /**
         * invalidate
         *
         * Make the data contained by this GlobalParameter as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IConfigGlobalParameter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
