 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPredefinedSDSMessage.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RAPredefinedSDSMessage is an implementation of IRAPredefinedSDSMessage. It holds the data specific to an RAPredefinedSDSMessage entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_PREDEFINED_SDS_MESSAGE_XX_INCLUDED_)
#define RA_PREDEFINED_SDS_MESSAGE_XX_INCLUDED_

#include <string>
#include <algorithm>
#include <functional>

#include "core/data_access_interface/radio/src/IRAPredefinedSDSMessage.h"

namespace TA_Base_Core
{


    class RAPredefinedSDSMessage : public IRAPredefinedSDSMessage
    {

    public:

        /**
         * RAPredefinedSDSMessage (constructor)
         *
         * Construct an RAPredefinedSDSMessage class based around a key.
         *
         * @param key The key of this RAPredefinedSDSMessage in the database
         */
        RAPredefinedSDSMessage(const std::string& shortName);

        RAPredefinedSDSMessage(
            const std::string& shortName,
            const std::string& Message,
            bool isNew
            );

        virtual ~RAPredefinedSDSMessage();


    public:
        
        virtual std::string getShortName(bool reloadFlag = true); 
        virtual IRAPredefinedSDSMessage& setShortName(const std::string& shortName); 
        
        virtual std::string getMessage();
        virtual IRAPredefinedSDSMessage& setMessage(const std::string& message);

        virtual void invalidate(); // IItem

		virtual void applyChanges();

    private:

        // Assignment RAPredefinedSDSMessage not used so it is made private
		RAPredefinedSDSMessage& operator=(const RAPredefinedSDSMessage &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RAPredefinedSDSMessageHelper
        // has only been written for ConfigLocation objects and will not copy the RAPredefinedSDSMessage helper
        // for a read-only object (ie this one).
        RAPredefinedSDSMessage( const RAPredefinedSDSMessage& theRAPredefinedSDSMessage);  


        /**                                                                                 
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void reload();

        std::string m_shortName;
        std::string m_message;

        bool m_isValidData;
		bool m_isWritten;
//        TA_Base_Core::RAPredefinedSDSMessageHelper * m_RAPredefinedSDSMessageHelper;
    };


} // closes TA_Base_Core


#endif // !defined(RA_PREDEFINED_SDS_MESSAGE_XX_INCLUDED_)
