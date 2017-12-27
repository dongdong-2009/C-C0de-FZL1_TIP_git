/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/scada_tree/src/ScadaTreeCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ScadaTreeCtrl.h : header file
//

#if !defined(AFX_SCADATREECTRL_H__D39DAE28_0791_47F5_8A62_0F5B2C4F2302__INCLUDED_)
#define AFX_SCADATREECTRL_H__D39DAE28_0791_47F5_8A62_0F5B2C4F2302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Bus
{
    class IEntityFactory;

    /////////////////////////////////////////////////////////////////////////////
    // CScadaTreeCtrl
    
    class CScadaTreeCtrl : public CObjectTreeCtrl
    {
    // Construction
    public:
        /**
         * Constructor
         *
         * @param IEntityFactory* - This is the factory that should be used to retrieve the entity data.
         *                          If this is NULL then the EntityAccessFactory will be used instead.
         * @param bool - This indicates whether this class should delete the IEntityFactory or whether
         *               the caller will do it. true = this class will delete. false = caller must delete
         *               Default is for the caller to delete.
		 * @param bool - This indicates whether the datapoints should be displayed or only the existing
		 *				 datanodes should be displayed //TD15806
         */
	    CScadaTreeCtrl(IEntityFactory* entityRetrievalFactory = NULL, bool shouldDeleteFactory = false,
			bool bExpandAllDataNodes = true);

    // Attributes
    public:

    // Operations
    public:

	    void setInitialDataPoint(std::string dataPointName);

        /**
         * countNodes
         *
         * This counts all nodes under the tree item specified. This is a recursive function
         * call. The node specified as the parameter is not included in the count.
         *
         * @param HTREEITEM - The item to count the nodes under
         *
         * @return int - The number of nodes not including the node passed in.
         */
        int countNodes(HTREEITEM item);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CScadaTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CScadaTreeCtrl();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CScadaTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

    private:

	    void loadTreeRootItems();


	    DECLARE_MESSAGE_MAP()

        // This is the entity type that will be at the root of our tree
        static const std::string ROOT_ENTITY_TYPE;

        // This is used to retrieve the entity information. If this is NULL then we use the
        // EntityAccessFactory instead and must delete our own pointers.
        IEntityFactory* m_entityRetrievalFactory;
        bool m_shouldDeleteEntityFactory;


        // This is used to indicate when we should load items and populate the tree. If this object is created
        // statically (using a DDX control) then the order of method calls is:
        // PreSubclassWindow()   - In here we populate the tree
        //
        // If this control is created dynamically (using Create() ) then the order of method calls is:
        // PreCreateWindow() - Set this member variable to false in here
        // PreSubclassWindow() - This member variable being set to false means we don't populate in here
        // OnCreate() - Now we can finally populate the tree.
        bool m_canInitialiseInPreSubclassWindow;
		// provides an option for the CScadaTreeCtrl to view up to the datanodes, dismissing
		// the datapoints
		bool m_bExpandAllDataNodes;  //TD15806 - added since this is the kind of CScadaTreeCtrl 
									 // behavior expected from the Incident Manager
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_SCADATREECTRL_H__D39DAE28_0791_47F5_8A62_0F5B2C4F2302__INCLUDED_)
