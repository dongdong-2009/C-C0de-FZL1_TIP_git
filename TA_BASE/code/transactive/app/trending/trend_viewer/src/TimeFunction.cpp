#if !defined(AFX_TIMEFUNCTION_CPP__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_)
#define AFX_TIMEFUNCTION_CPP__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_

/**
  * TimeFunction.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TimeFunction.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains time conversion routines.
  * This class is a singleton.
  *
  */

#include <string>

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/historyviewer.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/HistoryInitialisationException.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    TimeFunction* TimeFunction::s_timeFunctionInstance = NULL;
	//TA_Base_Core::NonReEntrantThreadLockable TimeFunction::s_singletonLock; //TD17935

    //
    // TimeFunction
    //
    TimeFunction::TimeFunction()
    {
        GetTimeZoneInformation( &m_TimeZoneInformation );
    }

    //
    // ~TimeFunction
    //
    TimeFunction::~TimeFunction()
    {
    }


    //
    // getInstance
    //
    TimeFunction& TimeFunction::getInstance()
    {
        // protect the instance check / creation
		// TA_Base_Core::ThreadGuard guard(s_singletonLock); //TD17935

        if(s_timeFunctionInstance == 0)
        {
            s_timeFunctionInstance = new TimeFunction();
        }
        
        return *s_timeFunctionInstance;
    }

    
    //
    // ConvertToUTC
    //
    CString TimeFunction::ConvertToUTC(const COleDateTime& inTime)
    {
        CTime newTime(inTime.GetYear(), inTime.GetMonth(), inTime.GetDay(),
                      inTime.GetHour(), inTime.GetMinute(), inTime.GetSecond());
        newTime.GetLocalTm(NULL);
		// TD12474 ++
		//return( newTime.FormatGmt( "%d/%m/%Y %H:%M" ) );
		TA_Base_Core::DateFormat m_DateFormat; 
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1);
		return( newTime.FormatGmt( dateFormatStr.c_str() ) );
		// ++ TD12474 
    }


    //
    // ConvertToUTCTimeT
    //
    time_t TimeFunction::ConvertToUTCTimeT(const COleDateTime& inTime)
    {
        // Convert to UTC
        CTime newTime(inTime.GetYear(), inTime.GetMonth(), inTime.GetDay(),
                      inTime.GetHour(), inTime.GetMinute(), inTime.GetSecond());
        tm t2 = *(newTime.GetGmtTm(NULL));
        CTime gmTime(t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday, t2.tm_hour, t2.tm_min, t2.tm_sec);
        return gmTime.GetTime();
    }


    //
    // ConvertToLocalTimeT
    //
    time_t TimeFunction::ConvertToLocalTimeT(time_t inTime)
    {
        COleDateTime convertedTime;
        ConvertToLocal(inTime, convertedTime);
        CTime theTime(convertedTime.GetYear(), convertedTime.GetMonth(), convertedTime.GetDay(),
            convertedTime.GetHour(), convertedTime.GetMinute(), convertedTime.GetSecond());
        return theTime.GetTime();
    }


    //
    // ConvertToUTC
    //
    time_t TimeFunction::ConvertToTimeT(const COleDateTime& inTime)
    {
        // Convert to UTC
        CTime newTime(inTime.GetYear(), inTime.GetMonth(), inTime.GetDay(),
                      inTime.GetHour(), inTime.GetMinute(), inTime.GetSecond());
        return newTime.GetTime();
    }


#ifdef USE_OCCI
    // 
    // ConvertToLocal
    //
    void TimeFunction::ConvertToLocal(const oracle::occi::Date& inDate, COleDateTime& outTime)
    {
        //
        // Get the values from the oracle Date object
        //
        int year;
        unsigned int month, day, hour, minute, second;
        inDate.getDate(year, month, day, hour, minute, second);

        //
        // Place them in a SYSTEMTIME object
        //
        SYSTEMTIME universalTime, localTime;
        universalTime.wYear = year;
        universalTime.wMonth = month;
        universalTime.wDay = day;
        universalTime.wHour = hour;
        universalTime.wMinute = minute;
        universalTime.wSecond = second;
        universalTime.wMilliseconds = 0;
        universalTime.wDayOfWeek = 0;

        //
        // Convert the UTC time to local time
        //
        SystemTimeToTzSpecificLocalTime(&m_TimeZoneInformation, &universalTime, &localTime);

        //
        // Set the values in outTime
        //
        outTime.SetDateTime(localTime.wYear, localTime.wMonth, localTime.wDay, 
                            localTime.wHour, localTime.wMinute, localTime.wSecond);

        return;
    }
#endif // USE_OCCI


    // 
    // ConvertToLocal
    //
    void TimeFunction::ConvertToLocal(const std::string& inDate, COleDateTime& outTime)
    {
        //
        // Parse the date/time string - this throws an exception, but let it be caught
        // higher up.
        //
        time_t parsedInDate = ParseDateTime(inDate);
        ConvertToLocal(parsedInDate, outTime);
    }


    // 
    // ConvertToLocal
    // 
    void TimeFunction::ConvertToLocal(time_t inDate, COleDateTime& outTime)
    {
        struct tm *newtime = NULL;
        newtime = localtime( &inDate ); 

        //
        // Place them in a SYSTEMTIME object
        //
        SYSTEMTIME universalTime, localTime;
        universalTime.wYear = newtime->tm_year + 1900;
        universalTime.wMonth = newtime->tm_mon + 1;
        universalTime.wDay = newtime->tm_mday;
        universalTime.wHour = newtime->tm_hour;
        universalTime.wMinute = newtime->tm_min;
        universalTime.wSecond = newtime->tm_sec;
        universalTime.wMilliseconds = 0;
        universalTime.wDayOfWeek = 0;

        //
        // Convert the UTC time to local time
        //
        SystemTimeToTzSpecificLocalTime(&m_TimeZoneInformation, &universalTime, &localTime);

        //
        // Set the values in outTime
        //
        outTime.SetDateTime(localTime.wYear, localTime.wMonth, localTime.wDay, 
                            localTime.wHour, localTime.wMinute, localTime.wSecond);
    }


    // 
    // ParseDateTime
    //
    time_t TimeFunction::ParseDateTime(const std::string& inDate)
    {
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "ParseDateTime" );

        //
        // The date string should have no non-numeric characters. This test will determine that.
        //
		std::string::size_type numberTest = inDate.find_first_not_of("0123456789");
        
        //
        // The target string MUST bet 14 characters long, and 14 characters long ONLY
        // If the correct database select has been used this will be guaranteed.
        // We have not used an assert here as it could be possible to load the data with a valid 
        // string and get something invalid
        //
        if ((14 != inDate.size()) || (std::string::npos != numberTest))
        {
            TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
        }

        std::string strYear = inDate.substr(0,4);
        std::string strMonth = inDate.substr(4,2);
        std::string strDay = inDate.substr(6,2);
        std::string strHour = inDate.substr(8,2);
        std::string strMinute = inDate.substr(10,2);
        std::string strSecond = inDate.substr(12,2);

        int iYear = atoi(strYear.c_str());
        int iMonth = atoi(strMonth.c_str());
        int iDay = atoi(strDay.c_str());
        int iHour = atoi(strHour.c_str());
        int iMinute = atoi(strMinute.c_str());
        int iSecond = atoi(strSecond.c_str());

        //
        // Need to sanity check the date data, as mktime won't do it for us
        // NB: It is not checked that the day is valid for the month
        //
        bool dateSanityError = false;
        dateSanityError = (iSecond > 59 || iMinute > 59 || iHour > 24);
        dateSanityError = (dateSanityError || iDay > 31 && iDay != 0);
        dateSanityError = (dateSanityError || iMonth > 12 || iMonth == 0);
        dateSanityError = (dateSanityError || iYear > 2038 || iYear < 1900);
        if (dateSanityError)
        {
            TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
        }

        const int FEBRUARY = 2;
        const int APRIL = 4;
        const int JUNE = 6;
        const int SEPTEMBER = 9;
        const int NOVEMBER = 11;

        //
        // Now need to check that the day is valid based on the month/year
        //
        if (iMonth == APRIL || iMonth == JUNE || iMonth == SEPTEMBER || iMonth == NOVEMBER) // 30 days
        {
            if (iDay == 31)
            {
                TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
            }
        }
        else if (iMonth == FEBRUARY) // more processing necessary
        {        
            if (0 == (iYear%4) && (0 != (iYear%100) || 0 == (iYear%400))) // This is a leap year
            {
                if (iDay > 29)
                {
                    TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
                }
            }
            else // not a leap year
            {
                if (iDay > 28)
                {
                    TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
                }
            }
        }
        // else - don't need to check for 31 days, as we did that above

        struct tm newtm;

        newtm.tm_sec = iSecond;        //seconds
        newtm.tm_min = iMinute;        // hours 
        newtm.tm_hour = iHour;        // minutes 
        newtm.tm_mday = iDay;        // day of the month 
        newtm.tm_mon = iMonth-1;    // month of Year - goes from 0 to 11
        newtm.tm_year = iYear-1900;    // years since 1900 - this structure only deals with years 
                                    // between 1901 and 2038
        newtm.tm_isdst = 0;            // disable daylight savings - this could have implications

        time_t value;
        value = mktime(&newtm);
        
        // check that the time conversion worked successfully
        if (-1 == value) // conversion failed
        {
            TA_THROW(TA_Base_Core::HistoryInitialisationException("Invalid time field"));
        }

        return value;
    }


	std::string TimeFunction::ConvertToString(const time_t inTime)
    {	
		
        struct tm *t;
		
        char str[22];
        t = localtime(&inTime);
		sprintf(str, "%04d%02d%02d%02d%02d%02d", 
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		
		std::string timeStr = str;		 
		return timeStr;
    }

} // TA_Base_App

#endif // AFX_TIMEFUNCTION_CPP__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_


