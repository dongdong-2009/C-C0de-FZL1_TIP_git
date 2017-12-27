#ifndef ME_OPERAND_H
#define ME_OPERAND_H

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include <string>

namespace TA_Base_Bus
{
	enum ME_OPERAND_DATA_TYPE
	{
		ME_NO_TYPE = 0,
		ME_INT_TYPE,
		ME_REAL_TYPE,
		ME_STRING_TYPE,
		ME_BOOL_TYPE,
		ME_ANY_TYPE
	};

	class MEOperand
	{
	public:
		MEOperand()	
			: m_type(ME_NO_TYPE) {}

		MEOperand(bool val)	
			: m_type(ME_BOOL_TYPE), m_boolValue(val) {}

		MEOperand(std::string& val)	
			: m_type(ME_STRING_TYPE), m_strValue(val) {}

		MEOperand(double val) 
			: m_type(ME_REAL_TYPE), m_realValue(val) {}

		MEOperand(int val) 
			: m_type(ME_INT_TYPE), m_intValue(val) {}

		~MEOperand() {}

		void setBoolValue(bool val)
		{
			m_type = ME_BOOL_TYPE;
			m_boolValue = val;
		}

		void setStringValue(const std::string& val)
		{
			m_type = ME_STRING_TYPE;
			m_strValue = val;
		}

		void setIntValue(int val)
		{
			m_type = ME_INT_TYPE;
			m_intValue = val;
		}

		void setRealValue(double val)
		{
			m_type = ME_REAL_TYPE;
			m_realValue = val;
		}

		bool getValueAsBool()
		{
			if (m_type == ME_BOOL_TYPE)
			{
				return m_boolValue;
			}
			else
			{
				std::string msg("Get wrong type value:ME_BOOL_TYPE, should be:");
				msg += getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}

		double getValueAsReal()
		{
			if (m_type == ME_REAL_TYPE)
			{
				return m_realValue;
			}
			else if (m_type == ME_INT_TYPE)
			{
				return m_intValue;
			}
			else
			{
				std::string msg("Get wrong type value:ME_REAL_TYPE or ME_INT_TYPE, should be:");
				msg += getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}

		int getValueAsInt()
		{
			if (m_type == ME_INT_TYPE)
			{
				return m_intValue;
			}
			else
			{
				std::string msg("Get wrong type value:ME_INT_TYPE, should be:");
				msg += getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}

		std::string getValueAsString()
		{
			if (m_type == ME_STRING_TYPE)
			{
				return m_strValue;
			}
			else
			{
				std::string msg("Get wrong type value:ME_STRING_TYPE, should be:");
				msg += getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		
		void setDataType(ME_OPERAND_DATA_TYPE type)
		{
			m_type = type;
		}

		ME_OPERAND_DATA_TYPE getDataType()
		{
			return m_type;
		}

		std::string getDataTypeAsString()
		{
			std::string strType("");

			switch(m_type)
			{
			case ME_BOOL_TYPE:
				strType = "ME_BOOL_TYPE";
				break;
			case ME_INT_TYPE:
				strType = "ME_INT_TYPE";
				break;
			case ME_REAL_TYPE:
				strType = "ME_REAL_TYPE";
				break;
			case ME_STRING_TYPE:
				strType = "ME_STRING_TYPE";
				break;
			case ME_ANY_TYPE:
				strType = "ME_ANY_TYPE";
				break;
			default:
				strType = "ME_NO_TYPE";
			}

			return strType;
		}
	private:
		ME_OPERAND_DATA_TYPE m_type;
		bool				 m_boolValue;
		double			     m_realValue;
		int					 m_intValue;
		std::string			 m_strValue;
	};
}

#endif
