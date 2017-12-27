#if !defined(AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_)
#define AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_

/**
  * TimeFunction.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TimeFunction.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ctime>
#include <afx.h>
#include <afxdisp.h>
#include <string>

//#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD17935

namespace TA_Base_App
{
    //
    // TimeFunction
    //
    class TimeFunction  
    {
    public:

        /**
          * getInstance
          * 
          * Returns the one and only TimeFunction instance.
          *
          * @return The TimeFunction instance
          *
          */
        static TimeFunction& getInstance();

        /**
          * ~TimeFunction
          * 
          * Destructor.
          *
          */
        virtual ~TimeFunction();

        /**
          * ConvertToUTC
          * 
          * Convert a local COleDateTime into a UTC string.
          *
          * @param inTime - Local time to convert
          *
          * @return The UTC string
          *
          */
        CString ConvertToUTC(const COleDateTime& inTime);

        /**
          * ConvertToUTCTimeT
          * 
          * Convert a local COleDateTime into a UTC time_t.
          *
          * @param inTime - Local time to convert
          *
          * @return The UTC time_t
          *
          */
        time_t ConvertToUTCTimeT(const COleDateTime& inTime);

        /**
          * ConvertToLocalTimeT
          * 
          * Convert a gm time_t into a local time_t.
          *
          * @param inTime - gm time to convert
          *
          * @return The lcoal time_t
          *
          */
        time_t ConvertToLocalTimeT(time_t inTime);

        /**
          * ConvertToTimeT
          * 
          * Convert a local COleDateTime into a local time_t.
          *
          * @param inTime - Local time to convert
          *
          * @return The local time_t
          *
          */
        time_t ConvertToTimeT(const COleDateTime& inTime);
    
    #ifdef USE_OCCI
        
        /**
          * ConvertToLocal
          * 
          * Convert from a UTC oracle Date in a occi::Date to a local COleDateTime.
          *
          * @param inDate - Oracle date
          * @param outTime - Converted time
          *
          */
        void ConvertToLocal(const oracle::occi::Date& inDate, COleDateTime& outTime);

    #endif // USE_OCCI

        /**
          * ConvertToLocal
          * 
          * Convert from a UTC oracle Date in a std::string to a local COleDateTime.
          *
          * @param inDate - Oracle date
          * @param outTime - Converted time
          *
          */
        void ConvertToLocal(const std::string& inDate, COleDateTime& outTime);

        /**
          * ConvertToLocal
          * 
          * Convert from a time_t Date to a local COleDateTime.
          *
          * @param inDate - Time_t date
          * @param outTime - Converted time
          *
          */
        void ConvertToLocal(time_t inDate, COleDateTime& outTime);

        /**
          * ConvertToString
          * 
          * Convert a local time_t to string format.
          *
          * @param inTime - local time_t to convert
          *
          * @return the string fromat of the local time_t
          *
          */
		std::string ConvertToString(const time_t inTime);

    private:
        
        TimeFunction();
        TimeFunction(const TimeFunction&) {}
        TimeFunction& operator=(const TimeFunction &) {}

        time_t ParseDateTime(const std::string& inDate);

    private:

        TIME_ZONE_INFORMATION m_TimeZoneInformation;                    // Stores the current time-zone parameters. These 
                                                                        // parameters control the translations between 
                                                                        // Coordinated Universal Time (UTC) and local time. 

        static TimeFunction* s_timeFunctionInstance;
		//TD17935 - no need to protect singleton creation thru a thread
		// static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock; 
    };

} // TA_Base_App

#endif // !defined(AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_)




