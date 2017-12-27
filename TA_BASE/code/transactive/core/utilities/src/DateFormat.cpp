/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DateFormat.cpp $
  * @author:   Azenith Arcilla
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This file provides a standard format for date and time
  */

#include "DateFormat.h"
#include "core/utilities/src/RunParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//DateFormat::DateFormat()
//{
//
//}
//
//DateFormat::~DateFormat()
//{
//
//}

namespace TA_Base_Core
{
	//const std::string RPRAM_KRTCDATEFORMAT = "KrtcDateFormat"; //transferred to RunParams.h
	const std::string DEFAULT_DATE_FORMAT = "%d/%m/%Y";

	struct DateFormatData
	{
		DateFormat::EDateFormatKey	dateFormatKey;
		const char*					strFormatDefault;
		const char*					strFormatKRTC;
	};

	DateFormatData DateFormatInfo[] =
	{
		//dateFormatKey						strFormatDefault				strFormatKRTC
		{DateFormat::DateFormat1,			"%d/%m/%Y",						"%Y/%m/%d"},
		{DateFormat::DateFormat1a,			"%d-%b-%y",						"%Y/%m/%d"}, //separator changed
		{DateFormat::DateFormat1b,			"DD/MM/YYYY",					"YYYY/MM/DD"}, // TD16458
		{DateFormat::DateFormat2,			"YYYYMMDD",						"YYYYMMDD"},
		{DateFormat::DateFormat2a,			"%Y%m%d",						"%Y%m%d"},   // Mysql
		{DateFormat::DateFormat3,			"YYYY_MM_DD",					"YYYY_MM_DD"},
		{DateFormat::DateFormat4,			"%A, %d-%b-%y",					"%A, %Y/%m/%d"}, //separator changed
		{DateFormat::DateFormat5,			"dd-mmm-yy",					"yy-mmm-dd"},
		{DateFormat::DateFormat6,			"dd'-'MMM'-'yyy",				"dd'-'MMM'-'yyy"}, // used only in 4669 incident manager

		{DateFormat::DateTimeFormat1,		"%d/%m/%Y %H:%M:%S",			"%Y/%m/%d %H:%M:%S"},
		{DateFormat::DateTimeFormat2,		"YYYYMMDDHH24MISS",				"YYYYMMDDHH24MISS"},
		{DateFormat::DateTimeFormat3,		"%a %#d/%#m/%Y %#H:%M:%S",		"%a %Y/%#m/%#d %#H:%M:%S"},
		{DateFormat::DateTimeFormat4,		"%d-%b-%y %H:%M:%S",			"%Y/%m/%d %H:%M:%S"}, 
		{DateFormat::DateTimeFormat5,		"%d/%m/%Y %H:%M:00",			"%Y/%m/%d %H:%M:00"},
		{DateFormat::DateTimeFormat6,		"%A, %B %d, %Y  %H:%M:%S",		"%A, %Y/%m/%d %H:%M:%S"}, //separator changed
		{DateFormat::DateTimeFormat7,		"%d/%m/%Y %I:%M:%S %p",			"%Y/%m/%d %I:%M:%S %p"},
		{DateFormat::DateTimeFormat8,		"%d/%m %H:%M:%S",				"%m/%d %H:%M:%S"},
		{DateFormat::DateTimeFormat9,		"dd-mmm-yy hh:nn",				"yy-mmm-dd hh:nn"},

		{DateFormat::DateTimeFormatNoSec1,	"%d/%m/%Y %H:%M",				"%Y/%m/%d %H:%M"},
		{DateFormat::DateTimeFormatNoSec1a,	"%d %B %Y %H:%M",				"%Y/%m/%d %H:%M"}, //separator changed
		{DateFormat::DateTimeFormatNoSec1b,	"%Y %B %d %H:%M",				"%Y/%m/%d %H:%M"},  //separator changed
		{DateFormat::DateTimeFormatNoSec1c,	"%d-%b-%y %H:%M",				"%Y/%m/%d %H:%M"},  //separator changed
		{DateFormat::DateTimeFormatNoSec2,	"YYYYMMDDHH24MI",				"YYYYMMDDHH24MI"},
		{DateFormat::DateTimeFormatNoSec3,	"%d %b %Y %I:%M%p",				"%Y/%m/%d %I:%M%p"}, //separator changed
		{DateFormat::DateTimeFormatNoSec3a,	"%d %b %Y - %I:%M%p",			"%Y/%m/%d - %I:%M%p"},  //separator changed
		{DateFormat::DateTimeFormatNoSec4,	"%H:%M - %d %b",				"%H:%M - %d %b"},
		{DateFormat::DateTimeFormatNoSec5,	"%B %d",						"%B %d"},
		{DateFormat::DateTimeFormatNoSec6,	"mmm-yy",						"yy-mmm"},

		/** adding new element?
		  * 1. insert new element before DateTimeFormatLast
		  * 2. insert new key in EDateFormatKey (refer to .h file)
		  */
	
		{DateFormat::DateTimeFormatLast,	"%d/%m/%Y",						"%Y/%m/%d"},
	};

	DateFormat::DateFormat()
	{
		//set value of m_IsForKRTC
		if (TA_Base_Core::RunParams::getInstance().isSet(RPRAM_KRTCDATEFORMAT))
		{
			m_IsForKRTC = true;
		}
		else
		{
			m_IsForKRTC = false;
		}
	}

	std::string DateFormat::getDateFormat(long dateFormatKey)
	{
		std::string dateFormatString = DEFAULT_DATE_FORMAT;

		const DateFormatData* dfd = DateFormatInfo;
        while ( dfd->dateFormatKey != DateTimeFormatLast )
        {
			if (dfd->dateFormatKey == dateFormatKey)
			{
				if (m_IsForKRTC)
				{
					dateFormatString = dfd->strFormatKRTC;
				}
				else
				{
					dateFormatString = dfd->strFormatDefault;
				}
				break;
			}

            ++dfd;
        }

		return dateFormatString;
	}
}