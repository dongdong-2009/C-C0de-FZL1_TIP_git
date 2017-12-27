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

#include <sstream>
#include "core/utilities/src/DataConversion.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    const unsigned char numberofString = 7;//Number of string

	//Set the true label
	const std::string stringTrueLabel[numberofString] = {	std::string("1"),
															std::string( "true"), 
															std::string("True"), 
															std::string("TRUE"), 
															std::string("Yes"),
															std::string("YES"),
															std::string("yes")
														};
    const std::set<std::string> DataConversion::s_trueLabels( stringTrueLabel,
															  stringTrueLabel+numberofString);


	//Set the false label
	const std::string stringFalseLabel[numberofString]= {	std::string("0"),
															std::string( "false"), 
															std::string("False"), 
															std::string("FALSE"), 
															std::string("No"),
															std::string("NO"),
															std::string("no"),
														};
    const std::set<std::string> DataConversion::s_falseLabels( stringFalseLabel,
															   stringFalseLabel+numberofString);	
    
	DataConversion::DataConversion()
	{
		FUNCTION_ENTRY("DataConversion::DataConversion");
		FUNCTION_EXIT;
	}



	DataConversion::~DataConversion()
	{
        FUNCTION_ENTRY("DataConversion::~DataConversion");
		FUNCTION_EXIT;        
	}


	bool DataConversion::toBoolean(const std::string& stringValue, const std::string& customTrueLabel, const std::string& customFalseLabel)
	{
		FUNCTION_ENTRY("DataConversion::toBoolean");
    
		//if the string is empty, return false
		if ( true == stringValue.empty() )
		{
			FUNCTION_EXIT;
			return false;
		}

		// if the true label is specified,
        // compare the string to the true label,
        // if it matches, return true
		if ( stringValue == customTrueLabel )
		{
			FUNCTION_EXIT;
			return true;
		}
        
		// if the false label is specified,
		// compare the string to the false label,
		// if it matches, return false
		if ( stringValue == customFalseLabel )
		{
			FUNCTION_EXIT;
			return false;
		}
                
		// if the string matches any of the labels
		// in s_trueLabels return true
	    if ( s_trueLabels.count( stringValue ) > 0 )
		{
			FUNCTION_EXIT;
			return true;
		}

		// if the string matches any of the labels
		// in s_false labels return false
	    if ( s_falseLabels.count( stringValue ) > 0 )
		{
			FUNCTION_EXIT;
			return false;
		}	

		FUNCTION_EXIT;
		return false;
	}


	std::string DataConversion::toString(const bool booleanValue, const std::string& trueLabel, const std::string& falseLabel)
	{
		FUNCTION_ENTRY("DataConversion::toString");

		FUNCTION_EXIT;
		// for true, return the trueLabel
        if ( true == booleanValue )
		{
			return trueLabel;
		}
        // for false, return the falseLabel
		else
		{
			return falseLabel;
		}
	}


	unsigned char DataConversion::toUnsignedChar(const std::string& value)
	{
		FUNCTION_ENTRY("DataConversion::toUnsignedChar");
    
		// insert the string into a stringstream,
		// extract to a UNSIGNED CHAR
		std::istringstream strStream( value );

		unsigned char dataReturned = 0;
        strStream >> dataReturned;
        
		FUNCTION_EXIT;
		return dataReturned;
	}


	std::string	DataConversion::toString(const unsigned char value)
	{
		FUNCTION_ENTRY("DataConversion::toString");

		// insert the value into a stringstream,
		// extract to a string
		std::ostringstream strStream;
		strStream << value;

		FUNCTION_EXIT;
		return strStream.str();
	}


	long DataConversion::toInteger(const std::string& value)
	{
		FUNCTION_ENTRY("DataConversion::toUnsignedInteger");
    
		// insert the string into a stringstream,
		// extract to a long
		std::istringstream strStream( value );

		long dataReturned = 0;
		strStream >> dataReturned;
		
		FUNCTION_EXIT;
		return dataReturned;
	}


	std::string DataConversion::toString(long value)
	{
		FUNCTION_ENTRY("DataConversion::toString");

		// insert the value into a stringstream,
		// extract to a string
		std::ostringstream strStream;
		strStream << value;

		FUNCTION_EXIT;
		return strStream.str();
	}


	unsigned long DataConversion::toUnsignedInteger(const std::string& value)
	{
		FUNCTION_ENTRY("DataConversion::toUnsignedInteger");
    
		//insert the string into a stringstream,
		//extract to an unsigned long
		std::istringstream strStream( value );

		unsigned long dataReturned = 0;
		strStream >> dataReturned;
		
		FUNCTION_EXIT;
		return dataReturned;
	}


	std::string DataConversion::toString(unsigned long value)
	{
		FUNCTION_ENTRY("DataConversion::toString");

		// insert the value into a stringstream,
		// extract to a string
		std::ostringstream strStream;
		strStream << value;
		
		FUNCTION_EXIT;
		return strStream.str();
	}


	double DataConversion::toDouble(const std::string& value, int precision)
	{
		FUNCTION_ENTRY("DataConversion::toDouble");
		// insert the string into a stringstream,
		// extract to a double using the given precision
		std::stringstream strStream;
		
		strStream.precision( precision );
		strStream << value;

		double dataReturned = 0.0;
		strStream >> dataReturned;
				
		FUNCTION_EXIT;
		return dataReturned;
	}


	std::string DataConversion::toString(double value, int precision)
	{
		FUNCTION_ENTRY("DataConversion::toString");
		// insert the value into a stringstream
		// with the given precision, extract to a string
		std::stringstream strStream;

		// set precision
		strStream.precision( precision );
		strStream << value;

		FUNCTION_EXIT;
		return strStream.str();
	}

}