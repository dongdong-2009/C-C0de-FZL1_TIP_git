/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigOperator.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigOperator - This is the interface supported by objects that allow configuration of
  * operators. It provides methods for retrieving, settings, and writing data for an operator object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigOperator_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigOperator_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IProfile;

    class IConfigOperator : public IOperator, public virtual IConfigItem
    {

    public:

        /**
         * setDescription
         *
         * Sets the description of this operator. This should only be used by the ConfigOperator class
         *
         * @param description The description to give this Operator
         *
         * pre: This operator has not been deleted
         */
        virtual void setDescription(const std::string& description) = 0;


        /**
         * setPassword
         *
         * Sets the password for this Operator. This should only be used by the ConfigOperator class.
         *
         * @param password The password to give this Operator.
         *
         * pre: This operator has not been deleted
         */
        virtual void setPassword( const std::string password ) = 0;


        /**
         * setAssociatedProfile
         *
         * Changes a profile association for this operator
         *
         * @param unsigned long - This is the key of the profile to add or remove from the associations
         * @param bool - This indicates whether this profile is to be added or removed. True if the
         *               profile is to be added to the associations, false if the profile is to be
         *               removed.
         *
         * pre: This operator has not been deleted
         */
        virtual void setAssociatedProfile( unsigned long profileKey, bool isAssociated) =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this operator
         *
         * @return The date created for this operator as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this operator.
         *
         * @return The date modified for this operator as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        /**
         * getAssociatedProfileKeys
         *
         * Returns the profile keys that are accessible by this operator
         *
         * @return The profiles accessible by this operator, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAssociatedProfileKeys() =0;

        
        /**
         * deleteThisObject
         *
         * Removes this operator from the database. 
         * The calling application MUST then delete this operator object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      This operator has not been deleted
         */
        virtual void deleteThisObject() = 0;


        virtual ~IConfigOperator() {};
    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigOperator_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
