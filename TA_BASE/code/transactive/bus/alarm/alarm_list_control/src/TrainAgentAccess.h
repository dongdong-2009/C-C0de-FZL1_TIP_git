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
#if !defined(TrainAgentAccess_H)
#define TrainAgentAccess_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainCctvCorba.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainInformationCorba.h"

#include "core/naming/src/NamedObject.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"

#include <ACE/Singleton.h>

namespace TA_Base_Bus
{

    /**
     * Common code used to get access to train CCTV objects
     * @author adamr
     * @version 1.0
     * @created 13-Mar-2008 6:38:02 PM
     */
    class TrainAgentAccess
    {

        friend class ACE_Singleton<TrainAgentAccess, ACE_Recursive_Thread_Mutex>;

    public:

        /**
         * The standard named object for access to the CORBA servant.
         * @author adamr
         * @version 1.0
         * @created 13-Mar-2008 6:38:02 PM
         */
        typedef TA_Base_Core::FlexibleTimeoutNamedObject< ITrainCctvCorba,
                                                          ITrainCctvCorba_ptr,
                                                          ITrainCctvCorba_var > TrainCctvNamedObject;

        /**
         * The standard named object for access to the CORBA servant.
         * @author adamr
         * @version 1.0
         * @created 13-Mar-2008 6:38:02 PM
         */
        typedef TA_Base_Core::NamedObject< ITrainInformationCorba,
                                           ITrainInformationCorba_ptr,
                                           ITrainInformationCorba_var > TrainInformationNamedObject;


        /**
         * Gets an instance of the object to use
         * 
         * @return an instance to the singleton
         */
        static TrainAgentAccess& getInstance();


        /**
         * Gets a reference to the named object to use to contact the train agent
         * 
         * @return a reference to the train cctv remote object
         * 
         * @exception TransactiveException if there was an error getting the object reference
         */
        TrainCctvNamedObject& getTrainCctvAgent();


        /**
         * Gets a reference to the named object to use to contact the train agent
         * 
         * @return a reference to the train information remote object
         * 
         * @exception TransactiveException if there was an error getting the object reference
         */
        TrainInformationNamedObject& getTrainInformationAgent();


        /**
         * Gets the current location key of this workstation
         * 
         * @return the location key
         * 
         * @exception TransactiveException if there was an error getting the information
         */
        unsigned long getLocationKey();


        /**
         * Gets the key of this workstation
         * 
         * @return the console key
         * 
         * @exception TransactiveException if there was an error getting the information
         */
        unsigned long getConsoleKey();


    private:

        /**
         * Private constructor and destructor - created using the singleton
         */
        TrainAgentAccess();
        ~TrainAgentAccess();


        /**
         * Initialises the member variables if not already initialised
         * 
         * @exception TransactiveException if there was an error getting the information or the object reference
         */
        void initialise();


        /**
         * Pointer to the TrainAgent's CCTV interface
         */
        TrainCctvNamedObject* m_cctvTrainAgent;


        /**
         * Pointer to the TrainAgent's Information interface
         */
        TrainInformationNamedObject* m_informationTrainAgent;


        /**
         * The current location key
         */
        unsigned long m_locationKey;


        /**
         * The current console key
         */
        unsigned long m_consoleKey;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 13-Mar-2008 6:38:03 PM
     */
    typedef ACE_Singleton<TrainAgentAccess, ACE_Recursive_Thread_Mutex> TrainAgentAccessSingleton;
}

#endif // !defined(TrainAgentAccess_H)
