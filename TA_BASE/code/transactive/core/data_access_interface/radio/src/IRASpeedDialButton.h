/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRASpeedDialButton.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRASpeedDialButton is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRASpeedDialButton_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRASpeedDialButton_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>


namespace TA_Base_Core
{

    class IRASpeedDialButton 
    {

    public:
        virtual ~IRASpeedDialButton() {};

        virtual unsigned long getKey(bool reloadFlag = true) = 0;

        /**
         * getSpeedDialSetKey
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
        virtual unsigned long getSpeedDialSetKey() = 0;
        virtual IRASpeedDialButton& setSpeedDialSetKey(unsigned long key) = 0;

        /**
         * getButtonPosition
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
        virtual unsigned long getButtonPosition() = 0;
        virtual IRASpeedDialButton& setButtonPosition(unsigned long buttonPosition) = 0;
		
        /**
         * getButtonLabel
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
        virtual std::string getButtonLabel() = 0;
        virtual IRASpeedDialButton& setButtonLabel(const std::string& buttonLabel) = 0;
		

        /**
         * getButtonTSI
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
        virtual std::string getButtonTSI() = 0;
        virtual IRASpeedDialButton& setButtonTSI(const std::string& buttonTSI) = 0;

    };

} //close namespace TA_Base_Core

#endif // !defined(IRASpeedDialButton_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
