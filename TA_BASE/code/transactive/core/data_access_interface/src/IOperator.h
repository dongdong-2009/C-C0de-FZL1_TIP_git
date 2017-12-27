/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IOperator.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IOperator is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IOperator_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IOperator_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
	class IProfile;

    class IOperator : public virtual IItem
    {

    public:

        /**
         * getDescription
         *
         * Returns the description for this Operator.
         *
         * @return The description for this Operator as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription() =0;


        /**
         * getPassword
         *
         * Returns the password for this Operator
         *
         * @return The password for the Operator as a string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual std::string getPassword() = 0;
		
        /**
         * setPassword
         *
         * Sets the password for this Operator. This updates the database straight away.
         *
         * @param password The password to give this Operator.
         *
         * pre: This operator has not been deleted
         */
        virtual void setPassword( const std::string password ) = 0;
		

        /**
         * isSystemOperator
         *
         * This indicates if this operator is a system operator or not
         *
         * @return bool - True if this operator is a system operator, false otherwise
         */
        virtual bool isSystemOperator() =0;


        /**
         * getAssociatedProfiles
         *
         * Returns the profiles that this operator can use
         *
         * @return The profiles accessible by this operator, as a vector of IProfile*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<IProfile*> getAssociatedProfiles() = 0;


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
		virtual std::vector<unsigned long> getAssociatedProfileKeys() = 0;


		/**
         * getAssociatedRegionsKeys
         *
         * Returns the profiles that this operator can use
         *
         * @return The profiles accessible by this operator, as a vector of pkeys.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAssociatedRegionKeys() = 0;

        virtual ~IOperator() {};
    };

    typedef boost::shared_ptr<IOperator> IOperatorPtr;
} //close namespace TA_Base_Core

#endif // !defined(IOperator_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
