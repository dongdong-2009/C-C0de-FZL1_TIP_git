//////////////////////////////////////////////////////////////////////
///		@file		DbDataImpl.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    Implementation of the Class CDbDataImpl. 
///                 
///                 
//////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  
#pragma warning(disable:4996)
#pragma warning(disable:4267)
#endif

#include <cmath>
#include <ctime>
#include <climits>

#include "DbDataImpl.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

NS_BEGIN(TA_AQ_Lib)

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;

CDbDataImpl::CDbDataImpl(const std::vector<std::string>& columnNames,Buffer& data)
{
    LOG0( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl (constructor)");
	// Need to check if the buffer is empty
	if (0 == data.size()) // This is fine...we just set the object to 0 all round.
	{
        LOG0 ( SourceInfo,  DebugUtil::DebugSQL, "CDbDataImpl::CDbDataImpl(): There was no data returned");
		m_numRows = 0;
	}
	else
	{
        LOG1 ( SourceInfo,  DebugUtil::DebugSQL, 
            "CDbDataImpl::CDbDataImpl(): Number of lines of data returned = %lu", data.size());
		// Need to check that each row in the data buffer has the same number of columns as 
		// specified in the columnNames vector		                
		for (std::vector<StringVector>::const_iterator iter = data.begin(); iter != data.end();  ++iter)
		{
			TA_ASSERT((columnNames.size()) == (iter->size()),"Specified number of columns does not match retrieved number of columns");
		}

		for (StrVeccIterT vIter = columnNames.begin(); vIter != columnNames.end();  ++vIter)
		{             
			// It is a pre-condition that all column names must be unique. This assert tests the 
			// pre-condition.
			DbDataIter mIter (m_data.find(*vIter));  

			// Need to test that the begin() and end() of the map are equal here as well, as
			// on the first pass that will actually be true, while mIter != end() will fail
			TA_ASSERT((m_data.begin() == m_data.end()) || 
					  (mIter == m_data.end()),"The column names specified were not unique");

			// Add the column name and a blank vector to the map.
			StringVector str;
			m_data.insert(DbDataValT((*vIter), str) );
		}

		// Loop through the dobule vector "data", which represents the data returned from the database
		// in a vector of rows. We need a map of columns, so we need to pull out the first item from
		// each row, then the second, then the third, etc.
        LOG0 ( SourceInfo,  DebugUtil::DebugDatabase, 
            "CDbDataImpl::CDbDataImpl(): Processing data into correct format");
		for (u_int column = 0;column < data[0].size();column++)
		{

			for (u_int row = 0;row < data.size();row++)
			{
				// find the column name in the map
				DbDataIter mIter (m_data.find(columnNames[column]));
				// add the specific item from the row to the column vector
				mIter->second.push_back(data[row][column]);
			}
		}

		// Set the number of rows (for use with the getNumRows() method)
		m_numRows = data.size();
        LOG0( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl (Constructor)");
	}
}


CDbDataImpl::~CDbDataImpl()
{
	DbDataIter mapIter = m_data.begin();
	while (mapIter != m_data.end())
	{
		mapIter->second.clear();
		
		 mapIter++;
	}
	m_data.clear();
}

unsigned long CDbDataImpl::getNumRows()
{
	// Easy: just return the number of rows
	return m_numRows;    
}

std::string CDbDataImpl::_GetData(const u_long rowIndex,const std::string& fieldName)
{
	TA_ASSERT(rowIndex < m_numRows,"rowIndex is greater than the number of rows");
	// Find the field name in the map
	DbDataIter iter (m_data.find(fieldName));

	TA_ASSERT(iter != m_data.end(),"The column names specified were not found");
	std::string target = iter->second[rowIndex];
    LOG1 ( SourceInfo, DebugUtil::DebugDatabase,
       "The target data is (excluding quotes): \"%s\"", target.c_str());
	return target;
}

int CDbDataImpl::getIntegerData(const u_long rowIndex, const std::string& fieldName, const int defaultValue)
{	
    LOG0( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getIntegerData" );
	std::string target = _GetData(rowIndex, fieldName);
    
    // The third test ensures the returned number is not null
    if (0 == target.size())
    {
        return defaultValue;
    }

	// The following checks need to be performed on the string: 
	// 1) Check there are no characters apart from 0-9, and "-"
	// 2) ensure the "-" (if present) is singular, and only in position 0
	// NB: leading spaces will cause the string to fail, and an exception will be generated
	int validCharacters = target.find_first_not_of("0123456789-");
	int dashLocation = target.find_last_of("-");

	// The first test ensures there are no invalid characters
	// The second test ensures the dash is in either the first or second location
	if ((-1 != validCharacters) || !(dashLocation == 0 || dashLocation == -1) )
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to an int.",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

	// The behaviour of atoi is undefined in the event of overflow, therefore we need to test that
	// the string is not too long. The maximum permitted range for an integer is 
	// from -2147483648 to 2147483647.
	// We can use a string lexicographical compare to determine if the retrieved integer is 
	// useable.
	
	static char tempBuffer[256];
	sprintf(tempBuffer,"%ld",INT_MAX);
	static const std::string maxInt(tempBuffer);
	sprintf(tempBuffer,"%ld",INT_MIN);
	static const std::string minInt(tempBuffer);
	
	//static char tempBuffer[256];
	//static const std::string maxInt(ltoa(INT_MAX,tempBuffer,10));
	//static const std::string minInt(ltoa(INT_MIN,tempBuffer,10));

	if ((dashLocation == 0 && target.size() >= minInt.size()) || 
		(dashLocation == -1 && target.size() >= maxInt.size())) // this could be a problem
	{
		int numberComparison = 0;
		if (target.size() == maxInt.size())
		{
			numberComparison = target.compare(maxInt);	// will return a negative number if
														// target < maxInt
		}
		else if (target.size() == minInt.size() && dashLocation == 0)
		{
			numberComparison = target.compare(minInt);	// will return a negative number if 
														// target > minInt
		}
		else
		{
			numberComparison = 1;	// set this to 1, as the number is too big, and doesn't need
									// to be compared
		}

		// final comparison
		if (numberComparison > 0)
		{
			char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" is a number too large to be an int",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
		}
	}

	// atoi will return 0 both when the string is 0, and when there's an error.
	// However, as we have already tested above, and thrown an exception if there 
	// are any non-numeric strings we can assume this will succeed.
	int value = atoi(target.c_str()); 
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getIntegerData" );
	return value;
}


unsigned long CDbDataImpl::getUnsignedLongData(const u_long rowIndex, const std::string& fieldName, const u_long defaultValue)
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getUnsignedLongData" );
	std::string target = _GetData(rowIndex,fieldName);

	// The second test ensures the returned number is not null
    if (0 == target.size())
    {
        return defaultValue;
    }
    
	// The following checks need to be performed on the string: 
	// 1) Check there are no characters apart from 0-9
	// NB: leading spaces will cause the string to fail, and an exception will be generated
	int validCharacters = target.find_first_not_of("0123456789");

	// The first test ensures there are no invalid characters
    if (-1 != validCharacters)
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to an unsigned long (got '%s').",
				rowIndex, fieldName.c_str(), target.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

	// The behaviour of atol is undefined in the event of overflow, therefore we need to test that
	// the string is not too long. The maximum permitted range for an integer is 
	// from 0 to 4294967295.
	// We can use a string lexicographical compare to determine if the retrieved integer is 
	// useable.
	
	static char tempBuffer[256];
	sprintf(tempBuffer,"%lu",ULONG_MAX);
	static const std::string maxul(tempBuffer);


	if (target.size() >= maxul.size()) // this could be a problem
	{
		int numberComparison = 0;
		if (target.size() == maxul.size())
		{
			numberComparison = target.compare(maxul);	// will return a negative number if
														// target < maxInt
		}
		else
		{
			numberComparison = 1;	// set this to 1, as the number is too big, and doesn't need
									// to be compared
		}

		// final comparison
		if (numberComparison > 0)
		{
			char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" is a number to large to be an unsigned long.",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
		}
	}

	// atol will return 0 both when the string is 0, and when there's an error.
	// However, as we have already tested above, and thrown an exception if there 
	// are any non-numeric characters in the string we can assume this will succeed.
	u_long value = atol(target.c_str());	
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getUnsignedLongData" );	
	return value;
}


bool CDbDataImpl::getBooleanData(const u_long rowIndex,const std::string& fieldName, const bool defaultValue)
{	
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getBooleanData" );
	std::string target = _GetData(rowIndex, fieldName);

    if ( target.empty() )
    {
        return defaultValue;
    }

	// The following checks need to be performed on the string: 
	// 1) Check there are no characters apart from 0-9, and "-"
	// 2) ensure the "-" (if present) is singular, and only in position 0
	// NB: leading spaces will cause the string to fail, and an exception will be generated
	int validCharacters = target.find_first_not_of("0123456789");

	// The first test ensures there are no invalid characters
	// The second test ensures the dash is in either the first or second location
	// The third test ensures the returned number is not null
	if ( DEF_RET_FAILED != validCharacters )
	{
        std::ostringstream reasonMessage;
        reasonMessage << "The data at row " << rowIndex << " in column \"" << fieldName.c_str();
        reasonMessage << "\" could not be converted from a string to a boolean.";
		TA_THROW(DataException(reasonMessage.str().c_str(),DataException::WRONG_TYPE,target));
	}

    if ( 0 == target.compare("0") )
    {
        LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getBooleanData" );
        return false;
    }

    // Otherwise we have a true value
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getBooleanData" );
    return true;
}


std::string CDbDataImpl::getStringData(const unsigned long rowIndex, const std::string& fieldName, const std::string defaultValue)
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getStringData" );
	std::string target = _GetData(rowIndex, fieldName);

    if ( target.empty() )
    {
        return defaultValue;
    }

    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getStringData" );
	return target;
}


// YYYYMMDDHH24MMSS
// Year		= string(00-03)
// Month	= string(04-05)
// Day		= string(06-07)
// Hour		= string(08-09)
// Minute	= string(10-11)
// Second	= string(12-13)
// Total length = 14 characters
time_t CDbDataImpl::getDateData(const unsigned long rowIndex, const std::string& fieldName, const time_t defaultValue )
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getDateData" );
	std::string target = _GetData(rowIndex,fieldName);

    if (0 == target.size())
    {
        return defaultValue;
    }
	//for the time zone issue
	if  ( isInitialDate( target.c_str() ) )
	{
		return (time_t)0;
	}

    // The date string should have no non-numeric characters. This test will determine that.
    int numberTest = target.find_first_not_of("0123456789");
	// The target string MUST bet 14 characters long, and 14 characters long ONLY
	// If the correct database select has been used this will be guaranteed.
	// We have not used an assert here as it could be possible to load the data with a valid 
	// string
	// and get something invalid
	if ((14 != target.size()) || (DEF_RET_FAILED != numberTest))
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The entry at row %lu in column \"%s\" is empty",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,target));
	}
	std::string strYear = target.substr(0,4);
	std::string strMonth = target.substr(4,2);
	std::string strDay = target.substr(6,2);
	std::string strHour = target.substr(8,2);
	std::string strMinute = target.substr(10,2);
	std::string strSecond = target.substr(12,2);

	int iYear = atoi(strYear.c_str());
	int iMonth = atoi(strMonth.c_str());
	int iDay = atoi(strDay.c_str());
	int iHour = atoi(strHour.c_str());
	int iMinute = atoi(strMinute.c_str());
	int iSecond = atoi(strSecond.c_str());

	// Need to sanity check the date data, as mktime won't do it for us
	// NB: It is not checked that the day is valid for the month
	bool dateSanityError = false;
	dateSanityError = (iSecond > 59 || iMinute > 59 || iHour > 24);
	dateSanityError = (dateSanityError || iDay > 31 && iDay != 0);
	dateSanityError = (dateSanityError || iMonth > 12 || iMonth == 0);
	dateSanityError = (dateSanityError || iYear > 2038 || iYear < 1900);
	if (dateSanityError)
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains invalid date/time specifications.",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

	// now need to check that the day is valid based on the month/year
	if (iMonth == APRIL || iMonth == JUNE || iMonth == SEPTEMBER || iMonth == NOVEMBER) // 30 days
	{
		if (iDay == 31)
		{
			char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
		}
	}
	else if (iMonth == FEBRUARY) // more processing necessary
	{		
		if (0 == (iYear%4) && (0 != (iYear%100) || 0 == (iYear%400))) // This is a leap year
		{
			if (iDay > 29)
			{
				char reasonMessage[256];
				sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
						rowIndex,fieldName.c_str());			
				TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
			}
		}
		else // not a leap year
		{
			if (iDay > 28)
			{
				char reasonMessage[256];
				sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
						rowIndex,fieldName.c_str());			
				TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
			}
		}
	}
	// else - don't need to check for 31 days, as we did that above

	struct tm newtm;

	newtm.tm_sec = iSecond;		//seconds
	newtm.tm_min = iMinute;		// hours 
	newtm.tm_hour = iHour;		// minutes 
	newtm.tm_mday = iDay;		// day of the month 
	newtm.tm_mon = iMonth-1;	// month of Year - goes from 0 to 11
	newtm.tm_year = iYear-1900;	// years since 1900 - this structure only deals with years 
								// between 1901 and 2038
	newtm.tm_isdst = -1;		// disable daylight savings - this could have implications

	time_t value;
	value = mktime(&newtm);
	
	// check that the time conversion worked successfully
	if (-1 == value) // conversion failed
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

    LOG0( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getDateData" );
	return value;
}

// YYYYMMDDHH24MMSSFF3
// Year		    = string(00-03)
// Month	    = string(04-05)
// Day		    = string(06-07)
// Hour		    = string(08-09)
// Minute	    = string(10-11)
// Second	    = string(12-13)
// Millisecond   = string(14-16)
// Total length = 17 characters
timeb CDbDataImpl::getTimestampData(const unsigned long rowIndex, const std::string& fieldName)
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getTimestampData" );
	std::string target = _GetData(rowIndex,fieldName);
    
    // The date string should have no non-numeric characters. This test will determine that.
	int numberTest = target.find_first_not_of("0123456789");
	// The target string MUST be 17 characters long, and 17 characters long ONLY
	// If the correct database select has been used this will be guaranteed.
	// We have not used an assert here as it could be possible to load the data with a valid 
	// string
	// and get something invalid
//	if ((17 != target.size()) || (-1 != numberTest))
	if ((target.size() < 14) || (-1 != numberTest))
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The entry at row %lu in column \"%s\" is empty, or contains invalid characters",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,target));
	}
	std::string strYear = target.substr(0,4);
	std::string strMonth = target.substr(4,2);
	std::string strDay = target.substr(6,2);
	std::string strHour = target.substr(8,2);
	std::string strMinute = target.substr(10,2);
	std::string strSecond = target.substr(12,2);
    std::string strMillisecond = target.substr(14,3);

	int iYear = atoi(strYear.c_str());
	int iMonth = atoi(strMonth.c_str());
	int iDay = atoi(strDay.c_str());
	int iHour = atoi(strHour.c_str());
	int iMinute = atoi(strMinute.c_str());
	int iSecond = atoi(strSecond.c_str());
    int iMillisecond = atoi(strMillisecond.c_str());

	// Need to sanity check the date data, as mktime won't do it for us
	// NB: It is not checked that the day is valid for the month
	bool dateSanityError = false;
	dateSanityError = (iMillisecond > 999 || iSecond > 59 || iMinute > 59 || iHour > 24);
	dateSanityError = (dateSanityError || iDay > 31 && iDay != 0);
	dateSanityError = (dateSanityError || iMonth > 12 || iMonth == 0);
	dateSanityError = (dateSanityError || iYear > 2038 || iYear < 1900);
	if (dateSanityError)
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains invalid date/time specifications.",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

	// now need to check that the day is valid based on the month/year
	if (iMonth == APRIL || iMonth == JUNE || iMonth == SEPTEMBER || iMonth == NOVEMBER) // 30 days
	{
		if (iDay == 31)
		{
			char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
		}
	}
	else if (iMonth == FEBRUARY) // more processing necessary
	{		
		if (0 == (iYear%4) && (0 != (iYear%100) || 0 == (iYear%400))) // This is a leap year
		{
			if (iDay > 29)
			{
				char reasonMessage[256];
				sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
						rowIndex,fieldName.c_str());			
				TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
			}
		}
		else // not a leap year
		{
			if (iDay > 28)
			{
				char reasonMessage[256];
				sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t, as it contains an invalid day for the given month.",
						rowIndex,fieldName.c_str());			
				TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
			}
		}
	}

	struct tm newtm;

	newtm.tm_sec = iSecond;		//seconds
	newtm.tm_min = iMinute;		// hours 
	newtm.tm_hour = iHour;		// minutes 
	newtm.tm_mday = iDay;		// day of the month 
	newtm.tm_mon = iMonth-1;	// month of Year - goes from 0 to 11
	newtm.tm_year = iYear-1900;	// years since 1900 - this structure only deals with years 
								// between 1901 and 2038
	newtm.tm_isdst = -1;		// setting this to -1 lets the function set the DST based on system settings

	time_t timeData;
	timeData = mktime(&newtm);
	
	// check that the time conversion worked successfully
	if (-1 == timeData) // conversion failed
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a time_t",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}

    // And now place the converted time into a timeb structure.
    timeb value;
    value.time = timeData;
    value.millitm = iMillisecond;
    value.timezone = -1;    // Set timezone and dstflag to -1, which means "unspecified"
    value.dstflag = -1;

    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getDateData" );
	return value;
}

double CDbDataImpl::getRealData(const unsigned long rowIndex, const std::string& fieldName, const double defaultValue)
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getRealData" );
	std::string target = _GetData(rowIndex, fieldName);

	if (target.size() == 0) // the data string is empty	
	{
        return defaultValue;
	}	

	//fixed by jianghp while integrating OCL to Transactive. since the table will define number(9,0) field. so weird.
	if (target.size() > 0) //if number are "1." or format as "-1.", an 0 will be appended to it.
	{
		if (target[target.size() - 1] == '.')
		{
			target += '0';
		}
	}

	// For reals need to check that the number is formatted correctly -- that is, there
	// is a maximum of 1 ".", 1 "-", and that at least one numerical character trails either
	// of the other two
	//		Fix for TD3326
    // If there is a problem with atof handling values like "-.123" or ".123"
    // then use the code below to insert a leading zero in-front of the ".".
    // If atof has no problems with these values then line /**1**/ below should
    // be replace with line /**2**/ and the block starting at line /**3**/ s
    // should be commented out, and the code below is then not required
    if ('.' == target.c_str()[0])
    {
        target.insert(0, "0");
    }
    else if ('-' == target.c_str()[0] && '.' == target.c_str()[1])
    {
        target.insert(1, "0");
    }
    // PDV: end change

	int validCharacters = target.find_first_not_of("0123456789-.");				// locates any invalid characters in this string
	int firstDash = target.find_first_of("-");									// finds the location of the first "-"
	int otherDash = target.find_first_of("-",firstDash+1);						// determines if there are any subsequent "-"s
	int trailingDash = target.find_first_of("0123456789",firstDash+1);			// finds the location of the first character that is a number following the dash - this should be firstDash+1
	int firstPoint = target.find_first_of(".");									// finds the location of the first "."
	int otherPoint = target.find_first_of(".",firstPoint+1);					// determines if there are any subsequent "."s
	int trailingPoint = target.find_first_of("0123456789",firstPoint+1);	// finds the locatino of the first -numerical character after the first point - this should return firstPoint+1

	if (-1 != validCharacters) // there is an invalid character
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as in contains invalid characters.",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (firstDash != otherDash && -1 != otherDash) // there is more than one dash
	{
		char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as there is more than one \"-\".",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (-1 != firstDash && 0 != firstDash) // the only dash is not in the first location
	{
		char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as the \"-\" is not in the first position.",
					rowIndex,fieldName.c_str());			
			TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (-1 != firstDash && (firstDash+1) != trailingDash) // there is not a numerical character immeidately trailing the dash
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as there is not a numeric character trailing the \"-\".",
				rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (firstPoint != otherPoint && -1 != otherPoint) // there is more than one point
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as there is more than one \".\".",
					rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (-1 != firstPoint && (firstPoint+1) != trailingPoint) // there is not a numerical character immeidately trailing the point
	{
		char reasonMessage[256];
			sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as there is not a numeric character trailing the \".\".",
					rowIndex,fieldName.c_str());			
				TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}
	else if (-1 != firstPoint && ((0 == firstPoint) || (-1 != firstDash && 1 == firstPoint))) // the decimal point is in an ivalid location (either the first position, or the second if there is a dash)
	{
		char reasonMessage[256];
		sprintf(reasonMessage, "The data at row %lu in column \"%s\" could not be converted from a string to a double, as the \".\" is in the wrong position.",
					rowIndex,fieldName.c_str());			
		TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,target));
	}	

	double value = atof(target.c_str());
	// atof will return 0 both when the string is 0, and when there's an error,
	// however, due to the checks above, there should be no error here
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::getRealData" );
	return value;
}


bool CDbDataImpl::isNull(const unsigned long rowIndex, const std::string& fieldName)
{
    LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::isNull" );
	std::string target = _GetData(rowIndex, fieldName);

	// NULL values and empty strings are treated as the same thing.
    
	LOG0 ( SourceInfo, DebugUtil::DebugTrace, "CDbDataImpl::isNull" );
	return (target.empty());
}

time_t CDbDataImpl::getNullTime()
{
	struct tm newtm;
	newtm.tm_sec = 0;		//seconds
	newtm.tm_min = 0;		// minutes 
	newtm.tm_hour = 12;		// hours 
	newtm.tm_mday = 1;		// day of the month 
	newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
	newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
								// between 1901 and 2038
	newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

	return mktime(&newtm);
}
//xinsong ++
bool CDbDataImpl::isInitialDate( const std::string & dataStr )
{
	const char * wholeStr = dataStr.c_str();

	if ( strstr(wholeStr, "19700101") == wholeStr )
	{
		return true;
	}
	if ( strstr(wholeStr, "19691231") == wholeStr )
	{
		return true;
	}
	if ( strstr(wholeStr, "19700102") == wholeStr )
	{
		return true;
	}
	return false;
	
}

NS_END(TA_AQ_Lib)
