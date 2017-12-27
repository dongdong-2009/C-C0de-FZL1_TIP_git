/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/sound_file/src/SoundFileDetailView.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an sound file's details.
  */

#if !defined(AFX_SoundFileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_SoundFileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/sound_file/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigSoundFile;
}


namespace TA_Base_App
{
    class SoundFileDatabaseAccessor;
    class SoundFileListView;


    class SoundFileDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    SoundFileDetailView();


        /**
         * Destructor
         */
	    virtual ~SoundFileDetailView();

        
        DECLARE_DYNCREATE(SoundFileDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(SoundFileDetailView)
	    enum { IDD = IDD_SOUNDFILE_VIEW };
	    CButton	m_type;
    	CTransparentImage	m_bitmapDescription;
	    CEdit	m_editDescription;
    	CTransparentImage	m_bitmapSoundFile;
	    CEdit	m_editSoundFile;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
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
	    //{{AFX_VIRTUAL(SoundFileDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(SoundFileDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditDescription();
	    afx_msg void onButtonSoundFile();
	    afx_msg void onKillfocusEditSoundFile();
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
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::SOUND_FILE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        SoundFileDetailView( const SoundFileDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        SoundFileDetailView& operator=(const SoundFileDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the sound file details. These controls will be
         * hidden until the user selects a alarm plan association from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:
        static const int DESCRIPTION_FIELD_LENGTH;
        static const int SOUNDFILE_FIELD_LENGTH;

        TA_Base_Core::IConfigSoundFile* m_currentSoundFile; // The current sound file the user
                                                            // is configuring. This is the database object
                                                            // for it.

        SoundFileListView* m_listView; // This is the view in the top pane of the splitter
                                       // showing the list of all sound files. We need to
                                       // use this to tell it when sound file details have
                                       // changed.

        SoundFileDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                        // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_SoundFileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
