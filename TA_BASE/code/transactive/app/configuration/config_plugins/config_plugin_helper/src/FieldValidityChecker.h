/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to check the validity of certain fields.
  */

#if !defined(FieldValidityChecker_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define FieldValidityChecker_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <string>

namespace TA_Base_App
{
    class FieldValidityChecker
    {

    public:

        /**
         * Constructor
         */
        FieldValidityChecker(){};


        /**
         * Destructor
         */
        virtual ~FieldValidityChecker(){};


        /**
         * isValidRealNumber
         *
         * This checks if the number string passed in contains a valid real number.
         *
         * @param string - A string to check if it contains a number
         *
         * @return bool - true if the string contains a valid number, false otherwise.
         */
        bool isValidRealNumber(const std::string& numberString) const;


        /**
         * isValidInteger
         *
         * This checks if the number string passed in contains a valid integer number.
         *
         * @param string - A string to check if it contains a number
         *
         * @return bool - true if the string contains a valid number, false otherwise.
         */
        bool isValidInteger(const std::string& numberString) const;


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        FieldValidityChecker( const FieldValidityChecker& theFieldValidityChecker){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        FieldValidityChecker& operator=(const FieldValidityChecker& pm);
    };
}

#endif // !defined(FieldValidityChecker_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
