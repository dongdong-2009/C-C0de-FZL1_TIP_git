/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/DpValue.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  * Represents the value of a DataPoint
  *
  */

#include <cstring>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "bus/scada/common_library/src/DpValue.h"

using namespace TA_Base_Bus;

namespace TA_Base_Bus
{

	const long DpValue::DEFAULT_PRECISION = 8;

	DpValue::DpValue(TA_Base_Bus::EDataPointDataType type,
					 const DpvEnumLabelsMap& enumLabelsMap,
					 const DpvBooleanLabels& booleanLabels)
	:
	m_type (type),
	m_floatValue (0),
	m_enumValue (0),
	m_booleanValue (false),
	m_stringValue (QUALITY_BAD_VALUE_UNKNOWN_STRING),
	m_precision (DEFAULT_PRECISION),
	m_enumLabelsMap (&enumLabelsMap),
	m_booleanLabels (&booleanLabels),
	m_localLabels (false),
	m_cotrolOperation(CONTROL_NO_OPERATION)
	{
	}

	DpValue::DpValue( const DpValue & copyMe )
	:
	m_type ( copyMe.m_type ),
	m_floatValue ( copyMe.m_floatValue ),
	m_enumValue ( copyMe.m_enumValue ),
	m_booleanValue ( copyMe.m_booleanValue ),
	m_stringValue ( copyMe.m_stringValue ),
	m_precision ( copyMe.m_precision ),
	m_cotrolOperation(copyMe.m_cotrolOperation)
	{
		TA_ASSERT(copyMe.m_booleanLabels != NULL || copyMe.m_localLabels == false, "No boolean labels available");
		TA_ASSERT(copyMe.m_enumLabelsMap != NULL || copyMe.m_localLabels == false, "Enum Labels Map not available");

		if (copyMe.m_localLabels)
		{
			m_enumLabelsMap = new DpvEnumLabelsMap(*(copyMe.m_enumLabelsMap));
			m_booleanLabels = new DpvBooleanLabels(*(copyMe.m_booleanLabels));
			m_localLabels = true;
		}
		else
		{
			m_enumLabelsMap = copyMe.m_enumLabelsMap;
			m_booleanLabels = copyMe.m_booleanLabels;
			m_localLabels = false;
		}
	}

	DpValue& DpValue::operator=( const DpValue & right )
	{
		TA_ASSERT(right.m_booleanLabels != NULL || right.m_localLabels == false, "No boolean labels available");
		TA_ASSERT(right.m_enumLabelsMap != NULL || right.m_localLabels == false, "Enum Labels Map not available");

        if (this == &right)
        {
            // Don't do anything if it's trying to copy itself
            return *this;
        }

        if (m_localLabels)
		{
			delete m_enumLabelsMap;
			m_enumLabelsMap = NULL;
			delete m_booleanLabels;
			m_booleanLabels = NULL;
		}
		
		if (right.m_localLabels)
		{
			m_enumLabelsMap = new DpvEnumLabelsMap(*(right.m_enumLabelsMap));
			m_booleanLabels = new DpvBooleanLabels(*(right.m_booleanLabels));
			m_localLabels = true;
		}
		else
		{
			m_enumLabelsMap = right.m_enumLabelsMap;
			m_booleanLabels = right.m_booleanLabels;
			m_localLabels = false;
		}

		setType(right.getType());
		setControlOperation(right.getControlOperation()); //extend for IEC 104 Protocol control

		switch(right.getType())
		{
		case DPT_BOOL_TYPE:
			setBoolean(right.getBoolean(), right.getValueAsString());
			break;

		case DPT_ENUM_TYPE:
			setEnum(right.getEnum(), right.getValueAsString());
			break;

		case DPT_TEXT_TYPE:
			setText(right.getText());
			break;
		
		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			setFloat(right.getFloat(), right.getPrecision());
			break;

		case DPT_NULL_TYPE:
			setType(right.m_type);
			m_floatValue = right.m_floatValue;
			m_enumValue = right.m_enumValue;
			m_booleanValue  = right.m_booleanValue;
			m_stringValue = right.m_stringValue;
			m_precision = right.m_precision;
			break;

		default:
			std::ostringstream description;
            description << "Invalid DpValue type " << right.getType();

            TA_THROW(TA_Base_Core::ScadaCommonException( description.str() ));	
			break;
		}

		return *this;
	}

	DpValue::DpValue( const TA_Base_Bus::DataPointValue & dataPointValue,
					  TA_Base_Bus::EDataPointDataType type,
					  const DpvEnumLabelsMap& enumLabelsMap,
					  const DpvBooleanLabels& booleanLabels,
					  long precision )
	:
	m_type ( type ),
	m_enumLabelsMap ( &enumLabelsMap ),
	m_booleanLabels ( &booleanLabels ),
	m_localLabels (false)
    {
		setDataPointValue(dataPointValue, precision);
    }	

	DpValue::DpValue( std::string value,
					  TA_Base_Bus::EDataPointDataType type,				  	
					  const DpvEnumLabelsMap& enumLabelsMap,
					  const DpvBooleanLabels& booleanLabels,
					  long precision )
	:
	m_type ( type ),
	m_enumLabelsMap ( &enumLabelsMap ),
	m_booleanLabels ( &booleanLabels ),
	m_localLabels (false),
	m_cotrolOperation(CONTROL_NO_OPERATION)
    {
		switch(getType())
		{
		case DPT_BOOL_TYPE:
			setBoolean(value);
			break;
		
		case DPT_ENUM_TYPE:
			setEnum(value);
			break;

		case DPT_TEXT_TYPE:
			setText(value);
			break;
		
		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			setFromPersist(value, precision);
			break;

		case DPT_NULL_TYPE:
		default:
			break;
		}
	}

	DpValue::~DpValue()
	{
		if (m_localLabels)
		{
			delete m_enumLabelsMap;
			m_enumLabelsMap = NULL;
			delete m_booleanLabels;
			m_booleanLabels = NULL;
		}
	}

	TA_Base_Bus::DataPointValue DpValue::getDataPointValue() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getDataPointValue(): type == DPT_NULL_TYPE" );

		TA_Base_Bus::DataPointValue dataPointValue;
        dataPointValue.dpType = getType();

		switch(getType())
		{
		case DPT_BOOL_TYPE:
			dataPointValue.booleanValue = getBoolean();
			break;

		case DPT_ENUM_TYPE:
			dataPointValue.enumValue = getEnum();
			break;

		case DPT_TEXT_TYPE:
			dataPointValue.stringValue = getByteSequence();
			break;

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			dataPointValue.floatValue = getFloat();
			break;

		case DPT_NULL_TYPE:
		default:
			break;	
		}
		//assign the default quality and time
        dataPointValue.quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
        dataPointValue.timeStamp.time = 0;
		dataPointValue.controlOperation = m_cotrolOperation; //extend for IEC 104 Protocol control 
		return dataPointValue;
	}

	void DpValue::setDataPointValue(const TA_Base_Bus::DataPointValue & dataPointValue,
									long precision)
	{
		m_cotrolOperation = dataPointValue.controlOperation; //extend for IEC 104 Protocol control 
		switch( dataPointValue.dpType )
        {
		    case DPT_INT16_TYPE:
		    case DPT_INT32_TYPE:
		    case DPT_INT16S_TYPE:
		    case DPT_INT32S_TYPE:
		    case DPT_UINT16_TYPE:
		    case DPT_UINT32_TYPE:
		    case DPT_IEEE32_TYPE:
                setFloat( dataPointValue.floatValue, precision );
            break;

		case DPT_ENUM_TYPE:
            setEnum( dataPointValue.enumValue );
            break;
        
		case DPT_BOOL_TYPE:
            setBoolean( dataPointValue.booleanValue );
            break;

		case DPT_TEXT_TYPE:
            setByteSequence( dataPointValue.stringValue );
            break;
        
        default:
            std::ostringstream description;
            description << "Invalid DpValue type " << dataPointValue.dpType;

            TA_THROW(TA_Base_Core::ScadaCommonException( description.str() ));
            break;
		}
	}

	const std::string & DpValue::getValueAsString() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getValueAsString(): type == DPT_NULL_TYPE" );

		return m_stringValue;
	}

	const std::string& DpValue::getText() const
	{

		TA_ASSERT( getType() != DPT_NULL_TYPE, "getText(): type == DPT_NULL_TYPE" );

		if (getType() != DPT_TEXT_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "getText(): Not a DPT_TEXT_TYPE" ));
		}

		return m_stringValue;
	}

	TA_Base_Bus::ByteSequence DpValue::getByteSequence() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getByteSequence(): type == DPT_NULL_TYPE" );

		if (getType() != DPT_TEXT_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "getByteSequence(): Not a DPT_TEXT_TYPE" ));
		}

        TA_Base_Bus::ByteSequence bytesSeq;

		// allocated the memory space on the sequence
		bytesSeq.resize( m_stringValue.length() );

		// copy the bytes across
		for ( int i = 0; i < static_cast<int>(m_stringValue.size()); i++ )
		{
			 bytesSeq[ i ] = m_stringValue[i];
		}

		return bytesSeq;
	}

	void DpValue::setText( const std::string & newValue )
	{
		if (m_type != DPT_TEXT_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setText(): Not a DPT_TEXT_TYPE" ));
		}

		// save the new string
		m_stringValue = newValue;

		// Set the other values to the defaults
		m_floatValue = 0;
		m_enumValue = 0;
		m_booleanValue = false;
	}

	void DpValue::setText( const char * newValue, unsigned int textLength )
	{
		if (m_type != DPT_TEXT_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setText(): Not a DPT_TEXT_TYPE" ));
		}

		// convert ByteSequence to std::string
        std::ostringstream valueStr;
        for ( unsigned int i = 0; i < textLength; ++i )
        {
            valueStr << (char) newValue[i];
        }

        setText(valueStr.str());
	}

	void DpValue::setByteSequence( const TA_Base_Bus::ByteSequence& newValue )
	{
		if (m_type != DPT_TEXT_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setByteSequence(): Not a DPT_TEXT_TYPE" ));
		}

		// convert ByteSequence to std::string
        std::ostringstream valueStr;
        for ( unsigned int i = 0; i < newValue.size(); ++i )
        {
            valueStr << (char) newValue[i];
        }

        m_stringValue = valueStr.str();

		// Set the other values to the defaults
		m_floatValue = 0;
		m_enumValue = 0;
		m_booleanValue = false;
	}
	
	unsigned long DpValue::getEnum() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getEnum(): type == DPT_NULL_TYPE" );

		if (getType() != DPT_ENUM_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "getEnum(): Not a DPT_ENUM_TYPE" ));
		}

		return m_enumValue;
	}

	void DpValue::setEnum( unsigned long value )
	{
		TA_ASSERT(m_enumLabelsMap != NULL, "Enum Labels Map not available");

		if (getType() != DPT_ENUM_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setEnum(): Not a DPT_ENUM_TYPE" ));
		}

		DpvEnumLabelsMap::const_iterator it = m_enumLabelsMap->find(value);


		if (it != m_enumLabelsMap->end())
		{
			setEnum(value, (*it).second);
		}
	}

	void DpValue::setEnum( const std::string& label )
	{
		TA_ASSERT(m_enumLabelsMap != NULL, "Enum Labels Map not available");

		if (getType() != DPT_ENUM_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setEnum(): Not a DPT_ENUM_TYPE" ));
		}

		DpvEnumLabelsMap::const_iterator it;

		// Find the label - unfortunately this requires iteration
		for (it = m_enumLabelsMap->begin(); it != m_enumLabelsMap->end() && (*it).second != label; it++);

		if (it != m_enumLabelsMap->end())
		{
			setEnum((*it).first, label);
		}
	}

	void DpValue::setEnum( unsigned long value, const std::string& label )
	{
		if (getType() != DPT_ENUM_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setEnum(): Not a DPT_ENUM_TYPE" ));
		}

		m_stringValue = label;
		m_enumValue = value;

		// Set the other values to the defaults
		m_floatValue = 0;
		m_booleanValue = false;
	}

	double DpValue::getFloat() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getFloat(): type == DPT_NULL_TYPE" );

		if (getType() != DPT_INT16_TYPE &&
			getType() != DPT_INT32_TYPE &&
			getType() != DPT_INT16S_TYPE &&
			getType() != DPT_INT32S_TYPE &&
			getType() != DPT_UINT16_TYPE &&
			getType() != DPT_UINT32_TYPE &&
			getType() != DPT_IEEE32_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "getFloat(): Not a DPT_INT16_TYPE, DPT_INT32_TYPE, DPT_INT16S_TYPE, DPT_INT32S_TYPE, DPT_UINT16_TYPE, DPT_UINT32_TYPE or DPT_IEEE32_TYPE"));
		}

		return m_floatValue;
	}

	void DpValue::setFloat( double value, long precision )
	{
		if (getType() != DPT_INT16_TYPE &&
			getType() != DPT_INT32_TYPE &&
			getType() != DPT_INT16S_TYPE &&
			getType() != DPT_INT32S_TYPE &&
			getType() != DPT_UINT16_TYPE &&
			getType() != DPT_UINT32_TYPE &&
			getType() != DPT_IEEE32_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setFloat(): Not a DPT_INT16_TYPE, DPT_INT32_TYPE, DPT_INT16S_TYPE, DPT_INT32S_TYPE, DPT_UINT16_TYPE, DPT_UINT32_TYPE or DPT_IEEE32_TYPE"));
		}

		// If precision not defined, use the default
		m_precision = ( precision >= 0 ) ? precision : DEFAULT_PRECISION;

		std::stringstream ss;
		ss.precision(m_precision);
		ss << std::fixed; //wenching++ (TD14421)
		ss << value;
		m_stringValue = ss.str();
		ss >> m_floatValue;

		// Set the other values to the defaults
		m_enumValue = 0;
		m_booleanValue = false;
	}

	bool DpValue::getBoolean() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getBoolean(): type == DPT_NULL_TYPE" );

		if (getType() != DPT_BOOL_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "getBoolean(): Not a DPT_BOOL_TYPE" ));
		}

		return m_booleanValue;
	}

	void DpValue::setBoolean( bool value )
	{
		TA_ASSERT(m_booleanLabels != NULL, "No boolean labels available");

		if (getType() != DPT_BOOL_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setBoolean(): Not a DPT_BOOL_TYPE" ));
		}

		if ( value )
		{
			setBoolean(true, m_booleanLabels->first);
		}
		else
		{
			setBoolean(false, m_booleanLabels->second);
		}
	}

	void DpValue::setBoolean( const std::string& label )
	{
		TA_ASSERT(m_booleanLabels != NULL, "No boolean labels available");

		if (getType() != DPT_BOOL_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setBoolean(): Not a DPT_BOOL_TYPE" ));
		}

		if ( label == m_booleanLabels->first )
		{
			setBoolean(true, m_booleanLabels->first);
		}
		else
		{
			setBoolean(false, m_booleanLabels->second);
		}
	}

	void DpValue::setBoolean( bool value, const std::string& label )
	{
		if (getType() != DPT_BOOL_TYPE)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException( "setBoolean(): Not a DPT_BOOL_TYPE" ));
		}

		m_stringValue = label;
		m_booleanValue = value;

		// Set the other values to the defaults
		m_floatValue = 0;
		m_enumValue = 0;
	}

	//
	// Functions to assist Persistence
	//

	const std::string DpValue::getPersistString() const
	{
		TA_ASSERT( getType() != DPT_NULL_TYPE, "getPersistString(): type == DPT_NULL_TYPE" );

		switch(getType())
		{
		case DPT_BOOL_TYPE:
			if (true == m_booleanValue)
			{
				return CFG_TRUE;
			}
			return CFG_FALSE;

		case DPT_TEXT_TYPE:
			return m_stringValue;

		case DPT_ENUM_TYPE:
			{
				std::ostringstream ss;
				ss << m_enumValue;
				return ss.str();
			}

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			return m_stringValue;

		case DPT_NULL_TYPE:
		default:
			return "";
		}
	}

	void DpValue::setFromPersist( const std::string& newValue, long precision )
	{
		switch(getType())
		{
		case DPT_BOOL_TYPE:
			setBoolean(newValue == CFG_TRUE);
			break;

		case DPT_TEXT_TYPE:
			setText(newValue);
			break;

		case DPT_ENUM_TYPE:
			{
				unsigned long enumValue;
				std::istringstream ss (newValue);
				ss >> enumValue;

				setEnum(enumValue);
			}
			break;

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			{
				double floatValue;
				std::stringstream ss (newValue);
				ss >> floatValue;

				setFloat(floatValue, precision);
			}
			break;

		case DPT_NULL_TYPE:
		default:
			std::ostringstream description;
            description << "Invalid DpValue type " << getType();

            TA_THROW(TA_Base_Core::ScadaCommonException( description.str() ));
            break;
		}
	}

	int DpValue::operator==(const DpValue & right) const
	{
		if (m_type != right.m_type)
		{
			return false;
		}

		switch(getType())
		{
		case DPT_BOOL_TYPE:
			return ((m_booleanValue == right.m_booleanValue) &&
				(m_stringValue == right.m_stringValue));

		case DPT_ENUM_TYPE:
			return ((m_enumValue == right.m_enumValue) &&
				(m_stringValue == right.m_stringValue));
		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			return ((m_floatValue == right.m_floatValue) &&
				(m_stringValue == right.m_stringValue));

		case DPT_TEXT_TYPE:
			return m_stringValue == right.m_stringValue;

		case DPT_NULL_TYPE:
		default:
			return false;
		}
	}

	int DpValue::operator!=(const DpValue & right) const
	{
		if (m_type != right.m_type)
		{
			return true;
		}

		switch(getType())
		{
		case DPT_BOOL_TYPE:
			return ((m_booleanValue != right.m_booleanValue) ||
				(m_stringValue != right.m_stringValue));

		case DPT_ENUM_TYPE:
			return ((m_enumValue != right.m_enumValue) ||
				(m_stringValue != right.m_stringValue));

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			return ((m_floatValue != right.m_floatValue) ||
				(m_stringValue != right.m_stringValue));

		case DPT_TEXT_TYPE:
			return m_stringValue != right.m_stringValue;

		case DPT_NULL_TYPE:
		default:
			return true;
		}
	}

	void DpValue::setType(TA_Base_Bus::EDataPointDataType type)
	{
		switch(type)
		{
		case TA_Base_Bus::DPT_INT16_TYPE:
			m_type = DPT_INT16_TYPE;
			break;

		case TA_Base_Bus::DPT_INT32_TYPE:
			m_type = DPT_INT32_TYPE;
			break;
			
		case TA_Base_Bus::DPT_INT16S_TYPE:
			m_type = DPT_INT16S_TYPE;
			break;
			
		case TA_Base_Bus::DPT_INT32S_TYPE:
			m_type = DPT_INT32S_TYPE;
			break;
			
		case TA_Base_Bus::DPT_UINT16_TYPE:
			m_type = DPT_UINT16_TYPE;
			break;
			
		case TA_Base_Bus::DPT_UINT32_TYPE:
			m_type = DPT_UINT32_TYPE;
			break;
			
		case TA_Base_Bus::DPT_IEEE32_TYPE:
			m_type = DPT_IEEE32_TYPE;
			break;
			
		case TA_Base_Bus::DPT_ENUM_TYPE:
			m_type = DPT_ENUM_TYPE;
			break;
			
		case TA_Base_Bus::DPT_BOOL_TYPE:
			m_type = DPT_BOOL_TYPE;
			break;
			
		case TA_Base_Bus::DPT_TEXT_TYPE:
			m_type = DPT_TEXT_TYPE;
			break;

		case TA_Base_Bus::DPT_NULL_TYPE:
		default:
			m_type = DPT_NULL_TYPE;
			break;
		}
	}

	EDataPointDataType DpValue::getType() const
	{
		return m_type;
	}

	long DpValue::getPrecision() const
	{
		return m_precision;
	}

	EControlOperation DpValue::getControlOperation() const
	{
		return m_cotrolOperation;

	}

	void DpValue::setControlOperation( EControlOperation controlOperation )
	{
		m_cotrolOperation = controlOperation;
	}


}


