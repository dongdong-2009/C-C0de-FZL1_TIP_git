/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IOnlinePrinter.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IOnlinePrinter is an interface to an OnlinePrinter object. It allows the OnlinePrinter object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IOnlinePrinter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IOnlinePrinter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IOnlinePrinter : public virtual IItem
    {

    public:

        virtual std::string getPrinter() =0;
        virtual unsigned long getSubsystem() =0;
        virtual unsigned long getLocation() =0;
        virtual bool getIsAlarm() = 0;

        virtual ~IOnlinePrinter() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IOnlinePrinter*> IOnlinePrinters;
    typedef IOnlinePrinters::iterator    IOnlinePrintersIt;
} //close namespace TA_IRS_Core

#endif // !defined(IOnlinePrinter_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
