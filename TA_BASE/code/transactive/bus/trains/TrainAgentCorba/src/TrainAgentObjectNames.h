/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainAgentObjectNames_H)
#define TrainAgentObjectNames_H

#include <string>

namespace TA_Base_Bus
{
    /**
     * This just holds constants for the object names of each train agent interface.
     * It is done this way so we do not need to create a new entity for each interface
     * the train agent hosts.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 11:38:43 AM
     */
    class TrainAgentObjectNames
    {
    public:

        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainInformationObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainOperationModeObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainRadioGroupObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainDriverCallObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainCctvObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainPaObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainPecObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainTisObjectName;


        /**
         * The object name, the second part of the corba name.
         */
        static const std::string TrainStateUpdateObjectName;

    };

}
#endif // !defined(TrainAgentObjectNames_H)
