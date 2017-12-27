/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Huang Qi
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * PaPriorityGroupAccessFactory is a singleton that is used to retrieve PaStation objects either from the
  * database or newly created. All PaStation objects returned will adhere to the IPaStation interface.
  */



#if !defined(PAPRIORITYGROUPACCESSFACTORY__INCLUDED_)
#define PAPRIORITYGROUPACCESSFACTORY__INCLUDED_

#include <string>
#include <map>

namespace TA_Base_Core
{
    const unsigned long DEFAULT_LOWEST_PA_PRIORITY_ID = 6;

	class PaPriorityGroupAccessFactory
	{
    public:
    

	private:
        
        // Disable the following methods
		PaPriorityGroupAccessFactory() { };
		PaPriorityGroupAccessFactory( const PaPriorityGroupAccessFactory& thePaPriorityGroupAccessFactory);
		PaPriorityGroupAccessFactory& operator=(const PaPriorityGroupAccessFactory &);


	public:
        
        
        /** 
         * ~PaPriorityGroupAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaPriorityGroupAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaPriorityGroupAccessFactory object.
         */
		static PaPriorityGroupAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();


        /**
         * getPaPriorityGroupIdBySessionId
	     *
	     * Get the group id by current session id
         *
         */
        unsigned long getPaPriorityGroupIdBySessionId( const std::string& strSessionId );

        /**
         * getAllPaPriorityGroupNames
	     *
	     * Get all group names and keys
         *
         */
        void getAllPaPriorityGroupNames( std::map<unsigned long, std::string>& mapKeyToNames );

    private:

        static PaPriorityGroupAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(PaPriorityGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
