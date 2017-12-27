/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RADAIHelper.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// RADAIHelper.h: interface for the RADAIHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADAIHELPER_H__21AD021C_8FF1_44F1_B5A8_3F603E63173D__INCLUDED_)
#define AFX_RADAIHELPER_H__21AD021C_8FF1_44F1_B5A8_3F603E63173D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace TA_Base_Core
{
	class RADAIHelper  
	{
	public:
		RADAIHelper();
		virtual ~RADAIHelper();

		static std::string getLocalDatabaseName();
		static long getLocationKey();

	};

}
#endif // !defined(AFX_RADAIHELPER_H__21AD021C_8FF1_44F1_B5A8_3F603E63173D__INCLUDED_)
