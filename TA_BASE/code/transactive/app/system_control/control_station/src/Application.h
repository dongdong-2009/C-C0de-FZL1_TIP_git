/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/Application.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds the detail for a single application configured in the database.
  * It will also hold some extra details such as the icon loaded from the exe and
  * the tooltip to display.
  */


#if !defined(Application_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
#define Application_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_

#include <string>

// Forward declarations
namespace TA_Base_Core
{
    class IGui;
}

class Application
{
public:
    /**
    * Constructor
    *
    * @param IGui* - The object holding the applications details from the database.
    */
    Application(TA_Base_Core::IGui* gui);


    /**
    * Destructor
    */
    virtual ~Application();


    /**
    * getGuiDatabaseEntry
    *
    * @return A reference to the Gui's database entry from the GUIAPPLICATION table
    */
    TA_Base_Core::IGui& getGuiDatabaseEntry()
    {
        return *m_guiDatabaseEntry;
    }


    /**
    * getToolTip
    *
    * @return A string representing the text to display in a tooltip
    */
    std::string getToolTip()
    {
        return m_toolTip;
    }


    /**
    * isAccessible
    *
    * @return A boolean indicating whether this application is accessible as per the security settings
    *         true means the operator has permission to access this application
    */
    bool isAccessible();


    /**
    * getHotIcon
    *
    * @return A handle to an icon. This icon is the icon to display when the user has their
    *         cursor over the application icon. If this returns NULL then the application should
    *         not be available to be launched.
    */
    HICON getHotIcon()
    {
        return m_hotIcon;
    };


    /**
    * getColdIcon
    *
    * @return A handle to an icon. This icon is the icon to display all the time for the application
    *         If this returns NULL then the application should not be available to be launched.
    */
    HICON getColdIcon()
    {
        return m_coldIcon;
    };


    /**
    * getDisplayName
    *
    * @return A string which is the display name of the application. This is retrieved from the
    *         IGui* object. It will return an empty string if the name cannot be retrieved. This
    *         method truncates the name to a set number of characters and adds an elipsis if this
    *         is done.
    */
    std::string getDisplayName();


    /**
    * getFullName
    *
    * @return std::string The full name of the application for display to the user.
    */
    std::string getFullName();


    /**
    * isIconVisible
    *
    * @return A boolean which indicates whether the application can be seen in the launch bar.
    *         This is retrieved from the IGui* object. It will return false if the value cannot
    *         be retrieved.
    */
    bool isIconVisible();

	unsigned int getResourceKey(){
	
		return m_resourceKey;
	}
    // This is used when converting an icon into it's black and white version.
	LPVOID	m_guiDefIconDir;

private:
   
    // Made private as they are not needed
	Application( const Application& theApplication);
	Application& operator=(const Application& pm);

    /**
    * loadIcons
    *
    * This methods loads the hot icon for this application (from the executable specified).
    * It also generates the cold icon so that the member variables m_hotIcon and m_coldIcon
    * are both populated.
    */
    void loadIcons();


    /**
    * getIconDirFromFile
    *
    * This method loads the icon resource from the executable.
    * 
    * @param HMODULE& - The result of calling LoadLibrary on the executable.
    * @param int - The resource ID of the required icon.
    *
    * @return LPVOID - The address of the resource data
    */
	LPVOID getIconDirFromFile(const HMODULE& iconMod, const int iconID);


    /**
    * getIconFromFile
    *
    * This retrieves the icon details from the file and actually creates a local version of
    * the icon.
    *
    * @param HMODULE& - The result of calling LoadLibrary on the executable.
    * @param LPVOID& - The address of the resource data
    * @param int - The width of the icon to be created from the loaded resource
    * @param int - The height of the icon to be created from the loaded resource
    * @param UINT - This indicates whether to use the default colours (LR_DEFAULTCOLOR) or
    *               monochrome (LR_MONOCHROME).
    *
    * @return HICON - A handle to the newly created icon.
    */
	HICON getIconFromFile(const HMODULE& iconMod, const LPVOID& lpRes, const int iconWidth, const int iconHeight, const UINT colour);


    /**
    * iconFound
    *
    * This is a callback used by the createGreyscaleIcon() method. For more information see
    * EnumResNameProc in the MSDN.
    *
    * @param HMODULE - Handle to the module whose executable file contains the resources for which
    *                  the names are being enumerated. If this parameter is NULL, the function
    *                  enumerates the resource names in the module used to create the current process. 
    * @param LPCTSTR - Pointer to a null-terminated string specifying the type name of the resource for
    *                  which the name is being enumerated.
    * @param LPTSTR  - Pointer to a null-terminated string specifying the name of the resource for
    *                  which the name is being enumerated. 
    * @param LONG    - Specifies the application-defined parameter passed to the EnumResourceNames
    *                  function. This parameter can be used in error checking.
    *
    * @return BOOL - This callback function should return TRUE to continue enumeration; otherwise,
    *                it should return FALSE to stop enumeration.
    */
    static BOOL CALLBACK iconFound( HMODULE  hLibrary, LPCTSTR  lpszType, LPTSTR  lpszName, LONG  here );


    /**
	 * CreateGreyscaleIcon()
	 *
	 * This function will greate a grey version of the hot icon automagically
	 */
	void createGreyscaleIcon();


    /**
     * initialiseResourceKey
     *
     * Sets the resource key of the application
     */
    void loadResourceKey();

    //////////////
    // CONSTANTS
    //////////////

    // The length of application name we can show in the launch area
    static const int APPLICATION_NAME_LENGTH;

    // The ID of each applications icon.
    static const int ICON_ID;

    // The size in pixels of the icons
    static const int ICON_WIDTH;
    static const int ICON_HEIGHT;

    // The default tooltip to use for applications
    static const std::string DEFAULT_TOOL_TIP;

    // The tool tip will look like:
    // TOOL_TIP_PRE_TEXT + Application Name + TOOL_TIP_POST_TEXT
    static const std::string TOOL_TIP_PRE_TEXT;
    static const std::string TOOL_TIP_POST_TEXT;

    // This is the extension to look for on exectuables when parsing the launch string
    static const CString EXECUTABLE_EXTENSION;

    // This is the runtime parameter name for entity name
    static const std::string ENTITYNAME_PARAM;

    //////////////
    // Member Variables
    //////////////

    // The GUIAPPLICATION table database entry for this GUI.
    TA_Base_Core::IGui* m_guiDatabaseEntry;

    // The icon to display when the user moves the mouse of the application icon.
    HICON m_hotIcon;

    // The icon to display when the user does not have their mouse over the application icon.
    HICON m_coldIcon;

    // The tooltip to display when the user moves their mouse over the application icon.
    std::string m_toolTip;

    // The resource key of this application
    unsigned int m_resourceKey;

    // This is a transactive application, so rights are defined.
    bool m_transActiveApplication;
};

#endif // !defined(Application_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
