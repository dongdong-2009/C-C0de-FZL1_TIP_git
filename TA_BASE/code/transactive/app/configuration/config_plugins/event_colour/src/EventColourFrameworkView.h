/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourFrameworkView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of event colours
  * and the right side as a view for the user to configure event colour details.
  */

// EventColourFrameworkView.h: interface for the EventColourFrameworkView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURFRAMEWORKVIEW_H__44A7C5CE_3D09_46E3_982D_C9E8EFB6E708__INCLUDED_)
#define AFX_EVENTCOLOURFRAMEWORKVIEW_H__44A7C5CE_3D09_46E3_982D_C9E8EFB6E708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h"

namespace TA_Base_App
{

	class EventColourFrameworkView : public AbstractFrameworkView  
	{

    protected:

        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    EventColourFrameworkView();

	    DECLARE_DYNCREATE(EventColourFrameworkView)


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EventColourFrameworkView)
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    virtual ~EventColourFrameworkView();
    
	#ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	// Generated message map functions
    protected:
	    //{{AFX_MSG(EventColourFrameworkView)
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         * getRuntimeClassForListView
         *
         * This returns the runtime class for the list view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForListView();


        /**
         * getRuntimeClassForDetailView
         *
         * This returns the runtime class for the detail view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
         virtual CRuntimeClass* getRuntimeClassForDetailView();
	};

} //close namespace

#endif // !defined(AFX_EVENTCOLOURFRAMEWORKVIEW_H__44A7C5CE_3D09_46E3_982D_C9E8EFB6E708__INCLUDED_)
