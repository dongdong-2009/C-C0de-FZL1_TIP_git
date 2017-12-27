#ifndef VIDEO_SWITCH_AGENT_FACTORY_H
#define VIDEO_SWITCH_AGENT_FACTORY_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is a singleton that provides access to the object monitored
  * by the Video Switch Agent.
  *
  * IMPORTANT: When using this class, you must make sure cleanup() is called
  * before Corba is cleaned up.
  */

#include <vector>

#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"

#include "core/data_access_interface/src/ILocation.h"

#include "core/naming/src/NamedObject.h"

// Forward declarations.

namespace TA_Base_Bus
{
    class VideoOutputGroupCorbaDef;
    class VideoOutputCorbaDef;
    class VideoInputCorbaDef;
    class VideoMonitorCorbaDef;
    class RecordingUnitCorbaDef;
    class CameraCorbaDef;
    class QuadCorbaDef;
    class BVSStageCorbaDef;
    class SequenceCorbaDef;
}

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

namespace TA_Base_Bus
{
    class VideoSwitchAgentFactory
    {
    public:

        //
        // Corba Named Obj Ref Typedefs
        //
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::VideoOutputGroupCorbaDef,
			                          TA_Base_Bus::VideoOutputGroupCorbaDef_ptr,
			                          TA_Base_Bus::VideoOutputGroupCorbaDef_var >    VideoOutputGroupNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::VideoMonitorCorbaDef,
			                          TA_Base_Bus::VideoMonitorCorbaDef_ptr,
			                          TA_Base_Bus::VideoMonitorCorbaDef_var >        VideoMonitorNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::RecordingUnitCorbaDef,
			                          TA_Base_Bus::RecordingUnitCorbaDef_ptr,
			                          TA_Base_Bus::RecordingUnitCorbaDef_var >       RecordingUnitNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::CameraCorbaDef,
			                          TA_Base_Bus::CameraCorbaDef_ptr,
			                          TA_Base_Bus::CameraCorbaDef_var >              CameraNamedObject;
     //   typedef TA_Base_Core::NamedObject< TA_Base_Bus::QuadCorbaDef,
	//		                          TA_Base_Bus::QuadCorbaDef_ptr,
	//		                          TA_Base_Bus::QuadCorbaDef_var >                QuadNamedObject;
    //    typedef TA_Base_Core::NamedObject< TA_Base_Bus::BVSStageCorbaDef,
	//		                          TA_Base_Bus::BVSStageCorbaDef_ptr,
	//		                          TA_Base_Bus::BVSStageCorbaDef_var >            BVSStageNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::SequenceCorbaDef,
			                          TA_Base_Bus::SequenceCorbaDef_ptr,
			                          TA_Base_Bus::SequenceCorbaDef_var >            SequenceNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::VideoOutputCorbaDef,
			                          TA_Base_Bus::VideoOutputCorbaDef_ptr,
			                          TA_Base_Bus::VideoOutputCorbaDef_var >         VideoOutputNamedObject;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::VideoInputCorbaDef,
			                          TA_Base_Bus::VideoInputCorbaDef_ptr,
			                          TA_Base_Bus::VideoInputCorbaDef_var >          VideoInputNamedObject;
		
		typedef TA_Base_Core::NamedObject< TA_Base_Bus::SwitchManagerCorbaDef,
									  TA_Base_Bus::SwitchManagerCorbaDef_ptr,
									  TA_Base_Bus::SwitchManagerCorbaDef_var >		SwitchManagerNamedObject;
        
        //
        // Vectors of pointers to Corba Named Obj Refs Typedefs
        //
        typedef std::vector< VideoOutputGroupNamedObject* >    VideoOutputGroupVector;
        typedef std::vector< VideoMonitorNamedObject* >        VideoMonitorVector;
        typedef std::vector< RecordingUnitNamedObject* >       RecordingUnitVector;
        typedef std::vector< CameraNamedObject* >              CameraVector;
      //  typedef std::vector< QuadNamedObject* >                QuadVector;
      //  typedef std::vector< BVSStageNamedObject* >            BVSStageVector;
        typedef std::vector< SequenceNamedObject* >            SequenceVector;
        typedef std::vector< VideoOutputNamedObject* >         VideoOutputVector;
        typedef std::vector< VideoInputNamedObject* >          VideoInputVector;

		/**
          * getInstance
          *
          * Gets the one and only VideoSwitchAgentFactory.
          *
          * @return     VideoSwitchAgentFactory&
          *             The only VideoSwitchAgentFactory.
          */

		static VideoSwitchAgentFactory& getInstance();

        /**
          * ~VideoSwitchAgentFactory
          *
          * Standard destructor.
          */

        virtual ~VideoSwitchAgentFactory() {};

        /**
          * cleanup
          *
          * Cleans up the NamedObject stored within this class.  This method must be
          * called before Corba is cleaned up.
          *
          * For GenericGui's that will be in onPrepareForClose() in the class
          * that inherits from AbstractDialog or whatever, before the Corba cleanup,
          * which is done extra last.
          */
        
        void cleanup();

        /**
          * getVideoOutputGroups
          *
	      * Retrieves the list of VideoOutputGroups. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoOutputGroupVector 
          *             The vector of Corba Named Obj Ref pointers to the VideoOutputGroups.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoOutputGroupVector getVideoOutputGroups();

	    /**
          * getVideoOutputGroup
          *
	      * Retrieves an VideoOutputGroupNamedObject by entity key.
	      * VideoOutputGroup is a video output group, 
          * such as a video wall monitor group, or a wallboard.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     VideoOutputGroupNamedObject*
          *             The requested VideoOutputGroupNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the VideoOutputGroupNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoOutputGroupNamedObject* getVideoOutputGroup( unsigned long entityKey );

        /**
          * getVideoOutputsForGroup
          *
	      * Retrieves the list of VideoOutputs contained within a particular VideoOutputGroup. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoOutputVector 
          *             The vector of Corba Named Obj Ref pointers to the VideoOutputs.
		  *
          * @param      unsigned long entityKey
          *             The entity key of the VideoOutputGroup.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          *             4. The group information could not be retrieved from the database.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoOutputVector getVideoOutputsForGroup( unsigned long entityKey );

        /**
          * getVideoMonitors
          *
	      * Retrieves the list of VideoMonitorsNamedObject. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoMonitorVector 
          *             The vector of Corba Named Obj Ref pointers to the VideoMonitors.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoMonitorVector getVideoMonitors();

	    /**
          * getVideoMonitor
          *
	      * Retrieves an VideoMonitorNamedObject by entity key.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     VideoMonitorNamedObject*
          *             The requested VideoMonitorNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the VideoMonitorNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoMonitorNamedObject* getVideoMonitorNamedObject( unsigned long entityKey );

        /**
          * getConsoleMonitors
          *
	      * Retrieves the list of VideoMonitors that are workstation monitors. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoMonitorVector 
          *             The vector of Corba Named Obj Ref pointers to the workstation VideoMonitors.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoMonitorVector getConsoleMonitors();
        
        /**
          * getLocalConsoleMonitors
          *
	      * Retrieves the list of VideoMonitors that are connected to the local workstation. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoOutputVector 
          *             The vector of Corba Named Obj Ref pointers to the workstation VideoMonitors.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          *             4. The local workstation name could not be determined.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::VideoMonitorVector getLocalConsoleMonitors();

        /**
          * getRecordingUnits
          *
	      * Retrieves the list of RecordingUnits. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     RecordingUnitVector 
          *             The vector of Corba Named Obj Ref pointers to the RecordingUnits.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::RecordingUnitVector getRecordingUnits();

        /**
          * getLocalConsoleRecordingUnits
          *
	      * Retrieves the list of RecordingUnits that are connected to the local console. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     RecordingUnitVector 
          *             The vector of Corba Named Obj Ref pointers to the console RecordingUnits.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          *             4. The local workstation name could not be determined.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoSwitchAgentFactory::RecordingUnitVector getLocalConsoleRecordingUnits();

	    /**
          * getRecordingUnit
          *
	      * Retrieves an RecordingUnitNamedObject by entity key.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     RecordingUnitNamedObject*
          *             The requested RecordingUnitNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the RecordingUnitNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        RecordingUnitNamedObject* getRecordingUnit( unsigned long entityKey );

        /**
          * getCameras
          *
	      * Retrieves the list of Cameras. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     CameraVector 
          *             The vector of Corba Named Obj Ref pointers to the Cameras.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        CameraVector getCameras();

        /**
          * getPTZCameras
          *
	      * Retrieves the list of PTZ Cameras. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     CameraVector 
          *             The vector of Corba Named Obj Ref pointers to the Cameras.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        CameraVector getPTZCameras();
		/**
          * getCamera
          *
          * It is the caller's responsibility to clean up the returned object.
	      * Retrieves an CameraNamedObject by entity key.
          *
          * @return     CameraNamedObject*
          *             The requested CameraNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the CameraNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        CameraNamedObject* getCamera( unsigned long entityKey );

        /**
          * getQuads
          *
	      * Retrieves the list of Quads. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     QuadVector 
          *             The vector of Corba Named Obj Ref pointers to the Quads.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

       // QuadVector getQuads();

	    /**
          * getQuad
          *
	      * Retrieves an QuadNamedObject by entity key.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     QuadNamedObject*
          *             The requested QuadNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the QuadNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

       // QuadNamedObject* getQuad( unsigned long entityKey );

        /**
          * getBVSStages
          *
	      * Retrieves the list of BVSStages. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     BVSStageVector 
          *             The vector of Corba Named Obj Ref pointers to the BVSStages.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

     //   BVSStageVector getBVSStages();

        /**
          * getSequences
          *
	      * Retrieves the list of Sequences. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     SequenceVector 
          *             The vector of Corba Named Obj Ref pointers to the Sequences.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        SequenceVector getSequences();

	    /**
          * getSequence
          *
	      * Retrieves an SequenceNamedObject by entity key.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     SequenceNamedObject*
          *             The requested SequenceNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the SequenceNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        SequenceNamedObject* getSequence( unsigned long entityKey );

        /**
          * getVideoOutputs
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
	      * Retrieves the list of all VideoOutputs. 
          *
          * @return     VideoOutputVector 
          *             The vector of Corba Named Obj Ref pointers to the VideoOutputs.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoOutputVector getVideoOutputs();

	    /**
          * getVideoOutput
          *
	      * Retrieves an VideoOutputNamedObject by entity key.
          *
          * @return     VideoOutputNamedObject
          *             The requested VideoOutputNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the VideoOutputNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        VideoOutputNamedObject* getVideoOutput( unsigned long entityKey );

        /**
          * getVideoInputs
          *
	      * Retrieves the list of all VideoInputs. 
          * It is the caller's responsibility to clean up the objects in the returned vector.
          *
          * @return     VideoInputVector 
          *             The vector of Corba Named Obj Ref pointers to the VideoInputs.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
          *             2. The Corba name for one of the objects could not be determined uniquely.
          *             3. One of the resolved objects could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if one of the objects could not be resolved.
	      */

        VideoInputVector getVideoInputs();

	    /**
          * getVideoInput
          *
	      * Retrieves an VideoInputNamedObject by entity key.
          * It is the caller's responsibility to clean up the returned object.
          *
          * @return     VideoInputNamedObject*
          *             The requested VideoInputNamedObject.
          *
          * @param      unsigned long entityKey
          *             The entity key of the VideoInputNamedObject to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
          *             3. The Corba name for the object could not be determined uniquely.
          *             4. The resolved object could not be converted to the appropriate named obj ref.
          * @exception  CORBA::SystemException
          *             Thrown if something goes wrong with the CORBA stuff.
          * @exception  ObjectResolutionException
          *             Thrown if the object could not be resolved.
	      */

        VideoInputNamedObject* getVideoInput( unsigned long entityKey );

		bool testAgentConnection();

		SwitchManagerNamedObject& getSwitchManager();
		SwitchManagerNamedObject& getOccSwitchManager();
		SwitchManagerNamedObject& getSwitchManager(unsigned long consoleId); // TD15853
        
		void setLocationName(std::string location);
		void setLocationNameFromSession(std::string sessionID);

    private:
        /**
          * VideoSwitchAgentFactory
          *
          * Standard constructor.  Declared private because this class is 
          * as singleton.  Use getInstance() to get access to the 
          * single instance of this class.
          */

        VideoSwitchAgentFactory();

        //
        // Disable copy constructor and assignment operator.
        //

        VideoSwitchAgentFactory( const VideoSwitchAgentFactory& theVideoSwitchAgentFactory );
        VideoSwitchAgentFactory& operator=( const VideoSwitchAgentFactory& theVideoSwitchAgentFactory );

        /**
          * resolveSwitchManager
          *
          * Creates a named obj ref for the SwitchManager.
          */

        void resolveSwitchManager();

        /**
          * getLocalConsoleKey
          *
          * Gets the key of the local console.
          *
          * @return     unsigned long
          *             The key of the local console.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the local console key could not be determined.
          */

        unsigned long getLocalConsoleKey();

		//
		// The one and only instance of this class.
		//

		static VideoSwitchAgentFactory* m_theClassInstance;
		
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

     	//
		// Pointer to the SwitchManager.
		//

		SwitchManagerNamedObject m_switchManager;

        //
        // The OCC switch manager if not at the OCC
        //
        SwitchManagerNamedObject m_occSwitchManager;

        //
        // The name of the agent, used for resolving the Corba objects.
        //

        std::string m_agentName;

        //
        // Flag to indicate if the name for m_switchManager has been set.
        //
        
        bool m_isNameSet;

		//if this is being run by a non managed app we cant get hostname/location
		// from session so another method is provided
		std::string m_location;

        // the location type - if not at the OCC then m_occSwitchManager will be set.
        TA_Base_Core::ILocation::ELocationType m_locationType;

    }; // VideoSwitchAgentFactory
}

#endif // VIDEO_SWITCH_AGENT_FACTORY_H
