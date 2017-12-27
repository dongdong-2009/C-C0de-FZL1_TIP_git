/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/DpValue.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Represents the value of a DataPoint
  *
  */

#ifndef DPVALUE_H
#define DPVALUE_H

#include <string>
#include <map>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

namespace TA_Base_Bus
{

	class DpValue
	{

	//
	// The default precision used when floating point
	// type values are used
	//
	static const long DEFAULT_PRECISION;

	public:

		/**
		 * DpValue::DpValue()
		 *
		 * @param	value 	the value that this object represents. Numerous
		 *					value types are supported.
		 *
		 */
		DpValue(TA_Base_Bus::EDataPointDataType type,
				const DpvEnumLabelsMap& enumLabelsMap,
				const DpvBooleanLabels& booleanLabels);

		//
		// Copy constructor
		//
		DpValue( const DpValue & copyMe );

		//
		// Assignment Operator
		//
		DpValue& operator=( const DpValue & right );

		//
		// Constructors from c++ types
		//
		DpValue( const TA_Base_Bus::DataPointValue & dataPointValue,
				 TA_Base_Bus::EDataPointDataType type,
				 const DpvEnumLabelsMap& enumLabelsMap,
				 const DpvBooleanLabels& booleanLabels,
				 long precision =-1);

		//
		// Constructors from strings
		//
		DpValue( std::string value,
				 TA_Base_Bus::EDataPointDataType type,				  	
				 const DpvEnumLabelsMap& enumLabelsMap,
				 const DpvBooleanLabels& booleanLabels,
				 long precision =-1);

		

		virtual ~DpValue(); 

		/**
		 * DpValue::getDataPointValue()
		 *
		 * @return	the value as a DataPointCorbaDef::DataPointValue
		 *
		 */
		virtual TA_Base_Bus::DataPointValue getDataPointValue() const;

		/**
		 * DpValue::getValueAsString()
		 *
		 * @return	the value as a std::string
		 *
		 */
		virtual const std::string& getValueAsString() const;
		
		/**
		 * DpValue::setType()
		 *
		 * Set the data type of the value represented by this object
		 *
		 */
		virtual void setType(TA_Base_Bus::EDataPointDataType type);

		/**
		 * DpValue::getDataPointDataType()
		 *
		 * Return the data type of the value represented by this object
		 *
		 */
		virtual TA_Base_Bus::EDataPointDataType getType() const;

		/**
		 * DpValue::getText()
		 *
		 * @return	the value as a std::string
		 *
		 */
		virtual const std::string& getText() const;

		/**
		 * DataPointValue::getByteSequence()
		 *
		 * Get the string as a sequence of bytes, i.e. unsigned char numbers
		 *
		 */
		virtual TA_Base_Bus::ByteSequence getByteSequence() const;

		/**
		 * DpValue::setText()
		 *
		 * @param 	newValue	replaces the current value with a new string
		 *
		 */
		virtual void setText( const std::string& newValue ); 
		virtual void setText( const char * newValue, unsigned int textLength = TA_Base_Bus::DPT_MAX_TEXT_LENGTH );

		/**
		 * DpValue::setByteSequence()
		 *
		 * @param 	newValue	replaces the current value with a new string
		 *
		 */
		virtual void setByteSequence( const TA_Base_Bus::ByteSequence& newValue );
		
		/**
		 * DpValue::getFloat()
		 *
		 * @return	the value as a floating point 
		 *
		 */
		virtual double getFloat() const;

		/**
		 * DpValue::getPrecision()
		 *
		 * @return	the precision of the value 
		 *
		 */
		virtual long getPrecision() const;
		
		/**
		 * DpValue::setFloat()
		 *
		 * @param	value		the new value as a floating point
		 *
		 * @param	precision	the precision to store the value in. Defaults
		 *						to -1 (use the DEFAULT_PRECISION constant)
		 */
		virtual void setFloat( double value, long precision =-1 );

		/**
		 * DpValue::getEnum()
		 *
		 * @return	the value as an unsigned long representing an enumeration 
		 *
		 */
		virtual unsigned long getEnum() const;

		/**
		 * DpValue::setEnum()
		 *
		 * @param	value		the new value as an unsigned long representing an enumeration 
		 *
		 */
		//virtual void setEnum( unsigned long value );
		virtual void setEnum( unsigned long value );
		virtual void setEnum( const std::string& label );

		/**
		 * DpValue::getBoolean()
		 *
		 * @return	the value as a boolean
		 *
		 */
		virtual bool getBoolean() const;
		
		/**
		 * DpValue::setBoolean()
		 *
		 * @param	value	the new boolean value to represent
		 *
		 */
		//virtual void setBoolean( bool value );
		virtual void setBoolean( bool value );
		virtual void setBoolean( const std::string& label );

		/**
		* DpValue::getControlOperation()
		*
		* @return	the value as a EControlOperation
		*
		*/
		virtual EControlOperation getControlOperation() const;

		/**
		* DpValue::setControlOperation()
		*
		* @param	value	the new EControlOperation value to represent
		*
		*/
		virtual void setControlOperation( EControlOperation controlOperation );

		//
		// Functions to assist Persistence
		//

		/**
		 * DpValue::getPersistString()
		 *
		 * @return	the value as a std::string
		 *
		 */
		virtual const std::string getPersistString() const;

		/**
		 * DpValue::setFromPersist()
		 *
		 * @param	value		the new value
		 *
		 * @param	precision	the precision to store the value in. Defaults
		 *						to -1 (use the DEFAULT_PRECISION constant)
		 */
		virtual void setFromPersist( const std::string& value, long precision =-1 );
		
		//
		// Comparison operators for DpValues
		//
		virtual int operator==( const DpValue & right ) const;
		virtual int operator!=( const DpValue & right ) const;

	protected:
  		/**
		 * DpValue::setDataPointValue()
		 *
		 * @param 	newValue	replaces the current value with a new value
		 *
		 */
		virtual void setDataPointValue(const TA_Base_Bus::DataPointValue& dataPointValue,
									   long precision =-1);
		


		virtual void setEnum( unsigned long value, const std::string& label );
		virtual void setBoolean( bool value, const std::string& label );

	protected:
		
		TA_Base_Bus::EDataPointDataType		m_type;
		double					m_floatValue;
		unsigned long			m_enumValue;
		bool					m_booleanValue;
		std::string				m_stringValue;
		long					m_precision;
		const DpvEnumLabelsMap* m_enumLabelsMap;
		const DpvBooleanLabels*	m_booleanLabels;
		bool					m_localLabels;
		TA_Base_Bus::EControlOperation		m_cotrolOperation;	//Using for IEC104 protocol
	};
}

#endif
