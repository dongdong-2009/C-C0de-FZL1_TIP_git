/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/DllHandler.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This implements the IDLLHandler so it can be passed to the SingletonHelper to allow it
  * to initialise all singletons.
  */

#ifndef DLL_HANDLER_H
#define DLL_HANDLER_H

#include "app/configuration/configuration_editor/src/IDllHandler.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    class CorbaUtil;
}

namespace TA_Base_App
{
    class DllHandler : public IDllHandler
    {
    public:

        /**
          * ~DllHandler
          *
          * Standard destructor.
          */

        ~DllHandler() {};

        ///////////////////////////////////////////////////////////////////
        //
        // These methods are to overcome the problems of singletons that
        // cannot be shared across the DLL interface because DLLs have
        // their own memory space.
        //
        ///////////////////////////////////////////////////////////////////

        /**
          * getRunParams
          *
          * Gets the RunParams from the main application.
          *
          * @return TA_Base_Core::RunParams::ParamVector& - The RunParams for the main application.
          */
        virtual TA_Base_Core::RunParams::ParamVector getRunParams() const;

        /**
          * getCorbaUtil
          *
          * Gets a reference to the main application's CorbaUtil instance.
          *
          * @return TA_Base_Core::CorbaUtil& - A reference to the CorbaUtil singleton instance.
          */
        virtual TA_Base_Core::CorbaUtil& getCorbaUtil() const;
        virtual TA_Base_Core::RunParams& getRunParamsInstance() const;
    };
} // TA_Base_App

#endif //DLL_HANDLER_H
