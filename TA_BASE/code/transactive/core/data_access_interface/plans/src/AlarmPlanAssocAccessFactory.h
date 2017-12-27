/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AlarmPlanAssocAccessFactory is a singleton that is used to retrieve 
  * IAlarmPlanAssocData objects from the database. It is able to provide 
  * read-only access.
  */

#if !defined(ALARMPLANASSOCACCESSFACTORY_H)
#define ALARMPLANASSOCACCESSFACTORY_H

#include <string>
#include <vector>

#include "core/configuration_updates/src/IOnlineUpdatable.h"

namespace TA_Base_Core
{
    typedef std::vector< std::string > PlanDetailsList; // list of plan paths

    // Forward delaration to help the compiler avoid unnecessary compilation
    class IAlarmPlanAssocData;
    class IConfigAlarmPlanAssoc;


    class AlarmPlanAssocAccessFactory : public virtual IOnlineUpdatable
    {
    public:
        virtual ~AlarmPlanAssocAccessFactory();


        /**
         * cleanUp
         *
         * If using the caching version of this class (ie the read-only mechanisms) then this
         * method should be called when finished with the access factory. This will ensure that
         * it closes all connections to the online update library
         */
        void cleanUp();


        /**
          * getInstance
          *
          * Creates and returns an instance of this object.
          *
          * @return A reference to an instance of an AlarmRuleAccessFactory object.
          */
        static AlarmPlanAssocAccessFactory& getInstance();


		/** 
		  * associationExists
		  *
		  * This function takes the entityKey, entityTypeKey and will
		  * return true if one (or more) associations exists for the combination
		  *
		  * @return 
		  *
		  * @param  unsigned long entitykey The entitykey associated with an alarm
		  * @param unsigned long entityTypeKey The type key associated with an alarm
		  * @param std::string& alarmType The alarm type
		  */
		bool associationExists( unsigned long entityKey, unsigned long entityTypeKey,
			unsigned long alarmType, const bool registerForOnlineUpdates = true );

		/** 
		  * getAssociatedPlanDetails
		  *
		  * This will return a vector of associated plan details for the given
		  * combination. 
		  *
		  * NOTE: Do not delete the returned PlanDetails objects!
		  *
		  * @return std::vector<PlanDetails> The vector of PlanID & PlanName structs
		  *
		  * @param  unsigned long entitykey The entitykey associated with an alarm
		  * @param unsigned long entityTypeKey The type key associated with an alarm
		  * @param std::string& alarmType The alarm type
		  */
		PlanDetailsList getAssociatedPlanDetails( unsigned long entityKey, unsigned long entityTypeKey,
			unsigned long alarmType, const bool registerForOnlineUpdates = true );


        /**
         * processUpdate
         *
         * When there is a configuration update of an alarm plan association then this method
         * will be invoked to process the update accordingly.
         *
         * @param ConfigUpdateDetails& - The update details so we know what to modify
         */
        virtual void processUpdate(const ConfigUpdateDetails& updateEvent);


        /**
         * getAllAlarmPlanAssociations
         *
         * Retrieves all specified alarm plan associations and returns the associated data as a vector of  
         * objects conforming to the IAlarmPlanAssocData interface.
         *
         * @return A vector of pointers to objects conforming to the IAlarmPlanAssocData interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAlarmPlanAssocData 
         * objects. This method does not use the cache the others do.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IAlarmPlanAssocData*> getAllAlarmPlanAssociations( const bool readWrite = false);


        /**
         * createAlarmPlanAssociation
         * 
         * Creates a new configuration alarm plan association object.
         *
         * @return The configuration alarm plan association object as a pointer to an IConfigAlarmPlanAssoc object.
         *         It is the *client* who must delete this pointer
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigAlarmPlanAssoc* createAlarmPlanAssociation();


        /**
         * copyAlarmPlanAssociation
         * 
         * Creates a new configuration alarm plan association object using the alarm plan association passed in
         *
         * @param IConfigAlarmPlanAssoc* The alarm plan association to copy
         *
         * @return The configuration alarm plan association object as a pointer to an IConfigAlarmPlanAssoc object.
         *         It is the *client* who must delete this pointer
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigAlarmPlanAssoc* copyAlarmPlanAssociation(const IConfigAlarmPlanAssoc* itemToCopy);


    private:

        AlarmPlanAssocAccessFactory();
        AlarmPlanAssocAccessFactory( const AlarmPlanAssocAccessFactory& theAlarmPlanAssocAccessFactory);
        AlarmPlanAssocAccessFactory& operator=(const AlarmPlanAssocAccessFactory &);
    
        static AlarmPlanAssocAccessFactory* s_instance;


		/** 
		  * loadAssociations
		  *
		  * This function will load all the AlarmPlanMapAssociations from the database. It will
		  * set the m_associaitonsLoaded variable to true on success.
		  */
		void loadAssociations(bool registerForOnlineUpdates = true);


        /**
         * getDateModified
         *
         * This will take a time_t and check if it is our default value. Our default value means that
         * the date modified is actually 0
         *
         * @param time_t The date modified read from the database
         *
         * @return time_t The date modified or 0 if it was not set in the database
         */
        time_t AlarmPlanAssocAccessFactory::getDateModified(time_t dateModified);


		/**
		* check to see if the associations have been loaded
		*/
		bool m_associationsLoaded;

		/**
		* Store all the associations in a cache. This cache is not used by the getAllAssocations method as
        * this needs to retrieve the items as separate objects.
		*/
		typedef std::vector<IAlarmPlanAssocData*> AssociationList;
		AssociationList m_associations;

        bool m_registeredOnlineUpdates;

	};

} // closes TA_Base_Core

   
#endif // ALARMPLANASSOCACCESSFACTORY_H

