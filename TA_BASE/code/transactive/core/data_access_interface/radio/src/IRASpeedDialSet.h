/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRASpeedDialSet.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRASpeedDialSet is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRASpeedDialSet_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRASpeedDialSet_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Core
{
	class IProfile;

    class IRASpeedDialSet 
    {

    public:
        virtual ~IRASpeedDialSet() {};


        virtual unsigned long getKey(bool reloadFlag = true) = 0;

        /**
         * isSystem
         *
         * Returns the XXX.
         *
         * @return The as XXX a bool
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isSystem() = 0;
        virtual IRASpeedDialSet& setSystem(bool isSystem) = 0;

        /**
         * getOperatorKey
         *
         * Returns the XXX.
         *
         * @return The as XXX a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual unsigned long getOperatorKey() = 0;
        virtual IRASpeedDialSet& setOperatorKey(unsigned long operatorKey) = 0;
		
        /**
         * getProfileId
         *
         * Returns the XXX.
         *
         * @return The as XXX a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getProfileId() = 0;
        virtual IRASpeedDialSet& setProfileId(unsigned long profileId) = 0;
		

        /**
         * getSetName
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
        virtual std::string getSetName() = 0;
        virtual IRASpeedDialSet& setSetName(const std::string& setName) = 0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IRASpeedDialSet_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
