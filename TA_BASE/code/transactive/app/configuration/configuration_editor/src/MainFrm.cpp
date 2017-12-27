/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/MainFrm.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the main frame of the application and this is where all our processing is done.
  * This object controls the toolbar and status bar as well as the menu items. This object
  * will set up the structure of the screen.
  */
#pragma warning(disable:4786 4407)
#include <sstream>
#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/MainFrm.h"
#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorDoc.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorView.h"
#include "app/configuration/configuration_editor/src/TitleView.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorToolBar.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorStatusBar.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorMenu.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSelectionBar.h"
#include "app/configuration/configuration_editor/src/ConfigurationProgressDlg.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/exceptions/src/GenericGUIException.h"

#define WM_BLANK_SCREEN                   (WM_USER + 150)
#define WM_SESSION_UPDATE                 (WM_USER + 151)
#define WM_START_VIEW_UPDATE              (WM_USER + 152)

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveFrame;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::RunParams;

// This is for the ConfigurationEditorConstants.h
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // TD15529
    const std::string CONFIGURATIONEDITOR_APPNAME = "ConfigurationEditor" ;
    const std::string CONFIGCONNECTIONFILE_NAME = "ConfigConnectionStrings.csv";
    const int CMainFrame::SELECTION_BAR_WIDTH = 150; // The width of the selection bar on the right
    const int CMainFrame::TITLE_BAR_HEIGHT    = 30;  // The height of the title bar which shows the
    // currently selected component

    // This is the application starting height and width. This may be changed by the Control Station or
    // user settings later but these are the default values that will be used.
    const int CMainFrame::APP_START_WIDTH = 1280;
    const int CMainFrame::APP_START_HEIGHT = 803;

    // These constants are used to pass to the splitter methods so the code is more readable
    const int CMainFrame::LEFT_PANE   = 0;
    const int CMainFrame::RIGHT_PANE  = 1;
    const int CMainFrame::TOP_PANE    = 0;
    const int CMainFrame::BOTTOM_PANE = 1;
    const int CMainFrame::ZERO_ROW    = 0;
    const int CMainFrame::ZERO_COLUMN = 0;

    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame

    IMPLEMENT_DYNCREATE(CMainFrame, TransActiveFrame)

    BEGIN_MESSAGE_MAP(CMainFrame, TransActiveFrame)
        //{{AFX_MSG_MAP(CMainFrame)
        ON_WM_CREATE()
        ON_COMMAND(ID_VIEW_LARGEICONS, onViewLargeIcons)
        ON_COMMAND(ID_VIEW_SMALLICONS, onViewSmallIcons)
        ON_COMMAND(ID_HELP_ABOUT, onAbout)
        ON_COMMAND(ID_APP_RELOAD, onReload)
        ON_WM_CLOSE()
        ON_WM_MENUSELECT()
        ON_MESSAGE(WM_OUTBAR_NOTIFY, onOutbarNotify)
        ON_MESSAGE(WM_BLANK_SCREEN, onBlankScreen)
        ON_MESSAGE(WM_SESSION_UPDATE, onSessionUpdate)
        ON_MESSAGE(WM_START_VIEW_UPDATE, onStartupViewUpdate)
        ON_COMMAND_RANGE(ConfigurationEditorMenu::WM_MENU_BEGIN, ConfigurationEditorMenu::WM_MENU_END, onMenuItemSelected)
        ON_UPDATE_COMMAND_UI_RANGE(ConfigurationEditorMenu::WM_MENU_BEGIN, ConfigurationEditorMenu::WM_MENU_END, onMenuItemUpdated)
        ON_COMMAND_RANGE(ConfigurationEditorToolBar::WM_TOOLBAR_BEGIN, ConfigurationEditorToolBar::WM_TOOLBAR_END, onToolBarItemSelected)
        ON_UPDATE_COMMAND_UI_RANGE(ConfigurationEditorToolBar::WM_TOOLBAR_BEGIN, ConfigurationEditorToolBar::WM_TOOLBAR_END, onToolBarItemUpdated)
        ON_NOTIFY(TBN_HOTITEMCHANGE, AFX_IDW_TOOLBAR, onToolbarHotItemChange)
        ON_NOTIFY(TBN_GETINFOTIP, AFX_IDW_TOOLBAR, onToolbarGetToolTip)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame construction/destruction

    CMainFrame::CMainFrame()
        : m_toolBar(NULL),
          m_currentlySelectedComponent(NULL),
          m_rightsChecker(RightsChecker::getInstance()),
          m_runWithoutControlStation(RunParams::getInstance().isSet(TA_Configuration::RPARAM_RUN_WITHOUT_CONTROL_STATION.c_str()))
    {
        FUNCTION_ENTRY("Constructor");

        setlocale(LC_ALL, "chs");

        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minWidth = 60;
        properties.minHeight = 30;
        setResizingProperties(properties);

        FUNCTION_EXIT;
    }

    CMainFrame::~CMainFrame()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            if (m_toolBar != NULL)
            {
                delete m_toolBar;
                m_toolBar = NULL;
            }

            if (m_menu != NULL)
            {
                delete m_menu;
                m_menu = NULL;
            }

            if (m_statusBar != NULL)
            {
                delete m_statusBar;
                m_statusBar = NULL;
            }

            if (m_selectionBar != NULL)
            {
                delete m_selectionBar;
                m_selectionBar = NULL;
            }

            RunParams::getInstance().deregisterRunParamUser(this);
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }

    BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
    {
        if (!TransActiveFrame::PreCreateWindow(cs))
        {
            return FALSE;
        }

        // This flag stops flicker in the CGfxOutBarCtrl control
        cs.style |= WS_CLIPCHILDREN;

        // Make sure we don't have the styles that force a document name to be listed
        // in the title bar.
        cs.style ^= FWS_ADDTOTITLE;

        // Set this to be the initial size. This will be changed later by the user settings or the
        // Control Station if required.
        cs.cx = APP_START_WIDTH;
        cs.cy = APP_START_HEIGHT;

        return TRUE;
    }

    BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
    {
        FUNCTION_ENTRY("OnCreateClient");

        //
        // First create the vertical splitter
        //
        // Create with 1 row and 2 columns
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating vertical splitter window");

        if (!m_wndVerticalSplitter.CreateStatic(this, 1, 2/*,WS_CLIPCHILDREN*/))
        {
            TA_THROW(TA_Base_Core::GenericGUIException("Failed to create vertical splitter window",
                     TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS));
        }

        // Creation of the standard view in the right pane to begin with. Once an item has been selected
        // this will be replaced with the horizontal splitter.
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating view in right pane of splitter window");

        if (!m_wndVerticalSplitter.CreateView(ZERO_ROW, RIGHT_PANE, pContext->m_pNewViewClass, CSize(0, 0), pContext))
        {
            TA_THROW(TA_Base_Core::GenericGUIException("Failed to create view in vertical splitter window",
                     TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS));
        }

        //
        // Load the components
        //

        //CConfigurationProgressDlg progress;
        //progress.Create( IDD_LOADPROGRESS );
        //m_libraryAccessor.loadComponents( &progress );
        //progress.DestroyWindow();

        //
        // Now determine which components we can display for this user
        //
        //if (m_libraryAccessor.getComponents().empty() )
        //{
        //    // If this is emtpy then no DLL's at all loaded
        //    LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "No component DLL's could be loaded so the application cannot continue");
        //    TA_Base_Bus::TransActiveMessage userMsg;
        //    userMsg.showMsgBox(IDS_UE_030056);
        //    TA_THROW( TA_Base_Core::ProcessAlreadyRunningException("We've already displayed an error to the user so throw this exception so the application closes quietly") );
        //}

        if (!RunParams::getInstance().isSet(TA_Configuration::RPARAM_RUN_WITHOUT_CONTROL_STATION.c_str()))
        {
            // Register for session id updates now that we have determined the available components for this session.
            RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
        }

        // Now we've loaded the components and determined which ones we can use,
        // we can load and populate the selection bar.
        m_selectionBar = new ConfigurationEditorSelectionBar(this,
                m_libraryAccessor,
                m_wndVerticalSplitter,
                m_wndVerticalSplitter.IdFromRowCol(ZERO_ROW, LEFT_PANE));

        redrawVerticalSplitter();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Set the active view to be the view in the right pane");
        SetActiveView(dynamic_cast<CView*>(m_wndVerticalSplitter.GetPane(ZERO_ROW, RIGHT_PANE)));

        FUNCTION_EXIT;
        return true;
    }

    int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
        FUNCTION_ENTRY("OnCreate");

        if (TransActiveFrame::OnCreate(lpCreateStruct) == -1)
        {
            return -1;
        }

        try
        {
            m_toolBar = new ConfigurationEditorToolBar(this, m_libraryAccessor);
        }
        catch (const TransactiveException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", "Cannot continue with onCreate");
            return -1;
        }

        try
        {
            m_statusBar = new ConfigurationEditorStatusBar(this);
        }
        catch (const TransactiveException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", "Cannot continue with onCreate");
            return -1;
        }

        m_menu = new ConfigurationEditorMenu(this, m_libraryAccessor);

        setWindowTitle();

        FUNCTION_EXIT;
        return 0;
    }

    void CMainFrame::setCallbackObject(TA_Base_Bus::IGUIAccess& gui)
    {
        FUNCTION_ENTRY("setCallbackObject");

        TransActiveFrame::setCallbackObject(gui);

        ////
        //// Set up the Configuration Editor pkey of the entity passed on the command line in the RunParams
        //// so the DLL's will know it.
        ////
        //TA_Base_Core::IEntityData* configEditorEntity = getGenericGUICallbackObject().getGUIEntity();
        //TA_ASSERT( configEditorEntity != NULL, "Configuration Editor entity is NULL");

        //std::ostringstream key;
        //key << configEditorEntity->getKey();
        //TA_Base_Core::RunParams::getInstance().set( RPARAM_CONFIGEDITOR_PKEY.c_str(), key.str().c_str() );

        //// Now tell the library accessor to set up the singletons in all the components
        //m_libraryAccessor.setUpSingletonsInComponents();

        //// Lastly, check the startup view RunParam
        //RunParams::getInstance().registerRunParamUser( this, RPARAM_STARTUP_VIEW.c_str() );
        //
        //std::string view(  );
        //if( !RunParams::getInstance().get(RPARAM_STARTUP_VIEW.c_str()).empty() )
        //{
        //    // set the component to show on startup
        //    setStartupView( );
        //}

        FUNCTION_EXIT;
    }

    void CMainFrame::setStartupView()
    {
        FUNCTION_ENTRY("setStartupView");

        CString startupView = RunParams::getInstance().get(RPARAM_STARTUP_VIEW.c_str()).c_str();

        std::vector<Component> components(m_libraryAccessor.getComponents());

        for (std::vector<Component>::iterator it = components.begin(); it != components.end(); it++)
        {
            Component component = *it;

            // Convert spaces in component to underscores
            CString componentName(component.component);
            componentName.Replace(' ', '_');

            if (componentName.Compare(startupView) == 0)
            {
                // set the view...
                m_selectionBar->viewComponent(component);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Startup view set to %s", it->component);
                break;
            }
        }

        FUNCTION_EXIT;
    }

    long CMainFrame::onOutbarNotify(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onOutbarNotify");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Parameters: (%d,%d)", wParam, lParam);

        switch (wParam)
        {
        case NM_FOLDERCHANGE:
            // cast the lParam to an integer to get the clicked folder
            // We don't care about the folder change so ignore this for now
            break;

        case NM_OB_ITEMCLICK:
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Item %d clicked in the selection bar", lParam);

            displayComponentDetails(lParam);

            break;
        }

        case NM_OB_ICON_SIZE_CHANGE:

            // lParam holds whether the icon is small.
            // BOOL isSmall. So it will equal 0 for a large icon
            if (m_menu != NULL)
            {
                if (lParam == 0)
                {
                    // icon size is large
                    m_menu->iconSizeChanged(LARGE_ICON_SIZE);
                }
                else
                {
                    m_menu->iconSizeChanged(SMALL_ICON_SIZE);
                }
            }

            break;

        case NM_OB_ONLABELENDEDIT:
            // cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
            // return 1 to do the change and 0 to cancel it

            // We are not performing label edits so we ignore
            break;

        case NM_OB_ONGROUPENDEDIT:
            // cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
            // return 1 to do the change and 0 to cancel it

            // We are not performing group edits so we ignore
            break;

        case NM_OB_DRAGITEM:
            // cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
            // return 1 to do the change and 0 to cancel it

            // We are not allowing dragging of items so ignore.
            break;
        }

        FUNCTION_EXIT;
        return 0;
    }

    void CMainFrame::displayComponentDetails(LPARAM item)
    {
        FUNCTION_ENTRY("displayComponentDetails");

        if (m_selectionBar != NULL)
        {
            // Retrieve the component associated with the item the user just selected
            Component* comp = m_selectionBar->getComponent(item);

            if (comp != NULL)
            {
                m_currentlySelectedComponent = comp;

                // We must set up a create context so the new view can be loaded and added to the
                // framework.
                CCreateContext context;
                context.m_pCurrentDoc = GetActiveDocument();
                context.m_pNewViewClass = comp->componentInterface->getComponentView();

                changeView(context, comp->component);
            }
        }

        FUNCTION_EXIT;
    }

    std::string CMainFrame::getHelpFile()
    {
        FUNCTION_ENTRY("getHelpFile");

        if (m_currentlySelectedComponent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No component currently selected");
            return "";
        }

        return m_currentlySelectedComponent->helpFile;

        FUNCTION_EXIT;
    }

    long CMainFrame::onBlankScreen(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onBlankScreen");

        // We must set up a create context so the new view can be loaded and added to the
        // framework.
        CCreateContext context;
        context.m_pCurrentDoc = GetActiveDocument();
        context.m_pNewViewClass = RUNTIME_CLASS(CConfigurationEditorView);

        changeView(context, "");

        FUNCTION_EXIT;
        return NULL;
    }

    long CMainFrame::onSessionUpdate(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onSessionUpdate");

        UINT selectedButton = showMsgBox(IDS_UW_010005);

        // Redetermine which components are available
        m_libraryAccessor.determineAvailableComponents(m_rightsChecker);
        m_selectionBar->updateSelectionBar();

        // If current selected item is now disabled then unselect it
        if ((m_currentlySelectedComponent == NULL) || !m_currentlySelectedComponent->isAccessible)
        {
            m_currentlySelectedComponent = NULL;

            // Post a message to blank the screen as this call is not in the main thread and cannot
            // perform such tasks
            PostMessage(WM_BLANK_SCREEN, 0, 0);
        }

        FUNCTION_EXIT;
        return NULL;
    }

    LRESULT CMainFrame::onStartupViewUpdate(WPARAM wParam, LPARAM lParam)
    {
        setStartupView();
        return 0;
    }

    void CMainFrame::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        if (name == RPARAM_STARTUP_VIEW)
        {
            // time to change views...
            PostMessage(WM_START_VIEW_UPDATE, 0, 0);
        }
        else if (name == RPARAM_SESSIONID)
        {
            // Display a message box to the user and update the GUI. We must make this call in the main thread
            m_rightsChecker.updateSession(value);
            //PostMessage(WM_SESSION_UPDATE, 0, 0);
        }

        FUNCTION_EXIT;
    }

    void CMainFrame::loadRightSide()
    {
        FUNCTION_ENTRY("loadRightSide");

        if (m_wndHorizontalSplitter.m_hWnd != NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Second splitter already created so just return");
            FUNCTION_EXIT;
            return;
        }

        //
        // Now split the screen again but horizontally
        //

        // Set up the required parts of a context so the splitter and view can be created
        // correctly
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting up the context structure");
        CCreateContext context;
        context.m_pCurrentDoc = GetActiveDocument();
        context.m_pNewViewClass = RUNTIME_CLASS(CConfigurationEditorView);

        // Create the splitter inside the right pane of the other splitter
        // Create with 2 rows and 1 column
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating the horizontal splitter inside the vertical one");

        if (!m_wndHorizontalSplitter.CreateStatic(&m_wndVerticalSplitter, 2, 1,
                WS_CHILD | WS_VISIBLE, m_wndVerticalSplitter.IdFromRowCol(ZERO_ROW, RIGHT_PANE)))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Failed to create horizontal splitter window");
            FUNCTION_EXIT;
            return;
        }

        // Creation of the standard view (for the bottom of the right side)
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating the view for the bottom of the splitter");

        if (!m_wndHorizontalSplitter.CreateView(BOTTOM_PANE, ZERO_COLUMN, context.m_pNewViewClass, CSize(0, 0), &context))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Failed to create view in horizontal splitter window");
            FUNCTION_EXIT;
            return;
        }

        // Delete the temporary view as the new view created in pane (1,0) of the horizontal
        // splitter will be our new active view.
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Deleting the old view from the right pane (so it can be replaced with the splitter)");
        m_wndVerticalSplitter.DeleteView(ZERO_ROW, RIGHT_PANE);

        // Create the title view (for the top of the right side
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating the title view to go in the top pane");
        m_wndHorizontalSplitter.CreateView(TOP_PANE, ZERO_COLUMN, RUNTIME_CLASS(TitleView), CSize(0, 0), &context);

        redrawVerticalSplitter();
        redrawHorizontalSplitter();

        // Set the active view to the view in the bottom right corner of the window
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting the active view to be the right, bottom pane");
        SetActiveView(dynamic_cast<CView*>(m_wndHorizontalSplitter.GetPane(BOTTOM_PANE, ZERO_COLUMN)));

        FUNCTION_EXIT;
    }

    void CMainFrame::redrawHorizontalSplitter()
    {
        FUNCTION_ENTRY("redrawHorizontalSplitter");

        CRect r;
        GetClientRect(&r);
        int h1 = TITLE_BAR_HEIGHT;
        int h2 = IsIconic() ? 0 : r.Height() - TITLE_BAR_HEIGHT;
        m_wndHorizontalSplitter.SetRowInfo(TOP_PANE, h1, h1);
        m_wndHorizontalSplitter.SetRowInfo(BOTTOM_PANE, h2, 0);
        m_wndHorizontalSplitter.RecalcLayout();

        FUNCTION_EXIT;
    }

    void CMainFrame::redrawVerticalSplitter()
    {
        FUNCTION_ENTRY("redrawVerticalSplitter");

        CRect r;
        GetClientRect(&r);
        int w1 = SELECTION_BAR_WIDTH;
        int w2 = r.Width() - SELECTION_BAR_WIDTH;
        m_wndVerticalSplitter.SetColumnInfo(LEFT_PANE, w1, 0);
        m_wndVerticalSplitter.SetColumnInfo(RIGHT_PANE, w2, 0);
        m_wndVerticalSplitter.RecalcLayout();

        FUNCTION_EXIT;
    }

    void CMainFrame::changeView(CCreateContext& context, const CString& itemTitle)
    {
        FUNCTION_ENTRY("changeView");

        // If the right side has not yet been loaded and set up then do this now
        loadRightSide();

        // We want to set this text in the title bar display
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "About to set the new title in the title view");
        TitleView* titleView = dynamic_cast<TitleView*>(m_wndHorizontalSplitter.GetPane(TOP_PANE, ZERO_COLUMN));

        if (titleView != NULL)
        {
            titleView->setTitleToDisplay(itemTitle);
        }

        // Replace the bottom right view with the one for this selected component
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "About to delete the old view");
        m_wndHorizontalSplitter.DeleteView(BOTTOM_PANE, ZERO_COLUMN);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "About to create the new view for the bottom-right pane");

        if (!m_wndHorizontalSplitter.CreateView(BOTTOM_PANE, ZERO_COLUMN, context.m_pNewViewClass, CSize(0, 0), &context))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "View could not be created. This is an internal error that needs to be fixed");

            UINT selectedButton = showMsgBox(IDS_UE_030063, itemTitle);

            // Since this failed then lets load our default view back in so next time everything works nicely
            context.m_pNewViewClass = RUNTIME_CLASS(CConfigurationEditorView);
            m_wndHorizontalSplitter.CreateView(BOTTOM_PANE, ZERO_COLUMN, context.m_pNewViewClass, CSize(0, 0), & context);
            redrawHorizontalSplitter();
            return;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "About to set the new view as the active view in the Main Frame");
        CView* currentView = dynamic_cast<CView*>(m_wndHorizontalSplitter.GetPane(BOTTOM_PANE, ZERO_COLUMN));

        if (currentView != NULL)
        {
            if (m_currentlySelectedComponent != NULL)
            {
                if (m_currentlySelectedComponent->componentInterface != NULL)
                {
                    m_currentlySelectedComponent->componentInterface->setStatusBar(m_statusBar);
                    m_currentlySelectedComponent->componentInterface->setCreatedView(currentView);
                }
            }

            SetActiveView(currentView);
        }

        redrawHorizontalSplitter();

        FUNCTION_EXIT;
    }

    void CMainFrame::setWindowTitle()
    {
        FUNCTION_ENTRY("setWindowTitle");

        CString title = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str();

        if (title.IsEmpty())
        {
            title = "ConfigurationEditor";
        }

        //CString databaseConnection = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION).c_str();
        std::string dbConnectionString;
        TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Read, dbConnectionString);
        CString databaseConnection = dbConnectionString.c_str();

        CString databaseName("");
        CString schemaName("");

        // We just want the database and schema name out of the database connection string. It should
        // be in the format DATABASE:SCHEMA:PASSWORD
        int firstColonPosition = databaseConnection.Find(':');

        if (firstColonPosition != -1)
        {
            databaseName = databaseConnection.Left(firstColonPosition);

            int secondColonPosition = databaseConnection.Find(":", firstColonPosition + 1);

            if (secondColonPosition != -1)
            {
                schemaName = databaseConnection.Mid(firstColonPosition + 1, secondColonPosition - firstColonPosition - 1);
            }
        }

        if (!databaseName.IsEmpty() && !schemaName.IsEmpty())
        {
            title += _T(" - ");
            title += schemaName;
            title += "@";
            title += databaseName;
        }

        SetWindowText(title);

        FUNCTION_EXIT;
    }

    bool CMainFrame::canReloadOrShutdown()
    {
        FUNCTION_ENTRY("canReloadOrShutdown");

        CString itemsNotApplied = "";
        // When this gets to ten we stop. We only report the first 10 items.
        int numberOfItemsNotApplied = 0;

        // Step through each component and check if it has changes pending

        for (std::vector<Component>::iterator iter = m_libraryAccessor.getComponents().begin();
                iter != m_libraryAccessor.getComponents().end();
                ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Component interface stored for thsi component is NULL");

            std::vector<std::string> listOfItems;
            std::string component = iter->component;

            if (iter->componentInterface->areCurrentChangesPending(listOfItems))
            {
                // Changes are pending so we need to add all items to a string we can use in
                // an error message at the end.
                for (std::vector<std::string>::iterator item = listOfItems.begin(); item != listOfItems.end(); ++item)
                {
                    if (numberOfItemsNotApplied < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                    {
                        itemsNotApplied += iter->category;
                        itemsNotApplied += _T(" - ");
                        itemsNotApplied += iter->component;

                        if (!item->empty())
                        {
                            itemsNotApplied += _T(" - ");
                            itemsNotApplied += item->c_str();
                        }

                        itemsNotApplied += _T("\n");
                        ++numberOfItemsNotApplied;
                    }
                    else
                    {
                        CString more;
                        more.LoadString(IDS_MORE);
                        itemsNotApplied += more;
                        //  Increment this again so we can tell outside this for-loop whether the
                        // ... has been added yet. This avoids the problem of having exactly
                        // MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE after this loop and breaking even though
                        // there are more items
                        ++numberOfItemsNotApplied;
                        break;
                    }
                } // End for

                // If we have the maximum number of items and have appended the ... then we can
                // finish checking for changes.
                if (numberOfItemsNotApplied > MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    break;
                }
            } // End if
        } // End for

        if (itemsNotApplied.IsEmpty())
        {
            // Then there were no unapplied items so we can return true
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No unapplied items were detected so we can continue");
            FUNCTION_EXIT;
            return true;
        }

        //UW-030017
        UINT selectedButton = showMsgBox(IDS_UW_030017, itemsNotApplied);

        if (selectedButton == IDOK)
        {
            // This means that the user is happy to lose their changes.
            FUNCTION_EXIT;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User indicated they were happy to lose any changes so we can continue");
            return true;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User does not want to lose changes so we cannot continue");
        FUNCTION_EXIT;
        return false;
    }

    void CMainFrame::setStatusBarTextForItem(UINT nID)
    {
        if (m_currentlySelectedComponent == NULL)
        {
            // Don't know which control we are referring to so just return
            return;
        }

        std::map<unsigned int, CString>::iterator matchingControl = m_currentlySelectedComponent->controlIds.find(nID);

        if (matchingControl == m_currentlySelectedComponent->controlIds.end())
        {
            // We do not have any status bar text to show for this control. It could be one of
            // our standard controls
            return;
        }

        if (m_statusBar != NULL)
        {
            m_statusBar->setStatusBarText(matchingControl->second);
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Menu and Toolbar Handlers

    void CMainFrame::OnClose()
    {
        FUNCTION_ENTRY("OnClose");

        if (!canReloadOrShutdown())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Exit has been aborted as the user is not willing to lose their changes");
            FUNCTION_EXIT;
            return;
        }

        TransActiveFrame::OnClose();

        FUNCTION_EXIT;
    }

    void CMainFrame::onReload()
    {
        FUNCTION_ENTRY("onReload");

        if (!canReloadOrShutdown())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Reload has been aborted as the user is not willing to lose their changes");
            FUNCTION_EXIT;
            return;
        }

        // Redetermine which components are available
        m_libraryAccessor.determineAvailableComponents(m_rightsChecker);
        m_selectionBar->updateSelectionBar();

        for (std::vector<Component>::iterator iter = m_libraryAccessor.getComponents().begin();
                iter != m_libraryAccessor.getComponents().end();
                ++iter)
        {
            if (iter->componentInterface != NULL)
            {
                iter->componentInterface->reload();
            }
        }

        // If the currently selected item is no longer accessible then we must click off it
        if ((m_currentlySelectedComponent != NULL) && (!m_currentlySelectedComponent->isAccessible))
        {
            m_currentlySelectedComponent = NULL;
            PostMessage(WM_BLANK_SCREEN, 0, 0);
        }

        // Now that the data has been reloaded we need to ensure that the view gets reloaded so all data in
        // it is fresh.
        if ((m_currentlySelectedComponent != NULL) && (m_currentlySelectedComponent->componentInterface != NULL))
        {
            // We must set up a create context so the view can be refreshed and re-loaded into the framework
            CCreateContext context;
            context.m_pCurrentDoc = GetActiveDocument();
            context.m_pNewViewClass = m_currentlySelectedComponent->componentInterface->getComponentView();

            changeView(context, m_currentlySelectedComponent->component);
        }

        FUNCTION_EXIT;
    }

    void CMainFrame::onViewLargeIcons()
    {
        if (m_selectionBar != NULL)
        {
            m_selectionBar->setIconSizeForSelectionBar(LARGE_ICON_SIZE);
        }
    }

    void CMainFrame::onViewSmallIcons()
    {
        if (m_selectionBar != NULL)
        {
            m_selectionBar->setIconSizeForSelectionBar(SMALL_ICON_SIZE);
        }
    }

    void CMainFrame::onAbout()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }

    void CMainFrame::onMenuItemSelected(UINT nID)
    {
        if (m_menu != NULL)
        {
            m_menu->onMenuItemSelected(nID, m_currentlySelectedComponent);
        }
    }

    void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
    {
        TransActiveFrame::OnMenuSelect(nItemID, nFlags, hSysMenu);

        setStatusBarTextForItem(nItemID);
    }

    void CMainFrame::onMenuItemUpdated(CCmdUI* pCmdUI)
    {
        if (m_menu != NULL)
        {
            m_menu->onMenuItemUpdated(pCmdUI, m_currentlySelectedComponent);
        }
    }

    void CMainFrame::onToolBarItemSelected(UINT nID)
    {
        if (m_toolBar != NULL)
        {
            m_toolBar->onToolBarItemSelected(nID, m_currentlySelectedComponent);
        }
    }

    void CMainFrame::onToolBarItemUpdated(CCmdUI* pCmdUI)
    {
        if (m_toolBar != NULL)
        {
            m_toolBar->onToolBarItemUpdated(pCmdUI, m_currentlySelectedComponent);
        }
    }

    void CMainFrame::onToolbarHotItemChange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        LPNMTBHOTITEM hotItem = reinterpret_cast<LPNMTBHOTITEM>(pNMHDR);

        setStatusBarTextForItem(hotItem->idNew);

        *pResult = 0;
    }

    void CMainFrame::onToolbarGetToolTip(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // We must retrieve the tool tip text and set it in the structrue passed in.
        LPNMTBGETINFOTIP lptbgit = reinterpret_cast<LPNMTBGETINFOTIP>(pNMHDR);

        if (m_toolBar != NULL)
        {
            CString text = m_toolBar->getToolTipText(lptbgit->iItem);

            if (text.GetLength() > lptbgit->cchTextMax)
            {
                text = text.Left(lptbgit->cchTextMax);
            }

            lptbgit->pszText = const_cast<char*>((LPCTSTR)text);
        }
    }

    void CMainFrame::init()
    {
        setWindowTitle();
        //
        // Load the components
        //
        CConfigurationProgressDlg progress;
        progress.Create(IDD_LOADPROGRESS, this);
        progress.CenterWindow(this);

        m_libraryAccessor.loadComponents(&progress);
        //m_libraryAccessor.loadComponents2(&progress);
        progress.DestroyWindow();

        //
        // Now determine which components we can display for this user
        //
        //m_libraryAccessor.determineAvailableComponents(m_rightsChecker); // already done in m_libraryAccessor.loadComponents2

        if (m_libraryAccessor.getComponents().empty())
        {
            // If this is emtpy then no DLL's at all loaded
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "No component DLL's could be loaded so the application cannot continue");
            showMsgBox(IDS_UE_030056);
            TA_THROW(TA_Base_Core::ProcessAlreadyRunningException("We've already displayed an error to the user so throw this exception so the application closes quietly"));
        }

        //
        // Set up the Configuration Editor pkey of the entity passed on the command line in the RunParams
        // so the DLL's will know it.
        //
        TA_Base_Core::IEntityData* configEditorEntity = getGenericGUICallbackObject().getGUIEntity();
        TA_ASSERT(configEditorEntity != NULL, "Configuration Editor entity is NULL");

        std::ostringstream key;
        key << configEditorEntity->getKey();
        TA_Base_Core::RunParams::getInstance().set(RPARAM_CONFIGEDITOR_PKEY.c_str(), key.str().c_str());

        // Now tell the library accessor to set up the singletons in all the components
        m_libraryAccessor.setUpSingletonsInComponents();

        // Lastly, check the startup view RunParam
        RunParams::getInstance().registerRunParamUser(this, RPARAM_STARTUP_VIEW.c_str());

        if (!RunParams::getInstance().get(RPARAM_STARTUP_VIEW.c_str()).empty())
        {
            // set the component to show on startup
            setStartupView();
        }

        m_toolBar->populateToolBar();
        m_selectionBar->init();
        redrawVerticalSplitter();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Set the active view to be the view in the right pane");
        SetActiveView(dynamic_cast<CView*>(m_wndVerticalSplitter.GetPane(ZERO_ROW, RIGHT_PANE))); //*/

        m_selectionBar->RemoveDefaultFolder();
    }

    void CMainFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
    {
        if (m_runWithoutControlStation)
        {
            CFrameWnd::OnWindowPosChanging(lpwndpos);
        }
        else
        {
            TransActiveFrame::OnWindowPosChanging(lpwndpos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // CMainFrame diagnostics

#ifdef _DEBUG
    void CMainFrame::AssertValid() const
    {
        TransActiveFrame::AssertValid();
    }

    void CMainFrame::Dump(CDumpContext& dc) const
    {
        TransActiveFrame::Dump(dc);
    }

#endif //_DEBUG
}
