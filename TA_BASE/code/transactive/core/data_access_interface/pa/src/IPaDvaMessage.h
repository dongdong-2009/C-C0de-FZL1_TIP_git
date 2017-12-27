/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/IPaDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IPaDvaMessage is an interface to a PaDvaMessage object.  It allows the PA DVA Message object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IPaDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaDvaMessage : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getLocationKey() =0;
        virtual unsigned long getId() =0;
        virtual std::string getLabel() =0;
        virtual std::string getDescription() =0;
        virtual unsigned short getType() =0;
        //virtual unsigned long getTisMessageTag() =0;
        //virtual unsigned long getTisLibrarySection() =0;
        //virtual unsigned long getTisLibraryVersion() =0;

        virtual ~IPaDvaMessage() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaDvaMessage*> IPaDvaMessages;
    typedef IPaDvaMessages::iterator    IPaDvaMessagesIt;

} //close namespace TA_Base_Core


#endif // !defined(IPaDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
