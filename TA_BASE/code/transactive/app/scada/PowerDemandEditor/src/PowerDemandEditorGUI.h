/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditorGUI.h $
  * @author:  Wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the managed process interface for the application.  This
  * handles the run parameters and the monitoring of changes passed from the control station.
  */

#ifndef POWERDEMANDEDITORGUI_H
#define POWERDEMANDEDITORGUI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_App
{
    class PowerDemandEditorGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
	  /**
		* Constructor
		*
		*/
		PowerDemandEditorGUI();

	  /**
		* Destructor
		* 
		*/
		virtual ~PowerDemandEditorGUI();

		//TD18095, jianghp
		virtual void onInitGenericGUICompleted();

	protected:

	  /**
		* createApplicationObject
		*
		* This method must set the m_application member variable. It should create a new
		* instance of the dialog/document/property sheet and assign it to m_application
		*/
		virtual void createApplicationObject();

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
		* saveAllUserSettings
		*
		* This will save all the application's user preferences if configured to do so.
		* This is provided in case the GUI needs to save user preferences at a predetermined
		* time. The settings are already saved when the application exits.
		*
		*/		
		virtual void saveAllUserSettings();


	  /**
		* loadAllUserSettings
		*
		* This will load all the application's user preferences if configured to do so.
		* This should be called by the application itself in OnInit() or equivalent. 
		*
		*/
		virtual void loadAllUserSettings();


	  /**
		* serverIsDown
		*
		* This is called when the server the workstation is using goes down. The GUI should
		* indicate to the user that the server is down. It should also then limit operations
		* so that the user can only view existing data. This must be implemented by the lowest
		* level class as it knows what to do when this occurs.
		*/
		virtual void serverIsDown();


	  /**
		* serverIsUp
		*
		* This is called when the server the workstation is using starts functioning again.
		* The GUI should indicate to the user that the server is functioning. It should 
		* then allow the user to perform all valid operations again. This must be implemented by
		* the lowest level class as it knows what to do when this occurs.
		*/
		virtual void serverIsUp();

	  
	  /**
		* entityChanged
		*
		* This will be called by GenericGUI when it receives a callback indicating that the
		* GUI entity has been modified. GenericGUI will have invalidated the entity database
		* object and will tell the GUI which items were modified. The GUI just has to retrieve
		* the details out of it and update accordingly. This should be implemented by the
		* lowest level class as it knows exactly what to do with the updates.
		*
		* @param vector<string>& changes - This is a list of all attributes changed in the
		*                                  object.
		*
		*/
		virtual void entityChanged(const std::vector <std::string>& changes);


	  /**
		* checkCommandLine
		*
		* This checks that all the appropriate RunParams have been set from the command line.
		* If anything is invalid or missing then this will throw an exception. This must be
		* implemented in the lowest level class as this will be where the GUI specific
		* command line details are known. All settings SHOULD be in the GUI entity and the
		* only additional command line parameter should be --control-entity.
		*
		* @exceptions GenericGUIException with the type set to COMMAND_LINE_INVALID
		*             It must throw this if the command line does not contain the correct
		*             information for the GUI to run.
		*/
		virtual void checkCommandLine();


	  /**
		* checkEntity
		* 
		* This will check that the entity is valid and then store a local pointer to the
		* entity. This must be implemented in the lowest level class as this will be where
		* the GUI specific configuration details are known.
		*
		* @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
		*
		* @exceptions GenericGUIException with the type set to ENTITY_CONFIGURATION_INVALID
		*             This should be thrown if the application checks the entity and it is
		*             not configured correctly for the GUI to run.
		*
		*/
		virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);


	  /**
		* getApplicationType
		* 
		* This returns the application type for this GUI. GenericGUI will then save
		* this in the RunParams and will use it to pass to the Control Station. In the
		* existing system the application type would need to be hard-coded into here.
		* This method must be implemented by the lowest level class.
		*
		* @return unsigned long representing the application type
		*
		*/
		virtual unsigned long getApplicationType();




	private:

	 
	};
}
#endif // POWERDEMANDEDITORGUI_H
