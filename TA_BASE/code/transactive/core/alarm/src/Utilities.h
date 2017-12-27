/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/alarm/src/Utilities.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/12/29 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains some utility methods for converting Alarm state types to string types.
  */

#ifndef __ALARM_UTILITIES_HEADER__
#define __ALARM_UTILITIES_HEADER__

#include <string>
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class Utilities 
	{
		public:

            /**
		      * getInstance
		      * 
		      * Returns a single static instance of Utilities
		      *
		      * @return The one and only Utilities instance
		      */
		    static Utilities& getInstance();

            /**
		      * ~Utilities
		      * 
		      * Destructor
		      */
		    virtual ~Utilities() {}

            /**
             * convertDSS2String
             *
             * convert the DSS type to string.
             *
             * @param TA_Base_Core::DecisionSupportStateType - DSS type to convert
             *
             * @return the string equivalent to DDS type.
             */
			std::string convertDSS2String(const TA_Base_Core::DecisionSupportStateType& dssType);
			
			/**
             * convertString2DDS
             *
             * convert the string to DSS type.
             *
             * @param std::string& strDDS - string to convert
             *
             * @return TA_Base_Core::DecisionSupportStateType.
             */
			TA_Base_Core::DecisionSupportStateType convertString2DDS(const std::string& strDDS);

            /**
             * convertMMS2String
             *
             * convert the MMS type to string.
             *
             * @param TA_Base_Core::MmsStateType - MMS type to convert
             *
             * @return The string equivalent to MMS type.
             */
			std::string convertMMS2String(const TA_Base_Core::MmsStateType& mmsType);

			/**
             * convertString2MMS
             *
             * convert the string to MMS type.
             *
             * @param std::string& strMMS - string to convert
             *
             * @return MMS type.
             */
			TA_Base_Core::MmsStateType convertString2MMS(const std::string& strMMS);

            /**
             * convertAvalanche2String
             *
             * converts the Avalanche as a string that is displayed in the Alarm List Control.
             *
             * @param bool - determine if head avalanche
			 *
			 * @param bool - determine if child avalanche
             *
             * @return The text to display in the column for the specified alarm.
             */
            std::string convertAvalanche2String(const bool& bIsHead, const bool& bIsChild);

			
            /**
             * convertString2Avalanche
             *
             * converts the string to Avalanche state of the alarm.
			 *
			 * @param std::string - string to convert
             *
             * @param bool - determine if head avalanche
			 *
			 * @param bool - determine if child avalanche
             *
             * @return none.
             */
			void convertString2Avalanche(const std::string& strAvl, bool& bIsHead, bool& bIsChild);

        private:

            Utilities() {}
            Utilities& operator=(const Utilities& rhs);
            Utilities(const Utilities&);

            // the one and only class instance
            static Utilities* s_utilities;

            // protect singleton creation
            static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
	};
};
#endif // !defined(__ALARM_UTILITIES_HEADER__)
