 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTrainDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTrainDvaMessage is an implementation of IPaTrainDvaMessage. It holds the data specific to an PaTrainDvaMessage entry
  * in the database, and allows read-only access to that data.
  *
  */


#if !defined(PaTrainDvaMessage_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaTrainDvaMessage_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaTrainDvaMessageHelper.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"

namespace TA_Base_Core
{


    class PaTrainDvaMessage : public IPaTrainDvaMessage
    {

    public:



        /**
         * PaTrainDvaMessage (constructor)
         *
         * Construct an PaTrainDvaMessage class based around a key.
         *
         * @param key The key of this PaTrainDvaMessage in the database
         */
        PaTrainDvaMessage(const unsigned long key);

        /**
         * PaTrainDvaMessage (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaTrainDvaMessageHelper::PaTrainDvaMessageHelper(IData*)
         *
         */
        PaTrainDvaMessage(unsigned long row, TA_Base_Core::IData& data);

        virtual ~PaTrainDvaMessage();

        virtual unsigned long getKey();
        virtual std::string getLabel();
         
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  
         * For the PA Train Dva Message Group this is 
         * just the key in a string format e.g. "PA Train Dva Message 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this PaTrainDvaMessage as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaTrainDvaMessage not used so it is made private
		PaTrainDvaMessage& operator=(const PaTrainDvaMessage &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaTrainDvaMessageHelper
        // has only been written for ConfigLocation objects and will not copy the PaTrainDvaMessage helper
        // for a read-only object (ie this one).
        PaTrainDvaMessage( const PaTrainDvaMessage& thePaTrainDvaMessage);  

        TA_Base_Core::PaTrainDvaMessageHelper * m_paTrainDvaMessageHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaTrainDvaMessage_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
