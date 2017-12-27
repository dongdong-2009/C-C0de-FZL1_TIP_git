/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InfoTabHelper.h $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This function is a helper for Information Tab to get and store the Inspector Panel
  * entity attibutes
  */


#if !defined(AFX_INFOTABHELPER_H__A57F2E17_B15E_44DF_9D6E_B2843D76F8A3__INCLUDED_)
#define AFX_INFOTABHELPER_H__A57F2E17_B15E_44DF_9D6E_B2843D76F8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
	class InfoTabHelper  
	{
	public:
		InfoTabHelper();
		virtual ~InfoTabHelper();

		static void setShowQuality(bool bShow);
		static void setShowTimestamp(bool bShow);

		static bool getShowQuality();
		static bool getShowTimestamp();


		/**
		 * Display the quality column in the info tab
		 **/
		static bool		m_showQuality;

		/**
		 * Display the timestamp column in the info tab
		 **/
		static bool		m_showTimestamp;
	};
}

#endif // !defined(AFX_INFOTABHELPER_H__A57F2E17_B15E_44DF_9D6E_B2843D76F8A3__INCLUDED_)
