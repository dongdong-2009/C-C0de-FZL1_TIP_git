/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(DataConversion_H)
#define DataConversion_H

#include <set>
#include <string>
#include <stdio.h>
#include <sstream>

namespace TA_Base_Core
{
	class DataConversion
	{

	public:

		/**
		 * Given a string, convert the data it contains to a boolean.
		 * 
		 * @return true or false, depending on the string contents.
		 * 
		 * @exception DataException if the conversion can not be done
		 */
		static bool toBoolean(const std::string& stringValue, const std::string& customTrueLabel = "", const std::string& customFalseLabel = "");


		/**
		 * Given a boolean, convert the data it contains to a string
		 * 
		 * @return the string representation of the boolean
		 */
		static std::string toString(const bool booleanValue, const std::string& trueLabel = "1", const std::string& falseLabel = "0");


	   /**
		 * Given a string, convert the data it contains to an unsigned char.
		 * 
		 * @return true or false, depending on the string contents.
		 * 
		 * @exception DataException if the conversion can not be done
		 */
		static unsigned char  toUnsignedChar(const std::string& stringValue);


		/**
		 * Given an unsigned char, convert the data it contains to a string
		 * 
		 * @return the string representation of the unsigned char
		 */
		static std::string toString(const unsigned char unsCharValue);



		/**
		 * Given a string, convert the data it contains into an integer.
		 * 
		 * @return the integer value
		 * 
		 * @exception DataException if the conversion can not be done
		 */
		static long toInteger(const std::string& value);


		/**
		 * Given a long, convert the data it contains into a string
		 * 
		 * @return the string value
		 */
		static std::string toString(long value);


		/**
		 * Given a string, convert the data it contains into an unsigned integer.
		 * 
		 * @return the unsigned integer value
		 * 
		 * @exception DataException if the conversion can not be done
		 */
		static unsigned long toUnsignedInteger(const std::string& value);


		/**
		 * Given a an unsigned long, convert the data it contains into a string
		 * 
		 * @return the string value
		 */
		static std::string toString(unsigned long value);


		/**
		 * Given a string, convert the data it contains into a double
		 * 
		 * @return the real value
		 * 
		 * @exception DataException if the conversion can not be done
		 */
		static double toDouble(const std::string& value, int precision = 2);


		/**
		 * Given a double, convert the data it contains into a string
		 * 
		 * @return the string value
		 */
		static std::string toString(double value, int precision = 2);

	private:
            
        DataConversion();
		virtual ~DataConversion();


		/**
		 * Possible boolean true labels
		 */
		static const std::set<std::string> s_trueLabels;


		/**
		 * Possible boolean false labels
		 */
		static const std::set<std::string> s_falseLabels;

	};
}
#endif // !defined(DataConversion_H)
