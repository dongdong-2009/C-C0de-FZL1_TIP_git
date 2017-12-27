#include "bus/mathematical_evaluation/src/IOperator.h"
#include <cmath>
#include <cstdlib>

namespace TA_Base_Bus
{
	const double EPSILON = 1e-15;

	void AddOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if (operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opLeft.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opRight.getDataType();
			if (op1Type == ME_INT_TYPE && op2Type == ME_INT_TYPE)
			{
				int addCount = opLeft.getValueAsInt() + opRight.getValueAsInt();
				operands.push(MEOperand(addCount));
			}
			else if ( (op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE) &&
				      (op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE))
			{
				double addCount = opLeft.getValueAsReal() + opRight.getValueAsReal();
				operands.push(MEOperand(addCount));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE)
			{	
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				int addCount = leftValue + opRight.getValueAsInt();
				operands.push(MEOperand(addCount));
			}
			else if(op1Type == ME_BOOL_TYPE && op2Type == ME_REAL_TYPE)
			{	
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				double addCount = leftValue + opRight.getValueAsReal();
				operands.push(MEOperand(addCount));
			}
			else if (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE)
			{
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				int addCount = opLeft.getValueAsInt() + rightValue;
				operands.push(MEOperand(addCount));
			}
			else if (op1Type == ME_REAL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				double addCount = opLeft.getValueAsReal() + rightValue;
				operands.push(MEOperand(addCount));
			}
			else
			{
				std::string msg("OP + : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP + : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void AddOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if (operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opLeft.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opRight.getDataType();
			if ((op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE || op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE || op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP + : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP + : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void SubOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if (operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if (op1Type == ME_INT_TYPE && op2Type == ME_INT_TYPE)
			{
				int subCount = opLeft.getValueAsInt() - opRight.getValueAsInt();
				operands.push(MEOperand(subCount));
			}
			else if ( (op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE) &&
				      (op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE) )
			{
				double subCount = opLeft.getValueAsReal() - opRight.getValueAsReal();
				operands.push(MEOperand(subCount));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE)
			{	
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				int subCount = opLeft.getValueAsInt() - rightValue;
				operands.push(MEOperand(subCount));
			}
			else if(op1Type == ME_BOOL_TYPE && op2Type == ME_REAL_TYPE)
			{	
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				double subCount = opLeft.getValueAsReal() - rightValue;
				operands.push(MEOperand(subCount));
			}
			else if (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE)
			{
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				int subCount = leftValue - opRight.getValueAsInt();
				operands.push(MEOperand(subCount));
			}
			else if (op1Type == ME_REAL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				double subCount = (double)(leftValue) - opRight.getValueAsReal();
				operands.push(MEOperand(subCount));
			}
			else
			{
				std::string msg("OP - : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP - : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void SubOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if (operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if ((op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE || op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE || op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP - : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP - : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void MultiplyOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if (op1Type == ME_INT_TYPE && op2Type == ME_INT_TYPE)
			{
				int ret = opLeft.getValueAsInt() * opRight.getValueAsInt();
				operands.push(MEOperand(ret));
			}
			else if ( (op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE) &&
					  (op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE)  )
			{
				double ret = opLeft.getValueAsReal() * opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else if (op2Type == ME_BOOL_TYPE && op1Type == ME_INT_TYPE)
			{	
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				int ret = leftValue * opRight.getValueAsInt();
				operands.push(MEOperand(ret));
			}
			else if(op2Type == ME_BOOL_TYPE && op1Type == ME_REAL_TYPE)
			{	
				int leftValue = (opLeft.getValueAsBool() ? 1 : 0);
				double ret = leftValue * opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else if (op2Type == ME_INT_TYPE && op1Type == ME_BOOL_TYPE)
			{
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				int ret = opLeft.getValueAsInt() * rightValue;
				operands.push(MEOperand(ret));
			}
			else if (op2Type == ME_REAL_TYPE && op1Type == ME_BOOL_TYPE)
			{
				int rightValue = (opRight.getValueAsBool() ? 1 : 0);
				double ret = opLeft.getValueAsReal() * rightValue;
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP * :Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP * :no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void MultiplyOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if ((op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE || op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE || op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP * :Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP * :no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void DivisionOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if (op1Type == ME_INT_TYPE && op2Type == ME_INT_TYPE)
			{
				int divisor = opRight.getValueAsInt();
				if (divisor != 0)
				{
					int ret = opLeft.getValueAsInt() / divisor;
					operands.push(MEOperand(ret));
				}
				else
				{
					std::string msg("OP / :Divisor is zero!");
					throw TA_Base_Core::MathematicalEvaluationException(msg);
				}
			}
			else if ( (op1Type == ME_INT_TYPE && op2Type == ME_REAL_TYPE) ||
				(op1Type == ME_REAL_TYPE && op2Type == ME_INT_TYPE) || 
				(op1Type == ME_REAL_TYPE && op2Type == ME_REAL_TYPE) )
			{
				double divisor = opRight.getValueAsReal();
				if (fabs(divisor) > EPSILON)
				{
					double ret = opLeft.getValueAsReal() / divisor;
					operands.push(MEOperand(ret));
				}
				else
				{
					std::string msg("OP / :Divisor is zero!");
					throw TA_Base_Core::MathematicalEvaluationException(msg);
				}
			}
			else
			{
				std::string msg("OP / : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP / :no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void DivisionOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();
			if ((op1Type == ME_INT_TYPE || op1Type == ME_REAL_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_INT_TYPE || op2Type == ME_REAL_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP * :Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP * :no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void EqualOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = (opLeft.getValueAsReal() == opRight.getValueAsReal());
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool ret = (opLeft.getValueAsBool() == opRight.getValueAsBool() );
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_STRING_TYPE && op2Type == ME_STRING_TYPE)
			{
				bool ret = (opLeft.getValueAsString() == opRight.getValueAsString() );
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool left = opLeft.getValueAsBool();
				bool right = ((opRight.getValueAsInt()) == 0 ? false : true);
				bool ret = (left == right);
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE)
			{
				bool left = ((opLeft.getValueAsInt()) == 0 ? false : true);
				bool right =  opRight.getValueAsBool();
				bool ret = (left == right);
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP = : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP = : not enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void EqualOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) && 
					  (op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_STRING_TYPE || op1Type == ME_ANY_TYPE) && 
				      (op2Type == ME_STRING_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE) || 
				      (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP = : Not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP = : not enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void NotEqualOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = (opLeft.getValueAsReal() != opRight.getValueAsReal() );
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool ret = (opLeft.getValueAsBool() != opRight.getValueAsBool() );
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_STRING_TYPE && op2Type == ME_STRING_TYPE)
			{
				bool ret = (opLeft.getValueAsString() != opRight.getValueAsString() );
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE)				
			{
				bool left = opLeft.getValueAsBool();
				bool right = ((opRight.getValueAsInt()) == 0 ? false : true);
				bool ret = (left != right);
				operands.push(MEOperand(ret));
			}
			else if (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE)
			{
				bool left = ((opLeft.getValueAsInt()) == 0 ? false : true);
				bool right =  opRight.getValueAsBool();
				bool ret = (left != right);
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP != : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP != : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void NotEqualOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) && 
				(op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_STRING_TYPE || op1Type == ME_ANY_TYPE) && 
				(op2Type == ME_STRING_TYPE || op2Type == ME_ANY_TYPE))
			{
				operands.push(MEOperand(true));
			}
			else if ( (op1Type == ME_INT_TYPE && op2Type == ME_BOOL_TYPE) || 
				      (op1Type == ME_BOOL_TYPE && op2Type == ME_INT_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP != : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP != : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void GreaterThanOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = opLeft.getValueAsReal() > opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP > : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP > : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void GreaterThanOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				 (op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP > : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP > : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void LessThanOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = opLeft.getValueAsReal() < opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP < :not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP < : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void LessThanOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP > : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP > : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void GreaterThanOrEqualOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = opLeft.getValueAsReal() >= opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP >=:not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP >=:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void GreaterThanOrEqualOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP > : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP > : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void LessThanOrEqualOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE) )
			{
				bool ret = opLeft.getValueAsReal() <= opRight.getValueAsReal();
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP <=:not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP <=:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void LessThanOrEqualOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if(operands.size() >= 2)
		{
			MEOperand opRight = operands.top();
			operands.pop();
			MEOperand opLeft = operands.top();
			operands.pop();
			ME_OPERAND_DATA_TYPE op1Type = opRight.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = opLeft.getDataType();

			if ( (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE) &&
				(op2Type == ME_REAL_TYPE || op2Type == ME_INT_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP > : not supported data type, left:");
				msg += opLeft.getDataTypeAsString();
				msg += ",right:";
				msg += opRight.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP > : no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void PowerOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand opExp = operands.top();
			operands.pop();
			MEOperand opBase = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE opBaseType = opBase.getDataType();
			ME_OPERAND_DATA_TYPE opExpType = opExp.getDataType();

			if ( (opBaseType == ME_REAL_TYPE || opBaseType == ME_INT_TYPE) &&
				(opExpType == ME_REAL_TYPE || opExpType == ME_INT_TYPE) )
			{
				double ret = pow(opBase.getValueAsReal(), opExp.getValueAsReal());
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP POWER:not supported data type, base:");
				msg += opBase.getDataTypeAsString();
				msg += ",exp:";
				msg += opExp.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP POWER:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void PowerOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand opExp = operands.top();
			operands.pop();
			MEOperand opBase = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE opBaseType = opBase.getDataType();
			ME_OPERAND_DATA_TYPE opExpType = opExp.getDataType();

			if ( (opBaseType == ME_REAL_TYPE || opBaseType == ME_INT_TYPE || opBaseType == ME_ANY_TYPE) &&
				(opExpType == ME_REAL_TYPE || opExpType == ME_INT_TYPE || opExpType == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP POWER:not supported data type, base:");
				msg += opBase.getDataTypeAsString();
				msg += ",exp:";
				msg += opExp.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP POWER:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void FabsOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_REAL_TYPE || op1Type  == ME_INT_TYPE)
			{
				double ret = fabs(op1.getValueAsReal());
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP FABS:not supported data type:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP FABS:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void FabsOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_REAL_TYPE || op1Type  == ME_INT_TYPE || op1Type == ME_ANY_TYPE)
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP FABS:not supported data type:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP FABS:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void SqrtOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE)
			{
				double ret = sqrt(op1.getValueAsReal());
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP SQRT:not supported data type:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP SQRT:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void SqrtOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_REAL_TYPE || op1Type == ME_INT_TYPE || op1Type == ME_ANY_TYPE)
			{
				operands.push(MEOperand(1.0));
			}
			else
			{
				std::string msg("OP SQRT:not supported data type:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP SQRT:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void AndOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if (op1Type == ME_BOOL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool op1Val = op1.getValueAsBool();
				bool op2Val = op2.getValueAsBool();
				bool ret =  (op1Val && op2Val);
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP AND:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP AND:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void AndOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if ((op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) && 
				(op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP AND:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP AND:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void OrOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if (op1Type == ME_BOOL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool op1Val = op1.getValueAsBool();
				bool op2Val = op2.getValueAsBool();
				bool ret =  (op1Val || op2Val);
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP OR:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP OR:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void OrOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if ((op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) && 
				(op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP OR:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP OR:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void XorOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if (op1Type == ME_BOOL_TYPE && op2Type == ME_BOOL_TYPE)
			{
				bool op1Val = op1.getValueAsBool();
				bool op2Val = op2.getValueAsBool();
				bool ret =  (op1Val && !op2Val) || (op2Val && !op1Val);
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP XOR:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP XOR:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	void XorOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 2 )
		{
			MEOperand op1 = operands.top();
			operands.pop();
			MEOperand op2 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			ME_OPERAND_DATA_TYPE op2Type = op2.getDataType();
			if ((op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE) && 
				(op2Type == ME_BOOL_TYPE || op2Type == ME_ANY_TYPE) )
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP XOR:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				msg += ",op2:";
				msg += op2.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP XOR:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
	void NotOperator::evaluate(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_BOOL_TYPE)
			{
				bool ret = !(op1.getValueAsBool());
				operands.push(MEOperand(ret));
			}
			else
			{
				std::string msg("OP NOT:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP NOT:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
	
		void NotOperator::verifySemantic(std::stack<MEOperand>& operands)
	{
		if ( operands.size() >= 1 )
		{
			MEOperand op1 = operands.top();
			operands.pop();

			ME_OPERAND_DATA_TYPE op1Type = op1.getDataType();
			if (op1Type == ME_BOOL_TYPE || op1Type == ME_ANY_TYPE)
			{
				operands.push(MEOperand(true));
			}
			else
			{
				std::string msg("OP NOT:not supported data type, op1:");
				msg += op1.getDataTypeAsString();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			std::string msg("OP NOT:no enough operands");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
}