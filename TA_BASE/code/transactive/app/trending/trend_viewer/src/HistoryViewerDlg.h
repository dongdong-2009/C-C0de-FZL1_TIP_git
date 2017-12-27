#if !defined(AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_)
#define AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_

/**
  * HistoryViewerDlg.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoryViewerDlg.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The main dialog class. Contains the graph object, toolbar and display area. This class
  * processes the notifications from the graph object and toolbar and updates the display
  * area accordingly.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include <map>

#include "app/trending/trend_viewer/src/dlgbars.h"
#include "app/trending/trend_viewer/src/resource.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "app/trending/trend_viewer/src/DlgBars.h"
#include "app/trending/trend_viewer/src/ILoaderObserver.h"
#include "app/trending/trend_viewer/src/IGraphObserver.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"

#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "boost/smart_ptr.hpp"
#include "bus/security/rights_library/src/RightsLibrary.h"

// TD12625 azenitha++
//#include "app/trending/trend_viewer/src/TrendsDialog.h" //TD17529 - not necessary
class CTrendsDialog;
// TD12625 ++azenitha

//TD17529++
class CTrendPropertiesDialog;
//++TD17529

//{{AFX_INCLUDES()
#include "cots/TeeChart/tchart.h"
//}}AFX_INCLUDES

#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_App
{
    //
    // Forward declarations
    //
    class IGraph;
    

    //
    // CHistoryViewerDlg
    //
    class CHistoryViewerDlg 
        : public TA_Base_Bus::TransActiveDialog, 
          public TA_Base_Core::RunParamUser,
          public ILoaderObserver,
          public IGraphObserver
    {
    
    public:

        /**
          * CHistoryViewerDlg
          * 
          * Constructor.
          *
          */
        CHistoryViewerDlg(TA_Base_Bus::IGUIAccess* controlClass);

        /**
          * ~CHistoryViewerDlg
          * 
          * Destructor.
          *
          */
        virtual ~CHistoryViewerDlg(); //TD17529 - moved the function definition to the class file
		     

		virtual void init();
        /**
          * onRunParamChange
          * 
          * Called when there is a change in a Run Param. Implemented to 
          * satisfy the RunParamUser interface.
          *
          * @param name Run param name
          * @param value New run param value
          *
          */
        virtual void onRunParamChange(const std::string& name, const std::string& value);

        /**
          * loadingComplete
          * 
          * This method is called once all items have been loaded from the database.
          */
        virtual void loadingComplete();

        /**
          * processError
          * 
          * This method is called if there has been a database loading error.
          *
          * @param error - error message
          */
        virtual void processError(const std::string& error);

        /**
          * addItem
          * 
          * Adds the item and its range to the display area.
          *
          * @param index - index of item
          * @param name - name of item
          * @param lowRange - minimum value for the item
          * @param highRange - maximum value for the item
          * @param unitLabel - engineering unit label
          * @param precision - number of decimal places to display the value
          */
        virtual void addItem(int index, const std::string& name, double lowRange, double highRange, 
            const std::string& unitLabel, int precision);

        /**
          * removeItem
          * 
          * Removes the item and its range from the display area.
          *
          * @param name - name of item
          */
        virtual void removeItem(const std::string& name);

        /**
          * setStartEndTime
          * 
          * Sets the start and end times on the GUI beneath the graph.
          *
          * @param startTime - start time
          * @param endTime - end time
          */
        virtual void setStartEndTime(time_t startTime, time_t endTime);

        /**
          * setCursorTime
          * 
          * Sets the cursor time in the display area.
          *
          * @param cursorTime - series number for the item
          */
        virtual void setCursorTime(time_t cursorTime);

        /**
          * setCursorTime
          * 
          * Sets the value at the cursor in the display area.
          *
          * @param index - index for the item
          * @param value - value at the cursor
          * @param precision - number of decimal places to display the value
          * @param unitLabel - engineering unit label
          */
        virtual void setCursorValue(int index, double value, int precision, const std::string& unitLabel);

        /**
          * resetDefaultAxis
          * 
          * Resets the default (percentage) axis.
          */
        virtual void resetDefaultAxis();

        /**
          * setOffsetTime
          * 
          * Sets the offset time in the display area.
          *
          * @param offsetTime - series number for the item
          */
        virtual void setOffsetTime(time_t offsetTime);

        /**
          * setOffsetValue
          * 
          * Sets the value at the cursor in the display area.
          *
          * @param index - index for the item
          * @param value - value at the cursor
          * @param precision - number of decimal places to display the value
          * @param offsetValue - value at the offset cursor
          * @param unitLabel - engineering unit label
          */
        virtual void setOffsetValue(int index, double value, double offsetValue, int precision, const std::string& unitLabel);

        /**
          * serverDown
          * 
          * Called when the server is down.
          */
        void serverDown();

        /**
          * serverUp
          * 
          * Called when the server has come back up.
          */
        void serverUp();


    protected:
        // Generated message map functions
        //{{AFX_MSG(CHistoryViewerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnNowCheck();
        afx_msg void OnToggleNowCheck();
        afx_msg void OnTrends();
        afx_msg void OnProperties();
        afx_msg void OnCursor();
        afx_msg void OnNew();
        afx_msg void OnRefresh();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnZoomGraph();
        afx_msg void OnUndoZoomGraph();
        afx_msg void OnScrollGraph();
        afx_msg void OnPrintGraph();
        afx_msg void OnExport();
        afx_msg void OnClose();
        afx_msg void OnOK();
        afx_msg void OnAppAbout();
        afx_msg void OnHelp();
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam = 0L);
        afx_msg LRESULT OnPopMessageString(WPARAM wParam, LPARAM lParam);
        afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
        afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
        afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
        afx_msg void OnAllowScroll(long Axis, double FAR* AMin, double FAR* AMax, BOOL FAR* AllowScroll);
        afx_msg void OnCursorToolChange(long Tool, long X, long Y, double XVal, double YVal, long Series, long ValueIndex);
        afx_msg void OnClickSeries(long SeriesIndex, long ValueIndex, long Button, long Shift, long X, long Y);
        afx_msg void OnClickBackground(long Button, long Shift, long X, long Y);
        afx_msg void OnClickLegend(long Button, long Shift, long X, long Y);
        afx_msg void OnOnStartDragTchart2();
        afx_msg void OnMouseDown(long Button, long Shift, long X, long Y);
        afx_msg void OnMouseUp(long Button, long Shift, long X, long Y);
	    afx_msg void OnGetAxisLabel(long Axis, long SeriesIndex, long ValueIndex, BSTR FAR* LabelText);
        DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
        // Loads new samples from the database if the same history type has been updated.
        afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnStatusBarCheck();
        afx_msg void OnToolBarCheck();
        afx_msg void OnUpdateStatusBarMenu(CCmdUI* pCmdUI);
        afx_msg void OnUpdateToolBarMenu(CCmdUI* pCmdUI);
        afx_msg LRESULT OnProcessErrorMessage(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnLoadItems(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnLoadingComplete(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnLaunchTrendsDialog(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CHistoryViewerDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        BOOL DestroyWindow();

    private:
        
        CHistoryViewerDlg(const CHistoryViewerDlg&);
        CHistoryViewerDlg& operator=(const CHistoryViewerDlg&);

        void loadItems();
        void parseParameters();
        void setUpGraph();
        void mapDisplayAreaResources();
        void saveUserSettings(std::map<std::string, std::string>& userSettings);
        void loadUserSettings(TA_Base_Bus::SettingsMgr& settingMgr);
        void restoreUserSettings();
        void createToolbar();
        void resizeGraph(int cx, int cy);
        void resizeGraph();
        void resizeDisplayArea(const CRect& oldDisplayAreaRect, const CRect& newDisplayAreaRect, bool firstTime);
        void addTextToButton(UINT buttonId, const CString& buttonLabel);
        void enableMenuItem(const CString& menuName, const UINT& menuItemID, bool enable);
        void setMenuItemCheck(const CString& menuString, const UINT& menuItemID, bool bChecked);
        int findMenuItem(const CString& menuName);
        void loadTrendTemplate(const TrendProperties& trendProperties, bool refresh = false);
        void setTransActiveResizingProperties();
        bool teeChart6IsInstalled();
        void setUpDisplayArea();
        void setAccessControlledButtons();
        void reloadDisplayArea();
        void resetCursor();
        void resetZoom();
        void setLiveMode(bool isLive, bool onFirstLoad = false); //TD17935
        void setComparisonSettings(bool display);
        void setXYSettings(bool graphTypeChanged);
        void setSpecialSettings(EGraphType graphType);
        void addItems(const std::map<std::string, TA_Base_Core::ItemInfo>& items);
        void initialiseMenuItems();
        // liborm 06/01/2005 - fix for TD#7900
        void checkArchivedDataNeeded(time_t startTime, long rangeSeconds);
        
    private:

        int m_nIDTracking;
        int m_nIDLastMessage;                                   
        bool m_loadingComplete;                                 // Indicates that all trendable items have been loaded
        std::string m_sessionId;                                // Current session Id
        boost::shared_ptr<IGraph> m_graph;                      // Pointer to object that manages the graph
        std::auto_ptr<IItemStore> m_itemStore;                  // Pointer to object that stores all the trendable items
        std::auto_ptr<IArchiveDataStore> m_archiveDataStore;    // Pointer to object that stores all the restored data dates
        TrendProperties m_currentTrendProperties;               // Properties of the current template displayed on the graph
        CDlgToolBar m_toolBar;                                  // Toolbar object
        CDlgStatusBar m_wndStatusBar;                           // Status bar object
        bool m_serverDown;                                      // Indicates that the server has gone down
        static TA_Base_Core::ReEntrantThreadLockable m_templateLock;

        typedef struct
        {
            int itemIndex;      // Index of item
            int displayIndex;   // Index in display area
            std::string name;   // Name of item
            std::string range;  // Range of item displayed as a string in the format: [<min>, <max>]
        } DisplayItems;

        std::vector<DisplayItems> m_displayItems;               // Items shown in the display area (right of chart)

        std::map<int, int> m_itemNameResourceIds;               // Resource Ids for Item Name Statics
        std::map<int, int> m_itemRangeResourceIds;              // Resource Ids for Item Range Statics
        std::map<int, int> m_itemValueResourceIds;              // Resource Ids for Item Value Statics

        long m_cursorToolIndex;                                 // Index of cursor tool
        long m_leftChartMargin;                                 // Original position of the chart's left margin
        bool m_zoomed;                                          // Indicates whether the graph is currently zoomed
        long m_axisScrolled;                                    // Index of the axis along which the graph has been scrolled

        long m_mouseDownX;                                      // X Coords for top left drag rectangle for zoom
        long m_mouseDownY;                                      // Y Coords for top left drag rectangle for zoom
        long m_mouseUpX;                                        // X Coords for bottom right drag rectangle for zoom        
        long m_mouseUpY;                                        // Y Coords for bottom right drag rectangle for zoom        

        bool m_useCmdLineParams;                                // Indicates that valid command line parameters have been
                                                                // set and the graph should be displayed based on these.

        long m_arrowToolIndex;                                  // Index for the scroll arrows

        bool m_singleItemDisplayed;                             // Indicates only one item is graphed

        bool m_preloadPropertyItems;                            // Indicates whether the current property items should be
                                                                // preloaded while the TV is waiting for all the items
                                                                // to be loaded
		TA_Base_Bus::RightsLibrary* m_rightsLibrary;

		CTrendsDialog* m_trendsDlg; // TD12625 azenitha

    protected:

        HICON m_hIcon;                                          // GUI icon

    public:

        // Dialog Data
        //{{AFX_DATA(CHistoryViewerDlg)
        enum { IDD = IDD_HISTORYVIEWER_DIALOG };
        CButton    m_displayArea;                               // Display area which shows the names of the items that
                                                                // have been added to the graph, their ranges and the 
                                                                // current value at the cursor when the cursor is displayed.
        
        CStatic    m_endTimeCtrl;                               // Displays the time at the right-most side of the graph
        
        CStatic    m_startTimeCtrl;                             // Displays the time at the left-most side of the graph
        
        CTChart    m_chartCtrl;                                 // Chart object. This object contains the graph.
        //}}AFX_DATA

        bool m_nowCheck;                                        // Indicates that graph is currently in live mode
        bool m_displayCursor;                                   // Indicates the cursor is currently displayed

        bool m_resetGraph;
        
        DWORD m_lastDisplayTime;

	private: 
		TA_Base_Core::DateFormat m_DateFormat; // TD12474

		//TD17529++		
		int m_maxTrendsDisplayed; 
		//++TD17529
		//TD18095, jianghp, to fix the performance of showing manager application
		TA_Base_Bus::IGUIAccess* m_controlClass;
    };

} // TA_Base_App


#endif // !defined(AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_)





