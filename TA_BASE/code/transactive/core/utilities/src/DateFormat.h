/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DateFormat.h $
  * @author:   Azenith Arcilla
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This file provides a standard format for date and time
  */

#if !defined(AFX_DATEFORMAT_H__47B17328_1EA5_45D6_A0F3_41F4A3D1A9A2__INCLUDED_)
#define AFX_DATEFORMAT_H__47B17328_1EA5_45D6_A0F3_41F4A3D1A9A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include <string>

//class DateFormat  
//{
//public:
//	DateFormat();
//	virtual ~DateFormat();
//
//};


namespace TA_Base_Core
{
	class DateFormat  
	{
	public:
		
        enum EDateFormatKey
        {
			DateFormat1,
			DateFormat1a,
			DateFormat1b,	// TD16458
			DateFormat2,
			DateFormat2a,  // mysql
			DateFormat3,
			DateFormat4,
			DateFormat5,
			DateFormat6,
			
			DateTimeFormat1,
			DateTimeFormat2,
			DateTimeFormat3,
			DateTimeFormat4,
			DateTimeFormat5,
			DateTimeFormat6,
			DateTimeFormat7,
			DateTimeFormat8,
			DateTimeFormat9,

			DateTimeFormatNoSec1,
			DateTimeFormatNoSec1a,
			DateTimeFormatNoSec1b,
			DateTimeFormatNoSec1c,
			DateTimeFormatNoSec2,
			DateTimeFormatNoSec3,
			DateTimeFormatNoSec3a,
			DateTimeFormatNoSec4,
			DateTimeFormatNoSec5,
			DateTimeFormatNoSec6,

			/** adding new key?
			  * 1. insert new key before DateTimeFormatLast
			  * 2. insert new element in DateFormatInfo (refer to .cpp file)
			  */

			DateTimeFormatLast, //never pass this one as a parameter!
        };

		/**
		  * DateFormat
		  * 
		  * Constructor
		  */
		DateFormat();

		/**
		  * ~DateFormat
		  * 
		  * Destructor
		  */
		virtual ~DateFormat() {}

		/**
		  * getDateFormat
		  * 
		  * Returns the date format
		  *
		  * @param dateFormat Format desired. Any of EDateFormatKey.
		  *
		  * @return Date format in string. Refer to DateFormatInfo.
		  *
		  */	
		std::string getDateFormat(long dateFormatKey);

	private:

		bool m_IsForKRTC;


		/* //DO NOT DELETE!

		////////////// Default format (4669) ////////////// 

		// Standard Date Format: 
		static const std::string DATE_FORMAT_1 = "%d/%m/%Y"; // conflict (%d-%b-%y)
		static const std::string DATE_FORMAT_2 = "YYYYMMDD"; // no change
		static const std::string DATE_FORMAT_3 = "YYYY_MM_DD"; // no change
		static const std::string DATE_FORMAT_4 = "%A, %d-%b-%y"; 
		static const std::string DATE_FORMAT_5 = "dd-mmm-yy"; 

		// Standard Date + Time format: 
		static const std::string DATE_TIME_FORMAT_1 = "%d/%m/%Y %H:%M:%S"; // conflict (%d-%b-%y %H:%M:%S)
		static const std::string DATE_TIME_FORMAT_2 = "YYYYMMDDHH24MISS"; // no change
		static const std::string DATE_TIME_FORMAT_3 = "%a %#d/%#m/%Y %#H:%M:%S"; 
		static const std::string DATE_TIME_FORMAT_4 = "%d-%b-%Y %H:%M:%S"; // no change // also used in 4677
		static const std::string DATE_TIME_FORMAT_5 = "%d/%m/%Y %H:%M:00"; 
		static const std::string DATE_TIME_FORMAT_6 = "%A, %B %d, %Y  %H:%M:%S"; 
		static const std::string DATE_TIME_FORMAT_7 = "%d/%m/%Y %I:%M:%S %p"; // used in 4677 only 
		static const std::string DATE_TIME_FORMAT_8 = "%d/%m %H:%M:%S"; 
		static const std::string DATE_TIME_FORMAT_9 = "dd-mmm-yy hh:nn"; 
		
		// Standard Date + Time format (without seconds): 
		static const std::string DATE_TIME_FORMAT_NO_SEC_1 = "%d/%m/%Y %H:%M"; // conflict (%Y %B %d %H:%M) (%d-%b-%y %H:%M) (%d %B %Y %H:%M)
		//static const std::string DATE_TIME_FORMAT_NO_SEC_2 = "YYYYMMDDHH24MI"; // 4677 only // unused?
		static const std::string DATE_TIME_FORMAT_NO_SEC_3 = "%d %b %Y %I:%M%p"; // conflict (%d %b %Y - %I:%M%p)
		static const std::string DATE_TIME_FORMAT_NO_SEC_4 = "%H:%M - %d %b"; // used in 4677 only // no change
		static const std::string DATE_TIME_FORMAT_NO_SEC_5 = "%B %d"; // no change
		static const std::string DATE_TIME_FORMAT_NO_SEC_6 = "mmm-yy"; 	

		////////////// Default format (4669) ////////////// 



		////////////// KRTC format (4669) ////////////// 

		// Standard Date Format: 
		static const std::string KRTC_DATE_FORMAT_1 = "%Y/%m/%d"; //Format: 2006/02/09
		static const std::string KRTC_DATE_FORMAT_2 = "YYYYMMDD"; //Format: 20060209
		static const std::string KRTC_DATE_FORMAT_3 = "YYYY_MM_DD"; //Format: 2006_02_09
		static const std::string KRTC_DATE_FORMAT_4 = "%A, %Y/%m/%d"; //Format: Thursday, 2006/02/09
		static const std::string KRTC_DATE_FORMAT_5 = "yy-mmm-dd"; //Format: 

		// Standard Date + Time format: 
		static const std::string KRTC_DATE_TIME_FORMAT_1 = "%Y/%m/%d %H:%M:%S"; //Format: 2006/02/09 18:21:01
		static const std::string KRTC_DATE_TIME_FORMAT_2 = "YYYYMMDDHH24MISS"; //Format: 20060209182101
		static const std::string KRTC_DATE_TIME_FORMAT_3 = "%a %Y/%#m/%#d %#H:%M:%S"; //Format: Fri 2006/2/10 10:34:10
		static const std::string KRTC_DATE_TIME_FORMAT_4 = "%d-%b-%Y %H:%M:%S"; //Format: 10-Feb-2006 11:06:14 //NOTE: Required format refer to TD11252
		static const std::string KRTC_DATE_TIME_FORMAT_5 = "%Y/%m/%d %H:%M:00"; //Format: 2006/02/18 16:58:00
		static const std::string KRTC_DATE_TIME_FORMAT_6 = "%A, %Y/%m/%d %H:%M:%S"; //Format: Tuesday, 2006/02/21 19:37:11
		static const std::string KRTC_DATE_TIME_FORMAT_7 = "%Y/%m/%d %I:%M:%S %p"; //Format: 2006/02/21 07:58:50 PM
		static const std::string KRTC_DATE_TIME_FORMAT_8 = "%m/%d %H:%M:%S"; //Format: 03/13 16:37:00
		static const std::string KRTC_DATE_TIME_FORMAT_9 = "yy-mmm-dd hh:nn"; //Format: 

		// Standard Date + Time format (without seconds): 
		static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_1 = "%Y/%m/%d %H:%M"; //Format: 2006/02/09 18:21
		//static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_2 = "YYYYMMDDHH24MI"; //Format: 200602091821
		static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_3 = "%Y/%m/%d %I:%M%p"; //Format: 2006/02/10 11:52AM
		static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_4 = "%H:%M - %d %b"; //Format: 20:02 - 21 Feb
		static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_5 = "%B %d"; //Format: March 13 
		static const std::string KRTC_DATE_TIME_FORMAT_NO_SEC_6 = "yy-mmm"; //Format:

		////////////// KRTC format (4669) ////////////// 

		*/
	};
}

#endif // !defined(AFX_DATEFORMAT_H__47B17328_1EA5_45D6_A0F3_41F4A3D1A9A2__INCLUDED_)
