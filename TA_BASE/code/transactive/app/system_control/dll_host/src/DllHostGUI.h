/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/dll_host/src/DllHostGUI.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * The class, DllHostGUI, provides the AbstractDialogGUI for this application.
  * The app uses the GenericGUI framework.
  *
  */

#if !defined(AFX_BANNERGUI_H__2A63FC16_95CE_48E6_9464_A662B7478D49__INCLUDED_)
#define AFX_BANNERGUI_H__2A63FC16_95CE_48E6_9464_A662B7478D49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"


namespace TA_Base_App
{

    class DllHostGUI : public TA_Base_Bus::AbstractDialogGUI
    {
    public:

	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    DllHostGUI();


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual ~DllHostGUI();


		//TD18095, jianghp
		virtual void onInitGenericGUICompleted();


	    /**
	    * saveAllUserSettings
	    * 
	    * This method does nothing.
	    * The method definition is needed to instantiate the abstract base class.
	    * There are no user settings for the Banner.
	    *
	    */
	    virtual void saveAllUserSettings();


	    /**
	    * loadAllUserSettings
	    * 
	    * This method does nothing other than call setWindowPosition().
	    * The method definition is needed to instantiate the abstract base class.
	    * There are no user settings for the Banner.
	    *
	    */
	    virtual void loadAllUserSettings();


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual void serverIsDown();


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual void serverIsUp();


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual void entityChanged(const std::vector <std::string>& changes);


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual void checkCommandLine();


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);


	    /**
	    * <method / function name>
	    * 
	    * <description>
	    *
	    * @return <return value> Optional
	    *
	    * @param <parameters> Optional
	    *
	    * @exception <exceptions> Optional ( C++ only )
	    *
	    */
	    virtual unsigned long getApplicationType();

    protected:

	    /**
	    * prepareForClose
	    *
	    * This method is used to prepare the GUI for closing. It should save the user settings
	    * and perform any other tidying up. eg Stop any processing threads, ask the user if they
	    * wish to save anything etc.
	    * This should be called before any close is done on the GUI whether initiated by the
	    * GUI itself or the Control Station. This means it should be the first call in the 
	    * implementation of terminateGUI().
	    */
	    virtual void prepareForClose();

        /**
        * createApplicationObject
        *
        * This method must set the m_application member variable. It should create a new
        * instance of the dialog/document/property sheet and assign it to m_application
        */
        virtual void createApplicationObject();

    private:

        TA_Base_Core::IEntityData* m_guiEntity;
    };


} // namespace TA_Base_App

#endif // !defined(AFX_BANNERGUI_H__2A63FC16_95CE_48E6_9464_A662B7478D49__INCLUDED_)
