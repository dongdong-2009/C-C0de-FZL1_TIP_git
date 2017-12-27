/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasDetailView.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ScadaAliasRecord's details.
  */

#if !defined(AFX_ScadaAliasDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ScadaAliasDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/scada_alias/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigOpcTagAlias;
}


namespace TA_Base_App
{
    class ScadaAliasDatabaseAccessor;
    class ScadaAliasListView;


    class ScadaAliasDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ScadaAliasDetailView();


        /**
         * Destructor
         */
	    virtual ~ScadaAliasDetailView();

        
        DECLARE_DYNCREATE(ScadaAliasDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ScadaAliasDetailView)
	    enum { IDD = IDD_SCADAALIAS_VIEW };
	    CButton	m_attributeBtn;
	    TA_Base_Bus::TextColourEdit	m_entityNameEdt;
	    TA_Base_Bus::TextColourEdit	m_opcAliasEdt;
	    TA_Base_Bus::TextColourEdit	m_attributeEdt;
    	CButton	            m_applyBtn;
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;
	//}}AFX_DATA
       

        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);

        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaAliasDetailView)
	public:
	    virtual void OnInitialUpdate();
    	virtual void OnButtonAttribute();

	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(ScadaAliasDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();
	    afx_msg void OnKillfocusEditOpcAlias();
	    afx_msg void OnKillfocusEditEntityName();
	    afx_msg void OnKillfocusEditAttribute();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::OPC_TAG_ALIAS; //todo: use the correct msg type
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ScadaAliasDetailView( const ScadaAliasDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ScadaAliasDetailView& operator=(const ScadaAliasDetailView&);

	    /**
	      * checkForValidity
	      * 
	      * check for the validity of the current record
          * 1). alias name must be unique; 2). entityname + alias name must be unique
	      * 3). entity name must not equal to alias name
	      * @return 
	      * @param : unsigned long
	      * 
	      * @exception <exceptions> Optional
	      */
        bool checkForValidity(unsigned long itemid);
        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ScadaAliasRecord details. These controls will be
         * hidden until the user selects a ScadaAliasRecord from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);    

    private:
        
        static const CString CHANGE_ATTRIBUTE_DIALOG_TITLE;
        static const int OPC_ALIAS_FIELD_LENGTH;
        static const int ENTITY_NAME_FIELD_LENGTH;
        static const int ATTRIBUTE_FIELD_LENGTH;
        
        TA_Base_Core::IConfigOpcTagAlias* m_configItem; // The current ScadaAliasRecord the user
                                                              // is configuring. This is the database object
                                                              // for it.

        ScadaAliasListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all ScadaAliasRecord. We need to
                                            // use this to tell it when ScadaAliasRecord details have
                                            // changed.

        ScadaAliasDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ScadaAliasDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
