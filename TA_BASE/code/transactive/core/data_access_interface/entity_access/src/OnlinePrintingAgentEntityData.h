#ifndef ONLINEPRINTINGAGENTENTITYDATA_H_INCLUDED
#define ONLINEPRINTINGAGENTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/OnlinePrintingAgentEntityData.h $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Online Printing Agent entity configuration access.
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class OnlinePrintingAgentEntityData : public EntityData
    {
    // Public constants
    public:
        OnlinePrintingAgentEntityData();
        OnlinePrintingAgentEntityData(unsigned long key);
        virtual ~OnlinePrintingAgentEntityData();

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

		/**
         * getEventView
         *
		 * Returns the name of the view from which to extract printable events.
         */
		std::string getEventView();

		/**
         * getViewPollFrequency
         *
		 * Returns how often to look up the view for new events (in seconds).
         */
		unsigned long getViewPollFrequency();

		/**
         * getOutputColumns
         *
		 * Returns the printout column specification.
         */
		std::string getOutputColumns();

		/**
         * getPrintCommand
         *
		 * Returns the command used to submit print requests to the spooler.
         */
		std::string getPrintCommand();

		std::string getAssetName();

		/**
         * isPrintingEnabled
         *
		 * Returns true if printing is on, false if it's off.
         */
		bool isPrintingEnabled();

		/**
         * printAlarmEventsOnly
         *
		 * Returns true if only alarm related events should be printed, false if all events should be printed.
         */
		bool printAlarmEventsOnly();

        /**
         * clone
         *
         * Returns a pointer to a new OnlinePrintingAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the OnlinePrintingAgentEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

	private:
        OnlinePrintingAgentEntityData( const OnlinePrintingAgentEntityData&);            
		OnlinePrintingAgentEntityData& operator=(const OnlinePrintingAgentEntityData&);

    public:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
	    static const std::string EVENT_VIEW_PARAM;
	    static const std::string VIEW_POLL_FREQUENCY_PARAM;
	    static const std::string OUTPUT_COLUMNS_PARAM;
	    static const std::string PRINT_COMMAND_PARAM;
	    static const std::string IS_PRINTING_ENABLED_PARAM;
	    static const std::string PRINT_ALARM_EVENTS_ONLY_PARAM;
		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<OnlinePrintingAgentEntityData> OnlinePrintingAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(ONLINEPRINTINGAGENTENTITYDATA_H_INCLUDED)
