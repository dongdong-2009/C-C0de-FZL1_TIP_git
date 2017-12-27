 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinter.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * OnlinePrinter is an implementation of IOnlinePrinter. It holds the data specific to an OnlinePrinter entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(OnlinePrinter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define OnlinePrinter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/OnlinePrinterHelper.h"
#include "core/data_access_interface/src/IOnlinePrinter.h"

namespace TA_Base_Core
{
    class OnlinePrinter : public IOnlinePrinter
    {

    public:

        /**
         * OnlinePrinter (constructor)
         *
         * Construct an OnlinePrinter class based around a key.
         *
         * @param key The key of this OnlinePrinter in the database
         */
        OnlinePrinter(const unsigned long idKey);
    
        /**
         * OnlinePrinter (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see OnlinePrinterHelper::OnlinePrinterHelper(IData*)
         *
         */
        OnlinePrinter(unsigned long row, TA_Base_Core::IData& data);

        virtual ~OnlinePrinter();

		virtual std::string getPrinter();
        virtual unsigned long getLocation();
        virtual unsigned long getSubsystem();
        virtual bool getIsAlarm();
 
        virtual void invalidate();

		virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


    private:

        // Assignment OnlinePrinter not used so it is made private
		OnlinePrinter& operator=(const OnlinePrinter &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the OnlinePrinterHelper
        // has only been written for ConfigLocation objects and will not copy the OnlinePrinter helper
        // for a read-only object (ie this one).
        OnlinePrinter( const OnlinePrinter& theOnlinePrinter);  


        OnlinePrinterHelper * m_OnlinePrinterHelper;
    };
} // closes TA_Core

#endif // !defined(OnlinePrinter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
