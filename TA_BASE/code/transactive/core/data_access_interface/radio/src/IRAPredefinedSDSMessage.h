/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRAPredefinedSDSMessage.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRAPredefinedSDSMessage is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRAPredefinedSDSMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRAPredefinedSDSMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>


namespace TA_Base_Core
{
	class IProfile;

    class IRAPredefinedSDSMessage 
    {

    public:
        virtual ~IRAPredefinedSDSMessage() {};


        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual std::string getShortName(bool reloadFlag = true) = 0; 

        /**
         *
         * Sets the XXX.
         * Makes use of Named Parameter Idiom to allow object.setA().setB().setC() style.
         */        
        virtual IRAPredefinedSDSMessage& setShortName(const std::string& shortName) = 0; 

        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getMessage() = 0;
        virtual IRAPredefinedSDSMessage& setMessage(const std::string& message) = 0;

        /**
         * invalidate
         *
         * Make the data contained by this RAPredefinedSDSMessage as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0; // IItem

        virtual void applyChanges() = 0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IRAPredefinedSDSMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
