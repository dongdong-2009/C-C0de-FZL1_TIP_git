#ifndef ICONFIGPREDEFINED_MESSAGE_LIBRARY_H_INCLUDED
#define ICONFIGPREDEFINED_MESSAGE_LIBRARY_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/IConfigPredefinedMessageLibrary.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * This interface contains the database retrieval and write methods
  * for Predefined Message Libraries for the Traveller Information System (TIS) Agent.
  */

#include <vector>
#include <string>
#include <map>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h"

namespace TA_Base_Core
{


    class IConfigPredefinedMessageLibrary : public TA_Base_Core::IPredefinedMessageLibrary, public virtual TA_Base_Core::IConfigItem
    {
        public:

            /**
             * ~IConfigPredefinedMessageLibrary
             *
             * Standard destructor.
             */
            virtual ~IConfigPredefinedMessageLibrary() {};


            /**
             * addMessage
             *
             * Adds a pre-defined message to this library.
             *
             * @param       theMessage
             *              The message to add to this library
             *
             * @exception   TA_Base_Core::DatabaseException
             *              Thrown if:
             *              1. There is some error with the database connection
             *              2. An SQL statement failed to execute for some reason.
             * @exception   DataException
             *              Thrown if the data cannot be converted to the correct format.
             */
            virtual void addMessage( const PredefinedMessage& theMessage ) = 0;


            /**
              * setDefaultDisplayAttributes
              *
              * Set the default STIS led, STIS plasma, and TTIS led attributes associated with
              * this message library.
              *
              * @param stisLedAttributes the structure holding the stis led attribute values.
              * @param stisPlasmaAttributes the structure holding stis plasma the attribute values.
              * @param ttisLedAttributes the structure holding the ttis led attribute values.
              *
              * @exception   TA_Base_Core::DatabaseException
              *              Thrown if:
              *              1. There is some error with the database connection
              *              2. An SQL statement failed to execute for some reason.
              * @exception   DataException
              *              Thrown if the data cannot be converted to the correct format.
              */


            virtual void setDefaultSTISDisplayAttributes( const DisplayAttributes& stisAttributes,
                                                          const LCDAttributesSTIS& stisLcdAttributes,
                                                          const LEDAttributes& stisLedAttributes ) = 0;


            /**
              * deleteThisLibrary
              *
              * Deletes the pre-defined message library and all associated data
              * from the database.
              *
              * @exception   TA_Base_Core::DatabaseException
              *              Thrown if:
              *              1. There is some error with the database connection
              *              2. An SQL statement failed to execute for some reason.
              * @exception   DataException
              *              Thrown if the data cannot be converted to the correct format.
              */
            virtual void deleteThisLibrary() = 0;


            /**
              * invalidate
              *
              * Marks the data contained by this object as invalid. The next call to get...()
              * following a call to invalidate() will cause all the data to be reloaded from
              * the database.
              */
            virtual void invalidate() = 0;

    };

} //end namespace TA_Base_Core

#endif
