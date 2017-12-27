#if !defined(AFX_TREND_CONFIG_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_CONFIG_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendConfigDatabaseAccess.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendConfigDatabaseAccess.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class maintains a store of trend configuration data used by the Trend Viewer.
  *
  */

#include "app/trending/trend_viewer/src/TrendConfigDatabaseAccess.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/trends/src/TrendAccessFactory.h"

namespace TA_Base_App
{

    TrendConfigDatabaseAccess* TrendConfigDatabaseAccess::s_trendConfigDatabaseAccess = 0;
	//TA_Base_Core::NonReEntrantThreadLockable TrendConfigDatabaseAccess::s_singletonLock; //TD17935


    //
    // getInstance
    //
    TrendConfigDatabaseAccess& TrendConfigDatabaseAccess::getInstance()
    {
        // protect the instance check / creation
		// TA_Base_Core::ThreadGuard guard(s_singletonLock); //TD17935

        if( s_trendConfigDatabaseAccess == 0 )
        {
            s_trendConfigDatabaseAccess = new TrendConfigDatabaseAccess();
        }
        
        return *s_trendConfigDatabaseAccess;
    }


    //
    // TrendConfigDatabaseAccess
    //
    TrendConfigDatabaseAccess::TrendConfigDatabaseAccess()
    {
        FUNCTION_ENTRY("TrendConfigDatabaseAccess");

        loadSamplePeriods();

        FUNCTION_EXIT;
    }

    //
    // loadSamplePeriods
    //
    void TrendConfigDatabaseAccess::loadSamplePeriods()
    {
        FUNCTION_ENTRY("loadSamplePeriods");

        try
        {
            m_samplePeriods = TA_Base_Core::TrendAccessFactory::getInstance().getAllSamplePeriods();
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());

            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, "An unexpected error occurred."));
        }

        FUNCTION_EXIT;
    }
}

#endif // !defined(AFX_TREND_CONFIG_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)




