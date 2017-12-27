/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRASubscribers.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRASubscribers is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRASubscribers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRASubscribers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>


namespace TA_Base_Core
{
	class IProfile;

    class IRASubscribers 
    {
    public:
        enum SubscriberType    
        {
            INVALID     = 0,

            DISPATCHER  = 'D',
            GROUP       = 'G',
            MOBILE      = 'M',
            PATCH_GROUP = 'P',
            INDIVIDUAL  = 'I' // temporarily added 
        };

    class compareByTSI;

    public:
        virtual ~IRASubscribers() {};


        /**
         * getTSI
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
        virtual std::string getTSI(bool reloadFlag = true) = 0; 
        /**
         * getTSI
         *
         * Sets the XXX.
         * Makes use of Named Parameter Idiom to allow object.setA().setB().setC() style.
         */        
        virtual IRASubscribers& setTSI(const std::string& TSI) = 0; 

        /**
         * getSubscriberName
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
        virtual std::string getSubscriberName() = 0;
        virtual IRASubscribers& setSubscriberName(const std::string& name) = 0;


        /**
         * getLocation
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
        virtual std::string getLocation() = 0;
        virtual IRASubscribers& setLocation(const std::string& location) = 0;
		
        /**
         * getSubscriberType
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
        virtual SubscriberType getSubscriberType() = 0;
        virtual IRASubscribers& setSubscriberType(const SubscriberType& type) = 0;
		

        /**
         * getLocationKey
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
        virtual unsigned long getLocationKey() = 0;
        virtual IRASubscribers& setLocationKey(const unsigned long& locationKey) = 0;


        /**
         * invalidate
         *
         * Make the data contained by this RASubscribers as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0; // IItem

        virtual void applyChanges() = 0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IRASubscribers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
