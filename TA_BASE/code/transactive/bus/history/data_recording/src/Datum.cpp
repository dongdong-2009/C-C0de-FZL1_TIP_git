/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/Datum.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2016/01/18 15:51:59 $
 * Last modified by:  $Author: Ouyang $
 * 
 * Stores a single data value.
 *
 */

#include "bus/history/data_recording/src/Datum.h"
#include "core/utilities/src/TAAssert.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Bus
{
    const std::string Datum::TRUE_STRING = "TRUE";
    const std::string Datum::FALSE_STRING = "FALSE";

    Datum::Datum( const std::string& name, double value )
    : m_name( name ), m_type( FLOAT_ITEM )
    {
        m_value.m_float = value;
    }


    Datum::Datum( const std::string& name, int value )
    : m_name( name ), m_type( INTEGER_ITEM )
    {
        m_value.m_integer = value;
    }


    Datum::Datum( const std::string& name, unsigned long value )
    : m_name( name ), m_type( INTEGER_ITEM )
    {
        m_value.m_integer = value;
    }


    Datum::Datum( const std::string& name, const std::string& value )
    : m_name( name ), m_type( STRING_ITEM )
    {
		size_t uSize = value.length() + 1;
        m_value.m_string = new char[ uSize ];
		memset(m_value.m_string, 0, uSize);
        strcpy( m_value.m_string, value.c_str() );
    }


    Datum::Datum( const std::string& name, const char* value )
    : m_name( name ), m_type( STRING_ITEM )
    {
		size_t uSize = strlen(value) + 1;
        m_value.m_string = new char[ uSize ];
		memset(m_value.m_string, 0, uSize);
        strcpy( m_value.m_string, value );
    }


    Datum::Datum( const std::string& name, bool value )
    : m_name( name ), m_type( BOOLEAN_ITEM )
    {
        m_value.m_bool = value;
    }

    Datum::Datum( const std::string& name, time_t value )
    : m_name( name ), m_type( INTEGER_ITEM )
    {
        m_value.m_integer = value;
    }

    Datum::~Datum()
    {
        if ( m_type == STRING_ITEM )
        {
            delete [] m_value.m_string;
            m_value.m_string = NULL;
        }
    }


    const std::string& Datum::getName() const
    {
        return m_name;
    }


    Datum::ERecordableItemType Datum::getType() const
    {
        return m_type;
    }


    const Datum::Value& Datum::getValue() const
    {
        return m_value;
    }


    std::string Datum::toString() const
    {
        switch( m_type )
        {
            case FLOAT_ITEM :

                return boost::lexical_cast<std::string>( m_value.m_float );
                break;

            case INTEGER_ITEM :

                return boost::lexical_cast<std::string>( m_value.m_integer );
                break;

            case STRING_ITEM :

                return std::string( m_value.m_string );
                break;

            case BOOLEAN_ITEM :

                return m_value.m_bool ? TRUE_STRING : FALSE_STRING;
                break;

            default : 

                TA_ASSERT( false, "Unsupported value type" );
                return "ASSERT: Unsupported value type";
        }
    }


    Datum::Datum( const Datum& copyMe )
    : m_name( copyMe.m_name ), m_type( copyMe.m_type )
    {
        if ( &copyMe != this )
        {
			size_t uSize = 0;
            switch( copyMe.m_type )
            {
                case FLOAT_ITEM :

                    m_value.m_float = copyMe.m_value.m_float;
                    break;

                case INTEGER_ITEM :

                    m_value.m_integer = copyMe.m_value.m_integer;
                    break;

                case STRING_ITEM :
					uSize = strlen( copyMe.m_value.m_string ) + 1;
                    m_value.m_string = new char[ uSize ];
					memset(m_value.m_string, 0, uSize);
                    strcpy( m_value.m_string, copyMe.m_value.m_string );
                    break;

                case BOOLEAN_ITEM :

                    m_value.m_bool = copyMe.m_value.m_bool;
                    break; 

                default : 

                    TA_ASSERT( false, "Unsupported value type" );
            }
        }
    }


    Datum& Datum::operator=( const Datum& copyMe )
    {
        if ( &copyMe != this )
        {
            if ( m_type == STRING_ITEM )
            {
                delete [] m_value.m_string;
                m_value.m_string = NULL;
            }

            m_name = copyMe.m_name;
            m_type = copyMe.m_type;
			size_t uSize = 0;
            switch( copyMe.m_type )
            {				
                case FLOAT_ITEM :

                    m_value.m_float = copyMe.m_value.m_float;
                    break;

                case INTEGER_ITEM :

                    m_value.m_integer = copyMe.m_value.m_integer;
                    break;

                case STRING_ITEM :
					uSize = strlen( copyMe.m_value.m_string ) + 1;
                    m_value.m_string = new char[ uSize ];
					memset(m_value.m_string, 0, uSize);
                    strcpy( m_value.m_string, copyMe.m_value.m_string );
                    break;

                case BOOLEAN_ITEM :

                    m_value.m_bool = copyMe.m_value.m_bool;
                    break; 

                default : 

                    TA_ASSERT( false, "Unsupported value type" );
            }
        }
        return *this;
    }
        
}
