#if !defined(AFX_TREND_CONFIG_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_CONFIG_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendConfigDatabaseAccess.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendConfigDatabaseAccess.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class maintains a store of trend configuration data used by the Trend Viewer.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include <string>
#include <map>

//#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD17935

namespace TA_Base_App
{
    //
    // TrendConfigDatabaseAccess
    //
    class TrendConfigDatabaseAccess  
    {
    public:
        
        /**
          * getInstance
          * 
          * Returns the one and only instance of this class.
          *
          */
        static TrendConfigDatabaseAccess& getInstance();

        /**
          * ~TrendConfigDatabaseAccess
          * 
          * Destructor.
          *
          */
        ~TrendConfigDatabaseAccess() {}

        /**
          * loadSamplePeriods
          * 
          * Loads the available samplePeriods from the database.
          *
          * @exception TrendViewerException - thrown when there is a problem loading
          * from the database
          */
        void loadSamplePeriods();

    public:

        std::map<unsigned long, std::string> getSamplePeriods()
        {
            return m_samplePeriods;
        }

    private:

        TrendConfigDatabaseAccess();
        TrendConfigDatabaseAccess(const TrendConfigDatabaseAccess&) {}
        TrendConfigDatabaseAccess& operator=(const TrendConfigDatabaseAccess&) {}

    private:

        std::map<unsigned long, std::string> m_samplePeriods;           // Stores all the available sample periods. Indexed by
                                                                        // their value in seconds.

        static TrendConfigDatabaseAccess* s_trendConfigDatabaseAccess;

        // protect singleton creation
		//TD17935 - no need to protect singleton creation thru a thread
		// static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock; 

    };
}

#endif // !defined(AFX_TREND_CONFIG_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)




