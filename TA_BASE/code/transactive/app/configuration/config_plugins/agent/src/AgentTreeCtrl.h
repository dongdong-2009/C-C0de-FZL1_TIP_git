/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentTreeCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a tree control that displays agents (primarily) and some other entities.
  */

#if !defined(AFX_AGENTTREECTRL_H__68A8D6A1_12BA_4AC7_B0E4_85F5BFF4D48B__INCLUDED_)
#define AFX_AGENTTREECTRL_H__68A8D6A1_12BA_4AC7_B0E4_85F5BFF4D48B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/mfc_extensions.h"

namespace TA_Base_App
{
    class AgentDatabaseAccessor;

    class AgentTreeCtrl : public CObjectTreeCtrl
    {
    public:

        /**
         * Constructor
         *
         * @param AgentDatabaseAccessor& - This is used to retrieve data for populating the tree. This must
         *                                 also be passed to AgentTreeItem's that are created.
         */
	    AgentTreeCtrl(AgentDatabaseAccessor& db);


        /**
         * Destructor
         */
        virtual ~AgentTreeCtrl();


        /** 
         * populateTree
         *
         * This will populate the tree with the top level of items
         */
        void populateTree();


        /**
         * setInitialSelection
         *
         * This selects the specified item in the tree.
         *
         * @param unsigned long - The unique identifier of the item to select
         */
        void setInitialSelection(unsigned long identifierOfItemToSelect);


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
	    //{{AFX_VIRTUAL(AgentTreeCtrl)
	    //}}AFX_VIRTUAL


    protected:
	    //{{AFX_MSG(AgentTreeCtrl)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
        // The database accessor used to retrieve items from the database to populate the tree
        AgentDatabaseAccessor& m_databaseAccessor;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_AGENTTREECTRL_H__68A8D6A1_12BA_4AC7_B0E4_85F5BFF4D48B__INCLUDED_)
