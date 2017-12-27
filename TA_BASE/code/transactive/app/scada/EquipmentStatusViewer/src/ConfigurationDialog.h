/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ConfigurationDialog.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is a Configuration Dialog box allowing operators to configure current 
 * equipment reports with equipment/datapoint information from a particular location
 *
 */

#if !defined(AFX_CONFIGURATIONDIALOG_H__A3CAB8E5_5723_4FF5_9097_3444151BDEC6__INCLUDED_)
#define AFX_CONFIGURATIONDIALOG_H__A3CAB8E5_5723_4FF5_9097_3444151BDEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <string>
#include <map>

#include "CfgEquipmentListCtrl.h"
#include "EquipmentTreeCtrl.h"
#include "EquipmentStatusListItem.h"
#include "LocationLimitedEntityFactory.h"

namespace TA_Base_App
{
	class CEquipmentStatusViewerDlg;

	class CConfigurationDialog : public CDialog
	{

	public:

        /** 
          * CConfigurationDialog
          *
          * Constructor
          *
          * @param locationId       Key of location the configured report refers to
          * @param reportId         Key of equipment status report to configure
          * @param locationName     Name of location (for display)
          * @param reportName       Name of report (for display)
          * @param progressBar      Main dialog's progress bar to use when loading configured report
          *                         and scada tree control
          * @param pParentWnd       The parent window this dialog belongs to.
          *
          */
        CConfigurationDialog( const unsigned long locationId, std::string& reportId,
            const std::string& locationName, const std::string& reportName,
            CProgressBar& progressBar, CWnd* pParentWnd = NULL );

        /** 
          * CConfigurationDialog
          *
          * Constructor
          *
          * @param locationId       Key of location the configured report refers to
          * @param reportId         Key of equipment status report to configure
          * @param locationName     Name of location (for display)
          * @param reportName       Name of report (for display)
          * @param progressBar      Main dialog's progress bar to use when loading configured report
          *                         and scada tree control
          * @param pParentWnd       The parent window this dialog belongs to.
          *
          */
        CConfigurationDialog( const unsigned long locationId, const std::string& locationName, std::string& reportId,const std::string& reportName,
            CProgressBar& progressBar, CWnd* pParentWnd = NULL , TA_Base_Bus::IEntityFactory* entityRetrievalFactory= NULL);

    private:

		/**
  		 * Specialised EntityAccessFactory that retrieves the SCADA tree for
         * the specific location only. This factory must be initialised before
  		 * the CScadaTreeCtrl, so must be declared before m_equipmentTree.
  		 */
        LocationLimitedEntityFactory m_entityFactory;

		//{{AFX_DATA(CConfigurationDialog)
		enum { IDD = IDD_CONFIGURATION };
		CButton					m_AddDataPtsBtn;
		CButton					m_AddDataNodeBtn;
		CButton					m_RemoveBtn;
		CStatic					m_equipmentDescription;
		CCfgEquipmentListCtrl	m_equipmentListCtrl;
		CEquipmentTreeCtrl		m_equipmentTree;
		//}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CConfigurationDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(CConfigurationDialog)
		virtual BOOL OnInitDialog();
		afx_msg void OnRemoveBtnClick();
		afx_msg void OnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnAddDatanodesClick();
		afx_msg void OnAddDatapointsClick();
		afx_msg void OnSetfocusEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnClickEquipmentTreeClick(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSetfocusEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnColumnclickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		virtual void OnOK();
		virtual void OnCancel();
	    afx_msg void OnClose();
	    afx_msg void OnItemExpandingEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnSelchangedEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	    afx_msg void OnTimer(UINT nIDEvent);
    	afx_msg LRESULT OnListItemUpdate(WPARAM wParam, LPARAM lParam);
	    //}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		bool ItemHasAtLeastOneChildDataPoint(HTREEITEM item);
		void Expand(NMHDR *pNMHDR);
		void ExamineMessageQueue();
		bool IsItemInList(unsigned long entityKey, Node_Item entityType);

	public:
		/**
  		 * SetStatusBarText()
  		 *
		 * Sets the text of the parent window's status bar
		 *
		 * @param	message		Text to display
		 *
  		 */
		void SetStatusBarText(const CString& message);

	private:
		/**
		 * InspectTreeNodeForDataNodes()
		 *
		 * Recursive function to traverse the TreeCtrl and look for datanodes and 
		 * perform the needed operations on these nodes
		 *
		 * @param	aNode	Node to start traversing from down to all its children
		 *
		 */
		void InspectTreeNodeForDataNodes(HTREEITEM aNode);

		/**
		 * InspectTreeNodeForDataPoints()
		 *
		 * Recursive function to traverse the TreeCtrl and look for datapoints and 
		 * perform the needed operations on these nodes
		 *
		 * @param	aNode	Node to start traversing from down to all its children
		 *
  		 */
		void InspectTreeNodeForDataPoints(HTREEITEM aNode);

		/**
  		 * AdjustGUI()
  		 *
		 * Changes the state of GUI controls based on the current state of the 
		 * dialog, enables/disable buttons etc.
		 *
  		 */
		void AdjustGUI();

		/**
  		 * Name of the current location
  		 *
  		 */
		std::string m_LocationName;

		/**
  		 * Name of the current report
  		 *
  		 */
		std::string m_ReportName;

		/**
  		 * Primary key of the current report
  		 *
  		 */
        std::string m_ReportId;

		/**
  		 * Primary key of the current location
  		 *
  		 */
		unsigned long m_LocationId;

		typedef std::set <unsigned long> EntityKeySet;

		/**
		 * Collection which stores the database keys and the report entity string id of all equipment/datapoints which 
		 * have been removed from the report during the execution of this dialog.
		 * If the operator selects OK, all keys in this collection will be used to delete
		 * data from the report, otherwise list will be discarded.
		 *
		 */
		std::map<unsigned long, std::string>  m_ToRemoveCollection;

		/** 
		 * this list stores the pkeys of those item that are added/removed since the
		 * dialog is open for configuration.
		 *
  		 */
		EntityKeySet m_addedRemovedItems;

		/**
		 *	PopulateEquipmentStatusList()
		 *
		 *	Populates the equipment status list with equipment/datapoint information corresponding
		 *	to the selected report.
		 *
		 *	@param	reportKey		Primary key of the report to use in populating the list
		 *
		 */
        void PopulateEquipmentStatusList(std::string& reportKey);

        /** 
          * StartStopPopulatorTimer
          *
          * Start or stop the Windows timer that is used to cycle through the list of
          * unprocessed entities and load them into the Equipment Status list.
          *
          * @param start                True = start the timer
          *                             False = stop the timer
          *
          * @param postInitialTimerMsg  True = post a timer message now 
          *
          * @return     True = the request to start has started the timer or
          *                     the request to stop has stopped the timer
          *             False = the request failed (timer already started or stopped)
          */
        bool StartStopPopulatorTimer( bool start, bool postInitialTimerMsg=false);

 		/**
		 *	Map of Datanode/point list items indexed by Entity key
		 *
		 */
        EquipmentStatusListItemMap m_reportItems;

        //  Unprocessed entities are extracted from the database and are added to the
        //  reportItems in the UI thread using a Windows timer (see OnTimer()).
        //  This vector will shrink as its entities are processed into the reportItems map.
		std::vector< EquipmentReportEntity>	m_unprocessedEntities;

		/**
		 *	Windows timer (WM_TIMER) used when populating the list control
		 *
		 */
        unsigned int    m_populatorTimer;

        CProgressBar&    m_progressBar;
        
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATIONDIALOG_H__A3CAB8E5_5723_4FF5_9097_3444151BDEC6__INCLUDED_)
