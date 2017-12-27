/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SsaParamHelper.cpp $
  * @author:  Chris DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  */
#include "core/data_access_interface/entity_access/src/SsaParamHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{


		SsaParamHelper::SsaParamHelper( const std::string& name, int defaultValue,
			int minValue, int maxValue) : m_name(name), m_type(SsaParamHelper::INT)
		{
			//check preconditions
			TA_ASSERT( (minValue <= defaultValue)  && (defaultValue <= maxValue), "defaultValue not valid" );

			m_intParam.value = defaultValue;
			m_intParam.defaultValue = defaultValue;
			m_intParam.min = minValue;
			m_intParam.max = maxValue;

			m_isLoaded = false;
		}


		SsaParamHelper::SsaParamHelper( const std::string& name, const std::string& defaultValue,
			bool stringCanBeEmpty /*= false*/) : m_name(name), m_type(SsaParamHelper::STRING)
		{
			//check preconditions
			if( !stringCanBeEmpty )
			{
				TA_ASSERT( defaultValue != "" , "defaultValue not valid" );
			}

			m_stringParam.value = defaultValue;
			m_stringParam.defaultValue = defaultValue;
			m_stringParam.canBeEmpty = stringCanBeEmpty;

			m_isLoaded = false;
		}


		unsigned int SsaParamHelper::getIntValue()
		{
			//check preconditions
			TA_ASSERT( m_type == SsaParamHelper::INT , "m_type must be SsaParamHelper::INT to call this method" );

			return m_intParam.value;

		}



		void SsaParamHelper::setIntValue( int newValue )
		{
			//check preconditions
			TA_ASSERT( m_type == SsaParamHelper::INT , "m_type must be SsaParamHelper::INT to call this method" );

			//if the new value is valid set it and return
			if( (m_intParam.min <= newValue)  && (newValue <= m_intParam.max) )
			{
				m_intParam.value = newValue;
				return;
			}
			else //the value is not valid so set the default value, log an error and return
			{
				m_intParam.value = m_intParam.defaultValue;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"setIntValue() called with invalid value for param %s", m_name.c_str() );
				return;
			}
		}



		std::string SsaParamHelper::getStringValue()
		{
			//check preconditions
			TA_ASSERT( m_type == SsaParamHelper::STRING , "m_type must be SsaParamHelper::STRING to call this method" );

			return m_stringParam.value;
		}



		void SsaParamHelper::setStringValue(const std::string& newValue)
		{
			//check preconditions
			TA_ASSERT( m_type == SsaParamHelper::STRING , "m_type must be SsaParamHelper::STRING to call this method" );

			//if the value is not valid set the default value, log an error and return
			if( !m_stringParam.canBeEmpty )
			{
				if( newValue == "" )
				{
					m_stringParam.value = m_stringParam.defaultValue;

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
						"setStringValue() called with invalid value for param %s", m_name.c_str() );
					return;
				}
			}
			else //the value is valid so set it  and return
			{
				m_stringParam.value = newValue;
			}
		}

		void SsaParamHelper::loadDefault()
		{
			switch(m_type)
			{
				case SsaParamHelper::INT :
				{
					m_intParam.value = m_intParam.defaultValue;
					break;
				}
				case SsaParamHelper::STRING :
				{
					m_stringParam.value = m_stringParam.defaultValue;
					break;
				}
				default :
				{
					TA_ASSERT( false , "param has non-supported data type");
				}
			}
		}

} //end namespace TA_Base_Core
