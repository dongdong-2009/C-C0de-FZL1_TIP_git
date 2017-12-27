/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$  * Last modified by:  $Author$
  * 
  * This class holds the detail for a single application configured in the database.
  * It will also hold some extra details such as the icon loaded from the exe and
  * the tooltip to display.
  */

#include "app/system_control/control_station/src/StdAfx.h"
#include "app/system_control/control_station/src/Application.h"
#include "app/system_control/control_station/src/resource.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "bus/resources/resource.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::IEntityData;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IResource;

// The length of application name we can show in the launch area
const int Application::APPLICATION_NAME_LENGTH = 9;

// The ID of each applications icon.
const int Application::ICON_ID = 301;

// The size in pixels of the icons
const int Application::ICON_WIDTH = 32;
const int Application::ICON_HEIGHT = 32;

// The default tooltip to use for applications
const std::string Application::DEFAULT_TOOL_TIP = "Launch application";

// The tool tip will look like:
// TOOL_TIP_PRE_TEXT + Application Name + TOOL_TIP_POST_TEXT
const std::string Application::TOOL_TIP_PRE_TEXT = "Launch ";
const std::string Application::TOOL_TIP_POST_TEXT = "";

// This is the extension to look for on exectuables when parsing the launch string
const CString Application::EXECUTABLE_EXTENSION = ".exe";

// This is the runtime parameter name for entity name
const std::string Application::ENTITYNAME_PARAM = "--entity-name=";

/////////////////////////////////
// Construction/Destruction
/////////////////////////////////

Application::Application(TA_Base_Core::IGui* gui)
    : m_guiDatabaseEntry(gui),
      m_hotIcon(NULL),
      m_coldIcon(NULL),
      m_toolTip(DEFAULT_TOOL_TIP),
      m_resourceKey(-1),
      m_transActiveApplication (true) // always start with transactive apps
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "Constructor");

    TA_ASSERT( m_guiDatabaseEntry != NULL, "IGui passed in to Application constructor was NULL" );

    if (SessionManager::getInstance().getScreenPositioning().isLaunchBarDisplayed())
    {
        // Only do this if we're using the launch bar as it takes up unnecessary processing
        // time otherwise.
        loadIcons();
    }

    std::string appName = getFullName();
    if( !appName.empty() )
    {
        m_toolTip = TOOL_TIP_PRE_TEXT;
        m_toolTip += appName.c_str();
        m_toolTip += TOOL_TIP_POST_TEXT;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "Constructor");

    loadResourceKey();
};


Application::~Application()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "Destructor");
    try
    {
        if (m_guiDatabaseEntry != NULL)
        {
            delete m_guiDatabaseEntry;
            m_guiDatabaseEntry = NULL;
        }
    }
    catch ( ... )
    {
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "Destructor");
}

/////////////////////////////////
// Icon Manipulation
/////////////////////////////////

void Application::loadIcons()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadIcons");

    // First retrieve the executable name
    CString executable("");
    try
    {
        executable = m_guiDatabaseEntry->getExecutable().c_str();

        // Now remove anything after '.exe' from the address
        int iSpace = executable.Find(EXECUTABLE_EXTENSION);
        if (iSpace != -1)
        {
		    if (executable.GetLength() > iSpace + EXECUTABLE_EXTENSION.GetLength())
		    {
			    executable = executable.Left(iSpace + EXECUTABLE_EXTENSION.GetLength());
		    }

		    executable.Replace("\\","\\\\"); // Replace backslash with double backslash
		    executable.Replace("/", "\\\\"); // Replace forward slash with double backslash
		    executable.Replace(" ", "\\ ");  // Add a backslash in front of all spaces.
        }
        else
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Executable not in valid format. Does not contain '.exe': %s",executable );
            executable = "";
        }
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not load the executable for the application. No icon will be loaded" );
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not load the executable for the application. No icon will be loaded" );
    }

   	HICON hotIcon = NULL;
   
    // OK if we successfully found an exectuble then start trying to find an icon
    if ( !executable.IsEmpty() )
    {
    	HMODULE iconMod = LoadLibrary(executable);
	    if( iconMod == NULL)
	    {
		    // File not found so do not display in Control Station
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application %s not found. Will not be shown in the launch bar.",executable );
            m_hotIcon = NULL;
            m_coldIcon = NULL;
            return;
	    }

	    // Get the hot icon as we don't care about the cold as we make our own!
    	LPVOID	lpCol = getIconDirFromFile(iconMod, ICON_ID);
	    if( lpCol != NULL )
	    {
		    hotIcon = getIconFromFile(iconMod, lpCol, ICON_WIDTH, ICON_HEIGHT, LR_DEFAULTCOLOR);
	    }
	    else // There was no .exe
	    {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application %s not a valid file. Will not be shown in the launch bar.",executable );
            m_hotIcon = NULL;
            m_coldIcon = NULL;
            return;
	    }

	    if (iconMod != NULL)
	    {
		    FreeLibrary(iconMod);
	    }
    }


	// If the hotIcon is still NULL then an error occurred so we must load the default icon.
	if (hotIcon == NULL)
    {
		hotIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DEFAULT_HOT));
    }

	m_hotIcon = hotIcon;

    // Now create the equivalent grey-scale icon as our cold Icon
    createGreyscaleIcon();

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadIcons");

}


BOOL CALLBACK Application::iconFound( HMODULE  hLibrary, LPCTSTR  lpszType, LPTSTR  lpszName, LONG here)	
{
	LPTSTR	lpID = NULL;
	HRSRC   hRsrc = NULL;
	HGLOBAL hGlobal = NULL; 
	LPVOID  lpIcon = NULL;

	if (((Application *) here)->m_guiDefIconDir != NULL)
		return TRUE;
  
	if( HIWORD(lpszName) == 0 )
	{
		lpID = lpszName;
	}
	else 
    {
		// Name is string
		lpID = strdup( lpszName );	
	}     
        
	// Find the group icon resource
	if( (hRsrc = FindResource( hLibrary, lpID, RT_GROUP_ICON )) == NULL )  
	{
		return TRUE;         
	}         
	if( (hGlobal = LoadResource( hLibrary, hRsrc )) == NULL ) 
	{
		return TRUE;
    }
	if( (lpIcon = LockResource(hGlobal)) == NULL )
	{
		return TRUE;
	}      
	((Application *) here)->m_guiDefIconDir = lpIcon;

	return false;
}


HICON Application::getIconFromFile(const HMODULE& iconMod, const LPVOID& lpResIn, const int iconWidth, const int iconHeight, const UINT colour)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getIconFromFile");

	int nID = LookupIconIdFromDirectoryEx((PBYTE) lpResIn, TRUE, iconWidth, iconHeight, colour); 

	HRSRC	iconResource;
	if ((iconResource = FindResource(iconMod, MAKEINTRESOURCE(nID), MAKEINTRESOURCE(RT_ICON))) == NULL)
    {
		return NULL;
    }

    HGLOBAL	hGlobal;
	if ((hGlobal = LoadResource(iconMod, iconResource)) == NULL)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "getIconFromFile");
		return NULL;
    }

    LPVOID	lpRes = NULL;
	if ((lpRes = LockResource(hGlobal)) == NULL)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "getIconFromFile");
		return NULL;
    }

	HICON hIcon = CreateIconFromResourceEx( (PBYTE) lpRes, SizeofResource(iconMod, iconResource), TRUE, 0x00030000, iconWidth, iconHeight, colour);

    LOG( SourceInfo, DebugUtil::FunctionExit, "getIconFromFile");
	return hIcon;
}


LPVOID Application::getIconDirFromFile(const HMODULE& iconMod, const int iconID)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getIconDirFromFile");

	HRSRC	iconResource;
	if ((iconResource = FindResource(iconMod, MAKEINTRESOURCE(iconID), RT_GROUP_ICON)) == NULL)
	{
		m_guiDefIconDir = NULL;
		EnumResourceNames(iconMod, RT_GROUP_ICON, this->iconFound, (LPARAM)this);
        LOG( SourceInfo, DebugUtil::FunctionExit, "getIconDirFromFile");
		return m_guiDefIconDir;
	}

    HGLOBAL	hGlobal;
	if ((hGlobal = LoadResource(iconMod, iconResource))  == NULL)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "getIconDirFromFile");
		return NULL;
    }

    LPVOID	lpRes = NULL;
	if((lpRes = LockResource(hGlobal)) == NULL )
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "getIconDirFromFile");
		return NULL;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "getIconDirFromFile");
    return lpRes;
}


void Application::createGreyscaleIcon()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "createGreyscaleIcon");

	ICONINFO	csII;
	BOOL bRetValue = ::GetIconInfo(m_hotIcon, &csII);
	if (bRetValue == FALSE)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "createGreyscaleIcon");
		return;
    }

	HDC hMainDC = ::GetDC(::AfxGetMainWnd()->m_hWnd);
	HDC hMemDC1 = ::CreateCompatibleDC(hMainDC);
	HDC hMemDC2 = ::CreateCompatibleDC(hMainDC);
	if (hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "createGreyscaleIcon");
		return;
    }
  
    ICONINFO    csGrayII;
	BITMAP		bmp;
	if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
	{
		csGrayII.hbmColor = ::CreateBitmap(csII.xHotspot*2, csII.yHotspot*2, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
		if (csGrayII.hbmColor)
		{
			HBITMAP hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, csII.hbmColor);
			HBITMAP hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, csGrayII.hbmColor);

			::BitBlt(hMemDC2, 0, 0, csII.xHotspot*2, csII.yHotspot*2, hMemDC1, 0, 0, SRCCOPY);

			for (DWORD dwLoopY = 0; dwLoopY < csII.yHotspot*2; dwLoopY++)
			{
				for (DWORD dwLoopX = 0; dwLoopX < csII.xHotspot*2; dwLoopX++)
				{
                    COLORREF crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);

					BYTE byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) + (GetGValue(crPixel) * 0.587) + (GetBValue(crPixel) * 0.114));
					if (crPixel)
                    {
                        ::SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
                    }
				} // for
			} // for

			::SelectObject(hMemDC1, hOldBmp1);
			::SelectObject(hMemDC2, hOldBmp2);

			csGrayII.hbmMask = csII.hbmMask;

			csGrayII.fIcon = TRUE;
			m_coldIcon = ::CreateIconIndirect(&csGrayII);
		} // if

		::DeleteObject(csGrayII.hbmColor);
	} // if

	::DeleteObject(csII.hbmColor);
	::DeleteObject(csII.hbmMask);
	::DeleteDC(hMemDC1);
	::DeleteDC(hMemDC2);
	::ReleaseDC(::AfxGetMainWnd()->m_hWnd, hMainDC);

    LOG( SourceInfo, DebugUtil::FunctionExit, "createGreyscaleIcon");
}

/////////////////////////////////
// Get Functions
/////////////////////////////////


std::string Application::getDisplayName()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getDisplayName");

    TA_ASSERT(m_guiDatabaseEntry != NULL, "GUI database entry held in Application is NULL");

    try
    {
        std::string name = m_guiDatabaseEntry->getName();

        // If the name is too large we must truncate it
        if (name.length() > APPLICATION_NAME_LENGTH) 
	    {
            // We subtract 2 so we have enough space for the ".."
		    name = name.substr(0,APPLICATION_NAME_LENGTH-2) + "..";
	    }

        LOG( SourceInfo, DebugUtil::FunctionExit, "getDisplayName");
        return name;
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve application name" );
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve application name" );
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "getDisplayName");
    return "";
}


std::string Application::getFullName()
{
    TA_ASSERT(m_guiDatabaseEntry != NULL, "GUI database entry held in Application is NULL");

    try
    {
        return m_guiDatabaseEntry->getFullName();
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve application name" );
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve application name" );
    }
    return "";
}


bool Application::isIconVisible()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "isIconVisible");
    TA_ASSERT(m_guiDatabaseEntry != NULL, "GUI database entry held in Application is NULL");

    try
    {
        return m_guiDatabaseEntry->isIconDisplayed();
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve whether application icon is visible or not" );
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve whether application icon is visible or not" );
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "isIconVisible");
    return false;
}


bool Application::isAccessible()
{
    if ( true == m_transActiveApplication )
    {
    return (SessionManager::getInstance().isActionPermitted(TA_Base_Bus::aca_RUN_APPLICATION, m_resourceKey) == TA_Base_App::TA_ControlStation::PERMITTED);
    }
    else
    {
        // always allow non transactive applications.
        return true;
    }
}


void Application::loadResourceKey()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialiseResourceKey" );

    try
    {
	    std::string executable = m_guiDatabaseEntry->getExecutable();
        std::string::size_type startIndex = executable.find(ENTITYNAME_PARAM, 0);
		if (startIndex == std::string::npos)
        {
            //  Not a transactive application, as no ENTITY name specified
            m_transActiveApplication = false;
            return;
        }
        std::string::size_type endIndex = executable.find(" ", startIndex+ENTITYNAME_PARAM.length());
		if (endIndex == std::string::npos)
            endIndex = executable.length();
        executable = executable.substr(startIndex+ENTITYNAME_PARAM.length(), endIndex - (startIndex+ENTITYNAME_PARAM.length()));

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Entity name is %s", executable.c_str());
        IEntityData* entity = EntityAccessFactory::getInstance().getEntity(executable.c_str());

        if (entity != NULL)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "IEntityData %s is available from database.  Going to fetch IResource now", 
                executable.c_str());

            IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( entity->getKey() );
            if (resource != NULL)
            {
                m_resourceKey = resource->getKey();
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                    "IResource key = %d", m_resourceKey);
                delete resource;
            }
            delete entity;
        }
    }
    catch ( DataException&)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while fetching entity or resource data from database");
    }
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown exception", 
            "Some unknown exception has occurred when attempting to get the entity key or resource key of the application");
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "initialiseResourceKey" );
}
