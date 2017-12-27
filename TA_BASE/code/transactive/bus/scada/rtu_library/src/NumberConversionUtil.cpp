/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/NumberConversionUtil.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/21 16:51:05 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  * Class for performing number conversion due to different numbering formats
  *
  */


#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "bus/scada/rtu_library/src/NumberConversionUtil.h"


namespace TA_Base_Bus
{

	const int INT32S_UPPER_LIMIT = INT_MAX;
	const int INT32S_LOWER_LIMIT = INT_MIN + 1;
	const int INT32_UPPER_LIMIT = INT_MAX;
	const int INT32_LOWER_LIMIT = INT_MIN;

	const int INT16S_UPPER_LIMIT = SHRT_MAX;
	const int INT16S_LOWER_LIMIT = SHRT_MIN + 1;
	const int INT16_UPPER_LIMIT = SHRT_MAX;
	const int INT16_LOWER_LIMIT = SHRT_MIN;


	NumberConversionUtil::NumberConversionUtil()
	{

	}


	NumberConversionUtil::~NumberConversionUtil()
	{

	}


	double NumberConversionUtil::convertFromIEEE32_Value ( WORD highOrderNumber, WORD lowOrderNumber )
	{
		unsigned int sign = 0;
		unsigned int exponent = 0;
		double fraction = 0;

		// get the sign bit of the ieee_32 number (bit 31)
		sign = 0x8000 & highOrderNumber;

		// get the exponent part of the ieee_32 number (bits 30 - 23)
		exponent = ( ( highOrderNumber << 1 ) >> 8 );

		// get the fraction part of the ieee_32 number (bits 22 - 0)
		unsigned long temp = 0;
		temp = ( ( ( highOrderNumber << 9 ) >> 9 ) << 16 );
		temp +=  lowOrderNumber;

		// calculate the fractional number
		for ( int index = 0; index <= 22; index++ )
		{
			if ( 1 == ( 0x0001 & ( temp >> index ) ) )
			{
				fraction += pow ( (double)2, index - 23 );
			}
			else
			{
				// do nothing
			}
		}

		//
		// calculating the IEEE_32 floating number
		//

		if ( ( 255 == exponent ) && ( 0 == fraction ) )
		{
			// +/- infinitive
			return 0;
		}
		else if ( ( 255 == exponent ) && ( 0 != fraction ) )
		{
			// overflow
			return 0;
		}
		else if ( ( 0 == exponent ) && ( 0 == fraction ) )
		{
			// zero
			return 0;
		}
		else if ( ( 0 == exponent ) && ( 0 != fraction ) )
		{
			// formulae: -1^(s) x 2^(-126) x 0.f
			return ( pow ( (double)-1, (int)sign ) * pow ( (double)2, -126 ) * fraction );
		}
		else
		{
			// formulae: -1^(s) x 2^(e-127) x 1.f
			return ( pow ( (double)-1, (int)sign ) * pow ( (double)2, (int)(exponent - 127) ) * ( 1 + fraction ) );
		}
	}


	bool NumberConversionUtil::convertToIEEE32_Value ( double originalNumber, double & convertedNumber )
	{
		// to be implemented
		throw;
	}


	double NumberConversionUtil::convertFromINT32S_Value ( WORD highOrderNumber, WORD lowOrderNumber )
	{
		unsigned int mostSignificantBit = 0;
		long actualValue = 0;

		// get the sign bit of the 32 bit number
		mostSignificantBit = highOrderNumber >> 15;

		// calculate the actual number with the most significant bit masked out
		actualValue = ( 0x7FFF & highOrderNumber ) << 16;
		actualValue += lowOrderNumber;

		if ( 1 == mostSignificantBit )
		{
			return actualValue * (-1);
		}

		return actualValue;
	}


	bool NumberConversionUtil::convertToINT32S_Value ( double originalNumber, double & convertedNumber )
	{
		bool result = false;

		// if the original number is within the limit for 32 bit integer with the most
		// significant bit the signed bit
		if ( ( INT32S_LOWER_LIMIT <= originalNumber ) && ( INT32S_UPPER_LIMIT >= originalNumber ) )
		{
			// assign the output number, assuming that the original number is +ve
			convertedNumber = originalNumber;

			// if the original number is -ve
			if ( 0 > originalNumber )
			{
				// turn on the most significant bit of the high order number
				convertedNumber = 0x80000000 | ( abs ( (long) originalNumber ) );
			}

			// set the flag
			result = true;
		}
		// else do nothing

		return result;
	}


	double NumberConversionUtil::convertFromINT32_Value ( WORD highOrderNumber, WORD lowOrderNumber )
	{
		unsigned int mostSignificantBit = 0;
		long thirty2BitNumber = 0;

		// get the sign bit of the 32 bit number
		mostSignificantBit = highOrderNumber >> 15;

		if ( 1 == mostSignificantBit )
		{
			// make 32bit number from the high and low order words
			thirty2BitNumber = highOrderNumber << 16;
			thirty2BitNumber += lowOrderNumber;

			// take ones complement
			thirty2BitNumber = ~thirty2BitNumber;

			// add one
			thirty2BitNumber += 1;

			// add -ve sign
			return ( thirty2BitNumber * (-1) );
		}
		else
		{
			// obtain the whole 32 bit number
			thirty2BitNumber = highOrderNumber << 16;
			thirty2BitNumber += lowOrderNumber;

			return thirty2BitNumber;
		}
	}


	bool NumberConversionUtil::convertToINT32_Value ( double originalNumber, double & convertedNumber )
	{
		bool result = false;
		long thirty2BitNumber = 0;

		// if the original number is within the limit for 32 bit integer with the most
		// significant bit the signed bit
		if ( ( INT32_LOWER_LIMIT <= originalNumber ) && ( INT32_UPPER_LIMIT >= originalNumber ) )
		{
			// assign the output number, assuming that the original number is +ve
			convertedNumber = originalNumber;

			// if the original number is -ve
			if ( 0 > originalNumber )
			{
				// convert the original to +ve
				thirty2BitNumber = abs ( (long) originalNumber );

				// take ones complement
				thirty2BitNumber = ~thirty2BitNumber;

				// add one to become 2's Complement format of the original -ve value
				thirty2BitNumber += 1;

				convertedNumber = thirty2BitNumber;
			}

			// set the flag
			result = true;
		}
		// else do nothing

		return result;
	}


	int NumberConversionUtil::convertFromINT16S_Value ( WORD originalNumber )
	{
		unsigned int mostSignificantBit = 0;
		WORD actualValue = 0;

		// get the sign bit of the 32 bit number
		mostSignificantBit = originalNumber >> 15;

		// calculate the actual number, less the sign bit
		actualValue = ( 0x7FFF & originalNumber );

		if ( 1 == mostSignificantBit )
		{
			actualValue = actualValue * (-1);
		}

		return (int) actualValue;
	}


	bool NumberConversionUtil::convertToINT16S_Value ( int originalNumber, WORD & convertedNumber )
	{
		bool result = false;

		// if the original number is within the limit for 16 bit integer with the most
		// significant bit the signed bit
		if ( ( INT16S_LOWER_LIMIT <= originalNumber ) && ( INT16S_UPPER_LIMIT >= originalNumber ) )
		{
			// assign the output number, assuming that the original number is +ve
			convertedNumber = originalNumber;

			// if the original number is -ve
			if ( 0 > originalNumber )
			{
				// take the absolute number of the original, turn on the most significant bit
				// and assign the output
				convertedNumber = ( 0x8000 | abs( originalNumber ) );
			}

			// set the flag
			result = true;
		}
		// else do nothing

		return result;
	}


	int NumberConversionUtil::convertFromINT16_Value ( WORD originalNumber )
	{
		unsigned int mostSignificantBit = 0;
		unsigned long thirty2BitNumber = 0;
		int converted = 0;

		// get the sign bit of the original number
		mostSignificantBit = originalNumber >> 15;

		if ( 1 == mostSignificantBit )
		{
			// make 32bit number from the high and low order words
			thirty2BitNumber = 0 << 16;
			thirty2BitNumber += originalNumber;

			// take ones complement
			thirty2BitNumber = ~thirty2BitNumber;
			thirty2BitNumber <<= 16;
			thirty2BitNumber >>= 16;

			// add one
			thirty2BitNumber += 1;

			// add -ve sign
			converted = thirty2BitNumber * (-1);
			return converted;
		}
		else
		{
			return originalNumber;
		}
	}


	bool NumberConversionUtil::convertToINT16_Value ( int originalNumber, WORD & convertedNumber )
	{
		bool result = false;

		// if the original number is within the limit for 32 bit integer with the most
		// significant bit the signed bit
		if ( ( INT16_LOWER_LIMIT <= originalNumber ) && ( INT16_UPPER_LIMIT >= originalNumber ) )
		{
			// assign the output number, assuming that the original number is +ve
			convertedNumber = originalNumber;

			// if the original number is -ve
			if ( 0 > originalNumber )
			{
				// one's complement of the absolute original number
				convertedNumber = ~(abs( originalNumber ));

				// add one
				convertedNumber += 1;
			}

			// set the flag
			result = true;
		}
		// else do nothing

		return result;
	}


	double NumberConversionUtil::convertFromUINT32_Value ( WORD highOrderNumber, WORD lowOrderNumber )
	{
		unsigned long value = 0;

		value = highOrderNumber << 16;
		value += lowOrderNumber;

		return value;
	}


	unsigned int NumberConversionUtil::convertFromUINT16_Value ( WORD originalNumber )
	{
		return originalNumber;
	}

}
