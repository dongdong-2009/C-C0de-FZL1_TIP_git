// PlanActiveInstanceListCtrl.h : header file
//

#if !defined(AFX_PLANACTIVEINSTANCELISTCTRL_H__8665FFF8_121D_4ADE_9EDA_2246C513C4B2__INCLUDED_)
#define AFX_PLANACTIVEINSTANCELISTCTRL_H__8665FFF8_121D_4ADE_9EDA_2246C513C4B2__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.h $
  * @author:  Bart Golab / Rob Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 18:34:29 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Customised CListCtrl to display ActivePlanInstances.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/active_plans_display/src/ActivePlansDisplayCommonDefs.h"


namespace TA_Base_Bus
{
    class IPlanInstanceFilter;
    class IActivePlanAccess;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanActiveInstanceListCtrl window

    class CPlanActiveInstanceListCtrl : public CListCtrl
    {
    public:
        // Support initialization language switch
        static std::string PLAN_COLUMN_LABEL;			    // Column 1 - Plan
        static std::string CATEGORY_COLUMN_LABEL;	    // Column 2 - Category
        static std::string INSTANCE_COLUMN_LABEL;	    // Column 3 - Instance
        static std::string LOCATION_COLUMN_LABEL;       // Column 4 - Location
        static std::string OWNER_COLUMN_LABEL;			    // Column 5 - Owner
        static std::string ACTIVATION_COLUMN_LABEL;	// Column 6 - Activation time
        static std::string STATE_COLUMN_LABEL;		    	// Column 7 - State
        static std::string STEP_COLUMN_LABEL;		    	// Column 8 - Step
        static std::string REMARKS_COLUMN_LABEL;		    // Column 9 - Remarks

	// Construction/Destruction
    public:
		/** 
		  * Constructor
		  *
		  * This constructs an instance of CPlanActiveInstanceListCtrl.
		  *
		  * @param bool planPathVisible	  -	Specifies whether to show the Plan Name and Category Path columns
          *                                 (true = yes | false = no) 
		  */
        CPlanActiveInstanceListCtrl(bool planPathVisible = false);

		/** 
		  * Destructor
		  *
		  */
        virtual ~CPlanActiveInstanceListCtrl();


    // Operations
    public:
		/** 
		  * SetReadOnly
		  *
		  * Set the CPlanActiveInstanceListCtrl instance's read only state.
		  *
		  * @param bool readOnly	-	(true = read-only | false = not read-only) 
		  *								(default value = true)
		  * 
		  * @return bool			-	Whether specified mode successfully applied
		  *								(true = success | false = failure)
		  */
        virtual bool SetReadOnly(bool readOnly = true);

		/** 
		  * SetPlanInstanceFilter
		  *
		  * Enable plan instance filtering according to the specified filter. 
          * If NULL filtering is disabled. 
		  *
		  * @param IPlanInstanceFilter* planInstanceFilter	- filter used to determine which plan instances
          *                                                   should be shown in the list
		  * 
		  */
        virtual void SetPlanInstanceFilter(IPlanInstanceFilter* planInstanceFilter);

		/** 
		  * SetActivePlanAccess
		  *
		  * Provide access to active plan details from Plan Agent. 
          * If NULL the list will not populate itself. 
		  *
		  * @param IActivePlanAccess* activePlanAccess	- plan agent access for active plan instances
		  * 
		  */
        virtual void SetActivePlanAccess(IActivePlanAccess* activePlanAccess);

		/** 
		  * AutoAdjustColumns
		  *
		  * Scale list columns to fit the current width of the control.
          * The following columns are adjusted: Plan, Category, Step and Remarks.
		  *
		  */
        virtual void AutoAdjustColumns();

		/** 
		  * Populate
		  *
		  * Populate the CPlanActiveInstanceListCtrl instance from container of 
		  * ActivePlanDetails.
		  *
		  * @param const TA_Base_Core::ActivePlanDetails &activePlanDetails	-	reference to const
		  *																	container of ActivePlanDetails
		  *
		  */
        virtual void Populate(const TA_Base_Core::ActivePlanDetails &activePlanDetails);

		/** 
		  * PopulateInstance
		  *
		  * Populate the CPlanActiveInstanceListCtrl instance from ActivePlanDetail 
		  * (update if exists, else add to list). This method will store the detail in queue
		  * to be processed later if the list has not completed initialisation yet. Else it
		  * will call the helper method ProcessInstance to insert/apply to correct position in list.
		  *	
		  *
		  *
		  * @param const TA_Base_Core::ActivePlanDetail &activePlanDetail	-	reference to const
		  *																	ActivePlanDetail for update
		  *																	or addition to list				
		  *
		  */
        virtual void PopulateInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);


		/** 
		  * Refresh
		  *
		  * Reload the contents of the list based on the current filtering rules. 
          * Does nothing if m_activePlanAccess has not been set.
          *
		  */        
        virtual void Refresh();

		/** 
		  * Purge
		  *
		  * Remove all instances that do not satisfy the current filtering rules.
          *
		  */
        virtual void Purge();

		/** 
		  * GetPlanInstanceId
		  *
		  * Returns the plan ID, instance number and location key stored as part of the specified item's data.
		  *
		  * @param int item		-	The list position of Plan Instance			
		  * @return				-	Identity of the instance
		  *
		  */
        virtual TA_Base_Core::ActivePlanId GetPlanInstanceId(int item);

		/** 
		  * GetSelectedPlanInstanceId
		  *
		  * Returns the plan ID, instance number and location key of the selected instance.
		  *
		  * @return	-	Identity of the currently selected instance. If none selected all members are -1.
		  *
		  */
        virtual TA_Base_Core::ActivePlanId GetSelectedPlanInstanceId();


    protected:
		/** 
		  * ProcessInstance
		  *
		  * Helper method to actually populate the CPlanActiveInstanceListCtrl instance from ActivePlanDetail 
		  * (update if exists, else add to list, else remove if expired).
		  *
		  * @param const TA_Base_Core::ActivePlanDetail &activePlanDetail	-	reference to const
		  *																	ActivePlanDetail for update
		  *																	or addition to list				
		  *
		  */
		virtual void ProcessInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);

 		/** 
		  * InitialiseControl
		  *
		  * Helper method to set the extended style and columns of the List Control
		  * so as to be suited to the display of Plan Instances. Protected Virtual, so any
		  * further derivations of this class can add further initialisation needed. This 
		  * method is called by this class when posted message WM_ACTIVE_PLANS_LIST_INITIALISE
		  * is handled. The WM_ACTIVE_PLANS_LIST_INITIALISE is posted by this class during PreSubclassWindow
		  * to ensure initialisation tasks are performed when wnd is ready. Note: We have to use posted message,
		  * as these initialsation tasks are not possible during PreSubclassWindow() when this wnd is created
		  * dynamically (i.e. using Create(...) instead of just assigning from resource with class wizard).
		  * After initialising the List this method also process any of the active plan details stored in the update queue
		  * (as any updates received before initialisation completes are buffered there).
		  *
		  */       
		virtual void InitialiseActivePlanList(); // set to report view and add the columns


        /**
          * IsInitialised
          *
          * Indicates whether the list is ready for inserting active plan entries.
          *
          */
        virtual bool IsInitialised() const;

		/** 
		  * SortActivePlanList
		  *
		  * Helper method to wrap sorting of Plan List. Will retun without action if list has not
		  * yet completed initialisation. 
		  *
		  */
        virtual void SortActivePlanList(); // sort the list based on m_columnSortData and m_sortAscending state members
	
		/** 
		  * DeleteInstance
		  *
		  * Removes from the list the instance at the specified index. Selection highlight is moved onto the
          * next item if necessary.
		  *
		  * @param int item     -    zero based index into the list
		  *
		  */	
        virtual void DeleteInstance(int item);

		/** 
		  * FindInstance
		  *
		  * Helper method to find the item corresponding to the specified plan ID, instance number and location key.
		  *
		  * @param const TA_Base_Core::ActivePlanId &activePlanId   -	identity of the instance to be located 		
		  *
          * @return -   The position of the corresponding list item, or -1 if the search failed.
		  */
        virtual int FindInstance(const TA_Base_Core::ActivePlanId &activePlanId);

		// helper function only used by this class to aid in custom sorting.
		static int CALLBACK ComparePlanInstances(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


    // Attributes
    private:
		//
		// Indicates whether the Plan and Category columns should be visible 
		//
        bool m_planPathVisible;

		//
		// Indicates whether this control is created dynamically through a call to Create().
		//
        bool m_isDynamicallyCreated;

 		//
		// Indicates wheter ListCtrl has been initialsed so as to be ready to
		// populate with Active Plan Instance Data
		//
		bool m_initialised;

        //
        // Used to control which entries get displayed in the list
        //
        IPlanInstanceFilter* m_planInstanceFilter;

        //
        // Used to retrieve active plan instances from Plan Agent
        //
        IActivePlanAccess* m_activePlanAccess;


    // Overrides
 	protected:
       // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CPlanActiveInstanceListCtrl)
        virtual void PreSubclassWindow();
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	    //}}AFX_VIRTUAL

    // Implementation        
        // Generated message map functions
    protected:
        //{{AFX_MSG(CPlanActiveInstanceListCtrl)
    	afx_msg void OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnDeleteAllItems(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    afx_msg void OnDestroy();
	    //}}AFX_MSG

		//
		// Message handler for WM_ACTIVE_PLAN_DETAIL_UPDATE.
		// Note: client code must send message to this class using valid 
		// pointer to ActivePlanDetail as WPARAM.
		//
		afx_msg LRESULT OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam);
		
		//
		// Message handler for posted message to self for initialisation
		//
		afx_msg void OnActivePlanListInitialise();

        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANACTIVEINSTANCELISTCTRL_H__8665FFF8_121D_4ADE_9EDA_2246C513C4B2__INCLUDED_)
