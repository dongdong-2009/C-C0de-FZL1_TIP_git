#if !defined( EventViewerH__INCLUDED_ )
#define EventViewerH__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EventViewer.h $
  * @author Nick Jardine 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * EventViewer provides access to the Event Viewer entity for loading configuration data.
  * It inherits from EntityData.
  */


#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class EventViewer : public EntityData
    {
    public:
        /**
          * EventViewer
          *
          * Constructs a new EventViewer object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        EventViewer();

        /**
          * EventViewer
          *
          * Constructs a new EventViewer object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this EventViewer in the database.
          */

        EventViewer( unsigned long key );

        /**
          * ~EventViewer
          *
          * Standard destructor.
          */

        virtual ~EventViewer() {};

        /**
         * areAlarmEventsColourCoded
         *
         * Returns a bool indicating if this Event Viewer is to colour code events associated with alarms
         *
         * Preconditions:  0 != getHelper()
         *
         * @return     bool    
         *             True if the ColourCodedAlarmEvents parameter is 1.
         *             False otherwise.
         * 
         * @exception  DatabaseException 
         *             Thrown if there is a database error.
         * @exception  DataException 
         *             Thrown if the ColourCodedAlarmEvents parameter value 
         *             cannot be converted to a bool.
         */
        bool areAlarmEventsColourCoded();

        /**
         * getDefaultExportLocation
         *
         * Returns the default export location for the event viewer, or the empty string if not set.
         *
         * Preconditions:  0 != getHelper()
         *
         * @return     string    
         *             The default export location
         * 
         * @exception  DatabaseException 
         *             Thrown if there is a database error.
         * @exception  DataException 
         *             Thrown if the ExportFilePath parameter value 
         *             cannot be converted to a bool.
         */
        std::string getDefaultExportLocation();

        /**
         * isCloseDisabled
         *
         * Returns a bool indicating if the Event Viewer is to diable the close options ("X" and File->exit).
         *
         * Preconditions:  0 != getHelper()
         *
         * @return     bool    
         *             True if the close options are to be disabled
         *             False otherwise
         * 
         * @exception  DatabaseException 
         *             Thrown if there is a database error.
         * @exception  DataException 
         *             Thrown if the WindowClose parameter value 
         *             cannot be converted to a bool.
         */
        bool isCloseDisabled();

        /**
         * getEventLimit
         *
         * Returns the maximum number of events the event viewer is allowed to display in its view.
         *
         * Preconditions:  0 != getHelper()
         *
         * @return     unsigned long    
         *             The maximum number of events the event viewer is allowed to display.
         * 
         * @exception  DatabaseException 
         *             Thrown if there is a database error.
         * @exception  DataException 
         *             Thrown if the MaxEvents parameter value 
         *             cannot be converted to an unsigned long.
         */
        unsigned long getEventLimit();

        /**
         * getEventRefreshTime
         *
         * Returns the number of seconds between each event refresh.
         *
         * Preconditions: 0 != getHelper()
         *
         * @return      unsigned long
         *              The number of seconds to wait between event refreshes.
         *
         * @exceptiion  DatabaseException
         *              Thrown if there is a database error.
         * @exception   DataException
         *              Thrown if the EventRefreshTime parameter value
         *              cannot be converted to an unsigned long.
         */
        unsigned long getEventRefreshTime();

        /**
         * getEventRefreshTimeAgent
         *
         * Returns the number of milli-seconds between each event refresh from event agent.
         *
         * Preconditions: 0 != getHelper()
         *
         * @return      unsigned long
         *              The number of seconds to wait between event refreshes.
         *
         * @exceptiion  DatabaseException
         *              Thrown if there is a database error.
         * @exception   DataException
         *              Thrown if the EventRefreshTime parameter value
         *              cannot be converted to an unsigned long.
         */
		unsigned long getEventRefreshTimeAgent();

		/**
         * getDefaultEventVisibilityLevel
         *
         * Returns the Default event level used to limit the visible events for an operator.
         *
         * Preconditions: 0 != getHelper()
         *
         * @return      unsigned long
         *              Default event level used to limit the visible events for an operator.
         *
         * @exceptiion  DatabaseException
         *              Thrown if there is a database error.
         * @exception   DataException
         *              Thrown if the DefaultEventVisibilityLevel parameter value
         *              cannot be converted to an unsigned long.
         */
        unsigned long getDefaultEventVisibilityLevel();

		/**
         * getExportDirectory
         *
         * Returns the default directory
         *
         *
         * @return      std::string
         *              string used to set the default directory
         *
         * @exceptiion  DatabaseException
         *              Thrown if there is a database error.
         */

		std::string getExportDirectory();

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getType
          *
          * Returns the type of this entity. This method must be callable when the object
          * is constructed using the default constructor. The entity type string should be
          * stored as a static const variable in the concrete class. The entity type should
          * not be loaded from the database (note that this method defines no exceptions).
          *
          * @return      std::string
          *              The entity type of this entity.
          */

	    virtual std::string getType();

		static std::string getStaticType();
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        /**
          * clone
          *
          * Returns a pointer to a new EventViewer object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the EventViewer object.
          */

        static IEntityData* clone(unsigned long key);


    private:

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        // 
        // Supported Features
        //

        static const std::string COLOUR_CODE_ALARM_EVENTS;
        static const std::string EXPORT_FILE_PATH;
        static const std::string DISABLE_CLOSE;
        static const std::string MAX_EVENTS;
        static const std::string REFRESH_TIME;
		static const std::string REFRESH_TIME_AGENT;
		static const std::string DEFAULT_EVENT_VISIBILITY_LEVEL;
		//TD17888 marc_bugfix
		static const std::string EXPORT_DIRECTORY;

        //
        // Disable copy constructor and assignment operator
        //

        EventViewer( const EventViewer& theEventViewer );
        EventViewer& operator=( const EventViewer& theEventViewer );

    };

    
    typedef boost::shared_ptr<EventViewer> EventViewerPtr;
} // TA_Base_Core

#endif // !defined(EventViewerH__INCLUDED_)
