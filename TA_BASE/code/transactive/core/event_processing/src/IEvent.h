/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/IEvent.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 15:24:34 $
  * Last modified by:  $Author: Noel $
  *
  * Defines the interface that all events must adhere to.
  *
  */

#if !defined(_I_EVENT_H_)
#define _I_EVENT_H_

#include "boost/smart_ptr.hpp"
#include <sys/timeb.h>
#include "core/message/src/NameValuePair.h"
#include <string>
namespace TA_Base_Core
{
    namespace EventConstants
    {
		//INVALID means the real value not provided
        const unsigned long INVALID_KEY = 0xFFFFFFFF;
		const int INVALID_SUBSYSTEMKEY = -2;
		const int INVALID_PHYSICALSUBSYSTEMKEY = -2;
		const timeb INVALID_CREATETIME = {0,0,-1,-1};


		const std::string SOURCE_TABLE_EVENT = "100";  //For Event,alarm
		const std::string SOURCE_TABLE_DP_CHANGE = "200";  //for Datapoint status change

		const std::string INTERNAL_PROCESSING_SESSION_KEY = "InternalProcessing";

		const std::string EVENT_TYPE_NAME_STUB = "#EventTypeName#";
    };

    class IEvent
    {
    public:
		/**
		  * Virtual Destructor
		  */
		virtual ~IEvent(){}

        /** 
          * getEntityKey
          *
          * Returns the key of the entity associated with the event.
          *
          * @return unsigned long The entity key.
          */
        virtual unsigned long getEntityKey() =0;

		 /** 
		  * getEventId
		  *
		  * Return the event uuid 
		  */
		virtual std::string getEventId(){return "";};

        /** 
          * getEntityTypeKey
          *
          * Returns the key of the entity type associated with the event.
          *
          * @return unsigned long The entity type key.
          */
        virtual unsigned long getEntityTypeKey() =0;


        /** 
          * getEventTypeKey
          *
          * Returns the event type key.
          *
          * @return unsigned long The event type key.
          */
        virtual unsigned long getEventTypeKey() =0;


        /** 
          * writeToDatabase
          *
          * Saves the event into the database.
          */
        virtual void writeToDatabase() =0;


        /** 
          * serialize
          *
          * TBD: Only required when a persistant queue is needed.
          */
        virtual void serialize() /*=0*/ {};


        /** 
          * deserialize
          *
          * TBD: Only required when a persistant queue is needed.
          */
        virtual void deserialize() /*=0*/ {};

		/** 
          * getSubsystemKey
          *
          * Returns the entity subsystem key.
          *
          * @return unsigned long the entity subsystem key
          */
		virtual unsigned long getSubsystemKey(){return 0;};

		/** 
          * getPhysicalSubsystemKey
          *
          * Returns the entity physical subsystem key.
          *
          * @return unsigned long the entity physical subsystem key
          */
		virtual unsigned long getPhysicalSubsystemKey() {return 0;};

		/** 
          * getEntityLocationKey
          *
          * Returns the entity location key.
          *
          * @return unsigned long the entity location key
          */
		virtual unsigned long getEntityLocationKey() {return 0;};

		/** 
          * getAssetName
          *
          * Returns the asset name.
          *
          * @return string the asset name
          */
		virtual std::string getAssetName() {return "";};

		/** 
          * getValue
          *
          * Returns the value of the event.
          *
          * @return string value
          */
		virtual std::string getValue() {return "";};

		/** 
          * getCreateTime
          *
          * Returns the create time of the event.
          *
          * @return timeb create time
          */
		virtual timeb getCreateTime() {  return EventConstants::INVALID_CREATETIME; };

		/** 
          * getDescriptionParameters
          *
          * Returns the DescriptionParameters of the event.
          *
          * @return DescriptionParameters description parameters of the event
          */
		virtual TA_Base_Core::DescriptionParameters getDescriptionParameters()
		{	TA_Base_Core::DescriptionParameters dp; return dp;};

    };

    typedef boost::shared_ptr<IEvent> Event;
};

#endif // !defined(_I_EVENT_H_)

