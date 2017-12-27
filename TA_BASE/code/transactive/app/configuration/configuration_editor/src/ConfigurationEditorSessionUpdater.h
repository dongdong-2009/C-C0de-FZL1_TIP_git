/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_TA_BASE_DDB/transactive/app/configuration/configuration_editor/src/ConfigurationEditorGUI.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2006/10/20 12:38:59 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This is the ConfigurationEditor implementation of the GenericGUI IGUIApplication and IGUIAccess
  * interfaces.
  */

#if !defined(AFX_CONFIGURATION_EDITOR_SESSION_UPDATER_H___INCLUDED_)
#define AFX_CONFIGURATION_EDITOR_SESSION_UPDATER_H___INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnection.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

namespace TA_Base_App
{
	class ConfigurationEditorSessionUpdater
    {
	public:

		ConfigurationEditorSessionUpdater();

		bool updateConfigSessionTimeStamp();
        void insertConfigSessionTimeStamp();

	private:

		TA_Base_Core::IDatabase* m_databaseConnection;
        TA_Base_Core::SQLStatement m_updateSql;
	};
	
}

#endif