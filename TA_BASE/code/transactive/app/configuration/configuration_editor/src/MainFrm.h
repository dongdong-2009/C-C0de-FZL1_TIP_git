/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/MainFrm.h $
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

#if !defined(AFX_MAINFRM_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
#define AFX_MAINFRM_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/configuration_editor/src/FixedSplitterWnd.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "bus/generic_gui/src/TransActiveFrame.h"
#include "core/utilities/src/RunParams.h"
#include <map>

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
    // Forward Declarations
    class ConfigurationEditorToolBar;
    class ConfigurationEditorStatusBar;
    class ConfigurationEditorMenu;
    class ConfigurationEditorSelectionBar;
    class RightsChecker;

    class CMainFrame : public TA_Base_Bus::TransActiveFrame, virtual public TA_Base_Core::RunParamUser
    {
    protected:
        /**
          * Constructor
          *
          * create from serialization only
          */
        CMainFrame();

        DECLARE_DYNCREATE(CMainFrame)

    public:

        //TD18095, jianghp
        void init();
        /**
          * getSelectionBar
          *
          * This retrieves the selection bar so other classes can then retrieve information
          * from it.
          *
          * @return ConfigurationEditorSelectionBar* - A pointer to the selection bar object.
          *                                            This can be NULL so should be checked.
          */
        ConfigurationEditorSelectionBar* getSelectionBar() const
        {
            return m_selectionBar;
        }

        /**
          * setCallbackObject
          *
          * This is overridden from TransActiveFrame. It calls the base version and then
          * makes sure that the RunParams are set for all DLLs.
          *
          * @param IGuiAccess& - The IGUIAccess object to perform calls on. This
          *                      cannot be NULL.
          *
          * @exception GenericGUIException - This may throw a GenericGUI exception of type UNKNOWN_ACCESS_RIGHTS
          *                                  if the rights cannnot be set up correctly.
          */
        virtual void setCallbackObject(TA_Base_Bus::IGUIAccess& gui);

        /**
          * onRunParamChange
          *
          * This will be called when one of the runparams we've registered for is updated
          *
          * @param string - The name of the parameter that has changed
          * @param value - The new value of the parameter
          */
        virtual void onRunParamChange(const std::string& name, const std::string& value);

        /**
          * displayComponentDetails
          *
          * This will simulate a user clicking on a component to display the
          * configuration for that component.
          *
          * @param item - the component number in the selection bar
          */
        void displayComponentDetails(LPARAM item);

        /**
          * getHelpFile
          *
          * This will retrieve the help file for the current component
          *
          * @return string - The help file path as configured for the component. This will be blank if
          *                  no help file is currently valid or no help file is configured.
          */
        virtual std::string getHelpFile();

    protected:

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CMainFrame)
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
        //}}AFX_VIRTUAL

        // Implementation
    public:
        virtual ~CMainFrame();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

    protected:

        // Generated message map functions
        //{{AFX_MSG(CMainFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void onViewLargeIcons();
        afx_msg void onViewSmallIcons();
        afx_msg void onAbout();
        afx_msg void onReload();
        afx_msg void OnClose();
        afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
        afx_msg void onToolbarHotItemChange(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onToolbarGetToolTip(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg long onOutbarNotify(WPARAM wParam, LPARAM lParam);
        afx_msg long onBlankScreen(WPARAM wParam, LPARAM lParam);
        afx_msg long onSessionUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg long onStartupViewUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg void onMenuItemSelected(UINT nID);
        afx_msg void onMenuItemUpdated(CCmdUI* pCmdUI);
        afx_msg void onToolBarItemSelected(UINT nID);
        afx_msg void onToolBarItemUpdated(CCmdUI* pCmdUI);
        virtual afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        CMainFrame(const CMainFrame&);

        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        CMainFrame& operator=(const CMainFrame&);

        /**
          * loadRightSide
          *
          * When the user first clicks an item in the selection bar we must divide up the
          * right-hand side of the window into the title at the top and the modification view
          * at the bottom. This method will return straight away if it has been called before.
          * It will only be executed in full once.
          */
        void loadRightSide();

        /**
          * changeView
          *
          * This is called when the view in the bottom-right pane needs to be changed because the
          * user has selected a new component. It will remove the old view and create a new one in
          * its place
          *
          * @param CCreateContext& - This is the context the window needs to create the view.
          * @param const CString& - This is the component's title or description that will be
          *                         used in the title strip.
          */
        void changeView(CCreateContext& context, const CString& itemTitle);

        /**
          * setStartupView
          *
          * Takes a RunParam value (StartupView) and uses it to set the component to edit
          * on startup, as opposed to the default value of the first component in the first
          * folder.
          */
        void setStartupView();

        /**
          * redrawHorizontalSplitter
          *
          * After a modification to the splitter on the right of the window this should be called to
          * redraw it correctly.
          */
        void redrawHorizontalSplitter();

        /**
          * redrawVerticalSplitter
          *
          * After a modification to the main splitter this should be called to
          * redraw it correctly.
          */
        void redrawVerticalSplitter();

        /**
          * setWindowTitle
          *
          * This sets the text in the title bar for the Configuration Editor
          * It will look something like 'Application Name - USER@DATABASE
          */
        void setWindowTitle();

        /**
          * canReloadOrShutdown
          *
          * This will check if it is ok to lose any changes before continuing and reloading or
          * exiting. This will display a message to the user asking if they are happy to lose
          * changes.
          *
          * @return bool - This will return true if it is ok to lose any data and continue. This will
          *                return false if the reload or exit should be aborted.
          */
        bool canReloadOrShutdown();

        /**
          * setStatusBarTextForItem
          *
          * When an item is selected from the toolbar or menu (selected includes just moving the mouse
          * over it) the status bar text should display an explanation of the item. This method takes
          * a control Id and updates the status bar to show a caption for it.
          *
          * @param UINT - The control Id of the item the user selected.
          */
        void setStatusBarTextForItem(UINT nID);

    private:

        // See the CPP file for descriptions of these constants
        static const int SELECTION_BAR_WIDTH;
        static const int TITLE_BAR_HEIGHT;
        static const int LEFT_PANE;
        static const int RIGHT_PANE;
        static const int TOP_PANE;
        static const int BOTTOM_PANE;
        static const int ZERO_ROW;
        static const int ZERO_COLUMN;
        static const int APP_START_WIDTH;
        static const int APP_START_HEIGHT;

        ComponentLibraryAccessor m_libraryAccessor;      // Used to load the components and access them

        Component* m_currentlySelectedComponent;         // This is the component the user currently has
        // selected in the selection bar.

        ConfigurationEditorStatusBar*    m_statusBar;    // The status bar at the bottom of the screen
        ConfigurationEditorToolBar*      m_toolBar;      // The toolbar displayed at the top of the screen.
        ConfigurationEditorMenu*         m_menu;         // The menu at the top of the screen
        ConfigurationEditorSelectionBar* m_selectionBar; // The bar at the side of the screen that shows the components

        RightsChecker& m_rightsChecker;     // This is used to check the rights of an action the user
        // wants to perform.

        CSplitterWnd 	m_wndVerticalSplitter;      // This is the first splitter used and separates the
        // selection bar and the view on the right.

        CFixedSplitterWnd 	m_wndHorizontalSplitter;    // This is the second splitter used and splits the view
        // on the right. It splits into a title bar at the top
        // and the view at the bottom.
        bool m_runWithoutControlStation;
    };
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
