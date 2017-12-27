/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocAccessFactory.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * EventPlanAssocAccessFactory is a singleton that is used to retrieve 
  * IEventPlanAssocData objects from the database. It is able to provide 
  * read-only access.
  */

#if !defined(_EVENT_PLAN_ASSOC_ACCESS_FACTORY_H_)
#define _EVENT_PLAN_ASSOC_ACCESS_FACTORY_H_

#include <memory>
#include <vector>

namespace TA_Base_Core
{
    // Forward delaration to help the compiler avoid unnecessary compilation
    class IEventPlanAssocData;
    class IConfigEventPlanAssocData;

    class EventPlanAssocAccessFactory
    {
    public:
        virtual ~EventPlanAssocAccessFactory();

        /**
          * getInstance
          *
          * Creates and returns an instance of this object.
          *
          * @return A reference to an instance of an EventPlanAssocAccessFactory object.
          */
        static EventPlanAssocAccessFactory& getInstance();


        /**
         * getAllEventPlanAssociations
         *
         * Retrieves all configured event plan associations and returns the associated data as a
         * vector of objects conforming to the IEventPlanAssocData interface.
         *
         * @param readWrite Determines if a writable object is returned.
         *
         * @return A vector of pointers to objects conforming to the IEventPlanAssocData interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IEventPlanAssocData 
         * objects.
         *
         * @exception DatabaseException Thrown if there are any problems in communicating with the
         *            database. The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException Thrown if an error was encountered extracting the data.
         */
        std::vector<IEventPlanAssocData*> getAllEventPlanAssociations( const bool readWrite = false );


        /**
         * getEnabledEventPlanAssociations
         *
         * Retrieves all configured event plan associations that are enabled and returns the associated
         * data as a vector of objects conforming to the IEventPlanAssocData interface.
         *
         * @param readWrite Determines if a writable object is returned.
         *
         * @return A vector of pointers to objects conforming to the IEventPlanAssocData interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IEventPlanAssocData 
         * objects.
         *
         * @exception DatabaseException Thrown if there are any problems in communicating with the
         *            database. The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException Thrown if an error was encountered extracting the data.
         */
        std::vector<IEventPlanAssocData*> getEnabledEventPlanAssociations( const bool readWrite = false );

        /**
         * createEventPlanAssociation
         * 
         * Creates a new configuration event plan association object.
         *
         * @return The configuration event plan association object as a pointer to an
         *         IConfigEventPlanAssocData object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        std::auto_ptr<IConfigEventPlanAssocData> createEventPlanAssociation();


        /**
         * copyEventPlanAssociation
         * 
         * Creates a new configuration event plan association object using the event plan association
         * passed in
         *
         * @param itemToCopy The event plan association to copy.
         *
         * @return The configuration event plan association object as a pointer to an
         *         IConfigEventPlanAssocData object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        std::auto_ptr<IConfigEventPlanAssocData> copyEventPlanAssociation( const IConfigEventPlanAssocData* itemToCopy );


    private:
        // Hidden methods.
        EventPlanAssocAccessFactory();
        EventPlanAssocAccessFactory( const EventPlanAssocAccessFactory& theEventPlanAssocAccessFactory );
        EventPlanAssocAccessFactory& operator=( const EventPlanAssocAccessFactory& );

        /**
         * getEventPlanAssociations
         *
         * Returns a vector of associations that match the specified requirements.
         *
         * @param onlyEnabled Specifies whether only enabled events should be retrieved.
         * @param readWrite Specifies whether writable objects should be returned.
         *
         * @return A vector of pointers to objects conforming to the IEventPlanAssocData interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IEventPlanAssocData 
         * objects.
         *
         * @exception DatabaseException Thrown if there are any problems in communicating with the
         *            database. The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException Thrown if an error was encountered extracting the data.
         */
        std::vector<IEventPlanAssocData*> getEventPlanAssociations( bool onlyEnabled, bool readWrite );

        static EventPlanAssocAccessFactory* s_instance;

    };

} // closes TA_Base_Core

   
#endif // _EVENT_PLAN_ASSOC_ACCESS_FACTORY_H_

