#ifndef VMSENQUIRER_H
#define VMSENQUIRER_H

#if defined (STEPTYPE_VMS_SET_SCENE) || defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsEnquirer.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Light-weight singleton class to communicate with the VMS Agent
  * for selected enquiry functions only :
  *     - get all signs
  *     - get scenes for a sign
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 )
#endif // defined( WIN32 )

#include "core/naming/src/NamedObject.h"
#include "app/signs/vms_agent/idl/src/SignControlCorbaDef.h"
#if defined (STEPTYPE_VMS_SET_SCENE)
#include "app/signs/vms_agent/idl/src/SceneCorbaDef.h"
#endif

namespace TA_Base_Core
{
    // Forward declarations
    class ThreadLockable;
}

namespace TA_Base_App
{
    namespace TA_Signs
    {

        // Forward declarations
        class SignObjectSeq;
        class SignSpecificObjectSeq;

        class VmsEnquirer
        {
        public:
            /**
              * getInstance
              *
              * Return a reference to a VmsEnquirer object, this allows the
              * VmsEnquirer object to only be created once.
              *
              * @return VmsEnquirer & - a reference to the VmsEnquirer object.
              *
              */
            static VmsEnquirer& getInstance();

            SignObjectSeq const VmsEnquirer::getSigns() ;

#if defined (STEPTYPE_VMS_SET_SCENE)

            SignSpecificObjectSeq const getScenesForSign( unsigned long signKey );

           /**
              * getVmsSceneName
              *
              * Gets the name of the given scene.
              *
              * @return     std::string
              *             The name of the scene.
              *
              * @param      unsigned long vmsSceneId
              *             The id of the VMS scene.
              *
              * @exception  TransactiveException
              *             Thrown if:
              *             1. The VMS Agent cannot be contacted
              *             2. The VMS scene with the given id was not found.
              */

            std::string getVmsSceneName( unsigned long vmsSceneId );

#endif

            /**
              * getVmsName
              *
              * Gets the name of the VMS with the specified key.
              *
              * @return     std::string
              *             The name of the VMS.
              *
              * @param      unsigned long vmsEntityKey
              *             The entity key of the VMS.
              *
              * @exception  TransactiveException
              *             Thrown if:
              *             1. The VMS Agent cannot be contacted
              *             2. The VMS with the given entity key was not found.
              */

            std::string getVmsName( unsigned long vmsEntityKey );

        private:

            /**
              * VmsEnquirer
              *
              * Standard constructor stub: no public construction available since
              * there will only be one instance of the VmsEnquirer.
              *
              */
            VmsEnquirer();

            /**
              * VmsEnquirer
              *
              * Copy constructor stub - not to be used
              *
              * @return
              *
              * @param VmsEnquirer &si
              */
            VmsEnquirer(const VmsEnquirer& si);

            /**
              * operator=
              *
              * Assignment operator stub - not to be used
              *
              * @return VmsEnquirer&
              *
              * @param VmsEnquirer &si
              */
            VmsEnquirer& operator=(const VmsEnquirer& si);

            static VmsEnquirer* m_singleton;                 //  the one and only instance
            static TA_Base_Core::ThreadLockable m_singletonLock; //  thread safety for getInstance()

            // Sign Factory Interface

            TA_Base_Core::NamedObject<
                ISignFactoryCorbaDef,
                ISignFactoryCorbaDef_ptr,
                ISignFactoryCorbaDef_var>  m_signFactory;

#if defined (STEPTYPE_VMS_SET_SCENE)

            // Scene Factory Interface

            TA_Base_Core::NamedObject<
                ISceneFactoryCorbaDef,
                ISceneFactoryCorbaDef_ptr,
                ISceneFactoryCorbaDef_var>  m_sceneFactory;

#endif

        };

    } // TA_Signs
} // TA_Base_App

#endif // STEPTYPE_VMS_SET_SCENE || STEPTYPE_VMS_BLANKOUT || STEPTYPE_VMS_RESTORE_SCHEDULE

#endif // VMSENQUIRER_H
