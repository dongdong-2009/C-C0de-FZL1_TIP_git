#ifndef DATUM_H
#define DATUM_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/Datum.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Stores a single data value.
 *
 */

#include <string>
#include <cstring>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Bus
{
    class Datum
    {
        /**
         * Static constants for use in toString() for boolean Datum's
         */
        static const std::string TRUE_STRING;
        static const std::string FALSE_STRING;
    
    public:

        enum ERecordableItemType
        {
            FLOAT_ITEM,
            INTEGER_ITEM,
            STRING_ITEM,
            BOOLEAN_ITEM
        };

        union Value
        {
            double     m_float;
            long       m_integer;
            char*      m_string;
            bool       m_bool;
        };


        /**
         * Constructors
         *
         * @param   name    unique name for the data - corresponds
         *                  to the LiveProperty Identifier value
         * @param   value   the value 
         *
         */
        Datum( const std::string& name, double value );
        Datum( const std::string& name, int value );
        Datum( const std::string& name, unsigned long value );
        Datum( const std::string& name, const std::string& value );
        Datum( const std::string& name, const char* value );
        Datum( const std::string& name, bool value );
        Datum( const std::string& name, time_t value );

        /**
         * Destructor
         *
         */
        ~Datum();


        /**
         * getName
         *
         * @return  the Datum's type name
         *
         */
        const std::string& getName() const;


        /**
         * getType
         *
         * @return the type of data stored by the Datum
         *
         */
        ERecordableItemType getType() const;


        /**
         * getValue
         *
         * @return  the Datum's value
         *
         */
        const Value& getValue() const;


        /**
         * toString
         *
         * @return  the Datum's value as a string
         *
         */
        std::string toString() const;


        /**
         * Copy Constructor
         */
        Datum( const Datum& copyMe );


        /**
         * Assignment operator
         */
        Datum& operator=( const Datum& copyMe );


    private:

        std::string             m_name;
        ERecordableItemType     m_type;
        Value                   m_value;

    };
}

#endif // DATUM_H
