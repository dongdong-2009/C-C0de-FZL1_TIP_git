/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRAPatchMembers.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRAPatchMembers is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRAPatchMembers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRAPatchMembers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#error "Not implemented"

namespace TA_Base_Core
{
	class IProfile;

    class IRAPatchMembers 
    {
    public:
#if 0 // enum defined in IRASubscribers.h
        enum SubscriberType    
        {
            INVALID     = 0,

            DISPATCHER  = 'D',
            GROUP       = 'G',
            MOBILE      = 'M',
            PATCH_GROUP = 'P',
            INDIVIDUAL  = 'I' 
        };
#endif

    public:
        virtual ~IRAPatchMembers() {};


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
        virtual std::string getSubscriberName() =0;


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
        virtual unsigned long getLocationKey() =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IRAPatchMembers_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
